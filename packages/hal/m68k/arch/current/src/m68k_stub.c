//==========================================================================
//
//      m68k_stub.c
//
//      M68K gdb support
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2008 Free Software Foundation, Inc.
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
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2003-06-04
//
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_stub.h>

// Translate between the eCos save state and what is expected by the gdb
// stubs. d0-d7/a0-a6 are in the right place already. SR/PS and the program
// counter need to come from variant-specific macros. The stack pointer
// can be determined from the saved context.
externC void
hal_get_gdb_registers(CYG_ADDRWORD* gdb_regs, HAL_SavedRegisters* ecos_regs)
{
    int     sr;
    int     pc;
    int     i;
    for (i = 0; i < 15; i++) {
        gdb_regs[i] = ecos_regs->da[i];
    }
    gdb_regs[15]    = (CYG_ADDRWORD) &(ecos_regs[1]);
    HAL_CONTEXT_PCSR_GET_SR(ecos_regs, sr);
    HAL_CONTEXT_PCSR_GET_PC(ecos_regs, pc);
    gdb_regs[16]    = sr;
    gdb_regs[17]    = pc;
}

externC void
hal_set_gdb_registers(HAL_SavedRegisters* ecos_regs, CYG_ADDRWORD* gdb_regs)
{
    int i;
    for (i = 0; i < 15; i++) {
        ecos_regs->da[i] = gdb_regs[i];
    }
    if (gdb_regs[15] != (CYG_ADDRWORD) &(ecos_regs[1])) {
        CYG_FAIL("gdb has requested a thread context switch - not supported,");
    }
    HAL_CONTEXT_PCSR_SET_SR(ecos_regs, gdb_regs[16]);
    HAL_CONTEXT_PCSR_SET_PC(ecos_regs, gdb_regs[17]);
}

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

externC int
__computeSignal(unsigned int trap_number)
{
    switch(trap_number) {
      case CYGNUM_HAL_EXCEPTION_DATA_ACCESS:
        return SIGBUS;
      case CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS:
      case CYGNUM_HAL_EXCEPTION_SYSTEM_ERROR:
        return SIGSEGV;
      case CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION:
        return SIGILL;
      case CYGNUM_HAL_EXCEPTION_DIV_BY_ZERO:
        return SIGFPE;
      case CYGNUM_HAL_EXCEPTION_TRACE:
      case CYGNUM_HAL_VECTOR_TRAP0 ... CYGNUM_HAL_VECTOR_TRAP15:
      default:
        return SIGTRAP;
    }
}

externC int
__get_trap_number(void)
{
    int result = CYGNUM_HAL_EXCEPTION_SYSTEM_ERROR;
    if ((HAL_SavedRegisters*)0 != _hal_registers) {
        HAL_CONTEXT_PCSR_GET_EXCEPTION(_hal_registers, result);
    }
    return result;
}

externC void
set_pc(target_register_t pc)
{
    _registers[PC]  = pc;
}

// Single-stepping just involves setting the trace bit in the status register
externC void
__single_step(void)
{
    _registers[PS]  |= HAL_M68K_SR_T;
}

externC void
__clear_single_step(void)
{
    _registers[PS]  &= ~HAL_M68K_SR_T;
}

// This breakpoint support is (probably) not needed because
// single-stepping is supported instead.
externC void
__install_breakpoints(void)
{
}

externC void
__clear_breakpoints(void)
{
}

externC int
__is_breakpoint_function(void)
{
    return _registers[PC]   == (target_register_t)&_breakinst;
}

// The 68K has variable length instructions so skipping an instruction
// is messy. However __skipinst() is only used for a breakpoint
// instruction or for a trap, and these are always 2 bytes.
externC void
__skipinst(void)
{
    _registers[PC] += 2;
}

#endif
