/*==========================================================================
//
//      hal_misc.c
//
//      HAL miscellaneous functions
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg, gthomas
// Contributors: nickg, gthomas
// Date:         1999-02-20
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_arm.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header

externC void diag_printf(const char *fmt, ...);

/*------------------------------------------------------------------------*/
/* First level C exception handler.                                       */

externC void __handle_exception (void);

externC HAL_SavedRegisters *_hal_registers;
extern void *__mem_fault_handler;

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
/* Force exception handling into the GDB stubs.  This is done by taking over
   the exception vectors while executing in the stubs.  This allows for the
   debugged program to handle exceptions itself, except while the GDB
   processing is underway.  The only vector that can't be handled this way
   is the illegal instruction vector which is used for breakpoint/single-step
   and must be maintained by the stubs at all times.
*/

#define ARM_VECTORS 8
extern unsigned long vectors[];  // exception vectors as defined by the stubs
static unsigned long *hardware_vectors = (unsigned long *)0x20;
static unsigned long hold_vectors[ARM_VECTORS];

static int exception_level;

static void
__take_over_debug_traps(void)
{
    int i;
    for (i = 0;  i < ARM_VECTORS;  i++) {
        hold_vectors[i] = hardware_vectors[i];
        hardware_vectors[i] = vectors[i];
    }
}

static void
__restore_debug_traps(void)
{
    int i;
    for (i = 0;  i < ARM_VECTORS;  i++) {
        hardware_vectors[i] = hold_vectors[i];
    }
}
#endif

void
exception_handler(HAL_SavedRegisters *regs)
{
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    if (__mem_fault_handler && 
        regs->vector == CYGNUM_HAL_EXCEPTION_DATA_ACCESS) {
        regs->pc = (unsigned long)__mem_fault_handler;
        return; // Caught an exception inside stubs        
    }

    if (++exception_level == 1) __take_over_debug_traps();

    _hal_registers = regs;
    __handle_exception();

    // We should decode the vector and pass a more appropriate
    // value as the second argument. For now we simply pass a
    // pointer to the saved registers. We should also divert
    // breakpoint and other debug vectors into the debug stubs.

    if (--exception_level == 0) __restore_debug_traps();

#elif defined(CYGPKG_KERNEL_EXCEPTIONS)

    cyg_hal_deliver_exception( regs->vector, (CYG_ADDRWORD)regs );

#else

    CYG_FAIL("Exception!!!");
    
#endif    
    
    return;
}

/*------------------------------------------------------------------------*/
/* C++ support - run initial constructors                                 */

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

typedef void (*pfunc) (void);
extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];

void
cyg_hal_invoke_constructors (void)
{

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    static pfunc *p = &__CTOR_END__[-1];
    
    cyg_hal_stop_constructors = 0;
    for (; p >= __CTOR_LIST__; p--) {
        (*p) ();
        if (cyg_hal_stop_constructors) {
            p--;
            break;
        }
    }
#else
    pfunc *p;

    for (p = &__CTOR_END__[-1]; p >= __CTOR_LIST__; p--)
        (*p) ();
#endif
}

/*------------------------------------------------------------------------*/
/* default ISR                                                            */

externC cyg_uint32
hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_TRACE1(true, "Interrupt: %d", vector);
    diag_printf("Spurious Interrupt!!! - vector: %d, data: %x\n", vector, 
                data);
    CYG_FAIL("Spurious Interrupt!!!");
    return 0;
}

/*------------------------------------------------------------------------*/
/* Idle thread action                                                     */

void
hal_idle_thread_action( cyg_uint32 count )
{
}

/*-------------------------------------------------------------------------*/
/* Misc functions                                                          */

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
/* This function will generate a breakpoint exception.  It is used at the
   beginning of a program to sync up with a debugger and can be used
   otherwise as a quick means to stop program execution and "break" into
   the debugger. */

void
breakpoint(void)
{
    HAL_BREAKPOINT(breakinst);
}


/* This function returns the opcode for a 'trap' instruction.  */

unsigned long
__break_opcode (void)
{
    return HAL_BREAKINST;
}
#endif

int
hal_lsbindex(int mask)
{
    int i;
    for (i = 0;  i < 32;  i++) {
      if (mask & (1<<i)) return (i);
    }
    return (-1);
}

int
hal_msbindex(int mask)
{
    int i;
    for (i = 32;  i >= 0;  i++) {
      if (mask & (1<<i)) return (i);
    }
    return (-1);
}

void
dump_buf_with_offset(unsigned char *p, 
                     int s, 
                     unsigned char *base)
{
    int i, c;
    if ((unsigned int)s > (unsigned int)p) {
        s = (unsigned int)s - (unsigned int)p;
    }
    while (s > 0) {
        if (base) {
            diag_printf("%08X: ", (int)p - (int)base);
        } else {
            diag_printf("%08X: ", p);
        }
        for (i = 0;  i < 16;  i++) {
            if (i < s) {
                diag_printf("%02X", p[i] & 0xFF);
            } else {
                diag_printf("  ");
            }
            if ((i % 2) == 1) diag_printf(" ");
            if ((i % 8) == 7) diag_printf(" ");
        }
        diag_printf(" |");
        for (i = 0;  i < 16;  i++) {
            if (i < s) {
                c = p[i] & 0xFF;
                if ((c < 0x20) || (c >= 0x7F)) c = '.';
            } else {
                c = ' ';
            }
            diag_printf("%c", c);
        }
        diag_printf("|\n");
        s -= 16;
        p += 16;
    }
}

void
dump_buf(unsigned char *p, int s)
{
   dump_buf_with_offset(p, s, 0);
}

#ifdef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
void
dump_frame(unsigned char *frame)
{
    HAL_SavedRegisters *rp = (HAL_SavedRegisters *)frame;
    int i;
    dump_buf(frame, 128);
    diag_printf("Registers:\n");
    for (i = 0;  i <= 10;  i++) {
        if ((i == 0) || (i == 6)) diag_printf("R%d: ", i);
        diag_printf("%08X ", rp->d[i]);
        if ((i == 5) || (i == 10)) diag_printf("\n");
    }
    diag_printf("FP: %08X, SP: %08X, LR: %08X, PC: %08X, PSR: %08X\n",
                rp->fp, rp->sp, rp->lr, rp->pc, rp->cpsr);
}
#endif

#if 0
void
show_frame_in(HAL_SavedRegisters *frame)
{
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("[IN] IRQ Frame:\n");
    dump_frame((unsigned char *)frame);
    HAL_RESTORE_INTERRUPTS(old);
}

void
show_frame_out(HAL_SavedRegisters *frame)
{
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("[OUT] IRQ Frame:\n");
    dump_frame((unsigned char *)frame);
    HAL_RESTORE_INTERRUPTS(old);
}
#endif

#ifdef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
// Debug routines
void undefined_instruction(HAL_SavedRegisters *frame)
{
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("[UNDEFINED INSTRUCTION] Frame:\n");
    dump_frame((unsigned char *)frame);
    HAL_RESTORE_INTERRUPTS(old);
}

void software_interrupt(HAL_SavedRegisters *frame)
{
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("[SOFTWARE INTERRUPT] Frame:\n");
    dump_frame((unsigned char *)frame);
    HAL_RESTORE_INTERRUPTS(old);
}

void abort_prefetch(HAL_SavedRegisters *frame)
{
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("[ABORT PREFETCH] Frame:\n");
    dump_frame((unsigned char *)frame);    
    HAL_RESTORE_INTERRUPTS(old);
}

void abort_data(HAL_SavedRegisters *frame)
{
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("[ABORT DATA] Frame:\n");
    dump_frame((unsigned char *)frame);
    HAL_RESTORE_INTERRUPTS(old);
}

void FIQ(HAL_SavedRegisters *frame)
{
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("[FIQ] Frame:\n");
    dump_frame((unsigned char *)frame);
    HAL_RESTORE_INTERRUPTS(old);
}

void exception_handler_returned(HAL_SavedRegisters *frame)
{    
    int old;
    HAL_DISABLE_INTERRUPTS(old);
    diag_printf("Exception handler returned!\n");
    dump_frame((unsigned char *)frame);
    HAL_RESTORE_INTERRUPTS(old);
}
#endif

/*------------------------------------------------------------------------*/
// EOF hal_misc.c
