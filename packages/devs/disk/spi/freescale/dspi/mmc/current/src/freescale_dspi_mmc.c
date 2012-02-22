//==========================================================================
//
//      freescale_dspi_mmc.c
//
//      Cortex-M4 Freescale DSPI disk device
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2011 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later
// version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with eCos; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// As a special exception, if other files instantiate templates or use
// macros or inline functions from this file, or you compile this file
// and link it with other works to produce a work based on this file,
// this file does not by itself cause the resulting work to be covered by
// the GNU General Public License. However the source code for this file
// must still be made available in accordance with section (3) of the GNU
// General Public License v2.
//
// This exception does not invalidate any other reasons why a work based
// on this file might be covered by the GNU General Public License.
// -------------------------------------------
// ####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):      ilijak
// Contributor(s):
// Date:           2011-12-27
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_kinetis.h>
#include <pkgconf/devs_disk_mmc_freescale_dspi.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/io/spi_freescale_dspi.h>

#define MMC_SPI_FRAME_SIZE      8
#define MMC_SPI_CLOCK_POL       0
#define MMC_SPI_CLOCK_PHASE     0
#define MMC_SPI_INITIAL_SPEED   CYGHWR_DEVS_MMC_SPI_INITIAL_SPEED
#define MMC_SPI_CSUP_DL         CYGHWR_DEVS_DISK_MMC_CS_DELAY
#define MMC_SPI_CSDW_DL         CYGHWR_DEVS_DISK_MMC_CS_DELAY
#define MMC_SPI_TRBD_DL         CYGHWR_DEVS_DISK_MMC_CS_DELAY
#define MMC_SPI_DELAY_UNIT      CYGHWR_DEVS_DISK_MMC_CS_DELAY_UNIT
#ifdef CYGHWR_DEVS_MMC_SPI_USE_DBR
# define MMC_SPI_DBR 1
#else
# define MMC_SPI_DBR 0
#endif


CYG_DEVS_SPI_FREESCALE_DSPI_DEVICE(
    cyg_spi_mmc_dev0,             // Device name
    CYGHWR_DEVS_DISK_MMC_FREESCALE_DSPI_BUS, // SPI bus
    CYGHWR_DEVS_DISK_MMC_FREESCALE_DSPI_CS,  // Dev num
    MMC_SPI_FRAME_SIZE,           // Frame size
    MMC_SPI_CLOCK_POL,            // Clock pol
    MMC_SPI_CLOCK_PHASE,          // Clock phase
    MMC_SPI_INITIAL_SPEED,        // Clock speed (Hz)
    MMC_SPI_CSUP_DL,              // CS assert delay
    MMC_SPI_CSDW_DL,              // CS negate delay
    MMC_SPI_TRBD_DL,              // Delay between transfers
    MMC_SPI_DELAY_UNIT,           // Delay unit (100 or 1000 ns)
    MMC_SPI_DBR                   // Use double baud rate
);

//==========================================================================
// EOF freescale_spi_disk.c
