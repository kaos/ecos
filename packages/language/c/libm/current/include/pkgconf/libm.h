#ifndef CYGONCE_PKGCONF_LIBM_H
#define CYGONCE_PKGCONF_LIBM_H
//========================================================================
//
//      libm.h
//
//      Configuration header for mathematical function library
//
//========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     Configuration of the standard mathematical function library
// Description: Configuration header for mathematical function library
// Usage:       #include <pkgconf/libm.h>
//
//####DESCRIPTIONEND####
//
//========================================================================


//========================================================================
//
// A NOTE ON COMPATIBILITY MODES
//
// This math library is capable of being operated in several different
// compatibility modes. These options deal solely with how errors are
// handled.
//
// There are 4 compatibility modes: ANSI/POSIX 1003.1; IEEE-754; X/Open
// Portability Guide issue 3 (XPG3); and System V Interface Definition
// Edition 3.
//
// In IEEE mode, the matherr() function (see below) is never called, no
// warning messages are printed on the stderr output stream, and errno is
// never set.
//
// In ANSI/POSIX mode, errno is set correctly, but matherr() is never
// called and no warning messages are printed on the stderr output stream.
//
// In X/Open mode, errno is set correctly, matherr() is called, but no
// warning messages are printed on the stderr output stream.
//
// In SVID mode, functions which overflow return a value "HUGE" (defined in
// math.h), which is the maximum single precision floating point value
// (as opposed to HUGE_VAL which is meant to stand for infinity). errno is
// set correctly and matherr() is called. If matherr() returns 0, warning
// messages are printed on the stderr output stream for some errors.
//
// The mode can be compiled-in as IEEE-only, or any one of the above
// methods settable at run-time. See the configuration options below for
//  more.
//
//========================================================================
//
// MATHERR()
//
// As said, in X/Open or SVID modes, the user can supply a function
// matherr() of the form:
//
// int matherr( struct exception * );
//
// where:
// struct exception {
//     int type;
//     char *name;
//     double arg1, arg2, retval;
// };
//
// - type is the exception type and is one of:
// DOMAIN    - argument domain exception
// SING      - argument singularity
// OVERFLOW  - overflow range exception
// UNDERFLOW - underflow range exception
// TLOSS     - total loss of significance
// PLOSS     - partial loss of significance
//
// - name is a string containing the name of the function
// - arg1 and arg2 are the arguments passed to the function
// - retval is the default value that will be returned by the function, and
//   can be changed by matherr
// 
// matherr must have "C" linkage, not C++
//
// If matherr returns zero, or the user doesn't supply their own matherr,
// then the following _usually_ happens in SVID mode:
// 
// If type is:
//   DOMAIN    - 0.0 returned, errno=EDOM, and a message printed on stderr
//   SING      - HUGE of appropriate sign is returned, errno=EDOM, and a
//               message is printed on stderr
//   OVERFLOW  - HUGE of appropriate sign is returned, and errno=ERANGE
//   UNDERFLOW - 0.0 is returned and errno=ERANGE
//   TLOSS     - 0.0 is returned, errno=ERANGE, and a message is printed on
//               stderr
//   PLOSS     - The current implementation doesn't return this type
//
// X/Open mode is similar except that the message is not printed on stderr
// and HUGE_VAL is used in place of HUGE
//
//========================================================================
//
// THREAD-SAFETY AND RE-ENTRANCY
//
// With the appropriate configuration options set below, the math library
// is fully thread-safe if:
//
// - Depending on the compatibility mode, the setting of the errno variable
//   from the C library is thread-safe
//
// - Depending on the compatibility mode, sending error messages to the
//   stderr output stream using the C library fputs() function is
//   thread-safe
//
// - Depending on the compatibility mode, the user-supplied matherr()
//   function and anything it depends on are thread-safe
//
// In addition, with the exception of the gamma*() and lgamma*() functions,
// the math library is re-entrant (and thus safe to use from interrupt
// handlers) if the Math library is always in IEEE mode
//
//========================================================================
//
// Also note that this math library assumes the hardware (or software
// floating point emulation) supports IEEE-754 style arithmetic, 32-bit
// 2's complement integer arithmetic, doubles are in 64-bit
// IEEE-754 format.
//
//========================================================================


// INCLUDES

// Include system configuration file to work out whether the Math library
// is to be included at all
#include <pkgconf/system.h>

// We want to check if the kernel supports per-thread data
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

/*

    {{CFG_DATA
        cdl_package CYGPKG_LIBM {
            display      "Math library"
            description  "ISO standard floating point mathematical library
                          containing many useful functions for mathematical
                          calculations."
            type         boolean
            requires     CYGPKG_ERROR
            doc          ref/ecos-ref/the-iso-standard-c-and-math-libraries.html
        }
    }}CFG_DATA

*/

// TYPE DEFINITIONS

// Compatibility mode selector - required for default below

typedef enum {
    CYGNUM_LIBM_COMPAT_UNINIT= 0,  // Default state. DO NOT set it to this
    CYGNUM_LIBM_COMPAT_POSIX = 1,  // ANSI/POSIX 1003.1
    CYGNUM_LIBM_COMPAT_IEEE  = 2,  // IEEE-754
    CYGNUM_LIBM_COMPAT_XOPEN = 3,  // X/OPEN Portability guide issue 3
                                   // (XPG3)
    CYGNUM_LIBM_COMPAT_SVID  = 4   // System V Interface Definition 3rd
                                   // edition
} Cyg_libm_compat_t;


//========================================================================
//
// COMPATIBILITY-MODE RELATED CONFIGURATION OPTIONS

/*
    {{CFG_DATA

        cdl_component CYGPKG_LIBM_COMPATIBILITY {
            display      "Compatibility mode"
            description  "These options deal with behaviour related to
                          the various compatibility modes - POSIX, IEEE,
                          X/OPEN and SVID."
            doc          ref/ecos-ref/math-library-compatibility-modes.html
            type         dummy
            parent       CYGPKG_LIBM
        }

        cdl_option CYGSEM_LIBM_COMPAT_IEEE_ONLY {
            display      "IEEE-only"
            description  "The math library can be hard-coded to only
                          behave in one compatibility mode - IEEE. This
                          cannot be changed at run-time. IEEE mode is the
                          most minimal of the compatibility modes, and so
                          this will best help code size and speed, as well
                          as omitting the code for other compatibility
                          modes. If not defined, the math library can be
                          set at run-time to any of the supported
                          compatibility modes."
            type         radio
            parent       CYGPKG_LIBM_COMPATIBILITY
        }

        cdl_component CYGPKG_LIBM_COMPATIBILITY_DEFAULT {
            display      "Default mode"
            description  "If you want to have support for more than one
                          compatibility mode settable at run-time, rather
                          than hard-coded IEEE mode, this component lets
                          you choose which mode should be the default."
            type         radio
            parent       CYGPKG_LIBM_COMPATIBILITY
            requires     CYGPKG_LIBC
        }
        
        cdl_option CYGNUM_LIBM_COMPATIBILITY_POSIX {
            display      "POSIX"
            description  "This sets the default compatibility mode to
                          POSIX"
            type         radio
            parent       CYGPKG_LIBM_COMPATIBILITY_DEFAULT
        }

        cdl_option CYGNUM_LIBM_COMPATIBILITY_IEEE {
            display      "IEEE"
            description  "This sets the default compatibility mode to IEEE"
            type         radio
            parent       CYGPKG_LIBM_COMPATIBILITY_DEFAULT
        }

        cdl_option CYGNUM_LIBM_COMPATIBILITY_XOPEN {
            display      "X/OPEN"
            description  "This sets the default compatibility mode to
                          X/OPEN"
            type         radio
            parent       CYGPKG_LIBM_COMPATIBILITY_DEFAULT
        }

        cdl_option CYGNUM_LIBM_COMPATIBILITY_SVID {
            display      "SVID"
            description  "This sets the default compatibility mode to SVID"
            type         radio
            parent       CYGPKG_LIBM_COMPATIBILITY_DEFAULT
        }

        cdl_option CYGFUN_LIBM_SVID3_scalb {
            display      "SVID3-style scalb()"
            description  "SVID3 defined the scalb() function as double
                          scalb(double, double) rather than double
                          scalb(double, int) which is used by IBM, DEC, and
                          probably others. Enabling this option chooses
                          the (double, double) version. Note there is a
                          function double scalbn(double, int) which is
                          unaffected by this choice."
            type         boolean
            parent       CYGPKG_LIBM_COMPATIBILITY
        }
        
        cdl_option CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION {
            display      "Reduce namespace pollution"
            description  "If you do not want to use either the X/Open or
                          SVID3 compatibility modes, you may want to define
                          this option to reduce the chance of namespace
                          pollution. This is particularly likely to occur
                          here as these standards define symbols with
                          names that often appear in applications, such as
                          exception, DOMAIN, OVERFLOW, etc. If your
                          application also used these names, it may cause
                          problems."
            type         boolean
            parent       CYGPKG_LIBM_COMPATIBILITY
        }

        cdl_option CYGSEM_LIBM_USE_STDERR {
            display      "Output to stderr for math errors"
            description  "The SVID3 standard says that error
                          messages should be output on the stderr console
                          output stream. This option allows this ability
                          to be explicitly controlled. However, this still
                          only has an effect in SVID3 compatibility mode."
            type         boolean
            requires     !CYGSEM_LIBM_COMPAT_IEEE_ONLY
            requires     CYGPKG_LIBC_STDIO
            parent       CYGPKG_LIBM_COMPATIBILITY
        }

    }}CFG_DATA
*/

#undef CYGSEM_LIBM_COMPAT_IEEE_ONLY
#define CYGPKG_LIBM_COMPATIBILITY_DEFAULT

#ifdef CYGPKG_LIBM_COMPATIBILITY_DEFAULT
# ifndef CYGPKG_LIBC
#  error You must include the libc package if enabling \
 CYGPKG_LIBM_COMPATIBILITY_DEFAULT
# endif
#endif

#define CYGNUM_LIBM_COMPATIBILITY_POSIX
#undef CYGNUM_LIBM_COMPATIBILITY_IEEE
#undef CYGNUM_LIBM_COMPATIBILITY_XOPEN
#undef CYGNUM_LIBM_COMPATIBILITY_SVID

#undef CYGNUM_LIBM_COMPAT_DEFAULT /* this is for permtest to find! */

#ifndef CYGNUM_LIBM_COMPAT_DEFAULT
# if defined(CYGPKG_LIBM_COMPATIBILITY_DEFAULT)
#  if defined(CYGNUM_LIBM_COMPATIBILITY_POSIX)
#   define CYGNUM_LIBM_COMPAT_DEFAULT CYGNUM_LIBM_COMPAT_POSIX
#  elif defined(CYGNUM_LIBM_COMPATIBILITY_IEEE)
#   define CYGNUM_LIBM_COMPAT_DEFAULT CYGNUM_LIBM_COMPAT_IEEE
#  elif defined(CYGNUM_LIBM_COMPATIBILITY_XOPEN)
#   define CYGNUM_LIBM_COMPAT_DEFAULT CYGNUM_LIBM_COMPAT_XOPEN
#  elif defined(CYGNUM_LIBM_COMPATIBILITY_SVID)
#   define CYGNUM_LIBM_COMPAT_DEFAULT CYGNUM_LIBM_COMPAT_SVID
#  else
#   error Configuration broken - no default mode set
#  endif
# endif
#endif

#define CYGFUN_LIBM_SVID3_scalb

#undef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION

#undef CYGSEM_LIBM_USE_STDERR

//========================================================================
//
// THREAD-SAFETY CONFIGURATION OPTIONS

/*
    {{CFG_DATA

        cdl_component CYGPKG_LIBM_THREAD_SAFETY {
            display      "Thread safety"
            description  "This option controls whether the C library has
                          support for thread safe operation in general.
                          This requires eCos kernel support for per-thread
                          data, and adjustment of the stack limit."
            type          dummy
            parent        CYGPKG_LIBM
            doc           ref/ecos-ref/c-and-math-library-overview-thread-safety.html
        }

        cdl_option CYGSEM_LIBM_THREAD_SAFE_COMPAT_MODE {
            display      "Compatibility mode setting"
            description  "This option makes the setting of the compatiblity
                          mode be a per-thread property. This directly
                          implies that it also becomes thread-safe."
            requires     !CYGSEM_LIBM_COMPAT_IEEE_ONLY
            requires     CYGVAR_KERNEL_THREADS_DATA
            parent       CYGPKG_LIBM_THREAD_SAFETY
        }
    }}CFG_DATA

*/


#ifndef CYGSEM_LIBM_COMPAT_IEEE_ONLY // irrelevant if mode is constant
#undef CYGSEM_LIBM_THREAD_SAFE_COMPAT_MODE
#endif


#if ( defined(CYGSEM_LIBM_THREAD_SAFE_COMPAT_MODE) && \
      !defined(CYGVAR_KERNEL_THREADS_DATA) )
# error The kernel must have thread-specific data \
  (CYGVAR_KERNEL_THREADS_DATA) turned on for a thread-safe math library
#endif

//========================================================================
//
// TRACING OPTIONS

/*
    {{CFG_DATA

        cdl_component CYGPKG_LIBM_TRACE {
            display      "Tracing output levels in Math library"
            description  "Tracing support is useful for debugging. Some
                          Math library modules can be configured with
                          different levels of tracing verbosity. These
                          levels can be configured here."
            type         boolean
            requires     CYGDBG_USE_TRACING
            parent       CYGPKG_LIBM
        }

        cdl_option CYGNUM_LIBM_COMPATMODE_TRACE_LEVEL {
            display      "Compatibility mode get/set"
            description  "Trace level for debugging the getting and
                          setting of the compatibility mode when it is
                          configured to be thread-safe."
            type         count
            legal_values 0 to 1
            requires     CYGSEM_LIBM_THREAD_SAFE_COMPAT_MODE
            parent       CYGPKG_LIBM_TRACE
        }
    }}CFG_DATA

*/

#undef CYGPKG_LIBM_TRACE

#ifdef CYGPKG_LIBM_TRACE

#define CYGNUM_LIBM_COMPATMODE_TRACE_LEVEL 0


#endif


#endif // CYGONCE_PKGCONF_LIBM_H multiple inclusion protection

// EOF libm.h
