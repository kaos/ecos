//==========================================================================
//
//      disk.h
//
//      Stand-alone disk support for RedBoot
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    msalter
// Contributors: msalter
// Date:         2001-07-02
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef CYGONCE_REDBOOT_DISK_H
#define CYGONCE_REDBOOT_DISK_H

#define SECTOR_SIZE 512

// Convenience macros to access disk/filesystem info which may
// be stored in a fixed endian format.

#define __SWAB16(x) \
    ((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF))

#define __SWAB32(x)         \
   ((((x) & 0xff) << 24)   |  \
    (((x) & 0xff00) <<  8) |  \
    (((x) >> 8) & 0xff)    |  \
    (((x) >> 24) & 0xff))

#if (CYG_BYTEORDER == CYG_MSBFIRST)
#define SWAB_LE16(x) __SWAB16(x)
#define SWAB_LE32(x) __SWAB32(x)
#define SWAB_BE16(x) (x)
#define SWAB_BE32(x) (x)
#else
#define SWAB_LE16(x) (x)
#define SWAB_LE32(x) (x)
#define SWAB_BE16(x) __SWAB16(x)
#define SWAB_BE32(x) __SWAB32(x)
#endif

struct partition;

// filesystem interface
typedef struct fs_funs {
    // Load a file into memory.
    void * (*open)(struct partition *p, const char *path);
    int    (*read)(void *fp, char *buf, cyg_uint32 nbytes);
} fs_funs_t;

struct disk;

typedef struct partition {
    struct disk *disk;
    fs_funs_t   *funs;
    cyg_uint32	start_sector;	// first sector in partition
    cyg_uint32	nr_sectors;	// number of sectors in partition
    cyg_uint8   systype;        // FAT12, FAT16, Linux, etc.
    cyg_uint8   bootflag;       // not really used...
} partition_t;

// System types
#define SYSTYPE_FAT12        0x01
#define SYSTYPE_FAT16_32M    0x04
#define SYSTYPE_EXTENDED     0x05
#define SYSTYPE_FAT16        0x06
#define SYSTYPE_LINUX_SWAP   0x82
#define SYSTYPE_LINUX        0x83

typedef struct disk_funs {
    int  (*read)(struct disk *d,
                 cyg_uint32  start_sector,
                 cyg_uint32  *buf,
                 cyg_uint8   nr_sectors);
} disk_funs_t;


typedef struct disk {
    disk_funs_t *funs;		// Disk driver functions
    void        *private;  	// Whatever is needed by disk functions
    cyg_uint32  nr_sectors;	// Total disk size in sectors
    short       kind;   	// IDE_HD, IDE_CDROM, SCSI_HD, etc
    short       index;  	// index within specific kind
    partition_t partitions[CYGNUM_REDBOOT_MAX_PARTITIONS];
} disk_t;

#define DISK_READ(d,s,p,n) ((d)->funs->read)((d),(s),(p),(n))
#define PARTITION_READ(part,s,p,n) \
    DISK_READ((part)->disk, (s) + (part)->start_sector, (p), (n))

// Kinds of disks
#define DISK_IDE_HD     1
#define DISK_IDE_CDROM  2
#define DISK_FLOPPY     3

// DOS partition table as laid out in the MBR
//
struct mbr_partition {
    cyg_uint8  boot_ind;	// 0x80 == active
    cyg_uint8  head;
    cyg_uint8  sector;
    cyg_uint8  cyl;
    cyg_uint8  sys_ind;		// partition type
    cyg_uint8  end_head;
    cyg_uint8  end_sector;
    cyg_uint8  end_cyl;
    cyg_uint8  start_sect[4];	// starting sector counting from 0
    cyg_uint8  nr_sects[4];	// number of sectors in partition
};

#define MBR_PTABLE_OFFSET 0x1be
#define MBR_MAGIC_OFFSET  0x1fe
#define MBR_MAGIC         0xaa55

// Add a disk to the disk table.
// Return zero if no more room in table.
//
externC int disk_register(disk_t *disk);


#define diskerr_badname   -1
#define diskerr_partition -2
#define diskerr_open      -3
#define diskerr_read      -4

externC int   disk_stream_open(char *filename, int *err);    
externC void  disk_stream_close(int *err);    
externC int   disk_stream_read(char *buf, int size, int *err);    
externC char *disk_error(int err);

#endif // CYGONCE_REDBOOT_DISK_H
