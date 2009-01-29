/*==========================================================================
//
//      cortexm_stub.c
//
//      Cortex-M GDB stub support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-07-30
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <stddef.h>

#include <pkgconf/hal.h>

#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_stub.h>

//==========================================================================

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

//==========================================================================
/* Given a trap value TRAP, return the corresponding signal. */

int __computeSignal (unsigned int trap_number)
{
    switch (trap_number)
    {
    case CYGNUM_HAL_VECTOR_BUS_FAULT:      // Fall through
    case CYGNUM_HAL_VECTOR_MEMORY_MAN:     // Fall through
        return SIGBUS;
    case CYGNUM_HAL_VECTOR_NMI:
    case CYGNUM_HAL_VECTOR_SYS_TICK:
        return SIGINT;
    default:
        return SIGTRAP;
    }
}


//==========================================================================
/* Return the trap number corresponding to the last-taken trap. */

int __get_trap_number (void)
{
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return _hal_registers->u.exception.vector;
}


//==========================================================================
/* Set the currently-saved pc register value to PC. */

void set_pc (target_register_t pc)
{
    put_register (PC, pc);
}

//==========================================================================
// Calculate byte offset a given register from start of register save area.

static int
reg_offset(regnames_t reg)
{
    int base_offset;

    if (reg < F0)
	return reg * 4;

    base_offset = 16 * 4;

    if (reg < FPS)
	return base_offset + ((reg - F0) * 12);

    base_offset += (8 * 12);

    if (reg <= PS)
	return base_offset + ((reg - FPS) * 4);

    return -1;  // Should never happen!
}


//==========================================================================
// Return the currently-saved value corresponding to register REG of
// the exception context.

target_register_t 
get_register (regnames_t reg)
{
    target_register_t val;
    int offset = reg_offset(reg);

    if (REGSIZE(reg) > sizeof(target_register_t) || offset == -1)
	return -1;

    val = _registers[offset/sizeof(target_register_t)];

    return val;
}

//==========================================================================
// Store VALUE in the register corresponding to WHICH in the exception
// context.

void 
put_register (regnames_t which, target_register_t value)
{
    int offset = reg_offset(which);

    if (REGSIZE(which) > sizeof(target_register_t) || offset == -1)
	return;

    _registers[offset/sizeof(target_register_t)] = value;
}

//==========================================================================
// Write the contents of register WHICH into VALUE as raw bytes. This
// is only used for registers larger than sizeof(target_register_t).
// Return non-zero if it is a valid register.

int
get_register_as_bytes (regnames_t which, char *value)
{
    int offset = reg_offset(which);

    if (offset != -1) {
	memcpy (value, (char *)_registers + offset, REGSIZE(which));
	return 1;
    }
    return 0;
}

//==========================================================================
// Alter the contents of saved register WHICH to contain VALUE. This
// is only used for registers larger than sizeof(target_register_t).
// Return non-zero if it is a valid register.

int
put_register_as_bytes (regnames_t which, char *value)
{
    int offset = reg_offset(which);

    if (offset != -1) {
	memcpy ((char *)_registers + offset, value, REGSIZE(which));
	return 1;
    }
    return 0;
}

//==========================================================================
// Single step the processor.
//
// We do this by setting the MON_STEP bit in the DEMCR. So long as we
// are in a DebugMonitor exception this will single step the CPU on
// return.
// We also need to block all pending interrupts by setting basepri
// before doing the step. Otherwise an interrupt may be delivered
// before the step happens, and may cause unpleasant things to happen.

cyg_uint32 __single_step_basepri = 0;

void __single_step (void)
{
    CYG_ADDRESS base = CYGARC_REG_DEBUG_BASE;
    cyg_uint32 demcr;

    // Save basepri and set it to mask all interrupts.
    __single_step_basepri = _hal_registers->u.exception.basepri;
    _hal_registers->u.exception.basepri = CYGNUM_HAL_CORTEXM_PRIORITY_MAX;

    // Set MON_STEP
    HAL_READ_UINT32( base+CYGARC_REG_DEBUG_DEMCR, demcr );
    demcr |= CYGARC_REG_DEBUG_DEMCR_MON_STEP;
    HAL_WRITE_UINT32( base+CYGARC_REG_DEBUG_DEMCR, demcr );

    // Clear any bits set in DFSR
    base = CYGARC_REG_NVIC_BASE;
    HAL_WRITE_UINT32( base+CYGARC_REG_NVIC_DFSR, 0xFFFFFFFF );    
    
}

//==========================================================================
// Clear the single-step state.

void __clear_single_step (void)
{
    CYG_ADDRESS base = CYGARC_REG_DEBUG_BASE;
    cyg_uint32 demcr;

    // Restore basepri
    _hal_registers->u.exception.basepri = __single_step_basepri;

    // Clear MON_STEP
    HAL_READ_UINT32( base+CYGARC_REG_DEBUG_DEMCR, demcr );
    demcr &= ~CYGARC_REG_DEBUG_DEMCR_MON_STEP;
    HAL_WRITE_UINT32( base+CYGARC_REG_DEBUG_DEMCR, demcr );

    // Clear any bits set in DFSR
    base = CYGARC_REG_NVIC_BASE;
    HAL_WRITE_UINT32( base+CYGARC_REG_NVIC_DFSR, 0xFFFFFFFF );        
}

//==========================================================================

void __install_breakpoints (void)
{
    __install_breakpoint_list();
}

//--------------------------------------------------------------------------

void __clear_breakpoints (void)
{
    __clear_breakpoint_list();
}

//--------------------------------------------------------------------------
/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */

int
__is_breakpoint_function ()
{
    return get_register (PC) == (target_register_t)&_breakinst;
}


//--------------------------------------------------------------------------
/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
   we can safely just skip 2. */

void __skipinst (void)
{
    unsigned long pc = get_register(PC);

    pc += 2;

    put_register(PC, pc);
}

//==========================================================================
#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
