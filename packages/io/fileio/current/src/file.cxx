//==========================================================================
//
//      file.cxx
//
//      Fileio file operations
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
// Purpose:             Fileio file operations
// Description:         These are the functions that operate on files as a whole,
//                      such as open(), creat(), mkdir() etc.
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

//==========================================================================
// Implement filesystem locking protocol. 

#define LOCK_FS( _mte_ ) cyg_fs_lock( _mte_, (_mte_)->fs->syncmode)

#define UNLOCK_FS( _mte_ ) cyg_fs_unlock( _mte_, (_mte_)->fs->syncmode)

//==========================================================================
// Open a file

__externC int open( const char *path, int oflag, ... )
{
    FILEIO_ENTRY();

    CYG_CANCELLATION_POINT;

    int ret = 0;
    int fd;
    cyg_file *file;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = path;

    fd = cyg_fd_alloc(0);

    if( fd < 0 )
        FILEIO_RETURN(EMFILE);
    
    file = cyg_file_alloc();

    if( file == NULL )
    {
        cyg_fd_free(fd);
        FILEIO_RETURN(ENFILE);
    }
    
    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
    {
        cyg_fd_free(fd);
        cyg_file_free(file);
        FILEIO_RETURN(ENOENT);
    }

    LOCK_FS( mte );
    
    ret = mte->fs->open( mte, dir, name, oflag, file );
    
    UNLOCK_FS( mte );
    
    if( 0 != ret )
    {
        cyg_fd_free(fd);
        cyg_file_free(file);
        FILEIO_RETURN(ret);
    }

    file->f_mte = mte;
    file->f_syncmode = mte->fs->syncmode;
    
    cyg_fd_assign( fd, file );

    FILEIO_RETURN_VALUE(fd);
}

//==========================================================================
// create a file

__externC int creat( const char *path, mode_t mode )
{
    return open( path, O_WRONLY | O_CREAT | O_TRUNC, mode );
}


//==========================================================================
// Unlink/remove a file

__externC int unlink( const char *path )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = path;

    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    LOCK_FS( mte );
        
    ret = mte->fs->unlink( mte, dir, name );
    
    UNLOCK_FS( mte );
    
    FILEIO_RETURN(ret);
}

//==========================================================================
// Make a directory

__externC int mkdir( const char *path, mode_t mode )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = path;

    mode=mode;
    
    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    LOCK_FS( mte );
    
    ret = mte->fs->mkdir( mte, dir, name );
    
    UNLOCK_FS( mte );
    
    FILEIO_RETURN(ret);
}

//==========================================================================
// Remove a directory

__externC int rmdir( const char *path )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = path;

    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    LOCK_FS( mte );
    
    ret = mte->fs->rmdir( mte, dir, name );
    
    UNLOCK_FS( mte );
    
    FILEIO_RETURN(ret);
}

//==========================================================================
// Rename a file

__externC int rename( const char *path1, const char *path2 )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte1 = cdir_mtab_entry;
    cyg_mtab_entry *mte2 = cdir_mtab_entry;
    cyg_dir dir1 = cdir_dir;
    cyg_dir dir2 = cdir_dir;
    const char *name1 = path1;
    const char *name2 = path2;

    ret = cyg_mtab_lookup( &dir1, &name1, &mte1 );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    ret = cyg_mtab_lookup( &dir2, &name2, &mte2 );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    // Cannot rename between different filesystems
    if( mte1 != mte2 )
        FILEIO_RETURN(EXDEV);

    LOCK_FS( mte1 );
    
    ret = mte1->fs->rename( mte1, dir1, name1, dir2, name2 );

    UNLOCK_FS( mte1 );
    
    FILEIO_RETURN(ret);
}

//==========================================================================
// Create a link from an existing file (path1) to a new one (path2)

__externC int link( const char *path1, const char *path2 )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte1 = cdir_mtab_entry;
    cyg_mtab_entry *mte2 = cdir_mtab_entry;
    cyg_dir dir1 = cdir_dir;
    cyg_dir dir2 = cdir_dir;
    const char *name1 = path1;
    const char *name2 = path2;

    ret = cyg_mtab_lookup( &dir1, &name1, &mte1 );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    ret = cyg_mtab_lookup( &dir2, &name2, &mte2 );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    // Cannot hard-link between different filesystems
    if( mte1 != mte2 )
        FILEIO_RETURN(EXDEV);

    LOCK_FS( mte1 );
    
    ret = mte1->fs->link( mte1, dir1, name1, dir2, name2, CYG_FSLINK_HARD );

    UNLOCK_FS( mte1 );
    
    FILEIO_RETURN(ret);
}

//==========================================================================
// Change current directory

__externC int chdir( const char *path )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = path;

    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    LOCK_FS(mte);
    
    ret = mte->fs->chdir( mte, dir, name, &dir );

    UNLOCK_FS(mte);
    
    if( 0 != ret )
        FILEIO_RETURN(ret);

    if( cdir_mtab_entry != NULL )
    {
        // Now detach from current cdir. We call the current directory's
        // chdir routine with a NULL dir_out pointer.

        LOCK_FS(cdir_mtab_entry);

        ret = cdir_mtab_entry->fs->chdir( cdir_mtab_entry, cdir_dir, NULL, NULL );
    
        UNLOCK_FS(cdir_mtab_entry);

        // We really shouldn't get an error here.
        if( 0 != ret )
            FILEIO_RETURN(ret);
    }
    
    cdir_mtab_entry = mte;
    cdir_dir = dir;
    
    FILEIO_RETURN(ENOERR);
}

//==========================================================================
// Get file statistics

__externC int stat( const char *path, struct stat *buf )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = path;

    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    LOCK_FS( mte );
    
    ret = mte->fs->stat( mte, dir, name, buf );
    
    UNLOCK_FS( mte );
    
    FILEIO_RETURN(ret);
}

//==========================================================================
// Get file configurable pathname variables

__externC long pathconf( const char *path, int vname )
{
    FILEIO_ENTRY();
    
    int ret = 0;
    cyg_mtab_entry *mte = cdir_mtab_entry;
    cyg_dir dir = cdir_dir;
    const char *name = path;

    ret = cyg_mtab_lookup( &dir, &name, &mte );
    
    if( 0 != ret )
        FILEIO_RETURN(ENOENT);

    struct cyg_pathconf_info info;

    info.name = vname;
    info.value = 0;
        
    LOCK_FS( mte );
    
    ret = mte->fs->getinfo( mte, dir, name,
                            FS_INFO_CONF, (char *)&info, sizeof(info) );
    
    UNLOCK_FS( mte );
    
    if( 0 != ret )
        FILEIO_RETURN(ret);

    FILEIO_RETURN_VALUE(info.value);
}

//==========================================================================
// Access() function.
// This simply piggybacks onto stat().

extern int 	access(const char *path, int amode)
{
    FILEIO_ENTRY();

    int ret;
    struct stat buf;
    
    ret = stat( path, &buf );

    // Translate not found into EACCES if the F_OK bit is
    // set.
    if( (amode & F_OK) && (ret < 0) && (errno == ENOENT) )
        FILEIO_RETURN(EACCES);

    // All other errors go straight back to the user.
    if( ret < 0 )
        FILEIO_RETURN_VALUE(ret);

    // At present we do not have any access modes, so there is nothing
    // to test.  Just return success for all access modes.
    
    FILEIO_RETURN(ENOERR);
}

// -------------------------------------------------------------------------
// EOF file.cxx
