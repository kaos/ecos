/* i386_stub.c - helper functions for stub, generic to all i386 processors
 * 
 * Copyright (c) 1998,1999 Cygnus Solutions
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

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#ifdef CYGPKG_HAL_I386_SIM
#error "GDB Stub support not implemented for i386 SIM"
#endif

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

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
	extern int hal_pc_trap_number ;
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return hal_pc_trap_number ;
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
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
	extern cyg_uint32 hal_pc_break_pc ;
	if (hal_pc_break_pc)
	{
		cyg_hal_gdb_remove_break(hal_pc_break_pc) ;
#if 0                
		hal_pc_break_pc = 0 ;
//		asm("movl %0, %%dr0" : /* No outputs */ : "m" (hal_pc_break_pc)) ;
		asm("
			movl %%dr7, %%eax
			andl $0xFFFFFFFC, %%eax
			movl %%eax, %%dr7"
		:	/* No outputs. */
		:	/* No inputs. */
		:	"eax");
#endif                
	}
#endif

}

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */

int isBreakpointFunction ;

int
__is_breakpoint_function ()
{
    isBreakpointFunction = (get_register (PC) == (target_register_t)&CYG_LABEL_NAME(breakinst));
    return isBreakpointFunction ;
}


/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
   we can safely just skip 4. */

void __skipinst (void)
{
//	FIXME() ;
}


#if 0
void hal_get_gdb_registers(target_register_t * d, HAL_SavedRegisters * s)
{
	d[ESP] = s->esp ;
	d[ESP] = s->esp ;
	d[EBP] = s->ebp ;
	d[EBX] = s->ebx ;
	d[ESI] = s->esi ;
	d[EDI] = s->edi ;
	d[PC] = s->eip ;
}


void hal_set_gdb_registers(HAL_SavedRegisters * d, target_register_t * s)
{
	d->esp = s[ESP] ;
	d->next_context = 0 ;
	d->ebp = s[EBP] ;
	d->ebx = s[EBX] ;
	d->esi = s[ESI] ;
	d->edi = s[EDI] ;
	d->eip = s[PC] ;
}
#endif


#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
