/** @file
  SPI Flash Protocol implementation for the AMD FCH SPI controller.

  Raw register access, ownership arbitration, command-mode transfers, and
  high-level flash operations (read, program, erase) against the SPI flash
  attached to the FCH.
**/

//
// Raw register access, exposed as SPI_PROTOCOL.Read8/16/32 and Write8/16/32.
//
// The FCH SPI register block is reachable as a 4 KiB window of the PCIe ECAM
// address space: PCIE_ECAM_ADDR() with Bus/Dev/Func = 0 reduces to Base +
// Offset, so the PCIe protocol's primitives serve as the MMIO path for the SPI
// controller and no caller has to touch MmioRead*/MmioWrite* directly.
//
// The write status is deliberately dropped here: the PCIe protocol verifies a
// write by reading the register back, which is meaningless for SPI registers
// with read-only or write-1-to-clear bits and for the SPIx80 FIFO window.
//

/**
  Reads an 8-bit SPI controller register.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase.

  @return The register value; 0xFF when the access was rejected.
**/
UINT8
EFIAPI
SpiRegRead8Impl(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset
)
{
  UINT8  Value;

  Value = 0xFF;
  gPcieProtocol.Read8(SpiBase, 0, 0, 0, Offset, &Value);

  return Value;
}

/**
  Reads a 16-bit SPI controller register.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (2-byte aligned).

  @return The register value; 0xFFFF when the access was rejected.
**/
UINT16
EFIAPI
SpiRegRead16Impl(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset
)
{
  UINT16  Value;

  Value = 0xFFFF;
  gPcieProtocol.Read16(SpiBase, 0, 0, 0, Offset, &Value);

  return Value;
}

/**
  Reads a 32-bit SPI controller register.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (4-byte aligned).

  @return The register value; 0xFFFFFFFF when the access was rejected.
**/
UINT32
EFIAPI
SpiRegRead32Impl(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset
)
{
  UINT32  Value;

  Value = 0xFFFFFFFF;
  gPcieProtocol.Read32(SpiBase, 0, 0, 0, Offset, &Value);

  return Value;
}

/**
  Writes an 8-bit SPI controller register.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase.
  @param[in]  Value    Value to write.
**/
VOID
EFIAPI
SpiRegWrite8Impl(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset,
  IN  UINT8   Value
)
{
  gPcieProtocol.Write8(SpiBase, 0, 0, 0, Offset, &Value);
}

/**
  Writes a 16-bit SPI controller register.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (2-byte aligned).
  @param[in]  Value    Value to write.
**/
VOID
EFIAPI
SpiRegWrite16Impl(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset,
  IN  UINT16  Value
)
{
  gPcieProtocol.Write16(SpiBase, 0, 0, 0, Offset, &Value);
}

/**
  Writes a 32-bit SPI controller register.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (4-byte aligned).
  @param[in]  Value    Value to write.
**/
VOID
EFIAPI
SpiRegWrite32Impl(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset,
  IN  UINT32  Value
)
{
  gPcieProtocol.Write32(SpiBase, 0, 0, 0, Offset, &Value);
}

/**
  Waits SPI_POLL_INTERVAL_US before the next controller status poll.

  Uses the boot service stall so the delay is real time; when the boot services
  table is not established yet the loop simply polls back to back.
**/
VOID
SpiPollDelay(
  VOID
)
{
  if ((gBS != NULL) && (gBS->Stall != NULL))
  {
    gBS->Stall(SPI_POLL_INTERVAL_US);
  }
}

/**
  Waits until the SPI controller has finished the running command.

  @param[in]  SpiBase    MMIO base address of the FCH SPI register block.
  @param[in]  TimeoutMs  Maximum time to wait, in milliseconds.

  @retval EFI_SUCCESS  The controller busy bit cleared.
  @retval EFI_TIMEOUT  The controller stayed busy for the whole timeout.
**/
EFI_STATUS
SpiWaitIdle(
  IN  UINTN  SpiBase,
  IN  UINTN  TimeoutMs
)
{
  UINTN                Iterations;
  SPI_CNTRL0_REGISTER  Cntrl0;

  //
  // SPIx00[31] is cleared by the controller when the transfer completes.
  //
  Iterations = TimeoutMs * (1000 / SPI_POLL_INTERVAL_US);
  while (Iterations > 0)
  {
    Cntrl0.Value = SpiRegRead32Impl(SpiBase, SPI_CNTRL0_OFFSET);
    if (!Cntrl0.Bits.SpiBusy)
    {
      return EFI_SUCCESS;
    }

    Iterations--;
    SpiPollDelay();
  }

  return EFI_TIMEOUT;
}

/**
  Runs one command-mode transfer against the SPI flash.

  @param[in]   SpiBase       MMIO base address of the FCH SPI register block.
  @param[in]   Opcode        SPI opcode written to SPIx45.
  @param[in]   AddressBytes  Address width in bytes: 0, 3, or 4.
  @param[in]   Address       Flash address, sent big endian.
  @param[in]   TxBuffer      Bytes sent behind the address; NULL when TxLength
                is 0.
  @param[in]   TxLength      Number of bytes to send from TxBuffer.
  @param[out]  RxBuffer      Receive buffer; NULL when RxLength is 0.
  @param[in]   RxLength      Number of bytes to receive.

  @retval EFI_SUCCESS  The transfer completed.
  @retval EFI_TIMEOUT  The controller stayed busy.
**/
EFI_STATUS
SpiTransferRaw(
  IN   UINTN   SpiBase,
  IN   UINT8   Opcode,
  IN   UINT8   AddressBytes,
  IN   UINT64  Address,
  IN   UINT8  *TxBuffer,
  IN   UINTN   TxLength,
  OUT  UINT8  *RxBuffer,
  IN   UINTN   RxLength
)
{
  EFI_STATUS           Status;
  UINTN                Index;
  UINT8                AddressData[SPI_MAX_ADDRESS_BYTES];
  CMDTRIGGER_REGISTER  CmdTrigger;

  Status = SpiWaitIdle(SpiBase, SPI_BUSY_TIMEOUT_MS);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  //
  // The opcode is not part of the FIFO payload: it goes into its own register.
  //
  SpiRegWrite8Impl(SpiBase, CMDCODE_OFFSET, Opcode);

  //
  // TX count covers the address plus the data; the RX count only the data that
  // the device shifts back, which the controller stores behind the TX bytes.
  //
  SpiRegWrite8Impl(SpiBase, TXBYTECOUNT_OFFSET, (UINT8)(AddressBytes + TxLength));
  SpiRegWrite8Impl(SpiBase, RXBYTECOUNT_OFFSET, (UINT8)RxLength);

  for (Index = 0; Index < AddressBytes; Index++)
  {
    AddressData[Index] = (UINT8)(Address >> ((AddressBytes - 1 - Index) * 8));
    SpiRegWrite8Impl(SpiBase, (UINT16)(SPI_FIFO_OFFSET + Index), AddressData[Index]);
  }

  for (Index = 0; Index < TxLength; Index++)
  {
    SpiRegWrite8Impl(SpiBase, (UINT16)(SPI_FIFO_OFFSET + AddressBytes + Index), TxBuffer[Index]);
  }

  //
  // SPIx47[7] starts the transfer; everything above must be written first.
  //
  CmdTrigger.Value = 0;
  CmdTrigger.Bits.Execute = 1;
  SpiRegWrite8Impl(SpiBase, CMDTRIGGER_OFFSET, CmdTrigger.Value);

  Status = SpiWaitIdle(SpiBase, SPI_BUSY_TIMEOUT_MS);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  for (Index = 0; Index < RxLength; Index++)
  {
    RxBuffer[Index] = SpiRegRead8Impl(
      SpiBase,
      (UINT16)(SPI_FIFO_OFFSET + AddressBytes + TxLength + Index)
    );
  }

  return EFI_SUCCESS;
}

/**
  Reads status register 1 of the SPI flash (internal helper).

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[out]  Status   Pointer to a caller-allocated UINT8 that receives the
            register.

  @retval EFI_SUCCESS  The status register was read.
  @retval EFI_TIMEOUT  The controller stayed busy.
**/
EFI_STATUS
SpiReadStatusRaw(
  IN   UINTN  SpiBase,
  OUT  UINT8  *Status
)
{
  return SpiTransferRaw(
    SpiBase,
    SPI_OPCODE_RDSR1,
    0,
    0,
    NULL,
    0,
    Status,
    1
  );
}

/**
  Claims the SPI controller for the host BIOS.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The controller is owned by the host BIOS.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_TIMEOUT            The mutex stayed with the PSP/HFP.
**/
EFI_STATUS
EFIAPI
SpiClaimOwnershipImpl(
  IN  UINTN  SpiBase
)
{
  UINTN                   Iterations;
  SPI_MISCCNTRL_REGISTER  Misc;
  SPI_CNTRL0_REGISTER     Cntrl0;

  if (SpiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  Iterations = SPI_OWNERSHIP_TIMEOUT_MS * (1000 / SPI_POLL_INTERVAL_US);
  while (Iterations > 0)
  {
    Misc.Value = SpiRegRead16Impl(SpiBase, SPI_MISCCNTRL_OFFSET);

    //
    // SPIxFC[10]/[9] are read-only owners; bit 5 is the Host-BIOS claim.
    //
    if (!Misc.Bits.PspOwnSpi && !Misc.Bits.HfpOwnSpi)
    {
      Misc.Bits.HbiosSpiMutex = 1;
      SpiRegWrite16Impl(SpiBase, SPI_MISCCNTRL_OFFSET, Misc.Value);

      //
      // Read back: hardware refuses the claim if the PSP/HFP won the race.
      //
      Misc.Value = SpiRegRead16Impl(SpiBase, SPI_MISCCNTRL_OFFSET);
      if (Misc.Bits.HbiosSpiMutex && !Misc.Bits.PspOwnSpi && !Misc.Bits.HfpOwnSpi)
      {
        //
        // Let the host reach the ROM: SPIx00[22] and SPIx00[23] are
        // clear-once enables, so setting them twice is harmless.
        //
        Cntrl0.Value = SpiRegRead32Impl(SpiBase, SPI_CNTRL0_OFFSET);
        if (!Cntrl0.Bits.SpiAccessMacRomEn || !Cntrl0.Bits.SpiHostAccessRomEn)
        {
          Cntrl0.Bits.SpiAccessMacRomEn = 1;
          Cntrl0.Bits.SpiHostAccessRomEn = 1;
          SpiRegWrite32Impl(SpiBase, SPI_CNTRL0_OFFSET, Cntrl0.Value);
        }

        return EFI_SUCCESS;
      }
    }

    Iterations--;
    SpiPollDelay();
  }

  return EFI_TIMEOUT;
}

/**
  Releases the SPI controller back to the PSP/HFP.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The controller was released.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
**/
EFI_STATUS
EFIAPI
SpiReleaseOwnershipImpl(
  IN  UINTN  SpiBase
)
{
  SPI_MISCCNTRL_REGISTER  Misc;

  if (SpiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  Misc.Value = SpiRegRead16Impl(SpiBase, SPI_MISCCNTRL_OFFSET);
  Misc.Bits.HbiosSpiMutex = 0;
  SpiRegWrite16Impl(SpiBase, SPI_MISCCNTRL_OFFSET, Misc.Value);

  return EFI_SUCCESS;
}

/**
  Reports whether the controller is executing a command.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[out]  Busy     Pointer to a caller-allocated BOOLEAN that receives the
            state.

  @retval EFI_SUCCESS            The busy state was read.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, or Busy is NULL.
**/
EFI_STATUS
EFIAPI
SpiIsBusyImpl(
  IN   UINTN    SpiBase,
  OUT  BOOLEAN  *Busy
)
{
  SPI_CNTRL0_REGISTER  Cntrl0;

  if ((SpiBase == 0) || (Busy == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Cntrl0.Value = SpiRegRead32Impl(SpiBase, SPI_CNTRL0_OFFSET);
  *Busy = (BOOLEAN)(Cntrl0.Bits.SpiBusy != 0);

  return EFI_SUCCESS;
}

/**
  Waits until the controller has finished the current command.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The controller is idle.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_TIMEOUT            The busy bit never cleared.
**/
EFI_STATUS
EFIAPI
SpiWaitReadyImpl(
  IN  UINTN  SpiBase
)
{
  if (SpiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  return SpiWaitIdle(SpiBase, SPI_BUSY_TIMEOUT_MS);
}

/**
  Runs one command-mode transfer against the SPI flash.

  @param[in]   SpiBase       MMIO base address of the FCH SPI register block.
  @param[in]   Opcode        SPI opcode (one of SPI_OPCODE_*).
  @param[in]   AddressBytes  Address width in bytes: 0, 3, or 4.
  @param[in]   Address       Flash address; ignored when AddressBytes is 0.
  @param[in]   TxBuffer      Pointer to the bytes written behind the address.
  @param[in]   TxLength      Number of bytes to write from TxBuffer.
  @param[out]  RxBuffer      Pointer to the caller-allocated receive buffer.
  @param[in]   RxLength      Number of bytes to receive into RxBuffer.

  @retval EFI_SUCCESS            The transfer completed.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, AddressBytes is not 0/3/4, a
                 buffer is NULL while its length is not 0, or
                 the payload exceeds the FIFO depth.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
EFI_STATUS
EFIAPI
SpiTransferImpl(
  IN   UINTN   SpiBase,
  IN   UINT8   Opcode,
  IN   UINT8   AddressBytes,
  IN   UINT64  Address,
  IN   UINT8  *TxBuffer,
  IN   UINTN   TxLength,
  OUT  UINT8  *RxBuffer,
  IN   UINTN   RxLength
)
{
  if ((SpiBase == 0) ||
    (AddressBytes > SPI_MAX_ADDRESS_BYTES) ||
    (AddressBytes == 1) ||
    (AddressBytes == 2))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (((TxLength != 0) && (TxBuffer == NULL)) ||
    ((RxLength != 0) && (RxBuffer == NULL)))
  {
    return EFI_INVALID_PARAMETER;
  }

  if ((AddressBytes + TxLength + RxLength) > SPI_FIFO_USABLE_BYTES)
  {
    return EFI_INVALID_PARAMETER;
  }

  return SpiTransferRaw(
    SpiBase,
    Opcode,
    AddressBytes,
    Address,
    TxBuffer,
    TxLength,
    RxBuffer,
    RxLength
  );
}

/**
  Reads the JEDEC identification of the SPI flash.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[out]  JedecId  Caller-allocated 3-byte array that receives the ID.

  @retval EFI_SUCCESS            The JEDEC ID was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, or JedecId is NULL.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
EFI_STATUS
EFIAPI
SpiReadJedecIdImpl(
  IN   UINTN  SpiBase,
  OUT  UINT8  JedecId[3]
)
{
  if ((SpiBase == 0) || (JedecId == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  return SpiTransferRaw(
    SpiBase,
    SPI_OPCODE_RDID,
    0,
    0,
    NULL,
    0,
    JedecId,
    3
  );
}

/**
  Reads one double word from the JEDEC SFDP parameter space of the flash.

  RDSFDP (5Ah) is followed by a 24-bit address and one dummy byte before the
  device drives the data back (MX25U12873F, "Read SFDP Mode (RDSFDP)"). The FCH
  controller has no separate dummy-cycle field in command mode, so the dummy
  byte is clocked out of the FIFO as transmit data; the four receive bytes land
  behind it in the FIFO.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]   Address  SFDP byte address (0 - 0xFFFFFF).
  @param[out]  Value    Receives the little-endian double word.

  @retval EFI_SUCCESS            The double word was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, Value is NULL, or Address does not fit in 24 bits.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
EFI_STATUS
EFIAPI
SpiReadSfdpImpl(
  IN   UINTN   SpiBase,
  IN   UINT32  Address,
  OUT  UINT32  *Value
)
{
  EFI_STATUS  Status;
  UINT8       Dummy;
  UINT8       Data[sizeof (UINT32)];

  if ((SpiBase == 0) || (Value == NULL) || (Address > 0xFFFFFF))
  {
    return EFI_INVALID_PARAMETER;
  }

  Dummy = 0xFF;

  Status = SpiTransferRaw(
    SpiBase,
    SPI_OPCODE_RDSFDP,
    3,
    (UINT64) Address,
    &Dummy,
    1,
    Data,
    sizeof (Data)
  );
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  *Value = ((UINT32) Data[0])       |
           ((UINT32) Data[1] << 8)  |
           ((UINT32) Data[2] << 16) |
           ((UINT32) Data[3] << 24);

  return EFI_SUCCESS;
}

/**
  Reads status register 1 of the SPI flash.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[out]  Status   Pointer to a caller-allocated UINT8 that receives the
            register.

  @retval EFI_SUCCESS            The status register was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, or Status is NULL.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
EFI_STATUS
EFIAPI
SpiReadStatusImpl(
  IN   UINTN  SpiBase,
  OUT  UINT8  *Status
)
{
  if ((SpiBase == 0) || (Status == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  return SpiReadStatusRaw(SpiBase, Status);
}

/**
  Arms the write enable latch with WREN (06h).

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The write enable latch was set.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_DEVICE_ERROR       The device did not report WEL after WREN.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
EFI_STATUS
EFIAPI
SpiWriteEnableImpl(
  IN  UINTN  SpiBase
)
{
  EFI_STATUS  Status;
  UINT8       FlashStatus;

  if (SpiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpiTransferRaw(SpiBase, SPI_OPCODE_WREN, 0, 0, NULL, 0, NULL, 0);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  //
  // WEL is the only proof that the following program/erase will be accepted.
  //
  Status = SpiReadStatusRaw(SpiBase, &FlashStatus);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  if ((FlashStatus & SPI_STATUS_WEL) == 0)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Waits until the flash has finished its internal program/erase cycle.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The device is idle.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_DEVICE_ERROR       The status register could not be read.
  @retval EFI_TIMEOUT            The device stayed busy for the whole timeout.
**/
EFI_STATUS
EFIAPI
SpiWaitFlashReadyImpl(
  IN  UINTN  SpiBase
)
{
  EFI_STATUS  Status;
  UINT8       FlashStatus;
  UINTN       Iterations;

  if (SpiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  Iterations = SPI_FLASH_TIMEOUT_MS * (1000 / SPI_POLL_INTERVAL_US);
  while (Iterations > 0)
  {
    Status = SpiReadStatusRaw(SpiBase, &FlashStatus);
    if (EFI_ERROR(Status))
    {
      return EFI_DEVICE_ERROR;
    }

    if ((FlashStatus & SPI_STATUS_WIP) == 0)
    {
      return EFI_SUCCESS;
    }

    Iterations--;
    SpiPollDelay();
  }

  return EFI_TIMEOUT;
}

/**
  Reads any range of the SPI flash with READ (03h).

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]   Address  Linear flash address of the first byte.
  @param[out]  Buffer   Pointer to the caller-allocated destination buffer.
  @param[in]   Length   Number of bytes to read.

  @retval EFI_SUCCESS            The range was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, Buffer is NULL while Length is
                 not 0, or the range wraps past 4 GiB.
  @retval EFI_TIMEOUT            A transfer timed out.
**/
EFI_STATUS
EFIAPI
SpiReadDataImpl(
  IN   UINTN   SpiBase,
  IN   UINT32  Address,
  OUT  UINT8  *Buffer,
  IN   UINTN   Length
)
{
  EFI_STATUS  Status;
  UINTN       Offset;
  UINTN       Chunk;

  if ((SpiBase == 0) || ((Length != 0) && (Buffer == NULL)))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (((UINT64)Address + (UINT64)Length) > 0x100000000ULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // One command-mode transfer carries at most SPI_CHUNK_BYTES of data.
  //
  for (Offset = 0; Offset < Length; Offset += Chunk)
  {
    Chunk = Length - Offset;
    if (Chunk > SPI_CHUNK_BYTES)
    {
      Chunk = SPI_CHUNK_BYTES;
    }

    Status = SpiTransferRaw(
      SpiBase,
      SPI_OPCODE_READ,
      3,
      (UINT64)(Address + (UINT32)Offset),
      NULL,
      0,
      Buffer + Offset,
      Chunk
    );
    if (EFI_ERROR(Status))
    {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Programs any range of the SPI flash with PAGEPROG (02h).

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Address  Linear flash address of the first byte.
  @param[in]  Buffer   Pointer to the caller-allocated source buffer.
  @param[in]  Length   Number of bytes to program.

  @retval EFI_SUCCESS            The range was programmed successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, Buffer is NULL while Length is
                 not 0, or the range wraps past 4 GiB.
  @retval EFI_DEVICE_ERROR       The write enable latch could not be armed.
  @retval EFI_TIMEOUT            A transfer or a program cycle timed out.
**/
EFI_STATUS
EFIAPI
SpiWriteDataImpl(
  IN  UINTN   SpiBase,
  IN  UINT32  Address,
  IN  UINT8  *Buffer,
  IN  UINTN   Length
)
{
  EFI_STATUS  Status;
  UINTN       Offset;
  UINTN       Chunk;
  UINTN       PageLeft;

  if ((SpiBase == 0) || ((Length != 0) && (Buffer == NULL)))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (((UINT64)Address + (UINT64)Length) > 0x100000000ULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  for (Offset = 0; Offset < Length; Offset += Chunk)
  {
    //
    // A page program never crosses a page boundary, and the command-mode
    // payload never crosses the FIFO limit.
    //
    PageLeft = SPI_PAGE_BYTES - ((Address + (UINT32)Offset) & (SPI_PAGE_BYTES - 1));
    Chunk = Length - Offset;
    if (Chunk > PageLeft)
    {
      Chunk = PageLeft;
    }

    if (Chunk > SPI_CHUNK_BYTES)
    {
      Chunk = SPI_CHUNK_BYTES;
    }

    Status = SpiWriteEnableImpl(SpiBase);
    if (EFI_ERROR(Status))
    {
      return Status;
    }

    Status = SpiTransferRaw(
      SpiBase,
      SPI_OPCODE_PAGEPROG,
      3,
      (UINT64)(Address + (UINT32)Offset),
      Buffer + Offset,
      Chunk,
      NULL,
      0
    );
    if (EFI_ERROR(Status))
    {
      return Status;
    }

    Status = SpiWaitFlashReadyImpl(SpiBase);
    if (EFI_ERROR(Status))
    {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Erases one 4 KiB sector of the SPI flash with SE (20h).

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Address  Linear flash address inside the target sector.

  @retval EFI_SUCCESS            The sector was erased.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_DEVICE_ERROR       The write enable latch could not be armed.
  @retval EFI_TIMEOUT            The transfer or the erase cycle timed out.
**/
EFI_STATUS
EFIAPI
SpiEraseSectorImpl(
  IN  UINTN   SpiBase,
  IN  UINT32  Address
)
{
  EFI_STATUS  Status;

  if (SpiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpiWriteEnableImpl(SpiBase);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  Status = SpiTransferRaw(SpiBase, SPI_OPCODE_SE_4K, 3, (UINT64)Address, NULL, 0, NULL, 0);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  return SpiWaitFlashReadyImpl(SpiBase);
}

///
/// Global instance of the SPI Protocol.
///
GLOBAL_REMOVE_IF_UNREFERENCED SPI_PROTOCOL  gSpiProtocol = {
  SpiClaimOwnershipImpl,
  SpiReleaseOwnershipImpl,

  SpiIsBusyImpl,
  SpiWaitReadyImpl,

  SpiTransferImpl,

  SpiReadJedecIdImpl,
  SpiReadSfdpImpl,
  SpiReadStatusImpl,
  SpiWriteEnableImpl,
  SpiWaitFlashReadyImpl,

  SpiReadDataImpl,
  SpiWriteDataImpl,
  SpiEraseSectorImpl,

  SpiRegRead8Impl,
  SpiRegRead16Impl,
  SpiRegRead32Impl,

  SpiRegWrite8Impl,
  SpiRegWrite16Impl,
  SpiRegWrite32Impl,
};

