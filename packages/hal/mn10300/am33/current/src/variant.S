##=============================================================================
##
##	variant.S
##
##	MN10300 AM33 variant code
##
##=============================================================================
#####COPYRIGHTBEGIN####
#                                                                          
# -------------------------------------------                              
# The contents of this file are subject to the Red Hat eCos Public License 
# Version 1.1 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://www.redhat.com/                                                   
#                                                                          
# Software distributed under the License is distributed on an "AS IS"      
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
# License for the specific language governing rights and limitations under 
# the License.                                                             
#                                                                          
# The Original Code is eCos - Embedded Configurable Operating System,      
# released September 30, 1998.                                             
#                                                                          
# The Initial Developer of the Original Code is Red Hat.                   
# Portions created by Red Hat are                                          
# Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
# All Rights Reserved.                                                     
# -------------------------------------------                              
#                                                                          
#####COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	nickg
## Contributors:	nickg
## Date:	1997-10-16
## Purpose:	MN10300 AM33 variant code
## Description:	This file contains variant specific assembly code for the AM33.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/arch.inc>	


##-----------------------------------------------------------------------------
## NMI trampoline VSRs. All NMI interrupts are routed here initially, where we
## decode the NMICR and ISR register contents and vector to the
## correct VSR later in the table.

	.text

	.extern hal_lsbit_table
			
	.globl nmi_vsr_trampoline
nmi_vsr_trampoline:
	movhu	(NMICR),d0		# D0 = NMI control register
	and	0x7,d0			# LS 3 bits only
	mov	3,d1			# search from bit 3
	bsch	d0,d1			# get ms bit in d1
	bcs	2f			# jump if no bits there
	add	9,d1			# D1 = offset into VSR table
	jmp	3f			# go to rest of code

	.globl nmi_sysef_trampoline
nmi_sysef_trampoline:		
	movhu	(ISR),d0		# D0 = Interrupt Status Register
	mov	d0,d1			# D1 = copy of D0
	add	-1,d1			# D1 = D0-1
	not	d1			# D1 = ~(D0-1) = -D0
	and	d1,d0			# D0 = D0 & -D0 = ls bit only
	mov	16,d1			# D1 = start of search bit
	bsch	d0,d1			# search for 1 bit in d0
	bcs	2f			# jump if failed
	add	12,d1			# d1 = offset in VSR table
3:	
	asl	2,d1			# D1 = word offset in vsr table
	mov	_hal_vsr_table,a0	# A0 = VSR table base
	add	d1,a0			# A0 = address of table entry we want
	mov	(a0),a0			# A0 = VSR to call
	jmp	(a0)			# Call it
	
2:	
	# no bits set in ISR or NMICR when we expected them.
	# This should never happen, but if it does, use an otherwise
	# unused VSR table entry to indicate this.
	
	mov	27,d1			# use last VSR table entry.
	jmp	3b
	
	
##-----------------------------------------------------------------------------

#define HAL_CHCTR               0xC0000070
#define HAL_DCACHE_PURGE_WAY0   0xC8400000
#define HAL_CHCTR_DCEN          0x0002
#define HAL_CHCTR_DCBUSY        0x0008

	.text
	
	.globl	_cyg_hal_dcache_store
_cyg_hal_dcache_store:	
	movm	[d2,d3],(sp)

	mov	HAL_CHCTR,a0			# A0 = control reg
	mov	HAL_DCACHE_PURGE_WAY0,a1	# A1 = purge base address
	
	# Disable DCACHE if it is enabled

	movhu	(a0),d2				# D2 = old value of control reg

	btst	HAL_CHCTR_DCEN,d2		# check for cache enabled
	beq	1f				# if not, skip disable
	
	mov	d2,d3				# make a copy of CHCTR
	and	~HAL_CHCTR_DCEN,d3		# clear DCEN bit
	movhu	d3,(a0)				# store in reg

2:	movhu	(a0),d3				# get CHCTR
	btst	HAL_CHCTR_DCBUSY,d3		# test DCBUSY bit
	bne	2b				# loop while set

1:
	# The cache is now disabled
	
	and	0x000003f0,d0			# isolate index bits of base addr
	add	d0,a1				# offset a1 to base address

	add	63,d1				# adjust size to whole multiple of
	and	0xFFFFFFC0,d1			# set size.
3:	
	mov	(0x0000,a1),d0			# purge way 0
	mov	(0x1000,a1),d0			# purge way 1
	mov	(0x2000,a1),d0			# purge way 2
	mov	(0x3000,a1),d0			# purge way 3

	add	16,a1				# advance to next set

	sub	64,d1				# decrement count
	bne	3b				# loop while non zero
	
	# Restore original cache state from saved CHCTR in D2

	movhu	d2,(a0)				
		
	movm	(sp),[d2,d3]			# restore work regs
		
	rets	

	
##-----------------------------------------------------------------------------
## end of variant.S
	

	
