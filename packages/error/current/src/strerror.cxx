//===========================================================================
//
//      strerror.cxx
//
//      ANSI error code string routine
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:        jlarmour
// Date:        1998-06-11
// Purpose:     To provide the strerror() implementation
// Description: This implements strerror() as described in ANSI chap 7.11.6.2
// Usage:       See <cyg/error/codes.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================


// CONFIGURATION

#include <pkgconf/error.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_ERROR

// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <cyg/infra/cyg_trac.h>   // Tracing support
#include <cyg/error/codes.h>      // Error code definitions and header for this
                                  // file

// EXPORTED SYMBOLS

externC char *
strerror( int errnum ) __attribute__ ((weak, alias("_strerror") ));

// FUNCTIONS

externC char *
_strerror( int errnum )
{
    register char *s;
    
    CYG_REPORT_FUNCNAMETYPE( "_strerror", "String form of error is \"%s\"" );

    switch (errnum)
    {

#ifdef ENOERR
    case ENOERR:
        s = "No error";
        break;
#endif

#ifdef EPERM
    case EPERM:
        s = "Not permitted";
        break;
#endif

#ifdef ENOENT
    case ENOENT:
        s = "No such entity";
        break;
#endif

#ifdef ESRCH
    case ESRCH:
        s = "No such process";
        break;
#endif

#ifdef EINTR
    case EINTR:
        s = "Operation interrupted";
        break;
#endif

#ifdef EIO
    case EIO:
        s = "I/O error";
        break;
#endif

#ifdef EBADF
    case EBADF:
        s = "Bad file handle";
        break;
#endif

#ifdef EAGAIN
    case EAGAIN:
        s = "Try again later";
        break;
#endif

#ifdef ENOMEM
    case ENOMEM:
        s = "Out of memory";
        break;
#endif

#ifdef EBUSY
    case EBUSY:
        s = "Resource busy";
        break;
#endif

#ifdef ENODEV
    case ENODEV:
        s = "No such device";
        break;
#endif

#ifdef EINVAL
    case EINVAL:
        s = "Invalid argument";
        break;
#endif

#ifdef EMFILE
    case EMFILE:
        s = "Too many open files";
        break;
#endif

#ifdef EDOM
    case EDOM:
        s = "Argument to math function outside valid domain";
        break;
#endif

#ifdef ERANGE
    case ERANGE:
        s = "Math result cannot be represented";
        break;
#endif

#ifdef ENOSYS
    case ENOSYS:
        s = "Function not implemented";
        break;
#endif

#ifdef EEOF
    case EEOF:
        s = "End of file reached";
        break;
#endif

#ifdef ENOSUPP
    case ENOSUPP:
        s = "Operation not supported";
        break;
#endif

#ifdef EDEVNOSUPP
    case EDEVNOSUPP:
        s = "Device does not support this operation";
        break;
#endif

    default:
        s = "Unknown error";
        break;

    } // switch

    CYG_REPORT_RETVAL(s);

    return s;
} // _strerror()

#endif // ifdef CYGPKG_ERROR

// EOF strerror.cxx
