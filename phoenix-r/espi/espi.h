/** @file
  Custom eSPI Protocol definitions for the AMD FCH eSPI controller.

  The FCH eSPI controller is the eSPI master.  Its flash channel is
  slave-initiated: the slave raises a GET_FLASH_NP request (surfaced through
  ESPIx10/ESPIx14) and the master answers with a Flash channel completion
  (ESPIx00/ESPIx0C).  All downstream traffic is launched by programming the
  ESPIx00..ESPIx0C registers and setting the DNCMD_STATUS bit last.

  @note  AMD PPR Family 19h Model 11h Rev B2, Volume 5, Section 10.3.8.2.
**/

#ifndef ESPI_PROTOCOL_H_
#define ESPI_PROTOCOL_H_

//
// Downstream command types (DNCMD_TYPE, ESPIx00[2:0]).
//

#define ESPI_DNCMD_SET_CONFIGURATION   0x0  ///< Set Configuration (Independent command).
#define ESPI_DNCMD_GET_CONFIGURATION   0x1  ///< Get Configuration (Independent command).
#define ESPI_DNCMD_INBAND_RESET        0x2  ///< In-band RESET (Independent command).
#define ESPI_DNCMD_PERIPH_UNSUCC_CPL   0x3  ///< Peripheral Unsuccessful Completion downstream.
#define ESPI_DNCMD_PERIPH_MESSAGE      0x4  ///< Peripheral Channel message downstream.
#define ESPI_DNCMD_VW_DOWN             0x5  ///< Virtual Wire Channel downstream.
#define ESPI_DNCMD_OOB_DOWN            0x6  ///< OOB Channel downstream.
#define ESPI_DNCMD_FLASH_CPL           0x7  ///< Flash Channel Cpl/CplD/Unsuccessful Cpl downstream.

//
// Upstream command types (UPCMD_TYPE, ESPIx10[2:0]).
//

#define ESPI_UPCMD_FLASH_REQUEST       0x0  ///< Flash Channel request (GET_FLASH_NP).
#define ESPI_UPCMD_OOB_MESSAGE         0x1  ///< Upstream OOB message (GET_OOB).

//
// eSPI cycle types written to DNCMD_HDATA0 / read from UPCMD_HDATA0.
//

#define ESPI_CYCLETYPE_COMPLETION       0x06  ///< Flash: successful completion, no data (Cpl).
#define ESPI_CYCLETYPE_COMPLETION_DATA  0x07  ///< Flash: successful completion with data (CplD).
#define ESPI_CYCLETYPE_NON_COMPLETION   0x46  ///< Flash: unsuccessful completion (NC).
#define ESPI_CYCLETYPE_MSG_WITH_DATA    0x10  ///< Peripheral: message with data.
#define ESPI_CYCLETYPE_MSG_NO_DATA      0x11  ///< Peripheral: message without data.
#define ESPI_CYCLETYPE_TUNNELED_SMBUS   0x21  ///< OOB: tunneled SMBus message.

//
// Controller limits (MASTER_CAP fixed values) and timeouts.
//

#define ESPI_FLASH_MAX_PAYLOAD   128  ///< FLASH_MAX_SIZE = 2h: 128-byte flash payload.
#define ESPI_OOB_MAX_PAYLOAD     128  ///< OOB_MAX_SIZE   = 2h: 128-byte OOB payload.
#define ESPI_PERIPH_MAX_PAYLOAD  64   ///< PR_MAX_SIZE     = 1h: 64-byte peripheral payload.
#define ESPI_VW_MAX_GROUPS       15   ///< VW_MAX_SIZE     = 0Fh: 15 virtual-wire groups.

#define ESPI_CMD_TIMEOUT_MS      500  ///< Downstream command dispatch timeout.
#define ESPI_POLL_INTERVAL_US    10   ///< Delay between two DNCMD_STATUS polls.

typedef struct _ESPI_PROTOCOL ESPI_PROTOCOL;

/**
  A slave-initiated flash channel read request (GET_FLASH_NP).

  @note  The flash channel is slave driven.  The FCH surfaces only
         GET_FLASH_NP upstream: the slave asks the master for a byte range of
         the flash, and the master answers with a Flash channel completion.
**/
typedef struct {
  UINT8   CycleType;   ///< Cycle type of the incoming GET_FLASH_NP packet.
  UINT8   Tag;         ///< 4-bit tag the completion must echo back.
  UINT16  Length;      ///< 12-bit transfer length in bytes (<= ESPI_FLASH_MAX_PAYLOAD).
  UINT32  Address;     ///< 32-bit flash address the slave wants to read.
} ESPI_FLASH_REQUEST;

/**
  A slave-initiated OOB message (GET_OOB, e.g. tunneled SMBus).

  The message data is available behind the header through the RX data port.
**/
typedef struct {
  UINT8   CycleType;    ///< Cycle type of the incoming GET_OOB packet.
  UINT8   Tag;          ///< 4-bit tag echoed by the slave.
  UINT16  Length;       ///< 12-bit OOB payload length in bytes.
  UINT8   SlaveAddress; ///< SMBus slave address.
  UINT8   Command;      ///< SMBus command opcode.
  UINT8   ByteCount;    ///< SMBus byte count (<= ESPI_OOB_MAX_PAYLOAD).
} ESPI_OOB_REQUEST;

/**
  Reads the fixed master capability register (ESPIx2C).

  @param[in]   EspiBase      MMIO base address of the FCH eSPI register block
                (ESPI_LINK0_MMIO_BASE or ESPI_LINK1_MMIO_BASE).
  @param[out]  Capabilities  Pointer to a caller-allocated register union that
                receives ESPIx2C.

  @retval EFI_SUCCESS            The capabilities were read.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Capabilities is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_READ_MASTER_CAP)(
  IN   UINTN                    EspiBase,
  OUT  ESPI_MASTER_CAP_REGISTER *Capabilities
  );

/**
  Reads the current slave 0 configuration (ESPIx68).

  @param[in]   EspiBase       MMIO base address of the FCH eSPI register block.
  @param[out]  Configuration  Pointer to a caller-allocated register union that
                receives ESPIx68.

  @retval EFI_SUCCESS            The configuration was read.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Configuration is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_GET_CONFIGURATION)(
  IN   UINTN                      EspiBase,
  OUT  ESPI_SLAVE0_CONFIG_REGISTER *Configuration
  );

/**
  Writes the slave 0 configuration (ESPIx68).

  Channel enables, clock frequency, I/O mode, alert mode, and CRC checking are
  all programmed through this one register; every bit is written as-is.

  @param[in]  EspiBase       MMIO base address of the FCH eSPI register block.
  @param[in]  Configuration  Pointer to a caller-initialized register union to
                program into ESPIx68.

  @retval EFI_SUCCESS            The configuration was written.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Configuration is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_SET_CONFIGURATION)(
  IN  UINTN                      EspiBase,
  IN  CONST ESPI_SLAVE0_CONFIG_REGISTER *Configuration
  );

/**
  Reports whether a downstream command is still being dispatched.

  The controller clears DNCMD_STATUS (ESPIx00[3]) once the programmed packet
  has been sent down; a set bit means the previous command has not finished.

  @param[in]   EspiBase  MMIO base address of the FCH eSPI register block.
  @param[out]  Busy      Pointer to a caller-allocated BOOLEAN that receives the
            DNCMD_STATUS state.

  @retval EFI_SUCCESS            The busy state was read.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Busy is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_IS_BUSY)(
  IN   UINTN    EspiBase,
  OUT  BOOLEAN  *Busy
  );

/**
  Waits until the previous downstream command has been fully dispatched.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.

  @retval EFI_SUCCESS            The controller is idle (DNCMD_STATUS clear).
  @retval EFI_INVALID_PARAMETER  EspiBase is 0.
  @retval EFI_TIMEOUT            DNCMD_STATUS never cleared.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_WAIT_READY)(
  IN  UINTN  EspiBase
  );

/**
  Sends one downstream eSPI command (the raw primitive).

  Programs the three DN_TXHDR registers (HDATA0..HDATA7) and pushes the data
  payload into the DN_TXDATA_PORT (four bytes per 32-bit write, least
  significant byte first), then sets DNCMD_STATUS last to launch the packet.

  @param[in]  EspiBase     MMIO base address of the FCH eSPI register block.
  @param[in]  CommandType  One of ESPI_DNCMD_*.
  @param[in]  Header       Pointer to 8 bytes packed into DNCMD_HDATA0..7.
  @param[in]  Data         Pointer to the payload bytes; NULL when DataLength is 0.
  @param[in]  DataLength   Number of payload bytes (<= ESPI_FLASH_MAX_PAYLOAD).

  @retval EFI_SUCCESS            The command was dispatched.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, Header is NULL, CommandType is
                 not an ESPI_DNCMD_* value, a buffer is NULL while
                 its length is not 0, or the payload exceeds 128 bytes.
  @retval EFI_TIMEOUT            The previous command never drained.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_SEND_COMMAND)(
  IN  UINTN        EspiBase,
  IN  UINT8        CommandType,
  IN  CONST UINT8  *Header,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
  );

/**
  Answers a slave flash read with a Flash channel completion.

  The completion header carries the completion cycle type, the tag echoed back
  from the request, and the 12-bit length; the read data (if any) follows in
  the data port.

  @param[in]  EspiBase    MMIO base address of the FCH eSPI register block.
  @param[in]  CycleType   One of ESPI_CYCLETYPE_COMPLETION,
             ESPI_CYCLETYPE_COMPLETION_DATA, or ESPI_CYCLETYPE_NON_COMPLETION.
  @param[in]  Tag         4-bit tag echoed from the flash request.
  @param[in]  Length      12-bit length of the completion payload.
  @param[in]  Data        Pointer to the read data; NULL when Length is 0.
  @param[in]  DataLength  Number of data bytes (must equal Length).

  @retval EFI_SUCCESS            The completion was sent.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, DataLength does not match Length,
                 or a buffer is NULL while its length is not 0.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_SEND_FLASH_COMPLETION)(
  IN  UINTN        EspiBase,
  IN  UINT8        CycleType,
  IN  UINT8        Tag,
  IN  UINT16       Length,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
  );

/**
  Sends a tunneled SMBus message downstream on the OOB channel.

  DNCMD_HDATA0 is fixed to the tunneled-SMBus cycle type
  (ESPI_CYCLETYPE_TUNNELED_SMBUS), the SMBus slave address, command opcode,
  and byte count go into HDATA3..5, and the message data follows in the data
  port.

  @param[in]  EspiBase      MMIO base address of the FCH eSPI register block.
  @param[in]  Tag           4-bit tag for the OOB message.
  @param[in]  SlaveAddress  SMBus slave address (bit 0 must be set).
  @param[in]  Command       SMBus command opcode.
  @param[in]  Data          Pointer to the message bytes; NULL when DataLength is 0.
  @param[in]  DataLength    Number of message bytes (<= ESPI_OOB_MAX_PAYLOAD).

  @retval EFI_SUCCESS            The OOB message was sent.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, a buffer is NULL while its length
                 is not 0, or the payload exceeds 128 bytes.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_SEND_OOB_MESSAGE)(
  IN  UINTN        EspiBase,
  IN  UINT8        Tag,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Command,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
  );

/**
  Sends virtual-wire values downstream on the VW channel.

  DNCMD_HDATA0 carries the number of virtual-wire groups; each group is one
  byte in the data port, packed four groups per 32-bit write.

  @param[in]  EspiBase    MMIO base address of the FCH eSPI register block.
  @param[in]  GroupCount  Number of virtual-wire groups (<= ESPI_VW_MAX_GROUPS).
  @param[in]  VwData      Pointer to GroupCount bytes, one value per group.

  @retval EFI_SUCCESS            The virtual-wire values were sent.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, VwData is NULL, or GroupCount
                 exceeds ESPI_VW_MAX_GROUPS.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_SEND_VIRTUAL_WIRE)(
  IN  UINTN        EspiBase,
  IN  UINTN        GroupCount,
  IN  CONST UINT8  *VwData
  );

/**
  Sends a peripheral channel message downstream.

  @param[in]  EspiBase       MMIO base address of the FCH eSPI register block.
  @param[in]  CycleType      Peripheral message cycle type (ESPI_CYCLETYPE_MSG_*).
  @param[in]  Tag            4-bit tag for the message.
  @param[in]  Length         12-bit message length in bytes.
  @param[in]  MessageCode    Peripheral message code (DNCMD_HDATA3).
  @param[in]  SpecificBytes  Pointer to 4 message-specific bytes (HDATA4..7).
  @param[in]  Data           Pointer to the message data; NULL when DataLength is 0.
  @param[in]  DataLength     Number of message data bytes (<= ESPI_PERIPH_MAX_PAYLOAD).

  @retval EFI_SUCCESS            The peripheral message was sent.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, SpecificBytes is NULL, a buffer is
                 NULL while its length is not 0, or the payload exceeds 64 bytes.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_SEND_PERIPHERAL_MESSAGE)(
  IN  UINTN        EspiBase,
  IN  UINT8        CycleType,
  IN  UINT8        Tag,
  IN  UINT16       Length,
  IN  UINT8        MessageCode,
  IN  CONST UINT8  *SpecificBytes,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
  );

/**
  Sends one Independent channel command: Set/Get Configuration or In-band reset.

  The 12-bit register address is packed into DNCMD_HDATA0/HDATA1, and up to
  four bytes of configuration data go into DNCMD_HDATA3..6.

  @param[in]  EspiBase     MMIO base address of the FCH eSPI register block.
  @param[in]  CommandType  ESPI_DNCMD_SET_CONFIGURATION,
             ESPI_DNCMD_GET_CONFIGURATION, or ESPI_DNCMD_INBAND_RESET.
  @param[in]  Address      12-bit configuration register address (4-byte aligned).
  @param[in]  Data         Pointer to the configuration data; NULL when DataLength is 0.
  @param[in]  DataLength   Number of configuration bytes (0 - 4).

  @retval EFI_SUCCESS            The independent command was sent.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, CommandType is not 0/1/2, a buffer
                 is NULL while its length is not 0, or DataLength exceeds 4.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_SEND_INDEPENDENT_COMMAND)(
  IN  UINTN        EspiBase,
  IN  UINT8        CommandType,
  IN  UINT16       Address,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
  );

/**
  Reads a pending slave flash read request (GET_FLASH_NP).

  The request is only valid while UPCMD_STATUS (ESPIx10[3]) is set; the
  controller will not surface the next request until the bit is cleared with
  AcknowledgeRequest().

  @param[in]   EspiBase  MMIO base address of the FCH eSPI register block.
  @param[out]  Request   Pointer to a caller-allocated ESPI_FLASH_REQUEST that
            receives the address, tag, length, and cycle type.

  @retval EFI_SUCCESS            A flash request was read.
  @retval EFI_NOT_READY          No flash request is pending.
  @retval EFI_UNSUPPORTED        The pending packet is not a flash request.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Request is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_GET_FLASH_REQUEST)(
  IN   UINTN               EspiBase,
  OUT  ESPI_FLASH_REQUEST  *Request
  );

/**
  Reads a pending upstream OOB message (GET_OOB).

  @param[in]   EspiBase    MMIO base address of the FCH eSPI register block.
  @param[out]  Request     Pointer to a caller-allocated ESPI_OOB_REQUEST that
              receives the OOB header fields.
  @param[out]  Data        Pointer to a caller-allocated buffer for the message
              data; NULL when the caller only wants the header.
  @param[in,out] DataLength  On input, the capacity of Data in bytes; on output,
              the number of bytes stored.

  @retval EFI_SUCCESS            An OOB message was read.
  @retval EFI_NOT_READY          No OOB message is pending.
  @retval EFI_UNSUPPORTED        The pending packet is not an OOB message.
  @retval EFI_BUFFER_TOO_SMALL   Data is too small for the message payload.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Request is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_GET_OOB_REQUEST)(
  IN      UINTN            EspiBase,
  OUT     ESPI_OOB_REQUEST *Request,
  OUT     UINT8            *Data,
  IN OUT  UINTN            *DataLength
  );

/**
  Clears the upstream request valid bit (UPCMD_STATUS, W1C).

  Must be called after a request has been consumed so the controller will
  surface the next GET_FLASH_NP or GET_OOB.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.

  @retval EFI_SUCCESS            The valid bit was cleared.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0.
**/
typedef
EFI_STATUS
(EFIAPI *ESPI_ACKNOWLEDGE_REQUEST)(
  IN  UINTN  EspiBase
  );

/**
  Reads an 8-bit register of the FCH eSPI register block.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (0 - 0xFFF).

  @return The register value; 0xFF when the access was rejected.
**/
typedef
UINT8
(EFIAPI *ESPI_READ_U8)(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset
  );

/**
  Reads a 16-bit register of the FCH eSPI register block.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (0 - 0xFFF, 2-byte aligned).

  @return The register value; 0xFFFF when the access was rejected.
**/
typedef
UINT16
(EFIAPI *ESPI_READ_U16)(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset
  );

/**
  Reads a 32-bit register of the FCH eSPI register block.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (0 - 0xFFF, 4-byte aligned).

  @return The register value; 0xFFFFFFFF when the access was rejected.
**/
typedef
UINT32
(EFIAPI *ESPI_READ_U32)(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset
  );

/**
  Writes an 8-bit register of the FCH eSPI register block.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (0 - 0xFFF).
  @param[in]  Value     Value to write.
**/
typedef
VOID
(EFIAPI *ESPI_WRITE_U8)(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset,
  IN  UINT8   Value
  );

/**
  Writes a 16-bit register of the FCH eSPI register block.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (0 - 0xFFF, 2-byte aligned).
  @param[in]  Value     Value to write.
**/
typedef
VOID
(EFIAPI *ESPI_WRITE_U16)(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset,
  IN  UINT16  Value
  );

/**
  Writes a 32-bit register of the FCH eSPI register block.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (0 - 0xFFF, 4-byte aligned).
  @param[in]  Value     Value to write.
**/
typedef
VOID
(EFIAPI *ESPI_WRITE_U32)(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset,
  IN  UINT32  Value
  );

///
/// Custom eSPI Protocol Structure.
///
struct _ESPI_PROTOCOL {
  //
  // Configuration and capability.
  //
  ESPI_READ_MASTER_CAP          ReadMasterCapabilities;
  ESPI_GET_CONFIGURATION        GetConfiguration;
  ESPI_SET_CONFIGURATION        SetConfiguration;

  //
  // Command dispatch state.
  //
  ESPI_IS_BUSY                  IsBusy;
  ESPI_WAIT_READY               WaitReady;

  //
  // Downstream channels.
  //
  ESPI_SEND_COMMAND             SendCommand;
  ESPI_SEND_FLASH_COMPLETION    SendFlashCompletion;
  ESPI_SEND_OOB_MESSAGE         SendOobMessage;
  ESPI_SEND_VIRTUAL_WIRE        SendVirtualWire;
  ESPI_SEND_PERIPHERAL_MESSAGE  SendPeripheralMessage;
  ESPI_SEND_INDEPENDENT_COMMAND  SendIndependentCommand;

  //
  // Upstream requests.
  //
  ESPI_GET_FLASH_REQUEST        GetFlashRequest;
  ESPI_GET_OOB_REQUEST          GetOobRequest;
  ESPI_ACKNOWLEDGE_REQUEST      AcknowledgeRequest;

  //
  // Raw register access to the controller block, routed through the PCIe
  // ECAM protocol rather than through MmioRead*/MmioWrite* in the callers.
  //
  ESPI_READ_U8                  Read8;
  ESPI_READ_U16                 Read16;
  ESPI_READ_U32                 Read32;

  ESPI_WRITE_U8                 Write8;
  ESPI_WRITE_U16                Write16;
  ESPI_WRITE_U32                Write32;
};

#endif // ESPI_PROTOCOL_H_
