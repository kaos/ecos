//==========================================================================
//
//      fileio.c
//
//      RedBoot fileio support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: dwmw2, msalter
// Date:         2004-11-21
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

// Shoot me. But I don't want struct timeval because redboot provides it.
#define _POSIX_SOURCE
#include <time.h>
#undef _POSIX_SOURCE

#include <redboot.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#ifdef CYGPKG_IO_FLASH
#include <pkgconf/io_flash.h>
#include <cyg/io/io.h>
#include <cyg/io/flash.h>
#include <cyg/io/config_keys.h>
#endif
#include <cyg/io/devtab.h>
#include <cyg/fileio/fileio.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros

//==========================================================================

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __FS_cmds_TAB__, FS_cmds);
CYG_HAL_TABLE_END( __FS_cmds_TAB_END__, FS_cmds);

extern struct cmd __FS_cmds_TAB__[], __FS_cmds_TAB_END__;

//==========================================================================

static void
fs_usage(char *why)
{
    diag_printf("*** invalid 'fs' command: %s\n", why);
    cmd_usage(__FS_cmds_TAB__, &__FS_cmds_TAB_END__, "fs ");
}


//==========================================================================

#define MAX_MOUNTS      4

static int mount_count = 0;

static struct
{
    char dev_str[PATH_MAX];
    char mp_str[PATH_MAX];
    char type_str[PATH_MAX];
} mounts[MAX_MOUNTS];

//==========================================================================

static void do_mount(int argc, char *argv[]);
static void do_umount(int argc, char *argv[]);

/* Temporary hack until flashv2 merged to trunk. We can't tell whether we're
 * working with flash v1 or v2 from the package version. So if legacy device isn't
 * defined we check whether, if there is a block device, there's a tell-tale define
 * that only exists with the v1 version.
 */
#if !defined(CYGPKG_IO_FLASH_BLOCK_DEVICE_LEGACY) && \
     defined(CYGPKG_IO_FLASH_BLOCK_DEVICE) && \
     defined(CYGINT_IO_FLASH_BLOCK_CFG_1)
# define CYGPKG_IO_FLASH_BLOCK_DEVICE_LEGACY 1
#endif

#ifdef CYGPKG_IO_FLASH_BLOCK_DEVICE_LEGACY
#define FLASHPART "[-f <partition>] "
#else
#define FLASHPART
#endif

local_cmd_entry("mount", 
                "Mount file system",
                FLASHPART "[-d <device>] -t <fstype> [<mountpoint>]",
                do_mount,
                FS_cmds
    );
local_cmd_entry("umount", 
                "Unmount file system",
                "<mountpoint>",
                do_umount,
                FS_cmds
    );

//==========================================================================

// Mount disk/filesystem
static void
do_mount(int argc, char *argv[])
{
    char *dev_str = "<undefined>", *type_str, *mp_str;
    bool dev_set = false, type_set = false;
    struct option_info opts[3];
    int err, num_opts = 2;
    int i,m=0; /* Set to 0 to silence warning */
#ifdef CYGPKG_IO_FLASH_BLOCK_DEVICE_LEGACY
    char *part_str;
    bool part_set = false;
#endif

    init_opts(&opts[0], 'd', true, OPTION_ARG_TYPE_STR,
              (void *)&dev_str, &dev_set, "device");
    init_opts(&opts[1], 't', true, OPTION_ARG_TYPE_STR,
              (void *)&type_str, &type_set, "fstype");
#ifdef CYGPKG_IO_FLASH_BLOCK_DEVICE_LEGACY
    init_opts(&opts[2], 'f', true, OPTION_ARG_TYPE_STR,
              (void *)&part_str, &part_set, "partition");
    num_opts++;
#endif

    CYG_ASSERT(num_opts <= NUM_ELEMS(opts), "Too many options");

    if (!scan_opts(argc, argv, 1, opts, num_opts, &mp_str, OPTION_ARG_TYPE_STR, "mountpoint"))
        return;

    if (!type_set) {
        err_printf("fs mount: Must specify file system type\n");
        return;
    }

    if( mp_str == 0 )
        mp_str = "/";

    if( mount_count >= MAX_MOUNTS )
    {
        err_printf("fs mount: Maximum number of mounts exceeded\n");
        return;
    }
    
#ifdef CYGPKG_IO_FLASH_BLOCK_DEVICE_LEGACY
    if (part_set) {
        cyg_uint32 len;
        cyg_io_handle_t h;

        if (dev_set && strcmp(dev_str, CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1)) {
            err_printf("fs mount: May only set one of <device> or <partition>\n");
            return;
        }

        dev_str = CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1;
        len = (cyg_uint32)strlen(part_str);

        err = cyg_io_lookup(dev_str, &h);
        if (err < 0) {
            err_printf("fs mount: cyg_io_lookup of \"%s\" returned %d\n", dev_str, err);
            return;
        }
        err = cyg_io_set_config(h, CYG_IO_SET_CONFIG_FLASH_FIS_NAME,
                                part_str, &len);
        if (err < 0) {
            diag_printf("fs mount: FIS partition \"%s\" not found\n",
                        part_str);
            return;
        }
    }
#endif

    for( i = 0; i < MAX_MOUNTS; i++ )
    {
        if( mounts[i].mp_str[0] != '\0' )
        {
            if( strcmp(mounts[i].dev_str, dev_str ) == 0 )
            {
                err_printf("fs mount: Device %s already mounted\n",dev_str);
                return;
            }
        }
        else
            m = i;
    }

    strcpy( mounts[m].mp_str, mp_str );
    strcpy( mounts[m].dev_str, dev_str );
    strcpy( mounts[m].type_str, type_str );
    
    err = mount(mounts[m].dev_str, mounts[m].mp_str, mounts[m].type_str);

    if (err)
    {
        err_printf("fs mount: mount(%s,%s,%s) failed %d\n", dev_str, mp_str, type_str, errno);
        mounts[m].mp_str[0] = '\0'; // mount failed so don't let it appear mounted
    }
    else
    {
        if( mount_count == 0 )
            chdir( "/" );
        mount_count++;
    }
}

//==========================================================================

static void
do_umount(int argc, char *argv[])
{
    char *dir_str;
    int err;
    int i;
    
     if( mount_count == 0 )
     {
         err_printf("fs: No filesystems mounted\n");
         return;
     }
     
    if (!scan_opts(argc, argv, 1, NULL, 0, &dir_str, OPTION_ARG_TYPE_STR, "mountpoint"))
        return;

    if( dir_str == 0 )
        dir_str = "/";

    for( i = 0; i < MAX_MOUNTS; i++ )
    {
        if( strcmp(mounts[i].mp_str, dir_str ) == 0 )
            break;
    }

    if( i == MAX_MOUNTS )
    {
        err_printf("fs unmount: unknown mountpoint %s\n",dir_str);
        return;
    }
    
    err = umount (dir_str);

    if (err)
        err_printf("fs umount: unmount failed %d\n", errno);
    else
    {
        mounts[i].mp_str[0] = '\0';
        mount_count--;
        if( mount_count == 0 )
            chdir( "/" );
    }
    
}

//==========================================================================

#include <dirent.h>

static char rwx[8][4] = { "---", "r--", "-w-", "rw-", "--x", "r-x", "-wx", "rwx" }; 

static void 
do_list(int argc, char * argv[])
{
     char * dir_str;
     DIR *dirp;
     char filename[PATH_MAX];
     char cwd[PATH_MAX];
     struct stat sbuf;
     int err;

     if( mount_count == 0 )
     {
         err_printf("fs: No filesystems mounted\n");
         return;
     }
     
     if (!scan_opts(argc, argv, 1, NULL, 0, &dir_str, OPTION_ARG_TYPE_STR, "directory"))
          return;

     if( dir_str == 0 )
     {
         dir_str = getcwd(cwd, sizeof(cwd));
     }
     
     dirp = opendir(dir_str);
     if (dirp==NULL) {
          err_printf("fs list: no such directory %s\n",dir_str);
          return;
     }
     
     for (;;) {
          struct dirent *entry = readdir(dirp);
          
          if( entry == NULL )
               break;
    
          strcpy(filename, dir_str);
          strcat(filename, "/");
          strcat(filename, entry->d_name);
          
          err = stat(filename, &sbuf);
          if (err < 0) {
               diag_printf("Unable to stat file %s\n", filename);
               continue;
          }
          diag_printf("%4d ", sbuf.st_ino);
          if (S_ISDIR(sbuf.st_mode)) diag_printf("d");
          if (S_ISCHR(sbuf.st_mode)) diag_printf("c");
          if (S_ISBLK(sbuf.st_mode)) diag_printf("b");
          if (S_ISREG(sbuf.st_mode)) diag_printf("-");
          if (S_ISLNK(sbuf.st_mode)) diag_printf("l");
          diag_printf("%s%s%s",    // Ho, humm, have to hard code the shifts
                      rwx[(sbuf.st_mode & S_IRWXU) >> 16],
                      rwx[(sbuf.st_mode & S_IRWXG) >> 19],
                      rwx[(sbuf.st_mode & S_IRWXO) >> 22]);
          diag_printf(" %2d size %6d %s\n",
                      sbuf.st_nlink,(int)sbuf.st_size, 
                      entry->d_name);
     }
     
     closedir(dirp);
     return;
}

local_cmd_entry("list", 
                "list directory contents",
                "[<directory>]",
                do_list,
                FS_cmds
    );


//==========================================================================


static void 
do_mkdir(int argc, char * argv[])
{
    char *dir_str;
    int err;
    
     if( mount_count == 0 )
     {
         err_printf("fs: No filesystems mounted\n");
         return;
     }
     
    if (!scan_opts(argc, argv, 1, NULL, 0, &dir_str, OPTION_ARG_TYPE_STR, "directory") ||
        dir_str == NULL)
    {
        fs_usage("invalid arguments");
        return;
    }

    err = mkdir( dir_str, 0 );

    if( err != 0 )
        err_printf("fs mkdir: failed to create directory %s\n",dir_str);
}

local_cmd_entry("mkdir", 
                "create directory",
                "<directory>",
                do_mkdir,
                FS_cmds
    );

//==========================================================================

static void 
do_deldir(int argc, char * argv[])
{
    char *dir_str;
    int err;
    
     if( mount_count == 0 )
     {
         err_printf("fs: No filesystems mounted\n");
         return;
     }
     
    if (!scan_opts(argc, argv, 1, NULL, 0, &dir_str, OPTION_ARG_TYPE_STR, "directory") ||
        dir_str == NULL)
    {
        fs_usage("invalid arguments");
        return;
    }

    err = rmdir( dir_str );

    if( err != 0 )
        err_printf("fs deldir: failed to remove directory %s\n",dir_str);
}

local_cmd_entry("deldir", 
                "delete directory",
                "<directory>",
                do_deldir,
                FS_cmds
    );

//==========================================================================

static void 
do_del(int argc, char * argv[])
{
    char *name_str = NULL;
    int err;
    
     if( mount_count == 0 )
     {
         err_printf("fs: No filesystems mounted\n");
         return;
     }
     
    if (!scan_opts(argc, argv, 1, NULL, 0, &name_str, OPTION_ARG_TYPE_STR, "file") ||
        name_str == NULL)
    {
        fs_usage("invalid arguments");
        return;
    }

    err = unlink( name_str );

    if( err != 0 )
        err_printf("fs del: failed to delete file %s\n",name_str);
}

local_cmd_entry("del", 
                "delete file",
                "<file>",
                do_del,
                FS_cmds
    );

//==========================================================================

static void 
do_move(int argc, char * argv[])
{
    int err;
    __externC int rename( const char *oldname, const char *newname );
    if( mount_count == 0 )
    {
        err_printf("fs: No filesystems mounted\n");
        return;
    }

    if( argc != 3 )
        fs_usage("bad arguments to move command\n");

    err = rename( argv[1], argv[2] );

    if( err != 0 )
        err_printf("fs move: failed to move file %s to %s\n",argv[1],argv[2]);
}

local_cmd_entry("move", 
                "move file",
                "<from> <to>",
                do_move,
                FS_cmds
    );

//==========================================================================

static void 
do_cd(int argc, char * argv[])
{
    char *dir_str;
    int err;
    
     if( mount_count == 0 )
     {
         err_printf("fs: No filesystems mounted\n");
         return;
     }
     
    if (!scan_opts(argc, argv, 1, NULL, 0, &dir_str, OPTION_ARG_TYPE_STR, "directory"))
        return;

    if( dir_str == NULL )
        dir_str = "/";
    
    err = chdir( dir_str );

    if( err != 0 )
        err_printf("fs cd: failed to change directory %s\n",dir_str);
}

local_cmd_entry("cd", 
                "change directory",
                "[<directory>]",
                do_cd,
                FS_cmds
    );

//==========================================================================

static void 
do_write(int argc, char * argv[])
{
    char *name_str = NULL;
    int err;
    struct option_info opts[2];    
    CYG_ADDRESS mem_addr = 0;
    unsigned long length = 0;
    bool mem_addr_set = false;
    bool length_set = false;
    int fd;
    
     if( mount_count == 0 )
     {
         err_printf("fs: No filesystems mounted\n");
         return;
     }

     init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
               (void *)&mem_addr, (bool *)&mem_addr_set, "memory base address");
     init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
               (void *)&length, (bool *)&length_set, "image length");
     
    if (!scan_opts(argc, argv, 1, opts, 2, &name_str, OPTION_ARG_TYPE_STR, "file name") ||
        name_str == NULL)
    {
        fs_usage("invalid arguments");
        return;
    }

//    diag_printf("load_address %08x %08x\n",load_address,load_address_end);
//    diag_printf("ram %08x %08x\n",ram_start, ram_end);
//    diag_printf("file name %08x >%s<\n",name_str,name_str);
    
    if (!mem_addr_set &&
        (load_address >= (CYG_ADDRESS)ram_start) &&
	((load_address_end) < (CYG_ADDRESS)ram_end))
    {
	mem_addr = load_address;
	mem_addr_set = true;
	if (!length_set)
        {
	    length = load_address_end - load_address;
	    length_set = true;
            // maybe get length from existing file size if no loaded
            // image?
        }
    }
    
    fd = open( name_str, O_WRONLY|O_CREAT|O_TRUNC );

    if( fd < 0 )
    {
        err_printf("fs write: Cannot open %s\n", name_str );
        return;
    }

//    diag_printf("write %08x %08x\n",mem_addr, length );
    
    err = write( fd, (void *)mem_addr, length );

    if( err != length )
    {
        err_printf("fs write: failed to write to file %d(%d) %d\n",err,length,errno);
    }

    err = close( fd );

    if( err != 0 )
        err_printf("fs write: close failed\n");
}

local_cmd_entry("write",
                "write data to file",
                "-b <mem_base> -l <image_length> <file_name>",
                do_write,
                FS_cmds
    );

//==========================================================================

__externC cyg_fstab_entry cyg_fstab[];
__externC cyg_fstab_entry cyg_fstab_end;
__externC cyg_mtab_entry cyg_mtab[];
__externC cyg_mtab_entry cyg_mtab_end;

static void 
do_info(int argc, char * argv[])
{
    cyg_bool found = false;
    cyg_fstab_entry *f;
    cyg_devtab_entry_t *t;

    for( f = &cyg_fstab[0] ; f != &cyg_fstab_end; f++ )
    {
        if( !found )
        {
            diag_printf("Filesystems available:\n");
            found = true;
        }
        diag_printf("%s\n",f->name);
    }

    found = false;
    for (t = &__DEVTAB__[0]; t != &__DEVTAB_END__; t++)
    {
        if( (t->status & CYG_DEVTAB_STATUS_BLOCK) == 0 ||
            (t->status & CYG_DEVTAB_STATUS_AVAIL) == 0 )
            continue;
        
        if( !found )
        {
            diag_printf("\nDevices available:\n");
            found = true;
        }
        diag_printf("%s\n",t->name);
    }

    if( mount_count != 0 )
    {
        int i;

        diag_printf("\nMounted filesystems:\n");
        diag_printf("            Device               Filesystem Mounted on\n");

        for( i = 0; i < MAX_MOUNTS; i++ )
        {
            if( mounts[i].mp_str[0] != '\0' )
                diag_printf("%32s %10s %s\n", mounts[i].dev_str, mounts[i].type_str, mounts[i].mp_str);
        }
    }
}

local_cmd_entry("info", 
                "filesystem info",
                "",
                do_info,
                FS_cmds
    );

//==========================================================================

static void
do_fs(int argc, char *argv[])
{
    struct cmd *cmd;

    if (argc < 2) {
        fs_usage("too few arguments");
        return;
    }
    if ((cmd = cmd_search(__FS_cmds_TAB__, &__FS_cmds_TAB_END__, 
                          argv[1])) != (struct cmd *)0) {
        (cmd->fun)(argc-1, argv+1);
        return;
    }
    fs_usage("unrecognized command");
}

RedBoot_nested_cmd("fs", 
            "Manage Filesystem files", 
            "{cmds}",
            do_fs,
            __FS_cmds_TAB__, &__FS_cmds_TAB_END__
    );


//==========================================================================

static int fd;

externC int 
fileio_stream_open(connection_info_t *info, int *err)
{
    char *filename = info->filename;

     if( mount_count == 0 )
     {
         diag_printf("fs: No filesystems mounted\n");
         return -1;
     }
    
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        diag_printf("fs: Open failed, error %d\n", errno);
        return -1;
    }
    return 0;
}

externC int 
fileio_stream_read(char *buf, int size, int *err)
{
    int nread;

    if ((nread = read(fd, buf, size)) < 0) {
        *err = errno;
        return -1;
    }
    return nread;
}

externC void
fileio_stream_close(int *err)
{
    close(fd);
}

externC char *
fileio_error(int err)
{
    static char myerr[10];

    diag_sprintf(myerr, "error %d", err);
    return myerr;
}

//
// RedBoot interface
//
GETC_IO_FUNCS(fileio_io, fileio_stream_open, fileio_stream_close,
              0, fileio_stream_read, fileio_error);
RedBoot_load(file, fileio_io, true, true, 0);

//==========================================================================
// End of fileio.c

