##=============================================================================
##
##	vectors.S
##
##	MN10300 exception vectors
##
##=============================================================================
#####COPYRIGHTBEGIN####
#
# -------------------------------------------
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	nickg
## Contributors:	nickg
## Date:	1997-10-16
## Purpose:	MN10300 exception vectors
## Description:	This file defines the code placed into the exception
##              vectors. It also contains the first level default VSRs
##		that save and restore state for both exceptions and
##		interrupts.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL

#include <pkgconf/kernel.h>
	
#endif		
	
	.extern _cyg_instrument

	
##-----------------------------------------------------------------------------
## Register addresses and initialization values
	
	.equ	IOBCTR	,0x32000010
	.equ	MEMCTR0	,0x32000020
	.equ	MEMCTR1	,0x32000022
	.equ	MEMCTR2	,0x32000024
	.equ	MEMCTR3	,0x32000026
	.equ	MEMCTR4	,0x32000028
	.equ	MEMCTR5	,0x3200002a
	.equ	MEMCTR6	,0x3200002c
	.equ	MEMCTR7	,0x3200002e
	.equ	DRAMCTR	,0x32000040
	.equ	REFCNT	,0x32000042

#ifdef CYG_HAL_MN10300_STDEVAL1	
	.equ	INIT_MEMCTR0,0x1200	# 2wait
	.equ	INIT_MEMCTR1,0x0120	# 1wait-32bit-
	.equ	INIT_MEMCTR2,0x0065
	.equ	INIT_DRAMCTR,0x0287
	.equ	INIT_REFCNT,0x00eb	
#endif	

	.equ	P2OUT	,0x36008004
	.equ	P2MD	,0x36008024
	.equ	P2SS	,0x36008044
	.equ	P2DIR	,0x36008064

	.equ	INIT_P2MD,0xf0
	.equ	INIT_P2DIR,0xff

##-----------------------------------------------------------------------------
		
	.macro	led val
	movm	[d2],(sp)
	mov	\val,d2
	asl	4,d2
	movbu	d2,(P2OUT)
	movm	(sp),[d2]
	.endm

			
##-----------------------------------------------------------------------------
	
	.file	"vectors.S"

##-----------------------------------------------------------------------------
		
	.section ".vectors","ax"

	.globl	reset_vector
	
reset_vector:	
	# Reset vector
	jmp	_start

	.p2align 3

	# NMI vector
	jmp	__nmi

	.p2align 3
	
	# Trap vector
	jmp	__trap

##-----------------------------------------------------------------------------
## NMI vector

	.globl __nmi
__nmi:
	add	-4,sp				# space for SP
	movm	[d2,d3,a2,a3,other],(sp)	# push all registers
	mov	(_hal_vsr_table+28),a0
	jmp	(a0)

##-----------------------------------------------------------------------------
## Trap vector

	.globl __trap
__trap:
	add	-8,sp				# space for SP and PSW
	movm	[d2,d3,a2,a3,other],(sp)	# push all registers
	mov	(_hal_vsr_table+32),a0
	jmp	(a0)
		
##-----------------------------------------------------------------------------
## Macro to define a hardware VSR	

	.macro	hardware_vector name
	.globl	__hardware_vector_\name
__hardware_vector_\name:
	add	-4,sp				# space for SP
	movm	[d2,d3,a2,a3,other],(sp)	# push all registers
	mov	\name,d0			# d0 = vector id
	mov	d0,(0,sp)			# save in dummy stack loc
	mov	_hal_vsr_table,a0		# a0 = vsr table
	add	\name<<2,a0			# a0 = &required vsr
	mov	(a0),a0				# a0 = required vsr
	jmp	(a0)				# go there
	.endm	

##-----------------------------------------------------------------------------
## Hardware Vectors. These are pointed to by the registers in
## mn10300_interrupt_vectors and must be in the range 0x4000XXXX.

	hardware_vector	0
	hardware_vector	1
	hardware_vector	2
	hardware_vector	3
	hardware_vector	4
	hardware_vector	5
	hardware_vector	6
		
##-----------------------------------------------------------------------------
## Startup code
			
	.text

	.globl	_start
_start:

#if defined(CYG_HAL_MN10300_STDEVAL1) 
	mov	INIT_MEMCTR0,d2
	movhu	d2,(MEMCTR0)

	mov	INIT_MEMCTR1,d2
	movhu	d2,(MEMCTR1)

	mov	INIT_MEMCTR2,d2
	movhu	d2,(MEMCTR2)

	mov	INIT_REFCNT,d2
	movhu	d2,(REFCNT)

	mov	INIT_DRAMCTR,d2
	movhu	d2,(DRAMCTR)

	# Set up LED
	
	mov	INIT_P2MD,d2
	movbu	d2,(P2MD)

	mov	INIT_P2DIR,d2
	movbu	d2,(P2DIR)


#endif	
	
	# Set up the PSW

	mov	0x0700,d0
	mov	d0,psw

	# set up stack

	mov	__interrupt_stack,a0
	sub	8,a0
	mov	a0,sp
	
#ifdef CYG_HAL_STARTUP_ROM
	
	# Copy data from ROM to RAM

	mov	__rom_data_start,a0
	mov	__ram_data_end,a1
	mov	__ram_data_start,a2
	
	cmp	a2,a1
	beq	8f
1:	
	movbu	(a0),d0
	movbu	d0,(a2)
	inc	a0
	inc	a2
	cmp	a2,a1
	bne	1b	
8:	
	
#endif
	
	# Clear BSS
	mov	__bss_start,a0
	mov	__bss_end,a1

	cmp	a0,a1
	beq	8f
	clr	d0
1:	
	movbu	d0,(a0)
	inc	a0
	cmp	a0,a1
	bne	1b	
8:	
	# Install interrupt vectors

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_MN10300_SIM)
	mov	_mn10300_interrupt_vectors,a0
	mov	__hardware_vector_0,d0
	movhu	d0,(0,a0)
	mov	__hardware_vector_1,d0
	movhu	d0,(4,a0)
	mov	__hardware_vector_2,d0
	movhu	d0,(8,a0)
	mov	__hardware_vector_3,d0
	movhu	d0,(12,a0)
	mov	__hardware_vector_4,d0
	movhu	d0,(16,a0)
	mov	__hardware_vector_5,d0
	movhu	d0,(20,a0)
	mov	__hardware_vector_6,d0
	movhu	d0,(24,a0)
#endif
#if defined(CYG_HAL_STARTUP_RAM) && !defined(CYG_HAL_MN10300_SIM)

	# init vsr table in SRAM where the ROM
	# vectors the interrupts.

	mov	_hal_vsr_table,a0
	mov	__default_interrupt_vsr,d0
	mov	d0,(0,a0)	
	mov	d0,(4,a0)
	mov	d0,(8,a0)
#ifndef CYG_HAL_USE_ROM_MONITOR_CYGMON
	# When using Cygmon, leave level 3 for GDB
	# Ctrl-C interrupts.
	mov	d0,(12,a0)	
#endif	
	mov	d0,(16,a0)	
	mov	d0,(20,a0)	
	mov	d0,(24,a0)
	mov	__default_nmi_vsr,d0
	mov	d0,(28,a0)
	mov	__default_trap_vsr,d0
	mov	d0,(32,a0)
#endif
	
	# Call constructors

	.extern _cyg_hal_invoke_constructors
	call    _cyg_hal_invoke_constructors,[],0

	.extern _cyg_hal_enable_caches
	call    _cyg_hal_enable_caches,[],0

	.extern _cyg_hal_debug_init
	call    _cyg_hal_debug_init,[],0

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        .extern _initialize_stub
        call    _initialize_stub,[],0
#endif

       	# Call cyg_start
	clr	d0
	mov	d0,(4,sp)

	.extern _cyg_start
	call    _cyg_start,[],0

9:	
	bra	9b		# Loop if we return

##-----------------------------------------------------------------------------
## Debug code to count interrupts on the led. 

#ifdef CYG_HAL_MN10300_STDEVAL1

	.macro	show_interrupts	
	mov	(led_count),d1
	inc	d1
#	cmp	20000,d1
#	ble	2f
	mov	0,d1
	movbu	(led_value),d0
	movbu	d0,(P2OUT)
	add	16,d0
	cmp	0x90,d0
	bne	1f
	mov	0,d0
1:		
	movbu	d0,(led_value)
2:
	mov	d1,(led_count)
	.endm

#else

	.macro	show_interrupts	
	.endm
			
#endif
	
##-----------------------------------------------------------------------------
## The following macros are defined depending on whether the HAL is configured
## to support the kernel or not. 

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	
	.extern	cyg_scheduler_sched_lock
	
	# Increment the scheduler lock
			
	.macro increment_sched_lock reg=d0
	mov	(cyg_scheduler_sched_lock),\reg
	inc	\reg
	mov	\reg,(cyg_scheduler_sched_lock)
	.endm

#else
	
	.macro increment_sched_lock reg=d0
	.endm

#endif		

##-----------------------------------------------------------------------------
## The following macros are defined depending on whether the Interrupt
## system is using isr tables or chaining.
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN

#if defined(CYG_HAL_MN10300_MN103000)

#define CYG_ISR_TABLE_SIZE	7
		
	# decode the interrupt
	.macro  decode_interrupt dreg,areg,dreg1
	mov	_mn10300_interrupt_control,\areg
	movhu	(0x100,\areg),\dreg		# dreg = IAGR
	movhu	(\dreg,\areg),\dreg1		# dreg1 = IGR[dreg]
	and	0xf,\dreg1			# dreg1 = ID0..3
	mov	hal_lsbit_table,\areg		# areg = ls bit table
	movbu	(\dreg1,\areg),\dreg1		# dreg1 = ls bit index
	add	\dreg,\dreg1			# dreg1 = word index of isr
	mov	(0,a2),\dreg			# dreg = vector priority
	mov	\dreg1,(0,a2)			# store real vector in saved state
	asl	2,\dreg				# dreg = byte index of isr
	.endm
	
#elif defined(CYG_HAL_MN10300_MN103002)	

#define CYG_ISR_TABLE_SIZE	10
	
	# decode the interrupt
	.macro  decode_interrupt dreg,areg,dreg1
	mov	_mn10300_interrupt_control,\areg
	movhu	(0x100,\areg),\dreg1		# dreg1 = IAGR
	mov	(0,a2),\dreg			# dreg = vector priority
	mov	\dreg1,(0,a2)			# store real vector in saved state
	asl	2,\dreg				# dreg = byte index of isr
	add	12,\dreg			# skip NMI vectors
	.endm
		
#endif

#else

#if defined(CYG_HAL_MN10300_MN103000)

#define CYG_ISR_TABLE_SIZE	100

	# decode the interrupt
	.macro  decode_interrupt dreg,areg,dreg1
	mov	_mn10300_interrupt_control,\areg
	movhu	(0x100,\areg),\dreg		# dreg = IAGR
	movhu	(\dreg,\areg),\dreg1		# dreg1 = IGR[dreg]
	and	0xf,\dreg1			# dreg1 = ID0..3
	mov	hal_lsbit_table,\areg		# areg = ls bit table
	movbu	(\dreg1,\areg),\dreg1		# dreg1 = ls bit index
	add	\dreg1,\dreg			# dreg = word index of isr
	mov	\dreg,(0,a2)			# store real vector in saved state
	asl	2,\dreg				# dreg = byte index of isr
	.endm

	
#elif defined(CYG_HAL_MN10300_MN103002)	

#define CYG_ISR_TABLE_SIZE	 34

	# decode the interrupt
	.macro  decode_interrupt dreg,areg,dreg1
	mov	_mn10300_interrupt_control,\areg
	movhu	(0x100,\areg),\dreg		# dreg = IAGR
	mov	\dreg,(0,a2)			# store real vector in saved state
	add	12,\dreg			# skip NMI vectors
	.endm
		
#endif
		
#endif
	
##-----------------------------------------------------------------------------
## Default interrupt VSR

	.text
	.globl	__default_interrupt_vsr
__default_interrupt_vsr:

	# We come here with all the registers pushed
	# onto the stack.

	increment_sched_lock
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	# Increment interrupt nesting counter
	mov	__interrupt_stack,a0		# A0 = interrupt stack top
	mov	sp,a2				# A2 = SP
	cmp	__interrupt_stack_base,a2	# compare with base of stack
	blt	1f				# if lt switch to int stack
	cmp	a0,a2				# compare sp with stack top
	ble	8f				# if le already on istack
1:
	mov	a0,sp				# switch to new SP
8:
	movm	[a2],(sp)			# save old SP

#else
	mov	sp,a2				# A2 = saved thread state
#endif

	# Here A2 -> saved thread state on the threads own
	# stack. We will be executing either on the same stack
	# or on the interrupt stack, depending on config options.
		
	decode_interrupt d3,a3,d0

	# Here D3 contains the table byte offset of the vector to
	# call. On the MN103000 this needs to be shifted right by
	# two to derive the vector. On the MN103002 this correponds
	# exactly to the vector used to attach it.

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)

	# Call cyg_instrument to record that this interrupt is being raised.
	
	add	-16,sp				# make space for return link + args	
	mov	0x0301,d0			# d0 = type = INTR,RAISE
	mov	(0,a2),d1			# d1 = arg1 = vector
	mov	d3,(12,sp)			# (12,sp) = arg2 = table offset
	calls	_cyg_instrument			# call instrumentation
	add	16,sp				# pop space
	
#endif		

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        .extern _cyg_hal_gdb_isr
        add     -16,sp                          # make space for return link + args
        mov     (0,a2),d0                       # d0 = vector
        mov     (56,a2),d1                      # d1 = pc
        calls   _cyg_hal_gdb_isr                # call GDB isr function
        cmp     0x00,d0                         # Call ISR proper?
        beq     2f                              # (d0 is 0 when skipping
                                                #  to avoid DSR call)
        add     16,sp                           # pop space
#endif
	
#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

	# To allow nested interrupts, we set the IE bit. We do
	# not touch the IPL bits, so only higher priority interrupts
	# will be nested on top of us. Also, new interrupts will not
	# be delivered until the ISR calls 
	# Cyg_Interrupt::acknowledge_interrupt(). At some future point
	# we may want to do the ack stuff here to allow immediate nesting.
	
	or	0x0800,psw
	
#endif	
	mov	_hal_interrupt_handlers,a0	# a0 = isr table
	mov	(d3,a0),a0			# a0 = isr
	
	mov	_hal_interrupt_data,a1		# a1 = data table
	mov	(d3,a1),d1			# d1 = isr data

	mov	(0,a2),d0			# d0 = vector. (d3 is...
						# ...adjusted for table reads)
#if defined(CYG_HAL_MN10300_MN103000)	
	lsr	2,d0				# d0 = vector number
#endif
	add	-16,sp				# make space for return link

	calls	(a0)				# call isr

	# on return d0 bit 1 will indicate whether a DSR is
	# to be posted. Pass this together with a pointer to
	# the interrupt object we have just used to the
	# interrupt tidy up routine.
	
	# D3 is defined to be saved across procedure calls, and
	# should still contain the vector byte index. Similarly,
	# A2 should still point to the saved machine state.

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        # If interrupt was caused by GDB, the ISR call above
        # is skipped by jumping here.
2:
#endif

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

	# If we are returning from the last nested interrupt, move back
	# to the thread stack. interrupt_end() must be called on the
	# thread stack since it potentially causes a context switch.
	
	add	16,sp				# pop call frame from int stack
	movm	(sp),[a3]			# pop old sp
	mov	a3,sp				# put in SP

	add	-16,sp				# make call frame for call

#endif		

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	
	# We only need to call _interrupt_end() when there is a kernel
	# present to do any tidying up.
		
	# Using the vector offset in D3, get the interrupt object pointer
	# into D1.
	mov	_hal_interrupt_objects,a0	# a0 = object table
	mov	(d3,a0),d1			# d1 = object

	# Even when this is not the last nested interrupt, we must call
	# _interrupt_end() to post the DSR and decrement the scheduler
	# lock.
		
	# interrupt_end must be called with interrupts enabled.
	# At this point we restore the PSW of the code we interrupted
	# to get the IPL bits back. By definition this PSW must have
	# interrupts enabled.
	mov	(52,a2),d2

	mov	d2,psw

	mov	a2,(12,sp)			# arg3 = saved state.
	
	calls	_interrupt_end			# call interrupt end fn		
#endif

	add	16,sp				# pop return link

	show_interrupts
	
	movm	(sp),[d2,d3,a2,a3,other]	# pop regs
	add	4,sp
	rti					# and return

hal_lsbit_table:
	.byte	0, 0, 1, 0
	.byte	2, 0, 1, 0
	.byte	3, 0, 1, 0
	.byte	2, 0, 1, 0


##-----------------------------------------------------------------------------
## Default NMI VSR

#define NMICR	0x34000100
#define DCR	0x20000030
#define ISR	0x20000034

	.globl	_cyg_hal_exception_handler
	
	.text
	.globl	__default_nmi_vsr
__default_nmi_vsr:

	# We come here with all the registers saved
	# on the stack.

	# Decode the cause of the NMI and cancel all the bits in all
	# the registers. We need to clear any bits set in the ISR and
	# then clear any bits set in the NMICR. Note that we can only 
	# access the ISR if the DCR:DE bit is set. 
	
	movhu	(NMICR),d0			# D0 = NMI Control register

	movhu	(DCR),d1			# D1 = Debug Control Register
	mov	d1,d2				# D2 = copy of DCR
	movhu	(ISR),d3			# D3 = Interrupt Status Register
	
	or	0x0010,d1			# Set DE bit
	movhu	d1,(DCR)			

	movhu	d3,(ISR)			# clear ISR bits

	movhu	d2,(DCR)			# restore DCR to original value

	movhu	d0,(NMICR)			# clear NMI bits
	and	0x7,d0				# LS 3 bits only

	mov	hal_lsbit_table,a0		
	movbu	(d0,a0),d1			# D1 = NMI code
						## 0 = NMI
						## 1 = Watchdog
						## 2 = System Error

	
	# It appears that there is an undocumented extra bit
	# in the PSW that masks the delivery of NMIs. It is in
	# the upper 16 bits of the register that we cannot access
	# directly. So, to clear it, we must set up a fake interrupt
	# state and do an RTI to load the PSW. We need to do this if
	# we are going to be able to set breakpoints in NMI handlers.
	# Note that the AM33 has a documented NMID at bit 17 of the
	# extended PSW (along with instructions to access it).
	
	mov	PSW,d3				# D3 = PSW
	and	0xFFFF,d3
	mov	1f,d2				# D2 = Next PC
	movm	[d2,d3],(sp)			# Push
	rti					# and load into CPU
1:	
	
	mov	sp,a1				# a1 = saved state
	mov	a1,d0				# d0 = arg1 = saved state

	add	-16,sp				# return link + args
	mov	d1,(16,sp)			# save in spare save slot

	calls	_cyg_hal_exception_handler	# call C code
	add	16,sp				# pop args
	movm	(sp),[d2,d3,a2,a3,other]	# pop all registers
	add	4,sp
	rti

##-----------------------------------------------------------------------------
## Default TRAP VSR

	.text
	.globl	__default_trap_vsr
__default_trap_vsr:

	# We come here with all the registers saved
	# on the stack.

	add	-8,sp				# return link + arg
	mov	3,d1				# 3 == TRAP trap
	mov	d1,(0,sp)			# save in spare save slot
	calls	_cyg_hal_exception_handler	# call C code
	add	8,sp				# pop args
	movm	(sp),[d2,d3,a2,a3,other]	# pop all registers
	add	4,sp
	rets
				
##-----------------------------------------------------------------------------
## VSR table. The VSRs pointed to by this table are called from the stubs
## connected to the hardware.

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_MN10300_SIM)
			
	.data

	.globl	_hal_vsr_table
_hal_vsr_table:	
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_nmi_vsr
	.long	__default_trap_vsr

#endif
		
##-----------------------------------------------------------------------------
## Interrupt tables
	
	.section ".bss"

	.globl	_hal_interrupt_handlers
_hal_interrupt_handlers:
	.rept	CYG_ISR_TABLE_SIZE
	.long	0
	.endr
	
	.globl	_hal_interrupt_data
_hal_interrupt_data:
	.rept	CYG_ISR_TABLE_SIZE
	.long	0
	.endr
	
	.globl	_hal_interrupt_objects
_hal_interrupt_objects:
	.rept	CYG_ISR_TABLE_SIZE
	.long	0
	.endr
	
			
##-----------------------------------------------------------------------------
## Temporary interrupt stack
	
	.section ".bss"

	.balign 16
__interrupt_stack_base:
	.rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
	.byte 0
	.endr
	.balign 16
__interrupt_stack:
	
	.long	0,0,0,0,0,0,0,0	

##-----------------------------------------------------------------------------
		
#ifdef CYG_HAL_MN10300_STDEVAL1
	.data
## Keep alignment to work around compiler/linker bug
led_count:	.long 0
led_value:	.byte 0x40
led_foo1:	.byte 0x00
led_foo2:	.byte 0x00
led_foo3:	.byte 0x00
#endif

##-----------------------------------------------------------------------------
## end of vectors.S

