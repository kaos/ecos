//==========================================================================
//
//      devfs.cxx
//
//      Fileio device filesystem
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
// Purpose:             Fileio device filesystem
// Description:         This file implements a simple filesystem that interfaces
//                      to the existing device IO subsystem.
//                      
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

#include <cyg/io/devtab.h>              // device subsystem
#include <cyg/io/config_keys.h>         // CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN

//==========================================================================
// Forward definitions

// Filesystem operations
static int dev_mount    ( cyg_fstab_entry *fste, cyg_mtab_entry *mte );
static int dev_umount   ( cyg_mtab_entry *mte );
static int dev_open     ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          int mode,  cyg_file *fte );
static int dev_unlink   ( cyg_mtab_entry *mte, cyg_dir dir, const char *name );
static int dev_mkdir    ( cyg_mtab_entry *mte, cyg_dir dir, const char *name );
static int dev_rmdir    ( cyg_mtab_entry *mte, cyg_dir dir, const char *name );
static int dev_rename   ( cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                          cyg_dir dir2, const char *name2 );
static int dev_link     ( cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                          cyg_dir dir2, const char *name2, int type );
static int dev_opendir  ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          cyg_file *fte );
static int dev_chdir    ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          cyg_dir *dir_out );
static int dev_stat     ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          struct stat *buf);
static int dev_getinfo  ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          int key, void *buf, int len );
static int dev_setinfo  ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          int key, void *buf, int len );

// File operations
static int dev_fo_read      (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int dev_fo_write     (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int dev_fo_lseek     (struct CYG_FILE_TAG *fp, off_t *pos, int whence );
static int dev_fo_ioctl     (struct CYG_FILE_TAG *fp, CYG_ADDRWORD com,
                             CYG_ADDRWORD data);
static int dev_fo_select    (struct CYG_FILE_TAG *fp, int which, CYG_ADDRWORD info);
static int dev_fo_fsync     (struct CYG_FILE_TAG *fp, int mode );        
static int dev_fo_close     (struct CYG_FILE_TAG *fp);
static int dev_fo_fstat     (struct CYG_FILE_TAG *fp, struct stat *buf );
static int dev_fo_getinfo   (struct CYG_FILE_TAG *fp, int key, void *buf, int len );
static int dev_fo_setinfo   (struct CYG_FILE_TAG *fp, int key, void *buf, int len );


//==========================================================================
// Filesystem table entries

FSTAB_ENTRY( dev_fste, "devfs", 0,
             CYG_SYNCMODE_NONE,         // dev system has its own sync mechanism
             dev_mount,
             dev_umount,
             dev_open,
             dev_unlink,
             dev_mkdir,
             dev_rmdir,
             dev_rename,
             dev_link,
             dev_opendir,
             dev_chdir,
             dev_stat,
             dev_getinfo,
             dev_setinfo);

MTAB_ENTRY( dev_mte,
                   "/dev",
                   "devfs",
                   "",
                   0);

static cyg_fileops dev_fileops =
{
    dev_fo_read,
    dev_fo_write,
    dev_fo_lseek,
    dev_fo_ioctl,
    dev_fo_select,
    dev_fo_fsync,
    dev_fo_close,
    dev_fo_fstat,
    dev_fo_getinfo,
    dev_fo_setinfo
};

//==========================================================================
// Filesystem operations

// -------------------------------------------------------------------------

static int dev_mount    ( cyg_fstab_entry *fste, cyg_mtab_entry *mte )
{
    // Nothing to do here. The device IO subsystem has already initialized
    // iteslf, and the fileio infrastructure will do the rest.
    
    return 0;
}

// -------------------------------------------------------------------------

static int dev_umount   ( cyg_mtab_entry *mte )
{
    // Nothing to do here.
    
    return 0;
}

// -------------------------------------------------------------------------

static int dev_open     ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          int mode,  cyg_file *file )
{
    Cyg_ErrNo err;
    cyg_io_handle_t handle;

    // The name we are passed will point to the first character after
    // "/dev/". We know that the full string contains the prefix string,
    // so we back the pointer up by 5 chars.
    // A better approach would be for the device table entries to only
    // contain the part of the string after the prefix.

    name -= 5;
    
    err = cyg_io_lookup( name, &handle );

    if( err < 0 )
        return -err;

    // If we want non-blocking mode, configure the device for it.
    if( (mode & (O_NONBLOCK|O_RDONLY)) == (O_NONBLOCK|O_RDONLY) )
    {
        cyg_uint32 f = 0;
        cyg_uint32 fsize = sizeof(f);
        err = cyg_io_set_config( handle, CYG_IO_SET_CONFIG_READ_BLOCKING,
                                 (void *)&f, &fsize);
        if( err < 0 )
            return -err;
    }

    if( (mode & (O_NONBLOCK|O_WRONLY)) == (O_NONBLOCK|O_WRONLY) )
    {
        cyg_uint32 f = 0;
        cyg_uint32 fsize = sizeof(f);        
        err = cyg_io_set_config( handle, CYG_IO_SET_CONFIG_WRITE_BLOCKING,
                                 (void *)&f, &fsize);
        if( err < 0 )
            return -err;
    }
    
    // Initialize the file object
    
    file->f_flag |= mode & CYG_FILE_MODE_MASK;
    file->f_type = CYG_FILE_TYPE_FILE;
    file->f_ops = &dev_fileops;
    file->f_offset = 0;
    file->f_data = (CYG_ADDRWORD)handle;
    file->f_xops = 0;

    return 0;
}

// -------------------------------------------------------------------------

static int dev_unlink   ( cyg_mtab_entry *mte, cyg_dir dir, const char *name )
{
    return EROFS;
}

// -------------------------------------------------------------------------

static int dev_mkdir    ( cyg_mtab_entry *mte, cyg_dir dir, const char *name )
{
    return EROFS;
}

// -------------------------------------------------------------------------

static int dev_rmdir    ( cyg_mtab_entry *mte, cyg_dir dir, const char *name )
{
    return EROFS;
}

// -------------------------------------------------------------------------

static int dev_rename   ( cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                          cyg_dir dir2, const char *name2 )
{
    return EROFS;
}

// -------------------------------------------------------------------------

static int dev_link     ( cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                          cyg_dir dir2, const char *name2, int type )
{
    return EROFS;
}

// -------------------------------------------------------------------------

static int dev_opendir  ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          cyg_file *fte )
{
    return ENOTDIR;
}

// -------------------------------------------------------------------------

static int dev_chdir    ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          cyg_dir *dir_out )
{
    return ENOTDIR;
}

// -------------------------------------------------------------------------

static int dev_stat     ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          struct stat *buf)
{
    Cyg_ErrNo err;
    cyg_io_handle_t handle;

    name -= 5;          // See comment in dev_open()
    
    err = cyg_io_lookup( name, &handle );

    if( err < 0 )
        return -err;

    // Just fill in the stat buffer with some constant values.

    // FIXME: change this when block devices are available
    buf->st_mode = __stat_mode_CHR;     

    buf->st_ino         = (ino_t)handle;    // map dev handle to inode
    buf->st_dev         = (dev_t)handle;    // same with dev id
    buf->st_nlink       = 0;
    buf->st_uid         = 0;
    buf->st_gid         = 0;
    buf->st_size        = 0;
    buf->st_atime       = 0;
    buf->st_mtime       = 0;
    buf->st_ctime       = 0;
    
    return ENOERR;
}

// -------------------------------------------------------------------------

static int dev_getinfo  ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          int key, void *buf, int len )
{
    return ENOSYS;
}

// -------------------------------------------------------------------------

static int dev_setinfo  ( cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                          int key, void *buf, int len )
{
    return ENOSYS;
}

//==========================================================================
// File operations


// -------------------------------------------------------------------------

static int dev_fo_read      (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{
    Cyg_ErrNo err = 0;
    int i;

    // Now loop over the iovecs until they are all done, or
    // we get an error.
    for( i = 0; i < uio->uio_iovcnt; i++ )
    {
        cyg_iovec *iov = &uio->uio_iov[i];
        cyg_uint32 len = iov->iov_len;
    
        err = cyg_io_read( (cyg_io_handle_t)fp->f_data,
                           iov->iov_base,
                           &len);

        if( err < 0 ) break;

        uio->uio_resid -= len;
    }
    
    return -err;
}

// -------------------------------------------------------------------------

static int dev_fo_write     (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{
    Cyg_ErrNo err = 0;
    int i;

    // Now loop over the iovecs until they are all done, or
    // we get an error.
    for( i = 0; i < uio->uio_iovcnt; i++ )
    {
        cyg_iovec *iov = &uio->uio_iov[i];
        cyg_uint32 len = iov->iov_len;
    
        err = cyg_io_write( (cyg_io_handle_t)fp->f_data,
                            iov->iov_base,
                            &len);

        if( err < 0 ) break;

        uio->uio_resid -= len;
    }

    return -err;
}

// -------------------------------------------------------------------------

static int dev_fo_lseek     (struct CYG_FILE_TAG *fp, off_t *pos, int whence )
{
    // All current devices have no notion of position. Just return zero
    // as the new position.

    *pos = 0;
    
    return ENOERR;
}

// -------------------------------------------------------------------------

static int dev_fo_ioctl     (struct CYG_FILE_TAG *fp, CYG_ADDRWORD com,
                             CYG_ADDRWORD data)
{
    return ENOSYS;
}

// -------------------------------------------------------------------------

static cyg_bool dev_fo_select    (struct CYG_FILE_TAG *fp, int which, CYG_ADDRWORD info)
{
    return cyg_io_select( (cyg_io_handle_t)fp->f_data,
                          which,
                          info);
}

// -------------------------------------------------------------------------

static int dev_fo_fsync     (struct CYG_FILE_TAG *fp, int mode )
{
    Cyg_ErrNo err;

    err = cyg_io_get_config((cyg_io_handle_t)fp->f_data,
                            CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN,
                            NULL, NULL);

    return -err;
}

// -------------------------------------------------------------------------

static int dev_fo_close     (struct CYG_FILE_TAG *fp)
{
    return ENOERR;
}

// -------------------------------------------------------------------------

static int dev_fo_fstat     (struct CYG_FILE_TAG *fp, struct stat *buf )
{
    // Just fill in the stat buffer with some constant values.

    // FIXME: change this when block devices are available
    buf->st_mode = __stat_mode_CHR;     

    buf->st_ino         = (ino_t)fp->f_data;    // map dev handle to inode
    buf->st_dev         = (dev_t)fp->f_data;    // same with dev id
    buf->st_nlink       = 0;
    buf->st_uid         = 0;
    buf->st_gid         = 0;
    buf->st_size        = 0;
    buf->st_atime       = 0;
    buf->st_mtime       = 0;
    buf->st_ctime       = 0;
    
    return ENOERR;
}

// -------------------------------------------------------------------------

static int dev_fo_getinfo   (struct CYG_FILE_TAG *fp, int key, void *buf, int len )
{
    Cyg_ErrNo err = 0;
    cyg_uint32 ll = len;
    
    err = cyg_io_get_config( (cyg_io_handle_t)fp->f_data, key, buf, &ll );
    
    return -err;
}

// -------------------------------------------------------------------------

static int dev_fo_setinfo   (struct CYG_FILE_TAG *fp, int key, void *buf, int len )
{
    Cyg_ErrNo err = 0;
    cyg_uint32 ll = len;
    
    err = cyg_io_set_config( (cyg_io_handle_t)fp->f_data, key, buf, &ll );
    
    return -err;
}

// -------------------------------------------------------------------------
// EOF devfs.cxx
