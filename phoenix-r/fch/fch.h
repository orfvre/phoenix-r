/** @file
  Custom FCH SPI Protocol definitions for accessing the AMD FCH SPI controller.
**/

#ifndef FCH_PROTOCOL_H_
#define FCH_PROTOCOL_H_

#define PLATFORM_ECAM_BASE  0xF0000000ULL

/**
  Fixed MMIO base address for the AMD FCH SPI controller registers.

  This is the raw APB bus address from the PPR (Table 185).
  Specific to AMD Family 19h Model 11h (Rembrandt).
  Requires Data Fabric MMIO routing to be configured before access.

  @note The below-4GB alias at 0xFEC10000 is preferred for direct access.
    Alternatively, read LPC bridge (B0:D14h:F3) config register 0xA0.
**/
#define SPI_MMIO_ADDRESS    0xFDF95FFE00ULL

/**
  PCI location and layout of the FCH LPC bridge, which owns the SPI controller
  base address register and the SPI ROM decode enables.
**/
#define FCH_LPC_BUS                    0
#define FCH_LPC_DEV                    0x14
#define FCH_LPC_FUNC                   0x03

/**
  LPC bridge configuration register 0xA0 (SPI base address register).

  Bits [31:8] hold the MMIO base of the SPI controller block, the low byte holds
  the SPI ROM chip-select enables.
**/
#define FCH_LPC_SPI_BASE_REGISTER      0xA0
#define FCH_LPC_SPI_BASE_MASK          0xFFFFFF00

/** SPI ROM decode enable for chip select 0 (bit 0 of the SPI base register). **/
#define FCH_SPI_ROM_ENABLE             0x00000001

/** SPI ROM decode enable for the alternate chip select 1 (bit 1 of the SPI base register). **/
#define FCH_SPI_ROM_ALT_ENABLE         0x00000002

typedef struct _FCH_PROTOCOL FCH_PROTOCOL;

/**
  Retrieves the SPI controller base address from the FCH LPC bridge.

  @param[out]  Buffer   Pointer to a caller-allocated UINT32 that receives the SPI base address.

  @retval EFI_SUCCESS            The SPI base address was retrieved successfully.
  @retval EFI_INVALID_PARAMETER  This or Buffer is NULL.
  @retval EFI_DEVICE_ERROR       The PCI read failed.
**/
typedef
EFI_STATUS
(EFIAPI *FCH_GET_LEGACY_SPI_BASE)(
  OUT UINT32  *Buffer
  );

/**
  Retrieves the SPI controller base address from the FCH fixed MMIO address.

  @param[out]  Buffer   Pointer to a caller-allocated UINT32 that receives the SPI base address.

  @retval EFI_SUCCESS            The SPI base address was retrieved successfully.
  @retval EFI_INVALID_PARAMETER  This or Buffer is NULL.
  @retval EFI_DEVICE_ERROR       The read failed.
**/
typedef
EFI_STATUS
(EFIAPI *FCH_GET_SPI_BASE)(
  OUT UINT32  *Buffer
  );

/**
  Retrieves the JEDEC manufacturer and device ID from the SPI flash.

  @param[out]  JedecId   Caller-allocated 3-byte array that receives the JEDEC ID
              (manufacturer, device type, capacity).

  @retval EFI_SUCCESS            The JEDEC ID was read successfully.
  @retval EFI_INVALID_PARAMETER  This or JedecId is NULL.
  @retval EFI_DEVICE_ERROR       The SPI transaction failed.
**/
typedef
EFI_STATUS
(EFIAPI *FCH_GET_SPI_FLASH_ID)(
  OUT UINT8            JedecId[3]
  );

/**
  Enables or disables the SPI ROM decode of the FCH LPC bridge.

  Only the two chip-select enable bits of the SPI base address register
  (B0:D14h:F3, offset 0xA0h) are touched; the base address itself and every other
  bit of the register are preserved.

  @param[in]  Enable   Bit mask of SPI ROM chip selects to enable:
             FCH_SPI_ROM_ENABLE (CS0), FCH_SPI_ROM_ALT_ENABLE (CS1).
             Pass 0 to disable both decodes.

  @retval EFI_SUCCESS            The decode bits were updated.
  @retval EFI_DEVICE_ERROR       The PCI configuration read or write failed.
**/
typedef
EFI_STATUS
(EFIAPI *FCH_SET_SPI_ROM_ENABLE)(
  IN  UINT32           Enable
  );

/**
  Reads the chip select that currently drives the SPI flash.

  The boot chip select of the flash is reported by SPIx1D[1:0] (AltSpiCs). It is
  only meaningful for platforms with dual SPI support; single-flash designs
  report 0.

  @param[out]  ChipSelect   Pointer to a caller-allocated UINT8 that receives the
              active chip select (0 - 3).

  @retval EFI_SUCCESS            The chip select was read successfully.
  @retval EFI_INVALID_PARAMETER  ChipSelect is NULL.
  @retval EFI_DEVICE_ERROR       The SPI base address could not be resolved.
**/
typedef
EFI_STATUS
(EFIAPI *FCH_GET_SPI_CHIP_SELECT)(
  OUT UINT8  *ChipSelect
  );


///
/// Custom FCH Protocol Structure.
///
struct _FCH_PROTOCOL {
  FCH_GET_LEGACY_SPI_BASE     GetLegacySpiBase;
  FCH_GET_SPI_BASE            GetSpiBase;
  FCH_GET_SPI_FLASH_ID        GetSpiFlashId;
  FCH_SET_SPI_ROM_ENABLE      SetSpiRomEnable;
  FCH_GET_SPI_CHIP_SELECT     GetSpiChipSelect;
};

#endif // FCH_PROTOCOL_H_
