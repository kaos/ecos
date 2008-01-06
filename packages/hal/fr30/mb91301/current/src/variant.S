##=============================================================================
##
##	variant.S
##
##	FR30 MB91301 variant code
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
## Author(s): 	larsi
## Contributors:larsi
## Date:	2006-07-22
## Purpose:	FR30 MB91301 variant code
## Description:	Variant specific code for MB91301 processor family.
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

#==============================================================================
# Vector table for storage in flash
# base address is 0x000FFC00, which stores the vector address for number 255
# address 0x000FFFFC is the last vector, the reset vector, which is not
# alterable. Vectors 255 to 80 are used by the INT instruction and set to 0
# here for now ...

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

     .section ".rom_vectors","ax"

#    .data

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
    .long   _start

#endif

##-----------------------------------------------------------------------------
# Interrupt vector tables.
# These tables contain the isr, data and object pointers used to deliver
# interrupts to user code.

    .extern hal_default_isr

    .data
    .balign 4

    .globl  hal_interrupt_handlers
hal_interrupt_handlers:
    .rept  CYGNUM_HAL_ISR_COUNT
    .long   hal_default_isr
    .endr

    .globl  hal_interrupt_data
hal_interrupt_data:
    .rept   CYGNUM_HAL_ISR_COUNT
    .long   0
    .endr

    .globl  hal_interrupt_objects
hal_interrupt_objects:
    .rept   CYGNUM_HAL_ISR_COUNT
    .long   0
    .endr

##-----------------------------------------------------------------------------
## end of variant.S
	
