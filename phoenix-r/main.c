/** @file
  phoenix-r UEFI driver entry point.

  Enumerates the SPI flash through the custom FCH and SPI protocols, dump the flash chip content and
  prints basic platform information.
**/

extern CONST UINT32 _gUefiDriverRevision = 0;
CHAR8 *gEfiCallerBaseName = "phoenix-r";

EFI_STATUS
EFIAPI
UefiUnload (
  IN EFI_HANDLE ImageHandle
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  UINT32     SpiBase;
  UINT8      JedecId[3];
  UINT8      FlashStatus;
  UINT8      ChipSelect;

  if (SystemTable != NULL)
  {
    gST = SystemTable;
    gBS = SystemTable->BootServices;
    gRT = SystemTable->RuntimeServices;
  }

  SpiBase = 0;
  Status = gFchProtocol.GetLegacySpiBase(&SpiBase);
  Print(L"SPI controller base, 0x%08X (%r)\n", (UINTN)SpiBase, Status);

  ChipSelect = 0;
  Status = gFchProtocol.GetSpiChipSelect(&ChipSelect);
  Print(L"SPI boot chip select, %u (%r)\n", (UINTN)ChipSelect, Status);

  //
  // Direct use of the SPI protocol: claim the controller, read the flash
  // status, read the JEDEC ID, then hand the controller back.
  //
  // 0xFFFFFF00 is what an unimplemented LPC register leaves behind; probing
  // MMIO there can freeze the machine, so skip the transactions.
  //
  if ((SpiBase == 0) || (SpiBase >= 0xFFFFFF00))
  {
    Print(L"SPI controller base unusable, skipping SPI transactions\n");
  }
  else
  {
    Status = gSpiProtocol.ClaimOwnership((UINTN)SpiBase);
    Print(L"SPI controller claim, %r\n", Status);

    if (!EFI_ERROR(Status))
    {
      FlashStatus = 0;
      Status = gSpiProtocol.ReadStatus((UINTN)SpiBase, &FlashStatus);
      Print(L"SPI flash status, 0x%02X (%r)\n", (UINTN)FlashStatus, Status);

      JedecId[0] = 0;
      JedecId[1] = 0;
      JedecId[2] = 0;
      Status = gSpiProtocol.ReadJedecId((UINTN)SpiBase, JedecId);
      Print(
        L"SPI flash JEDEC ID, %02X %02X %02X (%r)\n",
        (UINTN)JedecId[0],
        (UINTN)JedecId[1],
        (UINTN)JedecId[2],
        Status
        );
      gSpiProtocol.ReleaseOwnership((UINTN)SpiBase);
    }
  }

  //
  // Same read through the FCH protocol, which claims and releases the
  // controller itself.
  //
  JedecId[0] = 0;
  JedecId[1] = 0;
  JedecId[2] = 0;
  Status = gFchProtocol.GetSpiFlashId(JedecId);
  Print(
    L"SPI flash JEDEC ID via FCH, %02X %02X %02X (%r)\n",
    (UINTN)JedecId[0],
    (UINTN)JedecId[1],
    (UINTN)JedecId[2],
    Status
    );

  //
  // Dump the entire SPI flash to a USB volume if one is attached.
  //
  Status = gFlashProtocol.Dump();
  Print(L"SPI flash dump, %r\n", Status);

  Print(L"phoenix-r: loaded (ImageHandle=%p, SystemTable=%p)\n",ImageHandle, SystemTable);
  Print(L"phoenix-r: firmware %u.%02u\n",SystemTable->Hdr.Revision >> 16,SystemTable->Hdr.Revision & 0xFFFF);
  return EFI_SUCCESS;
}
