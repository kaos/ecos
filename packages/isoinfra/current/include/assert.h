#ifndef CYGONCE_ISO_ASSERT_H
#define CYGONCE_ISO_ASSERT_H
/*========================================================================
//
//      assert.h
//
//      ISO C assertions
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
// Date:          2000-04-14
// Purpose:       This file provides the assert functions required by 
//                ISO C and POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <assert.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */
#include <pkgconf/infra.h>             /* CYGDBG_USE_ASSERTS */

/* INCLUDES */

#ifdef CYGBLD_ISO_ASSERT_HEADER
# include CYGBLD_ISO_ASSERT_HEADER
#else

# ifdef NDEBUG
#  define assert( __bool ) ((void)0)
# else /* if NDEBUG is NOT defined */

/* First preference is to be standards compliant */

#if defined(CYGINT_ISO_STDIO_FORMATTED_IO) && defined(CYGINT_ISO_EXIT)

# include <stdio.h>
# include <stdlib.h>

# define assert( __bool )                                                 \
    do {                                                                  \
        if (0 == (__bool)) {                                              \
            fprintf( stderr, "User assertion failed: \"%s\", at %s:%d\n", \
                         #__bool, __FILE__, __LINE__);                    \
            abort();                                                      \
        }                                                                 \
    } while(0)


/* Second preference is to use the common infra assertion support */

#elif defined(CYGDBG_USE_ASSERTS)

# include <cyg/infra/cyg_ass.h>

# define assert( __bool ) \
        CYG_MACRO_START   \
        CYG_ASSERT( __bool, "User assertion failed: \""  #__bool "\"" ); \
        CYG_MACRO_END
#else /* Fallback */

# include <cyg/infra/diag.h>

# define assert( __bool )                                                 \
    do {                                                                  \
        if (0 == (__bool)) {                                              \
            diag_printf( "User assertion failed: \"%s\", at %s:%d\n",     \
                         #__bool, __FILE__, __LINE__);                    \
        for (;;);                                                         \
        }                                                                 \
    } while(0)

#endif

# endif /* NDEBUG not defined */
#endif

#endif /* CYGONCE_ISO_ASSERT_H multiple inclusion protection */

/* EOF assert.h */
