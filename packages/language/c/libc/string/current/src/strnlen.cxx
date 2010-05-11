//===========================================================================
//
//      strnlen.cxx
//
//      GNU extension strnlen() routine
//
//===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998-2010 Free Software Foundation, Inc.
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour, wry
// Contributors: 
// Date:         2010-05-10
// Purpose:     
// Description: 
// Usage:       
// Note:         Derivative work of strlen.cxx.
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_string.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <string.h>                // Header for this file
#include <stddef.h>         // Compiler definitions such as size_t, NULL etc.
#include <cyg/libc/string/stringsupp.hxx> // Useful string function support and
                                          // prototypes

// EXPORTED SYMBOLS

externC size_t
strnlen( const char *s , size_t maxlen ) CYGBLD_ATTRIB_WEAK_ALIAS(__strnlen);

// FUNCTIONS

size_t
__strnlen( const char *s , size_t maxlen )
{
    int retval;
    
    CYG_REPORT_FUNCNAMETYPE( "__strnlen", "returning length %d" );
    CYG_REPORT_FUNCARG1( "s=%08x", s );

    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );

#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) || defined(__OPTIMIZE_SIZE__)
    const char *start = s;
    
    while (*s && maxlen)
        s++, maxlen--;
    
    retval = s - start;

    CYG_REPORT_RETVAL( retval );

    return retval;

#else

    const char *start = s;
    CYG_WORD *aligned_addr;
    
    if (CYG_LIBC_STR_UNALIGNED (s))
    {
        while (*s && maxlen)
            s++, maxlen--;
        retval = s - start;

        CYG_REPORT_RETVAL( retval );
    
        return retval;
    }
    
    // If the string is word-aligned, we can check for the presence of 
    // a null in each word-sized block.
    
    aligned_addr = (CYG_WORD *)s;
    while (!CYG_LIBC_STR_DETECTNULL (*aligned_addr) && (maxlen>= sizeof(CYG_WORD)))
        aligned_addr++, maxlen -= sizeof(CYG_WORD);
    
    // Once a null is detected, we check each byte in that block for a
    // precise position of the null.
    s = (char*)aligned_addr; 
    while (*s && maxlen)
        s++, maxlen--;
    retval = s - start;

    CYG_REPORT_RETVAL( retval );
    
    return retval;
#endif // not defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) ||
       //     defined(__OPTIMIZE_SIZE__)
} // __strnlen()

// EOF strnlen.cxx
