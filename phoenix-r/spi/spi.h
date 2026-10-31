/** @file
  Custom SPI Flash Protocol definitions for the AMD FCH SPI controller.

  Transfers use the controller's command mode: the SPI opcode is written to
  SPIx45, the remaining bytes (address followed by TX data) go into the SPIx80
  FIFO, and the received bytes are read back from the FIFO behind the TX data.

  @note  AMD PPR Family 19h Model 11h Rev B2, Volume 5, Section 10.3.8.1.2.
**/

#ifndef SPI_PROTOCOL_H_
#define SPI_PROTOCOL_H_

//
// SPI flash opcodes (single I/O, JEDEC common set).
//
#define SPI_OPCODE_RDID       0x9F  ///< Read JEDEC ID (manufacturer, memory type, capacity).
#define SPI_OPCODE_RDSFDP     0x5A  ///< Read SFDP (JESD216 parameter tables).
#define SPI_OPCODE_RDSR1      0x05  ///< Read status register 1.
#define SPI_OPCODE_RDSR2      0x35  ///< Read status register 2.
#define SPI_OPCODE_WREN       0x06  ///< Write enable (sets WEL in status register 1).
#define SPI_OPCODE_WRSR1      0x01  ///< Write status register 1.
#define SPI_OPCODE_READ       0x03  ///< Read data (24-bit address, no dummy cycles).
#define SPI_OPCODE_FAST_READ  0x0B  ///< Fast read (24-bit address, dummy cycles).
#define SPI_OPCODE_PAGEPROG   0x02  ///< Page program (24-bit address, up to 256 bytes).
#define SPI_OPCODE_SE_4K      0x20  ///< Sector erase, 4 KiB.
#define SPI_OPCODE_BE_64K     0xD8  ///< Block erase, 64 KiB.
#define SPI_OPCODE_CE         0x60  ///< Chip erase.
#define SPI_OPCODE_RSTEN      0x66  ///< Reset enable.
#define SPI_OPCODE_RST        0x99  ///< Reset device.

//
// Status register 1 (RDSR1) bit masks.
//
#define SPI_STATUS_WIP  0x01  ///< Write in progress: device is busy.
#define SPI_STATUS_WEL  0x02  ///< Write enable latch: a write/erase is armed.

//
// Controller limits and timeouts.
//
#define SPI_FIFO_USABLE_BYTES   71   ///< SPIx80..SPIxC6; SPIxC7 is reserved on some FCH revisions.
#define SPI_MAX_ADDRESS_BYTES   4    ///< 4-byte addressing is supported through SPIx50[0].
#define SPI_MAX_XFER_BYTES      (SPI_FIFO_USABLE_BYTES - SPI_MAX_ADDRESS_BYTES)  ///< 67 bytes.
#define SPI_CHUNK_BYTES         64   ///< Data bytes per command-mode transfer (page aligned).
#define SPI_PAGE_BYTES          256  ///< SPI flash page program size.

#define SPI_OWNERSHIP_TIMEOUT_MS  1000  ///< Host-BIOS SPI mutex acquisition timeout.
#define SPI_BUSY_TIMEOUT_MS       500   ///< Controller busy-clear timeout per command.
#define SPI_FLASH_TIMEOUT_MS      5000  ///< Flash write-in-progress timeout per operation.
#define SPI_POLL_INTERVAL_US      10    ///< Delay between two status polls.

typedef struct _SPI_PROTOCOL SPI_PROTOCOL;

/**
  Claims the SPI controller for the host BIOS.

  Polls SPIxFC until neither the PSP nor the HFP owns the SPI mutex, sets the
  Host-BIOS ownership bit, and confirms the claim by reading the register back
  (hardware rejects the claim if the PSP/HFP grabbed it first).  Once owned, the
  host ROM access bits of SPIx00 are enabled so that the controller allows the
  transfer.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The controller is owned by the host BIOS.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_TIMEOUT            The mutex stayed with the PSP/HFP for the whole
                 timeout.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_CLAIM_OWNERSHIP)(
  IN  UINTN  SpiBase
  );

/**
  Releases the SPI controller back to the PSP/HFP.

  Clears only the Host-BIOS ownership bit of SPIxFC; everything else in the
  register is preserved.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The controller was released.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_RELEASE_OWNERSHIP)(
  IN  UINTN  SpiBase
  );

/**
  Reports whether the controller is executing a command.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[out]  Busy     Pointer to a caller-allocated BOOLEAN that receives the
            SPIx00[31] busy state.

  @retval EFI_SUCCESS            The busy state was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, or Busy is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_IS_BUSY)(
  IN   UINTN    SpiBase,
  OUT  BOOLEAN  *Busy
  );

/**
  Waits until the controller has finished the current command.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The controller is idle.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_TIMEOUT            The busy bit never cleared.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_WAIT_READY)(
  IN  UINTN  SpiBase
  );

/**
  Runs one command-mode transfer against the SPI flash.

  The opcode goes to SPIx45, the address (big endian, AddressBytes wide) and
  TxBuffer follow into the FIFO, and the received bytes are taken from the FIFO
  directly behind them.

  @param[in]   SpiBase       MMIO base address of the FCH SPI register block.
  @param[in]   Opcode        SPI opcode (one of SPI_OPCODE_*).
  @param[in]   AddressBytes  Address width in bytes: 0, 3, or 4.
  @param[in]   Address       Flash address; ignored when AddressBytes is 0.
  @param[in]   TxBuffer      Pointer to the bytes written behind the address;
                NULL when TxLength is 0.
  @param[in]   TxLength      Number of bytes to write from TxBuffer.
  @param[out]  RxBuffer      Pointer to the caller-allocated receive buffer;
                NULL when RxLength is 0.
  @param[in]   RxLength      Number of bytes to receive into RxBuffer.

  @retval EFI_SUCCESS            The transfer completed.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, AddressBytes is not 0/3/4, a
                 buffer is NULL while its length is not 0, or
                 the payload exceeds the FIFO depth.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_TRANSFER)(
  IN   UINTN   SpiBase,
  IN   UINT8   Opcode,
  IN   UINT8   AddressBytes,
  IN   UINT64  Address,
  IN   UINT8  *TxBuffer,
  IN   UINTN   TxLength,
  OUT  UINT8  *RxBuffer,
  IN   UINTN   RxLength
  );

/**
  Reads the JEDEC identification of the SPI flash.

  Issues RDID (9Fh) without an address and returns the three identification
  bytes: manufacturer, memory type, and capacity.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[out]  JedecId  Caller-allocated 3-byte array that receives the ID.

  @retval EFI_SUCCESS            The JEDEC ID was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, or JedecId is NULL.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_READ_JEDEC_ID)(
  IN   UINTN  SpiBase,
  OUT  UINT8   JedecId[3]
  );

/**
  Reads one double word from the JEDEC SFDP parameter space of the flash.

  Issues RDSFDP (5Ah) with a 24-bit address and one dummy byte, then clocks the
  four data bytes in, as described in the "Read SFDP Mode (RDSFDP)" section of
  the MX25U12873F datasheet. The address is a byte address inside the SFDP
  space; the double word that contains it is returned.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]   Address  SFDP byte address (0 - 0xFFFFFF).
  @param[out]  Value    Pointer to a caller-allocated UINT32 that receives the
                        double word, assembled little endian.

  @retval EFI_SUCCESS            The double word was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, Value is NULL, or Address does not fit in 24 bits.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_READ_SFDP)(
  IN   UINTN   SpiBase,
  IN   UINT32  Address,
  OUT  UINT32  *Value
  );

/**
  Reads status register 1 of the SPI flash.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[out]  Status   Pointer to a caller-allocated UINT8 that receives the
            register.

  @retval EFI_SUCCESS            The status register was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, or Status is NULL.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_READ_STATUS)(
  IN   UINTN  SpiBase,
  OUT  UINT8  *Status
  );

/**
  Arms the write enable latch with WREN (06h).

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The write enable latch was set.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_DEVICE_ERROR       The device did not report WEL after WREN.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_WRITE_ENABLE)(
  IN  UINTN  SpiBase
  );

/**
  Waits until the flash has finished its internal program/erase cycle.

  Polls the write-in-progress bit (RDSR1[0]) of the device, which is separate
  from the controller busy bit of SPIx00[31].

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.

  @retval EFI_SUCCESS            The device is idle.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_DEVICE_ERROR       The status register could not be read.
  @retval EFI_TIMEOUT            The device stayed busy for the whole timeout.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_WAIT_FLASH_READY)(
  IN  UINTN  SpiBase
  );

/**
  Reads any range of the SPI flash with READ (03h).

  The range is split into SPI_CHUNK_BYTES sized transfers because each
  command-mode transfer is limited by the FIFO depth.

  @param[in]   SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]   Address  Linear flash address of the first byte.
  @param[out]  Buffer   Pointer to the caller-allocated destination buffer.
  @param[in]   Length   Number of bytes to read.

  @retval EFI_SUCCESS            The range was read successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, Buffer is NULL while Length is
                 not 0, or Address plus Length wraps past 4 GiB.
  @retval EFI_TIMEOUT            A transfer timed out.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_READ_DATA)(
  IN   UINTN   SpiBase,
  IN   UINT32  Address,
  OUT  UINT8  *Buffer,
  IN   UINTN   Length
  );

/**
  Programs any range of the SPI flash with PAGEPROG (02h).

  The range is split on SPI_PAGE_BYTES boundaries, and every page is preceded by
  WREN and followed by a wait for the device to finish its program cycle.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Address  Linear flash address of the first byte.
  @param[in]  Buffer   Pointer to the caller-allocated source buffer.
  @param[in]  Length   Number of bytes to program.

  @retval EFI_SUCCESS            The range was programmed successfully.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0, Buffer is NULL while Length is
                 not 0, or Address plus Length wraps past 4 GiB.
  @retval EFI_DEVICE_ERROR       The write enable latch could not be armed.
  @retval EFI_TIMEOUT            A transfer or a program cycle timed out.

  @note The caller MUST erase the target range first; programming only clears
    bits.
  @warning Programming the live firmware image bricks the platform on the next
       boot.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_WRITE_DATA)(
  IN  UINTN   SpiBase,
  IN  UINT32  Address,
  IN  UINT8  *Buffer,
  IN  UINTN   Length
  );

/**
  Erases one 4 KiB sector of the SPI flash with SE (20h).

  The erase is preceded by WREN and followed by a wait for the device to finish
  its erase cycle.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Address  Linear flash address inside the target sector.

  @retval EFI_SUCCESS            The sector was erased.
  @retval EFI_INVALID_PARAMETER  SpiBase is 0.
  @retval EFI_DEVICE_ERROR       The write enable latch could not be armed.
  @retval EFI_TIMEOUT            The transfer or the erase cycle timed out.

  @warning Erasing the live firmware image bricks the platform on the next boot.
**/
typedef
EFI_STATUS
(EFIAPI *SPI_ERASE_SECTOR)(
  IN  UINTN   SpiBase,
  IN  UINT32  Address
  );

/**
  Reads an 8-bit register of the FCH SPI register block.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (0 - 0xFFF).

  @return The register value; 0xFF when the access was rejected.
**/
typedef
UINT8
(EFIAPI *SPI_READ_U8)(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset
  );

/**
  Reads a 16-bit register of the FCH SPI register block.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (0 - 0xFFF, 2-byte aligned).

  @return The register value; 0xFFFF when the access was rejected.
**/
typedef
UINT16
(EFIAPI *SPI_READ_U16)(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset
  );

/**
  Reads a 32-bit register of the FCH SPI register block.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (0 - 0xFFF, 4-byte aligned).

  @return The register value; 0xFFFFFFFF when the access was rejected.
**/
typedef
UINT32
(EFIAPI *SPI_READ_U32)(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset
  );

/**
  Writes an 8-bit register of the FCH SPI register block.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (0 - 0xFFF).
  @param[in]  Value    Value to write.
**/
typedef
VOID
(EFIAPI *SPI_WRITE_U8)(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset,
  IN  UINT8   Value
  );

/**
  Writes a 16-bit register of the FCH SPI register block.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (0 - 0xFFF, 2-byte aligned).
  @param[in]  Value    Value to write.
**/
typedef
VOID
(EFIAPI *SPI_WRITE_U16)(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset,
  IN  UINT16  Value
  );

/**
  Writes a 32-bit register of the FCH SPI register block.

  @param[in]  SpiBase  MMIO base address of the FCH SPI register block.
  @param[in]  Offset   Register offset from SpiBase (0 - 0xFFF, 4-byte aligned).
  @param[in]  Value    Value to write.
**/
typedef
VOID
(EFIAPI *SPI_WRITE_U32)(
  IN  UINTN   SpiBase,
  IN  UINT16  Offset,
  IN  UINT32  Value
  );

///
/// Custom SPI Protocol Structure.
///
struct _SPI_PROTOCOL {
  SPI_CLAIM_OWNERSHIP    ClaimOwnership;
  SPI_RELEASE_OWNERSHIP  ReleaseOwnership;

  SPI_IS_BUSY            IsBusy;
  SPI_WAIT_READY         WaitReady;

  SPI_TRANSFER           Transfer;

  SPI_READ_JEDEC_ID      ReadJedecId;
  SPI_READ_SFDP          ReadSfdp;
  SPI_READ_STATUS        ReadStatus;
  SPI_WRITE_ENABLE       WriteEnable;
  SPI_WAIT_FLASH_READY   WaitFlashReady;

  SPI_READ_DATA          ReadData;
  SPI_WRITE_DATA         WriteData;
  SPI_ERASE_SECTOR       EraseSector;

  //
  // Raw register access to the controller block, routed through the PCIe
  // ECAM protocol rather than through MmioRead*/MmioWrite* in the callers.
  //
  SPI_READ_U8            Read8;
  SPI_READ_U16           Read16;
  SPI_READ_U32           Read32;

  SPI_WRITE_U8           Write8;
  SPI_WRITE_U16          Write16;
  SPI_WRITE_U32          Write32;
};

#endif // SPI_PROTOCOL_H_

