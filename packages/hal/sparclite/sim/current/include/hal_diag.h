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
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
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

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

/*---------------------------------------------------------------------------*/

#define CYG_DIAG_USE_ERC32

/*---------------------------------------------------------------------------*/

#ifdef  CYG_DIAG_USE_ERC32

/*---------------------------------------------------------------------------*/
/* Register addresses                                                        */

#define SPARC_MEC_UART              (0x01f800e0)

/* These must be accessed word-wide to work! */
#define SPARC_MEC_UART_IO( x )      ((cyg_uint32)(x))

#define SPARC_MEC_UART_A_RX         ((volatile cyg_uint32 *)(SPARC_MEC_UART + 0))
#define SPARC_MEC_UART_A_TX         ((volatile cyg_uint32 *)(SPARC_MEC_UART + 0))
#define SPARC_MEC_UART_B_RX         ((volatile cyg_uint32 *)(SPARC_MEC_UART + 4))
#define SPARC_MEC_UART_B_TX         ((volatile cyg_uint32 *)(SPARC_MEC_UART + 4))
#define SPARC_MEC_UART_STATUS       ((volatile cyg_uint32 *)(SPARC_MEC_UART + 8))
#define SPARC_MEC_UART_RXAMASK      (0x00006)
#define SPARC_MEC_UART_RXBMASK      (0x60000)
#define SPARC_MEC_UART_TXAMASK      (0x00001)
#define SPARC_MEC_UART_TXBMASK      (0x10000)

  
/*---------------------------------------------------------------------------*/

#define HAL_DIAG_INIT()

#define HAL_DIAG_WRITE_CHAR(_c_)                                            \
{                                                                           \
    if( 1 || _c_ != '\r' )                                                  \
    {                                                                       \
        while( (SPARC_MEC_UART_RXAMASK & *SPARC_MEC_UART_STATUS) == 0 )     \
            continue;                                                       \
        *SPARC_MEC_UART_A_TX = SPARC_MEC_UART_IO(_c_);                      \
    }                                                                       \
}

#define HAL_DIAG_READ_CHAR(_c_)                                             \
{                                                                           \
    while( (SPARC_MEC_UART_TXAMASK & *SPARC_MEC_UART_STATUS) == 0 )         \
            continue;                                                       \
    _c_ = (char)*SPARC_MEC_UART_A_TX;                                       \
}

#define XHAL_DIAG_WRITE_CHAR(_c_)                                            \
{                                                                           \
    if( _c_ != '\r' )                                                       \
    {                                                                       \
        *SPARC_MEC_UART_A_TX = SPARC_MEC_UART_IO(_c_);                      \
    }                                                                       \
}

#define XHAL_DIAG_READ_CHAR(_c_)                                             \
{                                                                           \
    _c_ = (char)*SPARC_MEC_UART_A_TX;                                       \
}

#else
/*---------------------------------------------------------------------------*/
/* There is no diagnostic output on SPARCLITE simulator                      */

#define HAL_DIAG_INIT()

#define HAL_DIAG_WRITE_CHAR(_c_)

#define HAL_DIAG_READ_CHAR(_c_) (_c_) = 0

#endif

/*---------------------------------------------------------------------------*/
/* end of hal_diag.h                                                         */
#endif /* CYGONCE_HAL_HAL_DIAG_H */
