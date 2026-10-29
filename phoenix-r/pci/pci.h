/** @file
  Custom PCI Configuration Space Protocol definitions and implementation
  using Legacy PCI Configuration Mechanism #1 (I/O Ports 0xCF8 / 0xCFC).
**/

#ifndef PCI_PROTOCOL_H_
#define PCI_PROTOCOL_H_

#define CONFIG_ADDRESS  0xCF8
#define CONFIG_DATA     0xCFC

/**
  Macro to construct a 32-bit PCI Configuration Address for Port 0xCF8.

  @param[in]  Bus   PCI Bus number (0 - 255).
  @param[in]  Dev   PCI Device number (0 - 31).
  @param[in]  Func  PCI Function number (0 - 7).
  @param[in]  Reg   PCI Configuration Register offset (0 - 255).
**/
#define PCI_CF8_ADDR(Bus, Dev, Func, Reg) \
   ((UINT32)(0x80000000ULL | \
   (((UINT32)(Bus)  & 0xFF) << 16) | \
   (((UINT32)(Dev)  & 0x1F) << 11) | \
   (((UINT32)(Func) & 0x07) << 8)  | \
   ((UINT32)(Reg)   & 0xFC)))

typedef struct _PCI_PROTOCOL PCI_PROTOCOL;

/**
  Reads an 8-bit value from PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset.
  @param[out]  Buffer  Pointer to a caller-allocated UINT8 that receives the value.

  @retval EFI_SUCCESS            The 8-bit PCI register was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or Dev/Func is out of range.
**/
typedef
EFI_STATUS
(EFIAPI *PCI_READ_U8)(
  IN  UINT8  Bus,
  IN  UINT8  Dev,
  IN  UINT8  Func,
  IN  UINT8  Reg,
  OUT UINT8  *Buffer
  );

/**
  Reads a 16-bit value from PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset (must be 2-byte aligned).
  @param[out]  Buffer  Pointer to a caller-allocated UINT16 that receives the value.

  @retval EFI_SUCCESS            The 16-bit PCI register was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is unaligned, or Dev/Func is out of range.
**/
typedef
EFI_STATUS
(EFIAPI *PCI_READ_U16)(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  OUT UINT16  *Buffer
  );

/**
  Reads a 32-bit value from PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset (must be 4-byte aligned).
  @param[out]  Buffer  Pointer to a caller-allocated UINT32 that receives the value.

  @retval EFI_SUCCESS            The 32-bit PCI register was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is unaligned, or Dev/Func is out of range.
**/
typedef
EFI_STATUS
(EFIAPI *PCI_READ_U32)(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  OUT UINT32  *Buffer
  );

/**
  Reads a 64-bit value (two consecutive 32-bit DWORDs) from PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset (must be 4-byte aligned and <= 0xF8).
  @param[out]  Buffer  Pointer to a caller-allocated UINT64 that receives the value.

  @retval EFI_SUCCESS            The 64-bit PCI register pair was read successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, Reg is unaligned, or Dev/Func is out of range.
**/
typedef
EFI_STATUS
(EFIAPI *PCI_READ_U64)(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  OUT UINT64  *Buffer
  );

/**
  Write an 8-bit value to PCI configuration space.

  @param[in]   Bus     PCI Bus number.
  @param[in]   Dev     PCI Device number.
  @param[in]   Func    PCI Function number.
  @param[in]   Reg     PCI Register offset.
  @param[in]   Value   Pointer to the UINT8 value to write.

  @retval EFI_SUCCESS            The 8-bit PCI register was written successfully.
  @retval EFI_INVALID_PARAMETER  Buffer is NULL, or Dev/Func is out of range.
  @retval EFI_WARN_WRITE_FAILURE The write-back verification failed.
**/
typedef
EFI_STATUS
(EFIAPI *PCI_WRITE_U8)(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT8  *Value
  );

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
typedef
EFI_STATUS
(EFIAPI *PCI_WRITE_U16)(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT16  *Value
  );

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
typedef
EFI_STATUS
(EFIAPI *PCI_WRITE_U32)(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT32  *Value
  );

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
typedef
EFI_STATUS
(EFIAPI *PCI_WRITE_U64)(
  IN  UINT8   Bus,
  IN  UINT8   Dev,
  IN  UINT8   Func,
  IN  UINT8   Reg,
  IN  UINT64  *Value
  );


///
/// Custom PCI Protocol Structure.
///
struct _PCI_PROTOCOL {
  PCI_READ_U8   Read8;
  PCI_READ_U16  Read16;
  PCI_READ_U32  Read32;
  PCI_READ_U64  Read64;

  PCI_WRITE_U8  Write8;
  PCI_WRITE_U16 Write16;
  PCI_WRITE_U32 Write32;
  PCI_WRITE_U64 Write64;
};

#endif // PCI_PROTOCOL_H_
