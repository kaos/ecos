//==========================================================================
//
//      common/delete.cxx
//
//      Default delete implementation
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1999-08-13
// Purpose:     Gives a default delete operation if the C library isn't present
// Description: This file supplies a definition of the default delete
//              operation (aka __builtin_delete and __builtin_vec_delete)
//              for use when the normal delete can't be used - normally when
//              the C library is not present
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>

#include <cyg/infra/cyg_type.h>

// The configuration here is as follows:
//
// To deal with virtual destructors, where the correct delete() function
// must be called for the derived class in question, the underlying delete
// is called when needed, from destructors.  This is regardless of whether
// the destructor is called by delete itself.  So there is a reference to
// delete() from all destructors.  The default builtin delete() attempts to
// call free() if there is one defined.  So, if you have destructors, and
// you have free(), as in malloc() and free() in the C library, any
// destructor counts as a reference to free().  So the libc heap system is
// linked in regardless of whether it gets explicitly called.
//
// To defeat this undesirable behaviour, we define empty versions of delete
// and delete[].  But doing this prevents proper use of the libc heap in
// C++ programs via C++'s new and delete operators.
//
// Therefore, configuration is provided (in the libc package, near malloc
// et al) for explicitly disabling the provision of these empty functions,
// so that new and delete can be used, if that's what's required.
//
// It is also possible to override all this complexity anyway, but that is
// not yet represented in an external config option.



// If this is already externally defined, go ahead and override the
// builtin delete functions.
#ifndef CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS

# define CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS
# ifdef CYGPKG_LIBC
// then there is a libc: if there is a malloc, maybe do not supply empty
// delete functions.  Get the libc config info to find out.

#  include <pkgconf/libc.h>

// So, do NOT provide these functions if there is a malloc and we are not
// to provide these functions:
#  ifdef CYGPKG_LIBC_MALLOC
#   ifdef CYGIMP_LIBC_MALLOC_CXX_DELETE_CALLS_FREE
#    undef CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS
#   endif // CYGIMP_LIBC_MALLOC_CXX_DELETE_CALLS_FREE
#  endif // CYGPKG_LIBC_MALLOC
# endif // CYGPKG_LIBC
#endif // CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS not already defined

#ifdef CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS
// then define these empty functions:

void operator delete(void *x) throw()
{
    CYG_EMPTY_STATEMENT;
}

void operator delete[](void *x) throw()
{
    CYG_EMPTY_STATEMENT;
}

#endif // CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS

// EOF delete.cxx
