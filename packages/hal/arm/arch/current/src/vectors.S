// #========================================================================
// #
// #    vectors.S
// #
// #    ARM exception vectors
// #
// #========================================================================
// ####COPYRIGHTBEGIN####
// 
//  -------------------------------------------
//  The contents of this file are subject to the Cygnus eCos Public License
//  Version 1.0 (the "License"); you may not use this file except in
//  compliance with the License.  You may obtain a copy of the License at
//  http://sourceware.cygnus.com/ecos
//  
//  Software distributed under the License is distributed on an "AS IS"
//  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
//  License for the specific language governing rights and limitations under
//  the License.
//  
//  The Original Code is eCos - Embedded Cygnus Operating System, released
//  September 30, 1998.
//  
//  The Initial Developer of the Original Code is Cygnus.  Portions created
//  by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
//  -------------------------------------------
// 
// ####COPYRIGHTEND####
// #========================================================================
// ######DESCRIPTIONBEGIN####
// #
// # Author(s):     nickg, gthomas
// # Contributors:  nickg, gthomas
// # Date:          1999-02-20
// # Purpose:       ARM exception vectors
// # Description:   This file defines the code placed into the exception
// #                vectors. It also contains the first level default VSRs
// #		    that save and restore state for both exceptions and
// #		    interrupts.
// #
// #####DESCRIPTIONEND####
// #
// #========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_arm.h>
#ifdef CYGPKG_KERNEL  // no CDL yet
#include <pkgconf/kernel.h>
#else
# undef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
# undef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
#endif
#include <cyg/hal/hal_platform_setup.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
// The CDL should enforce this
#undef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
#endif

#include "arm.inc"

#define FUNC_START(name)	\
        .globl _name;		\
_name:	

#define PTR(name) 		\
.##name: .word	name

#ifdef CYGHWR_HAL_ARM_HAS_MMU
#define UNMAPPED(x) ((x)-__exception_handlers)
#else
#define UNMAPPED(x) (x)
#endif
                                
#define UNMAPPED_PTR(name) 		\
.##name: .word	UNMAPPED(name)


	.file	"vectors.S"

	
.macro LED x
.endm


//==========================================================================
// Hardware exception vectors.
//   This entire section will be copied to location 0x0000 at startup time.
//
	.code	32
	.section ".vectors","ax"
	.global	__exception_handlers
__exception_handlers:
	ldr	pc,.reset_vector		// 0x00
	ldr	pc,.undefined_instruction	// 0x04
	ldr	pc,.software_interrupt		// 0x08 start && software int
	ldr	pc,.abort_prefetch		// 0x0C
	ldr	pc,.abort_data			// 0x10
	.word	0				// unused
	ldr	pc,.IRQ				// 0x18
	ldr	pc,.FIQ				// 0x1C

// The layout of these pointers should match the vector table above since
// they are copied in pairs.
	.global	vectors
vectors:
UNMAPPED_PTR(reset_vector)			// 0x20
PTR(undefined_instruction)			// 0x24
PTR(software_interrupt)				// 0x28
PTR(abort_prefetch)				// 0x2C
PTR(abort_data)					// 0x30
	.word	0				// 0x34
PTR(IRQ)					// 0x38
PTR(FIQ)					// 0x3c
PTR(start) // This is copied to 0x28 for bootup // 0x40
	   // location 0x40 is used for storing DRAM size if known
	   // for some platforms.

	.text	
// Startup code which will get the machine into supervisor mode
	.global	reset_vector
	.type	reset_vector,function
reset_vector:

#if defined(CYG_HAL_STARTUP_RAM) && \
    !defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
	mrs	r7,cpsr			// move back to IRQ mode
	and	r7,r7,#CPSR_MODE_BITS
	cmp	r7,#CPSR_SUPERVISOR_MODE
	beq	start
#endif

	PLATFORM_SETUP1		// Early stage platform initialization
				// which can set DRAM size at 0x40
				// see <cyg/hal/hal_platform_setup.h>
	// We cannot access any LED registers until after PLATFORM_SETUP1
	LED 7

	mov	r0,#0		// move vectors
	// We cannot perform a store until after PLATFORM_SETUP1
	str	r0, [r0,#0x40]  // DRAM size to zero => unknown
	ldr	r1,.__exception_handlers
	ldr	r2,[r1,#0x04]	// undefined instruction
	str	r2,[r0,#0x04]
	ldr	r2,[r1,#0x24]	
	str	r2,[r0,#0x24]
	ldr	r2,[r1,#0x08]	// software interrupt
	str	r2,[r0,#0x08]
#if defined(CYG_HAL_STARTUP_RAM)
// Ugly hack to get into supervisor mode        
	ldr	r2,[r1,#0x40]
	str	r2,[r0,#0x28]

	LED 6
		
	swi			// switch to supervisor mode
#endif        

// =========================================================================
// Real startup code. We jump here from the reset vector to set up the world.
	.globl	start
start:	

	LED 5

#if defined(CYG_HAL_STARTUP_RAM) && \
    !defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
// If we get restarted, hang here to avoid corrupting memory
	ldr	r0,.init_flag
	ldr	r1,[r0]
1:	cmp	r1,#0
	bne	1b
	ldr	r1,init_done
	str	r1,[r0]
#endif

	// Reset software interrupt pointer
	mov	r0,#0		// move vectors
	ldr	r1,.__exception_handlers
#if defined(CYG_HAL_STARTUP_RAM) && \
    !defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
	cmp	r7,#CPSR_SUPERVISOR_MODE
	beq	10f
#endif
	ldr	r2,[r1,#0x28]	// software interrupt
	str	r2,[r0,#0x28]
10:
	ldr	r2,[r1,#0x18]	// IRQ
	str	r2,[r0,#0x18]
	ldr	r2,[r1,#0x38]
	str	r2,[r0,#0x38]
	ldr	r2,[r1,#0x1C]	// FIQ
	str	r2,[r0,#0x1C]
	ldr	r2,[r1,#0x3C]
	str	r2,[r0,#0x3C]
	ldr	r2,[r1,#0x0C]	// abort (prefetch)
	str	r2,[r0,#0x0C]
	ldr	r2,[r1,#0x2C]	
	str	r2,[r0,#0x2C]
	ldr	r2,[r1,#0x10]	// abort (data)
	str	r2,[r0,#0x10]
	ldr	r2,[r1,#0x30]
	str	r2,[r0,#0x30]

	LED 4
	
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_STUBS)
	// Relocate [copy] data from ROM to RAM
	ldr	r3,.__rom_data_start
	ldr	r4,.__ram_data_start
	ldr	r5,.__ram_data_end
	cmp	r4,r5		// jump if no data to move
	beq	2f
	sub	r3,r3,#4	// loop adjustments
	sub	r4,r4,#4
1:	ldr	r0,[r3,#4]!	// copy info
	str	r0,[r4,#4]!
	cmp	r4,r5
	bne	1b
2:
#endif

	// initialize interrupt/exception environments
	ldr	sp,.__startup_stack
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_IRQ_MODE)
	msr	cpsr,r0
	ldr	sp,.__exception_stack
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_FIQ_MODE)
	msr	cpsr,r0
	ldr	sp,.__FIQ_exception_stack
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_UNDEF_MODE)
	msr	cpsr,r0
	ldr	sp,.__exception_stack

	// initialize CPSR (machine state register)
	mov	r0,#(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_SUPERVISOR_MODE)
	msr	cpsr,r0

	// Note: some functions in LIBGCC1 will cause a "restore from SPSR"!!
	msr	spsr,r0

	// initialize stack
	ldr	sp,.__startup_stack

	// clear BSS
	ldr	r1,.__bss_start
	ldr	r2,.__bss_end
	mov	r0,#0
	cmp	r1,r2
	beq	2f
1:	str	r0,[r1],#4
	cmp	r1,r2
	bne	1b
2:


	LED 3
	
	// Call platform specific hardware initialization
	bl	hal_hardware_init

	LED 2
	
	// Run through static constructors
	bl	cyg_hal_invoke_constructors

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        bl      initialize_stub
#endif

	LED 1
	
	// This starts up the eCos kernel
	bl	cyg_start
_start_hang:
	b	_start_hang
	.code	32

init_done:
	.long	0xDEADB00B

//
// Exception handlers
// Assmumption: get here from a Supervisor context [mode]
//
	.code	32
undefined_instruction:
	ldr	sp,.__undef_exception_stack	// get good stack
	sub	lr,lr,#4		// PC at time of interrupt
	stmfd	sp!,{r0,fp,ip,lr}		
	mrs	r0,spsr
	stmfd	sp!,{r0}
	mov	ip,sp			// save SP which will vanish with
                                        //   mode switch
	mrs	r0,cpsr			// switch to Supervisor Mode
	bic	r0,r0,#CPSR_MODE_BITS
	orr	r0,r0,#CPSR_SUPERVISOR_MODE
	msr	cpsr,r0			// sp,lr are now old values
	mov	fp,sp
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
	ldr	sp,.__GDB_stack
	cmp	fp,sp			// already on GDB stack?
	bhi	10f	
	ldr	r0,.__GDB_stack_base		
	cmp	fp,r0
	bls	10f			// no - switch to GDB stack
                                        //      (already in sp)
	mov	sp,fp			// yes - no switch
10:
#endif
	sub	sp,sp,#ARMREG_SIZE	// make space for frame
	stmea	sp,{r0-r10,fp}		// save immediately visible registers
	ldmfd	ip,{r0-r4}		// saved registers
	str	r0,[sp,#armreg_cpsr]	// CPSR at time of interrupt
	str	r1,[sp,#armreg_r0]	// saved R0
	str	r2,[sp,#armreg_fp]	// saved FP
	str	r3,[sp,#armreg_ip]	// saved IP
	str	r4,[sp,#armreg_pc]	// PC at time of interrupt
	str	lr,[sp,#armreg_lr]	// LR at time of interrupt
	add	r0,ip,#ARMREG_SIZE
	str	fp,[sp,#armreg_sp]	// SP at time of interrupt
#ifdef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
	mov	r0,sp
	bl	cyg_hal_report_undefined_instruction
#endif
	mov	v1,#CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION
	b	call_exception_handler

	.code	32
software_interrupt:
	ldr	sp,.__startup_stack	// get good stack
	sub	lr,lr,#4		// PC at time of interrupt
	stmfd	sp!,{r0,ip,lr}		
	mrs	r0,spsr
	stmfd	sp!,{r0}
	mov	ip,sp			// save SP which will vanish with
                                        //   mode switch
	mrs	r0,cpsr			// switch to Supervisor Mode
	bic	r0,r0,#CPSR_MODE_BITS
	orr	r0,r0,#CPSR_SUPERVISOR_MODE
	msr	cpsr,r0			// sp,lr are now old values
	mov	ip,sp
	ldr	sp,.__startup_stack	// get good stack
	sub	sp,sp,#ARMREG_SIZE+16	// make space for frame
	stmea	sp,{r0-r10,fp}		// save immediately visible registers
	ldmfd	ip,{r0-r3}		// saved registers
	str	r0,[sp,#armreg_cpsr]	// CPSR at time of interrupt
	str	r1,[sp,#armreg_r0]	// saved R0
	str	r2,[sp,#armreg_ip]	// saved IP
	str	r3,[sp,#armreg_pc]	// PC at time of interrupt
	str	lr,[sp,#armreg_lr]	// LR at time of interrupt
	add	r0,ip,#ARMREG_SIZE
	str	r0,[sp,#armreg_sp]	// SP at time of interrupt
#ifdef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
	mov	r0,sp
	bl	cyg_hal_report_software_interrupt
#endif
	mov	v1,#CYGNUM_HAL_EXCEPTION_INTERRUPT
	b	call_exception_handler

	.code	32
abort_prefetch:
	ldr	sp,.__undef_exception_stack	// get good stack
	sub	lr,lr,#4		// PC at time of interrupt
	stmfd	sp!,{r0,fp,ip,lr}		
	mrs	r0,spsr
	stmfd	sp!,{r0}
	mov	ip,sp			// save SP which will vanish with
                                        //   mode switch
	mrs	r0,cpsr			// switch to Supervisor Mode
	bic	r0,r0,#CPSR_MODE_BITS
	orr	r0,r0,#CPSR_SUPERVISOR_MODE
	msr	cpsr,r0			// sp,lr are now old values
	mov	fp,sp
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
	ldr	sp,.__GDB_stack
	cmp	fp,sp			// already on GDB stack?
	bhi	10f	
	ldr	r0,.__GDB_stack_base		
	cmp	fp,r0
	bls	10f			// no - switch to GDB stack
                                        //      (already in sp)
	mov	sp,fp			// yes - no switch
10:
#endif
	sub	sp,sp,#ARMREG_SIZE	// make space for frame
	stmea	sp,{r0-r10,fp}		// save immediately visible registers
	ldmfd	ip,{r0-r4}		// saved registers
	str	r0,[sp,#armreg_cpsr]	// CPSR at time of interrupt
	str	r1,[sp,#armreg_r0]	// saved R0
	str	r2,[sp,#armreg_fp]	// saved FP
	str	r3,[sp,#armreg_ip]	// saved IP
	str	r4,[sp,#armreg_pc]	// PC at time of interrupt
	str	lr,[sp,#armreg_lr]	// LR at time of interrupt
	add	r0,ip,#ARMREG_SIZE
	str	fp,[sp,#armreg_sp]	// SP at time of interrupt
#ifdef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
	mov	r0,sp
	bl	cyg_hal_report_abort_prefetch
#endif
	mov	v1,#CYGNUM_HAL_EXCEPTION_CODE_ACCESS
	b	call_exception_handler

	.code	32
abort_data:
	ldr	sp,.__undef_exception_stack	// get good stack
	sub	lr,lr,#4		// PC at time of interrupt
	stmfd	sp!,{r0,fp,ip,lr}		
	mrs	r0,spsr
	stmfd	sp!,{r0}
	mov	ip,sp			// save SP which will vanish with
                                        //   mode switch
	mrs	r0,cpsr			// switch to Supervisor Mode
	bic	r0,r0,#CPSR_MODE_BITS
	orr	r0,r0,#CPSR_SUPERVISOR_MODE
	msr	cpsr,r0			// sp,lr are now old values
	mov	fp,sp
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
	ldr	sp,.__GDB_stack
	cmp	fp,sp			// already on GDB stack?
	bhi	10f	
	ldr	r0,.__GDB_stack_base		
	cmp	fp,r0
	bls	10f			// no - switch to GDB stack
                                        //      (already in sp)
	mov	sp,fp			// yes - no switch
10:
#endif
	sub	sp,sp,#ARMREG_SIZE	// make space for frame
	stmea	sp,{r0-r10,fp}		// save immediately visible registers
	ldmfd	ip,{r0-r4}		// saved registers
	str	r0,[sp,#armreg_cpsr]	// CPSR at time of interrupt
	str	r1,[sp,#armreg_r0]	// saved R0
	str	r2,[sp,#armreg_fp]	// saved FP
	str	r3,[sp,#armreg_ip]	// saved IP
	str	r4,[sp,#armreg_pc]	// PC at time of interrupt
	str	lr,[sp,#armreg_lr]	// LR at time of interrupt
	add	r0,ip,#ARMREG_SIZE
	str	fp,[sp,#armreg_sp]	// SP at time of interrupt
#ifdef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
	mov	r0,sp
	bl	cyg_hal_report_abort_data
#endif
	mov	v1,#CYGNUM_HAL_EXCEPTION_DATA_ACCESS
	b	call_exception_handler

        
//
// Dispatch an exception handler.

	.code	32
call_exception_handler:
	str	v1,[sp,#armreg_vector]
	mov	r0,sp
	bl	exception_handler
#ifdef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS
	mov	r0,sp
	bl	cyg_hal_report_exception_handler_returned
#endif

// Restore [interrupted] context
	mov	ip,sp			// get stack pointer

	ldr	lr,[ip,#armreg_lr]
	ldr	sp,[sp,#armreg_sp]

	mrs	r0,cpsr			// move back to IRQ mode
	bic	r0,r0,#CPSR_MODE_BITS
	orr	r0,r0,#CPSR_IRQ_MODE
	msr	cpsr,r0

	mov	sp,ip
	ldr	lr,[sp,#armreg_pc]
	ldr	r0,[sp,#armreg_cpsr]
	ldr	ip,[sp,#armreg_ip]
	msr	spsr,r0
	ldmfd	sp,{r0-r10,fp}

	movs	pc,lr			// restore PC from LR, CPSR from SPSR

// Handle device interrupts
// This is slightly more complicated than the other exception handlers because
// it needs to interface with the kernel (if present).

	.code	32
FIQ:
	// Note: I use this exception stack while saving the context because
	// the current SP does not seem to be always valid in this CPU mode.
	ldr	sp,.__FIQ_exception_stack	// get good stack
	sub	lr,lr,#4		// PC at time of interrupt
	stmfd	sp!,{r0,fp,ip,lr}		
	mrs	r0,spsr
	stmfd	sp!,{r0}
        b       handle_IRQ_or_FIQ

IRQ:
	// Note: I use this exception stack while saving the context because
	// the current SP does not seem to be always valid in this CPU mode.
	ldr	sp,.__exception_stack	// get good stack
	sub	lr,lr,#4		// PC at time of interrupt
	stmfd	sp!,{r0,fp,ip,lr}		
	mrs	r0,spsr
	stmfd	sp!,{r0}

handle_IRQ_or_FIQ:              
	mov	ip,sp			// save SP which will vanish with
                                        //   mode switch
	mrs	r0,cpsr			// switch to Supervisor Mode
	bic	r0,r0,#CPSR_MODE_BITS
	orr	r0,r0,#CPSR_SUPERVISOR_MODE|CPSR_FIQ_DISABLE|CPSR_IRQ_DISABLE
	msr	cpsr,r0			// sp,lr are now old values
	mov	fp,sp			// save old SP
	sub	sp,sp,#ARMREG_SIZE	// make space for frame
	stmea	sp,{r0-r10,fp}		// save immediately visible registers
	ldmfd	ip,{r0-r4}		// saved registers
	str	r0,[sp,#armreg_cpsr]	// CPSR at time of interrupt
	str	r1,[sp,#armreg_r0]	// saved R0
	str	r2,[sp,#armreg_fp]	// saved FP
	str	r3,[sp,#armreg_ip]	// saved IP
	str	r4,[sp,#armreg_pc]	// PC at time of interrupt
	str	lr,[sp,#armreg_lr]	// LR at time of interrupt
	str	fp,[sp,#armreg_sp]	// SP at time of interrupt
	mov	v6,sp			// Save pointer to register frame

//	mov	r0,sp
//	bl	_show_frame_in

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	// Switch to interrupt stack
	ldr	r2,.irq_level		// current number of nested interrupts
	ldr	r0,[r2]
	add	r1,r0,#1
	str	r1,[r2]			// if was zero, switch stacks
	cmp	r0,#0
	moveq	r1,sp			// save old stack pointer
	ldreq	sp,.__interrupt_stack
	stmeqfd	sp!,{r1}
10:
#endif

	// The entire CPU state is now stashed on the stack,
	// increment the scheduler lock and handle the interrupt

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT			
	.extern	cyg_scheduler_sched_lock
	ldr	r3,.cyg_scheduler_sched_lock
	ldr	r4,[r3]
	add	r4,r4,#1
	str	r4,[r3]
#endif

// TEMP
	mov	r0,v6
// TEMP
	bl	hal_IRQ_handler		// determine interrupt source
	mov	v1,r0			// returned vector #

#if defined(CYGPKG_KERNEL_INSTRUMENT) && \
    defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
	ldr	r0,=RAISE_INTR		// arg0 = type = INTR,RAISE
	mov	r1,v1			// arg1 = vector
	mov	r2,#0			// arg2 = 0
	bl	cyg_instrument		// call instrument function
#endif

	mov	r0,v1			// vector #
	ldr	r1,.hal_interrupt_data
	ldr	r1,[r1,v1,lsl #2]	// handler data
	ldr	r2,.hal_interrupt_handlers
	ldr	v3,[r2,v1,lsl #2]	// handler (indexed by vector #)
	mov	r2,v6			// register frame

	mov	lr,pc			// invoke handler (call indirect
	mov	pc,v3                   // thru v3)

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	// If we are returning from the last nested interrupt, move back
	// to the thread stack. interrupt_end() must be called on the
	// thread stack since it potentially causes a context switch.
	ldr	r2,.irq_level
	ldr	r3,[r2]
	subs	r1,r3,#1
	str	r1,[r2]
	ldreq	sp,[sp]		// This should be the saved stack pointer
#endif
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	// The return value from the handler (in r0) will indicate whether a 
	// DSR is to be posted. Pass this together with a pointer to the
	// interrupt object we have just used to the interrupt tidy up routine.

	ldr	r1,.hal_interrupt_objects
	ldr	r1,[r1,v1,lsl #2]
	bl	interrupt_end   // post any bottom layer handler
                                // threads and call scheduler
#endif


//	mov	r0,sp
//	bl	show_frame_out

	mov	ip,sp			// get stack pointer

	ldr	lr,[ip,#armreg_lr]
	ldr	sp,[sp,#armreg_sp]

	mrs	r0,cpsr			// move back to IRQ mode
	bic	r0,r0,#CPSR_MODE_BITS
	orr	r0,r0,#CPSR_IRQ_MODE
	msr	cpsr,r0

	mov	sp,ip
	ldr	lr,[sp,#armreg_pc]
	ldr	r0,[sp,#armreg_cpsr]
	ldr	ip,[sp,#armreg_ip]
	msr	spsr,r0
	ldmfd	sp,{r0-r10,fp}
	movs	pc,lr			// restore PC from LR, CPSR from SPSR

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
// Execute pending DSRs the interrupt stack
// Note: this can only be called from code running on a thread stack
	.global	hal_interrupt_stack_call_pending_DSRs
hal_interrupt_stack_call_pending_DSRs:
        stmfd	sp!,{r4,r5,lr}
	// Disable interrupts
	mrs	r4,cpsr			// disable IRQ's
	orr	r2,r4,#CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE
	bic	r5,r4,#CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE
	msr	cpsr,r2
	// Switch to interrupt stack
	mov	r3,sp			// save old stack pointer
	ldr	sp,.__interrupt_stack
	stmfd	sp!,{r3}		// stored at top of interrupt stack
	ldr	r2,.irq_level		// current number of nested interrupts
	ldr	r3,[r2]
	add	r3,r3,#1		// bump nesting level
	str	r3,[r2]
	msr	cpsr,r5			// enable interrupts

	bl	cyg_interrupt_call_pending_DSRs

	// Disable interrupts
	mrs	r1,cpsr			// disable IRQ's
	orr	r2,r1,#CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE
	msr	cpsr,r2

	// Move back to the thread stack.
	ldr	r2,.irq_level
	ldr	r3,[r2]
	sub	r3,r3,#1		// decrement nesting level
	str	r3,[r2]
	ldr	sp,[sp]			// This should be the saved stack pointer
	msr	cpsr,r4			// restore interrupts to original state

	ldmfd	sp!,{r4,r5,pc}		// return
#endif // CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
        

// Dummy functions

	.global	__gccmain
__gccmain:
	mov	pc,lr	

	.global	_psr
_psr:
	mrs	r0,cpsr
	mov	pc,lr

	.global	_sp
_sp:
	mov	r0,sp
	mov	pc,lr


//
// Pointers to various objects.
//
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
PTR(__GDB_stack_base)
PTR(__GDB_stack)
#endif
PTR(__startup_stack)
PTR(__FIQ_exception_stack)
PTR(__exception_stack)
PTR(__undef_exception_stack)
PTR(__bss_start)
PTR(__bss_end)
PTR(_end)
PTR(__rom_data_start)
PTR(__ram_data_start)
PTR(__ram_data_end)
PTR(hal_interrupt_handlers)
PTR(hal_interrupt_data)
PTR(hal_interrupt_objects)
PTR(__exception_handlers)
PTR(init_flag)
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
PTR(cyg_scheduler_sched_lock)
#endif
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
PTR(irq_level)
PTR(__interrupt_stack)
#endif

//
// Identification - useful to find out when a system was configured
_eCos_id:
	.asciz	"eCos v1.2.1: " __DATE__


// -------------------------------------------------------------------------
// Interrupt vector tables.
// These tables contain the isr, data and object pointers used to deliver
// interrupts to user code.

// Despite appearances, their sizes are not #defines, but .equ symbols
// generated by magic without proper dependencies in arm.inc
// Recompiling will not DTRT without manual intervention.

	.data

init_flag:
	.long	0

	.extern hal_default_isr

	.globl	hal_interrupt_handlers
hal_interrupt_handlers:
	.rept	CYGNUM_HAL_ISR_COUNT
	.long	hal_default_isr
	.endr

	.globl	hal_interrupt_data
hal_interrupt_data:
	.rept	CYGNUM_HAL_ISR_COUNT
	.long	0
	.endr

	.globl	hal_interrupt_objects
hal_interrupt_objects:
	.rept	CYGNUM_HAL_ISR_COUNT
	.long	0
	.endr

// -------------------------------------------------------------------------
// Temporary interrupt stack
        
        .section ".bss"

// Small stacks, only used for saving information between CPU modes
__exception_stack_base: 
        .rept   32
        .long   0
        .endr
__FIQ_exception_stack:
        .rept   32
        .long   0
        .endr
__exception_stack:
        .rept   32
        .long   0
        .endr
__undef_exception_stack:

// Runtime stack used during all interrupt processing
#ifndef CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
#define CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE 4096
#endif
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	.balign 16
	.global cyg_interrupt_stack_base
cyg_interrupt_stack_base:
__interrupt_stack_base:
        .rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
        .byte 0
        .endr
	.balign 16
	.global cyg_interrupt_stack
cyg_interrupt_stack:
__interrupt_stack:
irq_level:
	.long	0
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
	.balign 16
__GDB_stack_base:
        .rept 0x400
        .byte 0
        .endr
__GDB_stack:
#endif
	.balign 16
__startup_stack_base:
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
        .rept 512
#else
        .rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
#endif
        .byte 0
        .endr
	.balign 16
__startup_stack:

// --------------------------------------------------------------------------
//  end of vectors.S
