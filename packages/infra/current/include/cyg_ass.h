#ifndef CYGONCE_INFRA_CYG_ASS_H
#define CYGONCE_INFRA_CYG_ASS_H

//==========================================================================
//
//	assert.h
//
//	Macros and prototypes for the assert system
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg from an original by hmt
// Contributors:	nickg
// Date:	1997-09-08
// Purpose:	Use asserts to avoid writing duff code.
// Description:	Runtime tests that compile to nothing in
//              release versions of the code, to allow
//              as-you-go testing of alternate builds.
// Usage:	#include <cyg/infra/cyg_ass.h>
//		...
//		CYG_ASSERT( pcount > 0, "Number of probes should be > 0!" );
//
//      which can result, for example, in a message of the form:
//      ASSERT FAILED: probemgr.cxx:1340, scan_probes() :
//                     number of probes should be > 0!
//      if the boolean "pcount > 0" is false.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/infra.h>

#include <cyg/infra/cyg_type.h>         // for CYGBLD_NORET

// -------------------------------------------------------------------------
// If we do not have a function name macro, define it ourselves

#ifndef CYGDBG_INFRA_DEBUG_FUNCTION_PSEUDOMACRO
                                        // __PRETTY_FUNCTION__ does not work
# ifndef __PRETTY_FUNCTION__            // And it is not already defined
#  define __PRETTY_FUNCTION__ NULL
# endif
#endif

// ... before the dependency on having asserts at all.
// -------------------------------------------------------------------------

#ifdef CYGDBG_USE_ASSERTS

// -------------------------------------------------------------------------
// this is executed to deal with failure - breakpoint it first!

externC void
cyg_assert_fail( char *psz_func, char *psz_file,
                 cyg_uint32 linenum, char *psz_msg )
    CYGBLD_NORET;

// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
// We define macros and appropriate prototypes for the assert/fail
// system.  These are:
// 	CYG_FAIL	- unconditional panic
//	CYG_ASSERT	- panic if boolean expression is false
//

# ifdef CYGDBG_INFRA_DEBUG_ASSERT_MESSAGE
#  define CYGASSERT_docall( _msg_ ) \
        cyg_assert_fail( __PRETTY_FUNCTION__, __FILE__, __LINE__, _msg_ );
# else
#   define CYGASSERT_docall( _msg_ )    \
        CYG_MACRO_START                 \
        char *_tmp1_ = _msg_;           \
        _tmp1_ = _tmp1_;                \
        cyg_assert_fail( __PRETTY_FUNCTION__, __FILE__, __LINE__, NULL ); \
        CYG_MACRO_END
# endif

// unconditional failure; use like panic(), coredump() &c.
# define CYG_FAIL( _msg_ )              \
        CYG_MACRO_START                 \
        CYGASSERT_docall( _msg_ );      \
        CYG_MACRO_END

// conditioned assert; if the condition is false, fail.
# define CYG_ASSERT( _bool_, _msg_ )    \
        CYG_MACRO_START                 \
        if ( ! ( _bool_ ) )             \
            CYGASSERT_docall( _msg_ );  \
        CYG_MACRO_END

#else // ! CYGDBG_USE_ASSERTS

// -------------------------------------------------------------------------
// No asserts: we define empty statements for assert & fail.

# define CYG_ASSERT( _bool_, _msg_ ) CYG_EMPTY_STATEMENT
# define CYG_FAIL( _msg_ ) CYG_EMPTY_STATEMENT

#endif // ! CYGDBG_USE_ASSERTS

// -------------------------------------------------------------------------
// Pointer integrity checks.
// These check not only for NULL pointer, but can also check for pointers
// that are outside to defined memory areas of the platform or executable.
// We differentiate between data and function pointers, so that we can cope
// with different formats, and so we can check them against different memory
// regions.

externC cyg_bool cyg_check_data_ptr(void *ptr);
externC cyg_bool cyg_check_func_ptr(void (*ptr)(void));

#ifdef CYGDBG_USE_ASSERTS

# define CYG_CHECK_DATA_PTR( _ptr_, _msg_ )             \
        CYG_MACRO_START                                 \
        if( !cyg_check_data_ptr((void *)(_ptr_)))       \
           CYGASSERT_docall( _msg_ );                   \
        CYG_MACRO_END

# define CYG_CHECK_FUNC_PTR( _ptr_, _msg_ )             \
        CYG_MACRO_START                                 \
        if( !cyg_check_func_ptr((void (*)(void))(_ptr_))) \
           CYGASSERT_docall( _msg_ );                   \
        CYG_MACRO_END
        
#else // CYGDBG_USE_ASSERTS

# define CYG_CHECK_DATA_PTR( _ptr_, _msg_ ) CYG_EMPTY_STATEMENT
# define CYG_CHECK_FUNC_PTR( _ptr_, _msg_ ) CYG_EMPTY_STATEMENT           
           
           
#endif // CYGDBG_USE_ASSERTS
            
// -------------------------------------------------------------------------
// Unconditional definitions:

// Check an object for validity by calling its own checker.
// Usage:
//   ClassThing *p = &classobject;
//   CYG_ASSERTCLASS( p, "Object at p is broken!" );

// this enum gives some options as to how keenly to test; avoids cluttering
// the member function declaration if the implementor wants to do more
// zealous tests themselves.

enum cyg_assert_class_zeal {
  cyg_system_test       = -1,
  cyg_none              = 0,
  cyg_trivial,
  cyg_quick,
  cyg_thorough,
  cyg_extreme
};

// -------------------------------------------------------------------------
// Define macros for checking classes:
//
//	CYG_ASSERTCLASS         - do proforma check on a class pointer
//	CYG_ASSERTCLASSO        - do proforma check on a class object
//
// The idiom required is that a class have a member function called
// "bool check_this( cyg_assert_class_zeal )" that returns true iff the
// object is OK.  This need not be conditionally compiled against
// CYGDBG_USE_ASSERTS but it can be if only this macro is used to
// invoke it.  Alternatively it can be invoked by hand with other
// choices from the above enum.

// Assert the checker function of an object by pointer, or in hand.

#ifdef __cplusplus

# define CYG_ASSERT_CLASS_ZEAL (cyg_quick) // can be redefined locally

# define CYG_ASSERTCLASS( _pobj_, _msg_ ) \
    CYG_ASSERT( ((NULL != (_pobj_)) \
              && (_pobj_)->check_this( CYG_ASSERT_CLASS_ZEAL )), _msg_ );

# define CYG_ASSERTCLASSO( _obj_, _msg_ ) \
    CYG_ASSERT( (_obj_).check_this( CYG_ASSERT_CLASS_ZEAL ), _msg_ );

#define CYGDBG_DEFINE_CHECK_THIS \
    bool check_this( cyg_assert_class_zeal zeal );
    
#endif

// -------------------------------------------------------------------------
// Some alternative names for basic assertions that we can disable
// individually.
//
//      CYG_PRECONDITION        - argument checking etc
//      CYG_POSTCONDITION       - results etc
//      CYG_LOOP_INVARIANT      - for putting in loops

#ifdef CYGDBG_INFRA_DEBUG_PRECONDITIONS
# define CYG_PRECONDITION( _bool_ , _msg_ ) CYG_ASSERT( _bool_, _msg_ )
#else
# define CYG_PRECONDITION( _bool_ , _msg_ ) CYG_EMPTY_STATEMENT
#endif

#ifdef CYGDBG_INFRA_DEBUG_POSTCONDITIONS
# define CYG_POSTCONDITION( _bool_ , _msg_ ) CYG_ASSERT( _bool_, _msg_ )
#else
# define CYG_POSTCONDITION( _bool_ , _msg_ ) CYG_EMPTY_STATEMENT
#endif

#ifdef CYGDBG_INFRA_DEBUG_LOOP_INVARIANTS
# define CYG_LOOP_INVARIANT( _bool_ , _msg_ ) CYG_ASSERT( _bool_, _msg_ )
#else
# define CYG_LOOP_INVARIANT( _bool_ , _msg_ ) CYG_EMPTY_STATEMENT
#endif

// -------------------------------------------------------------------------
// Compile time failure; like #error but in a macro so we can use it in
// other definitions.
//
// Usage:
// #define new CYG_COMPILETIMEFAIL( "Do NOT use new!")

#define CYG_COMPILETIMEFAIL( _msg_ ) !!!-- _msg_ --!!!


// -------------------------------------------------------------------------

#endif // CYGONCE_INFRA_CYG_ASS_H multiple inclusion protection
// EOF cyg_ass.h
