/** @file
  Custom Flash Dump Protocol for dumping SPI flash contents to storage.
**/

#ifndef FLASH_PROTOCOL_H_
#define FLASH_PROTOCOL_H_

typedef struct _FLASH_PROTOCOL FLASH_PROTOCOL;

/**
  Locate a writable destination for a flash dump (e.g. a USB storage device).

  @param[out]  HandleUsb  On success, a pointer to the Simple File System
              protocol on the first USB volume found.

  @retval EFI_SUCCESS    A writable USB destination was found.
  @retval EFI_NOT_FOUND  No USB volume with a file system is available.
**/
typedef
EFI_STATUS
(EFIAPI *FLASH_FIND_WHERE_DUMP)(
  OUT EFI_SIMPLE_FILE_SYSTEM_PROTOCOL **HandleUsb
  );

/**
  Read the entire SPI flash and write its contents to the destination
  found by FLASH_FIND_WHERE_DUMP.

  @retval EFI_SUCCESS       Flash dump completed successfully.
  @retval EFI_NOT_FOUND     No writable destination available.
  @retval EFI_DEVICE_ERROR  SPI read or file write failed.
**/
typedef
EFI_STATUS
(EFIAPI *FLASH_DUMP)(
  VOID
  );

/**
  Read the flash size in bytes from the SFDP Basic Flash Parameter Table.

  @param[in]   SpiBase    SPI controller MMIO base address.
  @param[out]  FlashSize  Flash size in bytes.

  @retval EFI_SUCCESS       Size read successfully.
  @retval EFI_DEVICE_ERROR  SFDP read failed or signature mismatch.
**/
typedef
EFI_STATUS
(EFIAPI *FLASH_READ_SIZE)(
  IN  UINTN   SpiBase,
  OUT UINT32  *FlashSize
  );

///
/// Custom Flash Dump Protocol Structure.
///
struct _FLASH_PROTOCOL {
  FLASH_FIND_WHERE_DUMP  FindWhereDump;
  FLASH_DUMP             Dump;
  FLASH_READ_SIZE        ReadFlashSize;
};

#endif // FLASH_PROTOCOL_H_
