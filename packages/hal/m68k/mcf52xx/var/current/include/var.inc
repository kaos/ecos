##=============================================================================
##
##	var.inc
##
##	mcfxxxx variant assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 2003,2004,2006,2008 Free Software Foundation, Inc.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
##
## As a special exception, if other files instantiate templates or use macros
## or inline functions from this file, or you compile this file and link it
## with other works to produce a work based on this file, this file does not
## by itself cause the resulting work to be covered by the GNU General Public
## License. However the source code for this file must still be made available
## in accordance with section (3) of the GNU General Public License.
##
## This exception does not invalidate any other reasons why a work based on
## this file might be covered by the GNU General Public License.
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	bartv
## Date:	2003-06-04
######DESCRIPTIONEND####
##=============================================================================

// ----------------------------------------------------------------------------
// Generic support for the MAC and eMAC units.
// Not yet implemented.

// ----------------------------------------------------------------------------
// Now include the cpu-specific header, which can define context macros
// including the MAC, eMAC, and possibly other state
#include <cyg/hal/proc.inc>

// ----------------------------------------------------------------------------
// Thread context support. Usually this is generic for all ColdFires,
// but it is possible for processor or platform code to override
// these definitions.

// The 68K exception and interrupt VSR's can be used directly since the
// vector number is part of the hardware state automatically  pushed onto
// the stack. Since there are no trampolines there is no need to adjust
// the stack before returning.
#ifndef HAL_CONTEXT_PCSR_RTE_ADJUST
# define HAL_CONTEXT_PCSR_RTE_ADJUST	0
#endif

#ifndef HAL_CONTEXT_PCSR_SAVE_SR_DEFINED
	.macro hal_context_pcsr_save_sr reg=%sp,off=0,scratch=%d0
	mov.l	#0x40000000, \scratch
	mov.w 	%sr, \scratch
	mov.l	\scratch, (hal_context_pcsr_offset+\off)(\reg)
	.endm
#endif

// Get the ISR vector out of the context. This is encoded in one
// byte in the exception frame. There are two versions depending
// on the number of ISR vectors. If < 64, i.e. if an interrupt
// falls in the range 64 <= isrvec < 128, then a simple mask
// will do the trick. If > 164, i.e. if an interrupt falls in
// the range 64 <= isrvec < 256, then a subtract is required.

#ifndef HAL_CONTEXT_EXTRACT_ISR_DEFINED

# if HAL_M68K_VSR_COUNT > 128

        .macro hal_context_extract_isr_vector_shl2 reg=%sp,off=0,isr
	mov.w	(hal_context_pcsr_offset+\off)(\reg),\isr
	andi.l  #0x03FC, \isr
	subi.l	#0x0100, \isr
	.endm

        .macro hal_context_extract_isr_vector reg=%sp,off=0,isr
	mov.w	(hal_context_pcsr_offset+\off)(\reg),\isr
	andi.l  #0x03FC, \isr
	lsr.l	#2, \isr
	subi.l	#0x040, \isr
	.endm

# else
        .macro hal_context_extract_isr_vector_shl2 reg=%sp,off=0,isr
	mov.w	(hal_context_pcsr_offset+\off)(\reg),\isr
	andi.l  #0x00FC, \isr
	.endm

        .macro hal_context_extract_isr_vector reg=%sp,off=0,isr
	mov.w	(hal_context_pcsr_offset+\off)(\reg),\isr
	andi.l  #0x0FC, \isr
	lsr.l	#2, \isr
	.endm
# endif
#endif

#------------------------------------------------------------------------------
# end of var.inc
