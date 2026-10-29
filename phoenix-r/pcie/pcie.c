/**
  Reads an 8-bit value from PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095).
  @param[out]  Buffer    Pointer to a caller-allocated UINT8 that receives the value.

  @retval EFI_SUCCESS            The 8-bit PCIe register was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or Dev/Func/Reg exceeds limits.
**/
EFI_STATUS
EFIAPI
PcieReadU8Impl(
  IN  UINTN   EcamBase,
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT16  Reg,
  OUT UINT8  *Buffer
)
{
  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFFF))
  {
    return EFI_INVALID_PARAMETER;
  }

  *Buffer = MmioRead8(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));

  return EFI_SUCCESS;
}

/**
  Reads a 16-bit value from PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095, must be 2-byte aligned).
  @param[out]  Buffer    Pointer to a caller-allocated UINT16 that receives the value.

  @retval EFI_SUCCESS            The 16-bit PCIe register was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is unaligned, or Dev/Func/Reg exceeds limits.
**/
EFI_STATUS
EFIAPI
PcieReadU16Impl(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  OUT UINT16  *Buffer
)
{
  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFFF) || ((Reg & 0x01) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  *Buffer = MmioRead16(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));

  return EFI_SUCCESS;
}

/**
  Reads a 32-bit value from PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095, must be 4-byte aligned).
  @param[out]  Buffer    Pointer to a caller-allocated UINT32 that receives the value.

  @retval EFI_SUCCESS            The 32-bit PCIe register was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is unaligned, or Dev/Func/Reg exceeds limits.
**/
EFI_STATUS
EFIAPI
PcieReadU32Impl(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  OUT UINT32  *Buffer
)
{
  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFFF) || ((Reg & 0x03) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  *Buffer = MmioRead32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));

  return EFI_SUCCESS;
}

/**
  Reads a 64-bit value from PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095, must be 4-byte aligned and <= 0xFF8).
  @param[out]  Buffer    Pointer to a caller-allocated UINT64 that receives the value.

  @retval EFI_SUCCESS            The 64-bit PCIe register was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is invalid, or Dev/Func/Reg exceeds limits.
**/
EFI_STATUS
EFIAPI
PcieReadU64Impl(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  OUT UINT64  *Buffer
)
{
  UINT32 LowDword;
  UINT32 HighDword;

  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFF8) || ((Reg & 0x03) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  LowDword = MmioRead32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));
  HighDword = MmioRead32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg + 4));

  *Buffer = ((UINT64)HighDword << 32) | (UINT64)LowDword;

  return EFI_SUCCESS;
}

/**
  Writes an 8-bit value to PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095).
  @param[in]   Value     Pointer to a caller-allocated UINT8 with the value to write.

  @retval EFI_SUCCESS            The 8-bit PCIe register was written successfully.
  @retval EFI_INVALID_PARAMETER  Value is NULL, or Dev/Func/Reg exceeds limits.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PcieWriteU8Impl(
  IN  UINTN   EcamBase,
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT16  Reg,
  IN  UINT8  *Value
)
{
  UINT8 TempValue;

  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFFF))
  {
    return EFI_INVALID_PARAMETER;
  }

  MmioWrite8(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg), *Value);
  TempValue = MmioRead8(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));

  if (TempValue != *Value)
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

/**
  Writes a 16-bit value to PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095, must be 2-byte aligned).
  @param[in]   Value     Pointer to a caller-allocated UINT16 with the value to write.

  @retval EFI_SUCCESS            The 16-bit PCIe register was written successfully.
  @retval EFI_INVALID_PARAMETER  Value is NULL, Reg is unaligned, or Dev/Func/Reg exceeds limits.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PcieWriteU16Impl(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  IN  UINT16  *Value
)
{
  UINT16 TempValue;

  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFFF) || ((Reg & 0x01) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  MmioWrite16(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg), *Value);
  TempValue = MmioRead16(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));

  if (TempValue != *Value)
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

/**
  Writes a 32-bit value to PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095, must be 4-byte aligned).
  @param[in]   Value     Pointer to a caller-allocated UINT32 with the value to write.

  @retval EFI_SUCCESS            The 32-bit PCIe register was written successfully.
  @retval EFI_INVALID_PARAMETER  Value is NULL, Reg is unaligned, or Dev/Func/Reg exceeds limits.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PcieWriteU32Impl(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  IN  UINT32  *Value
)
{
  UINT32 TempValue;

  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFFF) || ((Reg & 0x03) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  MmioWrite32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg), *Value);
  TempValue = MmioRead32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));

  if (TempValue != *Value)
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

/**
  Writes a 64-bit value to PCIe configuration space via ECAM.

  @param[in]   EcamBase  ECAM base address for the target PCI segment.
  @param[in]   Bus       PCI Bus number.
  @param[in]   Dev       PCI Device number (0 - 31).
  @param[in]   Func      PCI Function number (0 - 7).
  @param[in]   Reg       PCI Register offset (0 - 4095, must be 4-byte aligned and <= 0xFF8).
  @param[in]   Value     Pointer to a caller-allocated UINT64 with the value to write.

  @retval EFI_SUCCESS            The 64-bit PCIe register was written successfully.
  @retval EFI_INVALID_PARAMETER  Value is NULL, Reg is invalid, or Dev/Func/Reg exceeds limits.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PcieWriteU64Impl(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  IN  UINT64  *Value
)
{
  UINT32 TempLo;
  UINT32 TempHi;
  UINT32 Lo;
  UINT32 Hi;

  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07) || (Reg > 0xFF8) || ((Reg & 0x03) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  Lo = (UINT32)(*Value);
  Hi = (UINT32)(*Value >> 32);

  MmioWrite32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg), Lo);
  MmioWrite32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg + 4), Hi);

  TempLo = MmioRead32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg));
  TempHi = MmioRead32(PCIE_ECAM_ADDR(EcamBase, Bus, Dev, Func, Reg + 4));

  if ((TempLo != Lo) || (TempHi != Hi))
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

///
/// Global instance of the PCIe Protocol.
///
GLOBAL_REMOVE_IF_UNREFERENCED PCIE_PROTOCOL gPcieProtocol = {
  PcieReadU8Impl,
  PcieReadU16Impl,
  PcieReadU32Impl,
  PcieReadU64Impl,

  PcieWriteU8Impl,
  PcieWriteU16Impl,
  PcieWriteU32Impl,
  PcieWriteU64Impl,
};