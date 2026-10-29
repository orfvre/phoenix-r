# phoenix-r

A UEFI driver that reimplements PCI, PCIe, SPI, and eSPI access from scratch, bypassing the standard EDK2 protocols entirely.

## Why

The EDK2 specification defines protocols for PCI I/O, SPI flash, and eSPI access (`EFI_PCI_IO_PROTOCOL`, `EFI_SPI_IO_PROTOCOL`, etc.), but on certain AMD platforms these protocols are either not installed by the firmware or gated behind proprietary interfaces that are not publicly documented. phoenix-r solves the problem by going straight to the hardware: legacy I/O ports for PCI configuration space, ECAM MMIO for PCIe extended configuration space, and direct FCH register manipulation for SPI and eSPI.

This project also served as a learning exercise for the EDK2 C coding standards, PCI, PCIE, MMIO manipulation and UEFI driver development in general hence the thorough Doxygen documentation on every function, structure, and register field.

## Target Platform

**AMD Family 19h Model 11h (Rembrandt).**

The SPI/eSPI register offsets, the PSP/HFP ownership arbitration sequence, ECAM base addresses, and FCH LPC bridge locations are all specific to this silicon revision. Other AMD families and revisions will have different register layouts, offsets, and behaviors.

> **If you are targeting a different platform**, consult the corresponding AMD Processor Programming Reference (PPR) for your family and model, and update the register definitions in `structure/` and the constants in `fch/`, `spi/`, and `espi/` accordingly.

## What's Implemented

| Module | Description |
|---|---|
| `pci/` | Legacy PCI configuration space access via I/O ports CF8h/CFCh. 8, 16, 32, and 64-bit reads and writes with write-back verification. |
| `pcie/` | PCIe extended configuration space access via ECAM MMIO. Same access widths as PCI, mapped through the ECAM base address. |
| `spi/` | AMD FCH SPI controller driver. Host-BIOS ownership arbitration against the PSP and HFP, command-mode FIFO transfers, JEDEC identification, status register polling, flash read, page program, and sector erase. |
| `espi/` | AMD FCH eSPI master driver. All five eSPI channels (peripheral, virtual wire, OOB, flash, independent), downstream command dispatch, upstream flash and OOB request handling, and slave configuration. |
| `fch/` | High-level FCH helpers. SPI base address resolution (both LPC bridge and fixed MMIO), JEDEC flash ID through the FCH protocol, boot chip-select readback, and SPI ROM decode enable control. |
| `structure/` | Hardware register bitfield definitions transcribed from the AMD PPR. Union-of-struct layouts for every SPI and eSPI controller register, with compile-time size assertions. |

## Building

phoenix-r is built with [VisualUefi](https://github.com/ionescu007/VisualUefi) under Visual Studio. The `.vcxproj` file is included in the repository.

**Prerequisites:**

- Visual Studio with the C/C++ workload
- VisualUefi (provides the EDK2 MdePkg headers: `Uefi.h`, `UefiLib.h`, `IoLib.h`, `PiDxe.h`, etc.)

Open `phoenix-r.vcxproj` in Visual Studio and build. The output is a `.efi` binary that can be loaded from the UEFI Shell or integrated into a firmware image as a DXE driver.

## Usage

From the UEFI Shell:

```
Shell> load phoenix-r.efi
```

The driver entry point enumerates the SPI flash (base address, chip select, JEDEC ID) and prints basic platform information to the console.

## Warning

**The SPI write and erase functions can permanently brick your motherboard.**

`SpiWriteDataImpl` and `SpiEraseSectorImpl` program and erase the live SPI flash the same flash that holds your system firmware. A bad write to the wrong address will make the platform unbootable, and recovery requires an external SPI programmer.

**I am not responsible for any bricked motherboards, lost data, hardware damage, or any other consequences resulting from the use of this software.** This driver talks directly to hardware registers with no firmware safety net. Use it at your own risk.

## Platform-Specific Values

Many constants in this codebase are specific to AMD Family 19h Model 11h (Rembrandt) and **will not be correct** for other platforms. These include but are not limited to:

- SPI controller MMIO base address (`SPI_MMIO_ADDRESS`, `SPI_MMIO_BASE`)
- eSPI link base addresses (`ESPI_LINK0_MMIO_BASE`, `ESPI_LINK1_MMIO_BASE`)
- FCH LPC bridge BDF and register offsets (`FCH_LPC_BUS`, `FCH_LPC_DEV`, `FCH_LPC_FUNC`)
- ECAM base address (`PLATFORM_ECAM_BASE`)
- SPI register bitfield layouts in `structure/spi.h` and `structure/espi.h`
- PSP/HFP ownership bits in `SPI_MISCCNTRL_REGISTER`

**Refer to the AMD Processor Programming Reference (PPR) for your specific family and model** to determine the correct values before running this on any other hardware.

## References

- **AMD PPR** — Family 19h Model 11h Rev B2, Volume 5, Section 10.3.8 (SPI Controller), Section 10.3.8.2 (eSPI Controller)
- **OpenSecurityTraining2** — [Architecture 4001: x86-64 Intel Firmware Attack & Defense](https://opensecuritytraining.info/)
- **UEFI Specification** — Version 2.10
- **EDK II C Coding Standards Specification** — Revision 2.4
- **VisualUefi** — [github.com/ionescu007/VisualUefi](https://github.com/ionescu007/VisualUefi)
