#ifndef CYGONCE_HAL_HAL_DIAG_H
#define CYGONCE_HAL_HAL_DIAG_H

/*=============================================================================
//
//      hal_diag.h
//
//      HAL Support for Kernel Diagnostic Routines
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   hmt
// Contributors:        hmt
// Date:        1999-01-11
// Purpose:     HAL Support for Kernel Diagnostic Routines
// Description: Diagnostic routines for use during kernel development.
// Usage:       #include <cyg/hal/hal_diag.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_sparclite.h>
#include <pkgconf/hal_sparclite_sleb.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_hwio.h>

/*---------------------------------------------------------------------------*/
/* SPARClite Evaluation Board - SLEB - 86940 SDTR output (+ the LEDs)        */

#define SLEB_LED (*(volatile char *)(0x02000003))

#define SLEB_86940_SDTR0_OUT
#ifdef  SLEB_86940_SDTR0_OUT


#define HAL_SPARC_86940_FLAG_TXRDY  (0x01)
#define HAL_SPARC_86940_FLAG_RXRDY  (0x02)

#define HAL_DIAG_WRITE_CHAR_DIRECT(_c_) CYG_MACRO_START                     \
    cyg_uint32 status = 0;                                                  \
    while ( 0 == (HAL_SPARC_86940_FLAG_TXRDY & status) ) {                  \
        HAL_SPARC_86940_SDTR0_STAT_READ( status );                          \
    }                                                                       \
    HAL_SPARC_86940_SDTR0_TXDATA_WRITE( _c_ );                              \
CYG_MACRO_END

#define HAL_DIAG_WRITE_CHAR_WAIT_FOR_EMPTY() CYG_MACRO_START                \
    cyg_uint32 status = HAL_SPARC_86940_FLAG_TXRDY;                         \
    while ( 0 != (HAL_SPARC_86940_FLAG_TXRDY & status) ) {                  \
        HAL_SPARC_86940_SDTR0_STAT_READ( status );                          \
    }                                                                       \
CYG_MACRO_END



#ifdef CYG_KERNEL_DIAG_GDB
// then use routines from hal_diag.c:

externC void hal_diag_init(void);
externC void hal_diag_write_char(char c);
#define HAL_DIAG_INIT() hal_diag_init()
#define HAL_DIAG_WRITE_CHAR(_c_) CYG_MACRO_START                    \
    SLEB_LED = (_c_); /* immediately */                             \
    hal_diag_write_char(_c_);                                       \
CYG_MACRO_END


#else // CYG_KERNEL_DIAG_GDB
// else go to the serial line directly (after initialization):

externC void hal_diag_init(void);
#define HAL_DIAG_INIT() hal_diag_init()

#define HAL_DIAG_WRITE_CHAR(_c_) CYG_MACRO_START                    \
    SLEB_LED = (_c_);                                               \
    HAL_DIAG_WRITE_CHAR_DIRECT( _c_ );                              \
CYG_MACRO_END

#endif // CYG_KERNEL_DIAG_GDB



#define HAL_DIAG_READ_CHAR(_c_) (_c_) = 0


#else
/*---------------------------------------------------------------------------*/
/* SPARClite Evaluation Board - SLEB - Just use the LEDs on board            */

#define HAL_DIAG_INIT()

#define HAL_DIAG_WRITE_CHAR(_c_) CYG_MACRO_START                            \
      SLEB_LED = (_c_);                                                     \
CYG_MACRO_END

#define HAL_DIAG_READ_CHAR(_c_) (_c_) = 0


#endif // SLEB_86940_SDTR0_OUT

/*---------------------------------------------------------------------------*/
/* end of hal_diag.h                                                         */
#endif /* CYGONCE_HAL_HAL_DIAG_H */
