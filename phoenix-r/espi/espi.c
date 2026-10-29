/** @file
  eSPI Protocol implementation for the AMD FCH eSPI controller.

  Raw register access, configuration, downstream command dispatch across all
  five channels, and upstream flash/OOB request handling for the FCH eSPI
  master block.

  @note  AMD PPR Family 19h Model 11h Rev B2, Volume 5, Section 10.3.8.2.
**/

//
// Raw register access, exposed as ESPI_PROTOCOL.Read8/16/32 and
// Write8/16/32.
//
// The FCH eSPI register block is reachable through the same 4 KiB ECAM window
// as the SPI controller: PCIE_ECAM_ADDR() with Bus/Dev/Func = 0 reduces to
// Base + Offset, so the PCIe protocol's primitives serve as the MMIO path for
// the eSPI controller and no caller has to touch MmioRead*/MmioWrite*
// directly.
//
// The write status is deliberately dropped here for the same reason as on the
// SPI side: the PCIe protocol verifies a write by reading the register back,
// which is meaningless for registers with read-only or write-1-to-clear bits
// and for the data-port FIFOs.
//

/**
  Reads an 8-bit eSPI controller register.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase.

  @return The register value; 0xFF when the access was rejected.
**/
UINT8
EFIAPI
EspiRegRead8Impl(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset
)
{
  UINT8  Value;

  Value = 0xFF;
  gPcieProtocol.Read8(EspiBase, 0, 0, 0, Offset, &Value);

  return Value;
}

/**
  Reads a 16-bit eSPI controller register.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (2-byte aligned).

  @return The register value; 0xFFFF when the access was rejected.
**/
UINT16
EFIAPI
EspiRegRead16Impl(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset
)
{
  UINT16  Value;

  Value = 0xFFFF;
  gPcieProtocol.Read16(EspiBase, 0, 0, 0, Offset, &Value);

  return Value;
}

/**
  Reads a 32-bit eSPI controller register.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (4-byte aligned).

  @return The register value; 0xFFFFFFFF when the access was rejected.
**/
UINT32
EFIAPI
EspiRegRead32Impl(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset
)
{
  UINT32  Value;

  Value = 0xFFFFFFFF;
  gPcieProtocol.Read32(EspiBase, 0, 0, 0, Offset, &Value);

  return Value;
}

/**
  Writes an 8-bit eSPI controller register.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase.
  @param[in]  Value     Value to write.
**/
VOID
EFIAPI
EspiRegWrite8Impl(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset,
  IN  UINT8   Value
)
{
  gPcieProtocol.Write8(EspiBase, 0, 0, 0, Offset, &Value);
}

/**
  Writes a 16-bit eSPI controller register.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (2-byte aligned).
  @param[in]  Value     Value to write.
**/
VOID
EFIAPI
EspiRegWrite16Impl(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset,
  IN  UINT16  Value
)
{
  gPcieProtocol.Write16(EspiBase, 0, 0, 0, Offset, &Value);
}

/**
  Writes a 32-bit eSPI controller register.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.
  @param[in]  Offset    Register offset from EspiBase (4-byte aligned).
  @param[in]  Value     Value to write.
**/
VOID
EFIAPI
EspiRegWrite32Impl(
  IN  UINTN   EspiBase,
  IN  UINT16  Offset,
  IN  UINT32  Value
)
{
  gPcieProtocol.Write32(EspiBase, 0, 0, 0, Offset, &Value);
}

/**
  Waits ESPI_POLL_INTERVAL_US before the next controller status poll.

  Uses the boot service stall so the delay is real time; when the boot services
  table is not established yet the loop simply polls back to back.
**/
VOID
EspiPollDelay(
  VOID
)
{
  if ((gBS != NULL) && (gBS->Stall != NULL))
  {
    gBS->Stall(ESPI_POLL_INTERVAL_US);
  }
}

/**
  Waits until the eSPI controller has finished dispatching the previous
  downstream command.

  DNCMD_STATUS (ESPIx00[3]) is write-0-only: software sets it to launch a
  packet, and hardware clears it once the packet has been sent down.  A clear
  bit therefore means the controller is free to accept the next command.

  @param[in]  EspiBase   MMIO base address of the FCH eSPI register block.
  @param[in]  TimeoutMs  Maximum time to wait, in milliseconds.

  @retval EFI_SUCCESS  The command status bit cleared.
  @retval EFI_TIMEOUT  The status bit stayed set for the whole timeout.
**/
EFI_STATUS
EspiWaitIdle(
  IN  UINTN  EspiBase,
  IN  UINTN  TimeoutMs
)
{
  UINTN                   Iterations;
  ESPI_DN_TXHDR_0TH_REGISTER  Hdr0;

  Iterations = TimeoutMs * (1000 / ESPI_POLL_INTERVAL_US);
  while (Iterations > 0)
  {
    Hdr0.Value = EspiRegRead32Impl(EspiBase, ESPI_DN_TXHDR_0TH_OFFSET);
    if (!Hdr0.Bits.DncmdStatus)
    {
      return EFI_SUCCESS;
    }

    Iterations--;
    EspiPollDelay();
  }

  return EFI_TIMEOUT;
}

/**
  Programs the DN_TXHDR registers, pushes the data payload, and launches the
  packet (internal helper).

  The three header registers and the data port are programmed first, and the
  DN_TXHDR_0 register -- which carries the command type, HDATA0..2, and the
  DNCMD_STATUS trigger -- is written once at the very end with the status bit
  set, so the launch is the last thing the controller sees.

  @param[in]  EspiBase     MMIO base address of the FCH eSPI register block.
  @param[in]  CommandType  One of ESPI_DNCMD_*.
  @param[in]  Header       Pointer to 8 bytes packed into DNCMD_HDATA0..7.
  @param[in]  Data         Pointer to the payload bytes; NULL when DataLength is 0.
  @param[in]  DataLength   Number of payload bytes.

  @retval EFI_SUCCESS  The command was dispatched.
  @retval EFI_TIMEOUT  The controller stayed busy.
**/
EFI_STATUS
EspiSendCommandRaw(
  IN  UINTN        EspiBase,
  IN  UINT8        CommandType,
  IN  CONST UINT8  *Header,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
)
{
  EFI_STATUS                 Status;
  UINTN                      Index;
  UINTN                      Byte;
  UINT32                     Dw;
  ESPI_DN_TXHDR_0TH_REGISTER Hdr0;
  ESPI_DN_TXHDR_1_REGISTER   Hdr1;
  ESPI_DN_TXHDR_2_REGISTER   Hdr2;

  Status = EspiWaitIdle(EspiBase, ESPI_CMD_TIMEOUT_MS);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  //
  // HDATA3..6 live in DN_TXHDR_1, HDATA7 in DN_TXHDR_2.
  //
  Hdr1.Value = 0;
  Hdr1.Bits.DncmdHdata3 = Header[3];
  Hdr1.Bits.DncmdHdata4 = Header[4];
  Hdr1.Bits.DncmdHdata5 = Header[5];
  Hdr1.Bits.DncmdHdata6 = Header[6];
  EspiRegWrite32Impl(EspiBase, ESPI_DN_TXHDR_1_OFFSET, Hdr1.Value);

  Hdr2.Value = 0;
  Hdr2.Bits.DncmdHdata7 = Header[7];
  EspiRegWrite32Impl(EspiBase, ESPI_DN_TXHDR_2_OFFSET, Hdr2.Value);

  //
  // The data port is a push FIFO: every 32-bit write advances by one DW.
  // Payload bytes are packed least significant byte first; a trailing partial
  // DW is zero padded.
  //
  for (Index = 0; Index < DataLength; Index += 4)
  {
    Dw = 0;
    for (Byte = 0; (Byte < 4) && ((Index + Byte) < DataLength); Byte++)
    {
      Dw |= (UINT32)Data[Index + Byte] << (Byte * 8);
    }
    EspiRegWrite32Impl(EspiBase, ESPI_DN_TXDATA_PORT_OFFSET, Dw);
  }

  //
  // DNCMD_STATUS is the trigger and must be set last; it rides in the same
  // register as the command type and HDATA0..2, so one final write launches
  // the whole packet.
  //
  Hdr0.Value = 0;
  Hdr0.Bits.DncmdType   = CommandType;
  Hdr0.Bits.DncmdHdata0 = Header[0];
  Hdr0.Bits.DncmdHdata1 = Header[1];
  Hdr0.Bits.DncmdHdata2 = Header[2];
  Hdr0.Bits.DncmdStatus = 1;
  EspiRegWrite32Impl(EspiBase, ESPI_DN_TXHDR_0TH_OFFSET, Hdr0.Value);

  return EFI_SUCCESS;
}

/**
  Reads the fixed master capability register (ESPIx2C).

  @param[in]   EspiBase      MMIO base address of the FCH eSPI register block.
  @param[out]  Capabilities  Pointer to a caller-allocated register union that
                receives ESPIx2C.

  @retval EFI_SUCCESS            The capabilities were read.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Capabilities is NULL.
**/
EFI_STATUS
EFIAPI
EspiReadMasterCapImpl(
  IN   UINTN                    EspiBase,
  OUT  ESPI_MASTER_CAP_REGISTER *Capabilities
)
{
  if ((EspiBase == 0) || (Capabilities == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Capabilities->Value = EspiRegRead32Impl(EspiBase, ESPI_MASTER_CAP_OFFSET);

  return EFI_SUCCESS;
}

/**
  Reads the current slave 0 configuration (ESPIx68).

  @param[in]   EspiBase       MMIO base address of the FCH eSPI register block.
  @param[out]  Configuration  Pointer to a caller-allocated register union that
                receives ESPIx68.

  @retval EFI_SUCCESS            The configuration was read.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Configuration is NULL.
**/
EFI_STATUS
EFIAPI
EspiGetConfigurationImpl(
  IN   UINTN                      EspiBase,
  OUT  ESPI_SLAVE0_CONFIG_REGISTER *Configuration
)
{
  if ((EspiBase == 0) || (Configuration == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Configuration->Value = EspiRegRead32Impl(EspiBase, ESPI_SLAVE0_CONFIG_OFFSET);

  return EFI_SUCCESS;
}

/**
  Writes the slave 0 configuration (ESPIx68).

  @param[in]  EspiBase       MMIO base address of the FCH eSPI register block.
  @param[in]  Configuration  Pointer to a caller-initialized register union to
                program into ESPIx68.

  @retval EFI_SUCCESS            The configuration was written.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Configuration is NULL.
**/
EFI_STATUS
EFIAPI
EspiSetConfigurationImpl(
  IN  UINTN                      EspiBase,
  IN  CONST ESPI_SLAVE0_CONFIG_REGISTER *Configuration
)
{
  if ((EspiBase == 0) || (Configuration == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  EspiRegWrite32Impl(EspiBase, ESPI_SLAVE0_CONFIG_OFFSET, Configuration->Value);

  return EFI_SUCCESS;
}

/**
  Reports whether a downstream command is still being dispatched.

  @param[in]   EspiBase  MMIO base address of the FCH eSPI register block.
  @param[out]  Busy      Pointer to a caller-allocated BOOLEAN that receives the
            DNCMD_STATUS state.

  @retval EFI_SUCCESS            The busy state was read.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Busy is NULL.
**/
EFI_STATUS
EFIAPI
EspiIsBusyImpl(
  IN   UINTN    EspiBase,
  OUT  BOOLEAN  *Busy
)
{
  ESPI_DN_TXHDR_0TH_REGISTER  Hdr0;

  if ((EspiBase == 0) || (Busy == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Hdr0.Value = EspiRegRead32Impl(EspiBase, ESPI_DN_TXHDR_0TH_OFFSET);
  *Busy = (BOOLEAN)(Hdr0.Bits.DncmdStatus != 0);

  return EFI_SUCCESS;
}

/**
  Waits until the previous downstream command has been fully dispatched.

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.

  @retval EFI_SUCCESS            The controller is idle (DNCMD_STATUS clear).
  @retval EFI_INVALID_PARAMETER  EspiBase is 0.
  @retval EFI_TIMEOUT            DNCMD_STATUS never cleared.
**/
EFI_STATUS
EFIAPI
EspiWaitReadyImpl(
  IN  UINTN  EspiBase
)
{
  if (EspiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  return EspiWaitIdle(EspiBase, ESPI_CMD_TIMEOUT_MS);
}

/**
  Sends one downstream eSPI command (the raw primitive).

  @param[in]  EspiBase     MMIO base address of the FCH eSPI register block.
  @param[in]  CommandType  One of ESPI_DNCMD_*.
  @param[in]  Header       Pointer to 8 bytes packed into DNCMD_HDATA0..7.
  @param[in]  Data         Pointer to the payload bytes; NULL when DataLength is 0.
  @param[in]  DataLength   Number of payload bytes.

  @retval EFI_SUCCESS            The command was dispatched.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, Header is NULL, CommandType is
                 not an ESPI_DNCMD_* value, a buffer is NULL while
                 its length is not 0, or the payload exceeds 128 bytes.
  @retval EFI_TIMEOUT            The previous command never drained.
**/
EFI_STATUS
EFIAPI
EspiSendCommandImpl(
  IN  UINTN        EspiBase,
  IN  UINT8        CommandType,
  IN  CONST UINT8  *Header,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
)
{
  if ((EspiBase == 0) || (Header == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (CommandType > ESPI_DNCMD_FLASH_CPL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if ((DataLength != 0) && (Data == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (DataLength > ESPI_FLASH_MAX_PAYLOAD)
  {
    return EFI_INVALID_PARAMETER;
  }

  return EspiSendCommandRaw(EspiBase, CommandType, Header, Data, DataLength);
}

/**
  Answers a slave flash read with a Flash channel completion.

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
EFI_STATUS
EFIAPI
EspiSendFlashCompletionImpl(
  IN  UINTN        EspiBase,
  IN  UINT8        CycleType,
  IN  UINT8        Tag,
  IN  UINT16       Length,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
)
{
  UINT8  Header[8];

  if (EspiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (DataLength != Length)
  {
    return EFI_INVALID_PARAMETER;
  }

  if ((Length != 0) && (Data == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Flash completion header: HDATA0 = cycle type, HDATA1 = tag and the upper
  // length nibble, HDATA2 = the low length byte.  HDATA3..7 are reserved.
  //
  Header[0] = CycleType;
  Header[1] = (UINT8)(((Tag & 0x0F) << 4) | ((Length >> 8) & 0x0F));
  Header[2] = (UINT8)(Length & 0xFF);
  Header[3] = 0;
  Header[4] = 0;
  Header[5] = 0;
  Header[6] = 0;
  Header[7] = 0;

  return EspiSendCommandRaw(EspiBase, ESPI_DNCMD_FLASH_CPL, Header, Data, DataLength);
}

/**
  Sends a tunneled SMBus message downstream on the OOB channel.

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
EFI_STATUS
EFIAPI
EspiSendOobMessageImpl(
  IN  UINTN        EspiBase,
  IN  UINT8        Tag,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Command,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
)
{
  UINT8  Header[8];

  if (EspiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  if ((DataLength != 0) && (Data == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (DataLength > ESPI_OOB_MAX_PAYLOAD)
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // OOB header: HDATA0 = tunneled-SMBus cycle type, HDATA1/2 = tag + 12-bit
  // length, HDATA3 = slave address, HDATA4 = command, HDATA5 = byte count.
  //
  Header[0] = ESPI_CYCLETYPE_TUNNELED_SMBUS;
  Header[1] = (UINT8)(((Tag & 0x0F) << 4) | ((DataLength >> 8) & 0x0F));
  Header[2] = (UINT8)(DataLength & 0xFF);
  Header[3] = SlaveAddress;
  Header[4] = Command;
  Header[5] = (UINT8)DataLength;
  Header[6] = 0;
  Header[7] = 0;

  return EspiSendCommandRaw(EspiBase, ESPI_DNCMD_OOB_DOWN, Header, Data, DataLength);
}

/**
  Sends virtual-wire values downstream on the VW channel.

  @param[in]  EspiBase    MMIO base address of the FCH eSPI register block.
  @param[in]  GroupCount  Number of virtual-wire groups (<= ESPI_VW_MAX_GROUPS).
  @param[in]  VwData      Pointer to GroupCount bytes, one value per group.

  @retval EFI_SUCCESS            The virtual-wire values were sent.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, VwData is NULL, or GroupCount
                 exceeds ESPI_VW_MAX_GROUPS.
  @retval EFI_TIMEOUT            The controller stayed busy.
**/
EFI_STATUS
EFIAPI
EspiSendVirtualWireImpl(
  IN  UINTN        EspiBase,
  IN  UINTN        GroupCount,
  IN  CONST UINT8  *VwData
)
{
  UINT8  Header[8];

  if ((EspiBase == 0) || (VwData == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (GroupCount > ESPI_VW_MAX_GROUPS)
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // VW header: HDATA0[5:0] = number of virtual-wire groups carried in the
  // data port.  Every other header byte is reserved.
  //
  Header[0] = (UINT8)GroupCount;
  Header[1] = 0;
  Header[2] = 0;
  Header[3] = 0;
  Header[4] = 0;
  Header[5] = 0;
  Header[6] = 0;
  Header[7] = 0;

  return EspiSendCommandRaw(EspiBase, ESPI_DNCMD_VW_DOWN, Header, VwData, GroupCount);
}

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
EFI_STATUS
EFIAPI
EspiSendPeripheralMessageImpl(
  IN  UINTN        EspiBase,
  IN  UINT8        CycleType,
  IN  UINT8        Tag,
  IN  UINT16       Length,
  IN  UINT8        MessageCode,
  IN  CONST UINT8  *SpecificBytes,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
)
{
  UINT8  Header[8];

  if ((EspiBase == 0) || (SpecificBytes == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if ((DataLength != 0) && (Data == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (DataLength > ESPI_PERIPH_MAX_PAYLOAD)
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Peripheral message header: HDATA0 = cycle type, HDATA1/2 = tag + 12-bit
  // length, HDATA3 = message code, HDATA4..7 = message-specific bytes.
  //
  Header[0] = CycleType;
  Header[1] = (UINT8)(((Tag & 0x0F) << 4) | ((Length >> 8) & 0x0F));
  Header[2] = (UINT8)(Length & 0xFF);
  Header[3] = MessageCode;
  Header[4] = SpecificBytes[0];
  Header[5] = SpecificBytes[1];
  Header[6] = SpecificBytes[2];
  Header[7] = SpecificBytes[3];

  return EspiSendCommandRaw(EspiBase, ESPI_DNCMD_PERIPH_MESSAGE, Header, Data, DataLength);
}

/**
  Sends one Independent channel command: Set/Get Configuration or In-band reset.

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
EFI_STATUS
EFIAPI
EspiSendIndependentCommandImpl(
  IN  UINTN        EspiBase,
  IN  UINT8        CommandType,
  IN  UINT16       Address,
  IN  CONST UINT8  *Data,
  IN  UINTN        DataLength
)
{
  UINT8  Header[8];

  if (EspiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (CommandType > ESPI_DNCMD_INBAND_RESET)
  {
    return EFI_INVALID_PARAMETER;
  }

  if ((DataLength != 0) && (Data == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if (DataLength > 4)
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Independent command header: HDATA0 = address[11:8], HDATA1 = address[7:0],
  // HDATA3..6 = up to four bytes of configuration data.  For the in-band
  // RESET command the address and data bits are ignored by the controller.
  //
  Header[0] = (UINT8)((Address >> 8) & 0x0F);
  Header[1] = (UINT8)(Address & 0xFF);
  Header[2] = 0;
  Header[3] = (DataLength > 0) ? Data[0] : 0;
  Header[4] = (DataLength > 1) ? Data[1] : 0;
  Header[5] = (DataLength > 2) ? Data[2] : 0;
  Header[6] = (DataLength > 3) ? Data[3] : 0;
  Header[7] = 0;

  return EspiSendCommandRaw(EspiBase, CommandType, Header, NULL, 0);
}

/**
  Reads a pending slave flash read request (GET_FLASH_NP).

  @param[in]   EspiBase  MMIO base address of the FCH eSPI register block.
  @param[out]  Request   Pointer to a caller-allocated ESPI_FLASH_REQUEST that
            receives the address, tag, length, and cycle type.

  @retval EFI_SUCCESS            A flash request was read.
  @retval EFI_NOT_READY          No flash request is pending.
  @retval EFI_UNSUPPORTED        The pending packet is not a flash request.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0, or Request is NULL.
**/
EFI_STATUS
EFIAPI
EspiGetFlashRequestImpl(
  IN   UINTN               EspiBase,
  OUT  ESPI_FLASH_REQUEST  *Request
)
{
  ESPI_UP_RXHDR_0_REGISTER  Hdr0;
  ESPI_UP_RXHDR_1_REGISTER  Hdr1;

  if ((EspiBase == 0) || (Request == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Hdr0.Value = EspiRegRead32Impl(EspiBase, ESPI_UP_RXHDR_0_OFFSET);
  if (!Hdr0.Bits.UpcmdStatus)
  {
    return EFI_NOT_READY;
  }

  if (Hdr0.Bits.UpcmdType != ESPI_UPCMD_FLASH_REQUEST)
  {
    return EFI_UNSUPPORTED;
  }

  Hdr1.Value = EspiRegRead32Impl(EspiBase, ESPI_UP_RXHDR_1_OFFSET);

  Request->CycleType = (UINT8)Hdr0.Bits.UpcmdHdata0;
  Request->Tag       = (UINT8)((Hdr0.Bits.UpcmdHdata1 >> 4) & 0x0F);
  Request->Length    = (UINT16)(((Hdr0.Bits.UpcmdHdata1 & 0x0F) << 8) | Hdr0.Bits.UpcmdHdata2);

  //
  // The flash address is stored byte-reversed in UP_RXHDR_1: HDATA3 holds
  // address[31:24], HDATA4 address[23:16], HDATA5 address[15:8], HDATA6
  // address[7:0].
  //
  Request->Address = ((UINT32)Hdr1.Bits.UpcmdHdata3 << 24) |
                     ((UINT32)Hdr1.Bits.UpcmdHdata4 << 16) |
                     ((UINT32)Hdr1.Bits.UpcmdHdata5 << 8)  |
                     ((UINT32)Hdr1.Bits.UpcmdHdata6);

  return EFI_SUCCESS;
}

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
EFI_STATUS
EFIAPI
EspiGetOobRequestImpl(
  IN      UINTN            EspiBase,
  OUT     ESPI_OOB_REQUEST *Request,
  OUT     UINT8            *Data,
  IN OUT  UINTN            *DataLength
)
{
  EFI_STATUS                Status;
  UINTN                     Capacity;
  UINTN                     Count;
  UINTN                     Index;
  UINTN                     Byte;
  UINT32                    Dw;
  ESPI_UP_RXHDR_0_REGISTER  Hdr0;
  ESPI_UP_RXHDR_1_REGISTER  Hdr1;

  if ((EspiBase == 0) || (Request == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Hdr0.Value = EspiRegRead32Impl(EspiBase, ESPI_UP_RXHDR_0_OFFSET);
  if (!Hdr0.Bits.UpcmdStatus)
  {
    return EFI_NOT_READY;
  }

  if (Hdr0.Bits.UpcmdType != ESPI_UPCMD_OOB_MESSAGE)
  {
    return EFI_UNSUPPORTED;
  }

  Hdr1.Value = EspiRegRead32Impl(EspiBase, ESPI_UP_RXHDR_1_OFFSET);

  Request->CycleType    = (UINT8)Hdr0.Bits.UpcmdHdata0;
  Request->Tag          = (UINT8)((Hdr0.Bits.UpcmdHdata1 >> 4) & 0x0F);
  Request->Length       = (UINT16)(((Hdr0.Bits.UpcmdHdata1 & 0x0F) << 8) | Hdr0.Bits.UpcmdHdata2);
  Request->SlaveAddress = (UINT8)Hdr1.Bits.UpcmdHdata3;
  Request->Command      = (UINT8)Hdr1.Bits.UpcmdHdata4;
  Request->ByteCount    = (UINT8)Hdr1.Bits.UpcmdHdata5;

  if ((Data == NULL) || (DataLength == NULL))
  {
    return EFI_SUCCESS;
  }

  Capacity = *DataLength;
  if (Capacity < Request->ByteCount)
  {
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // The RX data port is a pop FIFO: every 32-bit read advances by one DW,
  // matching the push side of DN_TXDATA_PORT.
  //
  Count = 0;
  Status = EFI_SUCCESS;
  for (Index = 0; Index < Request->ByteCount; Index += 4)
  {
    Dw = EspiRegRead32Impl(EspiBase, ESPI_UP_RXDATA_PORT_OFFSET);
    for (Byte = 0; (Byte < 4) && (Count < Request->ByteCount); Byte++, Count++)
    {
      Data[Count] = (UINT8)(Dw >> (Byte * 8));
    }
  }

  *DataLength = Count;

  return Status;
}

/**
  Clears the upstream request valid bit (UPCMD_STATUS, W1C).

  @param[in]  EspiBase  MMIO base address of the FCH eSPI register block.

  @retval EFI_SUCCESS            The valid bit was cleared.
  @retval EFI_INVALID_PARAMETER  EspiBase is 0.
**/
EFI_STATUS
EFIAPI
EspiAcknowledgeRequestImpl(
  IN  UINTN  EspiBase
)
{
  ESPI_UP_RXHDR_0_REGISTER  Hdr0;

  if (EspiBase == 0)
  {
    return EFI_INVALID_PARAMETER;
  }

  //
  // UPCMD_STATUS is write-1-to-clear: writing 1 to the bit clears the valid
  // flag and lets the controller surface the next GET_FLASH_NP / GET_OOB.
  // All other fields of UP_RXHDR_0 are read-only, so they are written as 0.
  //
  Hdr0.Value = 0;
  Hdr0.Bits.UpcmdStatus = 1;
  EspiRegWrite32Impl(EspiBase, ESPI_UP_RXHDR_0_OFFSET, Hdr0.Value);

  return EFI_SUCCESS;
}

///
/// Global instance of the eSPI Protocol.
///
GLOBAL_REMOVE_IF_UNREFERENCED ESPI_PROTOCOL  gESpiProtocol = {
  EspiReadMasterCapImpl,
  EspiGetConfigurationImpl,
  EspiSetConfigurationImpl,

  EspiIsBusyImpl,
  EspiWaitReadyImpl,

  EspiSendCommandImpl,
  EspiSendFlashCompletionImpl,
  EspiSendOobMessageImpl,
  EspiSendVirtualWireImpl,
  EspiSendPeripheralMessageImpl,
  EspiSendIndependentCommandImpl,

  EspiGetFlashRequestImpl,
  EspiGetOobRequestImpl,
  EspiAcknowledgeRequestImpl,

  EspiRegRead8Impl,
  EspiRegRead16Impl,
  EspiRegRead32Impl,

  EspiRegWrite8Impl,
  EspiRegWrite16Impl,
  EspiRegWrite32Impl,
};
