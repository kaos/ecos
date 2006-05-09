#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC

|==========================================================================
|
|      variant.inc
|
|      MCF5272 variant assembler header file
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
| Purpose:      MCF5272 variant definitions.
| Description:  This file contains the definitions specific to the
|               CPU variant, used in the architecture HAL assembler file.
| Usage:        Included by "vectors.S". Do not use directly.
|
|####DESCRIPTIONEND####
|==========================================================================

#include <cyg/hal/platform.inc>


| -----------------------------------------------------------------------------
| CPU specific macros. These provide a common assembler interface to
| operations that may have CPU specific implementations on different
| variants of the architecture.     

| CPU initialization macro
        .macro hal_cpu_init

        | Invalidate and disable the cache and ACRs.
        move.l  #0x01000000,%d0
        movec   %d0,%cacr

        move.l  #0x0,%d0
        movec   %d0,%acr0
        movec   %d0,%acr1
        .endm


| ----------------------------------------------------------------------------
| This macro retrieves the IPL of the current interrupt from the
| interrupt controller register. This is needed because on inetrrupt entry
| all interrupts are disabled by writing to the status register, and thus
| loosing the current IPL.
| Input: interrupt vector number in d0
| Output: IPL associated to interrupt in d0

        .macro hal_variant_retrieve_ipl

        | Subtract minimum interrupt vector number
        sub.l #CYGNUM_HAL_ISR_MIN,%d0
    
        | Load IPL table address in a0
        lea cyg_hal_ILVL_table,%a0

        | Retrieve IPL level for current interrupt
        move.b  (%a0,%d0.l),%d0

        .endm

| ----------------------------------------------------------------------------
| end of variant.inc
#endif // ifndef CYGONCE_HAL_VARIANT_INC
