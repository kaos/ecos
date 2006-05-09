|==========================================================================
|
|      variant.S
|
|      MCF5272 variant code
|
|==========================================================================
|###ECOSGPLCOPYRIGHTBEGIN####
| -------------------------------------------
| This file is part of eCos, the Embedded Configurable Operating System.
| Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
| Copyright (C) 2006 eCosCentric Ltd.
|
| eCos is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free
| Software Foundation; either version 2 or (at your option) any later version.
|
| eCos is distributed in the hope that it will be useful, but WITHOUT ANY
| WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
| for more details.
|
| You should have received a copy of the GNU General Public License along
| with eCos; if not, write to the Free Software Foundation, Inc.,
| 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
|
| As a special exception, if other files instantiate templates or use macros
| or inline functions from this file, or you compile this file and link it
| with other works to produce a work based on this file, this file does not
| by itself cause the resulting work to be covered by the GNU General Public
| License. However the source code for this file must still be made available
| in accordance with section (3) of the GNU General Public License.
|
| This exception does not invalidate any other reasons why a work based on
| this file might be covered by the GNU General Public License.
| -------------------------------------------
|###ECOSGPLCOPYRIGHTEND####
|=============================================================================
|#####DESCRIPTIONBEGIN####
|
| Author(s):    Enrico Piria
| Contributors:
| Date:         2005-25-06
| Purpose:      MCF5272 variant code.
| Description:  This file contains the VSR table for the MCF5272, and
|               other definitions used by the rest of the ColdFire HAL.
|
|####DESCRIPTIONEND####
|==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/cf_offsets.inc>


| ----------------------------------------------------------------------------
| ROM vector table

        .section ".romvec","ax"

        .extern cyg_interrupt_stack
        .extern cyg_hal_reset_vsr
        .extern cyg_hal_default_exception_vsr
        .extern cyg_hal_default_spurious_vsr
        .extern cyg_hal_default_interrupt_vsr

        .globl  rom_vsr_table
rom_vsr_table:

        | 0 - Initial SSP
        .long   cyg_interrupt_stack

        | 1 - Initial PC
        .long   cyg_hal_reset_vsr

        | 2-14 - Default exception handlers
        .rept   14-2+1
        .long   cyg_hal_default_exception_vsr
        .endr

        | 15 - Uninitialized interrupt. It should never happen, because
        | we configure interrupt controller at startup.
        .long   cyg_hal_default_spurious_vsr

        | 16-23 - Reserved, treat as exceptions
        .rept   23-16+1
        .long   cyg_hal_default_exception_vsr
        .endr

        | 24 - Spurious interrupt
        .long   cyg_hal_default_spurious_vsr

        | 25-31 - Autovectored interrupts 1-7. Not used in MCF5272.
        .rept   31-25+1
        .long   cyg_hal_default_interrupt_vsr
        .endr

        | 32-63 - Default exception handlers
        .rept   63-32+1
        .long   cyg_hal_default_exception_vsr
        .endr

        | 64 - User spurious interrupt. The MCF5272 interrupt controller
        | returns this vector number instead of vector 24
        .long   cyg_hal_default_spurious_vsr

        | 65-255 - User interrupt vectors
        .rept   255-65+1
        .long   cyg_hal_default_interrupt_vsr
        .endr
