//==========================================================================
//
//      fnmatch.c
//
//      Test fnmatch function
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2007 Free Software Foundation, Inc.                        
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           asl
// Contributors:        asl
// Date:                2007-01-27
// Purpose:             Test fnmatch function.
// Description:         //              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io_fileio.h>

#ifndef CYGPKG_FILEIO_FNMATCH
# define NA_MSG "FNMATCH function not available"
#endif

#include <cyg/infra/testcase.h>

#ifndef NA_MSG
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>

#include <fnmatch.h>

//==========================================================================
// main

int main( int argc, char **argv )
{
  int i;
  cyg_bool failed = false;
  
  struct 
  {
    const int result;
    const int flags;
    const char * string;
    const char * pattern;
  } test_case[] = {
    { 0,           0, "abc", "abc" },               /*  0 */
    { FNM_NOMATCH, 0, "cba", "abc" },
    { 0,           0, "abc", "a*"  },
    { FNM_NOMATCH, 0, "abc", "b*"  },
    { 0,           0, "abc", "[abc]*" },
    { FNM_NOMATCH, 0, "abc", "[def]*" },
    { FNM_NOMATCH, 0, "abc", "*[def]*" },
    { 0,           0, "a/b", "a/b" },
    { FNM_NOMATCH, 0, "a/b", "a/a" },
    { FNM_NOMATCH, 0, "a/b", "b/b" },

    { 0,           0,            "a b", "a\\ b"},  /* 10 */
    { FNM_NOMATCH, FNM_NOESCAPE, "a b", "a\\ b"},

    { 0,           0,            "a/b", "a*b" },
    { 0,           0,            "a/b", "a?b" },
    { 0,           0,            "a/b", "a[/]b" },
    { FNM_NOMATCH, FNM_PATHNAME, "a/b", "a*b"},
    { FNM_NOMATCH, FNM_PATHNAME, "a/b", "a?b"},
    { FNM_NOMATCH, FNM_PATHNAME, "a/b", "a[/]b"},
    { 0,           FNM_PATHNAME, "a/b", "a/b"},

    { 0,           0,          ".abc", "?abc" },
    { 0,           0,          ".abc", "*abc" },  /* 20 */
    { 0,           0,          ".abc", "[.]abc" },
    { 0,           0,          ".abc", ".abc" },
    { FNM_NOMATCH, FNM_PERIOD, ".abc", "?abc" },
    { FNM_NOMATCH, FNM_PERIOD, ".abc", "*abc" },
    { 0,           FNM_PERIOD, ".abc", "[.]abc" },       
    { 0,           FNM_PERIOD, ".abc", ".abc" },

    { 0,           0,                       "/.abc", "/?abc" },
    { 0,           0,                       "/.abc", "/*abc" },
    { 0,           0,                       "/.abc", "/[.]abc" },
    { 0,           0,                       "/.abc", "/.abc" },  /* 30 */
    { FNM_NOMATCH, FNM_PERIOD|FNM_PATHNAME, "/.abc", "/?abc" },
    { FNM_NOMATCH, FNM_PERIOD|FNM_PATHNAME, "/.abc", "/*abc" },
    { 0,           FNM_PERIOD|FNM_PATHNAME, "/.abc", "/[.]abc" },       
    { 0,           FNM_PERIOD|FNM_PATHNAME, "/.abc", "/.abc" }
  };
  
  CYG_TEST_INIT();

  for (i=0; i < CYG_NELEM(test_case); i++) {
    if (test_case[i].result != 
        fnmatch(test_case[i].pattern,
                test_case[i].string,
                test_case[i].flags)) {
      diag_printf("<INFO>: test number %d failed\n", i);
      failed = true;
    }
  }
  
  if (failed) {
    CYG_TEST_FAIL_FINISH("fnmatch failed");
  } else {
    CYG_TEST_PASS_FINISH("fnmatch passed");
  }
}

#else 

//==========================================================================
// main

int main( int argc, char **argv )
{
    CYG_TEST_INIT();

    CYG_TEST_NA(NA_MSG);
}

#endif

// -------------------------------------------------------------------------
// EOF fnmatch.c
