//==========================================================================
//
//      fd.cxx
//
//      Fileio file descriptor implementation
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-05-25
// Purpose:             Fileio file descriptor implementation
// Description:         This file contains the implementation of the file
//                      descriptor functions.
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include "fio.h"                       // Private header

#include <cyg/kernel/mutex.hxx>        // mutex definitions

//-----------------------------------------------------------------------------
// File data structures

// Mutex for controlling access to file desriptor arrays
Cyg_Mutex fdlock;

// Array of open file objects
static cyg_file file[CYGNUM_FILEIO_NFILE];

// Array of per-file mutexes
static Cyg_Mutex file_lock[CYGNUM_FILEIO_NFILE];

// Descriptor array
static cyg_file *desc[CYGNUM_FILEIO_NFD];

#define FD_ALLOCATED ((cyg_file *)1)

//==========================================================================
// Initialization

__externC void cyg_fd_init()
{
    int i;
    
    for( i = 0; i < CYGNUM_FILEIO_NFILE; i++ )
        file[i].f_flag = 0;

    for( i = 0; i < CYGNUM_FILEIO_NFD; i++ )
        desc[i] = NULL;
}

//==========================================================================
// File object allocation 

//--------------------------------------------------------------------------
// Locate and allocate a free file object.

__externC cyg_file *cyg_file_alloc()
{
    int i;
    cyg_file *fp = NULL;

    fdlock.lock();

    for( i = 0; i < CYGNUM_FILEIO_NFILE; i++ )
    {
        if( (file[i].f_flag & CYG_FALLOC) == 0 )
        {
            fp = &file[i];
            fp->f_flag = CYG_FALLOC;
            fp->f_ucount = 0;
            break;
        }
    }

    fdlock.unlock();

    return fp;
}

//--------------------------------------------------------------------------
// Free a file object. This is a straightforward freeing, usually used
// during error recovery. File objects are normally freed as a side
// effect of cyg_fd_assign() or cyg_fd_free().

__externC void cyg_file_free(cyg_file * fp)
{
    fdlock.lock();

    fp->f_flag = 0;
    
    fdlock.unlock();
}

//==========================================================================
// Internal routines for handling descriptor deallocation
// These must all be called with the fdlock already locked.

//--------------------------------------------------------------------------
// Decrement the use count on a file object and if it goes to zero,
// close the file and deallocate the file object.

static int fp_ucount_dec( cyg_file *fp )
{
    int error = 0;
    if( (--fp->f_ucount) <= 0 )
    {        
        cyg_file_lock( fp, fp->f_syncmode );
        
        error = fp->f_ops->fo_close(fp);

        cyg_file_unlock( fp, fp->f_syncmode );
            
        if( error == 0 )
            fp->f_flag = 0;
    }

    return error;
}

//--------------------------------------------------------------------------
// Clear out a descriptor. If this is the last reference to the file
// object, then that will be closed and deallocated.

static int fd_close( int fd )
{
    int error = 0;
    cyg_file *fp = desc[fd];

    if( fp != FD_ALLOCATED && fp != NULL)
    {
        // The descriptor is occupied, decrement its usecount and
        // close the file if it goes zero.

        error = fp_ucount_dec( fp );
    }

    desc[fd] = FD_ALLOCATED;

    return error;
}


//==========================================================================
// File descriptor allocation

//--------------------------------------------------------------------------
// Allocate a file descriptor. The allocated descriptor is set to the value
// FD_ALLOCATED to prevent it being reallocated by another thread.

__externC int cyg_fd_alloc(int low)
{
    int fd;

    fdlock.lock();
    
    for( fd = low; fd < CYGNUM_FILEIO_NFD; fd++ )
    {
        if( desc[fd] == NULL )
        {
            desc[fd] = FD_ALLOCATED;
            fdlock.unlock();
            return fd;
        }
    }

    fdlock.unlock();

    return -1;
}

//--------------------------------------------------------------------------
// Assign a file object to a descriptor. If the descriptor is already
// occupied, the occupying files usecount is decrement and it may be
// closed.

__externC void cyg_fd_assign(int fd, cyg_file *fp)
{
    fdlock.lock();

    fd_close( fd );

    fp->f_ucount++;
    desc[fd] = fp;

    fdlock.unlock();    
}

//--------------------------------------------------------------------------
// Free a descriptor. Any occupying files usecount is decremented and
// it may be closed.

__externC int cyg_fd_free(int fd)
{
    int error;
    
    fdlock.lock();
    
    error = fd_close( fd );

    desc[fd] = NULL;
    
    fdlock.unlock();

    return error;
}

//==========================================================================
// Descriptor to file object mapping


//--------------------------------------------------------------------------
// Map a descriptor to a file object. This is just a straightforward index
// into the descriptor array complicated by the need to lock the mutex and
// increment the usecount.

__externC cyg_file *cyg_fp_get( int fd )
{
    fdlock.lock();
    
    cyg_file *fp = desc[fd];

    if( fp != FD_ALLOCATED && fp != NULL)
    {
        // Increment use count while we work on this file
        
        fp->f_ucount++;
    }
    else fp = NULL;
    
    fdlock.unlock();

    return fp;
}

//--------------------------------------------------------------------------
// Free the usecount reference we acquired in cyg_fp_get(). If the usecount
// is zeroed, the file will be closed.

__externC void cyg_fp_free( cyg_file *fp )
{
    fdlock.lock();

    fp_ucount_dec( fp );
    
    fdlock.unlock();    
}

//==========================================================================
// File locking protocol

void cyg_file_lock( cyg_file *fp , cyg_uint32 syncmode )
{
    cyg_fs_lock( fp->f_mte, syncmode>>CYG_SYNCMODE_IO_SHIFT);

    if( syncmode & CYG_SYNCMODE_IO_FILE )
    {
        fp->f_flag |= CYG_FLOCKED;
        file_lock[fp-&file[0]].lock();
    }
}

void cyg_file_unlock( cyg_file *fp, cyg_uint32 syncmode )
{
    cyg_fs_unlock( fp->f_mte, syncmode>>CYG_SYNCMODE_IO_SHIFT);

    if( syncmode & CYG_SYNCMODE_IO_FILE )
    {
        fp->f_flag &= ~CYG_FLOCKED;
        file_lock[fp-&file[0]].unlock();
    }
}


//==========================================================================
// POSIX API routines

//--------------------------------------------------------------------------
// dup() - duplicate an FD into a random descriptor

__externC int dup( int fd )
{
    cyg_file *fp = cyg_fp_get( fd );

    if( fp == NULL )
    {
        errno = EBADF;
        return -1;
    }
    
    int fd2 = cyg_fd_alloc(0);

    if( fd2 == -1 )
    {
        errno = EMFILE;
        return -1;
    }

    cyg_fd_assign( fd2, fp );
    
    cyg_fp_free(fp);

    return fd2;
}

//--------------------------------------------------------------------------
// dup2() - duplicate an FD into a chosen descriptor

__externC int dup2( int fd, int fd2 )
{
    if( fd2 == fd ) return fd2;
        
    if( fd2 < 0 || fd2 >= OPEN_MAX )
    {
        errno = EBADF;
        return -1;
    }

    cyg_file *fp = cyg_fp_get( fd );

    if( fp == NULL )
    {
        errno = EBADF;
        return -1;
    }

    cyg_fd_assign( fd2, fp );
    
    cyg_fp_free(fp);

    return fd2;
}

// -------------------------------------------------------------------------
// EOF fd.cxx
