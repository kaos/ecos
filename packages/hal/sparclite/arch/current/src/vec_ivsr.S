/*===========================================================================
//
//	vec_ivsr.S
//
//	SPARClite vectors: interrupt vector service routine
//
//===========================================================================
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
//###COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	 hmt
// Contributors: hmt
// Date:	 1999-02-20
// Purpose:	 SPARClite vector code
// Description:	 see vectors.S; this is the default vector service routine
//               for interrupts.
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

!----------------------------------------------------------------------------

	.file	"vec_ivsr.S"

!----------------------------------------------------------------------------

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

!------------------------------------------------------------------------

#include <cyg/hal/vectors.h>

#define DELAYS_AFTER_WRPSR_SAME_WINDOW
#define DELAYS_AFTER_WRWIM

!------------------------------------------------------------------------

	.text

!---------------------------------------------------------------------------
! default interrupt VSR, which calls the appropriate ISR after scheduler
! lock andinterrupt masking, then interrupt_end().  interrupt_end() must be
! called with interrupts enabled, on the original thread stack.

	.global hal_default_interrupt_vsr
hal_default_interrupt_vsr:
	! here,locals have been set up as follows:
	! %l0 = psr (with this CWP/window-level in it)
	! %l1 = pc
	! %l2 = npc
	! %l3 = vector number (1-15 for interrupts)
	! and we are in our own register window, though it is likely that
	! the next one will need to be saved before we can use it:
	! ie. this one is the invalid register window.

	! must establish a safe stack before re-enabling interrupts + traps
	and	%l0, __WINBITS, %l7	! CWP extracted
	mov	1, %l6
	sll	%l6, %l7, %l6		! 1 << CWP
	rd	%wim, %l5
	cmp	%l5, %l6		! are they the same?
	bne	1f			! No, so the stack is OK as is.

	! now do by hand an overflow trap, effectively
	mov	%g1, %l7		! (DELAY SLOT)
	srl	%l5, 1, %l5
	sll	%l6, __WINSIZE-1, %l6
	or	%l6, %l5, %g1		! new WIM in %g1 so we can get it
					! within the save:
	save				! Slip into next window
	mov	%g1, %wim		! Install the new wim
					! (invalidates current window!)
#ifdef DELAYS_AFTER_WRWIM
	nop
	nop
	nop
#endif

	std	%l0, [%sp + 0 * 4]	! save L & I registers
	std	%l2, [%sp + 2 * 4]
	std	%l4, [%sp + 4 * 4]
	std	%l6, [%sp + 6 * 4]

	std	%i0, [%sp + 8 * 4]
	std	%i2, [%sp + 10 * 4]
	std	%i4, [%sp + 12 * 4]
	std	%i6, [%sp + 14 * 4]

	restore				! Go back to trap window.
	mov	%l7, %g1		! Restore %g1

1:	! now save away the regs we must preserve
	sub     %fp, 32 * 4, %sp
#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
	std	%g0, [%sp + 16 * 4]	! save G registers
	std	%g2, [%sp + 18 * 4]	! (set %g0 place to 0 to flag special context)
	std	%g4, [%sp + 20 * 4]
	std	%g6, [%sp + 22 * 4]
#else // not CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
	std	%l0, [%sp + 0 * 4]	! save L & I registers
	std	%l2, [%sp + 2 * 4]
	std	%l4, [%sp + 4 * 4]
	std	%l6, [%sp + 6 * 4]

	std	%i0, [%sp + 8 * 4]
	std	%i2, [%sp + 10 * 4]
	std	%i4, [%sp + 12 * 4]
	std	%i6, [%sp + 14 * 4]

	st	%g1, [%sp + 17 * 4]	! save G registers
	std	%g2, [%sp + 18 * 4]
	std	%g4, [%sp + 20 * 4]
	std	%g6, [%sp + 22 * 4]

	! no point whatsoever in saving O registers

	! and save the CWP in %g0 save place
	st	%l0, [%sp + 16 * 4]
#endif // ! CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT

	sub	%sp, 24 * 4, %sp	! fresh frame including
					! arg spill area for callees

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	! Lock the scheduler
	.extern	SCHED_LOCK_MANGLED_NAME
	sethi	%hi(SCHED_LOCK_MANGLED_NAME), %l7
	ld	[ %l7 + %lo(SCHED_LOCK_MANGLED_NAME) ], %l6
	add	%l6, 1, %l6
	st	%l6, [ %l7 + %lo(SCHED_LOCK_MANGLED_NAME) ]
#endif

	! HELP_GDB_WITH_BACKTRACE
	mov	%i7, %l5		! preserve it JIC
	mov	%l1, %i7		! bogus return link here

	! and we must preserve the Y register (multiply/divide auxiliary)
	! over these calls; we will keep it in %l4 which is otherwise unused.
	rd	%y, %l4

	! Now we can reenable traps and mask off only lower prio interrupts:
	andn	%l0, 0xf00, %l7		! clear PIL field
	or	%l7, 0x0e0, %l7		! and ET (+S,PS)
	sll	%l3, 8, %l6		! trap number (1-15) into PIL bitfield
	wr	%l7, %l6, %psr		! and enable!
#ifdef DELAYS_AFTER_WRPSR_SAME_WINDOW
	nop
	nop
	nop
#endif
	! now call the ISR and so on with the appropriate args:
	! ie.
	! isr_retcode = (*(hal_interrupt_handlers[ vector ]))
	!                          ( vector, hal_interrupt_data[ vector ] );

	! from hal_arch.h
	!// ISR tables
	!CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
	!CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
	!CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];        

	mov	%l3, %o0
	sll	%l3, 2, %l3		! %l3 to a word offset
	sethi	%hi(hal_interrupt_data), %l7
	or	%l7, %lo(hal_interrupt_data), %l7
	ld	[ %l7 + %l3 ], %o1

	sethi	%hi(hal_interrupt_handlers), %l7
	or	%l7, %lo(hal_interrupt_handlers), %l7
	ld	[ %l7 + %l3 ], %l6
	call	%l6
	nop

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	! We only need to call _interrupt_end() when there is a kernel
	! present to do any tidying up.

	! First restore the processor interrupt level to that interrupted
	! (otherwise a task-switch runs at the current PIL) on the assumption
	! that the ISR dealt with the interrupt source per se, so it is safe
	! to unmask it, effectively:
	or	%l0, 0x0e0, %l7		! original PSR and ET (+S,PS)
	wr	%l7, %psr		! and enable!
#ifdef DELAYS_AFTER_WRPSR_SAME_WINDOW
	nop
	nop
	nop
#endif
	! then call interrupt_end( isr_retcode, &intr_object, &regsave )
	! to unlock the scheduler and do any rescheduling that~s needed.
	! argument 0 (isr_retcode) is already in place in %o0
	sethi	%hi(hal_interrupt_objects), %l7
	or	%l7, %lo(hal_interrupt_objects), %l7
	ld	[ %l7 + %l3 ], %o1
	add	%sp, 24 * 4, %o2	! saved regset (maybe tiny)

	.extern interrupt_end
	call	interrupt_end
	nop
#endif

	! restore the Y register having done our callouts to C
	wr	%l4, %y

	! We can reinstall the original CWP here; even if interrupt_end()
	! performed a reschedule (ie. yield/resume pair) we will be in the
	! same window.  The window is preserved by reschedule precisely
	! because it is impossible atomically to disable traps here without
	! involving a CWP living in a register for a time when other
	! interrupts may occur.

	! disable traps (using the saved psr is fastest way)
	wr	%l0, %psr	! restores flags, disables traps, and old PIL
#ifdef DELAYS_AFTER_WRPSR_SAME_WINDOW
	nop
	nop
	nop
#endif

	! HELP_GDB_WITH_BACKTRACE
	mov	%l5, %i7	! restore (unused) return link

	! and restore other saved regs
	! (see CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT)
	add	%sp, 24 * 4, %sp	! undo fresh frame

	ld	[%sp + 17 * 4], %g1	! restore G registers
	ldd	[%sp + 18 * 4], %g2
	ldd	[%sp + 20 * 4], %g4
	ldd	[%sp + 22 * 4], %g6

	! and do NOT restore any other registers L, I or O

	! Now test for window underflow here and fix up if needs be.
	!
	! Why?  interrupt_end() might have yielded us, when only
	! its own frame was restored; its own return to us caused a
	! window underflow trap, as would our return to interruptee
	! unless we deal with it now.

	add	%l0, 1, %l7		! interruptee~s CWP plus noise
	and	%l7, __WINBITS, %l7	! CWP only
	mov	1, %l6
	sll	%l6, %l7, %l6		! 1 << CWP
	rd	%wim, %l5
	cmp	%l5, %l6		! are they the same?
	bne	2f			! No, so the stack is OK as is.

	! now do by hand an underflow trap, effectively
	sll	%l5, 1, %l5		! Rotate wim left
	srl	%l6, __WINSIZE-1, %l6
	wr	%l5, %l6, %wim
#ifdef DELAYS_AFTER_WRWIM
	nop				! are these delays needed?
	nop				! (following restore uses wim)
	nop
#endif
	restore				! Interruptee~s window
	ldd	[%sp + 0 * 4], %l0	! restore L & I registers
	ldd	[%sp + 2 * 4], %l2
	ldd	[%sp + 4 * 4], %l4
	ldd	[%sp + 6 * 4], %l6

	ldd	[%sp + 8 * 4], %i0
	ldd	[%sp + 10 * 4], %i2
	ldd	[%sp + 12 * 4], %i4
	ldd	[%sp + 14 * 4], %i6
	save				! Back to trap window

2:	! restore the condition codes, PSR and PIL and return from trap.
	wr	%l0, %psr	! restores flags, disables traps, and old PIL
#ifdef DELAYS_AFTER_WRPSR_SAME_WINDOW
	nop
	nop
	nop
#endif
	jmpl	%l1,  %g0
	rett	%l2


! end of vec_ivsr.S
