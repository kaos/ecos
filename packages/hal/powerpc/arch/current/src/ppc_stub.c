//========================================================================
//
//      ppc_stub.c
//
//      Helper functions for stub, generic to all PowerPC processors
//
//========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Red Hat, jskov
// Contributors:  Red Hat, jskov
// Date:          1998-08-20
// Purpose:       
// Description:   Helper functions for stub, generic to all PowerPC processors
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

#include <stddef.h>

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <cyg/hal/ppc_regs.h>

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

/* Given a trap value TRAP, return the corresponding signal. */

int __computeSignal (unsigned int trap_number)
{
    switch (trap_number)
    {
    case CYGNUM_HAL_VECTOR_MACHINE_CHECK:
        /* Machine check */
    case CYGNUM_HAL_VECTOR_DSI:
        /* Data access */
        return SIGSEGV;
      
    case CYGNUM_HAL_VECTOR_ISI:
        /* Instruction access (Ifetch) */
    case CYGNUM_HAL_VECTOR_ALIGNMENT:
        /* Data access */
        return SIGBUS;
                    
    case CYGNUM_HAL_VECTOR_INTERRUPT:
        /* External interrupt */
      return SIGINT;

    case CYGNUM_HAL_VECTOR_TRACE:
        /* Instruction trace */
        return SIGTRAP;
      
    case CYGNUM_HAL_VECTOR_PROGRAM:
        // The register PS contains the value of SRR1 at the time of
        // exception entry. Bits 11-15 contain information about the
        // cause of the exception. Bits 16-31 the PS (MSR) state.
        switch ((get_register (PS) >> 17) & 0xf){
        case 1:                         /* trap */
            return SIGTRAP;
        case 2:                         /* privileged instruction */
        case 4:                         /* illegal instruction */
            return SIGILL;
        case 8:                         /* floating point */
            return SIGFPE;
        default:                        /* should never happen! */
            return SIGTERM;
        }            

    case CYGNUM_HAL_VECTOR_RESERVED_A:
    case CYGNUM_HAL_VECTOR_RESERVED_B:
        return SIGILL;

    case CYGNUM_HAL_VECTOR_FP_UNAVAILABLE:
        /* FPU disabled */
    case CYGNUM_HAL_VECTOR_FP_ASSIST:
        /* FPU assist */
        return SIGFPE;

    case CYGNUM_HAL_VECTOR_DECREMENTER:
        /* Decrementer alarm */
        return SIGALRM;

    case CYGNUM_HAL_VECTOR_SYSTEM_CALL:
        /* System call */
        return SIGSYS;

#if defined(CYG_HAL_POWERPC_MPC8xx)
    case CYGNUM_HAL_VECTOR_SW_EMUL:
        /* A SW_EMUL is generated instead of PROGRAM for illegal
           instructions. */
        return SIGILL;

    case CYGNUM_HAL_VECTOR_DATA_BP:
    case CYGNUM_HAL_VECTOR_INSTRUCTION_BP:
    case CYGNUM_HAL_VECTOR_PERIPHERAL_BP:
    case CYGNUM_HAL_VECTOR_NMI:
        /* Developer port debugging exceptions. */
        return SIGTRAP;

    case CYGNUM_HAL_VECTOR_ITLB_MISS:
    case CYGNUM_HAL_VECTOR_DTLB_MISS:
        /* Software reload of TLB required. */
        return SIGTRAP;

    case CYGNUM_HAL_VECTOR_ITLB_ERROR:
        /* Invalid instruction access. */
        return SIGBUS;

    case CYGNUM_HAL_VECTOR_DTLB_ERROR:
        /* Invalid data access. */
        return SIGSEGV;
#endif // defined(CYG_HAL_POWERPC_MPC8xx)
        
    default:
        return SIGTERM;
    }
}


/* Return the trap number corresponding to the last-taken trap. */

int __get_trap_number (void)
{
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return _hal_registers->vector >> 8;
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

static target_register_t irq_state = 0;

void __single_step (void)
{
    target_register_t msr = get_register (PS);

    // Set single-step flag in the exception context.
    msr |= (MSR_SE | MSR_BE);
    // Disable interrupts.
    irq_state = msr & MSR_EE;
    msr &= ~MSR_EE;

    put_register (PS, msr);
}

/* Clear the single-step state. */

void __clear_single_step (void)
{
    target_register_t msr = get_register (PS);

    // Clear single-step flag in the exception context.
    msr &= ~(MSR_SE | MSR_BE);
    // Restore interrupt state.
    // FIXME: Should check whether the executed instruction changed the
    // interrupt state - or single-stepping a MSR changing instruction
    // may result in a wrong EE. Not a very likely scenario though.
    msr |= irq_state;

    // This function is called much more than its counterpart
    // __single_step.  Only re-enable interrupts if they where
    // disabled during the previous cal to __single_step. Otherwise,
    // this function only makes "extra sure" that no trace or branch
    // exception will happen.
    irq_state = 0;

    put_register (PS, msr);
}


void __install_breakpoints (void)
{
    /* NOP since single-step HW exceptions are used instead of
       breakpoints. */
}

void __clear_breakpoints (void)
{
}


/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */

int
__is_breakpoint_function ()
{
    return get_register (PC) == (target_register_t)&CYG_LABEL_NAME(breakinst);
}


/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
   we can safely just skip 4. */

void __skipinst (void)
{
    put_register (PC, get_register (PC) + 4);
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
