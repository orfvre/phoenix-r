/** @file
  FCH SPI Protocol implementation.

  Retrieves the SPI controller base address, JEDEC flash ID, chip select
  state, and SPI ROM decode enables from the AMD FCH LPC bridge.
**/

/**
  Checks that the SPI base address reported by the LPC bridge can be touched.

  A missing LPC function or an unimplemented configuration register reads back as
  all ones, which leaves 0xFFFFFF00 after the base mask; probing MMIO at such an
  address can freeze the platform instead of failing cleanly.

  @param[in]  SpiBase  Base address reported by the LPC bridge.

  @retval TRUE   The base addresses the SPI register block.
  @retval FALSE  The base is 0 or carries the unimplemented-register pattern.
**/
BOOLEAN
FchIsSpiBaseValid(
  IN UINT32 SpiBase
)
{
  if ((SpiBase == 0) || (SpiBase >= 0xFFFFFF00))
  {
    return FALSE;
  }

  return TRUE;
}

/**
  Retrieves the SPI controller base address from the FCH LPC bridge.

  @param[out]  Buffer   Pointer to a caller-allocated UINT32 that receives the SPI base address.

  @retval EFI_SUCCESS            The SPI base address was retrieved successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL.
  @retval EFI_DEVICE_ERROR       The PCI read failed.
**/
EFI_STATUS
EFIAPI
FchGetLegacySpiBaseImpl(
  OUT UINT32  *Buffer
)
{
  EFI_STATUS Status;
  UINT32     SpiBase;

  if (Buffer == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gPciProtocol.Read32(0, 0x14, 3, 0xA0, &SpiBase);
  if (EFI_ERROR(Status))
  {
    return EFI_DEVICE_ERROR;
  }

  *Buffer = SpiBase & 0xFFFFFF00;

  return EFI_SUCCESS;
}

/**
  Retrieves the SPI controller base address from the FCH fixed MMIO address.

  @param[out]  Buffer   Pointer to a caller-allocated UINT32 that receives the SPI base address.

  @retval EFI_SUCCESS            The SPI base address was retrieved successfully.
  @retval EFI_INVALID_PARAMETER  This or Buffer is NULL.
  @retval EFI_DEVICE_ERROR       The read failed.
**/
EFI_STATUS
EFIAPI
FchGetSpiBaseImpl(
  OUT UINT32  *Buffer
)
{
  UINT32 Probe;

  if (Buffer == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Probe = gSpiProtocol.Read32 ((UINTN)SPI_MMIO_ADDRESS, 0);
  if (Probe == 0xFFFFFFFF)
  {
    return EFI_DEVICE_ERROR;
  }

  *Buffer = (UINT32)(SPI_MMIO_ADDRESS & 0xFFFFFFFF);

  return EFI_SUCCESS;
}

/**
  Retrieves the JEDEC manufacturer and device ID from the SPI flash.

  Claims the SPI controller, reads the ID with RDID (9Fh) through the SPI
  protocol, and releases the controller again.

  @param[out]  JedecId   Caller-allocated 3-byte array that receives the JEDEC ID
              (manufacturer, device type, capacity).

  @retval EFI_SUCCESS            The JEDEC ID was read successfully.
  @retval EFI_INVALID_PARAMETER  JedecId is NULL.
  @retval EFI_DEVICE_ERROR       The SPI base could not be resolved, the
                 controller could not be claimed, or the
                 transaction failed.
**/
EFI_STATUS
EFIAPI
FchGetSpiFlashIdImpl(
  OUT UINT8            JedecId[3]
)
{
  EFI_STATUS Status;
  UINT32     SpiBase;

  if (JedecId == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = FchGetLegacySpiBaseImpl(&SpiBase);
  if (EFI_ERROR(Status) || !FchIsSpiBaseValid(SpiBase))
  {
    return EFI_DEVICE_ERROR;
  }

  Status = gSpiProtocol.ClaimOwnership((UINTN)SpiBase);
  if (EFI_ERROR(Status))
  {
    return EFI_DEVICE_ERROR;
  }

  Status = gSpiProtocol.ReadJedecId((UINTN)SpiBase, JedecId);

  gSpiProtocol.ReleaseOwnership((UINTN)SpiBase);

  if (EFI_ERROR(Status))
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Enables or disables the SPI ROM decode of the FCH LPC bridge.

  Only the two chip-select enable bits of the SPI base address register
  (B0:D14h:F3, offset 0xA0h) are touched.

  @param[in]  Enable   Bit mask of SPI ROM chip selects to enable:
             FCH_SPI_ROM_ENABLE (CS0), FCH_SPI_ROM_ALT_ENABLE (CS1).
             Pass 0 to disable both decodes.

  @retval EFI_SUCCESS       The decode bits were updated.
  @retval EFI_DEVICE_ERROR  The PCI configuration read or write failed.
**/
EFI_STATUS
EFIAPI
FchSetSpiRomEnableImpl(
  IN  UINT32           Enable
)
{
  EFI_STATUS Status;
  UINT32     SpiBaseRegister;

  //
  // Read-modify-write: the SPI base address in bits [31:8] must survive.
  //
  Status = gPciProtocol.Read32(FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_SPI_BASE_REGISTER, &SpiBaseRegister);
  if (EFI_ERROR(Status))
  {
    return EFI_DEVICE_ERROR;
  }

  SpiBaseRegister &= ~(UINT32)(FCH_SPI_ROM_ENABLE | FCH_SPI_ROM_ALT_ENABLE);
  SpiBaseRegister |= (Enable & (FCH_SPI_ROM_ENABLE | FCH_SPI_ROM_ALT_ENABLE));

  Status = gPciProtocol.Write32(FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_SPI_BASE_REGISTER, &SpiBaseRegister);

  //
  // EFI_WARN_WRITE_FAILURE is not an EFI_ERROR(), so compare against success
  // explicitly to catch a write-back verification failure.
  //
  if (Status != EFI_SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Reads the chip select that currently drives the SPI flash.

  The boot chip select of the flash is reported by SPIx1D[1:0] (AltSpiCs).

  @param[out]  ChipSelect   Pointer to a caller-allocated UINT8 that receives the
              active chip select (0 - 3).

  @retval EFI_SUCCESS            The chip select was read successfully.
  @retval EFI_INVALID_PARAMETER  ChipSelect is NULL.
  @retval EFI_DEVICE_ERROR       The SPI base address could not be resolved.
**/
EFI_STATUS
EFIAPI
FchGetSpiChipSelectImpl(
  OUT UINT8  *ChipSelect
)
{
  EFI_STATUS Status;
  UINT32     SpiBase;
  ALT_SPI_CS_REGISTER AltSpiCs;

  if (ChipSelect == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = FchGetLegacySpiBaseImpl(&SpiBase);
  if (EFI_ERROR(Status) || !FchIsSpiBaseValid(SpiBase))
  {
    return EFI_DEVICE_ERROR;
  }

  AltSpiCs.Value = gSpiProtocol.Read8 ((UINTN)SpiBase, ALT_SPI_CS_OFFSET);
  *ChipSelect = (UINT8)AltSpiCs.Bits.AltSpiCsEn;

  return EFI_SUCCESS;
}

///
/// Global instance of the FCH Protocol.
///
GLOBAL_REMOVE_IF_UNREFERENCED FCH_PROTOCOL gFchProtocol = {
  FchGetLegacySpiBaseImpl,
  FchGetSpiBaseImpl,
  FchGetSpiFlashIdImpl,
  FchSetSpiRomEnableImpl,
  FchGetSpiChipSelectImpl
};