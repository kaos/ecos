#ifndef CYGONCE_FIO_H
#define CYGONCE_FIO_H
//=============================================================================
//
//      fio.h
//
//      Fileio private header
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  nickg
// Date:          2000-05-25
// Purpose:       Fileio private header
// Description:   This file contains private definitions for communication
//                between the parts of the fileio package.
//              
// Usage:
//              #include "fio.h"
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>

#include <cyg/infra/cyg_type.h>

#include <stddef.h>             // NULL, size_t
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>

#include <cyg/fileio/fileio.h>
#include <cyg/fileio/sockio.h>

#include <errno.h>

#include <cyg/kernel/mutex.hxx>        // mutex definitions


//=============================================================================
// POSIX API support

#ifdef CYGPKG_POSIX

#include <cyg/posix/export.h>

#define CYG_FILEIO_FUNCTION_START() CYG_POSIX_FUNCTION_START()

#define CYG_FILEIO_FUNCTION_FINISH() CYG_POSIX_FUNCTION_FINISH()

#else

#define CYG_FILEIO_FUNCTION_START() CYG_EMPTY_STATEMENT

#define CYG_FILEIO_FUNCTION_FINISH() CYG_EMPTY_STATEMENT

#endif

//=============================================================================
// Fileio function entry and return macros.

// Handle entry to a fileio package function. 
#define FILEIO_ENTRY()                          \
CYG_MACRO_START                                 \
    CYG_REPORT_FUNCTYPE( "returning %d" );      \
    CYG_FILEIO_FUNCTION_START();                \
CYG_MACRO_END

// Do a fileio package defined return. This requires the error code
// to be placed in errno, and if it is non-zero, -1 returned as the
// result of the function. This also gives us a place to put any
// generic tidyup handling needed for things like signal delivery and
// cancellation.
#define FILEIO_RETURN(err)                      \
CYG_MACRO_START                                 \
    int __retval = 0;                           \
    CYG_FILEIO_FUNCTION_FINISH();               \
    if( err != 0 ) __retval = -1, errno = err;  \
    CYG_REPORT_RETVAL( __retval );              \
    return __retval;                            \
CYG_MACRO_END

#define FILEIO_RETURN_VALUE(val)                \
CYG_MACRO_START                                 \
    CYG_FILEIO_FUNCTION_FINISH();               \
    CYG_REPORT_RETVAL( val );                   \
    return val;                                 \
CYG_MACRO_END

#define FILEIO_RETURN_VOID()                    \
CYG_MACRO_START                                 \
    CYG_FILEIO_FUNCTION_FINISH();               \
    CYG_REPORT_RETURN();                        \
    return;                                     \
CYG_MACRO_END

//=============================================================================
// Cancellation support
// If the POSIX package is present we want to include cancellation points
// in the routines that are defined to contain them.
// The macro CYG_CANCELLATION_POINT does this.

#ifdef CYGPKG_POSIX

#include <pthread.h>

#define CYG_CANCELLATION_POINT pthread_testcancel()

#else

#define CYG_CANCELLATION_POINT CYG_EMPTY_STATEMENT

#endif

//=============================================================================
// Internal exports

//-----------------------------------------------------------------------------
// Exports from misc.cxx

// Current directory info
__externC cyg_mtab_entry *cdir_mtab_entry;
__externC cyg_dir cdir_dir;

__externC int cyg_mtab_lookup( cyg_dir *dir, const char **name, cyg_mtab_entry **mte);

__externC void cyg_fs_lock( cyg_mtab_entry *mte, cyg_uint32 syncmode );

__externC void cyg_fs_unlock( cyg_mtab_entry *mte, cyg_uint32 syncmode );

//-----------------------------------------------------------------------------
// Exports from fd.cxx

__externC void cyg_fd_init();

__externC cyg_file *cyg_file_alloc();

__externC void cyg_file_free(cyg_file * fp);

__externC int cyg_fd_alloc(int low);

__externC void cyg_fd_assign(int fd, cyg_file *fp);

__externC int cyg_fd_free(int fd);

__externC cyg_file *cyg_fp_get( int fd );

__externC void cyg_fp_free( cyg_file *fp );

__externC void cyg_file_lock( cyg_file *fp, cyg_uint32 syncmode );

__externC void cyg_file_unlock( cyg_file *fp, cyg_uint32 syncmode );

//-----------------------------------------------------------------------------
// Exports from socket.cxx

__externC void cyg_nstab_init();

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_FIO_H
// End of fio.h
