/* i386_stub.c - helper functions for stub, generic to all i386 processors
 * 
 * Copyright (c) 1998,1999, 2001 Cygnus Solutions
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

/*
- pjo, 29 sep 1999
- Copied from the ARM configuration and merged with an older GDB i386-stub.c.
*/

#include <stddef.h>

#include <pkgconf/hal.h>

#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#endif

#ifdef CYGPKG_HAL_I386_SIM
#error "GDB Stub support not implemented for i386 SIM"
#endif

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/i386_stub.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

/* Given a trap value TRAP, return the corresponding signal. */

int __computeSignal (unsigned int trap_number)
{
    switch (trap_number)
	{
		case CYGNUM_HAL_VECTOR_DIV0:
			/* This isn't quite accurrate: this is integer division only. */
			return SIGFPE ;

		case CYGNUM_HAL_VECTOR_DEBUG:
			return SIGTRAP ;

		case CYGNUM_HAL_VECTOR_NMI:
			return SIGINT ;

		case CYGNUM_HAL_VECTOR_BREAKPOINT:
			return SIGTRAP ;

		case CYGNUM_HAL_VECTOR_OVERFLOW:
		case CYGNUM_HAL_VECTOR_BOUND:
			return SIGSEGV ;

		case CYGNUM_HAL_VECTOR_OPCODE:
			return SIGILL ;

		case CYGNUM_HAL_VECTOR_NO_DEVICE:
		case CYGNUM_HAL_VECTOR_FPE:
			return SIGFPE ;

		case CYGNUM_HAL_VECTOR_DOUBLE_FAULT:
			return SIGTRAP ;

		case CYGNUM_HAL_VECTOR_INVALID_TSS:
		case CYGNUM_HAL_VECTOR_SEGV:
		case CYGNUM_HAL_VECTOR_STACK_FAULT:
		case CYGNUM_HAL_VECTOR_PROTECTION:
		case CYGNUM_HAL_VECTOR_PAGE:
		case CYGNUM_HAL_VECTOR_ALIGNMENT:
			return SIGSEGV ;

		default:
			return SIGTRAP;
    }
}


/* Return the trap number corresponding to the last-taken trap. */

int __get_trap_number (void)
{
#if 1
    // The vector is not not part of the GDB register set so get it
    // directly from the HAL saved context.
    return _hal_registers->vector;
#else
	extern int hal_pc_trap_number ;
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return hal_pc_trap_number ;
#endif        
}

#if defined(CYGSEM_REDBOOT_BSP_SYSCALLS)
int __is_bsp_syscall(void) 
{
    return __get_trap_number() == 0x80;
}
#endif

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


/* We just turn on the trap bit in the flags register for the particular
	thread.  When it resumes, we'll get another debugger trap.
*/
void __single_step (void)
{	put_register(PS, get_register(PS) | PS_T) ;
}

/* Clear the single-step state. */

void __clear_single_step (void)
{	put_register(PS, get_register(PS) & ~PS_T) ;
}

void __install_breakpoints (void)
{
//    FIXME();
}

void __clear_breakpoints (void)
{
    __clear_breakpoint_list();
}

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */

int isBreakpointFunction ;

int
__is_breakpoint_function ()
{
    isBreakpointFunction = (get_register (PC) == (target_register_t)&_breakinst);
    return isBreakpointFunction ;
}


/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
   we can safely just skip 4. */

void __skipinst (void)
{
//	FIXME() ;
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//----------------------------------------------------------------------
// We apparently need these function even when no stubinclude
// for Thread Debug purposes

void hal_get_gdb_registers(CYG_ADDRWORD * d, HAL_SavedRegisters * s)
{
	d[ESP] = s->esp ;
	d[EBP] = s->ebp ;
	d[ESI] = s->esi ;
	d[EDI] = s->edi ;
	d[EAX] = s->eax ;
	d[EBX] = s->ebx ;
	d[ECX] = s->ecx ;
	d[EDX] = s->edx ;
	d[PC]  = s->pc ;
	d[CS]  = s->cs ;
	d[PS]  = s->eflags ;
}

void hal_set_gdb_registers(HAL_SavedRegisters * d, CYG_ADDRWORD * s)
{
	d->esp    = s[ESP] ;
	d->ebp    = s[EBP] ;
	d->eax    = s[EAX] ;
	d->ebx    = s[EBX] ;
	d->ecx    = s[ECX] ;
	d->edx    = s[EDX] ;
	d->esi    = s[ESI] ;
	d->edi    = s[EDI] ;
	d->pc     = s[PC] ;
	d->cs     = s[CS] ;
	d->eflags = s[PS] ;
}


//----------------------------------------------------------------------
// End
