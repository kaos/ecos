//==========================================================================
//
//      fileio.c
//
//      RedBoot fileio support
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2002, 2003 Gary Thomas
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
// Author(s):    dwmw2, msalter
// Date:         2003-11-27
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
#include <cyg/io/io.h>
#include <cyg/io/flash.h>
#include <cyg/io/config_keys.h>
#endif
#include <cyg/fileio/fileio.h>

static void do_mount(int argc, char *argv[]);
static void do_umount(int argc, char *argv[]);

#ifdef CYGPKG_IO_FLASH_BLOCK_DEVICE
#define FLASHPART "[-f <partition>] "
#else
#define FLASHPART
#endif

RedBoot_cmd("mount", 
            "Mount file system",
	    FLASHPART "[-d <device>] -t fstype",
            do_mount
    );
RedBoot_cmd("umount", 
            "Unmount file system",
            "",
            do_umount
    );

int fileio_mounted = 0;

// Mount disk/filesystem
static void
do_mount(int argc, char *argv[])
{
    char *part_str, *dev_str, *type_str;
    bool part_set = false, dev_set = false, type_set = false;
    struct option_info opts[3];
    int err, num_opts = 2;

    init_opts(&opts[0], 'd', true, OPTION_ARG_TYPE_STR,
              (void *)&dev_str, &dev_set, "device");
    init_opts(&opts[1], 't', true, OPTION_ARG_TYPE_STR,
              (void *)&type_str, &type_set, "fstype");
#ifdef CYGPKG_IO_FLASH_BLOCK_DEVICE
    init_opts(&opts[2], 'f', true, OPTION_ARG_TYPE_STR,
              (void *)&part_str, &part_set, "partition");
    num_opts++;
#endif

    if (!scan_opts(argc, argv, 1, opts, num_opts, NULL, 0, NULL))
        return;

    if (!type_set) {
        diag_printf("Must specify file system type\n");
        return;
    }
    if (fileio_mounted) {
        diag_printf("A file system is already mounted\n");
        return;
    }
#ifdef CYGPKG_IO_FLASH_BLOCK_DEVICE
    if (part_set) {
        int len;
        cyg_io_handle_t h;

        if (dev_set && strcmp(dev_str, CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1)) {
            diag_printf("May only set one of <device> or <partition>\n");
            return;
        }

        dev_str = CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1;
        len = strlen(part_str);

        err = cyg_io_lookup(dev_str, &h);
        if (err < 0) {
            diag_printf("cyg_io_lookup of \"%s\" returned %d\n", err);
            return;
        }
        err = cyg_io_set_config(h, CYG_IO_SET_CONFIG_FLASH_FIS_NAME,
                                part_str, &len);
        if (err < 0) {
            diag_printf("FIS partition \"%s\" not found\n",
                        part_str);
            return;
        }
    }
#endif
    err = mount(dev_str, "/", type_str);

    if (err) {
        diag_printf("Mount failed %d\n", err);
    } else {
//        diag_printf("Mount %s file system succeeded\n", type_str);
        fileio_mounted = 1;
    }
}

static void
do_umount(int argc, char *argv[])
{
    if (!fileio_mounted) {
        return;
    }
    umount ("/");
    fileio_mounted = 0;
}

static int fd;

externC int 
fileio_stream_open(connection_info_t *info, int *err)
{
    char *filename = info->filename;

    if (!fileio_mounted) {
        diag_printf("No file system mounted\n");
        return -1;
    }
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        diag_printf("Open failed, error %d\n", errno);
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

    diag_sprintf(myerr, "error %d\n", err);
    return myerr;
}

//
// RedBoot interface
//
GETC_IO_FUNCS(fileio_io, fileio_stream_open, fileio_stream_close,
              0, fileio_stream_read, fileio_error);
RedBoot_load(file, fileio_io, true, true, 0);

