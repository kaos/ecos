/*=============================================================================
//
//      hal_diag.h
//
//      HAL Support for Kernel Diagnostic Routines
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   larsi
// Contributors:
// Date:        2007-07-09
// Purpose:     HAL Support for Kernel Diagnostic Routines
// Description: Diagnostic routines for use during kernel development.
// Usage:       #include <cyg/hal/hal_diag.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#ifndef CYGONCE_HAL_DIAG_H
#define CYGONCE_HAL_DIAG_H

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

#include <cyg/hal/hal_if.h>

#define HAL_DIAG_INIT() hal_if_diag_init()
#define HAL_DIAG_WRITE_CHAR(_c_) hal_if_diag_write_char(_c_)
#define HAL_DIAG_READ_CHAR(_c_) hal_if_diag_read_char(&_c_)


#else // old way of doing diagnostic I/O

/*---------------------------------------------------------------------------*/
/* functions implemented in hal_diag.c (old way without virtual vectors)     */

externC void hal_diag_init(void);
externC void hal_diag_write_char(char c);
externC void hal_diag_read_char(char *c);

/*---------------------------------------------------------------------------*/

#define HAL_DIAG_INIT() hal_diag_init()
#define HAL_DIAG_WRITE_CHAR(_c_) hal_diag_write_char(_c_)
#define HAL_DIAG_READ_CHAR(_c_) hal_diag_read_char(&_c_)

#endif  /* CYGSEM_HAL_VIRTUAL_VECTOR_DIAG */

/*---------------------------------------------------------------------------*/
// serial port0 defines
#define CYG_HAL_FR30_MB91301_SMR0     0x63
#define CYG_HAL_FR30_MB91301_SCR0     0x62
#define CYG_HAL_FR30_MB91301_SIDR0    0x61
#define CYG_HAL_FR30_MB91301_SODR0    0x61
#define CYG_HAL_FR30_MB91301_SSR0     0x60
#define CYG_HAL_FR30_MB91301_UTIM0    0x64
#define CYG_HAL_FR30_MB91301_UTIMR0   0x64
#define CYG_HAL_FR30_MB91301_DRCL0    0x66
#define CYG_HAL_FR30_MB91301_UTIMC0   0x67

/*---------------------------------------------------------------------------*/
// serial port1 defines
#define CYG_HAL_FR30_MB91301_SMR1     0x6b
#define CYG_HAL_FR30_MB91301_SCR1     0x6a
#define CYG_HAL_FR30_MB91301_SIDR1    0x69
#define CYG_HAL_FR30_MB91301_SODR1    0x69
#define CYG_HAL_FR30_MB91301_SSR1     0x68
#define CYG_HAL_FR30_MB91301_UTIM1    0x6c
#define CYG_HAL_FR30_MB91301_UTIMR1   0x6c
#define CYG_HAL_FR30_MB91301_DRCL1    0x6e
#define CYG_HAL_FR30_MB91301_UTIMC1   0x6f

#define CYG_HAL_FR30_MB91301_PDRJ 0x13
#define CYG_HAL_FR30_MB91301_DDRJ 0x403
#define CYG_HAL_FR30_MB91301_PFRJ 0x413

/*---------------------------------------------------------------------------*/
// LED
#define CYG_HAL_FR30_MB91301_PDRG 0x10
#define CYG_HAL_FR30_MB91301_DDRG 0x400
#define CYG_HAL_FR30_MB91301_PFRG 0x410
// our MB91302A does not have PCRG (pull up resistor register G)
// but it is here anyway
#define CYG_HAL_FR30_MB91301_PCRG 0x420

// externC void hal_diag_init_led(void);
externC void hal_diag_led(cyg_uint8);

/*---------------------------------------------------------------------------*/
/* end of hal_diag.h                                                         */
#endif /* CYGONCE_HAL_DIAG_H */
