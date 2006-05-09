#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC

|=============================================================================
|
|  platform.inc
|
|  M5272C3 board assembler header file
|
|=============================================================================
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
|###ECOSGPLCOPYRIGHTEND###
|=============================================================================
|#####DESCRIPTIONBEGIN####
|
| Author(s):     Enrico Piria
| Contributors:
| Date:          2005-25-06
| Purpose:       Assembler macro definitions specific to the M5272C3 board.
| Usage:         Included by "variant.inc". Do not use directly.
|
|####DESCRIPTIONEND####
|========================================================================

#include <cyg/hal/plf_offsets.inc>

|-------------------------------------------------------------------------------
| Platform initialization macros

        .macro hal_hardware_init
        | Initialize RAMBAR: locate SRAM and validate it.
        move.l  #CYGMEM_REGION_sram,%d0
        add.l   #0x21,%d0
        movec   %d0,%rambar0
        .endm


        | Setup stack for startup routines. Use SRAM module.
        .macro hal_boot_stack_init
        | Point Stack Pointer into SRAM temporarily. 
        move.l   #CYGMEM_REGION_sram,%d0
        add.l    #CYGMEM_REGION_sram_SIZE,%d0
        move.l   %d0,%sp
        .endm

|-----------------------------------------------------------------------------
| End of platform.inc
#endif // CYGONCE_HAL_PLATFORM_INC
