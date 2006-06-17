#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

//=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2006 eCosCentric Ltd
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Koco <ilijak@siva.com.mk>
// Contributors:
// Date:        2006-03-12
// Purpose:     MAC7100/MACE1 platform specific support routines
// Description:
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <cyg/hal/var_io.h>
#include <cyg/hal/hal_var_setup.h>
//===========================================================================

   .macro _led y
        ldr r0,=MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET)
        ldrh r1,[r0]
        and r1,r1,#0x00ff
        add r1,r1,#(\y<<8)
        strh r1,[r0]
    .endm


// Initialize LED PORT
// Set appropriate peripheral pins
	.macro  _led_init
        mov r1, #MAC7100_PIM_DDR  // Pin setting: Output+Low
        ldr r0,=MAC7100_PIM_CONFIG(MAC7100_PORT_A_OFFSET, 8) // LED pin cfg
        strh r1,[r0],#2 // LED: LSB ...
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2 // LED: MSB ...
      _led    0         // Set initial LED state. 
    .endm


// Clock initilalization
	.macro  _pclock_init
		_mac7100_setpll
    .endm

// Memory re-mapping
	.macro _memory_remap
		_mac7100_remap_single_chip
    .endm


// Initialize paralel port
    .macro  _pio_init
    .endm


#define CYGHWR_LED_MACRO _led \x

//===========================================================================
                        
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

    .macro  _setup
        ldr r0,=VAE_MAC7100_FlashSecurity
        _led_init
        _memory_remap
        _pclock_init
        _pio_init
    .endm

#define CYGSEM_HAL_ROM_RESET_USES_JUMP

#else

        .macro  _setup
        _led_init
        _led 16
        _pclock_init
        _pio_init        
        .endm
        
#endif

#define PLATFORM_SETUP1     _setup

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
