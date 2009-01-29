##=============================================================================
##
##	arch.inc
##
##	M68K architecture assembler header file
##
##=============================================================================
## ####ECOSGPLCOPYRIGHTBEGIN####                                            
## -------------------------------------------                              
## This file is part of eCos, the Embedded Configurable Operating System.   
## Copyright (C) 2003, 2006, 2008 Free Software Foundation, Inc.            
##
## eCos is free software; you can redistribute it and/or modify it under    
## the terms of the GNU General Public License as published by the Free     
## Software Foundation; either version 2 or (at your option) any later      
## version.                                                                 
##
## eCos is distributed in the hope that it will be useful, but WITHOUT      
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
## for more details.                                                        
##
## You should have received a copy of the GNU General Public License        
## along with eCos; if not, write to the Free Software Foundation, Inc.,    
## 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
##
## As a special exception, if other files instantiate templates or use      
## macros or inline functions from this file, or you compile this file      
## and link it with other works to produce a work based on this file,       
## this file does not by itself cause the resulting work to be covered by   
## the GNU General Public License. However the source code for this file    
## must still be made available in accordance with section (3) of the GNU   
## General Public License v2.                                               
##
## This exception does not invalidate any other reasons why a work based    
## on this file might be covered by the GNU General Public License.         
## -------------------------------------------                              
## ####ECOSGPLCOPYRIGHTEND####                                              
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	bartv
## Date:	2003-06-04
######DESCRIPTIONEND####
##=============================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/var.inc>

// ----------------------------------------------------------------------------
// Utility macros for defining normal and weak functions.
        
#define FUNC_START(name)                        \
        .section .text. ## name ;               \
        .balign 4 ;                             \
        .globl name ;                           \
        .type  name, @function          ;       \
name:   

#define FUNC_START_WEAK(name)                   \
        .section .text._ ## name ;              \
        .weak name ;                            \
        .set name, _ ## name ;                  \
        .balign 4 ;                             \
        .globl _ ## name ;                      \
        .type  name, @function          ;       \
_ ## name:   

// ----------------------------------------------------------------------------
// Default system and interrupt stack. This is used only if the
// platform HAL has not already supplied suitable stacks.

#ifndef _HAL_M68K_STARTUP_STACK_
	.extern _hal_m68k_startup_stack_base_
	.extern _hal_m68k_startup_stack_
# define _HAL_M68K_STARTUP_STACK_	# _hal_m68k_startup_stack_
# define _HAL_M68K_INSTANTIATE_STARTUP_STACK_
#endif
#ifndef _HAL_M68K_INTERRUPT_STACK_BASE_
	.extern _hal_m68k_interrupt_stack_base_
	.extern _hal_m68k_interrupt_stack_
# define _HAL_M68K_INTERRUPT_STACK_BASE_	# _hal_m68k_interrupt_stack_base_
# define _HAL_M68K_INTERRUPT_STACK_		# _hal_m68k_interrupt_stack_
# define _HAL_M68K_INSTANTIATE_INTERRUPT_STACK_
#endif

#------------------------------------------------------------------------------
# end of arch.inc
