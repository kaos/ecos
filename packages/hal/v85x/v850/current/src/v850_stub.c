//========================================================================
//
//      nec_stub.c
//
//      Helper functions for stub, generic to all NEC processors
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Red Hat, gthomas, jlarmour
// Contributors:  Red Hat, gthomas, jskov
// Date:          1998-11-26
// Purpose:       
// Description:   Helper functions for stub, generic to all NEC processors
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

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

/* Given a trap value TRAP, return the corresponding signal. */

int __computeSignal (unsigned int trap_number)
{
    unsigned short curins, *pc;
    switch (trap_number) {
    case CYGNUM_HAL_VECTOR_INTWDT: // watchdog timer NMI
        pc = (unsigned short *)_hal_registers->pc;
        curins = *pc;
        if (curins == 0x0585) {
            // "br *" - used for breakpoint
            return SIGTRAP;
        } else {
            // Anything else - just ignore it happened
            return 0;
        }
    case CYGNUM_HAL_VECTOR_NMI:
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

static unsigned short *ss_saved_pc = 0;
static unsigned short  ss_saved_instr[2];
static int             ss_saved_instr_size;

#define FIXME() {diag_printf("FIXME - %s\n", __FUNCTION__); }

static unsigned short *
next_pc(unsigned short *pc)
{
    unsigned short curins = *pc;
    unsigned short *newpc = pc;

    switch ((curins & 0x0780) >> 7) {
    case 0x0:
        if ((curins & 0x60) == 0x60) {
            int Rn = curins & 0x1F;
            newpc = (unsigned short *)get_register(Rn);
        } else {
            newpc = pc+1;
        }
        break;
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
        // Arithmetic - no branch opcodes
        newpc = pc+1;
        break;
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
        // Load and store - no branch opcodes
        newpc = pc+2;
        break;
    case 0xB:
        // Conditional branch
        if (1) {
            unsigned long psw = get_register(PSW);
#define PSW_SAT 0x10
#define PSW_CY  0x08
#define PSW_OV  0x04
#define PSW_S   0x02
#define PSW_Z   0x01
            long disp = ((curins & 0xF800) >> 8) | ((curins & 0x70) >> 4);    
            int cc = curins & 0x0F;
            int S = (psw & PSW_S) != 0;
            int Z = (psw & PSW_Z) != 0;
            int OV = (psw & PSW_OV) != 0;
            int CY = (psw & PSW_CY) != 0;
            int do_branch = 0;
            if (curins & 0x8000) disp |= 0xFFFFFF00;
            switch (cc) {
            case 0x0: // BV
                do_branch = (OV == 1);
                break;
            case 0x1: // BL
                do_branch = (CY == 1);
                break;
            case 0x2: // BE
                do_branch = (Z == 1);
                break;
            case 0x3: // BNH
                do_branch = ((CY | Z) == 1);
                break;
            case 0x4: // BN
                do_branch = (S == 1);
                break;
            case 0x5: // - always
                do_branch = 1;
                break;
            case 0x6: // BLT
                do_branch = ((S ^ OV) == 1);
                break;
            case 0x7: // BLE
                do_branch = (((S ^ OV) | Z) == 1);
                break;
            case 0x8: // BNV
                do_branch = (OV == 0);
                break;
            case 0x9: // BNL
                do_branch = (CY == 0);
                break;
            case 0xA: // BNE
                do_branch = (Z == 0);
                break;
            case 0xB: // BH
                do_branch = ((CY | Z) == 0);
                break;
            case 0xC: // BP
                do_branch = (S == 0);
                break;
            case 0xD: // BSA
                do_branch = ((psw & PSW_SAT) != 0);
                break;
            case 0xE: // BGE
                do_branch = ((S ^ OV) == 0);
                break;
            case 0xF: // BGT
                do_branch = (((S ^ OV) | Z) == 0);
                break;
            }
            if (do_branch) {
                newpc = pc + disp;
            } else {
                newpc = pc + 1;
            }
        }
        break;
    case 0xC:
    case 0xD:
    case 0xE:
        // Arithmetic & load/store - no branch opcodes
        newpc = pc+2;
        break;
    case 0xF:
        if ((curins & 0x60) >= 0x40) {
            // Bitfield and extended instructions - no branch opcodes
            newpc = pc+2;
        } else {
            // JR/JARL
            long disp = ((curins & 0x3F) << 16) | *(pc+1);
            if (curins & 0x20) disp |= 0xFFC00000;
            newpc = pc + (disp>>1);
        }
    }
    return newpc;
}

void __single_step (void)
{
    unsigned short *pc = (unsigned short *)get_register(PC);
    unsigned short *break_pc;
    unsigned short  _breakpoint[] = {0x07E0, 0x0780};
    unsigned short *breakpoint = _breakpoint;
    // If the current instruction is a branch, decide if the branch will
    // be taken to determine where to set the breakpoint.
    break_pc = next_pc(pc);
    // Now see what kind of breakpoint can be used.
    // Note: since this is a single step, always use the 32 bit version.
    ss_saved_pc = break_pc;
    ss_saved_instr_size = 2;
    ss_saved_instr[0] = *break_pc;
    *break_pc++ = *breakpoint++;
    ss_saved_instr[1] = *break_pc;
    *break_pc++ = *breakpoint++;
}

/* Clear the single-step state. */

void __clear_single_step (void)
{
    unsigned short *pc, *val;
    int i;
    if (ss_saved_instr_size != 0) {
        pc = ss_saved_pc;
        val = ss_saved_instr;
        for (i = 0;  i < ss_saved_instr_size;  i++) {
            *pc++ = *val++;
        }
        ss_saved_instr_size = 0;
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
    return get_register (PC) == (target_register_t)&_breakinst;
}


/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
*/

void __skipinst (void)
{
    unsigned short *pc = (unsigned short *)get_register(PC);
    pc = next_pc(pc);
    put_register(PC, (unsigned long)pc);
}


#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

/* ----------------------------------------------------------------------- */
/* ICE debugging */

#ifdef CYGDBG_HAL_V850_ICE

#include <cyg/infra/cyg_type.h>

/* Magic area for syscall information, from vectors.S */
__externC char hal_v85x_ice_syscall_info[];

/* Syscall operation codes. This is a "contract" with the host. */

#define V850ICE_SYSCALL_GET_THREADNEXT 1
#define V850ICE_SYSCALL_GET_THREADREGS 2
#define V850ICE_SYSCALL_SET_THREADREGS 3
#define V850ICE_SYSCALL_GET_CURRTHREAD 4
#define V850ICE_SYSCALL_GET_THREADINFO 5

/* System call information area layout */
#define ICE_SYSCALL_INFO_VALIDATOR (*(int *)&hal_v85x_ice_syscall_info[0])
#define ICE_SYSCALL_INFO_STARTPC   (*(int *)&hal_v85x_ice_syscall_info[4])
#define ICE_SYSCALL_INFO_ENDPC     (*(int *)&hal_v85x_ice_syscall_info[8])
#define ICE_SYSCALL_INFO_STACK     (*(int *)&hal_v85x_ice_syscall_info[12])
#define ICE_SYSCALL_INFO_BUF       (*(int *)&hal_v85x_ice_syscall_info[16])

/* Number of regs in GDB regs context */
#define V850_GDB_REGS 66

static int ice_stack[128]; // ints so as to ensure alignment
static int ice_iobuf[128];

static void
hal_v85x_ice_syscall_end(void)
{
    for (;;);
}

#define ICEDEBUG 0

#if ICEDEBUG
#include <cyg/infra/diag.h>
#endif

/* We can't use normal memcpy when invoked via the ICE. It may be unsafe */
static void
my_memcpy( void *vd, void *vs, int n)
{
    char *d = vd;
    char *s = vs;

    while (n--)
        *d++=*s++;
}

static void
my_memset( void *vs, char c, int n)
{
    char *s = vs;

    while (n--)
        *s++ = c;
}

static void
hal_v85x_ice_syscall(void)
{
    int code, len;
    code = ice_iobuf[0];
    len = ice_iobuf[1];
    switch (code) {
    case V850ICE_SYSCALL_GET_THREADNEXT:
        {
            int ret;
            threadref currthread, nextthread;
            int thread_id;

            /* Unmarshall thread ref */
            my_memcpy( &currthread, &ice_iobuf[2], 8 );
#if ICEDEBUG
            diag_printf("*NEXTTHREAD* currthread=%08x,%08x\n",
                        *(int *)&currthread[0],
                        *(int *)(((char *)&currthread[0])+4));
#endif
            // null threadref?
            if ((ice_iobuf[2] == 0) &&
                (ice_iobuf[3] == 0)) {
#if ICEDEBUG
                diag_printf("null threadref\n");
#endif
                ret = dbg_threadlist( 1, NULL, &nextthread );
            } else {
#if ICEDEBUG
                diag_printf("non-null threadref\n");
#endif
                ret = dbg_threadlist( 0, &currthread, &nextthread );
            }
#if ICEDEBUG
            diag_printf("*NEXTTHREAD* nextthread=%08x,%08x\n",
                        *(int *)&nextthread[0],
                        *(int *)(((char *)&nextthread[0])+4));
#endif
            if (ret) { // if valid thread found
                thread_id = dbg_thread_id( &nextthread );
                /* Returns 0 on error */
                if (thread_id != 0) {
                    ice_iobuf[1] = thread_id;
                    my_memcpy( &ice_iobuf[2], nextthread, 8 );
                    
                    // set return data size to 12
                    ice_iobuf[0] = 12;
#if ICEDEBUG
                    {
                        int i;
                        for (i=0; i<3; i++)
                            diag_printf("%x, ", ice_iobuf[i]);
                        diag_printf("\n");
                    }
#endif
                } else {
                    ret = 0;
                }
            } 
            if (!ret) {
                // set to null
                my_memset( &ice_iobuf[1], 0, 12 );
            }
        }
        break;
    case V850ICE_SYSCALL_GET_THREADREGS:
        {
            int ret;
            threadref thread;

            /* Unmarshall thread ref */
            my_memcpy( &thread, &ice_iobuf[2], 8 );
#if ICEDEBUG
            diag_printf("*GTHREADREGS* thread=%08x,%08x\n", *(int *)&thread[0],
                        *(int *)(((char *)&thread[0])+4));
#endif
            ret = dbg_getthreadreg( &thread, V850_GDB_REGS, &ice_iobuf[1]);
            if (ret)
                ice_iobuf[0] = V850_GDB_REGS * 4;
            else
                ice_iobuf[0] = 0;
                                    
        }
        break;
    case V850ICE_SYSCALL_SET_THREADREGS:
        {
            int ret;
            threadref thread;

            /* Unmarshall thread ref */
            my_memcpy( &thread, &ice_iobuf[2], 8 );
#if ICEDEBUG
            diag_printf("*STHREADREGS* thread=%08x,%08x\n", *(int *)&thread[0],
                        *(int *)(((char *)&thread[0])+4));
#endif
            ret = dbg_setthreadreg( &thread, V850_GDB_REGS, &ice_iobuf[4]);
            if (ret)
                ice_iobuf[0] = 1;
            else
                ice_iobuf[0] = 0;
        }
        break;
    case V850ICE_SYSCALL_GET_CURRTHREAD:
        {
            int ret, thread_id;
            threadref thread;

            ret = dbg_currthread( &thread );
#if ICEDEBUG
            diag_printf("*CURRTHREAD* thread=%08x,%08x\n", *(int *)&thread[0],
                        *(int *)(((char *)&thread[0])+4));
#endif
            
            if (ret) {
                thread_id = dbg_thread_id( &thread );
                /* Returns 0 on error */
                if (thread_id != 0) {
                    ice_iobuf[1] = thread_id;
                    my_memcpy( &ice_iobuf[2], thread, 8 );
                }
                else {
                    ret = 0;
                }
            }
            if (ret)
                ice_iobuf[0] = 12;
            else
                ice_iobuf[0] = 0;
        } 
        break;
    case V850ICE_SYSCALL_GET_THREADINFO:
        {
            int ret;
            threadref thread;
            struct cygmon_thread_debug_info info;
            char *s=(char *)&ice_iobuf[1], *p;

            /* Unmarshall thread ref */
            my_memcpy( &thread, &ice_iobuf[2], 8 );
#if ICEDEBUG
            diag_printf("*INFO* thread=%08x,%08x\n", *(int *)&thread[0],
                        *(int *)(((char *)&thread[0])+4));
#endif

            ret = dbg_threadinfo( &thread, &info );
            if (ret) {
                if (info.thread_display) {
                    my_memcpy (s, "State: ", 7);
                    s += 7;
                    p = info.thread_display;
                    while (*p) {
                        *s++ = *p++;
                    }
                }

                if (info.unique_thread_name && info.unique_thread_name[0]) {
                    my_memcpy (s, ", Name: ", 8);
                    s += 8;
                    p = info.unique_thread_name;
                    while (*p) {
                        *s++ = *p++;
                    }
                }

                if (info.more_display) {
                    my_memcpy (s, ", ", 2);
                    s += 2;
                    p = info.more_display;
                    while (*p) {
                        *s++ = *p++;
                    }
                }

            }
            *s++ = '\0';
            if (ret)
                ice_iobuf[0] = s - (char *)&ice_iobuf[1];
            else
                ice_iobuf[0] = 0;
        }
        break;
    default:
        // set return data size to 0
        ice_iobuf[0] = 0;
        break;
    }
    hal_v85x_ice_syscall_end();
}

__externC void
hal_v85x_init_ice_syscall_info(void)
{
    int valid_string = 0x45434956; // "VICE"
    ICE_SYSCALL_INFO_STARTPC = (int)&hal_v85x_ice_syscall;
    ICE_SYSCALL_INFO_ENDPC = (int)&hal_v85x_ice_syscall_end;
    ICE_SYSCALL_INFO_STACK = (int)&ice_stack[0];
    ICE_SYSCALL_INFO_BUF = (int)&ice_iobuf[0];
    // Leave validation string to last
    ICE_SYSCALL_INFO_VALIDATOR = valid_string;
}

#endif // ifdef CYGDBG_HAL_V850_ICE

// EOF v850_stub.c
