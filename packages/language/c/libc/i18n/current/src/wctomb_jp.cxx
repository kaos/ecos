//===========================================================================
//
//      wctomb_jp.cxx
//
//      Internal __wctombc_jp() routine 
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
// Date:          2000-11-16
// Purpose:       Provide internal use __wctomb_jp() routine
// Description:   Japanese locale version of wctomb()
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================
//
// This code was taken from newlib/libc/stdlib/wctomb_r.c
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
#include "internal.h"              // internal _isxxxx macros

#ifdef CYGSEM_LIBC_STDLIB_PER_THREAD_WCTOMB
# include <pkgconf/kernel.h>       // kernel configuration
# include <cyg/kernel/thread.hxx>  // per-thread data
# include <cyg/kernel/thread.inl>  // per-thread data
# include <cyg/kernel/mutex.hxx>   // mutexes
#endif

// TRACE

#if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_STDLIB_WCTOMB_TRACE_LEVEL)
static int wctomb_trace = CYGNUM_LIBC_STDLIB_WCTOMB_TRACE_LEVEL;
# define TL1 (0 < wctomb_trace)
#else
# define TL1 (0)
#endif

// FUNCTIONS

int
__wctomb_jp ( char *s, wchar_t wchar, int *state )
{
  const char *cur_locale = __current_ctype_locale->name;
  int         retval;

  CYG_REPORT_FUNCNAMETYPE( "__wctomb_jp", "returning %d" );
  CYG_REPORT_FUNCARG3( "s=%08x, wchar=%08x, state=%08x", s, wchar, state );
  
  if (s != NULL)
    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );
  CYG_CHECK_DATA_PTR( state, "state is not a valid pointer!" );

  if (strlen (cur_locale) <= 1)
    { /* fall-through */ }
#ifdef CYGFUN_LIBC_I18N_LOCALE_C_SJIS
  else if (!strcmp (cur_locale, "C-SJIS"))
    {
      unsigned char char2 = (unsigned char)wchar;
      unsigned char char1 = (unsigned char)(wchar >> 8);

      if (s == NULL)
	{
	  retval =  0;  /* not state-dependent */
	  CYG_REPORT_RETVAL( retval );
	  return retval;
	}

      if (char1 != 0x00)
        {
        /* first byte is non-zero..validate multi-byte char */
          if (_issjis1(char1) && _issjis2(char2)) 
            {
              *s++ = (char)char1;
              *s = (char)char2;
              retval = 2;
            }
          else
            retval = -1;

	  CYG_REPORT_RETVAL( retval );
	  return retval;
        }
    }
#endif /* CYGFUN_LIBC_I18N_LOCALE_C_SJIS */
#ifdef CYGFUN_LIBC_I18N_LOCALE_C_EUCJP
  else if (!strcmp (cur_locale, "C-EUCJP"))
    {
      unsigned char char2 = (unsigned char)wchar;
      unsigned char char1 = (unsigned char)(wchar >> 8);

      if (s == NULL)
	{
	  retval =  0;  /* not state-dependent */
	  CYG_REPORT_RETVAL( retval );
	  return retval;
	}

      if (char1 != 0x00)
        {
        /* first byte is non-zero..validate multi-byte char */
          if (_iseucjp (char1) && _iseucjp (char2)) 
            {
              *s++ = (char)char1;
              *s = (char)char2;
              retval = 2;
            }
          else
            retval = -1;

	  CYG_REPORT_RETVAL( retval );
	  return retval;
        }
    }
#endif /* CYGFUN_LIBC_I18N_LOCALE_C_EUCJP */
#ifdef CYGFUN_LIBC_I18N_LOCALE_C_JIS
  else if (!strcmp (cur_locale, "C-JIS"))
    {
      int cnt = 0; 
      unsigned char char2 = (unsigned char)wchar;
      unsigned char char1 = (unsigned char)(wchar >> 8);

      if (s == NULL)
	{
	  retval = 1;  /* state-dependent */
	  CYG_REPORT_RETVAL( retval );
	  return retval;
	}

      if (char1 != 0x00)
        {
        /* first byte is non-zero..validate multi-byte char */
          if (_isjis (char1) && _isjis (char2)) 
            {
              if (*state == 0)
                {
                  /* must switch from ASCII to JIS state */
                  *state = 1;
                  *s++ = ESC_CHAR;
                  *s++ = '$';
                  *s++ = 'B';
                  cnt = 3;
                }
              *s++ = (char)char1;
              *s = (char)char2;
              retval = cnt + 2;
            }
          else
            retval = -1;
        }
      else
        {
          if (*state != 0)
            {
              /* must switch from JIS to ASCII state */
              *state = 0;
              *s++ = ESC_CHAR;
              *s++ = '(';
              *s++ = 'B';
              cnt = 3;
            }
          *s = (char)char2;
          retval = cnt + 1;
        }

      CYG_REPORT_RETVAL( retval );
      return retval;
    }
#endif /* CYGFUN_LIBC_I18N_LOCALE_C_JIS */

  if (s == NULL)
    retval = 0;
  else
    {
      *s = (char) wchar;
      retval = 1;
    }

  CYG_REPORT_RETVAL( retval );
  return retval;

} // __wctomb_jp()

// EOF wctomb_jp.cxx
