//========================================================================
//
//      coldfire_stub.c
//
//      Helper functions for stub
//
//========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       Helper functions for stub, generic to all ColdFire
//                processors.
//
//####DESCRIPTIONEND####
//========================================================================

#include <stddef.h>
#include <string.h>                     // memcpy, memset

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#include <cyg/hal/hal_stub.h>

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

// Given a trap value TRAP, return the corresponding signal.
int __computeSignal (unsigned int trap_number)
{
    switch (trap_number)
    {

        case CYGNUM_HAL_VECTOR_BUSERR:
        case CYGNUM_HAL_VECTOR_ADDRERR:
            return SIGBUS;

        case CYGNUM_HAL_VECTOR_ILLINST:
        case CYGNUM_HAL_VECTOR_UNSUPINST:
            return SIGILL;

        case CYGNUM_HAL_VECTOR_ZERODIV:
        case CYGNUM_HAL_VECTOR_FP_BRANCH:
        case CYGNUM_HAL_VECTOR_FP_INEXACT:
        case CYGNUM_HAL_VECTOR_FP_ZERODIV:
        case CYGNUM_HAL_VECTOR_FP_UNDERFLOW:
        case CYGNUM_HAL_VECTOR_FP_OPERAND:
        case CYGNUM_HAL_VECTOR_FP_OVERFLOW:
        case CYGNUM_HAL_VECTOR_FP_NAN:
        case CYGNUM_HAL_VECTOR_FP_DENORM:
            // Although not quite accurate, use this signal also for
            // integer division.
            return SIGFPE;

        case CYGNUM_HAL_VECTOR_PRIVVIOLATION:
            return SIGILL;
        case CYGNUM_HAL_VECTOR_TRACE:
            // Instruction trace
            return SIGTRAP;

        case CYGNUM_HAL_VECTOR_L1010:
        case CYGNUM_HAL_VECTOR_L1111:
        case CYGNUM_HAL_VECTOR_UNINITINT:
        case CYGNUM_HAL_VECTOR_SPURINT:
            return SIGTRAP;

        case CYGNUM_HAL_VECTOR_TRAPFIRST ... CYGNUM_HAL_VECTOR_TRAPLAST:
            return SIGTRAP;

        case CYGNUM_HAL_VECTOR_AUTOVEC1 ... CYGNUM_HAL_VECTOR_AUTOVEC7:
        case CYGNUM_HAL_VECTOR_USERINTRFIRST ... CYGNUM_HAL_VECTOR_USERINTRLAST:
            // External interrupt
            return SIGINT;

        default:
            return SIGTERM;
    }
}


// Return the trap number corresponding to the last-taken trap.
int __get_trap_number (void)
{
    // The vector is not not part of the GDB register set so get it
    // directly from the saved context.
    return HAL_CF_EXCEPTION_VECTOR(_hal_registers->fmt_vec_word);
}


// Set the currently-saved pc register value to PC.
void set_pc (target_register_t pc)
{
    put_register (PC, pc);
}


// Return the offset of a register in the GDB_Registers structure.
static int reg_offset(regnames_t reg)
{
    switch(reg)
    {
        case D0 ... A7:
            return reg * 4;
    
        case SR:
            return offsetof(GDB_Registers, sr);

        default:
        case PC:
            return offsetof(GDB_Registers, pc);
    }
}


// Return the currently-saved value corresponding to register REG of
// the exception context.
target_register_t get_register(regnames_t reg)
{
    target_register_t val;
    int offset = reg_offset(reg);

    if (REGSIZE(reg) > sizeof(target_register_t))
        return -1;

    val = _registers[offset/sizeof(target_register_t)];

    return val;
}


// Store VALUE in the register corresponding to WHICH in the exception
// context.
void put_register(regnames_t which, target_register_t value)
{
    int offset = reg_offset(which);

    if (REGSIZE(which) > sizeof(target_register_t))
        return;

    _registers[offset/sizeof(target_register_t)] = value;
}
        

// Write the contents of register WHICH into VALUE as raw bytes. This
// is only used for registers larger than sizeof(target_register_t).
// Return non-zero if it is a valid register.
int get_register_as_bytes(regnames_t which, char *value)
{
    int offset = reg_offset(which);

    memcpy (value, (char *)_registers + offset, REGSIZE(which));
    return 1;
}


// Alter the contents of saved register WHICH to contain VALUE. This
// is only used for registers larger than sizeof(target_register_t).
// Return non-zero if it is a valid register.
int put_register_as_bytes(regnames_t which, char *value)
{
    int offset = reg_offset(which);

    memcpy ((char *)_registers + offset, value, REGSIZE(which));
    return 1;
}


// ---------------------------------------------------------------------
// Single-step support

// Set things up so that the next user resume will execute one instruction.
// This may be done by setting breakpoints or setting a single step flag
// in the saved user registers, for example.

#define SR_TRACE 0x8000

void __single_step(void)
{
    target_register_t sr = get_register (SR);

    // Set trace flag in the exception context.
    sr |= SR_TRACE;

    put_register (SR, sr);
}


// Clear the single-step state.
void __clear_single_step(void)
{
    target_register_t sr = get_register (SR);

    // Clear single-step flag in the exception context.
    sr &= ~SR_TRACE;

    put_register (SR, sr);
}


void __install_breakpoints(void)
{
    // NOP since single-step HW exceptions are used instead of
    // breakpoints.
}


void __clear_breakpoints(void)
{
    // NOP since single-step HW exceptions are used instead of
    // breakpoints.
}


// If the breakpoint we hit is in the breakpoint() instruction, return a
// non-zero value.
int __is_breakpoint_function(void)
{
    return (get_register(PC) == (target_register_t) &CYG_LABEL_NAME(_breakinst));
}


// Skip the current instruction. Since this is only called by the
// stub when the PC points to a breakpoint or trap instruction,
// we can safely just skip 2.
void __skipinst(void)
{
    put_register (PC, get_register (PC) + HAL_BREAKINST_SIZE);
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
