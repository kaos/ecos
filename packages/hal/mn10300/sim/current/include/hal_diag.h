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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1998-03-02
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
/* Register addresses                                                        */
  
#define PAL_RESET        0x00
#define PAL_CPU_NR       0x04
#define PAL_INT          0x08
#define PAL_NR_CPU       0x0a

#define PAL_READ_FIFO    0x10
#define PAL_READ_STATUS  0x14
#define PAL_WRITE_FIFO   0x18
#define PAL_WRITE_STATUS 0x1a

#define OEA_DEV          0x31000000

/*---------------------------------------------------------------------------*/

#define HAL_DIAG_INIT()

#define HAL_DIAG_WRITE_CHAR(_c_)                        \
{                                                       \
    volatile unsigned char *tty_buffer =                \
        (unsigned char*)(OEA_DEV + PAL_WRITE_FIFO);     \
    volatile unsigned char *tty_status =                \
        (unsigned char*)(OEA_DEV + PAL_WRITE_STATUS);   \
    if( _c_ != '\r' )                                   \
    {                                                   \
        while( *tty_status == 0 ) continue;             \
        *tty_buffer = _c_;                              \
    }                                                   \
}

#define HAL_DIAG_READ_CHAR(_c_)                         \
{                                                       \
    volatile unsigned char *tty_buffer =                \
        (unsigned char*)(OEA_DEV + PAL_READ_FIFO);      \
    volatile unsigned char *tty_status =                \
        (unsigned char*)(OEA_DEV + PAL_READ_STATUS);    \
    while( *tty_status == 0 ) continue;                 \
    _c_ = *tty_buffer;                                  \
}

/*---------------------------------------------------------------------------*/
/* end of hal_diag.h                                                         */
#endif /* CYGONCE_HAL_HAL_DIAG_H */
