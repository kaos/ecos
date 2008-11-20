#ifndef CYGONCE_HAL_VAR_ARCH_H
#define CYGONCE_HAL_VAR_ARCH_H
//=============================================================================
//
//      var_arch.h
//
//      Architecture variant specific abstractions
//
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2006,2008 Free Software Foundation, Inc.
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
// Author(s):    bartv
// Date:         2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k_mcfxxxx.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/proc_arch.h>

// ----------------------------------------------------------------------------
// Context support. ColdFire exceptions/interrupts are simpler than the
// 68000 variants, with a much more regular exception frame. Usually the
// hardware simply pushes two longs on to the stack. The program counter
// is at the top. Then the 16-bit status register. Then some extra
// information identifying the exception number etc. If the stack was not
// aligned when the exception occurred) (which should not happen for
// eCos code) then the hardware will do some extra stack alignment and
// store this information in a fmt field. That possibility is ignored
// for now.

#define HAL_CONTEXT_PCSR            \
    cyg_uint32  sr_vec;             \
    CYG_ADDRESS pc;

// An exception aligns the stack to a 32-bit boundary, and the fmt part
// of the exception frame encodes how much adjustment was done. The
// 0x40000000 specifies 0 bytes adjustment since the code should be
// running with stacks always aligned.
#define HAL_CONTEXT_PCSR_INIT(_regs_, _entry_, _sr_)                \
    CYG_MACRO_START                                                 \
    (_regs_)->sr_vec    = 0x40000000 | (cyg_uint32)(_sr_);          \
    (_regs_)->pc        = (CYG_ADDRESS)(_entry_);                   \
    CYG_MACRO_END

#define HAL_CONTEXT_PCSR_GET_SR(_regs_, _sr_)                       \
    CYG_MACRO_START                                                 \
    _sr_ = (_regs_)->sr_vec & 0x0000FFFF;                           \
    CYG_MACRO_END

#define HAL_CONTEXT_PCSR_GET_PC(_regs_, _pc_)                       \
    CYG_MACRO_START                                                 \
    _pc_    = (_regs_)->pc;                                         \
    CYG_MACRO_END

#define HAL_CONTEXT_PCSR_SET_SR(_regs_, _sr_)                       \
    CYG_MACRO_START                                                 \
    (_regs_)->sr_vec = ((_regs_)->sr_vec & 0xFFFF0000) | (_sr_);    \
    CYG_MACRO_END

#define HAL_CONTEXT_PCSR_SET_PC(_regs_, _pc_)                       \
    CYG_MACRO_START                                                 \
    (_regs_)->pc = (CYG_ADDRESS)(_pc_);                             \
    CYG_MACRO_END

#define HAL_CONTEXT_PCSR_GET_EXCEPTION(_regs_, _code_)              \
    CYG_MACRO_START                                                 \
    (_code_) = (((_regs_)->sr_vec) >> 18) & 0x000000FF;             \
    CYG_MACRO_END

// ----------------------------------------------------------------------------
// LSBIT/MSBIT. Most ColdFires have ff1 and bitrev instructions which
// allow for more efficient implementations than the default ones in
// the architectural HAL.
#ifndef _HAL_M68K_MCFxxxx_NO_FF1_
# define HAL_LSBIT_INDEX(_index_, _mask_)       \
    CYG_MACRO_START                             \
    cyg_uint32  _tmp_   = (_mask_);             \
    int  _idx_;                                 \
    if (0 == _tmp_) {                           \
        _idx_ = -1;                             \
    } else {                                    \
        __asm__ volatile (                      \
            "move.l %1, %0 ; \n"                \
            "bitrev.l %0 ; \n"                  \
            "ff1.l    %0 ; \n"                  \
            : "=d" (_idx_)                      \
            : "d"  (_mask_)                     \
            );                                  \
    }                                           \
    _index_ = _idx_;                            \
    CYG_MACRO_END

# define HAL_MSBIT_INDEX(_index_, _mask_)       \
    CYG_MACRO_START                             \
    cyg_uint32  _tmp_   = (_mask_);             \
    int         _idx_;                          \
    __asm__ volatile (                          \
        "move.l %1, %0 ; \n"                    \
        "ff1.l  %0\n"                           \
        : "=d" (_idx_)                          \
        : "d"  (_tmp_)                          \
        );                                      \
    _index_ = 31 - _idx_;                       \
    CYG_MACRO_END

#endif

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_ARCH_H

