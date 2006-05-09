//==========================================================================
//
//      hal_mk_defs.c
//
//      HAL "make defs" program
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2002 Gary Thomas
// Copyright (C) 2006 eCosCentric Ltd.
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Enrico Piria
// Contributors: gthomas, jskov
// Date:         2005-25-06
// Purpose:      ColdFire architecture dependent definition generator
// Description:  This file contains code that can be compiled by the target
//               compiler and used to generate machine specific definitions
//               suitable for use in assembly code.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
# include <cyg/kernel/instrmnt.h>
#endif

// This program is used to generate definitions needed by
// assembly language modules.
//
// This technique was first used in the OSF Mach kernel code:
// generate asm statements containing #defines,
// compile this file to assembler, and then extract the
// #defines from the assembly-language output.

#define DEFINE(sym, val) \
        asm volatile("\n\t.equ\t" #sym ",%0" : : "i" (val))

int
main(void)
{
    // Exception/interrupt/context save buffer
    DEFINE(CYGARC_CFREG_AREGS, offsetof(HAL_SavedRegisters, a[0]));
    DEFINE(CYGARC_CFREG_DREGS, offsetof(HAL_SavedRegisters, d[0]));
    DEFINE(CYGARC_CFREG_A0, offsetof(HAL_SavedRegisters, a[0]));
    DEFINE(CYGARC_CFREG_A1, offsetof(HAL_SavedRegisters, a[1]));
    DEFINE(CYGARC_CFREG_A2, offsetof(HAL_SavedRegisters, a[2]));
    DEFINE(CYGARC_CFREG_A6, offsetof(HAL_SavedRegisters, a[6]));
    DEFINE(CYGARC_CFREG_D0, offsetof(HAL_SavedRegisters, d[0]));
    DEFINE(CYGARC_CFREG_D1, offsetof(HAL_SavedRegisters, d[1]));
    DEFINE(CYGARC_CFREG_D2, offsetof(HAL_SavedRegisters, d[2]));
    DEFINE(CYGARC_CFREG_PC, offsetof(HAL_SavedRegisters, pc));
    DEFINE(CYGARC_CFREG_SP, offsetof(HAL_SavedRegisters, a[7]));

#ifdef CYGHWR_HAL_COLDFIRE_MAC
    DEFINE(CYGARC_CFREG_MACC, offsetof(HAL_SavedRegisters, macc));
    DEFINE(CYGARC_CFREG_MACSR, offsetof(HAL_SavedRegisters, macsr));
    DEFINE(CYGARC_CFREG_MASK, offsetof(HAL_SavedRegisters, mask));
#endif
    
    DEFINE(CYGARC_CF_CONTEXT_SIZE, sizeof(HAL_SavedRegisters));

    // Below only saved on exceptions/interrupts
    DEFINE(CYGARC_CF_FMTVECWORD, offsetof(HAL_SavedRegisters, fmt_vec_word));
    DEFINE(CYGARC_CF_SR, offsetof(HAL_SavedRegisters, sr));
    DEFINE(CYGARC_CF_EXCEPTION_SIZE, sizeof(HAL_SavedRegisters));
    DEFINE(CYGARC_CF_EXCEPTION_DECREMENT, offsetof(HAL_SavedRegisters, fmt_vec_word));

    // Some other exception related definitions
    DEFINE(CYGNUM_HAL_ISR_MIN, CYGNUM_HAL_ISR_MIN);
    DEFINE(CYGNUM_HAL_ISR_COUNT, CYGNUM_HAL_ISR_COUNT);
    DEFINE(CYGNUM_HAL_SPURIOUS_INTERRUPT, CYGNUM_HAL_SPURIOUS_INTERRUPT);
    DEFINE(CYGNUM_HAL_VECTOR_DEBUGTRAP, CYGNUM_HAL_VECTOR_DEBUGTRAP);

    // setjmp/longjmp related definitions
    DEFINE(CYGARC_JMPBUF_REG_D2, offsetof(hal_jmp_buf_t, d2));
    DEFINE(CYGARC_JMPBUF_REG_A2, offsetof(hal_jmp_buf_t, a2));
    DEFINE(CYGARC_JMPBUF_REG_SP, offsetof(hal_jmp_buf_t, sp));
    DEFINE(CYGARC_JMPBUF_REG_PC, offsetof(hal_jmp_buf_t, pc));

#ifdef CYGHWR_HAL_COLDFIRE_MAC
    DEFINE(CYGARC_JMPBUF_REG_MACC, offsetof(hal_jmp_buf_t, macc));
    DEFINE(CYGARC_JMPBUF_REG_MACSR, offsetof(hal_jmp_buf_t, macsr));
    DEFINE(CYGARC_JMPBUF_REG_MASK, offsetof(hal_jmp_buf_t, mask));
#endif

    return 0;
}

// -------------------------------------------------------------------------
// EOF hal_mk_defs.c
