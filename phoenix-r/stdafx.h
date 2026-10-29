/** @file
  Precompiled header for the phoenix-r UEFI driver.

  Aggregates the UEFI base headers, boot/runtime service tables, shell
  support, and the custom protocol definitions used across the driver.
**/

#ifndef STDAFX_H_
#define STDAFX_H_

//
// Basic UEFI libraries
//
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>

//
// Boot and runtime services
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IoLib.h>

//
// Shell library
//

#include <Library/ShellLib.h>

//
// EDK2 protocols / register definitions
//

#include <PiDxe.h>

//
// Custom Structures
//

#include "structure/spi.h"
#include "structure/espi.h"

//
// Custom Protocols
//

#include "pci/pci.h"

extern PCI_PROTOCOL  gPciProtocol;

#include "pcie/pcie.h"

extern PCIE_PROTOCOL  gPcieProtocol;

#include "fch/fch.h"

extern FCH_PROTOCOL  gFchProtocol;

#include "spi/spi.h"

extern SPI_PROTOCOL  gSpiProtocol;

#include "espi/espi.h"

extern ESPI_PROTOCOL gESpiProtocol;

#endif // STDAFX_H_
