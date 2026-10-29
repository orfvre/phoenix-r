/** @file
  AMD FCH SPI Controller Register Definitions.

  Bitfield structure layouts for every register in the FCH SPI
  controller block. Base address: 0xFEC10000 (below-4GB MMIO alias).

  @note  AMD PPR Family 19h Model 11h Rev B2, Volume 5, Section 10.3.8.1.
     Notation: SPIx<offset> means offset from SPI base (FEC1_0000h).
**/

#ifndef SPI_H_
#define SPI_H_

//
// EDK2 provides STATIC_ASSERT in MdePkg/Include/Base.h; the VisualUEFI header
// set does not, so fall back to a portable C form until it is defined upstream.
//
#ifndef STATIC_ASSERT
#define SPI_STATIC_ASSERT_CONCAT_(A, B)  A##B
#define SPI_STATIC_ASSERT_CONCAT(A, B)   SPI_STATIC_ASSERT_CONCAT_ (A, B)
#define STATIC_ASSERT(Expression, Message) \
  typedef char SPI_STATIC_ASSERT_CONCAT (SpiStaticAssertLine, __LINE__)[(Expression) ? 1 : -1]
#endif

/**
  Below-4GB MMIO alias for the FCH SPI controller register block.

  @note  PPR Vol 5, Table 185 "Address Space Mapping under APB BUS".
     The raw APB address (0xFDF95FFE00) requires Data Fabric MMIO
     routing and will freeze if accessed without it.
**/
#define SPI_MMIO_BASE  0xFEC10000UL

//
// Register offsets from SPI_MMIO_BASE
//

#define SPI_CNTRL0_OFFSET                    0x00  /**< SPIx00 — SPI Control 0. */
#define SPI_RESTRICTEDCMD_OFFSET             0x04  /**< SPIx04 — Restricted Command 0. */
#define SPI_RESTRICTEDCMD2_OFFSET            0x08  /**< SPIx08 — Restricted Command 1. */
#define SPI_CNTRL1_OFFSET                    0x0C  /**< SPIx0C — SPI Control 1. */
#define ESPI_CNTRL_OFFSET                    0x10  /**< SPIx10 — eSPI Control. */
#define SPI_CMDVALUE1_OFFSET                 0x14  /**< SPIx14 — SPI Command Value 1. */
#define SPI_CMDVALUE2_OFFSET                 0x18  /**< SPIx18 — SPI Command Value 2. */
#define ROMCP_CS_OFFSET                      0x1C  /**< SPIx1C — ROMCP Chip Select. */
#define ALT_SPI_CS_OFFSET                    0x1D  /**< SPIx1D — Alternate SPI Chip Select. */
#define SPI100ENABLE_OFFSET                  0x20  /**< SPIx20 — SPI100 Enable. */
#define SPI100_PRECYC0_OFFSET                0x24  /**< SPIx24 — Reserved. */
#define SPI100_PRECYC1_OFFSET                0x28  /**< SPIx28 — Reserved. */
#define SPI100_HOST_PREFETCH_CFG_OFFSET      0x2C  /**< SPIx2C — Host Prefetch Config. */
#define TPM_SPI_DI_TIMEOUT_OFFSET            0x2E  /**< SPIx2E — TPM SPI DI Timeout. */
#define ROM2_ADDR_OVERRIDE_OFFSET            0x30  /**< SPIx30 — ROM2 Address Override. */
#define SPI100_DUMMY_CYCLE_CFG_OFFSET        0x32  /**< SPIx32 — Dummy Cycle Config. */
#define SPI100_RX_TIMING_CFG0_OFFSET         0x34  /**< SPIx34 — RX Timing Config 0. */
#define SPI100_RX_TIMING_CFG1_OFFSET         0x38  /**< SPIx38 — RX Timing Config 1 (reserved). */
#define SPI100_RX_TIMING_CFG2_OFFSET         0x3C  /**< SPIx3C — RX Timing Config 2 (reserved). */
#define DDRCMDCODE_OFFSET                    0x40  /**< SPIx40 — DDR Command Code. */
#define QDRCMDCODE_OFFSET                    0x41  /**< SPIx41 — QDR Command Code. */
#define DPRCMDCODE_OFFSET                    0x42  /**< SPIx42 — DPR Command Code. */
#define QPRCMDCODE_OFFSET                    0x43  /**< SPIx43 — QPR Command Code. */
#define MODEBYTE_OFFSET                      0x44  /**< SPIx44 — Mode Byte. */
#define CMDCODE_OFFSET                       0x45  /**< SPIx45 — Command Code (SPI Opcode). */
#define CMDTRIGGER_OFFSET                    0x47  /**< SPIx47 — Command Trigger. */
#define TXBYTECOUNT_OFFSET                   0x48  /**< SPIx48 — TX Byte Count. */
#define RXBYTECOUNT_OFFSET                   0x4B  /**< SPIx4B — RX Byte Count. */
#define SPISTATUS_OFFSET                     0x4C  /**< SPIx4C — SPI Status. */
#define ADDR32CTRL0_OFFSET                   0x50  /**< SPIx50 — 32-bit Address Control 0. */
#define ADDR32CTRL1_OFFSET                   0x54  /**< SPIx54 — 32-bit Address Control 1. */
#define ADDR32CTRL2_OFFSET                   0x58  /**< SPIx58 — 32-bit Address Control 2. */
#define ADDR32CTRL3_OFFSET                   0x5C  /**< SPIx5C — 32-bit Address Control 3. */
#define BAR_64MB_ROM3_LOW_OFFSET             0x60  /**< SPIx60 — 64MB ROM3 BAR Low. */
#define BAR_64MB_ROM3_HIGH_OFFSET            0x64  /**< SPIx64 — 64MB ROM3 BAR High. */
#define SPI_RESTRICTEDCMD3_OFFSET            0x68  /**< SPIx68 — Restricted Command 2. */
#define SPISPEED_OFFSET                      0x6C  /**< SPIx6C — SPI Speed. */
#define HOST_INT_OFFSET                      0x70  /**< SPIx70 — Host Interrupt. */
#define LPC_SPI_INT_OFFSET                   0x74  /**< SPIx74 — LPC SPI Interrupt. */
#define SPI_FIFO_OFFSET                      0x80  /**< SPIx80 — Data FIFO (72 bytes, 0x80..0xC7). */
#define SPI_MISCCNTRL_OFFSET                 0xFC  /**< SPIxFC — SPI Miscellaneous Control. */

#pragma pack(1)

//
// SPIx00 - SPI_CNTRL0
//

/** SPIx00000000 — SPI Control 0. Reset: 0FC00000h. **/
typedef union {
  struct {
    UINT32  Reserved0 : 8;   /**< [7:0]   Reserved. */
    UINT32  DisIndexRetry : 1;   /**< [8]     Disable index retry. */
    UINT32  IndexCachelineStop : 1;   /**< [9]     Index cacheline stop. */
    UINT32  ProtCompareRange : 2;   /**< [11:10] Protection compare range. */
    UINT32  Reserved1 : 6;   /**< [17:12] Reserved. */
    UINT32  SpiReadMode0 : 1;   /**< [18]    SPI read mode bit 0. */
    UINT32  Reserved2 : 2;   /**< [20:19] Reserved. */
    UINT32  IllegalAccess : 1;   /**< [21]    Illegal access (RO). */
    UINT32  SpiAccessMacRomEn : 1;   /**< [22]    SPI access MAC ROM enable (clear-once). */
    UINT32  SpiHostAccessRomEn : 1;   /**< [23]    SPI host access ROM enable (clear-once). */
    UINT32  Reserved3 : 4;   /**< [27:24] Reserved. */
    UINT32  SpiClkGate : 1;   /**< [28]    SPI clock gate. */
    UINT32  SpiReadMode2_1 : 2;   /**< [30:29] SPI read mode bits [2:1]. */
    UINT32  SpiBusy : 1;   /**< [31]    SPI busy (RO). */
  } Bits;
  UINT32  Value;
} SPI_CNTRL0_REGISTER;

//
// SPIx04 - SPI_RESTRICTEDCMD
//

/** SPIx00000004 — Restricted Command Register 0. **/
typedef union {
  struct {
    UINT32  RestrictedCmd0 : 8;   /**< [7:0]   Restricted command 0. */
    UINT32  RestrictedCmd1 : 8;   /**< [15:8]  Restricted command 1. */
    UINT32  RestrictedCmd2 : 8;   /**< [23:16] Restricted command 2. */
    UINT32  RestrictedCmd3 : 8;   /**< [31:24] Restricted command 3. */
  } Bits;
  UINT32  Value;
} SPI_RESTRICTEDCMD_REGISTER;

//
// SPIx08 - SPI_RESTRICTEDCMD2
//

/** SPIx00000008 — Restricted Command Register 1. **/
typedef union {
  struct {
    UINT32  RestrictedCmd4 : 8;   /**< [7:0]   Restricted command 4. */
    UINT32  RestrictedCmd5 : 8;   /**< [15:8]  Restricted command 5. */
    UINT32  RestrictedCmd6 : 8;   /**< [23:16] Restricted command 6. */
    UINT32  RestrictedCmd7 : 8;   /**< [31:24] Restricted command 7. */
  } Bits;
  UINT32  Value;
} SPI_RESTRICTEDCMD2_REGISTER;

//
// SPIx0C - SPI_CNTRL1
//

/** SPIx0000000C — SPI Control 1. Reset: 02000000h. **/
typedef union {
  struct {
    UINT32  Reserved : 24;  /**< [23:0]  Reserved. */
    UINT32  ByteCommand : 8;   /**< [31:24] Byte program command opcode. */
  } Bits;
  UINT32  Value;
} SPI_CNTRL1_REGISTER;

//
// SPIx10 - ESPI_CNTRL
//

/** SPIx00000010 — eSPI Control. Reset: 0000h. **/
typedef union {
  struct {
    UINT16  DisEspiMasCtlRegWr : 1;  /**< [0]     Disable eSPI master control register write. */
    UINT16  RomAddrWrProt : 1;  /**< [1]     ROM address write protect. */
    UINT16  Reserved0 : 6;  /**< [7:2]   Reserved. */
    UINT16  Espi0UseSpi1 : 1;  /**< [8]     eSPI0 use SPI1. */
    UINT16  TpmPspUseSpi1 : 1;  /**< [9]     TPM PSP use SPI1. */
    UINT16  HostIndexUseSpi1 : 1;  /**< [10]    Host index use SPI1. */
    UINT16  PspIndexUseSpi1 : 1;  /**< [11]    PSP index use SPI1. */
    UINT16  HfpIndexUseSpi1 : 1;  /**< [12]    HFP index use SPI1. */
    UINT16  RomcpUseSpi1 : 1;  /**< [13]    ROMCP use SPI1. */
    UINT16  Reserved1 : 1;  /**< [14]    Reserved. */
    UINT16  LockAllUseSpi1 : 1;  /**< [15]    Lock all UseSpi1 bits. */
  } Bits;
  UINT16  Value;
} ESPI_CNTRL_REGISTER;

//
// SPIx14 - SPI_CMDVALUE1
//

/** SPIx00000014 — SPI Command Value 1. Reset: 0000E2B2h. **/
typedef union {
  struct {
    UINT32  DpwCmd : 8;   /**< [7:0]   Dual page write command (B2h). */
    UINT32  QpwCmd : 8;   /**< [15:8]  Quad page write command (E2h). */
    UINT32  Reserved : 16;  /**< [31:16] Reserved. */
  } Bits;
  UINT32  Value;
} SPI_CMDVALUE1_REGISTER;

//
// SPIx18 - SPI_CMDVALUE2
//

/** SPIx00000018 — SPI Command Value 2. **/
typedef union {
  struct {
    UINT32  ReadCmd : 8;   /**< [7:0]   Read command (03h). */
    UINT32  FRead : 8;   /**< [15:8]  Fast read command (0Bh). */
    UINT32  PageWr : 8;   /**< [23:16] Page write command (0Ah). */
    UINT32  ByteWr : 8;   /**< [31:24] Byte write command (02h). */
  } Bits;
  UINT32  Value;
} SPI_CMDVALUE2_REGISTER;

//
// SPIx1C - ROMCP_CS
//

/** SPIx0000001C — ROMCP Chip Select. Reset: 00h. **/
typedef union {
  struct {
    UINT8  RomCpCs : 2;   /**< [1:0] ROMCP chip select. */
    UINT8  LockRomcpCs : 1;   /**< [2]   Lock ROMCP chip select. */
    UINT8  Reserved : 5;   /**< [7:3] Reserved. */
  } Bits;
  UINT8  Value;
} ROMCP_CS_REGISTER;

//
// SPIx1D - ALT_SPI_CS
//

/** SPIx0000001D — Alternate SPI Chip Select. Reset: 08h. **/
typedef union {
  struct {
    UINT8  AltSpiCsEn : 2;   /**< [1:0] Alternate SPI chip select enable. */
    UINT8  Reserved0 : 1;   /**< [2]   Reserved. */
    UINT8  SpiProtectEn0 : 1;   /**< [3]   SPI protect enable 0. */
    UINT8  Reserved1 : 1;   /**< [4]   Reserved. */
    UINT8  SpiProtectLock : 1;   /**< [5]   SPI protect lock. */
    UINT8  LockSpiCs : 1;   /**< [6]   Lock SPI chip select. */
    UINT8  Reserved2 : 1;   /**< [7]   Reserved. */
  } Bits;
  UINT8  Value;
} ALT_SPI_CS_REGISTER;

//
// SPIx20 - SPI100ENABLE
//

/** SPIx00000020 — SPI100 Enable. Reset: 31330700h. **/
typedef union {
  struct {
    UINT32  UseSpi100 : 1;   /**< [0]     Use SPI 100MHz mode. */
    UINT32  Reserved0 : 7;   /**< [7:1]   Reserved. */
    UINT32  SpiCsDlySel : 4;   /**< [11:8]  SPI chip-select delay select. */
    UINT32  Reserved1 : 4;   /**< [15:12] Reserved. */
    UINT32  TpmSpeed : 4;   /**< [19:16] TPM SPI speed. */
    UINT32  AltSpeedNew : 4;   /**< [23:20] Alternate SPI speed. */
    UINT32  FastSpeedNew : 4;   /**< [27:24] Fast SPI speed. */
    UINT32  NormSpeed : 4;   /**< [31:28] Normal SPI speed. */
  } Bits;
  UINT32  Value;
} SPI100ENABLE_REGISTER;

//
// SPIx2C - SPI100_HOST_PREFETCH_CONFIG
//

/** SPIx0000002C — SPI100 Host Prefetch Config. Reset: 0000h. **/
typedef union {
  struct {
    UINT16  Reserved : 15;  /**< [14:0] Reserved. */
    UINT16  Rd4dwEnHost : 1;   /**< [15]   Read 4-dword enable (host). */
  } Bits;
  UINT16  Value;
} SPI100_HOST_PREFETCH_CONFIG_REGISTER;

//
// SPIx2E - TPM_SPI_DI_TIMEOUT
//

/** SPIx0000002E — TPM SPI DI Timeout. Reset: 8000h. **/
typedef union {
  struct {
    UINT16  Reserved : 9;   /**< [8:0]   Reserved. */
    UINT16  TpmDiToEnable : 1;   /**< [9]     TPM DI timeout enable. */
    UINT16  TpmDiToStatus : 1;   /**< [10]    TPM DI timeout status. */
    UINT16  TpmDiToCnt : 5;   /**< [15:11] TPM DI timeout count. */
  } Bits;
  UINT16  Value;
} TPM_SPI_DI_TIMEOUT_REGISTER;

//
// SPIx30 - ROM2_ADDR_OVERRIDE
//

/** SPIx00000030 — ROM2 Address Override. Reset: 14C0h. **/
typedef union {
  struct {
    UINT16  R2Val24 : 1;   /**< [0]    ROM2 address bit 24 value. */
    UINT16  R2Val25 : 1;   /**< [1]    ROM2 address bit 25 value. */
    UINT16  R2Msk24 : 1;   /**< [2]    ROM2 address bit 24 mask. */
    UINT16  R2Msk25 : 1;   /**< [3]    ROM2 address bit 25 mask. */
    UINT16  Reserved : 12;  /**< [15:4] Reserved. */
  } Bits;
  UINT16  Value;
} ROM2_ADDR_OVERRIDE_REGISTER;

//
// SPIx32 - SPI100_DUMMY_CYCLE_CONFIG
//

/** SPIx00000032 — SPI100 Dummy Cycle Config. Reset: 4608h. **/
typedef union {
  struct {
    UINT16  FastReadDummyCyc : 5;   /**< [4:0]   Fast read dummy cycles. */
    UINT16  Reserved0 : 3;   /**< [7:5]   Reserved. */
    UINT16  QprDummyCyc : 4;   /**< [11:8]  Quad page read dummy cycles. */
    UINT16  DprDummyCyc : 4;   /**< [15:12] Dual page read dummy cycles. */
  } Bits;
  UINT16  Value;
} SPI100_DUMMY_CYCLE_CONFIG_REGISTER;

//
// SPIx34 - SPI100_RX_TIMING_CONFIG0
//

/** SPIx00000034 — SPI100 RX Timing Config 0. Reset: 00000000h. **/
typedef union {
  struct {
    UINT32  Reserved0 : 4;   /**< [3:0]   Reserved. */
    UINT32  FlushHostPrefetch : 1;   /**< [4]     Flush host prefetch. */
    UINT32  Reserved1 : 1;   /**< [5]     Reserved. */
    UINT32  DisIndexFlushRBuf : 1;   /**< [6]     Disable index flush read buffer. */
    UINT32  Reserved2 : 25;  /**< [31:7]  Reserved. */
  } Bits;
  UINT32  Value;
} SPI100_RX_TIMING_CONFIG0_REGISTER;

//
// SPIx40 - DDRCMDCODE
//

/** SPIx00000040 — DDR Command Code. Reset: 3Bh. **/
typedef union {
  struct {
    UINT8  DdrCmd : 8;   /**< [7:0] DDR read command opcode (3Bh). */
  } Bits;
  UINT8  Value;
} DDRCMDCODE_REGISTER;

//
// SPIx41 - QDRCMDCODE
//

/** SPIx00000041 — QDR Command Code. Reset: 6Bh. **/
typedef union {
  struct {
    UINT8  QdrCmd : 8;   /**< [7:0] QDR read command opcode (6Bh). */
  } Bits;
  UINT8  Value;
} QDRCMDCODE_REGISTER;

//
// SPIx42 - DPRCMDCODE
//

/** SPIx00000042 — DPR Command Code. Reset: BBh. **/
typedef union {
  struct {
    UINT8  DprCmd : 8;   /**< [7:0] Dual page read command opcode (BBh). */
  } Bits;
  UINT8  Value;
} DPRCMDCODE_REGISTER;

//
// SPIx43 - QPRCMDCODE
//

/** SPIx00000043 — QPR Command Code. Reset: EBh. **/
typedef union {
  struct {
    UINT8  QprCmd : 8;   /**< [7:0] Quad page read command opcode (EBh). */
  } Bits;
  UINT8  Value;
} QPRCMDCODE_REGISTER;

//
// SPIx44 - MODEBYTE
//

/** SPIx00000044 — Mode Byte. Reset: 00h. **/
typedef union {
  struct {
    UINT8  ModeByte : 8;   /**< [7:0] Mode byte sent after address in fast reads. */
  } Bits;
  UINT8  Value;
} MODEBYTE_REGISTER;

//
// SPIx45 - CMDCODE
//

/** SPIx00000045 — Command Code (SPI Opcode). Reset: 00h. **/
typedef union {
  struct {
    UINT8  SpiOpcode : 8;   /**< [7:0] SPI opcode for command-mode transfer. */
  } Bits;
  UINT8  Value;
} CMDCODE_REGISTER;

//
// SPIx47 - CMDTRIGGER
//

/** SPIx00000047 — Command Trigger. Reset: 00h. **/
typedef union {
  struct {
    UINT8  Reserved : 7;   /**< [6:0] Reserved. */
    UINT8  Execute : 1;   /**< [7]   Write 1 to execute the SPI command. */
  } Bits;
  UINT8  Value;
} CMDTRIGGER_REGISTER;

//
// SPIx48 - TXBYTECOUNT
//

/** SPIx00000048 — TX Byte Count. Reset: 00h. **/
typedef union {
  struct {
    UINT8  TxByteCount : 8;   /**< [7:0] Number of bytes to transmit. */
  } Bits;
  UINT8  Value;
} TXBYTECOUNT_REGISTER;

//
// SPIx4B - RXBYTECOUNT
//

/** SPIx0000004B — RX Byte Count. Reset: 00h. **/
typedef union {
  struct {
    UINT8  RxByteCount : 8;   /**< [7:0] Number of bytes to receive. */
  } Bits;
  UINT8  Value;
} RXBYTECOUNT_REGISTER;

//
// SPIx4C - SPISTATUS
//

/** SPIx0000004C — SPI Status (read-only). Reset: 00000000h. **/
typedef union {
  struct {
    UINT32  DoneByteCount : 8;   /**< [7:0]   Bytes transferred so far (RO). */
    UINT32  FifoWritePtr : 7;    /**< [14:8]  FIFO write pointer (RO). */
    UINT32  Reserved0 : 1;       /**< [15]    Reserved. */
    UINT32  FifoReadPtr : 7;     /**< [22:16] FIFO read pointer (RO). */
    UINT32  Reserved1 : 8;       /**< [30:23] Reserved. */
    UINT32  SpiBusy : 1;         /**< [31]    SPI busy (RO). */
  } Bits;
  UINT32  Value;
} SPISTATUS_REGISTER;

//
// SPIx50 - ADDR32CTRL0
//

/** SPIx00000050 — 32-bit Address Control 0. **/
typedef union {
  struct {
    UINT32  SpiRomAddr32 : 1;   /**< [0]     Enable 32-bit (4-byte) addressing. */
    UINT32  Reserved : 7;   /**< [7:1]   Reserved. */
    UINT32  ByteWrCmdAddr32 : 8;   /**< [15:8]  Byte-write cmd for 4-byte addr (12h). */
    UINT32  ReadCmdAddr32 : 8;   /**< [23:16] Read cmd for 4-byte addr (13h). */
    UINT32  FReadCmdAddr32 : 8;   /**< [31:24] Fast-read cmd for 4-byte addr (0Ch). */
  } Bits;
  UINT32  Value;
} ADDR32CTRL0_REGISTER;

//
// SPIx54 - ADDR32CTRL1
//

/** SPIx00000054 — 32-bit Address Control 1. **/
typedef union {
  struct {
    UINT32  DdrCmdAddr32 : 8;   /**< [7:0]   DDR cmd for 4-byte addr. */
    UINT32  QdrCmdAddr32 : 8;   /**< [15:8]  QDR cmd for 4-byte addr. */
    UINT32  DprCmdAddr32 : 8;   /**< [23:16] DPR cmd for 4-byte addr. */
    UINT32  QprCmdAddr32 : 8;   /**< [31:24] QPR cmd for 4-byte addr. */
  } Bits;
  UINT32  Value;
} ADDR32CTRL1_REGISTER;

//
// SPIx58 - ADDR32CTRL2
//

/** SPIx00000058 — 32-bit Address Control 2. **/
typedef union {
  struct {
    UINT32  FastReadDummyCycAddr32 : 5;   /**< [4:0]   Fast-read dummy cycles (4-byte mode). */
    UINT32  Reserved0 : 3;   /**< [7:5]   Reserved. */
    UINT32  QprDummyCycAddr32 : 4;   /**< [11:8]  QPR dummy cycles (4-byte mode). */
    UINT32  DprDummyCycAddr32 : 4;   /**< [15:12] DPR dummy cycles (4-byte mode). */
    UINT32  Reserved1 : 16;  /**< [31:16] Reserved. */
  } Bits;
  UINT32  Value;
} ADDR32CTRL2_REGISTER;

//
// SPIx5C - ADDR32CTRL3
//

/** SPIx0000005C — 32-bit Address Control 3. **/
typedef union {
  struct {
    UINT32  SpiRomPage31_24 : 8;   /**< [7:0]   Upper page bits [31:24] for ROM access. */
    UINT32  Reserved : 24;  /**< [31:8]  Reserved. */
  } Bits;
  UINT32  Value;
} ADDR32CTRL3_REGISTER;

//
// SPIx60 - BAR_64MB_ROM3_LOW
//

/** SPIx00000060 — 64MB ROM3 BAR Low. **/
typedef union {
  struct {
    UINT32  Reserved : 26;  /**< [25:0]  Reserved. */
    UINT32  Bar64mb31_26 : 6;   /**< [31:26] BAR bits [31:26]. */
  } Bits;
  UINT32  Value;
} BAR_64MB_ROM3_LOW_REGISTER;

//
// SPIx64 - BAR_64MB_ROM3_HIGH
//

/** SPIx00000064 — 64MB ROM3 BAR High. Reset: 000000FDh. **/
typedef union {
  struct {
    UINT32  Bar64mb63_32 : 32;  /**< [31:0] BAR bits [63:32]. */
  } Bits;
  UINT32  Value;
} BAR_64MB_ROM3_HIGH_REGISTER;

//
// SPIx68 - SPI_RESTRICTEDCMD3
//

/** SPIx00000068 — Restricted Command Register 2. **/
typedef union {
  struct {
    UINT32  RestrictedCmd8 : 8;   /**< [7:0]   Restricted command 8. */
    UINT32  RestrictedCmd9 : 8;   /**< [15:8]  Restricted command 9. */
    UINT32  RestrictedCmd10 : 8;   /**< [23:16] Restricted command 10. */
    UINT32  RestrictedCmd11 : 8;   /**< [31:24] Restricted command 11. */
  } Bits;
  UINT32  Value;
} SPI_RESTRICTEDCMD3_REGISTER;

//
// SPIx6C - SPISPEED
//

/** SPIx0000006C — SPI Speed. Reset: 3204h. **/
typedef union {
  struct {
    UINT16  SpiSpd6 : 6;   /**< [5:0]   SPI speed 6. */
    UINT16  Reserved0 : 2;   /**< [7:6]   Reserved. */
    UINT16  SpiSpd7 : 6;   /**< [13:8]  SPI speed 7. */
    UINT16  Reserved1 : 2;   /**< [15:14] Reserved. */
  } Bits;
  UINT16  Value;
} SPISPEED_REGISTER;

//
// SPIx70 - HOST_INT
//

/** SPIx00000070 — Host Interrupt. Reset: 00000000h. **/
typedef union {
  struct {
    UINT32  HostIntStatus : 8;   /**< [7:0]   Host interrupt status (W1C).
                       [0] Host index transfer done.
                       [1] DMA ROMCP done.
                       [7:2] Reserved. */
    UINT32  HostIntMask : 8;   /**< [15:8]  Host interrupt mask.
                       Write 1 to mask the corresponding source. */
    UINT32  HostIntEnable : 8;   /**< [23:16] Host interrupt enable.
                       Write 1 to enable the corresponding source. */
    UINT32  Reserved : 8;   /**< [31:24] Reserved. */
  } Bits;
  UINT32  Value;
} HOST_INT_REGISTER;

//
// SPIx74 - LPC_SPI_INT
//

/** SPIx00000074 — LPC SPI Interrupt (read-only). Reset: 00000000h. **/
typedef union {
  struct {
    UINT32  HostIndexInt : 1;   /**< [0]    Host index interrupt active (RO). */
    UINT32  RomcpInt : 1;   /**< [1]    ROMCP interrupt active (RO). */
    UINT32  HfpIndexInt : 1;   /**< [2]    HFP index interrupt active (RO). */
    UINT32  PspIndexInt : 1;   /**< [3]    PSP index interrupt active (RO). */
    UINT32  Reserved0 : 4;   /**< [7:4]  Reserved. */
    UINT32  HidIntStatus : 8;   /**< [15:8] HID interrupt status (RO).
                      Only valid when HIDx150[15]=1.
                      [8]  HID index transfer done.
                      [9]  HID DMA copy done.
                      [10] HID basic write done.
                      [11] HID basic read done.
                      [12] HID acc write done.
                      [13] HID acc read done.
                      [14] HID DMA acc read done.
                      [15] HID device IRQ input. */
    UINT32  Reserved1 : 16;  /**< [31:16] Reserved. */
  } Bits;
  UINT32  Value;
} LPC_SPI_INT_REGISTER;

//
// SPIx80..SPIxC7 - FIFO (72 bytes)
//

/**
  SPIx00000080..SPIx000000C7 — Data FIFO (72 bytes).

  Used in command mode to send/receive data. Accessible as
  byte, word, or dword. Modeled as a raw byte array.
**/
typedef struct {
  UINT8  Data[72];   /**< FIFO bytes 0..71. */
} SPI_FIFO;

//
// SPIxFC - SPI_MISCCNTRL
//

/** SPIx000000FC — SPI Miscellaneous Control. Reset: 0000h. **/
typedef union {
  struct {
    UINT16  Reserved0 : 5;   /**< [4:0]   Reserved. */
    UINT16  HbiosSpiMutex : 1;   /**< [5]     Host-BIOS SPI mutex ownership.
                        Cannot set if PSP or HFP owns the mutex. */
    UINT16  RomcpNewScheme : 1;   /**< [6]     ROMCP new scheme.
                        Set 1 to allow direct/index reads during DMA copy. */
    UINT16  EnMapSpiBxToCfg : 1;   /**< [7]     Map SPIxB0..xBF to LPC PCI CFG xB0..xBF.
                        Enables processor SPI DMA copy via SPI register space. */
    UINT16  Reserved1 : 1;   /**< [8]     Reserved. */
    UINT16  HfpOwnSpi : 1;   /**< [9]     HFP owns SPI (RO). Value of HFPxFC[5]. */
    UINT16  PspOwnSpi : 1;   /**< [10]    PSP owns SPI (RO). Value of PSPxFC[5]. */
    UINT16  Reserved2 : 5;   /**< [15:11] Reserved. */
  } Bits;
  UINT16  Value;
} SPI_MISCCNTRL_REGISTER;

//
// Full SPI register block overlay
//

/**
  Complete SPI controller register block mapped at SPI_MMIO_BASE.

  @note  Cast a pointer to this struct over the MMIO base to access
     any register: ((SPI_REGISTER_BLOCK *)(UINTN)SPI_MMIO_BASE)->Cntrl0.Value
**/
typedef struct {
  SPI_CNTRL0_REGISTER                Cntrl0;              /**< 0x00 */
  SPI_RESTRICTEDCMD_REGISTER         RestrictedCmd;        /**< 0x04 */
  SPI_RESTRICTEDCMD2_REGISTER        RestrictedCmd2;       /**< 0x08 */
  SPI_CNTRL1_REGISTER                Cntrl1;              /**< 0x0C */
  ESPI_CNTRL_REGISTER                EspiCntrl;           /**< 0x10 */
  UINT16                             Reserved12;          /**< 0x12 */
  SPI_CMDVALUE1_REGISTER             CmdValue1;           /**< 0x14 */
  SPI_CMDVALUE2_REGISTER             CmdValue2;           /**< 0x18 */
  ROMCP_CS_REGISTER                  RomcpCs;             /**< 0x1C */
  ALT_SPI_CS_REGISTER                AltSpiCs;            /**< 0x1D */
  UINT16                             Reserved1E;          /**< 0x1E */
  SPI100ENABLE_REGISTER              Spi100Enable;        /**< 0x20 */
  UINT32                             Spi100PreCyc0;       /**< 0x24  Reserved. */
  UINT32                             Spi100PreCyc1;       /**< 0x28  Reserved. */
  SPI100_HOST_PREFETCH_CONFIG_REGISTER  HostPrefetchCfg;  /**< 0x2C */
  TPM_SPI_DI_TIMEOUT_REGISTER        TpmSpiDiTimeout;     /**< 0x2E */
  ROM2_ADDR_OVERRIDE_REGISTER        Rom2AddrOverride;     /**< 0x30 */
  SPI100_DUMMY_CYCLE_CONFIG_REGISTER DummyCycleCfg;        /**< 0x32 */
  SPI100_RX_TIMING_CONFIG0_REGISTER  RxTimingCfg0;        /**< 0x34 */
  UINT32                             RxTimingCfg1;        /**< 0x38  Reserved. */
  UINT16                             RxTimingCfg2;        /**< 0x3C  Reserved. */
  UINT16                             Reserved3E;          /**< 0x3E */
  DDRCMDCODE_REGISTER                DdrCmdCode;          /**< 0x40 */
  QDRCMDCODE_REGISTER                QdrCmdCode;          /**< 0x41 */
  DPRCMDCODE_REGISTER                DprCmdCode;          /**< 0x42 */
  QPRCMDCODE_REGISTER                QprCmdCode;          /**< 0x43 */
  MODEBYTE_REGISTER                  ModeByte;            /**< 0x44 */
  CMDCODE_REGISTER                   CmdCode;             /**< 0x45 */
  UINT8                              Reserved46;          /**< 0x46 */
  CMDTRIGGER_REGISTER                CmdTrigger;          /**< 0x47 */
  TXBYTECOUNT_REGISTER               TxByteCount;         /**< 0x48 */
  UINT16                             Reserved49;          /**< 0x49 */
  RXBYTECOUNT_REGISTER               RxByteCount;         /**< 0x4B */
  SPISTATUS_REGISTER                 SpiStatus;           /**< 0x4C */
  ADDR32CTRL0_REGISTER               Addr32Ctrl0;         /**< 0x50 */
  ADDR32CTRL1_REGISTER               Addr32Ctrl1;         /**< 0x54 */
  ADDR32CTRL2_REGISTER               Addr32Ctrl2;         /**< 0x58 */
  ADDR32CTRL3_REGISTER               Addr32Ctrl3;         /**< 0x5C */
  BAR_64MB_ROM3_LOW_REGISTER         Bar64MbRom3Low;      /**< 0x60 */
  BAR_64MB_ROM3_HIGH_REGISTER        Bar64MbRom3High;     /**< 0x64 */
  SPI_RESTRICTEDCMD3_REGISTER        RestrictedCmd3;      /**< 0x68 */
  SPISPEED_REGISTER                  SpiSpeed;            /**< 0x6C */
  UINT16                             Reserved6E;          /**< 0x6E */
  HOST_INT_REGISTER                  HostInt;             /**< 0x70 */
  LPC_SPI_INT_REGISTER               LpcSpiInt;           /**< 0x74 */
  UINT64                             Reserved78;          /**< 0x78 */
  SPI_FIFO                           Fifo;                /**< 0x80..0xC7 */
  UINT8                              ReservedC8[0xFC - 0xC8]; /**< 0xC8..0xFB */
  SPI_MISCCNTRL_REGISTER             MiscCntrl;           /**< 0xFC */
  UINT16                             ReservedFE;          /**< 0xFE */
} SPI_REGISTER_BLOCK;

#pragma pack()

//
// Compile-time check: the register block must be exactly 256 bytes (0x100).
//
STATIC_ASSERT(
  sizeof(SPI_REGISTER_BLOCK) == 0x100,
  "SPI_REGISTER_BLOCK size mismatch — must be 256 bytes"
);

#endif // SPI_H_