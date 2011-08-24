#ifndef CYGONCE_CORTEXM_REGS_H
#define CYGONCE_CORTEXM_REGS_H
//==========================================================================
//
//      cortexm_regs.h
//
//      Cortex-M architecture, special machine instruction wrappers
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Ilija Kocho <ilijak@siva.com.mk>
//               Sergei Gavrikov
// Date:         2011-06-18
// Description:  C wrappers for some special architecture instructions.
//
//####DESCRIPTIONEND####
//
//=========================================================================

#ifndef __ASSEMBLER__

//---------------------------------------------------------------------------
// Change processor state instructions

// Disable / enable interrupts
#define CYGARC_CPSID( _flags_ ) __asm__ volatile ("cpsid " #_flags_ "\n")

// Enable interrupts and fault handlers (clear FAULTMASK)
#define CYGARC_CPSIE( _flags_ ) __asm__ volatile ("cpsie " #_flags_ "\n")

//---------------------------------------------------------------------------
// Byte swapping instructions

// Reverse word
#define CYGARC_REV( _swapped_, _origin_ ) \
        __asm__ volatile ("rev %0, %1\n" : "=r"(_swapped_) : "r"(_origin_))

// Reverse halfwords
#define CYGARC_REV16( _swapped_, _origin_ ) \
        __asm__ volatile ("rev16 %0, %1\n" : "=r"(_swapped_) : "r"(_origin_))

// Reverse signed halfword
#define CYGARC_REVSH( _swapped_, _origin_ ) \
        __asm__ volatile ("revsh %0, %1\n" : "=r"(_swapped_) : "r"(_origin_))

#endif // __ASSEMBLER__

//==========================================================================
#endif // CYGONCE_CORTEXM_REGS_H
