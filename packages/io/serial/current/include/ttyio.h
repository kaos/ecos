#ifndef CYGONCE_TTYIO_H
#define CYGONCE_TTYIO_H
// ====================================================================
//
//      ttyio.h
//
//      Device I/O 
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     Special support for tty I/O devices
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// This file contains the user-level visible I/O interfaces

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/serialio.h>
#include <cyg/io/config_keys.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    cyg_uint32         tty_out_flags;
    cyg_uint32         tty_in_flags;
} cyg_tty_info_t;

// TTY flags - used to control behaviour when sending data to tty
#define CYG_TTY_OUT_FLAGS_CRLF  0x0001  // Map '\n' => '\r\n' on output

#define CYG_TTY_OUT_FLAGS_DEFAULT (CYG_TTY_OUT_FLAGS_CRLF)

// TTY flags - used to control behaviour when receiving data from tty
#define CYG_TTY_IN_FLAGS_CR      0x0001  // Map '\r' => '\n' on input
#define CYG_TTY_IN_FLAGS_CRLF    0x0002  // Map '\r\n' => '\n' on input
#define CYG_TTY_IN_FLAGS_ECHO    0x0004  // Echo characters as processed
#define CYG_TTY_IN_FLAGS_BINARY  0x0008  // No input processing

#define CYG_TTY_IN_FLAGS_DEFAULT (CYG_TTY_IN_FLAGS_CR|CYG_TTY_IN_FLAGS_ECHO)

#ifdef __cplusplus
}
#endif

#endif  /* CYGONCE_TTYIO_H */
/* EOF ttyio.h */
