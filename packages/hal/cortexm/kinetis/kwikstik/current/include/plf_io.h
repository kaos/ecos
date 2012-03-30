#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      Platform specific registers
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):      Tomas Frydrych <tomas@sleepfive.com>
// Original:       Based on plf_io.h for twr-k40x256 board by Ilija Kocho
// Date:           2011-12-15
// Purpose:        Kwikstik platform specific registers
// Description:
// Usage:          #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm_kinetis_kwikstik.h>


// UART PINs

#ifndef CYGHWR_HAL_FREESCALE_UART5_PIN_RX
# define CYGHWR_HAL_FREESCALE_UART5_PIN_RX CYGHWR_HAL_KINETIS_PIN(E, 9, 3, 0)
# define CYGHWR_HAL_FREESCALE_UART5_PIN_TX CYGHWR_HAL_KINETIS_PIN(E, 8, 3, 0)
# define CYGHWR_HAL_FREESCALE_UART5_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_HAL_FREESCALE_UART5_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE

# define CYGHWR_IO_FREESCALE_UART5_PIN_RX CYGHWR_HAL_FREESCALE_UART5_PIN_RX
# define CYGHWR_IO_FREESCALE_UART5_PIN_TX CYGHWR_HAL_FREESCALE_UART5_PIN_TX
# define CYGHWR_IO_FREESCALE_UART5_PIN_RTS CYGHWR_HAL_FREESCALE_UART5_PIN_RTS
# define CYGHWR_IO_FREESCALE_UART5_PIN_CTS CYGHWR_HAL_FREESCALE_UART5_PIN_CTS
#endif

// LCD
#define CYGHWR_HAL_KINETIS_SIM_SCGC3_SLCD_M            0x40000000
#define CYGHWR_HAL_KINETIS_SIM_SCGC3_SLCD_S            30

#ifndef CYGHWR_HAL_KINETIS_SIM_SCGC3_ALL_M
#define CYGHWR_HAL_KINETIS_SIM_SCGC3_ALL_M             \
            (CYGHWR_HAL_KINETIS_SIM_SCGC3_RNGB_M |     \
             CYGHWR_HAL_KINETIS_SIM_SCGC3_FLEXCAN1_M | \
             CYGHWR_HAL_KINETIS_SIM_SCGC3_SPI2_M |     \
             CYGHWR_HAL_KINETIS_SIM_SCGC3_SDHC_M |     \
             CYGHWR_HAL_KINETIS_SIM_SCGC3_FTM2_M |     \
             CYGHWR_HAL_KINETIS_SIM_SCGC3_ADC1_M |     \
             CYGHWR_HAL_KINETIS_SIM_SCGC3_SLCD_M)
#endif

// I2C pins
#ifndef CYGHWR_HAL_I2C0_PIN_SDA
# define CYGHWR_HAL_I2C0_PIN_SDA CYGHWR_HAL_KINETIS_PIN(B, 1, 2, 0)
# define CYGHWR_HAL_I2C0_PIN_SCL CYGHWR_HAL_KINETIS_PIN(B, 0, 2, 0)
# define CYGHWR_HAL_I2C1_PIN_SDA CYGHWR_HAL_KINETIS_PIN(C, 11, 2, 0)
# define CYGHWR_HAL_I2C1_PIN_SCL CYGHWR_HAL_KINETIS_PIN(C, 10, 2, 0)
#endif

//=============================================================================
// Memory access checks.
//
// Accesses to areas not backed by real devices or memory can cause
// the CPU to hang. These macros allow the GDB stubs to avoid making
// accidental accesses to these areas.

__externC int cyg_hal_stub_permit_data_access( CYG_ADDRESS addr, cyg_uint32 count );

#define CYG_HAL_STUB_PERMIT_DATA_READ(_addr_, _count_) cyg_hal_stub_permit_data_access( _addr_, _count_ )

#define CYG_HAL_STUB_PERMIT_DATA_WRITE(_addr_, _count_ ) cyg_hal_stub_permit_data_access( _addr_, _count_ )

//=============================================================================


//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H
