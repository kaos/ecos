//==========================================================================
//
//      fatfs.c
//
//      FAT file system
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Savin Zlobec
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           savin (based on ramfs.c)
// Original data:       nickg
// Date:                2003-06-29
// Purpose:             FAT file system
// Description:         This is a FAT filesystem for eCos. 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/fileio/fileio.h>
#include <cyg/io/io.h>
#include <blib/blib.h>

#include "fatfs.h"

//==========================================================================
// Tracing support defines 

#ifdef FATFS_TRACE_FS_OP
# define TFS 1
#else
# define TFS 0
#endif

#ifdef FATFS_TRACE_FILE_OP
# define TFO 1
#else
# define TFO 0
#endif

//==========================================================================
// Forward definitions

// Filesystem operations
static int fatfs_mount  (cyg_fstab_entry *fste, cyg_mtab_entry *mte);
static int fatfs_umount (cyg_mtab_entry *mte);
static int fatfs_open   (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int mode, cyg_file *fte);
static int fatfs_unlink (cyg_mtab_entry *mte, cyg_dir dir, const char *name);
static int fatfs_mkdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name);
static int fatfs_rmdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name);
static int fatfs_rename (cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                         cyg_dir dir2, const char *name2 );
static int fatfs_link   (cyg_mtab_entry *mte, cyg_dir dir1, const char *name1,
                         cyg_dir dir2, const char *name2, int type );
static int fatfs_opendir(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         cyg_file *fte );
static int fatfs_chdir  (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         cyg_dir *dir_out );
static int fatfs_stat   (cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         struct stat *buf);
static int fatfs_getinfo(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int key, void *buf, int len );
static int fatfs_setinfo(cyg_mtab_entry *mte, cyg_dir dir, const char *name,
                         int key, void *buf, int len );

// File operations
static int fatfs_fo_read   (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int fatfs_fo_write  (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int fatfs_fo_lseek  (struct CYG_FILE_TAG *fp, off_t *pos, int whence );
static int fatfs_fo_ioctl  (struct CYG_FILE_TAG *fp, CYG_ADDRWORD com,
                            CYG_ADDRWORD data);
static int fatfs_fo_fsync  (struct CYG_FILE_TAG *fp, int mode );        
static int fatfs_fo_close  (struct CYG_FILE_TAG *fp);
static int fatfs_fo_fstat  (struct CYG_FILE_TAG *fp, struct stat *buf );
static int fatfs_fo_getinfo(struct CYG_FILE_TAG *fp, int key, 
                            void *buf, int len );
static int fatfs_fo_setinfo(struct CYG_FILE_TAG *fp, int key, 
                            void *buf, int len );

// Directory operations
static int fatfs_fo_dirread (struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio);
static int fatfs_fo_dirlseek(struct CYG_FILE_TAG *fp, off_t *pos, int whence);

//==========================================================================
// Filesystem table entries

// -------------------------------------------------------------------------
// Fstab entry.

FSTAB_ENTRY(fatfs_fste, "fatfs", 0,
            CYG_SYNCMODE_FILE_FILESYSTEM|CYG_SYNCMODE_IO_FILESYSTEM,
            fatfs_mount,
            fatfs_umount,
            fatfs_open,
            fatfs_unlink,
            fatfs_mkdir,
            fatfs_rmdir,
            fatfs_rename,
            fatfs_link,
            fatfs_opendir,
            fatfs_chdir,
            fatfs_stat,
            fatfs_getinfo,
            fatfs_setinfo);

// -------------------------------------------------------------------------
// File operations.

static cyg_fileops fatfs_fileops =
{
    fatfs_fo_read,
    fatfs_fo_write,
    fatfs_fo_lseek,
    fatfs_fo_ioctl,
    cyg_fileio_seltrue,
    fatfs_fo_fsync,
    fatfs_fo_close,
    fatfs_fo_fstat,
    fatfs_fo_getinfo,
    fatfs_fo_setinfo
};

// -------------------------------------------------------------------------
// Directory file operations.

static cyg_fileops fatfs_dirops =
{
    fatfs_fo_dirread,
    (cyg_fileop_write *)cyg_fileio_enosys,
    fatfs_fo_dirlseek,
    (cyg_fileop_ioctl *)cyg_fileio_enosys,
    cyg_fileio_seltrue,
    (cyg_fileop_fsync *)cyg_fileio_enosys,
    fatfs_fo_close,
    (cyg_fileop_fstat *)cyg_fileio_enosys,
    (cyg_fileop_getinfo *)cyg_fileio_enosys,
    (cyg_fileop_setinfo *)cyg_fileio_enosys
};

// -------------------------------------------------------------------------
// Directory search data
// Parameters for a directory search. The fields of this structure are
// updated as we follow a pathname through the directory tree.

struct fatfs_dirsearch_t
{
    fatfs_disk_t        *disk;     // Disk info 
    fatfs_node_t        *dir;      // Directory to search
    const char          *path;     // Path to follow
    fatfs_node_t        *node;     // Node found
    const char          *name;     // Last name fragment used
    int                  namelen;  // Name fragment length
    cyg_bool             last;     // Last name in path?
};

typedef struct fatfs_dirsearch_t fatfs_dirsearch_t;

//==========================================================================

#if TFS
static void 
print_disk_info(fatfs_disk_t *disk)
{
    diag_printf("FAT: sector size:        %u\n", disk->sector_size);
    diag_printf("FAT: cluster size:       %u\n", disk->cluster_size);
    diag_printf("FAT: FAT table position: %u\n", disk->fat_tbl_pos);
    diag_printf("FAT: FAT table num ent:  %u\n", disk->fat_tbl_nents);
    diag_printf("FAT: FAT table size:     %u\n", disk->fat_tbl_size);
    diag_printf("FAT: FAT tables num:     %u\n", disk->fat_tbls_num);
    diag_printf("FAT: FAT root dir pos:   %u\n", disk->fat_root_dir_pos);
    diag_printf("FAT: FAT root dir nents: %u\n", disk->fat_root_dir_nents);
    diag_printf("FAT: FAT root dir size:  %u\n", disk->fat_root_dir_size);
    diag_printf("FAT: FAT data position:  %u\n", disk->fat_data_pos);
}
#endif

static void
init_dirsearch(fatfs_dirsearch_t *ds,
               fatfs_disk_t      *disk, 
               fatfs_node_t      *dir,
               const char        *name)
{
    ds->disk    = disk;
    if (NULL == dir)
        ds->dir = disk->root;
    else
        ds->dir = dir;
    ds->path    = name;
    ds->node    = ds->dir;
    ds->namelen = 0;
    ds->last    = false;
}

static int
find_direntry(fatfs_dirsearch_t *ds)
{
    int err;
    cyg_uint32 pos = 0;
    fatfs_node_t node_data;

    CYG_TRACE1(TFS, "Finding dir entry '%s'", ds->name);

    ds->node = fatfs_node_find(ds->disk, ds->name, 
                               ds->namelen, ds->dir->cluster);

    if (ds->node != NULL)
    {
        CYG_TRACE1(TFS, "Found dir entry '%s' in cache", ds->name);
        fatfs_node_touch(ds->disk, ds->node);
        return ENOERR;
    }
    
    while (true)
    {   
        err = fatfs_get_dir_entry_node(ds->disk, ds->dir, &pos, &node_data);
        if (err != ENOERR)
            return (err == EEOF ? ENOERR : err);

        if ('\0' == node_data.filename[ds->namelen] &&
            0 == strncasecmp(node_data.filename, ds->name, ds->namelen))
        {
            CYG_TRACE2(TFS, "Read dir entry '%s' at %d", 
                            node_data.filename, pos);

            ds->node = fatfs_node_alloc(ds->disk, &node_data);
            if (NULL == ds->node)
                return ENOMEM;
            return ENOERR;
        }
        pos++;
    }
}

static int 
find_entry(fatfs_dirsearch_t *ds)
{
    int err;
    const char *name = ds->path;
    const char *n = name;
    char namelen = 0;
    
    // check that we really have a directory
    if( !S_ISDIR(ds->dir->mode) )
    {
        CYG_TRACE1(TFS, "Entry '%s' not dir", ds->dir->filename);
        return ENOTDIR;
    }
    
    // Isolate the next element of the path name.
    while (*n != '\0' && *n != '/')
        n++, namelen++;

    // If we terminated on a NUL, set last flag.
    if (*n == '\0')
        ds->last = true;

    // update name in dirsearch object
    ds->name    = name;
    ds->namelen = namelen;

    err = find_direntry(ds);
    if (err != ENOERR)
        return err;

    CYG_TRACE2(TFS, "Entry '%s' %s", name, (ds->node ? "found" : "not found"));
    
    if (ds->node != NULL)
       return ENOERR;
    else
       return ENOENT; 
}

static int
fatfs_find(fatfs_dirsearch_t *ds)
{
    int err;

    CYG_TRACE1(TFS, "Find path='%s'", ds->path);
    
    // Short circuit empty paths
    if (*(ds->path) == '\0')
        return ENOERR;
    
    // Iterate down directory tree until we find the object
    // we want.
    for(;;)
    {
        err = find_entry(ds);
        
        if (err != ENOERR)
            return err;
        
        if (ds->last)
        {
            CYG_TRACE0(TFS, "Entry found");
            return ENOERR;
        }

        // Update dirsearch object to search next directory.
        ds->dir = ds->node;
        ds->path += ds->namelen;
        
        // Skip dirname separators
        if (*(ds->path) == '/') ds->path++;
        
        CYG_TRACE1(TFS, "Find path to go='%s'", ds->path);
    }
}

//==========================================================================
// Filesystem operations

// -------------------------------------------------------------------------
// fatfs_mount()
// Process a mount request. This mainly creates a root for the
// filesystem.

static int 
fatfs_mount(cyg_fstab_entry *fste, cyg_mtab_entry *mte)
{
    cyg_io_handle_t dev_h;
    Cyg_ErrNo err;
    fatfs_disk_t *disk;
    fatfs_node_t root_data;

    CYG_TRACE2(TFS, "Mount fste=%p mte=%p", fste, mte);
    
    CYG_TRACE1(TFS, "Looking up disk device '%s'", mte->devname);
    
    err = cyg_io_lookup(mte->devname, &dev_h);
    if (err != ENOERR)
        return err;

    disk = (fatfs_disk_t *)malloc(sizeof(fatfs_disk_t));
    if (NULL == disk)
        return ENOMEM;
        
    CYG_TRACE0(TFS, "Initializing FAT table cache"); 
   
    if (ENOERR != fatfs_tcache_create(disk, 
                      CYGNUM_FS_FAT_FAT_TABLE_CACHE_MEMSIZE))
    {
        free(disk);
        return ENOMEM;
    }
    
    CYG_TRACE0(TFS, "Initializing block cache"); 

    disk->bcache_mem = (cyg_uint8 *)malloc(CYGNUM_FS_FAT_BLOCK_CACHE_MEMSIZE);
    if (NULL == disk->bcache_mem)
    {
        fatfs_tcache_delete(disk);
        free(disk);
        return ENOMEM;
    }
    // FIXME: get block size from disk device
    err = cyg_blib_io_create(dev_h, disk->bcache_mem, 
            CYGNUM_FS_FAT_BLOCK_CACHE_MEMSIZE, 512, &disk->blib);
    if (err != ENOERR)
    {
        fatfs_tcache_delete(disk);
        free(disk->bcache_mem);
        free(disk);
        return err;
    }
    
    disk->dev_h = dev_h;

    CYG_TRACE0(TFS, "Initializing disk");
    
    err = fatfs_get_disk_info(disk);
    if (err != ENOERR)
    {
        fatfs_tcache_delete(disk);
        cyg_blib_delete(&disk->blib);
        free(disk->bcache_mem);
        free(disk);
        return err;
    }
   
#if TFS    
    print_disk_info(disk);
#endif

    CYG_TRACE0(TFS, "Initializing node cache");

    fatfs_node_cache_init(disk);
    
    CYG_TRACE0(TFS, "Initializing root node");
    
    fatfs_get_root_node(disk, &root_data);
    disk->root = fatfs_node_alloc(disk, &root_data);
    fatfs_node_ref(disk, disk->root);
    
    mte->root = (cyg_dir)disk->root;
    mte->data = (CYG_ADDRWORD)disk;

    CYG_TRACE0(TFS, "Disk mounted");

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_umount()
// Unmount the filesystem. This will currently only succeed if the
// filesystem is empty.

static int
fatfs_umount(cyg_mtab_entry *mte)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;
    fatfs_node_t *root = (fatfs_node_t *)mte->root;

    CYG_TRACE3(TFS, "Umount mte=%p %d live nodes %d dead nodes", 
                    mte, fatfs_get_live_node_count(disk), 
                    fatfs_get_dead_node_count(disk));

    if (root->refcnt > 1)
        return EBUSY;
    
    if (fatfs_get_live_node_count(disk) != 1)
        return EBUSY;

    fatfs_node_unref(disk, root);
    fatfs_node_cache_flush(disk);
    fatfs_tcache_delete(disk);
    // FIXME: cache delete can fail if cache can't be synced
    cyg_blib_delete(&disk->blib); 
    free(disk->bcache_mem);
    free(disk);
    
    mte->root = CYG_DIR_NULL;
    mte->data = (CYG_ADDRWORD)NULL;
    
    CYG_TRACE0(TFS, "Disk umounted");
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_open()
// Open a file for reading or writing.

static int 
fatfs_open(cyg_mtab_entry *mte,
           cyg_dir         dir, 
           const char     *name,
           int             mode,  
           cyg_file       *file)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;
    fatfs_node_t *node = NULL;
    fatfs_dirsearch_t ds;
    int err;

    CYG_TRACE5(TFS, "Open mte=%p dir=%p name='%s' mode=%d file=%p", 
                    mte, dir, name, mode, file);

    init_dirsearch(&ds, disk, (fatfs_node_t *)dir, name);

    err = fatfs_find(&ds);

    if (err == ENOENT)
    {
        if (ds.last && (mode & O_CREAT))
        {
            fatfs_node_t node_data;
            
            // No node there, if the O_CREAT bit is set then we must
            // create a new one. The dir and name fields of the dirsearch
            // object will have been updated so we know where to put it.

            CYG_TRACE1(TFS, "Creating new file '%s'", name); 

            err = fatfs_create_file(disk, ds.dir, ds.name, 
                                    ds.namelen, &node_data);
            if (err != ENOERR)
                return err;

            node = fatfs_node_alloc(disk, &node_data);
            if (NULL == node)
                return ENOMEM;
           
            // Update directory times
            ds.dir->atime =
            ds.dir->mtime = cyg_timestamp();
            
            err = ENOERR;
        }
    }
    else if (err == ENOERR)
    {
        // The node exists. If the O_CREAT and O_EXCL bits are set, we
        // must fail the open.

        if ((mode & (O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL))
            err = EEXIST;
        else
            node = ds.node;
    }

    if (err == ENOERR && (mode & O_TRUNC))
    {
        // If the O_TRUNC bit is set we must clean out the file data.
        CYG_TRACE0(TFS, "Truncating file"); 
        fatfs_trunc_file(disk, node);
    }

    if (err != ENOERR)
        return err;
    
    // Check that we actually have a file here
    if (S_ISDIR(node->mode))
        return EISDIR;

    // Make a reference to this file node     
    fatfs_node_ref(disk, node);

    // Initialize the file object

    file->f_flag   |= mode & CYG_FILE_MODE_MASK;
    file->f_type    = CYG_FILE_TYPE_FILE;
    file->f_ops     = &fatfs_fileops;
    file->f_offset  = (mode & O_APPEND) ? node->size : 0;
    file->f_data    = (CYG_ADDRWORD)node;
    file->f_xops    = 0;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_unlink()
// Remove a file link from its directory.

static int 
fatfs_unlink(cyg_mtab_entry *mte, 
             cyg_dir         dir, 
             const char     *name)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;
    fatfs_dirsearch_t ds;
    int err;

    CYG_TRACE3(TFS, "Unlink mte=%p dir=%p name='%s'", mte, dir, name);

    init_dirsearch(&ds, disk, (fatfs_node_t *)dir, name);

    err = fatfs_find(&ds);

    if (err != ENOERR)
        return err;
  
    if (ds.node->refcnt > 0)
        return EBUSY;
    
    // Delete node
    err = fatfs_delete_file(disk, ds.node);
    if (err == ENOERR)
        fatfs_node_free(disk, ds.node);
    
    return err;
}

// -------------------------------------------------------------------------
// fatfs_mkdir()
// Create a new directory.

static int
fatfs_mkdir(cyg_mtab_entry *mte, cyg_dir dir, const char *name)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;
    fatfs_dirsearch_t ds;
    int err;
    
    CYG_TRACE3(TFS, "Mkdir mte=%p dir=%p name='%s'", mte, dir, name);

    init_dirsearch(&ds, disk, (fatfs_node_t *)dir, name);

    err = fatfs_find(&ds);

    if (err == ENOENT)
    {
        if (ds.last)
        {
            fatfs_node_t node_data;
            
            // The entry does not exist, and it is the last element in
            // the pathname, so we can create it here.

            err = fatfs_create_dir(disk, ds.dir, ds.name, 
                                   ds.namelen, &node_data);
            if (err != ENOERR)
                return err;
            
            fatfs_node_alloc(disk, &node_data);

            return ENOERR;
        }
    } 
    else if (err == ENOERR)
    {
        return EEXIST;
    }
    
    return err;
}

// -------------------------------------------------------------------------
// fatfs_rmdir()
// Remove a directory.

static int 
fatfs_rmdir(cyg_mtab_entry *mte, cyg_dir dir, const char *name)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;
    fatfs_dirsearch_t ds;
    int err;

    CYG_TRACE3(TFS, "Rmdir mte=%p dir=%p name='%s'", mte, dir, name);

    init_dirsearch(&ds, disk, (fatfs_node_t *)dir, name);

    err = fatfs_find(&ds);

    if (err != ENOERR)
        return err;
  
    // Check that this is actually a directory.
    if (!S_ISDIR(ds.node->mode))
        return EPERM;
 
    if (ds.node->refcnt > 0)
        return EBUSY;
    
    err = fatfs_delete_file(disk, ds.node);
    if (err == ENOERR)
        fatfs_node_free(disk, ds.node);
    
    return err;
}

// -------------------------------------------------------------------------
// fatfs_rename()
// Rename a file/dir.

static int 
fatfs_rename(cyg_mtab_entry *mte, 
             cyg_dir         dir1, 
             const char     *name1,
             cyg_dir         dir2, 
             const char     *name2)
{
    fatfs_disk_t *disk  = (fatfs_disk_t *)mte->data;
    fatfs_dirsearch_t ds1, ds2;
    int err;
 
    CYG_TRACE5(TFS, "Rename mte=%p dir1=%p name1='%s' dir2=%p name2='%s'", 
                    mte, dir1, name1, dir2, name2);

    init_dirsearch(&ds1, disk, (fatfs_node_t *)dir1, name1);

    err = fatfs_find(&ds1);
    if (err != ENOERR)
        return err;

    // Protect the found nodes from being reused by the
    // following search
    fatfs_node_ref(disk, ds1.dir);
    fatfs_node_ref(disk, ds1.node);
    
    init_dirsearch(&ds2, disk, (fatfs_node_t *)dir2, name2);
   
    err = fatfs_find(&ds2);

    if (err == ENOERR && ds2.last)
    {
        err = EEXIST;  
        goto out;  
    }
    
    if (err == ENOENT && !ds2.last)
        goto out;
    
    if (ds1.node == ds2.node)
    {
        err = ENOERR;
        goto out;
    }
    
    err = fatfs_rename_file(disk, ds1.dir, ds1.node, ds2.dir, 
                            ds2.name, ds2.namelen);  
    fatfs_node_rehash(disk, ds1.node);

out:
    fatfs_node_unref(disk, ds1.dir);
    fatfs_node_unref(disk, ds1.node);
   
    if (err == ENOERR)
    {
        ds1.dir->atime =
        ds1.dir->mtime = 
        ds2.dir->atime = 
        ds2.dir->mtime = cyg_timestamp();    
    } 
    return err;
}

// -------------------------------------------------------------------------
// fatfs_link()
// Make a new directory entry for a file.

static int 
fatfs_link(cyg_mtab_entry *mte, 
           cyg_dir         dir1, 
           const char     *name1,
           cyg_dir         dir2, 
           const char     *name2, 
           int             type)
{
    CYG_TRACE6(TFS, "Link mte=%p dir1=%p name1='%s' dir2=%p name2='%s' type=%d",
                    mte, dir1, name1, dir2, name2, type);

    // Linking not supported
    return EINVAL;
}

// -------------------------------------------------------------------------
// fatfs_opendir()
// Open a directory for reading.

static int
fatfs_opendir(cyg_mtab_entry *mte,
              cyg_dir         dir,
              const char     *name,
              cyg_file       *file)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;
    fatfs_dirsearch_t ds;
    int err;

    CYG_TRACE4(TFS, "Opendir mte=%p dir=%p name='%s' file=%p", 
                    mte, dir, name, file);

    init_dirsearch(&ds, disk, (fatfs_node_t *)dir, name);

    err = fatfs_find(&ds);

    if (err != ENOERR)
        return err;

    // Check it is really a directory.
    if (!S_ISDIR(ds.node->mode)) 
        return ENOTDIR;
  
    // Make a reference to this dir node
    fatfs_node_ref(disk, ds.node);
    
    // Initialize the file object
    file->f_type        = CYG_FILE_TYPE_FILE;
    file->f_ops         = &fatfs_dirops;
    file->f_data        = (CYG_ADDRWORD)ds.node;
    file->f_xops        = 0;
    file->f_offset      = 0;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_chdir()
// Change directory support.

static int
fatfs_chdir(cyg_mtab_entry *mte,
            cyg_dir         dir,
            const char     *name,
            cyg_dir        *dir_out)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;

    CYG_TRACE4(TFS, "Chdir mte=%p dir=%p dir_out=%p name=%d", 
                    mte, dir, dir_out, name);

    if (dir_out != NULL)
    {
        // This is a request to get a new directory pointer in
        // *dir_out.
        
        fatfs_dirsearch_t ds;
        int err;

        init_dirsearch(&ds, disk, (fatfs_node_t *)dir, name);

        err = fatfs_find(&ds);

        if (err != ENOERR)
            return err;

        // Check it is a directory
        if (!S_ISDIR(ds.node->mode))
            return ENOTDIR;

        if (ds.node != disk->root)
            fatfs_node_ref(disk, ds.node); 
        *dir_out = (cyg_dir)ds.node;
    }
    else
    {
        // If no output dir is required, this means that the mte and
        // dir arguments are the current cdir setting and we should
        // forget this fact.

        fatfs_node_t *node = (fatfs_node_t *)dir;

        if (node != disk->root)
            fatfs_node_unref(disk, node);
    }

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_stat()
// Get struct stat info for named object.

static int
fatfs_stat(cyg_mtab_entry *mte,
           cyg_dir         dir,
           const char     *name,
           struct stat    *buf)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)mte->data;
    fatfs_dirsearch_t ds;
    int err;

    CYG_TRACE4(TFS, "Stat mte=%p dir=%p name='%s' buf=%p", 
                    mte, dir, name, buf);

    init_dirsearch(&ds, disk, (fatfs_node_t *)dir, name);

    err = fatfs_find(&ds);

    if (err != ENOERR)
        return err;

    // Fill in the status
    buf->st_mode        = ds.node->mode;
    buf->st_ino         = (ino_t)ds.node->cluster;
    buf->st_dev         = 0;
    buf->st_nlink       = 1;
    buf->st_uid         = 0;
    buf->st_gid         = 0;
    buf->st_size        = ds.node->size;
    buf->st_atime       = ds.node->atime;
    buf->st_mtime       = ds.node->mtime;
    buf->st_ctime       = ds.node->ctime;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_getinfo()
// Getinfo. Nothing to support here at present.

static int 
fatfs_getinfo(cyg_mtab_entry *mte, 
              cyg_dir         dir, 
              const char     *name,
              int             key, 
              void           *buf, 
              int             len)
{
    CYG_TRACE6(TFS, "Getinfo mte=%p dir=%p name='%s' key=%d buf=%p len=%d",
                    mte, dir, name, key, buf, len);
    return EINVAL;
}

// -------------------------------------------------------------------------
// fatfs_setinfo()
// Setinfo. Nothing to support here at present.

static int 
fatfs_setinfo(cyg_mtab_entry *mte, 
              cyg_dir         dir, 
              const char     *name,
              int             key, 
              void           *buf, 
              int             len)
{
    CYG_TRACE6(TFS, "Getinfo mte=%p dir=%p name='%s' key=%d buf=%p len=%d",
                    mte, dir, name, key, buf, len);
    return EINVAL;
}

//==========================================================================
// File operations

// -------------------------------------------------------------------------
// fatfs_fo_read()
// Read data from the file.

static int 
fatfs_fo_read(struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)fp->f_mte->data;
    fatfs_node_t *node = (fatfs_node_t *)fp->f_data;
    cyg_uint32 pos = fp->f_offset;
    ssize_t resid = uio->uio_resid;
    int i;

    CYG_TRACE3(TFO, "Read fp=%p uio=%p pos=%d", fp, uio, pos);

    // Loop over the io vectors until there are none left
    for (i = 0; i < uio->uio_iovcnt; i++)
    {
        cyg_iovec *iov = &uio->uio_iov[i];
        char *buf = (char *)iov->iov_base;
        off_t len = iov->iov_len;

        // Loop over each vector filling it with data from the file.
        while (len > 0 && pos < node->size)
        {
            cyg_uint32 l = len;
            int err;

            // Adjust size to end of file if necessary
            if (l > node->size-pos)
                l = node->size-pos;

            // Read data
            err = fatfs_read_data(disk, node, buf, &l, pos);

            if (err != ENOERR)
                return err;

            // Update working vars
            len   -= l;
            buf   += l;
            pos   += l;
            resid -= l;
        }
    }

    // We successfully read some data,
    // update the access time, file offset and transfer residue.
    node->atime    = cyg_timestamp(); 
    uio->uio_resid = resid;
    fp->f_offset   = (off_t)pos;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_fo_write()
// Write data to file.

static int 
fatfs_fo_write(struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)fp->f_mte->data;
    fatfs_node_t *node = (fatfs_node_t *)fp->f_data;
    cyg_uint32 pos = fp->f_offset;
    ssize_t resid = uio->uio_resid;
    int err = ENOERR;
    int i;

    CYG_TRACE3(TFO, "Write fp=%p uio=%p pos=%d", fp, uio, pos);
    
    // If the APPEND mode bit was supplied, force all writes to
    // the end of the file.
    if (fp->f_flag & CYG_FAPPEND)
        pos = fp->f_offset = node->size;

    // Check that pos is within current file size, or at the very end.
    if (pos < 0 || pos > node->size)
        return EINVAL;
   
    // Now loop over the iovecs until they are all done, or
    // we get an error.
    for (i = 0; i < uio->uio_iovcnt; i++)
    {
        cyg_iovec *iov = &uio->uio_iov[i];
        char *buf = (char *)iov->iov_base;
        off_t len = iov->iov_len;
 
        // Loop over the vector writing it to the file until it has
        // all been done.
        while (len > 0)
        {
            cyg_uint32 l = len;

            // Write data
            err = fatfs_write_data(disk, node, buf, &l, pos);

            // Update working vars
            len   -= l;
            buf   += l;
            pos   += l;
            resid -= l;

            // Stop writing if there is no more space in the file and
            // indicate end of data.
            if (err == ENOSPC)
                break;
            
            if (err != ENOERR)
                return err;
        }
    }        

    // We wrote some data successfully, update the modified and access
    // times of the node, increase its size appropriately, and update
    // the file offset and transfer residue.
    node->mtime = 
    node->atime = cyg_timestamp();
    if (pos > node->size)
        node->size = pos;

    uio->uio_resid = resid;
    fp->f_offset = (off_t)pos;

    return err;
}

// -------------------------------------------------------------------------
// fatfs_fo_lseek()
// Seek to a new file position.

static int 
fatfs_fo_lseek(struct CYG_FILE_TAG *fp, off_t *apos, int whence)
{
    fatfs_node_t *node = (fatfs_node_t *)fp->f_data;
    off_t pos = *apos;

    CYG_TRACE3(TFO, "Lseek fp=%p pos=%d whence=%d", fp, fp->f_offset, whence);

    switch (whence)
    {
        case SEEK_SET:
            // Pos is already where we want to be.
            break;
         case SEEK_CUR:
            // Add pos to current offset.
            pos += fp->f_offset;
            break;
         case SEEK_END:
            // Add pos to file size.
            pos += node->size;
            break;
         default:
            return EINVAL;
    }

    // Check that pos is still within current file size, or at the
    // very end.
    if (pos < 0 || pos > node->size)
        return EINVAL;
  
    // All OK, set fp offset and return new position.
    *apos = fp->f_offset = pos;

    CYG_TRACE2(TFO, "Lseek fp=%p new pos=%d", fp, *apos);

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_fo_ioctl()
// Handle ioctls. Currently none are defined.

static int 
fatfs_fo_ioctl(struct CYG_FILE_TAG *fp, CYG_ADDRWORD com, CYG_ADDRWORD data)
{
    CYG_TRACE3(TFO, "Ioctl fp=%p com=%x data=%x", fp, com, data);
    return EINVAL;
}

// -------------------------------------------------------------------------
// fatfs_fo_fsync().
// Force the file out to data storage.

static int 
fatfs_fo_fsync(struct CYG_FILE_TAG *fp, int mode)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)fp->f_mte->data;
    fatfs_node_t *node = (fatfs_node_t *)fp->f_data;
    int err;
    
    CYG_TRACE2(TFO, "Fsync fp=%p mode=%d", fp, mode);

    err = fatfs_write_file_attr(disk, node);
    if (ENOERR == err)
        err = cyg_blib_sync(&disk->blib);
    return err;
}

// -------------------------------------------------------------------------
// fatfs_fo_close()
// Close a file.

static int
fatfs_fo_close(struct CYG_FILE_TAG *fp)
{    
    fatfs_disk_t *disk = (fatfs_disk_t *)fp->f_mte->data;
    fatfs_node_t *node = (fatfs_node_t *)fp->f_data;
    int err = ENOERR;

    CYG_TRACE1(TFO, "Close fp=%p", fp);

    if (node != disk->root)
        err = fatfs_write_file_attr(disk, node);
    fatfs_node_unref(disk, node);    

    return err;
}

// -------------------------------------------------------------------------
// fatfs_fo_fstat()
// Get file status.

static int
fatfs_fo_fstat(struct CYG_FILE_TAG *fp, struct stat *buf)
{
    fatfs_node_t *node = (fatfs_node_t *)fp->f_data;
    
    CYG_TRACE2(TFO, "Fstat fp=%p buf=%p", fp, buf);

    // Fill in the status
    buf->st_mode   = node->mode;
    buf->st_ino    = (ino_t)node->cluster;
    buf->st_dev    = 0;
    buf->st_nlink  = 1;
    buf->st_uid    = 0;
    buf->st_gid    = 0;
    buf->st_size   = node->size;
    buf->st_atime  = node->atime;
    buf->st_mtime  = node->mtime;
    buf->st_ctime  = node->ctime;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_fo_getinfo()
// Get info.

static int
fatfs_fo_getinfo(struct CYG_FILE_TAG *fp, int key, void *buf, int len)
{
    CYG_TRACE4(TFO, "Getinfo fp=%p key=%d buf=%p len=%d", fp, key, buf, len);
    return EINVAL;
}

// -------------------------------------------------------------------------
// fatfs_fo_setinfo()
// Set info.

static int
fatfs_fo_setinfo(struct CYG_FILE_TAG *fp, int key, void *buf, int len)
{
    CYG_TRACE4(TFO, "Setinfo fp=%p key=%d buf=%p len=%d", fp, key, buf, len);
    return EINVAL;
}

//==========================================================================
// Directory operations

// -------------------------------------------------------------------------
// fatfs_fo_dirread()
// Read a single directory entry from a file.

static int 
fatfs_fo_dirread(struct CYG_FILE_TAG *fp, struct CYG_UIO_TAG *uio)
{
    fatfs_disk_t *disk = (fatfs_disk_t *)fp->f_mte->data;
    fatfs_node_t *dir  = (fatfs_node_t *)fp->f_data;
    struct dirent *ent = (struct dirent *)uio->uio_iov[0].iov_base;
    cyg_uint32 pos     = fp->f_offset;
    char *nbuf         = ent->d_name;
    off_t len          = uio->uio_iov[0].iov_len;
    fatfs_node_t node;
    int err;
    
    CYG_TRACE3(TFO, "Dirread fp=%p uio=%p pos=%d", fp, uio, pos);

    if (len < sizeof(struct dirent))
        return EINVAL;

    err = fatfs_get_dir_entry_node(disk, dir, &pos, &node);

    if (err != ENOERR)
        return (err == EEOF ? ENOERR : err);

    strcpy(nbuf, node.filename);

    dir->atime      = cyg_timestamp();
    uio->uio_resid -= sizeof(struct dirent);
    fp->f_offset    = (off_t)(pos + 1);
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_fo_dirlseek()
// Seek directory to start.

static int 
fatfs_fo_dirlseek(struct CYG_FILE_TAG *fp, off_t *pos, int whence)
{
    CYG_TRACE2(TFO, "Dirlseek fp=%p whence=%d", fp, whence);

    // Only allow SEEK_SET to zero
    
    if (whence != SEEK_SET || *pos != 0)
        return EINVAL;
    
    *pos = fp->f_offset = 0;
    return ENOERR;
}

// -------------------------------------------------------------------------
// EOF fatfs.c
