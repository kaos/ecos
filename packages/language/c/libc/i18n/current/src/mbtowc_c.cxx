//===========================================================================
//
//      mbtowc_c.cxx
//
//      Internal __mbtowc_c() routine 
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jjohnstn
// Contributors:  jjohnstn
// Date:          2000-11-24
// Purpose:       Provide internal use __mbtowc_c() routine
// Description:   C locale version of mbtowc()
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================
//

// CONFIGURATION

#include <pkgconf/libc_i18n.h>     // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <locale.h>
#include <stdlib.h>                // Header for this file
#include <string.h>                // strcmp definition
#include <stddef.h>                // size_t definition
#include "internal.h"              // internal __isxxxx macros

// TRACE

#if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_STDLIB_MBTOWC_TRACE_LEVEL)
static int mbtowc_trace = CYGNUM_LIBC_STDLIB_MBTOWC_TRACE_LEVEL;
# define TL1 (0 < mbtowc_trace)
#else
# define TL1 (0)
#endif

// STATICS

// FUNCTIONS

int
__mbtowc_c ( wchar_t *pwc, const char *s, size_t n, int *state )
{
  wchar_t dummy;
  unsigned char *t = (unsigned char *)s;
  int retval;

  CYG_REPORT_FUNCNAMETYPE( "__mbtowc_c", "returning %d" );
  CYG_REPORT_FUNCARG4( "pwc=%08x, s=%08x, n=%ud, state=%08x", pwc, s, n, state );
  
  if (pwc != NULL)
    CYG_CHECK_DATA_PTR( pwc, "pwc is not a valid pointer!" );
  if (s != NULL)
    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );
 
  if (pwc == NULL)
    pwc = &dummy;

  if (s != NULL && n == 0)
    {
      retval = -1;
      CYG_REPORT_RETVAL (retval);
      return retval;
    }

  if (s == NULL)
    retval = 0;  /* not state-dependent */
  else
    {
      if (pwc)
	*pwc = (wchar_t)*t;
      retval = (*t != '\0');
    }
  CYG_REPORT_RETVAL (retval);
  return retval;
} // __mbtowc_c()

// EOF mbtowc_c.cxx
