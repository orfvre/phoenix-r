/** @file
  AMD FCH eSPI Controller Register Definitions.

  Bitfield structure layouts for every register in the FCH eSPI
  controller block.  Base addresses:
    Link 0: 0xFEC20000 (HOST LEGACY alias)  / SMN 0x02DC5000
    Link 1: 0xFEC30000 (HOST LEGACY alias)  / SMN 0x02DCA000

  @note  AMD PPR Family 19h Model 11h Rev B2, Volume 5, Section 10.3.8.2.
         Notation: ESPIx<offset> means offset from eSPI base.
**/

#ifndef ESPI_H_
#define ESPI_H_

/**
  Below-4GB MMIO alias for the FCH eSPI controller register blocks.

  @note  eSPI base addresses are derived from SPI BASE ADDR register
         (LPC Bridge PCI Config Bus 0, Dev 14h, Func 3, offset A0h):
           Link 0 = SPI_BASE + 0x10000
           Link 1 = SPI_BASE + 0x20000
**/
#define ESPI_LINK0_MMIO_BASE  0xFEC20000UL
#define ESPI_LINK1_MMIO_BASE  0xFEC30000UL

//
// Register offsets from ESPI_LINKn_MMIO_BASE
//

#define ESPI_DN_TXHDR_0TH_OFFSET              0x00  /**< ESPIx00 — Down TX Header 0. */
#define ESPI_DN_TXHDR_1_OFFSET                0x04  /**< ESPIx04 — Down TX Header 1. */
#define ESPI_DN_TXHDR_2_OFFSET                0x08  /**< ESPIx08 — Down TX Header 2. */
#define ESPI_DN_TXDATA_PORT_OFFSET            0x0C  /**< ESPIx0C — Down TX Data Port. */
#define ESPI_UP_RXHDR_0_OFFSET                0x10  /**< ESPIx10 — Up RX Header 0. */
#define ESPI_UP_RXHDR_1_OFFSET                0x14  /**< ESPIx14 — Up RX Header 1. */
#define ESPI_UP_RXDATA_PORT_OFFSET            0x18  /**< ESPIx18 — Up RX Data Port. */
#define ESPI_RESERVED_REG0_OFFSET             0x1C  /**< ESPIx1C — Reserved Register 0. */
#define ESPI_RESERVED_REG1_OFFSET             0x20  /**< ESPIx20 — Reserved Register 1. */
#define ESPI_RESERVED_REG2_OFFSET             0x24  /**< ESPIx24 — Reserved Register 2. */
#define ESPI_MASTER_CAP_OFFSET                0x2C  /**< ESPIx2C — Master Capability. */
#define ESPI_MISC_CONTROL_1_OFFSET            0x3C  /**< ESPIx3C — Misc Control 1. */
#define ESPI_SLAVE0_IO_BASE_REG0_OFFSET       0x44  /**< ESPIx44 — Slave0 IO Base Reg 0. */
#define ESPI_SLAVE0_IO_BASE_REG1_OFFSET       0x48  /**< ESPIx48 — Slave0 IO Base Reg 1. */
#define ESPI_SLAVE0_IO_SIZE_OFFSET            0x4C  /**< ESPIx4C — Slave0 IO Size. */
#define ESPI_SLAVE0_MMIO_BASE_REG0_OFFSET     0x50  /**< ESPIx50 — Slave0 MMIO Base Reg 0. */
#define ESPI_SLAVE0_MMIO_BASE_REG1_OFFSET     0x54  /**< ESPIx54 — Slave0 MMIO Base Reg 1. */
#define ESPI_SLAVE0_MMIO_BASE_REG2_OFFSET     0x58  /**< ESPIx58 — Slave0 MMIO Base Reg 2. */
#define ESPI_SLAVE0_MMIO_BASE_REG3_OFFSET     0x5C  /**< ESPIx5C — Slave0 MMIO Base Reg 3. */
#define ESPI_SLAVE0_MMIO_SIZE_REG0_OFFSET     0x60  /**< ESPIx60 — Slave0 MMIO Size Reg 0. */
#define ESPI_SLAVE0_MMIO_SIZE_REG1_OFFSET     0x64  /**< ESPIx64 — Slave0 MMIO Size Reg 1. */
#define ESPI_SLAVE0_CONFIG_OFFSET             0x68  /**< ESPIx68 — Slave0 Config. */
#define ESPI_SLAVE0_INT_EN_OFFSET             0x6C  /**< ESPIx6C — Slave0 Interrupt Enable. */
#define ESPI_SLAVE0_INT_STS_OFFSET            0x70  /**< ESPIx70 — Slave0 Interrupt Status. */
#define ESPI_SLAVE0_RXMSG_HDR0_OFFSET         0x74  /**< ESPIx74 — Slave0 RX Message Header 0. */
#define ESPI_SLAVE0_RXMSG_HDR1_OFFSET         0x78  /**< ESPIx78 — Slave0 RX Message Header 1. */
#define ESPI_SLAVE0_RXMSG_DATA_PORT_OFFSET    0x7C  /**< ESPIx7C — Slave0 RX Message Data Port. */
#define ESPI_SLAVE0_IO_BASE_REG2_OFFSET       0x80  /**< ESPIx80 — Slave0 IO Base Reg 2. */
#define ESPI_SLAVE0_IO_BASE_REG3_OFFSET       0x84  /**< ESPIx84 — Slave0 IO Base Reg 3. */
#define ESPI_SLAVE0_IO_SIZE1_OFFSET           0x88  /**< ESPIx88 — Slave0 IO Size 1. */
#define ESPI_SLAVE0_IO_BASE_REG4_OFFSET       0x8C  /**< ESPIx8C — Slave0 IO Base Reg 4. */
#define ESPI_SLAVE0_IO_BASE_REG5_OFFSET       0x90  /**< ESPIx90 — Slave0 IO Base Reg 5. */
#define ESPI_SLAVE0_IO_SIZE2_OFFSET           0x94  /**< ESPIx94 — Slave0 IO Size 2. */
#define ESPI_RESERVED_RXMSG_REG6_OFFSET       0x98  /**< ESPIx98 — Reserved / VW IRQ Polarity. */
#define ESPI_SLAVE0_RXVW_OFFSET               0x9C  /**< ESPIx9C — Slave0 RX Virtual Wire. */
#define ESPI_SLAVE0_RXVW_DATA_OFFSET          0xA0  /**< ESPIxA0 — Slave0 RX VW Data. */
#define ESPI_SLAVE0_RXVW_INDEX_OFFSET         0xA4  /**< ESPIxA4 — Slave0 RX VW Index Selection. */
#define ESPI_SLAVE0_RXVW_MISC_CNTL_OFFSET     0xA8  /**< ESPIxA8 — Slave0 RX VW Misc Control. */
#define ESPI_SLAVE0_RXVW_POLARITY_OFFSET      0xAC  /**< ESPIxAC — Slave0 RX VW Polarity. */
#define ESPI_SLAVE0_IO_BASE_REG6_OFFSET       0xB0  /**< ESPIxB0 — Slave0 IO Base Reg 6. */
#define ESPI_SLAVE0_IO_BASE_REG7_OFFSET       0xB4  /**< ESPIxB4 — Slave0 IO Base Reg 7. */
#define ESPI_SLAVE0_IO_SIZE3_OFFSET           0xB8  /**< ESPIxB8 — Slave0 IO Size 3. */
#define ESPI_SLAVE0_MMIO_BASE_REG4_OFFSET     0xBC  /**< ESPIxBC — Slave0 MMIO Base Reg 4. */
#define ESPI_SLAVE0_MMIO_SIZE_REG2_OFFSET     0xC0  /**< ESPIxC0 — Slave0 MMIO Size Reg 2. */
#define ESPI_MMIO_CPUTEMP_OFFSET              0xC4  /**< ESPIxC4 — MMIO CPU Temp Address. */
#define ESPI_MMIO_RTCTIME_OFFSET              0xC8  /**< ESPIxC8 — MMIO RTC Time Address. */

#pragma pack(1)

//
// ESPIx00 - DN_TXHDR_0th  (FCH::ITF::ESPI::DN_TXHDR_0th)
//

/** ESPIx00000000 — Down TX Header 0. Reset: 0020_0000h. **/
typedef union {
    struct {
        UINT32  DncmdType : 3;   /**< [2:0]   TX command type. */
        UINT32  DncmdStatus : 1;   /**< [3]     Command status (Read, Write-0-only). */
        UINT32  Reserved0 : 4;   /**< [7:4]   Reserved. */
        UINT32  DncmdHdata0 : 8;   /**< [15:8]  Header data byte 0. */
        UINT32  DncmdHdata1 : 8;   /**< [23:16] Header data byte 1. */
        UINT32  DncmdHdata2 : 8;   /**< [31:24] Header data byte 2. */
    } Bits;
    UINT32  Value;
} ESPI_DN_TXHDR_0TH_REGISTER;

//
// ESPIx04 - DN_TXHDR_1  (FCH::ITF::ESPI::DN_TXHDR_1)
//

/** ESPIx00000004 — Down TX Header 1. Reset: 0000_0001h. **/
typedef union {
    struct {
        UINT32  DncmdHdata3 : 8;   /**< [7:0]   Header data byte 3. */
        UINT32  DncmdHdata4 : 8;   /**< [15:8]  Header data byte 4. */
        UINT32  DncmdHdata5 : 8;   /**< [23:16] Header data byte 5. */
        UINT32  DncmdHdata6 : 8;   /**< [31:24] Header data byte 6. */
    } Bits;
    UINT32  Value;
} ESPI_DN_TXHDR_1_REGISTER;

//
// ESPIx08 - DN_TXHDR_2  (FCH::ITF::ESPI::DN_TXHDR_2)
//

/** ESPIx00000008 — Down TX Header 2. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  DncmdHdata7 : 8;   /**< [7:0]   Header data byte 7. */
        UINT32  Reserved0 : 24;  /**< [31:8]  Reserved. */
    } Bits;
    UINT32  Value;
} ESPI_DN_TXHDR_2_REGISTER;

//
// ESPIx0C - DN_TXDATA_PORT  (FCH::ITF::ESPI::DN_TXDATA_PORT)
//

/** ESPIx0000000C — Down TX Data Port. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  DnTxdataB0 : 8;   /**< [7:0]   TX data byte 0. */
        UINT32  DnTxdataB1 : 8;   /**< [15:8]  TX data byte 1. */
        UINT32  DnTxdataB2 : 8;   /**< [23:16] TX data byte 2. */
        UINT32  DnTxdataB3 : 8;   /**< [31:24] TX data byte 3. */
    } Bits;
    UINT32  Value;
} ESPI_DN_TXDATA_PORT_REGISTER;

//
// ESPIx10 - UP_RXHDR_0  (FCH::ITF::ESPI::UP_RXHDR_0)
//

/** ESPIx00000010 — Up RX Header 0. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  UpcmdType : 3;   /**< [2:0]   Upstream command type (RO). */
        UINT32  UpcmdStatus : 1;   /**< [3]     Valid bit status (Read, Write-1-to-clear). */
        UINT32  Reserved0 : 1;   /**< [4]     Reserved. */
        UINT32  SlaveSel : 2;   /**< [6:5]   Slave N received selected (RO). */
        UINT32  Reserved1 : 1;   /**< [7]     Reserved. */
        UINT32  UpcmdHdata0 : 8;   /**< [15:8]  Cycle Type (RO). */
        UINT32  UpcmdHdata1 : 8;   /**< [23:16] RX_TAG_LEN (RO). */
        UINT32  UpcmdHdata2 : 8;   /**< [31:24] RX_LOW_LEN (RO). */
    } Bits;
    UINT32  Value;
} ESPI_UP_RXHDR_0_REGISTER;

//
// ESPIx14 - UP_RXHDR_1  (FCH::ITF::ESPI::UP_RXHDR_1)
//

/** ESPIx00000014 — Up RX Header 1. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  UpcmdHdata3 : 8;   /**< [7:0]   SMBus Slave Address / Flash Address (RO). */
        UINT32  UpcmdHdata4 : 8;   /**< [15:8]  SMBus Command Opcode / Flash Address (RO). */
        UINT32  UpcmdHdata5 : 8;   /**< [23:16] SMBus Byte Count / Flash Address (RO). */
        UINT32  UpcmdHdata6 : 8;   /**< [31:24] Flash Address (RO). */
    } Bits;
    UINT32  Value;
} ESPI_UP_RXHDR_1_REGISTER;

//
// ESPIx18 - UP_RXDATA_PORT  (FCH::ITF::ESPI::UP_RXDATA_PORT)
//

/** ESPIx00000018 — Up RX Data Port. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  UpRxdata : 32;  /**< [31:0] Upstream received data (RO). */
    } Bits;
    UINT32  Value;
} ESPI_UP_RXDATA_PORT_REGISTER;

//
// ESPIx2C - MASTER_CAP  (FCH::ITF::ESPI::MASTER_CAP)
//

/** ESPIx0000002C — Master Capability. Reset: E649_E91Fh (Fixed/Read-only). **/
typedef union {
    struct {
        UINT32  FlashSupport : 1;   /**< [0]     Flash channel support (Fixed: 1). */
        UINT32  OobSupport : 1;   /**< [1]     OOB channel support (Fixed: 1). */
        UINT32  VwSupport : 1;   /**< [2]     Virtual Wire channel support (Fixed: 1). */
        UINT32  PrSupport : 1;   /**< [3]     Peripheral channel support (Fixed: 1). */
        UINT32  EspiVersion : 3;   /**< [6:4]   eSPI version (Fixed: 1h). */
        UINT32  FlashMaxSize : 3;   /**< [9:7]   Flash max payload size (Fixed: 2h). */
        UINT32  OobMaxSize : 3;   /**< [12:10] OOB max payload size (Fixed: 2h). */
        UINT32  VwMaxSize : 6;   /**< [18:13] VW max count support (Fixed: 0Fh). */
        UINT32  PrMaxSize : 3;   /**< [21:19] Peripheral max payload size (Fixed: 1h). */
        UINT32  SlaveNum : 3;   /**< [24:22] Number of slaves supported (Fixed: 1h). */
        UINT32  ClkFreqSupport : 3;   /**< [27:25] Clock frequency support (Fixed: 3h). */
        UINT32  IoModeSupport : 2;   /**< [29:28] IO mode support (Fixed: 2h). */
        UINT32  AlertModeSupport : 1;   /**< [30]    Alert mode support (Fixed: 1). */
        UINT32  CrcCheckSupport : 1;   /**< [31]    CRC checking support (Fixed: 1). */
    } Bits;
    UINT32  Value;
} ESPI_MASTER_CAP_REGISTER;

//
// ESPIx3C - MISC_CONTROL_1  (FCH::ITF::ESPI::MISC_CONTROL_1)
//

/** ESPIx0000003C — Misc Control 1. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  MiscControl1Reserved : 32;  /**< [31:0] Reserved. */
    } Bits;
    UINT32  Value;
} ESPI_MISC_CONTROL_1_REGISTER;

//
// ESPIx44 - SLAVE0_IO_BASE_REG0  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG0)
//

/** ESPIx00000044 — Slave0 IO Base Register 0. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range0 : 16;  /**< [15:0]  IO decode base address for Range 0. */
        UINT32  Range1 : 16;  /**< [31:16] IO decode base address for Range 1. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG0_REGISTER;

//
// ESPIx48 - SLAVE0_IO_BASE_REG1  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG1)
//

/** ESPIx00000048 — Slave0 IO Base Register 1. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range2 : 16;  /**< [15:0]  IO decode base address for Range 2. */
        UINT32  Range3 : 16;  /**< [31:16] IO decode base address for Range 3. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG1_REGISTER;

//
// ESPIx4C - SLAVE0_IO_SIZE  (FCH::ITF::ESPI::SLAVE0_IO_SIZE)
//

/** ESPIx0000004C — Slave0 IO Size. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range0 : 8;   /**< [7:0]   Programmable IO Range 0 size. */
        UINT32  Range1 : 8;   /**< [15:8]  Programmable IO Range 1 size. */
        UINT32  Range2 : 8;   /**< [23:16] Programmable IO Range 2 size. */
        UINT32  Range3 : 8;   /**< [31:24] Programmable IO Range 3 size. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_SIZE_REGISTER;

//
// ESPIx50 - SLAVE0_MMIO_BASE_REG0  (FCH::ITF::ESPI::SLAVE0_MMIO_BASE_REG0)
//

/** ESPIx00000050 — Slave0 MMIO Base Register 0. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range0 : 32;  /**< [31:0] MMIO decode base address for Range 0. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_BASE_REG0_REGISTER;

//
// ESPIx54 - SLAVE0_MMIO_BASE_REG1  (FCH::ITF::ESPI::SLAVE0_MMIO_BASE_REG1)
//

/** ESPIx00000054 — Slave0 MMIO Base Register 1. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range1 : 32;  /**< [31:0] MMIO decode base address for Range 1. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_BASE_REG1_REGISTER;

//
// ESPIx58 - SLAVE0_MMIO_BASE_REG2  (FCH::ITF::ESPI::SLAVE0_MMIO_BASE_REG2)
//

/** ESPIx00000058 — Slave0 MMIO Base Register 2. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range2 : 32;  /**< [31:0] MMIO decode base address for Range 2. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_BASE_REG2_REGISTER;

//
// ESPIx5C - SLAVE0_MMIO_BASE_REG3  (FCH::ITF::ESPI::SLAVE0_MMIO_BASE_REG3)
//

/** ESPIx0000005C — Slave0 MMIO Base Register 3. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range3 : 32;  /**< [31:0] MMIO decode base address for Range 3. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_BASE_REG3_REGISTER;

//
// ESPIx60 - SLAVE0_MMIO_SIZE_REG0  (FCH::ITF::ESPI::SLAVE0_MMIO_SIZE_REG0)
//

/** ESPIx00000060 — Slave0 MMIO Size Register 0. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range0 : 16;  /**< [15:0]  Programmable MMIO Range 0 size. */
        UINT32  Range1 : 16;  /**< [31:16] Programmable MMIO Range 1 size. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_SIZE_REG0_REGISTER;

//
// ESPIx64 - SLAVE0_MMIO_SIZE_REG1  (FCH::ITF::ESPI::SLAVE0_MMIO_SIZE_REG1)
//

/** ESPIx00000064 — Slave0 MMIO Size Register 1. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range2 : 16;  /**< [15:0]  Programmable MMIO Range 2 size. */
        UINT32  Range3 : 16;  /**< [31:16] Programmable MMIO Range 3 size. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_SIZE_REG1_REGISTER;

//
// ESPIx68 - SLAVE0_CONFIG  (FCH::ITF::ESPI::SLAVE0_CONFIG)
//

/** ESPIx00000068 — Slave0 Configuration. Reset: 0000_000Ch. **/
typedef union {
    struct {
        UINT32  FlashEn : 1;   /**< [0]     Flash channel enable. */
        UINT32  OobEn : 1;   /**< [1]     OOB channel enable. */
        UINT32  VwEn : 1;   /**< [2]     Virtual Wire channel enable. */
        UINT32  PrEn : 1;   /**< [3]     Peripheral channel enable. */
        UINT32  Reserved0 : 21;  /**< [24:4]  Reserved. */
        UINT32  ClkFreqSel : 3;   /**< [27:25] Operating frequency select. */
        UINT32  IoModeSel : 2;   /**< [29:28] IO mode select. */
        UINT32  AlertModeSel : 1;   /**< [30]    Alert mode select. */
        UINT32  CrcCheckEn : 1;   /**< [31]    CRC checking enable. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_CONFIG_REGISTER;

//
// ESPIx6C - SLAVE0_INT_EN  (FCH::ITF::ESPI::SLAVE0_INT_EN)
//

/** ESPIx0000006C — Slave0 Interrupt Enable. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  BusErrIntEn : 1;   /**< [0]     Bus error interrupt enable. */
        UINT32  WaitTimeoutIntEn : 1;   /**< [1]     Wait timeout interrupt enable. */
        UINT32  CrcErrIntEn : 1;   /**< [2]     CRC error interrupt enable. */
        UINT32  Reserved0 : 1;   /**< [3]     Reserved. */
        UINT32  NoRspIntEn : 1;   /**< [4]     No response interrupt enable. */
        UINT32  FatalErrIntEn : 1;   /**< [5]     Fatal error interrupt enable. */
        UINT32  NonFatalErrIntEn : 1;   /**< [6]     Non-fatal error interrupt enable. */
        UINT32  InvalidRspIntEn : 1;   /**< [7]     Invalid response interrupt enable. */
        UINT32  InvalidCtIntEn : 1;   /**< [8]     Invalid cycle type interrupt enable. */
        UINT32  UnsucsCplIntEn : 1;   /**< [9]     Unsuccessful completion interrupt enable. */
        UINT32  IllegalTagIntEn : 1;   /**< [10]    Illegal tag interrupt enable. */
        UINT32  IllegalLenIntEn : 1;   /**< [11]    Illegal length interrupt enable. */
        UINT32  RxoobOverflowIntEn : 1;   /**< [12]    RX OOB overflow interrupt enable. */
        UINT32  RxmsgOverflowIntEn : 1;   /**< [13]    RX message overflow interrupt enable. */
        UINT32  RxflashOverflowIntEn : 1;   /**< [14]    RX flash overflow interrupt enable. */
        UINT32  ProtocolErrIntEn : 1;   /**< [15]    Protocol error interrupt enable. */
        UINT32  Reserved1 : 2;   /**< [17:16] Reserved. */
        UINT32  MstAbortIntEn : 1;   /**< [18]    Master abort interrupt enable. */
        UINT32  WdgTimeoutIntEn : 1;   /**< [19]    Watchdog timeout interrupt enable. */
        UINT32  Reserved2 : 4;   /**< [23:20] Reserved. */
        UINT32  RxvwGrp0IntEn : 1;   /**< [24]    RX VW group 0 interrupt enable. */
        UINT32  RxvwGrp1IntEn : 1;   /**< [25]    RX VW group 1 interrupt enable. */
        UINT32  RxvwGrp2IntEn : 1;   /**< [26]    RX VW group 2 interrupt enable. */
        UINT32  RxvwGrp3IntEn : 1;   /**< [27]    RX VW group 3 interrupt enable. */
        UINT32  DncmdIntEn : 1;   /**< [28]    DN command interrupt enable. */
        UINT32  RxmsgIntEn : 1;   /**< [29]    RX message interrupt enable. */
        UINT32  RxoobIntEn : 1;   /**< [30]    RX OOB interrupt enable. */
        UINT32  FlashReqIntEn : 1;   /**< [31]    Flash request interrupt enable. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_INT_EN_REGISTER;

//
// ESPIx70 - SLAVE0_INT_STS  (FCH::ITF::ESPI::SLAVE0_INT_STS)
//

/** ESPIx00000070 — Slave0 Interrupt Status. Reset: 0000_0000h.
    All status bits are Read, Write-1-to-clear. **/
typedef union {
    struct {
        UINT32  BusErrInt : 1;   /**< [0]     Bus error interrupt (W1C). */
        UINT32  WaitTimeoutInt : 1;   /**< [1]     Wait timeout interrupt (W1C). */
        UINT32  CrcErrInt : 1;   /**< [2]     CRC error interrupt (W1C). */
        UINT32  Reserved0 : 1;   /**< [3]     Reserved. */
        UINT32  NoRspInt : 1;   /**< [4]     No response interrupt (W1C). */
        UINT32  FatalErrInt : 1;   /**< [5]     Fatal error interrupt (W1C). */
        UINT32  NonFatalErrInt : 1;   /**< [6]     Non-fatal error interrupt (W1C). */
        UINT32  UnknownRspInt : 1;   /**< [7]     Unknown response interrupt (W1C). */
        UINT32  UnknownCtInt : 1;   /**< [8]     Unknown cycle type interrupt (W1C). */
        UINT32  UnsucsCplInt : 1;   /**< [9]     Unsuccessful completion interrupt (W1C). */
        UINT32  IllegalTagInt : 1;   /**< [10]    Illegal tag interrupt (W1C). */
        UINT32  IllegalLenInt : 1;   /**< [11]    Illegal length interrupt (W1C). */
        UINT32  RxoobOverflowInt : 1;   /**< [12]    RX OOB overflow interrupt (W1C). */
        UINT32  RxmsgOverflowInt : 1;   /**< [13]    RX message overflow interrupt (W1C). */
        UINT32  RxflashOverflowInt : 1;   /**< [14]    RX flash overflow interrupt (W1C). */
        UINT32  ProtocolErrInt : 1;   /**< [15]    Protocol error interrupt (W1C). */
        UINT32  Reserved1 : 2;   /**< [17:16] Reserved. */
        UINT32  MstAbortInt : 1;   /**< [18]    Master abort interrupt (W1C). */
        UINT32  WdgTimeoutInt : 1;   /**< [19]    Watchdog timeout interrupt (W1C). */
        UINT32  Reserved2 : 4;   /**< [23:20] Reserved. */
        UINT32  RxvwGrp0Int : 1;   /**< [24]    RX VW group 0 interrupt (W1C). */
        UINT32  RxvwGrp1Int : 1;   /**< [25]    RX VW group 1 interrupt (W1C). */
        UINT32  RxvwGrp2Int : 1;   /**< [26]    RX VW group 2 interrupt (W1C). */
        UINT32  RxvwGrp3Int : 1;   /**< [27]    RX VW group 3 interrupt (W1C). */
        UINT32  DncmdInt : 1;   /**< [28]    DN command interrupt (W1C). */
        UINT32  RxmsgInt : 1;   /**< [29]    RX message interrupt (W1C). */
        UINT32  RxoobInt : 1;   /**< [30]    RX OOB interrupt (W1C). */
        UINT32  FlashReqInt : 1;   /**< [31]    Flash request interrupt (W1C). */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_INT_STS_REGISTER;

//
// ESPIx74 - SLAVE0_RXMSG_HDR0  (FCH::ITF::ESPI::SLAVE0_RXMSG_HDR0)
//

/** ESPIx00000074 — Slave0 RX Message Header 0. Reset: 0000_0000h (RO). **/
typedef union {
    struct {
        UINT32  CycleType : 8;   /**< [7:0]   CycleType for Peripheral Msg/MsgD (RO). */
        UINT32  Byte1 : 8;   /**< [15:8]  Tag[15:12], Length[11:8] (RO). */
        UINT32  Byte2 : 8;   /**< [23:16] Peripheral Message Length[7:0] (RO). */
        UINT32  Byte3 : 8;   /**< [31:24] Received Peripheral Message code (RO). */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXMSG_HDR0_REGISTER;

//
// ESPIx78 - SLAVE0_RXMSG_HDR1  (FCH::ITF::ESPI::SLAVE0_RXMSG_HDR1)
//

/** ESPIx00000078 — Slave0 RX Message Header 1. Reset: 0000_0000h (RO). **/
typedef union {
    struct {
        UINT32  SpecificByte0 : 8;   /**< [7:0]   Peripheral Message Specific Byte 0 (RO). */
        UINT32  SpecificByte1 : 8;   /**< [15:8]  Peripheral Message Specific Byte 1 (RO). */
        UINT32  SpecificByte2 : 8;   /**< [23:16] Peripheral Message Specific Byte 2 (RO). */
        UINT32  SpecificByte3 : 8;   /**< [31:24] Peripheral Message Specific Byte 3 (RO). */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXMSG_HDR1_REGISTER;

//
// ESPIx7C - SLAVE0_RXMSG_DATA_PORT  (FCH::ITF::ESPI::SLAVE0_RXMSG_DATA_PORT)
//

/** ESPIx0000007C — Slave0 RX Message Data Port. Reset: 0000_0000h (RO). **/
typedef union {
    struct {
        UINT32  RxmsgData : 32;  /**< [31:0] Received message data (RO). */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXMSG_DATA_PORT_REGISTER;

//
// ESPIx80 - SLAVE0_IO_BASE_REG2  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG2)
//

/** ESPIx00000080 — Slave0 IO Base Register 2. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range4 : 16;  /**< [15:0]  IO decode base address for Range 4. */
        UINT32  Range5 : 16;  /**< [31:16] IO decode base address for Range 5. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG2_REGISTER;

//
// ESPIx84 - SLAVE0_IO_BASE_REG3  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG3)
//

/** ESPIx00000084 — Slave0 IO Base Register 3. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range6 : 16;  /**< [15:0]  IO decode base address for Range 6. */
        UINT32  Range7 : 16;  /**< [31:16] IO decode base address for Range 7. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG3_REGISTER;

//
// ESPIx88 - SLAVE0_IO_SIZE1  (FCH::ITF::ESPI::SLAVE0_IO_SIZE1)
//

/** ESPIx00000088 — Slave0 IO Size 1. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range4 : 8;   /**< [7:0]   Programmable IO Range 4 size. */
        UINT32  Range5 : 8;   /**< [15:8]  Programmable IO Range 5 size. */
        UINT32  Range6 : 8;   /**< [23:16] Programmable IO Range 6 size. */
        UINT32  Range7 : 8;   /**< [31:24] Programmable IO Range 7 size. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_SIZE1_REGISTER;

//
// ESPIx8C - SLAVE0_IO_BASE_REG4  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG4)
//

/** ESPIx0000008C — Slave0 IO Base Register 4. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range8 : 16;  /**< [15:0]  IO decode base address for Range 8. */
        UINT32  Range9 : 16;  /**< [31:16] IO decode base address for Range 9. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG4_REGISTER;

//
// ESPIx90 - SLAVE0_IO_BASE_REG5  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG5)
//

/** ESPIx00000090 — Slave0 IO Base Register 5. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range10 : 16;  /**< [15:0]  IO decode base address for Range 10. */
        UINT32  Range11 : 16;  /**< [31:16] IO decode base address for Range 11. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG5_REGISTER;

//
// ESPIx94 - SLAVE0_IO_SIZE2  (FCH::ITF::ESPI::SLAVE0_IO_SIZE2)
//

/** ESPIx00000094 — Slave0 IO Size 2. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range8 : 8;   /**< [7:0]   Programmable IO Range 8 size. */
        UINT32  Range9 : 8;   /**< [15:8]  Programmable IO Range 9 size. */
        UINT32  Range10 : 8;   /**< [23:16] Programmable IO Range 10 size. */
        UINT32  Range11 : 8;   /**< [31:24] Programmable IO Range 11 size. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_SIZE2_REGISTER;

//
// ESPIx98 - RESERVED_RXMSG_REG6  (FCH::ITF::ESPI::RESERVED_RXMSG_REG6)
//

/** ESPIx00000098 — Reserved / VW IRQ Inactive Polarity. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  VwIrqInactivePolarity : 24;  /**< [23:0]  VW IRQ inactive polarity. */
        UINT32  Reserved0 : 8;   /**< [31:24] Reserved. */
    } Bits;
    UINT32  Value;
} ESPI_RESERVED_RXMSG_REG6_REGISTER;

//
// ESPIx9C - SLAVE0_RXVW  (FCH::ITF::ESPI::SLAVE0_RXVW)
//

/** ESPIx0000009C — Slave0 RX Virtual Wire. Reset: 0007_0C00h. **/
typedef union {
    struct {
        UINT32  IrqSel : 5;   /**< [4:0]   IRQ selection. */
        UINT32  IrqSts : 3;   /**< [7:5]   IRQ status (RO). */
        UINT32  OobRstAck : 1;   /**< [8]     OOB reset acknowledge (W1C). */
        UINT32  Reserved0 : 1;   /**< [9]     Reserved. */
        UINT32  WakeB : 1;   /**< [10]    WAKE# (Read, Write-1-only). Reset: 1. */
        UINT32  PmeB : 1;   /**< [11]    PME# (Read, Write-1-only). Reset: 1. */
        UINT32  SlaveBootLoadDone : 1;   /**< [12]    Slave boot load done (W1C). */
        UINT32  ErrorFatal : 1;   /**< [13]    Fatal error (W1C). */
        UINT32  ErrorNonfatal : 1;   /**< [14]    Non-fatal error (W1C). */
        UINT32  SlaveBootLoadSts : 1;   /**< [15]    Slave boot load status (W1C). */
        UINT32  SciB : 1;   /**< [16]    SCI# (Read, Write-1-only). Reset: 1. */
        UINT32  SmiB : 1;   /**< [17]    SMI# (Read, Write-1-only). Reset: 1. */
        UINT32  RcinB : 1;   /**< [18]    RCIN# (Read, Write-1-only). Reset: 1. */
        UINT32  HostRstAck : 1;   /**< [19]    Host reset acknowledge (W1C). */
        UINT32  Reserved1 : 10;  /**< [29:20] Reserved. */
        UINT32  RtctimeReq : 1;   /**< [30]    RTC time request (W1C). */
        UINT32  CputempReq : 1;   /**< [31]    CPU temp request (W1C). */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXVW_REGISTER;

//
// ESPIxA0 - SLAVE0_RXVW_DATA  (FCH::ITF::ESPI::SLAVE0_RXVW_DATA)
//

/** ESPIx000000A0 — Slave0 RX VW Data. Reset: 0000_0000h (RO). **/
typedef union {
    struct {
        UINT32  Grp0 : 8;   /**< [7:0]   Group 0 Virtual Wire data (RO). */
        UINT32  Grp1 : 8;   /**< [15:8]  Group 1 Virtual Wire data (RO). */
        UINT32  Grp2 : 8;   /**< [23:16] Group 2 Virtual Wire data (RO). */
        UINT32  Grp3 : 8;   /**< [31:24] Group 3 Virtual Wire data (RO). */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXVW_DATA_REGISTER;

//
// ESPIxA4 - SLAVE0_RXVW_INDEX  (FCH::ITF::ESPI::SLAVE0_RXVW_INDEX)
//

/** ESPIx000000A4 — Slave0 RX VW Index Selection. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Grp0 : 8;   /**< [7:0]   Group 0 VW Index Selection Register. */
        UINT32  Grp1 : 8;   /**< [15:8]  Group 1 VW Index Selection Register. */
        UINT32  Grp2 : 8;   /**< [23:16] Group 2 VW Index Selection Register. */
        UINT32  Grp3 : 8;   /**< [31:24] Group 3 VW Index Selection Register. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXVW_INDEX_REGISTER;

//
// ESPIxA8 - SLAVE0_RXVW_MISC_CNTL  (FCH::ITF::ESPI::SLAVE0_RXVW_MISC_CNTL)
//

/** ESPIx000000A8 — Slave0 RX VW Misc Control. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Grp0En : 1;   /**< [0]     Group 0 enable. */
        UINT32  Grp1En : 1;   /**< [1]     Group 1 enable. */
        UINT32  Grp2En : 1;   /**< [2]     Group 2 enable. */
        UINT32  Grp3En : 1;   /**< [3]     Group 3 enable. */
        UINT32  SusStatVwen : 1;   /**< [4]     SUS_STAT# VW enable. */
        UINT32  Reserved0 : 1;   /**< [5]     Reserved. */
        UINT32  CputempRtctimeVwEn : 1;   /**< [6]     CPU temp / RTC time VW enable. */
        UINT32  CputempRtctimeVwIdxSel : 1;   /**< [7]     CPU temp / RTC time VW index select. */
        UINT32  Irq0Mask : 1;   /**< [8]     IRQ 0 mask. */
        UINT32  Irq1Mask : 1;   /**< [9]     IRQ 1 mask. */
        UINT32  Irq2Mask : 1;   /**< [10]    IRQ 2 mask. */
        UINT32  Irq3Mask : 1;   /**< [11]    IRQ 3 mask. */
        UINT32  Irq4Mask : 1;   /**< [12]    IRQ 4 mask. */
        UINT32  Irq5Mask : 1;   /**< [13]    IRQ 5 mask. */
        UINT32  Irq6Mask : 1;   /**< [14]    IRQ 6 mask. */
        UINT32  Irq7Mask : 1;   /**< [15]    IRQ 7 mask. */
        UINT32  Irq8Mask : 1;   /**< [16]    IRQ 8 mask. */
        UINT32  Irq9Mask : 1;   /**< [17]    IRQ 9 mask. */
        UINT32  Irq10Mask : 1;   /**< [18]    IRQ 10 mask. */
        UINT32  Irq11Mask : 1;   /**< [19]    IRQ 11 mask. */
        UINT32  Irq12Mask : 1;   /**< [20]    IRQ 12 mask. */
        UINT32  Irq13Mask : 1;   /**< [21]    IRQ 13 mask. */
        UINT32  Irq14Mask : 1;   /**< [22]    IRQ 14 mask. */
        UINT32  Irq15Mask : 1;   /**< [23]    IRQ 15 mask. */
        UINT32  Irq16Mask : 1;   /**< [24]    IRQ 16 mask. */
        UINT32  Irq17Mask : 1;   /**< [25]    IRQ 17 mask. */
        UINT32  Irq18Mask : 1;   /**< [26]    IRQ 18 mask. */
        UINT32  Irq19Mask : 1;   /**< [27]    IRQ 19 mask. */
        UINT32  Irq20Mask : 1;   /**< [28]    IRQ 20 mask. */
        UINT32  Irq21Mask : 1;   /**< [29]    IRQ 21 mask. */
        UINT32  Irq22Mask : 1;   /**< [30]    IRQ 22 mask. */
        UINT32  Irq23Mask : 1;   /**< [31]    IRQ 23 mask. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXVW_MISC_CNTL_REGISTER;

//
// ESPIxAC - SLAVE0_RXVW_POLARITY  (FCH::ITF::ESPI::SLAVE0_RXVW_POLARITY)
//

/** ESPIx000000AC — Slave0 RX VW Polarity. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Irq0Polarity : 1;   /**< [0]     IRQ 0 polarity. */
        UINT32  Irq1Polarity : 1;   /**< [1]     IRQ 1 polarity. */
        UINT32  Irq2Polarity : 1;   /**< [2]     IRQ 2 polarity. */
        UINT32  Irq3Polarity : 1;   /**< [3]     IRQ 3 polarity. */
        UINT32  Irq4Polarity : 1;   /**< [4]     IRQ 4 polarity. */
        UINT32  Irq5Polarity : 1;   /**< [5]     IRQ 5 polarity. */
        UINT32  Irq6Polarity : 1;   /**< [6]     IRQ 6 polarity. */
        UINT32  Irq7Polarity : 1;   /**< [7]     IRQ 7 polarity. */
        UINT32  Irq8Polarity : 1;   /**< [8]     IRQ 8 polarity. */
        UINT32  Irq9Polarity : 1;   /**< [9]     IRQ 9 polarity. */
        UINT32  Irq10Polarity : 1;   /**< [10]    IRQ 10 polarity. */
        UINT32  Irq11Polarity : 1;   /**< [11]    IRQ 11 polarity. */
        UINT32  Irq12Polarity : 1;   /**< [12]    IRQ 12 polarity. */
        UINT32  Irq13Polarity : 1;   /**< [13]    IRQ 13 polarity. */
        UINT32  Irq14Polarity : 1;   /**< [14]    IRQ 14 polarity. */
        UINT32  Irq15Polarity : 1;   /**< [15]    IRQ 15 polarity. */
        UINT32  Irq16Polarity : 1;   /**< [16]    IRQ 16 polarity. */
        UINT32  Irq17Polarity : 1;   /**< [17]    IRQ 17 polarity. */
        UINT32  Irq18Polarity : 1;   /**< [18]    IRQ 18 polarity. */
        UINT32  Irq19Polarity : 1;   /**< [19]    IRQ 19 polarity. */
        UINT32  Irq20Polarity : 1;   /**< [20]    IRQ 20 polarity. */
        UINT32  Irq21Polarity : 1;   /**< [21]    IRQ 21 polarity. */
        UINT32  Irq22Polarity : 1;   /**< [22]    IRQ 22 polarity. */
        UINT32  Irq23Polarity : 1;   /**< [23]    IRQ 23 polarity. */
        UINT32  PltrstbDis : 1;   /**< [24]    PLTRST# disable. */
        UINT32  HostRstWarnDis : 1;   /**< [25]    Host reset warn disable. */
        UINT32  SmioutEn : 1;   /**< [26]    SMIOUT enable. */
        UINT32  NmioutEn : 1;   /**< [27]    NMIOUT enable. */
        UINT32  OobRstWarnEn : 1;   /**< [28]    OOB reset warn enable. */
        UINT32  Reserved0 : 3;   /**< [31:29] Reserved. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_RXVW_POLARITY_REGISTER;

//
// ESPIxB0 - SLAVE0_IO_BASE_REG6  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG6)
//

/** ESPIx000000B0 — Slave0 IO Base Register 6. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range12 : 16;  /**< [15:0]  IO decode base address for Range 12. */
        UINT32  Range13 : 16;  /**< [31:16] IO decode base address for Range 13. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG6_REGISTER;

//
// ESPIxB4 - SLAVE0_IO_BASE_REG7  (FCH::ITF::ESPI::SLAVE0_IO_BASE_REG7)
//

/** ESPIx000000B4 — Slave0 IO Base Register 7. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range14 : 16;  /**< [15:0]  IO decode base address for Range 14. */
        UINT32  Range15 : 16;  /**< [31:16] IO decode base address for Range 15. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_BASE_REG7_REGISTER;

//
// ESPIxB8 - SLAVE0_IO_SIZE3  (FCH::ITF::ESPI::SLAVE0_IO_SIZE3)
//

/** ESPIx000000B8 — Slave0 IO Size 3. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range12 : 8;   /**< [7:0]   Programmable IO Range 12 size. */
        UINT32  Range13 : 8;   /**< [15:8]  Programmable IO Range 13 size. */
        UINT32  Range14 : 8;   /**< [23:16] Programmable IO Range 14 size. */
        UINT32  Range15 : 8;   /**< [31:24] Programmable IO Range 15 size. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_IO_SIZE3_REGISTER;

//
// ESPIxBC - SLAVE0_MMIO_BASE_REG4  (FCH::ITF::ESPI::SLAVE0_MMIO_BASE_REG4)
//

/** ESPIx000000BC — Slave0 MMIO Base Register 4. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range4 : 32;  /**< [31:0] MMIO decode base address for Range 4. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_BASE_REG4_REGISTER;

//
// ESPIxC0 - SLAVE0_MMIO_SIZE_REG2  (FCH::ITF::ESPI::SLAVE0_MMIO_SIZE_REG2)
//

/** ESPIx000000C0 — Slave0 MMIO Size Register 2. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  Range4 : 25;  /**< [24:0]  Programmable MMIO Range 4 size. */
        UINT32  Reserved0 : 7;   /**< [31:25] Reserved. */
    } Bits;
    UINT32  Value;
} ESPI_SLAVE0_MMIO_SIZE_REG2_REGISTER;

//
// ESPIxC4 - MMIO_CPUTEMP  (FCH::ITF::ESPI::MMIO_CPUTEMP)
//

/** ESPIx000000C4 — MMIO CPU Temperature Address. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  MmioCputemp : 32;  /**< [31:0] MMIO address to send down CPU temperature. */
    } Bits;
    UINT32  Value;
} ESPI_MMIO_CPUTEMP_REGISTER;

//
// ESPIxC8 - MMIO_RTCTIME  (FCH::ITF::ESPI::MMIO_RTCTIME)
//

/** ESPIx000000C8 — MMIO RTC Time Address. Reset: 0000_0000h. **/
typedef union {
    struct {
        UINT32  MmioRtctime : 32;  /**< [31:0] MMIO address to send down RTC TIME. */
    } Bits;
    UINT32  Value;
} ESPI_MMIO_RTCTIME_REGISTER;

//
// Complete eSPI register block overlay
//

/**
  Complete eSPI controller register block mapped at ESPI_LINKn_MMIO_BASE.

  @note  Cast a pointer to this struct over the MMIO base to access
         any register:
           ((ESPI_REGISTER_BLOCK *)(UINTN)ESPI_LINK0_MMIO_BASE)->Slave0Config.Value
**/
typedef struct {
    ESPI_DN_TXHDR_0TH_REGISTER             DnTxhdr0th;          /**< 0x00 */
    ESPI_DN_TXHDR_1_REGISTER               DnTxhdr1;            /**< 0x04 */
    ESPI_DN_TXHDR_2_REGISTER               DnTxhdr2;            /**< 0x08 */
    ESPI_DN_TXDATA_PORT_REGISTER            DnTxdataPort;        /**< 0x0C */
    ESPI_UP_RXHDR_0_REGISTER               UpRxhdr0;            /**< 0x10 */
    ESPI_UP_RXHDR_1_REGISTER               UpRxhdr1;            /**< 0x14 */
    ESPI_UP_RXDATA_PORT_REGISTER            UpRxdataPort;        /**< 0x18 */
    UINT32                                  ReservedReg0;        /**< 0x1C  Reserved. */
    UINT32                                  ReservedReg1;        /**< 0x20  Reserved. */
    UINT32                                  ReservedReg2;        /**< 0x24  Reserved. */
    UINT32                                  Reserved28;          /**< 0x28  Gap. */
    ESPI_MASTER_CAP_REGISTER               MasterCap;           /**< 0x2C */
    UINT32                                  Reserved30;          /**< 0x30  Gap. */
    UINT32                                  Reserved34;          /**< 0x34  Gap. */
    UINT32                                  Reserved38;          /**< 0x38  Gap. */
    ESPI_MISC_CONTROL_1_REGISTER           MiscControl1;        /**< 0x3C */
    UINT32                                  Reserved40;          /**< 0x40  Gap. */
    ESPI_SLAVE0_IO_BASE_REG0_REGISTER      Slave0IoBaseReg0;    /**< 0x44 */
    ESPI_SLAVE0_IO_BASE_REG1_REGISTER      Slave0IoBaseReg1;    /**< 0x48 */
    ESPI_SLAVE0_IO_SIZE_REGISTER           Slave0IoSize;        /**< 0x4C */
    ESPI_SLAVE0_MMIO_BASE_REG0_REGISTER    Slave0MmioBaseReg0;  /**< 0x50 */
    ESPI_SLAVE0_MMIO_BASE_REG1_REGISTER    Slave0MmioBaseReg1;  /**< 0x54 */
    ESPI_SLAVE0_MMIO_BASE_REG2_REGISTER    Slave0MmioBaseReg2;  /**< 0x58 */
    ESPI_SLAVE0_MMIO_BASE_REG3_REGISTER    Slave0MmioBaseReg3;  /**< 0x5C */
    ESPI_SLAVE0_MMIO_SIZE_REG0_REGISTER    Slave0MmioSizeReg0;  /**< 0x60 */
    ESPI_SLAVE0_MMIO_SIZE_REG1_REGISTER    Slave0MmioSizeReg1;  /**< 0x64 */
    ESPI_SLAVE0_CONFIG_REGISTER            Slave0Config;        /**< 0x68 */
    ESPI_SLAVE0_INT_EN_REGISTER            Slave0IntEn;         /**< 0x6C */
    ESPI_SLAVE0_INT_STS_REGISTER           Slave0IntSts;        /**< 0x70 */
    ESPI_SLAVE0_RXMSG_HDR0_REGISTER       Slave0RxmsgHdr0;     /**< 0x74 */
    ESPI_SLAVE0_RXMSG_HDR1_REGISTER       Slave0RxmsgHdr1;     /**< 0x78 */
    ESPI_SLAVE0_RXMSG_DATA_PORT_REGISTER  Slave0RxmsgDataPort; /**< 0x7C */
    ESPI_SLAVE0_IO_BASE_REG2_REGISTER      Slave0IoBaseReg2;    /**< 0x80 */
    ESPI_SLAVE0_IO_BASE_REG3_REGISTER      Slave0IoBaseReg3;    /**< 0x84 */
    ESPI_SLAVE0_IO_SIZE1_REGISTER          Slave0IoSize1;       /**< 0x88 */
    ESPI_SLAVE0_IO_BASE_REG4_REGISTER      Slave0IoBaseReg4;    /**< 0x8C */
    ESPI_SLAVE0_IO_BASE_REG5_REGISTER      Slave0IoBaseReg5;    /**< 0x90 */
    ESPI_SLAVE0_IO_SIZE2_REGISTER          Slave0IoSize2;       /**< 0x94 */
    ESPI_RESERVED_RXMSG_REG6_REGISTER     ReservedRxmsgReg6;   /**< 0x98 */
    ESPI_SLAVE0_RXVW_REGISTER             Slave0Rxvw;          /**< 0x9C */
    ESPI_SLAVE0_RXVW_DATA_REGISTER        Slave0RxvwData;      /**< 0xA0 */
    ESPI_SLAVE0_RXVW_INDEX_REGISTER       Slave0RxvwIndex;     /**< 0xA4 */
    ESPI_SLAVE0_RXVW_MISC_CNTL_REGISTER   Slave0RxvwMiscCntl;  /**< 0xA8 */
    ESPI_SLAVE0_RXVW_POLARITY_REGISTER    Slave0RxvwPolarity;  /**< 0xAC */
    ESPI_SLAVE0_IO_BASE_REG6_REGISTER      Slave0IoBaseReg6;    /**< 0xB0 */
    ESPI_SLAVE0_IO_BASE_REG7_REGISTER      Slave0IoBaseReg7;    /**< 0xB4 */
    ESPI_SLAVE0_IO_SIZE3_REGISTER          Slave0IoSize3;       /**< 0xB8 */
    ESPI_SLAVE0_MMIO_BASE_REG4_REGISTER    Slave0MmioBaseReg4;  /**< 0xBC */
    ESPI_SLAVE0_MMIO_SIZE_REG2_REGISTER    Slave0MmioSizeReg2;  /**< 0xC0 */
    ESPI_MMIO_CPUTEMP_REGISTER            MmioCputemp;         /**< 0xC4 */
    ESPI_MMIO_RTCTIME_REGISTER            MmioRtctime;         /**< 0xC8 */
} ESPI_REGISTER_BLOCK;

#pragma pack()

//
// Compile-time check: the register block must be exactly 204 bytes (0xCC).
//
STATIC_ASSERT(
    sizeof(ESPI_REGISTER_BLOCK) == 0xCC,
    "ESPI_REGISTER_BLOCK size mismatch - must be 204 bytes (0xCC)"
);

#endif // ESPI_H_