#ifndef CYGONCE_ERROR_CODES_H
#define CYGONCE_ERROR_CODES_H
//===========================================================================
//
//      codes.h
//
//      Common error code definitions
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
// Purpose:     To provide a common set of error codes
// Description: This provides a common set of error codes that all packages can
//              agree on. It doesn't preclude them defining their own error
//              return system, but this is a preferable system to use to help
//              error support be as general as possible.
//
//              We try and conform to the ANSI/POSIX error code format, namely
//              starting with the character 'E'
//
// Usage:       #include <cyg/error/codes.h>
//
//              Example:
//              
//              err=myfun();
//              if (err != ENOERR)
//              {
//                str=strerror(err);
//                printf("myfun returned error: %s\n", str);
//              }
//              else ....
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/error.h>          // Configuration header
#include <cyg/infra/cyg_type.h>     // externC

// Include the error package?
#ifdef CYGPKG_ERROR


// TYPE DEFINITIONS

// A type for error codes which may be useful to explain the purpose of
// a variable or return code. It shows that it contains an error code
// of the type defined below

typedef int Cyg_ErrNo;


// FUNCTION PROTOTYPES

// ANSI standard strerror() function as described by ANSI chap. 7.11.6.2.
// This is normally provided by <string.h>

externC char *
strerror( Cyg_ErrNo );

// prototype for the actual implementation. Equivalent to the above, but
// used internally by this product in preference

externC char *
_strerror( Cyg_ErrNo );


// CONSTANT DEFINITIONS

// If adding to this list, you must also update strerror() with its text
// If there is a common error of the same purpose on Unix, try and use its
// name and number. If not, use one above 200 to prevent future conflicts
//
// Do not use negative numbers, so that functions can return positive on
// success and -ESOMETHING on error, and it all works consistently.

#define ENOERR           0     // No error
#define EPERM            1     // Not permitted
#define ENOENT           2     // No such entity
#define ESRCH            3     // No such process
#define EINTR            4     // Operation interrupted
#define EIO              5     // I/O error
#define EBADF            9     // Bad file handle
#define EAGAIN           11    // Try again later
#define EWOULDBLOCK      EAGAIN
#define ENOMEM           12    // Out of memory
#define EBUSY            16    // Resource busy
#define ENODEV           19    // No such device
#define EINVAL           22    // Invalid argument
#define EMFILE           24    // Too many open files
#define EDOM             33    // Argument to math function outside valid
                               // domain
#define ERANGE           34    // Math result cannot be represented
#define ENOSYS           38    // Function not implemented

#define EEOF             200   // End of file reached
#define ENOSUPP          201   // Operation not supported
#define EDEVNOSUPP       202   // Device does not support this operation


#endif // ifdef CYGPKG_ERROR

#endif // CYGONCE_ERROR_CODES_H multiple inclusion protection

// EOF codes.h
