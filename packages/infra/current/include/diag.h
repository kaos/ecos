#ifndef CYGONCE_INFRA_DIAG_H
#define CYGONCE_INFRA_DIAG_H

/*=============================================================================
//
//      diag.h
//
//      Diagnostic Routines for Infra Development
//
//==========================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:        nickg, gthomas
// Date:        1998-03-02
// Purpose:     Diagnostic Routines for Infra Development
// Description: Diagnostic routines for use during infra development.
// Usage:       #include <cyg/infra/diag.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================*/

#include <pkgconf/infra.h>
#include <cyg/infra/cyg_type.h>
#include <stdarg.h>

/*---------------------------------------------------------------------------*/
/* Diagnostic routines                                                       */

externC void diag_init(void);         /* Initialize, call before any others*/

externC void diag_write_char(char c); /* Write single char to output       */

externC void diag_write_string(const char *psz); /* Write zero terminated string */

externC void diag_write_dec( cyg_int32 n);    /* Write decimal value       */

externC void diag_write_hex( cyg_uint32 n);   /* Write hexadecimal value   */

externC void diag_dump_buf(void *buf, CYG_ADDRWORD len);
externC void diag_dump_buf_with_offset(cyg_uint8     *p, 
                                       CYG_ADDRWORD   s, 
                                       cyg_uint8     *base);

externC int  diag_printf( const char *fmt, ... );  /* Formatted print      */

externC void diag_init_putc(void (*putc)(char c, void **param));
externC int  diag_sprintf(char *buf, const char *fmt, ...);
externC int  diag_vsprintf(char *buf, const char *fmt, va_list ap);
externC int  diag_vprintf(const char *fmt, va_list ap);


/*---------------------------------------------------------------------------*/
/* Internal Diagnostic MACROS                                                */

#define DIAG_DEVICE_START_SYNC()
#define DIAG_DEVICE_END_SYNC()

/*---------------------------------------------------------------------------*/
#endif /* CYGONCE_INFRA_DIAG_H */
/* EOF diag.h */
