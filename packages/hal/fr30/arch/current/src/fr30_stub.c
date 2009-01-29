//========================================================================
//
//      fr30_stub.c
//
//      Fujitsu FR30-specific code for remote debugging via gdb
//
//========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     larsi
// Contributors:  
// Date:          2007-07-09
// Purpose:       
// Description:   Helper functions for gdb stub for FR30 processors
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

#include <stddef.h>

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/cyg_ass.h>          // assertion macros

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

/* Given a trap value TRAP, return the corresponding signal. */

int __computeSignal (unsigned int trap_number)
{
    switch (trap_number)
    {

    case CYGNUM_HAL_VECTOR_COPR_NOT_FOUND:
    case CYGNUM_HAL_VECTOR_COPR_ERROR:
        return SIGFPE;

        // step trace TRAP
    case CYGNUM_HAL_VECTOR_DEBUG:
        // INTE
    case CYGNUM_HAL_VECTOR_BREAKPOINT:
        return SIGTRAP;
        /* System call instruction executed */
    case CYGNUM_HAL_VECTOR_SYSTEM_CALL ... CYGNUM_HAL_VECTOR_TRAPLAST:
        return SIGSYS;
        /* External interrupt */
    case CYGNUM_HAL_INTERRUPT_0 ... CYGNUM_HAL_INTERRUPT_DELAYED_IRQ:
      return SIGINT;
      // Illegal or reserved instruction
    case CYGNUM_HAL_VECTOR_OPCODE:
        return SIGILL;

    // Marks port does think to return SIGTRAP as default.
    default:
        return SIGTERM;
    }
}

/* Return the trap number corresponding to the last-taken trap. */

int __get_trap_number (void)
{
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return _hal_registers->last_trap;
}

/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */

void set_pc (target_register_t pc)
{
    put_register (PC, pc);
}

/*----------------------------------------------------------------------
 * Single-step support
 */

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */

void __single_step (void)
{
  /* Trying to use processors single stepping.
     This means to set T flag in PS register. */
    put_register (PS, get_register (PS) | 0x100);
}

/* Clear the single-step state. */
void __clear_single_step (void)
{
    put_register (PS, get_register (PS) & ~0x100);
}

void __install_breakpoints ()
{
  /*  if (instrBuffer.targetAddr != NULL)
    {
      instrBuffer.savedInstr = *instrBuffer.targetAddr;
      *instrBuffer.targetAddr = __break_opcode ();
      } */

  /* Install the breakpoints in the breakpoint list */
  __install_breakpoint_list();

  // No need to flush caches here; Generic stub code will handle this.
}

void __clear_breakpoints (void)
{
  __clear_breakpoint_list();
}

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */

int
__is_breakpoint_function ()
{
    return get_register (PC) == (target_register_t)&_breakinst;
}

/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
   we can safely just skip 2. */

void __skipinst (void)
{
    put_register (PC, get_register (PC) + 2);
}

/* Get a register out of the GDB register structure */
target_register_t
get_register (regnames_t reg)
{
    GDB_Registers* gdb_regs;

    gdb_regs = (GDB_Registers*)_registers;

    if (reg >= R0 && reg <= MDL)
        return gdb_regs->r[reg];

    return 0xdeadbeef;
}

/* Put a register into the GDB register structure */
void
put_register (regnames_t reg, target_register_t value)
{
    GDB_Registers* gdb_regs;

    gdb_regs = (GDB_Registers*)_registers;

    if (reg >= R0 && reg <= MDL) {
        gdb_regs->r[reg] = value;
    } else {
        CYG_FAIL("Attempt to write to non-existent register ");
    }
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

// EOF openrisc_stub.c
