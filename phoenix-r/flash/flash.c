/** @file
  Flash Dump Protocol implementation: dump the SPI flash to a USB volume.

  Locates the first USB volume that exposes a Simple File System, reads the
  entire SPI flash, and writes the image to \flash_dump.bin on that volume.
**/

/**
  Locate a writable destination for a flash dump (e.g. a USB storage device).

  @param[out]  HandleUsb  On success, a pointer to the Simple File System
              protocol on the first USB volume found.

  @retval EFI_SUCCESS    A writable USB destination was found.
  @retval EFI_NOT_FOUND  No USB volume with a file system is available.
**/
EFI_STATUS
EFIAPI
FindWhereDumpImpl(
    OUT EFI_SIMPLE_FILE_SYSTEM_PROTOCOL** HandleUsb
)
{
    EFI_STATUS                       Status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* Fallback;
    EFI_HANDLE* HandleBuffer;
    UINTN                            HandleCount;
    EFI_DEVICE_PATH_PROTOCOL* DevicePath;
    EFI_DEVICE_PATH_PROTOCOL* Node;
    UINTN                            Index;
    BOOLEAN                          IsUsb;

    if (HandleUsb == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    *HandleUsb = NULL;
    Fallback = NULL;

    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
    );

    if (EFI_ERROR(Status) || (HandleCount == 0))
    {
        return EFI_NOT_FOUND;
    }

    for (Index = 0; Index < HandleCount; Index++)
    {
        Status = gBS->HandleProtocol(
            HandleBuffer[Index],
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID**)&FileSystem
        );
        if (EFI_ERROR(Status))
        {
            continue;
        }

        //
        // Remember the first volume as a fallback in case no USB is found.
        //
        if (Fallback == NULL)
        {
            Fallback = FileSystem;
        }

        Status = gBS->HandleProtocol(
            HandleBuffer[Index],
            &gEfiDevicePathProtocolGuid,
            (VOID**)&DevicePath
        );
        if (EFI_ERROR(Status))
        {
            continue;
        }

        IsUsb = FALSE;
        for (Node = DevicePath; !IsDevicePathEnd(Node); Node = NextDevicePathNode(Node))
        {
            if ((Node->Type == MESSAGING_DEVICE_PATH) && (Node->SubType == MSG_USB_DP))
            {
                IsUsb = TRUE;
                break;
            }
        }

        if (IsUsb)
        {
            gBS->FreePool(HandleBuffer);
            *HandleUsb = FileSystem;
            return EFI_SUCCESS;
        }
    }

    gBS->FreePool(HandleBuffer);

    //
    // No USB volume found; fall back to the first file system volume.
    //
    if (Fallback != NULL)
    {
        *HandleUsb = Fallback;
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

/**
  Read the flash size in bytes from the SFDP Basic Flash Parameter Table.

  The SFDP header at address 0 carries the "SFDP" signature; the first
  parameter header at address 0x08 points at the JEDEC Basic Flash Parameter
  Table, whose second double word holds the density in bits minus one
  (JESD216, MX25U12873F datasheet Table 15/16).

  @param[in]   SpiBase    SPI controller MMIO base address.
  @param[out]  FlashSize  Flash size in bytes.

  @retval EFI_SUCCESS       Size read successfully.
  @retval EFI_DEVICE_ERROR  SFDP read failed or signature mismatch.
**/
EFI_STATUS
EFIAPI
ReadFlashSizeFromSfdpImpl(
    IN  UINTN   SpiBase,
    OUT UINT32* FlashSize
)
{
    EFI_STATUS  Status;
    UINT32      Signature;
    UINT32      TablePointer;
    UINT32      Density;

    if ((SpiBase == 0) || (FlashSize == NULL))
    {
        return EFI_DEVICE_ERROR;
    }

    //
    // Verify the "SFDP" signature at address 0x000000.
    //
    Status = gSpiProtocol.ReadSfdp(SpiBase, 0x000000, &Signature);
    if (EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }

    if (Signature != 0x50444653)
    {
        return EFI_DEVICE_ERROR;
    }

    //
    // Table pointer in bits [23:0] of the parameter header's second dword.
    //
    Status = gSpiProtocol.ReadSfdp(SpiBase, 0x0C, &TablePointer);
    if (EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }

    TablePointer &= 0x00FFFFFF;

    //
    // Density field: bits [30:0] = size in bits minus one; bit 31 = >= 4 Gbit.
    //
    Status = gSpiProtocol.ReadSfdp(SpiBase, TablePointer + 4, &Density);
    if (EFI_ERROR(Status))
    {
        return EFI_DEVICE_ERROR;
    }

    if ((Density & BIT31) != 0)
    {
        return EFI_DEVICE_ERROR;
    }

    *FlashSize = (Density + 1) / 8;

    if (*FlashSize == 0)
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
  Read the entire SPI flash and write its contents to the destination
  found by FLASH_FIND_WHERE_DUMP.

  @retval EFI_SUCCESS       Flash dump completed successfully.
  @retval EFI_NOT_FOUND     No writable destination available.
  @retval EFI_DEVICE_ERROR  SPI read or file write failed.
**/
EFI_STATUS
EFIAPI
DumpImpl(
    VOID
)
{
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    EFI_FILE_PROTOCOL* Root;
    EFI_FILE_PROTOCOL* File;
    EFI_STATUS                       Status;
    UINT32                           SpiBase;
    UINT32                           FlashSize;
    UINTN                            BufferSize;
    UINTN                            Offset;
    UINT8* Buffer;

    Status = FindWhereDumpImpl(&FileSystem);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Status = FileSystem->OpenVolume(FileSystem, &Root);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Status = Root->Open(Root, &File, L"\\flash_dump.bin", EFI_FILE_MODE_READ, 0);
    if (!EFI_ERROR(Status))
    {
        File->Delete(File);
    }

    Status = Root->Open(
        Root,
        &File,
        L"\\flash_dump.bin",
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
        0
    );
    if (EFI_ERROR(Status))
    {
        Root->Close(Root);
        return Status;
    }

    SpiBase = 0;
    Status = gFchProtocol.GetLegacySpiBase(&SpiBase);
    if (EFI_ERROR(Status))
    {
        goto CloseFile;
    }

    if ((SpiBase == 0) || (SpiBase >= 0xFFFFFF00))
    {
        Status = EFI_DEVICE_ERROR;
        goto CloseFile;
    }

    Status = gSpiProtocol.ClaimOwnership((UINTN)SpiBase);
    if (EFI_ERROR(Status))
    {
        goto CloseFile;
    }

    Status = gSpiProtocol.WaitFlashReady((UINTN)SpiBase);
    if (EFI_ERROR(Status))
    {
        goto ReleaseOwnership;
    }

    FlashSize = 0;
    Status = ReadFlashSizeFromSfdpImpl((UINTN)SpiBase, &FlashSize);
    if (EFI_ERROR(Status))
    {
        goto ReleaseOwnership;
    }

    //
    // Read the entire flash into memory.
    //
    Buffer = AllocatePool(FlashSize);
    if (Buffer == NULL)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto ReleaseOwnership;
    }

    Status = gSpiProtocol.ReadData((UINTN)SpiBase, 0, Buffer, FlashSize);
    if (EFI_ERROR(Status))
    {
        FreePool(Buffer);
        goto ReleaseOwnership;
    }

    //
    // Write the buffer to the file, handling partial writes.
    //
    Status = EFI_SUCCESS;
    Offset = 0;
    while (Offset < FlashSize)
    {
        BufferSize = FlashSize - Offset;
        Status = File->Write(File, &BufferSize, Buffer + Offset);
        if (EFI_ERROR(Status))
        {
            break;
        }
        if (BufferSize == 0)
        {
            Status = EFI_DEVICE_ERROR;
            break;
        }
        Offset += BufferSize;
    }

    FreePool(Buffer);

ReleaseOwnership:
    gSpiProtocol.ReleaseOwnership((UINTN)SpiBase);

CloseFile:
    File->Close(File);
    Root->Close(Root);

    return Status;
}

///
/// Global instance of the Flash Protocol.
///
GLOBAL_REMOVE_IF_UNREFERENCED FLASH_PROTOCOL  gFlashProtocol = {
  FindWhereDumpImpl,
  DumpImpl,
  ReadFlashSizeFromSfdpImpl,
};