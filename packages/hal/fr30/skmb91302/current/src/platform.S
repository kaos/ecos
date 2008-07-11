##=============================================================================
##
##	platform.S
##
##	Fujitsu Starterkit MB91302 platform code
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
##
## Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
## at http://sources.redhat.com/ecos/ecos-license/
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):       larsi
## Contributors:
## Date:            2007-07-09
## Purpose:         Fujitsu Starterkit MB91302 platform code
## Description:     Platform specific code for Fujitsu Starterkit MB91302 board.
##
##
##
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

#include <cyg/hal/arch.inc>

##-----------------------------------------------------------------------------
## platforms special entry point.
## it copies the code from hal_fr30_ram_startup_trampoline until
## hal_fr30_ram_startup_trampoline_end to ram and jumps there. The code there
## is for mapping the flash to 0x1000000 and then jumping to real entry
## (_start).

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

    .section ".rom_startup_trampoline","ax"

    .balign 4
    .global hal_fr30_rom_startup_trampoline
hal_fr30_rom_startup_trampoline:
    # disable interrupts and set priority to lowest (=disable)
    andccr  #0xef
    stilm   #0x0

    ldi:32  #__rom_trampoline_start - 4,       r11
    ldi:32  #__ram_trampoline_start - 4,                r13
    ldi:32  #__rom_trampoline_end - 8,   r12
1:
    add     #0x4,   r11
    add     #0x4,   r13
    ld      @r11,   r0
    cmp     r12,    r11
    ble:d   1b
    st      r0,     @r13

## jump to the code in ram
    ldi:32  #hal_fr30_ram_startup_trampoline, r0
    jmp     @r0

#endif

##-----------------------------------------------------------------------------
# Platform Initialization.
# This code is copied to a location in RAM on startup and is executed there.
# It maps the the FLASH chip to 0x1000000 and sets the TBR to the new location
# of the hardware vector table (hal_fr30_vector_table). So we can reach the full
# range of the FLASH and it can be used for flashfilesystem. After that it jumps
# to the real application.

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

    .section ".ram_startup_trampoline","ax"

    .balign 4
    .global hal_fr30_ram_startup_trampoline
hal_fr30_ram_startup_trampoline:
    hal_flash_init_from_ram

## map TBR to the new location (for vector tables)
    ldi:32  #0x10ffc00, r0
    mov     r0,     tbr

## jump to real startup
    ldi:32  #_start,    r0
    jmp     @r0

#endif


#==============================================================================
# Vector table for storage in flash, version for skmb91302 platform.
# it differs from variants/archs version in that uses a different reset vector
# that points to the special startup procedure that jumps to the normal entry
# (_start) later.
# base address is 0x000FFC00 and 0x10ffc00 after remapping the flash.
# This address stores the vector address for number 255
# address 0x000FFFFC (0x10ffffc) is the last vector, the reset vector, which
# is not alterable. Vectors 255 to 80 are used by the INT instruction and set
# to 0 here for now ...


#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

     .section ".rom_vectors","ax"

    .balign 4
    .global hal_fr30_vector_table
hal_fr30_vector_table:
    .rept   255-80 + 1
    .long   0x0
    .endr
    .rept   79-67 + 1
    .long   0x0
    .endr

    .long   0x0
    .long   0x0
    .long   0x0
# number 63 first interrupt source
    .long   hal_fr30_irq_63
    .long   hal_fr30_irq_62
    .long   hal_fr30_irq_61
    .long   hal_fr30_irq_60
    .long   hal_fr30_irq_59
    .long   hal_fr30_irq_58
    .long   hal_fr30_irq_57
    .long   hal_fr30_irq_56
    .long   hal_fr30_irq_55
    .long   hal_fr30_irq_54
    .long   hal_fr30_irq_53
    .long   hal_fr30_irq_52
    .long   hal_fr30_irq_51
    .long   hal_fr30_irq_50
    .long   hal_fr30_irq_49
    .long   hal_fr30_irq_48
    .long   hal_fr30_irq_47
    .long   hal_fr30_irq_46
    .long   hal_fr30_irq_45
    .long   hal_fr30_irq_44
    .long   hal_fr30_irq_43
    .long   hal_fr30_irq_42
    .long   hal_fr30_irq_41
    .long   hal_fr30_irq_40
    .long   hal_fr30_irq_39
    .long   hal_fr30_irq_38
    .long   hal_fr30_irq_37
    .long   hal_fr30_irq_36
    .long   hal_fr30_irq_35
    .long   hal_fr30_irq_34
    .long   hal_fr30_irq_33
    .long   hal_fr30_irq_32
    .long   hal_fr30_irq_31
    .long   hal_fr30_irq_30
    .long   hal_fr30_irq_29
    .long   hal_fr30_irq_28
    .long   hal_fr30_irq_27
    .long   hal_fr30_irq_26
    .long   hal_fr30_irq_25
    .long   hal_fr30_irq_24
    .long   hal_fr30_irq_23
    .long   hal_fr30_irq_22
    .long   hal_fr30_irq_21
    .long   hal_fr30_irq_20
    .long   hal_fr30_irq_19
    .long   hal_fr30_irq_18
    .long   hal_fr30_irq_17
    .long   hal_fr30_irq_16
    .long   hal_fr30_irq_15
    .long   hal_fr30_exception_noerr_14
    .long   hal_fr30_exception_noerr_13
    .long   hal_fr30_exception_noerr_12
    .long   hal_fr30_exception_noerr_11
    .long   hal_fr30_exception_noerr_10
    .long   hal_fr30_exception_noerr_9
    .long   hal_fr30_exception_noerr_8
    .long   hal_fr30_exception_noerr_7
    .long   hal_fr30_exception_noerr_6
    .long   hal_fr30_exception_noerr_5
    .long   hal_fr30_exception_noerr_4
    .long   hal_fr30_exception_noerr_3
    .long   hal_fr30_exception_noerr_2

# mode vector (only the first of the four byte is relevant)
    .byte   0x1
    .byte   0
    .byte   0
    .byte   0

# reset vector
    .long   hal_fr30_rom_startup_trampoline

#endif /*CYG_HAL_STARTUP_ROM || CYG_HAL_STARTUP_ROMRAM*/


##-----------------------------------------------------------------------------
## end of platform.S
