//========================================================================
//
//      itoa.cxx
//
//      FIXME
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
// Author(s):    jlarmour
// Contributors: jlarmour
// Date:         1999-03-17
// Purpose:       
// Description:   
// Usage:        
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_ass.h>     // Assertion infrastructure
#include <cyg/infra/cyg_trac.h>    // Tracing infrastructure
#include <stdlib.h>                // Header for this file
#include <string.h>                // For memcpy()

// FUNCTIONS

/////////////////////
// cyg_libc_itoa() //
/////////////////////
//
// This converts num to a string and puts it into s padding with
// "0"'s if padzero is set, or spaces otherwise if necessary.
// The number of chars written to s is returned
//

// This implementation is probably suboptimal in terms of performance
// but there wouldn't be much in it with only 11 chars max to convert :-/.
// Actually FIXME: what if someone passes a width >11

externC cyg_uint8
cyg_libc_itoa( cyg_uint8 *s, cyg_int32 num, cyg_uint8 width, cyg_bool padzero )
{
    CYG_REPORT_FUNCNAMETYPE("cyg_libc_itoa", "returning %d");
    CYG_REPORT_FUNCARG4( "s=%08x, num=%d, width=%d, padzero=%d",
                         s, num, width, padzero );

    CYG_CHECK_DATA_PTR(s, "input string not a valid pointer");

    // special case for zero otherwise we'd have to treat it specially later
    // on anyway
    
    if (num==0) {
        cyg_ucount8 i;
        
        for (i=0; i<width; ++i)
            s[i] = padzero ? '0' : ' ';
        CYG_REPORT_RETVAL(i);
        return i;
    }
    
    // return value
    cyg_uint8 ret=0;
    
    // Pre-fiddle for negative numbers
    if ((num < 0) && (width > 0)) {
        *s++ = '-';
        --width;
        num = abs(num);
        ++ret;
    }
        
    cyg_ucount32 i;
    cyg_bool reachednum = false;
    cyg_uint8 c, j;

    // i starts off with factor of 10 digits - which is the string length
    // of a positive 32-bit number
    for (i=1000000000, j=10; i>0; i/=10, --j) {
        c = (num / i);

        if (!reachednum && c==0) {
            if (j <= width) {
                *s++ = padzero ?  '0' : ' ';
                ++ret;
            }
        } // if
        else {
            *s++ = c + '0';
            ++ret;
            reachednum = true;
        }
        num %= i;
    } // for
    
    CYG_POSTCONDITION(ret >= width, "Didn't output enough chars!");

    CYG_REPORT_RETVAL(ret);
    return ret;
} // cyg_libc_itoa()

// EOF itoa.cxx
