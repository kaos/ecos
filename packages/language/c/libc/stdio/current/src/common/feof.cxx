//========================================================================
//
//      feof.cxx
//
//      Implementations of ISO C feof(), ferror(), clearerr() functions
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
// Copyright (C) 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2001-03-16
// Purpose:       Implementations of ISO C feof(), ferror(), clearerr()
//                functions
// Description:   
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>          // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>           // Common type definitions and support
#include <stdio.h>                        // Header for this file
#include <cyg/libc/stdio/stream.hxx>      // Cyg_StdioStream class

externC int
feof( FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;

    return (real_stream->get_eof_state() != 0);
} // feof()

externC int
ferror( FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;

    return (real_stream->get_error() != 0);
} // ferror()

externC void
clearerr( FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;

    real_stream->set_error(0);
    real_stream->set_eof_state(false);
} // clearerr()



// EOF feof.cxx
