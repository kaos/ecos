#ifndef CYGONCE_LIBC_STDIO_STDIOSUPP_HXX
#define CYGONCE_LIBC_STDIO_STDIOSUPP_HXX
//========================================================================
//
//      stdiosupp.hxx
//
//      Support for C library standard I/O routines
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-19
// Purpose:     
// Description: 
// Usage:         #include <cyg/libc/stdio/stdiosupp.hxx>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>     // common type definitions and support
#include <stdio.h>                  // Main standard I/O header
#include <cyg/libc/stdio/stream.hxx>// Cyg_StdioStream

// FUNCTION PROTOTYPES

//========================================================================

// Miscellaneous support functions

externC cyg_stdio_handle_t
Cyg_libc_stdio_find_filename( const char *filename,
                              const Cyg_StdioStream::OpenMode rw,
                              const cyg_bool binary,
                              const cyg_bool append );

externC Cyg_ErrNo
cyg_libc_stdio_flush_all_but( Cyg_StdioStream *not_this_stream );

#endif // CYGONCE_LIBC_STDIOL_STDIOSUPP_HXX multiple inclusion protection

// EOF stdiosupp.hxx
