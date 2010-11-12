//==========================================================================
//
//      usb_test_ramdisk.c
//
//      Example application for the USB MSD layer in eCos.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
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
// Author(s):    ccoutand
// Contributors:
// Date:         2010-06-27
// Description:  USB Slave Mass Storage example application.
//
//####DESCRIPTIONEND####
//===========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#if defined(CYGPKG_FS_FAT)
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>
#endif

#include <pkgconf/io_usb_slave_msd.h>
#include <pkgconf/kernel.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/io/usb/usbs_msd_io.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(CYGPKG_FS_FAT)
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/testcase.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <cyg/fileio/fileio.h>
#include <cyg/fs/fatfs.h>
#endif

#if defined(CYGBLD_IO_USB_SLAVE_MSD_DEBUG)
#define DBG diag_printf
#else
#define DBG (1) ? (void)0 : diag_printf
#endif

// ----------------------------------------------------------------------------
// FAT12 file-system FBR, FAT and ROOT directory entries are hardcoded
// and later copied over to the RAM disk
//

#define USBS_MSD_RAMDISK_BLOCKLEN   512
#define USBS_MSD_RAMDISK_NUM_BLOCK   32

// Allocate space for the RAM disk
static char ram_disk[ USBS_MSD_RAMDISK_BLOCKLEN * USBS_MSD_RAMDISK_NUM_BLOCK ] __attribute__((aligned(4)));

// FAT12 Boot Record
const char fat12_br[] = {
   0xEB, 0x3C, 0x90,                               // Field for Bootable Media
   0x4D, 0x53, 0x57, 0x49, 0x4E, 0x34, 0x2E, 0x31, // MSWIN4.1
   0x00, 0x02,                                     // 512 bytes sector
   0x01,                                           // 1 sector per cluster
   0x01, 0x00,                                     // 1 reserved sector (including MBR)
   0x02,                                           // 2 FATs
   0x40, 0x00,                                     // 64 entries
   USBS_MSD_RAMDISK_NUM_BLOCK, 0x00,               // Number of sectors is less than 32K: 32 sectors
   0xF0,                                           // Removable media
   0x01, 0x00,                                     // 1sector per FAT
   0x00, 0x00,                                     // Sector per track (not used in LBA)
   0x00, 0x00,                                     // Number of Heads (not used in LBA)
   0x00, 0x00, 0x00, 0x00,                         // Hidden sector
   0x00, 0x00, 0x00, 0x00,                         // Number of sectors is more than 32K
   0x00,                                           // Logical drive number
   0x00,                                           // Reserved
   0x29,                                           // Extended boot signature
   0x04, 0x03, 0x02, 0x01,                         // Serial Number
   0x65, 0x43, 0x6F, 0x73, 0x20, 0x64, 0x69, 0x73, // VOLUME: eCos disk
   0x6B, 0x20, 0x20,
   0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20  // FAT12
};

// MBR and Volume sector signature
const char mbr_signature[] = {
   0x55, 0xAA
};

// Partition table
const char partition_tbl[] = {
   // First (FAT12)
   0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
   0x01, 0x00, 0x00, 0x00, (USBS_MSD_RAMDISK_NUM_BLOCK-1), 0x00, 0x00, 0x00,
   // Second
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   // Third
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   // Fourth
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Root directory (eCos-3.0)
const char root_dir[] = {
   0x65, 0x43, 0x6F, 0x73, 0x2d, 0x33, 0x2e, 0x30,
   0x20, 0x20, 0x20, 0x28, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x9e,
   0x65, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// File-system Allocation Table
const char fat[] = {
  0xF0, 0xff, 0xff
};

// Initialize FAT12
static void
init_fat12_disk( char *start_addr )
{
  char * dst = start_addr;

  // Copy partition table
  dst = start_addr + 446;
  memcpy(dst, partition_tbl, 64);

  // Copy signature
  dst = start_addr + (USBS_MSD_RAMDISK_BLOCKLEN - 2 );
  memcpy(dst, mbr_signature, 2);

  // Copy FBR
  dst = start_addr + USBS_MSD_RAMDISK_BLOCKLEN;
  memcpy(dst, fat12_br, 62);

  // Copy signature
  dst = start_addr + (( USBS_MSD_RAMDISK_BLOCKLEN * 2 ) - 2 );
  memcpy(dst, mbr_signature, 2);

  // Copy FAT0
  dst = start_addr + ( USBS_MSD_RAMDISK_BLOCKLEN * 2);
  memcpy(dst, fat, 3);

  // Copy FAT1
  dst = start_addr + ( USBS_MSD_RAMDISK_BLOCKLEN * 3);
  memcpy(dst, fat, 3);

  // Copy Root directory
  dst = start_addr + ( USBS_MSD_RAMDISK_BLOCKLEN * 4);
  memcpy(dst, root_dir, 32);

}


// ----------------------------------------------------------------------------
// Create a tiny "RAM disk" device
//
//

#define RAM_DISK_NAME "/dev/ramdisk0/"

typedef struct {
    volatile cyg_uint8 *base;
    cyg_uint16 size;
} ram_disk_info_t;

static cyg_bool ram_disk_init(struct cyg_devtab_entry *tab);

static Cyg_ErrNo ram_disk_bread(disk_channel *chan,
                              void         *buf,
                              cyg_uint32    len,
                              cyg_uint32    block_num);


static Cyg_ErrNo ram_disk_bwrite(disk_channel *chan,
                               const void   *buf,
                               cyg_uint32    len,
                               cyg_uint32    block_num);

static Cyg_ErrNo ram_disk_get_config(disk_channel *chan,
                               cyg_uint32    key,
                               const void   *xbuf,
                               cyg_uint32   *len);

static Cyg_ErrNo ram_disk_set_config(disk_channel *chan,
                               cyg_uint32    key,
                               const void   *xbuf,
                               cyg_uint32   *len);

static Cyg_ErrNo ram_disk_lookup(struct cyg_devtab_entry **tab,
                                struct cyg_devtab_entry  *sub_tab,
                                const char               *name);

DISK_FUNS(ram_disk_funs,
          ram_disk_bread,
          ram_disk_bwrite,
          ram_disk_get_config,
          ram_disk_set_config
);

DISK_CONTROLLER(ram_disk_controller, ram_disk_controller);

#define RAM_DISK_INSTANCE(_number_,_base_,_mbr_supp_,_name_)    \
static ram_disk_info_t ram_disk_info##_number_ = {              \
    base: (volatile cyg_uint8 *)_base_,                         \
};                                                              \
DISK_CHANNEL(ram_disk_channel##_number_,                        \
             ram_disk_funs,                                     \
             ram_disk_info##_number_,                           \
             ram_disk_controller,                               \
             _mbr_supp_,                                        \
             1                                                  \
);                                                              \
BLOCK_DEVTAB_ENTRY(ram_disk_io##_number_,                       \
             _name_,                                            \
             0,                                                 \
             &cyg_io_disk_devio,                                \
             ram_disk_init,                                     \
             ram_disk_lookup,                                   \
             &ram_disk_channel##_number_                        \
);

RAM_DISK_INSTANCE(0, 0, true, RAM_DISK_NAME);


// ----------------------------------------------------------------------------
static void
read_data(volatile cyg_uint8  *base,
          void                *buf,
          cyg_uint32           len)
{
    cyg_uint8 *dst = (cyg_uint8 *)buf;
    cyg_uint8 *src = (cyg_uint8 *)base;
    cyg_uint32 i = 0;

    for (i = 0; i < len; i += 1)
    {
        *dst = *src;
        dst = dst + 1; src = src + 1;
    }

}

static void
write_data(volatile cyg_uint8  *base,
           const void          *buf,
           cyg_uint16           len)
{
    cyg_uint8 *dst = (cyg_uint8 *)base;
    cyg_uint8 *src = (cyg_uint8 *)buf;
    cyg_uint32 i = 0;

    for (i = 0; i < len; i += 1)
    {
        *dst = *src;
         dst = dst + 1; src = src + 1;
    }
}

// ----------------------------------------------------------------------------
static cyg_bool
ram_disk_init(struct cyg_devtab_entry *tab)
{
    disk_channel           *chan = (disk_channel *) tab->priv;
    ram_disk_info_t         *info = (ram_disk_info_t *) chan->dev_priv;
    cyg_disk_identify_t     ident;

    if (chan->init)
        return true;

    info->base = (volatile cyg_uint8 *) &ram_disk[0];
    info->size = USBS_MSD_RAMDISK_BLOCKLEN * USBS_MSD_RAMDISK_NUM_BLOCK;

    DBG("RAM DISK ( %p ) init\n", info->base);

    init_fat12_disk( &ram_disk[0] );

    ident.cylinders_num   =  0;
    ident.heads_num       =  0;
    ident.lba_sectors_num = USBS_MSD_RAMDISK_NUM_BLOCK-2;
    ident.phys_block_size = USBS_MSD_RAMDISK_BLOCKLEN;
    ident.max_transfer    = USBS_MSD_RAMDISK_BLOCKLEN;

    if (!(chan->callbacks->disk_init)(tab))
        return false;

    if (ENOERR != (chan->callbacks->disk_connected)(tab, &ident))
        return false;

    return true;
}

static Cyg_ErrNo
ram_disk_lookup(struct cyg_devtab_entry **tab,
               struct cyg_devtab_entry  *sub_tab,
               const char *name)
{
    disk_channel *chan = (disk_channel *) (*tab)->priv;
    return (chan->callbacks->disk_lookup)(tab, sub_tab, name);
}

static Cyg_ErrNo
ram_disk_bread(disk_channel *chan,
             void         *buf,
             cyg_uint32    len,
             cyg_uint32    block_num)
{
    ram_disk_info_t *info = (ram_disk_info_t *)chan->dev_priv;
    volatile cyg_uint8 *src = info->base + USBS_MSD_RAMDISK_BLOCKLEN * block_num;
    cyg_uint32 len_byte = len * USBS_MSD_RAMDISK_BLOCKLEN;

    if ( len_byte == 0 )
      return EINVAL;

    read_data(src, buf, len_byte);

    return ENOERR;
}

static Cyg_ErrNo
ram_disk_bwrite(disk_channel *chan,
              const void   *buf,
              cyg_uint32    len,
              cyg_uint32    block_num)
{
    ram_disk_info_t *info = (ram_disk_info_t *)chan->dev_priv;
    volatile cyg_uint8 *dst = info->base + USBS_MSD_RAMDISK_BLOCKLEN * block_num;
    cyg_uint32 len_byte = len * USBS_MSD_RAMDISK_BLOCKLEN;

    if ( len_byte == 0 )
      return EINVAL;

    write_data(dst, buf, len_byte);

    return ENOERR;
}

static Cyg_ErrNo
ram_disk_get_config(disk_channel *chan,
                   cyg_uint32    key,
                   const void   *xbuf,
                   cyg_uint32   *len)
{
    return -EINVAL;
}

static Cyg_ErrNo
ram_disk_set_config(disk_channel *chan,
                   cyg_uint32    key,
                   const void   *xbuf,
                   cyg_uint32   *len)
{
    return -EINVAL;
}


// ----------------------------------------------------------------------------
// In case FAT file-system package is included, this application will try
// to play with the file-system before it is mounted as a mass storage device.
// The following code is a copy of the fatfs1.c from the fs/test directory
//
#if defined(CYGPKG_FS_FAT)

#define SHOW_RESULT( _fn, _res ) \
  DBG("<FAIL>: " #_fn "() returned %d %s\n", _res, _res<0?strerror(errno):"");

#define IOSIZE  100

//==========================================================================

void checkcwd( const char *cwd )
{
    static char cwdbuf[PATH_MAX];
    char *ret;

    ret = getcwd( cwdbuf, sizeof(cwdbuf));
    if( ret == NULL ) SHOW_RESULT( getcwd, (int)ret );

    if( strcmp( cwdbuf, cwd ) != 0 )
    {
        DBG( "cwdbuf %s cwd %s\n",cwdbuf, cwd );
        CYG_TEST_FAIL( "Current directory mismatch");
    }
}

//==========================================================================

static void listdir( char *name, int statp, int numexpected, int *numgot )
{
    int err;
    DIR *dirp;
    int num=0;

    DBG("<INFO>: reading directory %s\n",name);

    dirp = opendir( name );
    if( dirp == NULL ) SHOW_RESULT( opendir, -1 );

    for(;;)
    {
        struct dirent *entry = readdir( dirp );

        if( entry == NULL )
            break;
        num++;
        DBG("<INFO>: entry %14s",entry->d_name);
#ifdef CYGPKG_FS_FAT_RET_DIRENT_DTYPE
        DBG(" d_type %2x", entry->d_type);
#endif
        if( statp )
        {
            char fullname[PATH_MAX];
            struct stat sbuf;

            if( name[0] )
            {
                strcpy(fullname, name );
                if( !(name[0] == '/' && name[1] == 0 ) )
                    strcat(fullname, "/" );
            }
            else fullname[0] = 0;

            strcat(fullname, entry->d_name );

            err = stat( fullname, &sbuf );
            if( err < 0 )
            {
                if( errno == ENOSYS )
                    DBG(" <no status available>");
                else SHOW_RESULT( stat, err );
            }
            else
            {
                DBG(" [mode %08x ino %08x nlink %d size %ld]",
                            sbuf.st_mode,sbuf.st_ino,sbuf.st_nlink,(long)sbuf.st_size);
            }
#ifdef CYGPKG_FS_FAT_RET_DIRENT_DTYPE
            if ((entry->d_type & S_IFMT) != (sbuf.st_mode & S_IFMT))
              CYG_TEST_FAIL("File mode's don't match between dirent and stat");
#endif
        }

        DBG("\n");
    }

    err = closedir( dirp );
    if( err < 0 ) SHOW_RESULT( stat, err );
    if (numexpected >= 0 && num != numexpected)
        CYG_TEST_FAIL("Wrong number of dir entries\n");
    if ( numgot != NULL )
        *numgot = num;
}

//==========================================================================

static void createfile( char *name, size_t size )
{
    char buf[IOSIZE];
    int fd;
    ssize_t wrote;
    int i;
    int err;

    DBG("<INFO>: create file %s size %zd \n",name,size);

    err = access( name, F_OK );
    if( err < 0 && errno != EACCES ) SHOW_RESULT( access, err );

    for( i = 0; i < IOSIZE; i++ ) buf[i] = i%256;

    fd = open( name, O_WRONLY|O_CREAT );
    if( fd < 0 ) SHOW_RESULT( open, fd );

    while( size > 0 )
    {
        ssize_t len = size;
        if ( len > IOSIZE ) len = IOSIZE;

        wrote = write( fd, buf, len );
        if( wrote != len ) SHOW_RESULT( write, (int)wrote );

        size -= wrote;
    }

    err = close( fd );
    if( err < 0 ) SHOW_RESULT( close, err );
}

//==========================================================================

static void checkfile( char *name )
{
    char buf[IOSIZE];
    int fd;
    ssize_t done;
    int i;
    int err;
    off_t pos = 0;

    DBG("<INFO>: check file %s\n",name);

    err = access( name, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );

    fd = open( name, O_RDONLY );
    if( fd < 0 ) SHOW_RESULT( open, fd );

    for(;;)
    {
        done = read( fd, buf, IOSIZE );
        if( done < 0 ) SHOW_RESULT( read, (int)done );

        if( done == 0 ) break;

        for( i = 0; i < done; i++ )
            if( buf[i] != i%256 )
            {
                DBG("buf[%ld+%d](%02x) != %02x\n",pos,i,buf[i],i%256);
                CYG_TEST_FAIL("Data read not equal to data written\n");
            }

        pos += done;
    }

    err = close( fd );
    if( err < 0 ) SHOW_RESULT( close, err );
}

//==========================================================================

static void copyfile( char *name2, char *name1 )
{

    int err;
    char buf[IOSIZE];
    int fd1, fd2;
    ssize_t done, wrote;

    DBG("<INFO>: copy file %s -> %s\n",name2,name1);

    err = access( name1, F_OK );
    if( err < 0 && errno != EACCES ) SHOW_RESULT( access, err );

    err = access( name2, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );

    fd1 = open( name1, O_WRONLY|O_CREAT );
    if( fd1 < 0 ) SHOW_RESULT( open, fd1 );

    fd2 = open( name2, O_RDONLY );
    if( fd2 < 0 ) SHOW_RESULT( open, fd2 );

    for(;;)
    {
        done = read( fd2, buf, IOSIZE );
        if( done < 0 ) SHOW_RESULT( read, (int)done );

        if( done == 0 ) break;

        wrote = write( fd1, buf, done );
        if( wrote != done ) SHOW_RESULT( write, (int) wrote );

        if( wrote != done ) break;
    }

    err = close( fd1 );
    if( err < 0 ) SHOW_RESULT( close, err );

    err = close( fd2 );
    if( err < 0 ) SHOW_RESULT( close, err );

}

//==========================================================================

static void comparefiles( char *name2, char *name1 )
{
    int err;
    char buf1[IOSIZE];
    char buf2[IOSIZE];
    int fd1, fd2;
    ssize_t done1, done2;
    int i;

    DBG("<INFO>: compare files %s == %s\n",name2,name1);

    err = access( name1, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );

    err = access( name1, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );

    fd1 = open( name1, O_RDONLY );
    if( fd1 < 0 ) SHOW_RESULT( open, fd1 );

    fd2 = open( name2, O_RDONLY );
    if( fd2 < 0 ) SHOW_RESULT( open, fd2 );

    for(;;)
    {
        done1 = read( fd1, buf1, IOSIZE );
        if( done1 < 0 ) SHOW_RESULT( read, (int)done1 );

        done2 = read( fd2, buf2, IOSIZE );
        if( done2 < 0 ) SHOW_RESULT( read, (int)done2 );

        if( done1 != done2 )
            DBG("Files different sizes\n");

        if( done1 == 0 ) break;

        for( i = 0; i < done1; i++ )
            if( buf1[i] != buf2[i] )
            {
                DBG("buf1[%d](%02x) != buf1[%d](%02x)\n",i,buf1[i],i,buf2[i]);
                CYG_TEST_FAIL("Data in files not equal\n");
            }
    }

    err = close( fd1 );
    if( err < 0 ) SHOW_RESULT( close, err );

    err = close( fd2 );
    if( err < 0 ) SHOW_RESULT( close, err );

}

#endif



// ----------------------------------------------------------------------------
// Entry point of the test application. If the FAT file-system package is
// included, the file-system is mounted and tested before the USB Mass Storage
// service is started.
//
// Once the mass storage device is mounted on the host computer, 2 files
// (foo and fee) and one directory (bar) should appear in the device
//

int cyg_start(void)
{
#if defined(CYGPKG_FS_FAT)
  int err;
  int existingdirents=-1;
#endif

  DBG("Start USB MSD application\n\r");

#if defined(CYGPKG_FS_FAT)

  // Mount RAM disk partition 1
  err = mount( "/dev/ramdisk0/1", "/", "fatfs" );

  if( err < 0 )
     SHOW_RESULT( mount, err );

  err = chdir( "/" );
  if( err < 0 )
     SHOW_RESULT( chdir, err );

  checkcwd( "/" );

  // Display list of all files/directories from root
  listdir( "/", true, -1, &existingdirents );

  // Play around with the file-system, create / copy /
  // compare files
  createfile( "/foo", 1000 );
  checkfile( "foo" );
  copyfile( "foo", "fee" );
  checkfile( "fee" );
  comparefiles( "foo", "/fee" );
  DBG("<INFO>: mkdir bar\n");

  // Create new directory
  err = mkdir( "/bar", 0 );
  if( err < 0 )
     SHOW_RESULT( mkdir, err );

  // Display list of all files/directories from root
  listdir( "/" , true, existingdirents+3, NULL );

  // Umount file-system
  err = umount( "/" );

  if( err < 0 )
     SHOW_RESULT( umount, err );

#endif

  // Start Mass Storage Service
  usbs_msd_start();

  // Start scheduler
  cyg_scheduler_start();

}
