//===========================================================================
//
//      strerror.cxx
//
//      ANSI error code string routine
//
//===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-04-14
// Purpose:      To provide the strerror() implementation
// Description:  This implements strerror() as described in ANSI chap 7.11.6.2
// Usage:        See <cyg/error/codes.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================


// CONFIGURATION

#include <pkgconf/error.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <cyg/infra/cyg_trac.h>   // Tracing support
#include <cyg/error/codes.h>      // Error code definitions and header for this
                                  // file

// EXPORTED SYMBOLS

externC char *
strerror( int errnum ) __attribute__ ((weak, alias("__strerror") ));

// FUNCTIONS

externC char *
__strerror( int errnum )
{
    register char *s;
    
    CYG_REPORT_FUNCNAMETYPE( "__strerror", "String form of error is \"%s\"" );

    switch (errnum)
    {

#ifdef ENOERR
    case ENOERR:
        s = (char *)"No error";
        break;
#endif

#ifdef EPERM
    case EPERM:
        s = (char *)"Not permitted";
        break;
#endif

#ifdef ENOENT
    case ENOENT:
        s = (char *)"No such entity";
        break;
#endif

#ifdef ESRCH
    case ESRCH:
        s = (char *)"No such process";
        break;
#endif

#ifdef EINTR
    case EINTR:
        s = (char *)"Operation interrupted";
        break;
#endif

#ifdef EIO
    case EIO:
        s = (char *)"I/O error";
        break;
#endif

#ifdef EBADF
    case EBADF:
        s = (char *)"Bad file handle";
        break;
#endif

#ifdef EAGAIN
    case EAGAIN:
        s = (char *)"Try again later";
        break;
#endif

#ifdef ENOMEM
    case ENOMEM:
        s = (char *)"Out of memory";
        break;
#endif

#ifdef EBUSY
    case EBUSY:
        s = (char *)"Resource busy";
        break;
#endif

#ifdef ENODEV
    case ENODEV:
        s = (char *)"No such device";
        break;
#endif

#ifdef ENOTDIR
    case ENOTDIR:
        s = (char *)"Not a directory";
        break;
#endif

#ifdef EISDIR
    case EISDIR:
        s = (char *)"Is a directory";
        break;
#endif

#ifdef EINVAL
    case EINVAL:
        s = (char *)"Invalid argument";
        break;
#endif

#ifdef ENFILE
    case ENFILE:
        s = (char *)"Too many open files in system";
        break;
#endif

#ifdef EMFILE
    case EMFILE:
        s = (char *)"Too many open files";
        break;
#endif

#ifdef EFBIG
    case EFBIG:
        s = (char *)"File too large";
        break;
#endif
        
#ifdef ENOSPC
    case ENOSPC:
        s = (char *)"No space left on device";
        break;
#endif

#ifdef ESPIPE
    case ESPIPE:
        s = (char *)"Illegal seek";
        break;
#endif
        
#ifdef EROFS
    case EROFS:
        s = (char *)"Read-only file system";
        break;
#endif
        
#ifdef EDOM
    case EDOM:
        s = (char *)"Argument to math function outside valid domain";
        break;
#endif

#ifdef ERANGE
    case ERANGE:
        s = (char *)"Math result cannot be represented";
        break;
#endif

#ifdef EDEADLK
    case EDEADLK:
        s = (char *)"Resource deadlock would occur";
        break;
#endif

#ifdef ENOSYS
    case ENOSYS:
        s = (char *)"Function not implemented";
        break;
#endif

#ifdef ENAMETOOLONG
    case ENAMETOOLONG:
        s = (char *)"File name too long";
        break;
#endif
        
#ifdef ENOTSUP
    case ENOTSUP:
        s = (char *)"Not supported";
        break;
#endif

#ifdef EEOF
    case EEOF:
        s = (char *)"End of file reached";
        break;
#endif

#ifdef ENOSUPP
    case ENOSUPP:
        s = (char *)"Operation not supported";
        break;
#endif

#ifdef EDEVNOSUPP
    case EDEVNOSUPP:
        s = (char *)"Device does not support this operation";
        break;
#endif

#ifdef EXDEV
    case EXDEV:
        s = (char *)"Improper link";
        break;
#endif
        
// Additional errors used by networking
#ifdef ENXIO
    case ENXIO:
        s = (char *)"Device not configured";
        break;
#endif
#ifdef EACCES
    case EACCES:
        s = (char *)"Permission denied";
        break;
#endif
#ifdef EEXIST
    case EEXIST:
        s = (char *)"File exists";
        break;
#endif
#ifdef ENOTTY
    case ENOTTY:
        s = (char *)"Inappropriate ioctl for device";
        break;
#endif
#ifdef EPIPE
    case EPIPE:
        s = (char *)"Broken pipe";
        break;
#endif
#ifdef EINPROGRESS
    case EINPROGRESS:
        s = (char *)"Operation now in progress";
        break;
#endif
#ifdef EALREADY
    case EALREADY:
        s = (char *)"Operation already in progress";
        break;
#endif
#ifdef ENOTSOCK
    case ENOTSOCK:
        s = (char *)"Socket operation on non-socket";
        break;
#endif
#ifdef EDESTADDRREQ
    case EDESTADDRREQ:
        s = (char *)"Destination address required";
        break;
#endif
#ifdef EMSGSIZE
    case EMSGSIZE:
        s = (char *)"Message too long";
        break;
#endif
#ifdef EPROTOTYPE
    case EPROTOTYPE:
        s = (char *)"Protocol wrong type for socket";
        break;
#endif
#ifdef ENOPROTOOPT
    case ENOPROTOOPT:
        s = (char *)"Protocol not available";
        break;
#endif
#ifdef EPROTONOSUPPORT
    case EPROTONOSUPPORT:
        s = (char *)"Protocol not supported";
        break;
#endif
#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT:
        s = (char *)"Socket type not supported";
        break;
#endif
#ifdef EOPNOTSUPP
    case EOPNOTSUPP:
        s = (char *)"Operation not supported";
        break;
#endif
#ifdef EPFNOSUPPORT
    case EPFNOSUPPORT:
        s = (char *)"Protocol family not supported";
        break;
#endif
#ifdef EAFNOSUPPORT
    case EAFNOSUPPORT:
        s = (char *)"Address family not supported by protocol family";
        break;
#endif
#ifdef EADDRINUSE
    case EADDRINUSE:
        s = (char *)"Address already in use";
        break;
#endif
#ifdef EADDRNOTAVAIL
    case EADDRNOTAVAIL:
        s = (char *)"Can't assign requested address";
        break;
#endif
#ifdef ENETDOWN
    case ENETDOWN:
        s = (char *)"Network is down";
        break;
#endif
#ifdef ENETUNREACH
    case ENETUNREACH:
        s = (char *)"Network is unreachable";
        break;
#endif
#ifdef ENETRESET
    case ENETRESET:
        s = (char *)"Network dropped connection on reset";
        break;
#endif
#ifdef ECONNABORTED
    case ECONNABORTED:
        s = (char *)"Software caused connection abort";
        break;
#endif
#ifdef ECONNRESET
    case ECONNRESET:
        s = (char *)"Connection reset by peer";
        break;
#endif
#ifdef ENOBUFS
    case ENOBUFS:
        s = (char *)"No buffer space available";
        break;
#endif
#ifdef EISCONN
    case EISCONN:
        s = (char *)"Socket is already connected";
        break;
#endif
#ifdef ENOTCONN
    case ENOTCONN:
        s = (char *)"Socket is not connected";
        break;
#endif
#ifdef ESHUTDOWN
    case ESHUTDOWN:
        s = (char *)"Can't send after socket shutdown";
        break;
#endif
#ifdef ETOOMANYREFS
    case ETOOMANYREFS:
        s = (char *)"Too many references: can't splice";
        break;
#endif
#ifdef ETIMEDOUT
    case ETIMEDOUT:
        s = (char *)"Operation timed out";
        break;
#endif
#ifdef ECONNREFUSED
    case ECONNREFUSED:
        s = (char *)"Connection refused";
        break;
#endif
#ifdef EHOSTDOWN
    case EHOSTDOWN:
        s = (char *)"Host is down";
        break;
#endif
#ifdef EHOSTUNREACH
    case EHOSTUNREACH:
        s = (char *)"No route to host";
        break;
#endif

    default:
        s = (char *)"Unknown error";
        break;

    } // switch

    CYG_REPORT_RETVAL(s);

    return s;
} // __strerror()

// EOF strerror.cxx
