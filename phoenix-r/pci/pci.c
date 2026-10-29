/**
  Reads an 8-bit value from PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number (0 - 31).
  @param[in]   Func    PCI Function number (0 - 7).
  @param[in]   Reg     PCI Register offset.
  @param[out]  Buffer  Pointer to the UINT8 output buffer.

  @retval EFI_SUCCESS            The PCI register was read into Buffer.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or Dev/Func exceeds PCI limits.
**/
EFI_STATUS
EFIAPI
PciReadU8Impl(
  IN  UINT8  Bus,
  IN  UINT8  Dev,
  IN  UINT8  Func,
  IN  UINT8  Reg,
  OUT UINT8  *Buffer
)
{
  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07))
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Port 0xCF8 MUST be written as a 32-bit DWORD.
  // Port 0xCFC + (Reg & 3) selects the exact byte lane (0, 1, 2, or 3).
  //
  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  *Buffer = IoRead8(CONFIG_DATA + (Reg & 0x03));

  return EFI_SUCCESS;
}

/**
  Reads a 16-bit value from PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number (0 - 31).
  @param[in]   Func    PCI Function number (0 - 7).
  @param[in]   Reg     PCI Register offset (must be 2-byte aligned).
  @param[out]  Buffer  Pointer to the UINT16 output buffer.

  @retval EFI_SUCCESS            The PCI register was read into Buffer.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is unaligned, or Dev/Func exceeds limits.
**/
EFI_STATUS
EFIAPI
PciReadU16Impl(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  OUT UINT16  *Buffer
)
{
  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07) || ((Reg & 0x01) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  *Buffer = IoRead16(CONFIG_DATA + (Reg & 0x02));

  return EFI_SUCCESS;
}

/**
  Reads a 32-bit value from PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number (0 - 31).
  @param[in]   Func    PCI Function number (0 - 7).
  @param[in]   Reg     PCI Register offset (must be 4-byte aligned).
  @param[out]  Buffer  Pointer to the UINT32 output buffer.

  @retval EFI_SUCCESS            The PCI register was read into Buffer.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is unaligned, or Dev/Func exceeds limits.
**/
EFI_STATUS
EFIAPI
PciReadU32Impl(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  OUT UINT32  *Buffer
)
{
  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07) || ((Reg & 0x03) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  *Buffer = IoRead32(CONFIG_DATA);

  return EFI_SUCCESS;
}

/**
  Reads a 64-bit value from PCI configuration space using two 32-bit cycles.
  (Note: Legacy x86 I/O ports 0xCF8/0xCFC are 32-bit wide max, so 64-bit reads
  are split into lower DWORD at Reg and upper DWORD at Reg + 4).

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number (0 - 31).
  @param[in]   Func    PCI Function number (0 - 7).
  @param[in]   Reg     PCI Register offset (must be 4-byte aligned and <= 0xF8).
  @param[out]  Buffer  Pointer to the UINT64 output buffer.

  @retval EFI_SUCCESS            The 64-bit PCI value was read into Buffer.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is invalid, or Dev/Func exceeds limits.
**/
EFI_STATUS
EFIAPI
PciReadU64Impl(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  OUT UINT64  *Buffer
)
{
  UINT32  LowDword;
  UINT32  HighDword;

  if ((Buffer == NULL) || (Dev > 0x1F) || (Func > 0x07) || ((Reg & 0x03) != 0) || (Reg > 0xF8))
  {
    return EFI_INVALID_PARAMETER;
  }

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  LowDword = IoRead32(CONFIG_DATA);

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, (UINT8)(Reg + 4)));
  HighDword = IoRead32(CONFIG_DATA);

  *Buffer = ((UINT64)HighDword << 32) | (UINT64)LowDword;

  return EFI_SUCCESS;
}

/**
  Write an 8-bit value to PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset.
  @param[in]   Value   Pointer to the UINT8 value to write.

  @retval EFI_SUCCESS            The 8-bit PCI register was written successfully.
  @retval EFI_INVALID_PARAMETER  Value is NULL, or Dev/Func is out of range.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PciWriteU8Impl(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT8  *Value
)
{
  UINT8 TempValue;
  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07))
  {
    return EFI_INVALID_PARAMETER;
  }

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  IoWrite8(CONFIG_DATA + (Reg & 0x03), *Value);
  TempValue = IoRead8(CONFIG_DATA + (Reg & 0x03));

  if (TempValue != *Value)
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

/**
  Write a 16-bit value to PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset.
  @param[in]   Value   Pointer to the UINT16 value to write.

  @retval EFI_SUCCESS            The 16-bit PCI register was written successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or Dev/Func is out of range.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PciWriteU16Impl(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT16  *Value
)
{
  UINT16 TempValue;
  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07) || ((Reg & 0x01) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  IoWrite16(CONFIG_DATA + (Reg & 0x02), *Value);
  TempValue = IoRead16(CONFIG_DATA + (Reg & 0x02));

  if (TempValue != *Value)
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

/**
  Write a 32-bit value to PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset.
  @param[in]   Value   Pointer to the UINT32 value to write.

  @retval EFI_SUCCESS            The 32-bit PCI register was written successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or Dev/Func is out of range.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PciWriteU32Impl(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT32  *Value
)
{
  UINT32 TempValue;
  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07) || ((Reg & 0x03) != 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  IoWrite32(CONFIG_DATA, *Value);
  TempValue = IoRead32(CONFIG_DATA);

  if (TempValue != *Value)
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

/**
  Write a 64-bit value to PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset.
  @param[in]   Value   Pointer to the UINT64 value to write.

  @retval EFI_SUCCESS            The 64-bit PCI register was written successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or Dev/Func is out of range.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
EFI_STATUS
EFIAPI
PciWriteU64Impl(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT64  *Value
)
{
  UINT32 TempLo;
  UINT32 TempHi;
  UINT32 Lo;
  UINT32 Hi;

  if ((Value == NULL) || (Dev > 0x1F) || (Func > 0x07) || ((Reg & 0x03) != 0) || (Reg > 0xF8))
  {
    return EFI_INVALID_PARAMETER;
  }

  Lo = (UINT32)(*Value);
  Hi = (UINT32)(*Value >> 32);

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  IoWrite32(CONFIG_DATA, Lo);

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg + 4));
  IoWrite32(CONFIG_DATA, Hi);

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg));
  TempLo = IoRead32(CONFIG_DATA);

  IoWrite32(CONFIG_ADDRESS, PCI_CF8_ADDR(Bus, Dev, Func, Reg + 4));
  TempHi = IoRead32(CONFIG_DATA);

  if ((TempLo != Lo) || (TempHi != Hi))
  {
    return EFI_WARN_WRITE_FAILURE;
  }

  return EFI_SUCCESS;
}

///
/// Global instance of the PCI Protocol.
///
GLOBAL_REMOVE_IF_UNREFERENCED PCI_PROTOCOL gPciProtocol = {
  PciReadU8Impl,
  PciReadU16Impl,
  PciReadU32Impl,
  PciReadU64Impl,

  PciWriteU8Impl,
  PciWriteU16Impl,
  PciWriteU32Impl,
  PciWriteU64Impl,
};