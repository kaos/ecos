//========================================================================
//
//      arm_stub.c
//
//      Helper functions for stub, generic to all ARM processors
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
// Author(s):     Red Hat, gthomas
// Contributors:  Red Hat, gthomas, jskov
// Date:          1998-11-26
// Purpose:       
// Description:   Helper functions for stub, generic to all ARM processors
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

#include <stddef.h>

#include <pkgconf/hal.h>
#ifdef CYGPKG_CYGMON
#include <pkgconf/cygmon.h>
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#ifdef CYGPKG_HAL_ARM_SIM
#error "GDB Stub support not implemented for ARM SIM"
#endif

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

// Use bit 0 as a thumb-mode flag for next address to be executed.
// Alternative would be to keep track of it using a C variable, but
// since bit 0 is used by the BX instruction, we might as well do the
// same thing and thus avoid checking two different flags.
#define IS_THUMB_ADDR(addr)     ((addr) & 1)
#define MAKE_THUMB_ADDR(addr) ((addr) | 1)
#define UNMAKE_THUMB_ADDR(addr) ((addr) & ~1)

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

/* Given a trap value TRAP, return the corresponding signal. */

int __computeSignal (unsigned int trap_number)
{
    // should also catch CYGNUM_HAL_VECTOR_UNDEF_INSTRUCTION here but we
    // can't tell the different between a real one and a breakpoint :-(
    switch (trap_number) {
    case CYGNUM_HAL_VECTOR_ABORT_PREFETCH:      // Fall through
    case CYGNUM_HAL_VECTOR_ABORT_DATA:          // Fall through
    case CYGNUM_HAL_VECTOR_reserved:
        return SIGBUS;
    case CYGNUM_HAL_VECTOR_IRQ:
    case CYGNUM_HAL_VECTOR_FIQ:
        return SIGINT;
    default:
        return SIGTRAP;
    }
}


/* Return the trap number corresponding to the last-taken trap. */
int __get_trap_number (void)
{
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return _hal_registers->vector;
}

/* Set the currently-saved pc register value to PC. */

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

static unsigned long  ss_saved_pc = 0;
static unsigned long  ss_saved_instr;
static unsigned short ss_saved_thumb_instr;

#define FIXME() {diag_printf("FIXME - %s\n", __FUNCTION__); }

static int
ins_will_execute(unsigned long ins)
{
    unsigned long psr = get_register(PS);  // condition codes
    int res = 0;
    switch ((ins & 0xF0000000) >> 28) {
    case 0x0: // EQ
        res = (psr & PS_Z) != 0;
        break;
    case 0x1: // NE
        res = (psr & PS_Z) == 0;
        break;
    case 0x2: // CS
        res = (psr & PS_C) != 0;
        break;
    case 0x3: // CC
        res = (psr & PS_C) == 0;
        break;
    case 0x4: // MI
        res = (psr & PS_N) != 0;
        break;
    case 0x5: // PL
        res = (psr & PS_N) == 0;
        break;
    case 0x6: // VS
        res = (psr & PS_V) != 0;
        break;
    case 0x7: // VC
        res = (psr & PS_V) == 0;
        break;
    case 0x8: // HI
        res = ((psr & PS_C) != 0) && ((psr & PS_Z) == 0);
        break;
    case 0x9: // LS
        res = ((psr & PS_C) == 0) || ((psr & PS_Z) != 0);
        break;
    case 0xA: // GE
        res = ((psr & (PS_N|PS_V)) == (PS_N|PS_V)) ||
            ((psr & (PS_N|PS_V)) == 0);
        break;
    case 0xB: // LT
        res = ((psr & (PS_N|PS_V)) == PS_N) ||
            ((psr & (PS_N|PS_V)) == PS_V);
        break;
    case 0xC: // GT
        res = ((psr & (PS_N|PS_V)) == (PS_N|PS_V)) ||
            ((psr & (PS_N|PS_V)) == 0);
        res = ((psr & PS_Z) == 0) && res;
        break;
    case 0xD: // LE
        res = ((psr & (PS_N|PS_V)) == PS_N) ||
            ((psr & (PS_N|PS_V)) == PS_V);
        res = ((psr & PS_Z) == PS_Z) || res;
        break;
    case 0xE: // AL
        res = TRUE;
        break;
    case 0xF: // NV
        res = FALSE;
        break;
    }
    return res;
}

static unsigned long
RmShifted(int shift)
{
    unsigned long Rm = get_register(shift & 0x00F);
    int shift_count;
    if ((shift & 0x010) == 0) {
        shift_count = (shift & 0xF80) >> 7;
    } else {
        shift_count = get_register((shift & 0xF00) >> 8);
    }
    switch ((shift & 0x060) >> 5) {
    case 0x0: // Logical left
        Rm <<= shift_count;
        break;
    case 0x1: // Logical right
        Rm >>= shift_count;
        break;
    case 0x2: // Arithmetic right
        Rm = (unsigned long)((long)Rm >> shift_count);
        break;
    case 0x3: // Rotate right
        if (shift_count == 0) {
            // Special case, RORx
            Rm >>= 1;
            if (get_register(PS) & PS_C) Rm |= 0x80000000;
        } else {
            Rm = (Rm >> shift_count) | (Rm << (32-shift_count));
        }
        break;
    }
    return Rm;
}

// Decide the next instruction to be executed for a given instruction
static unsigned long *
target_ins(unsigned long *pc, unsigned long ins)
{
    unsigned long new_pc, offset, op2;
    unsigned long Rn;
    int i, reg_count, c;
    switch ((ins & 0x0C000000) >> 26) {
    case 0x0:
        // BX
        if ((ins & 0x0FFFFFF0) == 0x012FFF10) {
            new_pc = (unsigned long)get_register(ins & 0x0000000F);
            return ((unsigned long *)new_pc);
        }
        // Data processing
        new_pc = (unsigned long)(pc+1);
        if ((ins & 0x0000F000) == 0x0000F000) {
            // Destination register is PC
            if ((ins & 0x0FBF0000) != 0x010F0000) {
                Rn = (unsigned long)get_register((ins & 0x000F0000) >> 16);
                if ((ins & 0x02000000) == 0) {
                    op2 = RmShifted(ins & 0x00000FFF);
                } else {
                    op2 = ins & 0x000000FF;
                    i = (ins & 0x00000F00) >> 8;  // Rotate count                
                    op2 = (op2 >> (i*2)) | (op2 << (32-(i*2)));
                }
                switch ((ins & 0x01E00000) >> 21) {
                case 0x0: // AND
                    new_pc = Rn & op2;
                    break;
                case 0x1: // EOR
                    new_pc = Rn ^ op2;
                    break;
                case 0x2: // SUB
                    new_pc = Rn - op2;
                    break;
                case 0x3: // RSB
                    new_pc = op2 - Rn;
                    break;
                case 0x4: // ADD
                    new_pc = Rn + op2;
                    break;
                case 0x5: // ADC
                    c = (get_register(PS) & PS_C) != 0;
                    new_pc = Rn + op2 + c;
                    break;
                case 0x6: // SBC
                    c = (get_register(PS) & PS_C) != 0;
                    new_pc = Rn - op2 + c - 1;
                    break;
                case 0x7: // RSC
                    c = (get_register(PS) & PS_C) != 0;
                    new_pc = op2 - Rn +c - 1;
                    break;
                case 0x8: // TST
                case 0x9: // TEQ
                case 0xA: // CMP
                case 0xB: // CMN
                    break; // PC doesn't change
                case 0xC: // ORR
                    new_pc = Rn | op2;
                    break;
                case 0xD: // MOV
                    new_pc = op2;
                    break;
                case 0xE: // BIC
                    new_pc = Rn & ~op2;
                    break;
                case 0xF: // MVN
                    new_pc = ~op2;
                    break;
                }
            }
        }
        return ((unsigned long *)new_pc);
    case 0x1:
        if ((ins & 0x02000010) == 0x02000010) {
            // Undefined!
            return (pc+1);
        } else {
            if ((ins & 0x00100000) == 0) {
                // STR
                return (pc+1);
            } else {
                // LDR
                if ((ins & 0x0000F000) != 0x0000F000) {
                    // Rd not PC
                    return (pc+1);
                } else {
                    Rn = (unsigned long)get_register((ins & 0x000F0000) >> 16);
                    if ((ins & 0x000F0000) == 0x000F0000) Rn += 8;  // PC prefetch!
                    if (ins & 0x01000000) {
                        // Add/subtract offset before
                        if ((ins & 0x02000000) == 0) {
                            // Immediate offset
                            if (ins & 0x00800000) {
                                // Add offset
                                Rn += (ins & 0x00000FFF);
                            } else {
                                // Subtract offset
                                Rn -= (ins & 0x00000FFF);
                            }
                        } else {
                            // Offset is in a register
                            if (ins & 0x00800000) {
                                // Add offset
                                Rn += RmShifted(ins & 0x00000FFF);
                            } else {
                                // Subtract offset
                                Rn -= RmShifted(ins & 0x00000FFF);
                            }
                        }
                    }
                    return ((unsigned long *)*(unsigned long *)Rn);
                }
            }
        }
        return (pc+1);
    case 0x2:  // Branch, LDM/STM
        if ((ins & 0x02000000) == 0) {
            // LDM/STM
            if ((ins & 0x00100000) == 0) {
                // STM
                return (pc+1);
            } else {
                // LDM
                if ((ins & 0x00008000) == 0) {
                    // PC not in list
                    return (pc+1);
                } else {
                    Rn = (unsigned long)get_register((ins & 0x000F0000) >> 16);
                    if ((ins & 0x000F0000) == 0x000F0000) Rn += 8;  // PC prefetch!
                    offset = ins & 0x0000FFFF;
                    reg_count = 0;
                    for (i = 0;  i < 15;  i++) {
                        if (offset & (1<<i)) reg_count++;
                    }                    
                    if (ins & 0x00800000) {
                        // Add offset
                        Rn += reg_count*4;
                    } else {
                        // Subtract offset
                        Rn -= 4;
                    }
                    return ((unsigned long *)*(unsigned long *)Rn);
                }
            }
        } else {
            // Branch
            if (ins_will_execute(ins)) {
                offset = (ins & 0x00FFFFFF) << 2;
                if (ins & 0x00800000) offset |= 0xFC000000;  // sign extend
                new_pc = (unsigned long)(pc+2) + offset;
                return ((unsigned long *)new_pc);
            } else {
                // Falls through
                return (pc+1);
            }
        }
    case 0x3:  // Coprocessor & SWI
        return (pc+1);
    default:
        // Never reached - but fixes compiler warning.
        return 0;
    }
}

// FIXME: target_ins also needs to check for CPSR/THUMB being set and
//        set the thumb bit accordingly.

static unsigned long
target_thumb_ins(unsigned long pc, unsigned short ins)
{
    unsigned long new_pc = MAKE_THUMB_ADDR(pc+2); // default is fall-through 
                                        // to next thumb instruction
    unsigned long offset, arm_ins;

    switch ((ins & 0xf000) >> 12) {
    case 0x4:
        // Check for BX
        if ((ins & 0xff87) == 0x4700)
            new_pc = (unsigned long)get_register((ins & 0x00078) >> 3);
        break;
    case 0xd:
        // Bcc
        // Use ARM function to check condition
        arm_ins = ((unsigned long)(ins & 0x0f00)) << 20;
        if (ins_will_execute(arm_ins)) {
            offset = (ins & 0x00FF) << 1;
            if (ins & 0x0080) offset |= 0xFFFFFE00;  // sign extend
            new_pc = MAKE_THUMB_ADDR((unsigned long)(pc+4) + offset);
        }
        break;
    case 0xe:
        // check for B
        if ((ins & 0x0800) == 0) {
            offset = (ins & 0x07FF) << 1;
            if (ins & 0x0400) offset |= 0xFFFFF800;  // sign extend
            new_pc = MAKE_THUMB_ADDR((unsigned long)(pc+4) + offset);
        }
        break;
    case 0xf:
        // BL (4byte instruction!)
        // First instruction (bit 11 == 0) holds top-part of offset
        offset = (ins & 0x07FF) << 12;
        if (ins & 0x0400) offset |= 0xFF800000;  // sign extend
        // Get second instruction
        // Second instruction (bit 11 == 1) holds bottom-part of offset
        ins = *(unsigned short*)(pc+2);
        offset |= (ins & 0x07ff) << 1;
        new_pc = MAKE_THUMB_ADDR((unsigned long)(pc+4) + offset);
        break;
    }

    return new_pc;
}

void __single_step (void)
{
    unsigned long pc = get_register(PC);
    unsigned long cpsr = get_register(PS);

    // Calculate address of next instruction to be executed
    if (cpsr & CPSR_THUMB_ENABLE) {
        // thumb
        ss_saved_pc = target_thumb_ins(pc, *(unsigned short*)pc);
    } else {
        // ARM
        unsigned long curins = *(unsigned long*)pc;
        if (ins_will_execute(curins)) {
            // Decode instruction to decide what the next PC will be
            ss_saved_pc = (unsigned long) target_ins((unsigned long*)pc, 
                                                     curins);
        } else {
            // The current instruction will not execute (the conditions 
            // don't hold)
            ss_saved_pc = pc+4;
        }
    }

    // Set breakpoint according to type
    if (IS_THUMB_ADDR(ss_saved_pc)) {
        // Thumb instruction
        unsigned long t_pc = UNMAKE_THUMB_ADDR(ss_saved_pc);
        ss_saved_thumb_instr = *(unsigned short*)t_pc;
        *(unsigned short*)t_pc = HAL_BREAKINST_THUMB;
    } else {
        // ARM instruction
        ss_saved_instr = *(unsigned long*)ss_saved_pc;
        *(unsigned long*)ss_saved_pc = HAL_BREAKINST_ARM;
    }
}

/* Clear the single-step state. */

void __clear_single_step (void)
{
    if (ss_saved_pc != 0) {
        // Restore instruction according to type
        if (IS_THUMB_ADDR(ss_saved_pc)) {
            // Thumb instruction
            unsigned long t_pc = UNMAKE_THUMB_ADDR(ss_saved_pc);
            *(unsigned short*)t_pc = ss_saved_thumb_instr;
        } else {
            // ARM instruction
            *(unsigned long*)ss_saved_pc = ss_saved_instr;
        }
        ss_saved_pc = 0;
    }
}

#if !defined(CYGPKG_CYGMON)
void __install_breakpoints (void)
{
//    FIXME();
}

void __clear_breakpoints (void)
{
//    FIXME();
}
#endif // !CYGPKG_CYGMON

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
    unsigned long pc = get_register(PC);
    unsigned long cpsr = get_register(PS);

    if (cpsr & CPSR_THUMB_ENABLE)
        pc += 2;
    else
        pc += 4;

    put_register(PC, pc);
}

//-----------------------------------------------------------------------
// Thumb-aware GDB interrupt handler.
// This is a brute-force replacement of the ones in hal_stub.c. Need to
// find a better way of handling it... Maybe... Probably only ARM/thumb
// that is this weird.

typedef struct
{
    cyg_uint32 targetAddr;
    union {
        cyg_uint32 arm_instr;
        cyg_uint16 thumb_instr;
    } savedInstr;
} instrBuffer;

static instrBuffer break_buffer;

volatile int cyg_hal_gdb_running_step = 0;

void 
cyg_hal_gdb_interrupt (target_register_t pc)
{
    // Clear flag that we Continued instead of Stepping
    cyg_hal_gdb_running_step = 0;
    // and override existing break? So that a ^C takes effect...
    if (0 != break_buffer.targetAddr)
        cyg_hal_gdb_remove_break( break_buffer.targetAddr );

    if (0 == break_buffer.targetAddr) {
        cyg_uint32 cpsr = get_register(PS);

        if (cpsr & CPSR_THUMB_ENABLE) {
            break_buffer.targetAddr = MAKE_THUMB_ADDR((cyg_uint32)pc);
            break_buffer.savedInstr.thumb_instr = *(cyg_uint16*)pc;
            *(cyg_uint16*)pc = HAL_BREAKINST_THUMB;
        } else {
            break_buffer.targetAddr = (cyg_uint32)pc;
            break_buffer.savedInstr.arm_instr = *(cyg_uint32*)pc;
            *(cyg_uint32*)pc = HAL_BREAKINST_ARM;
        }

        __data_cache(CACHE_FLUSH);
        __instruction_cache(CACHE_FLUSH);
    }
}

// This function is passed thumb/arm information about the PC address
// in bit 0. This information is passed on to the break_buffer.
void 
cyg_hal_gdb_place_break (target_register_t pc)
{
    // Clear flag that we Continued instead of Stepping
    cyg_hal_gdb_running_step = 0;

    if (0 == break_buffer.targetAddr) {
        // Setting a breakpoint in Thumb or ARM code?
       if (IS_THUMB_ADDR(pc)) {
            break_buffer.targetAddr = (cyg_uint32)pc;
            pc = UNMAKE_THUMB_ADDR(pc);
            break_buffer.savedInstr.thumb_instr = *(cyg_uint16*)pc;
            *(cyg_uint16*)pc = HAL_BREAKINST_THUMB;
        } else {
            break_buffer.targetAddr = (cyg_uint32)pc;
            break_buffer.savedInstr.arm_instr = *(cyg_uint32*)pc;
            *(cyg_uint32*)pc = HAL_BREAKINST_ARM;
        }
        
        __data_cache(CACHE_FLUSH);
        __instruction_cache(CACHE_FLUSH);
    }
}

int 
cyg_hal_gdb_remove_break (target_register_t pc)
{
    if ( cyg_hal_gdb_running_step )
        return 0; // Do not remove the break: we must hit it!

    if (pc == UNMAKE_THUMB_ADDR(break_buffer.targetAddr)) {
        if (IS_THUMB_ADDR(break_buffer.targetAddr)) {
            *(cyg_uint16*)pc = break_buffer.savedInstr.thumb_instr;
        } else {
            *(cyg_uint32*)pc = break_buffer.savedInstr.arm_instr;
        }
        break_buffer.targetAddr = 0;

        __data_cache(CACHE_FLUSH);
        __instruction_cache(CACHE_FLUSH);
        return 1;
    }
    return 0;
}

int
cyg_hal_gdb_break_is_set (void)
{
    if (0 != break_buffer.targetAddr) {
        return 1;
    }
    return 0;
}


#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
