//==========================================================================
//
//      sh_stub.c
//
//      GDB Stub code for SH
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov
// Contributors: jskov, Ben Lee, Steve Chamberlain
// Date:         1999-05-18
// Description:  GDB Stub support for sh CPU.
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <stddef.h>

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/sh_regs.h>

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

/* Given a trap value TRAP, return the corresponding signal. */

int 
__computeSignal(unsigned int trap_number)
{
    switch( trap_number ) {
    case CYGNUM_HAL_EXCEPTION_TRAP:
    case CYGNUM_HAL_EXCEPTION_INSTRUCTION_BP:
        return SIGTRAP;

    case CYGNUM_HAL_EXCEPTION_POWERON:
    case CYGNUM_HAL_EXCEPTION_RESET:
        // Reset - given that the CPU resets if it gets confused we
        // want to treat it as an interupt so the developer has a
        // chance to find out what happened (as opposed to SIGTERM).
        return SIGINT;

    case CYGNUM_HAL_EXCEPTION_TLBMISS_ACCESS:
    case CYGNUM_HAL_EXCEPTION_TLBMISS_WRITE:
    case CYGNUM_HAL_EXCEPTION_INITIAL_WRITE:
    case CYGNUM_HAL_EXCEPTION_TLBERROR_ACCESS:
    case CYGNUM_HAL_EXCEPTION_TLBERROR_WRITE:
        return SIGSEGV;

    case CYGNUM_HAL_EXCEPTION_DATA_ACCESS:
    case CYGNUM_HAL_EXCEPTION_DATA_WRITE:
           return SIGBUS;

    case CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION:
    case CYGNUM_HAL_EXCEPTION_ILLEGAL_SLOT_INSTRUCTION:
        return SIGILL;

    default:
        return SIGTERM;
    }
}

/* Return the trap number corresponding to the last-taken trap. */

int 
__get_trap_number(void)
{
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return _hal_registers->event;
}

/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */

void 
set_pc(target_register_t pc)
{
    put_register(PC, pc);
}

#ifdef CYGARC_SH_MOD_UBC

// This implementation of the single-stepper relies on the User Break
// Controller which may not be available on all cores.

// Note: This should be enhanced to coorperate with either two regular
// breakpoints or watchpoints. Requires GDB to be aware of the stub's
// ability though, so for now just use channel A without further
// considerations.

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */

void __single_step (void)
{
    // The address of the instruction to execute.
    HAL_WRITE_UINT32(CYGARC_REG_BARA, get_register(PC));
    // Match entire address.
    HAL_WRITE_UINT8(CYGARC_REG_BAMRA, CYGARC_REG_BAMRA_BARA_UNMASKED);
    // Stop after instruction at matching address has executed.
    HAL_WRITE_UINT16(CYGARC_REG_BRCR, CYGARC_REG_BRCR_PCBA);
    // Stop on IFETCH/READ
    HAL_WRITE_UINT16(CYGARC_REG_BBRA, 
                     CYGARC_REG_BBRA_IFETCH|CYGARC_REG_BBRA_READ);
}

/* Clear the single-step state. */

void __clear_single_step (void)
{
    // Don't stop on any condition
    HAL_WRITE_UINT16(CYGARC_REG_BBRA, 0);
    // Clear status flags
    HAL_WRITE_UINT16(CYGARC_REG_BRCR, 0);
}

#else // CYGARC_SH_MOD_UBC

/*----------------------------------------------------------------------
 * Single-step support, copied from gdb/sh-stub.c, written by Ben Lee
 * and Steve Chamberlain. Extended to handle braf, bsrf, bt/s and bf/s.
 */

#define COND_BRx_MASK           0xfb00
#define UCOND_DBR_MASK          0xe000
#define UCOND_RBR_MASK          0xf0df
#define TRAPA_MASK              0xff00

#define COND_DISP               0x00ff
#define UCOND_DISP              0x0fff
#define UCOND_REG               0x0f00

#define BFx_INSTR               0x8b00
#define BTx_INSTR               0x8900
#define BRA_INSTR               0xa000
#define BSR_INSTR               0xb000
#define JMP_INSTR               0x402b
#define JSR_INSTR               0x400b
#define RTS_INSTR               0x000b
#define RTE_INSTR               0x002b
#define TRAPA_INSTR             0xc300
#define BxxF_INSTR              0x0003

#define SSTEP_INSTR             0xc3ff

#define T_BIT_MASK              0x0001

typedef struct {
    short *memAddr;
    short oldInstr;
} stepData;

static stepData instrBuffer;
static char stepped;

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */

void __single_step (void)
{
    short *instrMem;
    int displacement;
    int reg;
    unsigned short opcode;

    cyg_uint32 pc = get_register(PC);
    cyg_uint32 sr = get_register(SR);

    instrMem = (short *) pc;

    opcode = *instrMem;
    stepped = 1;

    if ((opcode & UCOND_RBR_MASK) == BxxF_INSTR) { 
        reg = (char) ((opcode & UCOND_REG) >> 8);
        displacement = get_register(reg);
        instrMem = (short *) (pc + displacement + 4);
            /*
             * Remember PC points to second instr.
             * after PC of branch ... so add 4
             */
    } else if ((opcode & COND_BRx_MASK) == BTx_INSTR) {
        if (sr & T_BIT_MASK) {
            displacement = (opcode & COND_DISP) << 1;
            if (displacement & 0x80) {
                displacement |= 0xffffff00;
            }
            /*
             * Remember PC points to second instr.
             * after PC of branch ... so add 4
             */
            instrMem = (short *) (pc + displacement + 4);
        } else {
            instrMem += 1;
        }
    } else if ((opcode & COND_BRx_MASK) == BFx_INSTR) {
        if (sr & T_BIT_MASK) {
            instrMem += 1;
        } else {
            displacement = (opcode & COND_DISP) << 1;
            if (displacement & 0x80)
                displacement |= 0xffffff00;
            /*
             * Remember PC points to second instr.
             * after PC of branch ... so add 4
             */
            instrMem = (short *) (pc + displacement + 4);
        }
    } else if ((opcode & UCOND_DBR_MASK) == BRA_INSTR) {
        displacement = (opcode & UCOND_DISP) << 1;
        if (displacement & 0x0800) {
            displacement |= 0xfffff000;
        }
        
        /*
         * Remember PC points to second instr.
         * after PC of branch ... so add 4
         */
        instrMem = (short *) (pc + displacement + 4);
    } else if ((opcode & UCOND_RBR_MASK) == JSR_INSTR) {
        reg = (char) ((opcode & UCOND_REG) >> 8);
        instrMem = (short *) get_register(reg);
    } else if (opcode == RTS_INSTR) {
        instrMem = (short *) get_register(PR);
    } else if (opcode == RTE_INSTR) {
        instrMem = (short *) get_register(SP);
    } else if ((opcode & TRAPA_MASK) == TRAPA_INSTR) {
        instrMem += 1;                  // skip traps
    } else {
        instrMem += 1;
    }
    
    instrBuffer.memAddr = instrMem;
    instrBuffer.oldInstr = *instrMem;
    *instrMem = SSTEP_INSTR;
}

/* Clear the single-step state. */

void __clear_single_step (void)
{
    /* Undo the effect of a previous doSStep.  If we single stepped,
       restore the old instruction. */

    if (stepped) {
        short *instrMem;
        instrMem = instrBuffer.memAddr;
        *instrMem = instrBuffer.oldInstr;
    }
    stepped = 0;
}

#endif // CYGARC_SH_MOD_UBC


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
    return get_register(PC) == (target_register_t)&_breakinst;
}


/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
   we can safely just skip 2. */
void __skipinst (void)
{
    put_register(PC, get_register (PC) + 2);
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
