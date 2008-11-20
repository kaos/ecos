##=============================================================================
##
##	plf.inc
##
##	m5272c3 assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 2003,2006,2008 Free Software Foundation, Inc.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
##
## As a special exception, if other files instantiate templates or use macros
## or inline functions from this file, or you compile this file and link it
## with other works to produce a work based on this file, this file does not
## by itself cause the resulting work to be covered by the GNU General Public
## License. However the source code for this file must still be made available
## in accordance with section (3) of the GNU General Public License.
##
## This exception does not invalidate any other reasons why a work based on
## this file might be covered by the GNU General Public License.
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	bartv
## Date:	2003-06-04
######DESCRIPTIONEND####
##=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k_m5272c3.h>
#include <cyg/hal/var_io.h>

// ----------------------------------------------------------------------------
// Various constants to poke into coprocessor or SIM registers
//
// MBAR controls the location of the System Integration Module,
// i.e. all the on-chip hardware. Conventionally this is at
// location 0x10000000. Obviously it cannot contain code.
	.equ _HAL_MCF5272_MBAR_VALUE_, (HAL_MCFxxxx_MBAR + HAL_MCF5272_MBAR_SC + HAL_MCF5272_MBAR_UC + HAL_MCF5272_MBAR_V)

// Chip select 0 is connected to 2MB of flash @ 0xFFE00000.
// This chip is 16 bits wide and requires 6 wait states according
// to the docs. (dBUG uses 8 wait states initially, 5 later on).
// 5 seems to work fine.
	.equ _HAL_MCF5272_CSBR0_VALUE_, (0xFFE00000 + HAL_MCF5272_CSBR_EBI_16_32 + HAL_MCF5272_CSBR_BW_16 + HAL_MCF5272_CSBR_ENABLE)
	.equ _HAL_MCF5272_CSOR0_VALUE_, (0xFFE00000 + (0x05 << HAL_MCF5272_CSOR_WS_SHIFT))

// The board can be populated with sram on cs2, but for now assume a bare board.

// Chip select 7 is connected to 4MB of SDRAM, which should be @ 0x0
	.equ _HAL_MCF5272_CSBR7_VALUE_, (0x00000000 + HAL_MCF5272_CSBR_EBI_SDRAM + HAL_MCF5272_CSBR_BW_CACHELINE + HAL_MCF5272_CSBR_ENABLE)
	.equ _HAL_MCF5272_CSOR7_VALUE_, (0xFFC00000 + (0x1F << HAL_MCF5272_CSOR_WS_SHIFT))

// SDRAM control registers
	.equ _HAL_MCF5272_SDCR_VALUE_, (HAL_MCF5272_SDCR_MCAS_A9 + HAL_MCF5272_SDCR_BALOC_A22_A21 + HAL_MCF5272_SDCR_REG + HAL_MCF5272_SDCR_INIT)
#if defined(CYGHWR_HAL_SYSTEM_CLOCK_HZ_66000000)
	.equ _HAL_MCF5272_SDTR_VALUE_, (HAL_MCF5272_SDTR_RTP_66+HAL_MCF5272_SDTR_RC_6+HAL_MCF5272_SDTR_RP_4+HAL_MCF5272_SDTR_RCD_3+HAL_MCF5272_SDTR_CLT_2)
#elif defined(CYGHWR_HAL_SYSTEM_CLOCK_HZ_48000000)
	.equ _HAL_MCF5272_SDTR_VALUE_, (HAL_MCF5272_SDTR_RTP_48+HAL_MCF5272_SDTR_RC_6+HAL_MCF5272_SDTR_RP_4+HAL_MCF5272_SDTR_RCD_3+HAL_MCF5272_SDTR_CLT_2)
#else
# error Unsupported system clock speed.
#endif

// ----------------------------------------------------------------------------
// Startup code. Use the processor defaults.
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMFFE)
# define _HAL_MCF5272_STANDARD_INIT_ROM_	1
#else
# define _HAL_MCF5272_STANDARD_INIT_RAM_	1
#endif

//------------------------------------------------------------------------------
// end of plf.inc
