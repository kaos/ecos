//===========================================================================
//
//      wcstombs.cxx
//
//      ISO standard wcstombs() routine 
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
// Date:          2000-11-02
// Purpose:       Provide ISO C wcstombs()
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================
//
// This code was based on newlib/libc/stdlib/wcstombs.c and newlib/libc/stdlib/wcstombs_r.c
// The following is modified from the original newlib description:
//
/*
FUNCTION
<<wcstombs>>---wide char string to multibyte string converter

INDEX
	wcstombs

ANSI_SYNOPSIS
	#include <stdlib.h>
	int wcstombs(const char *<[s]>, wchar_t *<[pwc]>, size_t <[n]>);

TRAD_SYNOPSIS
	#include <stdlib.h>
	int wcstombs(<[s]>, <[pwc]>, <[n]>)
	const char *<[s]>;
	wchar_t *<[pwc]>;
	size_t <[n]>;

DESCRIPTION
When CYGINT_LIBC_I18N_MB_REQUIRED is not defined, this is a minimal ANSI-conforming 
implementation of <<wcstombs>>.  In this case,
all wide-characters are expected to represent single bytes and so
are converted simply by casting to char.

When CYGINT_LIBC_I18N_MB_REQUIRED is defined, this routine calls the LC_CTYPE locale wcstomb_fn 
repeatedly to perform the conversion, passing a state variable to allow state dependent
decoding.  The result is based on the locale setting which may
be restricted to a defined set of locales.

RETURNS
This implementation of <<wcstombs>> returns <<0>> if
<[s]> is <<NULL>> or is the empty string; 
it returns <<-1>> if CYGINT_LIBC_I18N_MB_REQUIRED and one of the
wide-char characters does not represent a valid multi-byte character;
otherwise it returns the minimum of: <<n>> or the
number of bytes that are transferred to <<s>>, not including the
nul terminator.

If the return value is -1, the state of the <<pwc>> string is
indeterminate.  If the input has a length of 0, the output
string will be modified to contain a wchar_t nul terminator if
<<n>> > 0.

PORTABILITY
<<wcstombs>> is required in the ANSI C standard.  However, the precise
effects vary with the locale.

<<wcstombs>> requires no supporting OS subroutines.
*/

// CONFIGURATION

#include <pkgconf/libc_i18n.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <locale.h>
#include <stdlib.h>                // Header for this file
#include <string.h>                // strcmp definition
#include <stddef.h>                // size_t definition
#include "internal.h"              // __current_ctype_locale 

#ifdef CYGSEM_LIBC_I18N_PER_THREAD_WCSTOMBS
# include <pkgconf/kernel.h>       // kernel configuration
# include <cyg/kernel/thread.hxx>  // per-thread data
# include <cyg/kernel/thread.inl>  // per-thread data
# include <cyg/kernel/mutex.hxx>   // mutexes
#endif

// TRACE

#if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_I18N_WCSTOMBS_TRACE_LEVEL)
static int wcstombs_trace = CYGNUM_LIBC_I18N_WCSTOMBS_TRACE_LEVEL;
# define TL1 (0 < wcstombs_trace)
#else
# define TL1 (0)
#endif

// STATICS

// FUNCTIONS

size_t 
wcstombs ( char *s, const wchar_t *pwcs, size_t n )
{
  size_t  retval;
  
  CYG_REPORT_FUNCNAMETYPE( "wcstombs", "returning %ud" );
  CYG_REPORT_FUNCARG3( "s=%08x, pwcs=%08x, n=%ud", s, pwcs, n );

  if (s != NULL)
    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );
  if (pwcs != NULL)
    CYG_CHECK_DATA_PTR( pwcs, "pwcs is not a valid pointer!" );
  
#ifdef CYGINT_LIBC_I18N_MB_REQUIRED  

  char *ptr = s;
  size_t max = n;
  int state = 0;
  char buff[8];
  int i, num_to_copy;
  int (*wctomb_fn)(char *, wchar_t, int *) = __current_ctype_locale->wctomb_fn;

  if (wctomb_fn)
    {
      while (n > 0)
	{
	  int bytes = (size_t)(wctomb_fn (buff, *pwcs, &state));
	  if (bytes == -1)
	    {
	      retval = (size_t)-1;
	      CYG_REPORT_RETVAL( retval );
	      return retval;
	    }
	  num_to_copy = ((int)n > bytes ? bytes : (int)n);
	  for (i = 0; i < num_to_copy; ++i)
	    *ptr++ = buff[i];
	  
	  if (*pwcs == 0x00)
	    {
	      retval = ptr - s - ((int)n >= bytes);
	      CYG_REPORT_RETVAL( retval );
	      return retval;
	    }
	  ++pwcs;
	  n -= num_to_copy;
	}

      retval = max;
      CYG_REPORT_RETVAL( retval );
      return retval;
    }
#endif /* CYGINT_LIBC_I18N_MB_REQUIRED */
  
  int count = 0;

  if (n != 0) {
    do {
      if ((*s++ = (char) *pwcs++) == 0)
	break;
      count++;
    } while (--n != 0);
  }
  
  retval = count;
  CYG_REPORT_RETVAL( retval );
  return retval;
} // wcstombs()

// EOF wcstombs.cxx
