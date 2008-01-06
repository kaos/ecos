#ifndef CYGONCE_HAL_ARCH_INC
#define CYGONCE_HAL_ARCH_INC
##=============================================================================
##
##	arch.inc
##
##	fr30 assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
## Copyright (C) 2007 eCosCentric Ltd.
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
## Author(s): 	 larsi
## Contributors: larsi
## Date:	2006-06-20
## Purpose:	Architecture definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the fr30 CPU family.
## Usage:
##		#include <cyg/hal/arch.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/fr30.inc>

#include <cyg/hal/variant.inc>

##-----------------------------------------------------------------------------
## define some .equ's to access c-code #define's from assembler code
.equ    CYGNUM_ASM_CALL_IF_TABLE_SIZE,   CYGNUM_CALL_IF_TABLE_SIZE

##-----------------------------------------------------------------------------
## CPU specific macros. These provide a common assembler interface to
## operations that may have CPU specific implementations on different
## variants of the architecture

#ifndef CYGPKG_HAL_FR30_CPU_INIT_DEFINED
    # Initialize CPU
    .macro  hal_cpu_init
    .endm
#endif /* !CYGPKG_HAL_FR30_CPU_INIT_DEFINED */

.macro  hal_cpu_int_enable
    orccr   #0x10
.endm

.macro  hal_cpu_int_disable
    andccr  #0xef
.endm


# Merge the interrupt enable state of the status register in
# \sr with the current sr.

.macro  hal_cpu_int_merge sr
    FIXME hal_cpu_int_merge not implemented yet
.endm

##-----------------------------------------------------------------------------
# Default FR30 interrupt controller macros. Every FR30 has an integrated
# interrupt controller, which we use here. This should be enough if there is
# no special external interrupt controller (which I did not see yet).

#ifndef CYGPKG_HAL_FR30_INTC_DEFINED

#ifndef CYGPKG_HAL_FR30_INTC_INIT_DEFINED
# initialize all interrupts to disabled. This is done automatically during
# CPU reset and the macro is not used during ECOS startup. It is not
# supplied here but would be setting all ICRs to 31 (to disable the particular
# interrupt and maybe setting ILM in PS to 0.
    .macro   hal_intc_init
    .endm
#endif


# Normally interrupts are decoded by hardware and can not be software decoded,
# so this is empty here.

    .macro  hal_intc_decode vnum
    .endm

# Also translation interrupt number <--> vector number is done automatically
# in hardware, so the macros are not supplied here.

#endif

#------------------------------------------------------------------------------
# These defines are for the ISR and VSR tables which are defined in assembler
# code. (currently in variant.S / vectors.S) and have to be the same like in
# hal_intr.h

#define CYGNUM_HAL_VECTOR_INTRFIRST               15
#define CYGNUM_HAL_VECTOR_INTRLAST                63
#define CYGNUM_HAL_VECTOR_NUMINTRS                (CYGNUM_HAL_VECTOR_INTRLAST-CYGNUM_HAL_VECTOR_INTRFIRST+1)

// Common interrupt vectors
#ifndef CYGNUM_HAL_ISR_MIN
#define CYGNUM_HAL_ISR_MIN                       CYGNUM_HAL_VECTOR_INTRFIRST
#define CYGNUM_HAL_ISR_MAX                       CYGNUM_HAL_VECTOR_INTRLAST
#define CYGNUM_HAL_ISR_COUNT                     (CYGNUM_HAL_VECTOR_NUMINTRS)
#endif

// The default size of the VSR table is 256 entries.
#ifndef CYGNUM_HAL_VSR_MIN
#define CYGNUM_HAL_VSR_MIN                        0
#define CYGNUM_HAL_VSR_MAX                        255
#define CYGNUM_HAL_VSR_COUNT                      256
#endif

#------------------------------------------------------------------------------
# Register save and restore macros. These expect a pointer to a CPU save state
# area in the register \ptr. The GPR indicated by \reg will be saved into its
# slot in that structure.

# TODO do this macros if needed, look at MIPS arch.inc for inspiration


#------------------------------------------------------------------------------
# Stack switching macros



#------------------------------------------------------------------------------
# MEMC macros.


#------------------------------------------------------------------------------
# Cache macros.

#ifndef CYGPKG_HAL_FR30_CACHE_DEFINED

    .macro  hal_cache_init

    .endm

#endif

#------------------------------------------------------------------------------
# Diagnostics macros.

#------------------------------------------------------------------------------
# Timer initialization.

#ifndef CYGPKG_HAL_FR30_TIMER_DEFINED

    .macro  hal_timer_init
    .endm

#endif


#endif // ifndef CYGONCE_HAL_ARCH_INC
# end of arch.inc
