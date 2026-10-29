/** @file
  Custom PCIe Configuration Space Protocol definitions and implementation
  using ECAM (Enhanced Configuration Access Mechanism) via MMIO.
**/

#ifndef PCIE_PROTOCOL_H_
#define PCIE_PROTOCOL_H_

/**
  Macro to construct a PCIe ECAM Configuration Address.

  @param[in]  Base  ECAM base address for the target PCI segment.
  @param[in]  Bus   PCI Bus number (0 - 255).
  @param[in]  Dev   PCI Device number (0 - 31).
  @param[in]  Func  PCI Function number (0 - 7).
  @param[in]  Reg   PCI Configuration Register offset (0 - 4095).
**/
#define PCIE_ECAM_ADDR(Base, Bus, Dev, Func, Reg) \
  ((Base) + ((((UINTN)(Bus)) << 20) | \
  (((UINTN)(Dev)) << 15) | \
  (((UINTN)(Func)) << 12) | \
  ((UINTN)(Reg))))

typedef struct _PCIE_PROTOCOL PCIE_PROTOCOL;

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
typedef
EFI_STATUS
(EFIAPI *PCIE_READ_U8)(
  IN  UINTN   EcamBase,
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT16  Reg,
  OUT UINT8  *Buffer
  );

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
typedef
EFI_STATUS
(EFIAPI *PCIE_READ_U16)(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  OUT UINT16  *Buffer
  );

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
typedef
EFI_STATUS
(EFIAPI *PCIE_READ_U32)(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  OUT UINT32  *Buffer
  );

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
typedef
EFI_STATUS
(EFIAPI *PCIE_READ_U64)(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  OUT UINT64  *Buffer
  );

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
typedef
EFI_STATUS
(EFIAPI *PCIE_WRITE_U8)(
  IN  UINTN   EcamBase,
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT16  Reg,
  IN  UINT8  *Value
  );

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
typedef
EFI_STATUS
(EFIAPI *PCIE_WRITE_U16)(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  IN  UINT16  *Value
  );

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
typedef
EFI_STATUS
(EFIAPI *PCIE_WRITE_U32)(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  IN  UINT32  *Value
  );

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
typedef
EFI_STATUS
(EFIAPI *PCIE_WRITE_U64)(
  IN  UINTN    EcamBase,
  IN  UINT8    Bus,
  IN  UINT8    Dev,
  IN  UINT8    Func,
  IN  UINT16   Reg,
  IN  UINT64  *Value
  );

struct _PCIE_PROTOCOL {
  PCIE_READ_U8    Read8;
  PCIE_READ_U16   Read16;
  PCIE_READ_U32   Read32;
  PCIE_READ_U64   Read64;

  PCIE_WRITE_U8   Write8;
  PCIE_WRITE_U16  Write16;
  PCIE_WRITE_U32  Write32;
  PCIE_WRITE_U64  Write64;
};

#endif // PCIE_PROTOCOL_H_
