//==========================================================================
//
//      fatfs_supp.c
//
//      FAT file system support functions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           savin 
// Date:                2003-06-30
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/fs_fat.h>
#include <pkgconf/infra.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/diag.h>
#include <cyg/io/io.h>
#include <blib/blib.h>
#include <sys/types.h>
#include <ctype.h>

#include "fatfs.h"

//==========================================================================
// FAT defines & macros

// -------------------------------------------------------------------------
// FAT dir entry attributes

#define DENTRY_ATTR_RDONLY  0x01 // Read only
#define DENTRY_ATTR_HIDDEN  0x02 // Hidden
#define DENTRY_ATTR_SYSTEM  0x04 // System
#define DENTRY_ATTR_VOLUME  0x08 // Volume label
#define DENTRY_ATTR_DIR     0x10 // Subdirectory
#define DENTRY_ATTR_ARCHIVE 0x20 // Needs archiving

// -------------------------------------------------------------------------
// FAT dir entry attributes macros

#define DENTRY_IS_RDONLY(_dentry_)  ((_dentry_)->attr & DENTRY_ATTR_RDONLY)
#define DENTRY_IS_HIDDEN(_dentry_)  ((_dentry_)->attr & DENTRY_ATTR_HIDDEN)
#define DENTRY_IS_SYSTEM(_dentry_)  ((_dentry_)->attr & DENTRY_ATTR_SYSTEM)
#define DENTRY_IS_VOLUME(_dentry_)  ((_dentry_)->attr & DENTRY_ATTR_VOLUME)
#define DENTRY_IS_DIR(_dentry_)     ((_dentry_)->attr & DENTRY_ATTR_DIR)
#define DENTRY_IS_ARCHIVE(_dentry_) ((_dentry_)->attr & DENTRY_ATTR_ARCHIVE)

#define DENTRY_IS_DELETED(_dentry_) \
    (0xE5 == (unsigned char)((_dentry_)->name[0]))

#define DENTRY_IS_ZERO(_dentry_) \
    (0x00 == (unsigned char)((_dentry_)->name[0]))

// -------------------------------------------------------------------------
// FAT disk data access macros

// FIXME: support big endian machines!
   
#define GET_BYTE(_data_, _var_, _off_) \
    (_var_ = *( ((cyg_uint8 *)_data_) + (_off_) ) )

#define GET_WORD(_data_, _var_, _off_)                      \
    (_var_ = *( ((cyg_uint8 *)_data_) + (_off_) ) |         \
             *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) << 8)

#define GET_DWORD(_data_, _var_, _off_)                         \
    (_var_ = *( ((cyg_uint8 *)_data_) + (_off_))             |  \
             *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) << 8   |  \
             *( ((cyg_uint8 *)_data_) + (_off_) + 2 ) << 16  |  \
             *( ((cyg_uint8 *)_data_) + (_off_) + 3 ) << 24)

#define GET_BYTES(_data_, _var_, _size_, _off_) \
    memcpy((void *)(_var_), (void*)(((cyg_uint8 *)_data_)+(_off_)),_size_)

#define SET_BYTE(_data_, _val_, _off_) \
    (*( ((cyg_uint8 *)_data_) + (_off_) ) = _val_)

#define SET_WORD(_data_, _val_, _off_)                                   \
    do {                                                                 \
        *( ((cyg_uint8 *)_data_) + (_off_) )     = _val_         & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) = (_val_ >> 8)  & 0xFF; \
    } while (0)

#define SET_DWORD(_data_, _val_, _off_)                                  \
    do {                                                                 \
        *( ((cyg_uint8 *)_data_) + (_off_) )     = _val_         & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 1 ) = (_val_ >> 8)  & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 2 ) = (_val_ >> 16) & 0xFF; \
        *( ((cyg_uint8 *)_data_) + (_off_) + 3 ) = (_val_ >> 24) & 0xFF; \
    } while (0)

#define SET_BYTES(_data_, _var_, _size_, _off_) \
    memcpy((void *)(((cyg_uint8 *)_data_)+(_off_)), (void *)(_var_), _size_)

// -------------------------------------------------------------------------
// FAT table entries types 

#define TENTRY_REGULAR  0 // Used when entry points to next file cluster 
#define TENTRY_FREE     1 // Free cluster
#define TENTRY_LAST     2 // Last cluster of file 
#define TENTRY_RESERVED 3 // Reserved cluster
#define TENTRY_BAD      4 // Bad cluster 

// -------------------------------------------------------------------------
// FAT table structures size 

#define DENTRY_SIZE      0x20 // Dir entry size

// -------------------------------------------------------------------------
// Time & date defines 

#define JD_1_JAN_1970 2440588 // 1 Jan 1970 in Julian day number

// -------------------------------------------------------------------------
// Code tracing defines 

#ifdef FATFS_TRACE_FAT_TABLE
# define TFT 1
#else
# define TFT 0
#endif

#ifdef FATFS_TRACE_DIR_ENTRY
# define TDE 1
#else
# define TDE 0
#endif

#ifdef FATFS_TRACE_CLUSTER
# define TCL 1
#else
# define TCL 0
#endif

#ifdef FATFS_TRACE_DATA
# define TDO 1
#else
# define TDO 0
#endif
    
// -------------------------------------------------------------------------
// FAT table entry type strings 

#if TFT
static const char *tentry_type_name[5] = {
    "REGULAR", "FREE", "LAST", "RESERVED", "BAD"
};   
#endif

//==========================================================================
// FAT structures 

// -------------------------------------------------------------------------
// FAT table boot record structure 
   
typedef struct fat_boot_record_s
{
    cyg_uint16    jump;           // 00h : Jump code
//  cyg_uint8     jump0;          //                 + NOP
    char          oem_name[8+1];  // 03h : OEM name
    cyg_uint16    bytes_per_sec;  // 0Bh : cyg_bytes per sector
    cyg_uint8     sec_per_clust;  // 0Dh : Sectors per cluster
    cyg_uint16    res_sec_num;    // 0Eh : Number of reserved sectors
    cyg_uint8     fat_tbls_num;   // 10h : Number of copies of fat
    cyg_uint16    max_root_dents; // 11h : Maximum number of root dir entries
    cyg_uint16    sec_num_32;     // 13h : Number of sectors in partition < 32MB
    cyg_uint8     media_desc;     // 15h : Media descriptor
    cyg_uint16    sec_per_fat;    // 16h : Sectors per FAT
    cyg_uint16    sec_per_track;  // 18h : Sectors per track
    cyg_uint16    heads_num;      // 1Ah : Number of heads
    cyg_uint32    hsec_num;       // 1Ch : Number of hidden sectors
    cyg_uint32    sec_num;        // 20h : Number of sectors in partition
    cyg_uint16    ldrv_num;       // 24h : Logical drive number of partition
    cyg_uint8     ext_sig;        // 26h : Extended signature
    cyg_uint32    ser_num;        // 27h : Serial number of partition
    char          vol_name[11+1]; // 2Bh : Volume name of partition
    char          fat_name[8+1];  // 36h : FAT name
//  unsigned char exe_code[448];  // 3Eh : Executable code
    unsigned char exe_marker[2];  // 1FEh: Executable marker (55h AAh)
} fat_boot_record_t;

// -------------------------------------------------------------------------
// FAT dir entry structure 
 
typedef struct fat_dir_entry_s
{
    char       name[8+1];       // 00h : Name
    char       ext[3+1];        // 08h : Extension
    cyg_uint8  attr;            // 0Bh : Attribute
    cyg_uint8  nt_reserved;     // 0Ch : Win NT Reserved field
    cyg_uint8  crt_sec_100;     // 0Dh : Creation time ms stamp 0 - 199
    cyg_uint16 crt_time;        // 0Eh : Creation time
    cyg_uint16 crt_date;        // 10h : Creation date
    cyg_uint16 acc_date;        // 12h : Last access date
    cyg_uint16 cluster_HI;      // 14h : Starting cluster HI WORD (FAT32)
    cyg_uint16 wrt_time;        // 16h : Time    
    cyg_uint16 wrt_date;        // 18h : Date
    cyg_uint16 cluster;         // 1Ah : Starting cluster 
    cyg_uint32 size;            // 1Ch : Size of the file    

    fatfs_data_pos_t pos;       // Positon on disk
} fat_dir_entry_t;

// -------------------------------------------------------------------------
// FAT cluster opts 
 
typedef enum cluster_opts_e
{
    CO_NONE       = 0x00, // NULL option
    CO_EXTEND     = 0x01, // Extend cluster chain if one cluster too short
    CO_ERASE_NEW  = 0x02, // Erase newly allocated cluster
    CO_MARK_LAST  = 0x04  // Mark  newly allocated cluster as last
} cluster_opts_t;

//==========================================================================
// Utility functions 

// -------------------------------------------------------------------------
// get_val_log2()
// Gets the log2 of given value or returns 0 if value is 
// not power of 2. 
 
static cyg_uint32 
get_val_log2(cyg_uint32 val)
{
    cyg_uint32 i, log2;
    
    i = val;
    log2 = 0;
    while (0 == (i & 1))
    {
        i >>= 1;
        log2++;
    }
    if (i != 1)
        return 0;
    else
        return log2;
}

// -------------------------------------------------------------------------
// get_data_disk_apos()
// Gets the absolute data position on disk from cluster number and
// position inside given cluster. 
 
static __inline__ cyg_uint32
get_data_disk_apos(fatfs_disk_t *disk, cyg_uint32 cluster, cyg_uint32 pos)
{
    return (disk->fat_data_pos + disk->cluster_size * (cluster - 2) + pos);
}

// -------------------------------------------------------------------------
// jdays_to_gdate()
// Converts juilan days into gregorian date.
 
static void
jdays_to_gdate(cyg_uint32 jd, int *day, int *month, int *year)
{
    cyg_uint32 l, n, i, j;

    l = jd + 68569;
    n = (4 * l) / 146097;
    l = l - (146097 * n + 3) / 4;
    i = (4000 * (l + 1)) / 1461001;
    l = l - (1461 * i) / 4 + 31;
    j = (80 * l) / 2447;
    *day = l - (2447 * j) / 80;

    l = j / 11;
    *month = j + 2 - (12 * l);
    *year = 100 * (n - 49) + i + l;
}

// -------------------------------------------------------------------------
// gdate_to_jdays()
// Converts gregorian date to juilan days.
 
static void
gdate_to_jdays(int day, int month, int year, cyg_uint32 *jd)
{
    *jd = day - 32075 + 1461*(year + 4800 + (month - 14)/12)/4 +
          367*(month - 2 - (month - 14)/12*12)/12 - 
          3*((year + 4900 + (month - 14)/12)/100)/4;
}
 
// -------------------------------------------------------------------------
// date_unix2dos()
// Converts unix timestamp to dos time and date. 
                 
static void
date_unix2dos(cyg_uint32  unix_timestamp, 
              cyg_uint16 *dos_time,
              cyg_uint16 *dos_date)
{
    cyg_uint32 jd;
    cyg_uint16 dtime, ddate;
    int hour, min, sec;
    int day, month, year;
    
    hour = (unix_timestamp / 3600) % 24;
    min  = (unix_timestamp / 60) % 60;
    sec  =  unix_timestamp % 60;

    jd = JD_1_JAN_1970 + unix_timestamp / (3600 * 24);
    jdays_to_gdate(jd, &day, &month, &year);

    CYG_TRACE7(TDE, "ts=%d date=%d:%d:%d %d-%d-%d",
                   unix_timestamp, hour, min, sec, year, month, day);

    if (year < 1980)
        year = 1980;

    dtime = (hour << 11) | (min << 5) | (sec >> 1);
    ddate = ((year - 1980) << 9) | (month << 5) | day;
 
    CYG_TRACE2(TDE, "dos time=%d date=%d", dtime, ddate);
    
    if (NULL != dos_time) *dos_time = dtime;
    if (NULL != dos_date) *dos_date = ddate;
}

// -------------------------------------------------------------------------
// date_dos2unix()
// Converts dos time and date to unix timestamp. 
 
static void
date_dos2unix(cyg_uint16  dos_time, 
              cyg_uint16  dos_date, 
              cyg_uint32 *unix_timestamp)
{
    int hour, min, sec;
    int day, month, year;
    cyg_uint32 ts; 
    
    sec = (dos_time & ((1<<5)-1)) * 2;
    dos_time >>= 5;
    min = (dos_time & ((1<<6)-1));
    dos_time >>= 6;
    hour = dos_time;
    
    day = (dos_date & ((1<<5)-1));
    dos_date >>= 5;
    month = (dos_date & ((1<<4)-1));
    dos_date >>= 4;
    year = dos_date + 1980;

    gdate_to_jdays(day, month, year, &ts);
    ts -= JD_1_JAN_1970;
    ts = (ts * 24 * 3600) + (sec + min * 60 + hour * 3600);
    
    *unix_timestamp = ts;

    CYG_TRACE2(TDE, "dos time=%d date=%d", dos_time, dos_date);
    CYG_TRACE7(TDE, "timestamp=%d date=%d:%d:%d %d-%d-%d",
                    ts, hour, min, sec, year, month, day);
}

//==========================================================================
// FAT boot record functions 

// -------------------------------------------------------------------------
// print_boot_record()
// Prints FAT boot record.

#if TFT 
static void
print_boot_record(fat_boot_record_t* fbr)
{
    diag_printf("FAT: FBR jump code:       0x%02X\n", fbr->jump);
    diag_printf("FAT: FBR oem name:       '%.8s'\n",  fbr->oem_name);
    diag_printf("FAT: FBR bytes per sec:   %u\n",     fbr->bytes_per_sec);
    diag_printf("FAT: FBR sec per cluster: %u\n",     fbr->sec_per_clust);
    diag_printf("FAT: FBR reserved sec:    %u\n",     fbr->res_sec_num);
    diag_printf("FAT: FBR fat tbls num:    %u\n",     fbr->fat_tbls_num);
    diag_printf("FAT: FBR max root dents:  %u\n",     fbr->max_root_dents);
    diag_printf("FAT: FBR sec num (32):    %u\n",     fbr->sec_num_32);
    diag_printf("FAT: FBR media desc:      0x%02X\n", fbr->media_desc);
    diag_printf("FAT: FBR sec per fat:     %u\n",     fbr->sec_per_fat);
    diag_printf("FAT: FBR sec per track:   %u\n",     fbr->sec_per_track);
    diag_printf("FAT: FBR heads num:       %u\n",     fbr->heads_num);
    diag_printf("FAT: FBR hidden sec num:  %u\n",     fbr->hsec_num);
    diag_printf("FAT: FBR sec num:         %u\n",     fbr->sec_num);
    diag_printf("FAT: FBR log drv num:     %u\n",     fbr->ldrv_num);
    diag_printf("FAT: FBR ext sig:         0x%02X\n", fbr->ext_sig);
    diag_printf("FAT: FBR ser num:         0x%08X\n", fbr->ser_num);
    diag_printf("FAT: FBR vol name:       '%.11s'\n", fbr->vol_name);
    diag_printf("FAT: FBR fat name:       '%.8s'\n",  fbr->fat_name);
    diag_printf("FAT: FBR exe mark:        0x%02X 0x%02X\n", 
                fbr->exe_marker[0], fbr->exe_marker[1]);
}
#endif // TFT

// -------------------------------------------------------------------------
// read_boot_record()
// Reads FAT boot record from disk.
 
static int 
read_boot_record(fatfs_disk_t *disk, fat_boot_record_t *fbr)
{
    int len, err;
    unsigned char data[0x3E];
    
    len = 0x3E;
    err = cyg_blib_read(&disk->blib, (void*)data, &len, 0, 0);
    if (err != ENOERR)
        return err;
   
    GET_WORD(data,  fbr->jump,           0x00);
    GET_BYTES(data, fbr->oem_name, 8,    0x03);
    GET_WORD(data,  fbr->bytes_per_sec,  0x0B);
    GET_BYTE(data,  fbr->sec_per_clust,  0x0D);
    GET_WORD(data,  fbr->res_sec_num,    0x0E);
    GET_BYTE(data,  fbr->fat_tbls_num,   0x10);
    GET_WORD(data,  fbr->max_root_dents, 0x11);
    GET_WORD(data,  fbr->sec_num_32,     0x13);
    GET_BYTE(data,  fbr->media_desc,     0x15);
    GET_WORD(data,  fbr->sec_per_fat,    0x16);
    GET_WORD(data,  fbr->sec_per_track,  0x18);
    GET_WORD(data,  fbr->heads_num,      0x1A);
    GET_DWORD(data, fbr->hsec_num,       0x1C);
    GET_DWORD(data, fbr->sec_num,        0x20);
    GET_WORD(data,  fbr->ldrv_num,       0x24);
    GET_BYTE(data,  fbr->ext_sig,        0x26);
    GET_DWORD(data, fbr->ser_num,        0x27);
    GET_BYTES(data, fbr->vol_name, 11,   0x2B);
    GET_BYTES(data, fbr->fat_name, 8,    0x36);

    // Skip the exe code and read the end marker
    len = 0x02;
    err = cyg_blib_read(&disk->blib, (void*)data, &len, 0, 0x1FE);
    if (err != ENOERR)
        return err;

    GET_BYTES(data, fbr->exe_marker, 2,  0);

    // Zero terminate strings
    fbr->oem_name[8]  = '\0';
    fbr->vol_name[11] = '\0';
    fbr->fat_name[8]  = '\0';
  
#if TFT 
    print_boot_record(fbr);
#endif
    
    return ENOERR;
}

//==========================================================================
// FAT table entry functions 

// -------------------------------------------------------------------------
// read_tentry()
// Reads FAT table entry from disk.

static int
read_tentry_fat12(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    unsigned char data[2];
    cyg_uint32 pos, num3;
    cyg_uint16 e;
    int len, err;

    num3 = num * 3;
    pos  = disk->fat_tbl_pos + (num3 >> 1);
    len  = 2;
    
    err = cyg_blib_read(&disk->blib, (void*)data, &len, 0, pos);
    if (err != ENOERR)
        return err;

    GET_WORD(data, e, 0x00);

    if (0 == (num3 & 1))
        *entry = e & 0x0FFF;
    else
        *entry = (e >> 4) & 0x0FFF;
        
    CYG_TRACE3(TFT, "tentry=%x num=%d at %d", *entry, num, pos);
    
    return ENOERR;
}

static int
read_tentry_fat16(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    unsigned char data[2];
    cyg_uint32 pos;
    cyg_uint16 e;
    int len, err;

    pos = disk->fat_tbl_pos + (num << 1);
    len = 2;
    
    err = cyg_blib_read(&disk->blib, (void*)data, &len, 0, pos);
    if (err != ENOERR)
        return err;

    GET_WORD(data, e, 0x00);
    *entry = e;

    CYG_TRACE3(TFT, "tentry=%x num=%d at %d", *entry, num, pos);
    
    return ENOERR;
}

static int
read_tentry(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    switch (disk->fat_type)
    {
        case FATFS_FAT12: return read_tentry_fat12(disk, num, entry);
        case FATFS_FAT16: return read_tentry_fat16(disk, num, entry);
        default: return EINVAL;                  
    }
}

// -------------------------------------------------------------------------
// write_tentry()
// Writes FAT table entry to disk (to all copies of FAT).
 
static int
write_tentry_fat12(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    unsigned char data[2];
    cyg_uint32 pos, num3; 
    cyg_uint16 e;
    int i, len, err;

    num3 = num * 3;
    pos  = disk->fat_tbl_pos + (num3 >> 1);
    len  = 2;
   
    err = cyg_blib_read(&disk->blib, (void*)data, &len, 0, pos);
    if (err != ENOERR)
        return err;

    GET_WORD(data, e, 0x00);
  
    if (0 == (num3 & 1)) 
        e = (e & 0xF000) | (*entry & 0x0FFF);
    else
        e = (e & 0x000F) | ((*entry & 0x0FFF) << 4);
    
    SET_WORD(data, e, 0x00);

    for (i = 0; i < disk->fat_tbls_num; i++)
    {
        err = cyg_blib_write(&disk->blib, (void*)data, &len, 0, pos);
        if (err != ENOERR)
            return err;

        CYG_TRACE4(TFT, "tentry=%x num=%d at %d tbl=%d", *entry, num, pos, i);

        pos += disk->fat_tbl_size;
    }
    
    return ENOERR;
}

static int
write_tentry_fat16(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    unsigned char data[2];
    cyg_uint32 pos; 
    cyg_uint16 e;
    int i, len, err;

    pos = disk->fat_tbl_pos + (num << 1);
    len = 2;
    
    e = *entry;
    SET_WORD(data, e, 0x00);

    for (i = 0; i < disk->fat_tbls_num; i++)
    {
        err = cyg_blib_write(&disk->blib, (void*)data, &len, 0, pos);
        if (err != ENOERR)
            return err;

        CYG_TRACE4(TFT, "tentry=%x num=%d at %d tbl=%d", *entry, num, pos, i);

        pos += disk->fat_tbl_size;
    }
    
    return ENOERR;
}

static int
write_tentry(fatfs_disk_t *disk, cyg_uint32 num, cyg_uint32 *entry)
{
    switch (disk->fat_type)
    {
        case FATFS_FAT12: return write_tentry_fat12(disk, num, entry);
        case FATFS_FAT16: return write_tentry_fat16(disk, num, entry);
        default: return EINVAL;                  
    }
}

// -------------------------------------------------------------------------
// get_tentry_type()
// Gets the type of FAT table entry.
 
static int
get_tentry_type_fat12(fatfs_disk_t *disk, cyg_uint32 entry)
{
    int type;

    if (entry < 0x0FF0)
    {
        if (0x0000 == entry)  type = TENTRY_FREE;
        else                  type = TENTRY_REGULAR;
    }
    else if (entry >= 0x0FF8) type = TENTRY_LAST;
    else if (0x0FF7 == entry) type = TENTRY_BAD;
    else                      type = TENTRY_RESERVED;

#if TFT
    CYG_TRACE2(TFT, "tentry=%04X type=%s", entry, tentry_type_name[type]);
#endif

    return type;
}

static int
get_tentry_type_fat16(fatfs_disk_t *disk, cyg_uint32 entry)
{
    int type;

    if (entry < 0xFFF0)
    {
        if (0x0000 == entry)  type = TENTRY_FREE;
        else                  type = TENTRY_REGULAR;
    }
    else if (entry >= 0xFFF8) type = TENTRY_LAST;
    else if (0xFFF7 == entry) type = TENTRY_BAD;
    else                      type = TENTRY_RESERVED;

#if TFT
    CYG_TRACE2(TFT, "tentry=%04X type=%s", entry, tentry_type_name[type]);
#endif

    return type;
}

static int
get_tentry_type(fatfs_disk_t *disk, cyg_uint32 entry)
{
    switch (disk->fat_type)
    {
        case FATFS_FAT12: return get_tentry_type_fat12(disk, entry);
        case FATFS_FAT16: return get_tentry_type_fat16(disk, entry);
        default: return TENTRY_BAD;
    }
}

// -------------------------------------------------------------------------
// set_tentry_type()
// Sets the type of FAT table entry.
 
static void 
set_tentry_type_fat12(fatfs_disk_t *disk, cyg_uint32 *entry, cyg_uint32 type)
{
    switch (type)
    {
        case TENTRY_FREE:     *entry = 0x0000; break;
        case TENTRY_LAST:     *entry = 0x0FF8; break;
        case TENTRY_RESERVED: *entry = 0x0FF0; break;
        case TENTRY_BAD:      *entry = 0x0FF7; break;      
        default:
            CYG_ASSERT(false, "Unknown tentry type");
    }
}

static void 
set_tentry_type_fat16(fatfs_disk_t *disk, cyg_uint32 *entry, cyg_uint32 type)
{
    switch (type)
    {
        case TENTRY_FREE:     *entry = 0x0000; break;
        case TENTRY_LAST:     *entry = 0xFFF8; break;
        case TENTRY_RESERVED: *entry = 0xFFF0; break;
        case TENTRY_BAD:      *entry = 0xFFF7; break;      
        default:
            CYG_ASSERT(false, "Unknown tentry type");
    }
}

static void 
set_tentry_type(fatfs_disk_t *disk, cyg_uint32 *entry, cyg_uint32 type)
{
    switch (disk->fat_type)
    {
        case FATFS_FAT12: set_tentry_type_fat12(disk, entry, type);
        case FATFS_FAT16: set_tentry_type_fat16(disk, entry, type);
    }
}

// -------------------------------------------------------------------------
// get_tentry_next_cluster()
// Gets the the next file cluster number from FAT table entry.
 
static __inline__ cyg_uint32 
get_tentry_next_cluster(fatfs_disk_t *disk, cyg_uint32 entry)
{
    return entry;
}

// -------------------------------------------------------------------------
// set_tentry_next_cluster()
// Sets the the next cluster number to FAT table entry.
 
static __inline__ void 
set_tentry_next_cluster(fatfs_disk_t *disk, 
                        cyg_uint32   *entry, 
                        cyg_uint32    next_cluster)
{
    *entry = next_cluster;
}

//==========================================================================
// FAT cluster functions 

// -------------------------------------------------------------------------
// is_pos_inside_cluster()
// Checks if the given position is inside cluster size.

static __inline__ bool 
is_pos_inside_cluster(fatfs_disk_t *disk, cyg_uint32 pos)
{
    return (pos < disk->cluster_size);
}

// -------------------------------------------------------------------------
// erase_cluster()
// Erases cluster (fills with 0x00). 

static int
erase_cluster(fatfs_disk_t *disk, cyg_uint32 cluster)
{
    unsigned char data[32];
    cyg_uint32 apos;
    int err, len, i;
    
    len = 32;
    memset((void*)data, 0x00, len);
    apos = get_data_disk_apos(disk, cluster, 0);
    
    CYG_TRACE1(TCL, "cluster=%d", cluster);

    for (i = 0; i < (disk->cluster_size >> 5); i++)
    {
        err = cyg_blib_write(&disk->blib, (void*)data, &len, 0, apos);
        if (err != ENOERR)
            return err;
        apos += len;
    }
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// mark_cluster()
// Marks cluster (sets the cluster's FAT table entry to given type). 

static int
mark_cluster(fatfs_disk_t *disk, cyg_uint32 cluster, cyg_uint32 type)
{
    cyg_uint32 tentry;
    int err;
 
    set_tentry_type(disk, &tentry, type);
    err = write_tentry(disk, cluster, &tentry);

    CYG_TRACE3(TCL, "cluster=%d type=%d tentry=%d", 
                    cluster, type, tentry);
    return err;
}

// -------------------------------------------------------------------------
// link_cluster()
// Links two clusters.

static int
link_cluster(fatfs_disk_t *disk, cyg_uint32 cluster1, cyg_uint32 cluster2)
{
    cyg_uint32 tentry;
    int err;
    
    set_tentry_next_cluster(disk, &tentry, cluster2);
    err = write_tentry(disk, cluster1, &tentry);
 
    CYG_TRACE3(TCL, "cluster1=%d cluster2=%d tentry=%d", 
                    cluster1, cluster2, tentry);
    return err;
}

// -------------------------------------------------------------------------
// find_next_free_cluster()
// Finds first free cluster starting from given cluster.
// If none is available free_cluster is set to 0.
// If mark_as_last is set the found cluster is marked as LAST.

static int
find_next_free_cluster(fatfs_disk_t  *disk,
                       cyg_uint32     start_cluster, 
                       cyg_uint32    *free_cluster,
                       cluster_opts_t opts)
{
    cyg_uint32 c, tentry;
    int err;

    if (start_cluster < 2)
        c = 2;
    else
        c = start_cluster + 1;

    *free_cluster = 0;

    CYG_TRACE1(TCL, "c=%d", c);
   
    // Search from the starting cluster to the end of FAT and
    // from start of FAT to the starting cluster 
    while (c != start_cluster)
    {
        // End of FAT check
        if (c >= disk->fat_tbl_nents)
        {
            c = 2;
            if (c >= start_cluster)
                break;
        }

        err = read_tentry(disk, c, &tentry);
        if (err != ENOERR)
            return err;

        if (TENTRY_FREE == get_tentry_type(disk, tentry))
        {
            CYG_TRACE1(TCL, "free_cluster=%d", c);
            *free_cluster = c;
            if (opts & CO_MARK_LAST)
                err = mark_cluster(disk, c, TENTRY_LAST);
            if ((err == ENOERR) && (opts & CO_ERASE_NEW))
                err = erase_cluster(disk, c);
            return err;
        }
        c++;
    }   

    // No free clusters found
    CYG_TRACE0(TCL, "!!! no free clusters found");
 
    // Return out of space
    return ENOSPC;
}

// -------------------------------------------------------------------------
// find_and_append_cluster()
// Finds a free cluster on disk and appends it to the given cluster. 
// New cluster is marked as LAST. 

static int
find_and_append_cluster(fatfs_disk_t  *disk, 
                        cyg_uint32     cluster, 
                        cyg_uint32    *new_cluster,
                        cluster_opts_t opts)
{
    cyg_uint32 free_cluster;
    int err;

    CYG_TRACE1(TCL, "cluster=%d", cluster);
    
    err = find_next_free_cluster(disk, cluster, 
            &free_cluster, opts | CO_MARK_LAST);
    if (err != ENOERR)
        return err;

    CYG_TRACE1(TCL, "free_cluster=%d", free_cluster);
    
    // Link clusters    
    err = link_cluster(disk, cluster, free_cluster);
    if (err != ENOERR)
        return err;

    *new_cluster = free_cluster;

    CYG_TRACE1(TCL, "new_cluster=%d", free_cluster);
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// find_nth_cluster0()
// Finds nth cluster in chain (ie nth cluster of file).
// Searching from given position.
 
static int
find_nth_cluster0(fatfs_disk_t     *disk,
                  fatfs_data_pos_t *dpos, 
                  cyg_uint32        n,
                  fatfs_tcache_t   *tcache)
{
    cyg_uint32 cluster, cluster_snum;
    int err = ENOERR;
 
    // Trivial case check
    if (dpos->cluster_snum == n)
        return ENOERR;

    // First look in cache
    if (NULL != tcache)
    {
        cyg_uint32 c, ln;
        if (fatfs_tcache_get(disk, tcache, n, &c))
        {
            // Cluster in cache
            dpos->cluster = c;
            dpos->cluster_snum = n;
            CYG_TRACE2(TCL, "cluster=%d cluster_snum=%d in cache", c, n);
            return ENOERR;
        }
        else if (fatfs_tcache_get_last(disk, tcache, &ln, &c))
        {
            // Cluster not in cache - get last
            // in cache and search from there
            dpos->cluster = c;
            dpos->cluster_snum = ln;  
            CYG_TRACE2(TCL, "cluster=%d cluster_snum=%d last in cache", c, ln);
        }
        else
        {
            // Empty cache - put first cluster in
            fatfs_tcache_set(disk, tcache, dpos->cluster_snum, dpos->cluster);
        }
    }
   
    cluster      = dpos->cluster;
    cluster_snum = dpos->cluster_snum;
   
    CYG_TRACE4(TCL, "cluster=%d cluster_snum=%d n=%d n_to_search=%d",
                    cluster, cluster_snum, n, n-cluster_snum);
   
    // Adjust the number of clusters that should be
    // walked according to the given position
    n -= cluster_snum;

    // Walk the cluster chain for n clusters or
    // until last cluster
    while (n > 0)
    {
        cyg_uint32 tentry;

        err = read_tentry(disk, cluster, &tentry);
        if (err != ENOERR)
            return err;

        switch (get_tentry_type(disk, tentry))
        {
            case TENTRY_REGULAR:
                break;
            case TENTRY_LAST:
                // Oops early last cluster
                CYG_TRACE1(TCL, "chain end n=%d", n);
                err = EEOF; // File has less clusters than given n
                            // this err should be caught by the 
                            // calling function 
                goto out;
            default:
                // Inconsistant FAT table state !!!
                CYG_TRACE2(TCL, "!!! inconsistant FAT tentry=%x n=%d", 
                                tentry, n);
                err = EIO;                 
                goto out;
        }
        cluster = get_tentry_next_cluster(disk, tentry);
        cluster_snum++;
        if (NULL != tcache)
            fatfs_tcache_set(disk, tcache, cluster_snum, cluster);
        n--;
    }
    
out:
    dpos->cluster      = cluster;
    dpos->cluster_snum = cluster_snum;

    CYG_TRACE2(TCL, "nth cluster=%d cluster_snum=%d", cluster, cluster_snum);
    return err;
}

// -------------------------------------------------------------------------
// find_nth_cluster()
// Finds nth cluster in chain (ie nth cluster of file) searching 
// from given position. If the chain ends one cluster before the 
// given nth cluster and the CO_EXTEND is specifide, than the 
// chain is extended by one cluster.
 
static int
find_nth_cluster(fatfs_disk_t     *disk,
                 fatfs_data_pos_t *dpos, 
                 cyg_uint32        n,
                 fatfs_tcache_t   *tcache,
                 cluster_opts_t    opts)
{
    int err;
   
    // Find nth cluster 
    err = find_nth_cluster0(disk, dpos, n, tcache);    

    // EEOF meens that the cluster chain ended early
    if ((err != EEOF) || !(opts & CO_EXTEND))
        return err;
    
    CYG_TRACE2(TCL, "cluster_snum=%d n=%d", dpos->cluster_snum, n);
    
    // Check if one cluster short
    if (dpos->cluster_snum == (n - 1))
    {
        // Extend the chain for one cluster
        cyg_uint32 new_cluster;

        // Append new cluster to the end of chain
        err = find_and_append_cluster(disk, dpos->cluster, 
                                      &new_cluster, opts);
        if (err != ENOERR)
            return err;

        // Update position
        dpos->cluster = new_cluster;
        dpos->cluster_snum++;
        dpos->cluster_pos = 0;

        CYG_TRACE2(TCL, "cluster=%d cluster_snum=%d", 
                        dpos->cluster, dpos->cluster_snum);
        
        // Update cache
        if (NULL != tcache)
            fatfs_tcache_set(disk, tcache, dpos->cluster_snum, dpos->cluster);
    }
    
    return err;
}

// -------------------------------------------------------------------------
// get_next_cluster()
// Gets next cluster in chain (ie next cluster of file).
// If CO_EXTEND is specified and the current cluster is last in 
// chain then the chain is extended by one cluster.

static int
get_next_cluster(fatfs_disk_t     *disk,
                 fatfs_data_pos_t *dpos,
                 fatfs_tcache_t   *tcache,
                 cluster_opts_t    opts)
{
    int err;

    CYG_TRACE2(TCL, "cluster=%d cluster_snum=%d", 
                    dpos->cluster, dpos->cluster_snum);

    err = find_nth_cluster(disk, dpos, dpos->cluster_snum + 1, tcache, opts);

    if (err != ENOERR)
        return err;

    // Update position
    dpos->cluster_pos = 0;
 
    CYG_TRACE2(TCL, "cluster=%d cluster_snum=%d", 
                    dpos->cluster, dpos->cluster_snum);
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// get_data_position_from_off()
// Gets data position from given file offset.
// If CO_EXTEND is specified the file is extended if 
// one cluster too short. 
 
static int 
get_data_position_from_off(fatfs_disk_t     *disk,
                           cyg_uint32        first_cluster, 
                           cyg_uint32        offset, 
                           fatfs_data_pos_t *dpos,
                           fatfs_tcache_t   *tcache,
                           cluster_opts_t    opts)
{
    cyg_uint32 n;
    int err;

    // Position inside the cluster
    dpos->cluster_pos = offset & (disk->cluster_size - 1);

    // Cluster seq number to be searched for
    n = offset >> disk->cluster_size_log2;

    // Start searching from first cluster
    dpos->cluster      = first_cluster;
    dpos->cluster_snum = 0;

    CYG_TRACE4(TCL, "off=%d first_cluster=%d cluster_pos=%d n=%d\n",
                    offset, first_cluster, dpos->cluster_pos, n);

    err = find_nth_cluster(disk, dpos, n, tcache, opts);

    // Err could be EEOF wich means that the given 
    // offset if out of given file (cluster chain)

    return err;
}

// -------------------------------------------------------------------------
// free_cluster_chain()
// Marks all clusters FREE from given cluster to the last cluster in chain.

static int
free_cluster_chain(fatfs_disk_t *disk, cyg_uint32 start_cluster)
{
    cyg_uint32 c, next_c, tentry;
    bool last;
    int err;

    CYG_TRACE1(TCL, "start_cluster=%d", start_cluster);

    c = next_c = start_cluster;
    last = false;
    while (!last)
    {
        err = read_tentry(disk, c, &tentry);
        if (err != ENOERR)
            return err;

        switch (get_tentry_type(disk, tentry))
        {
            case TENTRY_LAST:
                // Last cluster in chain
                last = true;
                break;
            case TENTRY_REGULAR:
                // Get next cluster in chain
                next_c = get_tentry_next_cluster(disk, tentry);
                break;
            default:
                CYG_TRACE2(TCL, "!!! inconsistant FAT tentry=%x c=%d", 
                                tentry, c);
                return EIO;
        }

        // Set the current tentry to FREE 
        set_tentry_type(disk, &tentry, TENTRY_FREE);
        err = write_tentry(disk, c, &tentry);
        if (err != ENOERR)
            return err;

        // Next cluster in chain
        c = next_c; 
    }

    CYG_TRACE1(TCL, "last_cluster=%d", c);
    
    return ENOERR;
}

//==========================================================================
// FAT dir entry functions 

// -------------------------------------------------------------------------
// print_dentry()
// Prints FAT directory entry. 

#if TDE
static void
print_dentry(fat_dir_entry_t* fde)
{
    if (DENTRY_IS_DELETED(fde))
        diag_printf("FAT: FDE name:    '?%.7s'\n", &fde->name[1]);
    else    
        diag_printf("FAT: FDE name:    '%.8s'\n", fde->name);
    diag_printf("FAT: FDE ext:     '%.3s'\n", fde->ext);
    diag_printf("FAT: FDE attr:     %c%c%c%c%c%c\n", 
                (DENTRY_IS_RDONLY(fde)  ? 'R' : '-'),
                (DENTRY_IS_HIDDEN(fde)  ? 'H' : '-'),
                (DENTRY_IS_SYSTEM(fde)  ? 'S' : '-'),
                (DENTRY_IS_VOLUME(fde)  ? 'V' : '-'),
                (DENTRY_IS_DIR(fde)     ? 'D' : '-'),
                (DENTRY_IS_ARCHIVE(fde) ? 'A' : '-'));
    diag_printf("FAT: FDE crt time: %u\n", fde->crt_time);
    diag_printf("FAT: FDE crt date: %u\n", fde->crt_date);
    diag_printf("FAT: FDE acc date: %u\n", fde->acc_date);
    diag_printf("FAT: FDE wrt time: %u\n", fde->wrt_time);
    diag_printf("FAT: FDE wrt date: %u\n", fde->wrt_date);
    diag_printf("FAT: FDE cluster:  %u\n", fde->cluster);
    diag_printf("FAT: FDE size:     %u\n", fde->size);
}
#endif // TDE

// -------------------------------------------------------------------------
// read_dentry()
// Reads dir entry from disk. 
// If cluster is 0 reads from root dir.
 
static int
read_dentry(fatfs_disk_t     *disk,
            fatfs_data_pos_t *dpos, 
            fat_dir_entry_t  *fde)
{
    unsigned char data[DENTRY_SIZE];
    cyg_uint32 apos;
    int len, err;
   
    // Check if we are reading the root directory
    if (0 == dpos->cluster)
        apos = disk->fat_root_dir_pos + dpos->cluster_pos;
    else
        apos = get_data_disk_apos(disk, dpos->cluster, dpos->cluster_pos);    
   
    CYG_TRACE3(TDE, "cluster=%d pos=%d apos=%d",
                    dpos->cluster, dpos->cluster_pos, apos); 
    
    len = DENTRY_SIZE;
    err = cyg_blib_read(&disk->blib, (void*)data, &len, 0, apos);
    if (err != ENOERR)
        return err;

    GET_BYTES(data, fde->name,     8, 0x00);
    GET_BYTES(data, fde->ext,      3, 0x08);
    GET_BYTE(data,  fde->attr,        0x0B);
    GET_BYTE(data,  fde->nt_reserved, 0x0C);
    GET_BYTE(data,  fde->crt_sec_100, 0x0D);
    GET_WORD(data,  fde->crt_time,    0x0E);
    GET_WORD(data,  fde->crt_date,    0x10);
    GET_WORD(data,  fde->acc_date,    0x12);
    GET_WORD(data,  fde->cluster_HI,  0x14);
    GET_WORD(data,  fde->wrt_time,    0x16);
    GET_WORD(data,  fde->wrt_date,    0x18);
    GET_WORD(data,  fde->cluster,     0x1A);
    GET_DWORD(data, fde->size,        0x1C);

     // Zero terminate strings
    fde->name[8] = '\0';    
    fde->ext[3]  = '\0';    
  
    // Store position
    fde->pos = *dpos;
       
#if TDE    
    print_dentry(fde);
#endif

    return ENOERR;
}

// -------------------------------------------------------------------------
// write_dentry()
// Writes dir entry to disk. 
// If cluster is 0 writes to root dir.
 
static int
write_dentry(fatfs_disk_t     *disk,
             fatfs_data_pos_t *dpos, 
             fat_dir_entry_t  *fde)
{
    unsigned char data[DENTRY_SIZE];
    cyg_uint32 apos;
    int len, err;
    
    // Check if we are writting to the root directory
    if (0 == dpos->cluster)
        apos = disk->fat_root_dir_pos + dpos->cluster_pos;
    else
        apos = get_data_disk_apos(disk, dpos->cluster, dpos->cluster_pos);    
 
    CYG_TRACE3(TDE, "cluster=%d pos=%d apos=%d",
                    dpos->cluster, dpos->cluster_pos, apos); 

    SET_BYTES(data, fde->name,     8, 0x00);
    SET_BYTES(data, fde->ext,      3, 0x08);
    SET_BYTE(data,  fde->attr,        0x0B);
    SET_BYTE(data,  fde->nt_reserved, 0x0C);
    SET_BYTE(data,  fde->crt_sec_100, 0x0D);
    SET_WORD(data,  fde->crt_time,    0x0E);
    SET_WORD(data,  fde->crt_date,    0x10);
    SET_WORD(data,  fde->acc_date,    0x12);
    SET_WORD(data,  fde->cluster_HI,  0x14);
    SET_WORD(data,  fde->wrt_time,    0x16);
    SET_WORD(data,  fde->wrt_date,    0x18);
    SET_WORD(data,  fde->cluster,     0x1A);
    SET_DWORD(data, fde->size,        0x1C);
   
    len = DENTRY_SIZE;
    err = cyg_blib_write(&disk->blib, (void*)data, &len, 0, apos);
    if (err != ENOERR)
        return err;

#if TDE    
    print_dentry(fde);
#endif

    return ENOERR;
}

// -------------------------------------------------------------------------
// dentry_set_deleted()
// Sets the dentry filename first char to 0xE5 (ie deleted). 
 
static __inline__ void 
dentry_set_deleted(fatfs_disk_t *disk, fat_dir_entry_t *dentry)
{
    dentry->name[0] = 0xE5;
}

// -------------------------------------------------------------------------
// get_dentry_filename()
// Gets the filename from given dir entry. 
 
static void 
get_dentry_filename(fat_dir_entry_t *fde, char *name)
{
    int i = 0;
    char *cptr  = fde->name;
    char *cname = name;

    while (*cptr != ' ' && i < 8)
    {
        *cname++ = *cptr++; i++;
    }
    cptr = fde->ext;

    if (*cptr != ' ')
    {
        *cname++ = '.'; i = 0;
        while (*cptr != ' ' && i < 3)
        {
            *cname++ = *cptr++; i++;
        }
    }
    *cname = '\0';

    CYG_TRACE3(TDE, "dos_name='%s' dos_ext='%s' filename='%s'",
                    fde->name, fde->ext, name);
}

// -------------------------------------------------------------------------
// set_dentry_filename()
// Sets the filename to given dir entry. 
 
static void 
set_dentry_filename(fat_dir_entry_t *fde, const char *name, int namelen)
{
    int i, nidx;
    const char *cname;
    char *cptr;

    // Special case check
    if (name[0] == '.')
    {
        if (name[1] == '\0')
        {
            strcpy(fde->name, ".       ");
            strcpy(fde->ext,  "   ");
            return;
        }
        else if (name[1] == '.' && name[2] == '\0')
        {
            strcpy(fde->name, "..      ");
            strcpy(fde->ext,  "   ");
            return;
        }
    }
    
    if (0 == namelen)
        namelen = 9999;
    
    nidx  = 0;
    cname = name;
    cptr  = fde->name;
    for (i = 0; i < 8; i++)
    {
        if (*cname != '.' && *cname != '\0' && nidx++ < namelen)
            *cptr++ = toupper(*cname++);
        else
            *cptr++ = ' ';
    }
    *cptr = '\0';
    
    while (*cname != '.' && *cname != '\0' && nidx++ < namelen)
        cname++;
   
    if (*cname == '.' && nidx++ < namelen) 
        cname++;
    
    cptr = fde->ext;
    for (i = 0; i < 3; i++)
    {
        if (*cname != '.' && *cname != '\0' && nidx++ < namelen)
            *cptr++ = toupper(*cname++);
        else
            *cptr++ = ' ';
    }
    *cptr = '\0';

    CYG_TRACE4(TDE, "filename='%s' namelen=%d dos_name='%s' dos_ext='%s'", 
                    name, namelen, fde->name, fde->ext);
}

// -------------------------------------------------------------------------
// get_next_dentry()
// Gets next dir entry searching from given position to the end.
// Position is expected in DENTRY_SIZE units.
// If EEOF is returned there are no more extries in given dir.
 
static int
get_next_dentry(fatfs_disk_t    *disk,
                fatfs_node_t    *dir,
                cyg_uint32      *pos,
                fat_dir_entry_t *dentry)
{
    fatfs_data_pos_t dpos;
    cyg_uint32 off;
    int err = ENOERR;

    // Calculate dentry offset
    off = *pos * DENTRY_SIZE;
    
    CYG_TRACE4(TDE, "pos=%d off=%d dir=%p cluster=%d",
                    off, *pos, dir, dir->cluster);

    // Root dir check
    if (0 == dir->cluster)
    {
        dpos.cluster      = 0;
        dpos.cluster_snum = 0;
        dpos.cluster_pos  = off;
    }
    else
    {
        err = get_data_position_from_off(disk, dir->cluster, 
            off, &dpos, &dir->tcache, false);
        if (err != ENOERR)
            return err;
    }

    while (true)
    {
        // Root dir check
        if (0 != dir->cluster) 
        {
            // Change cluster if needed
            if (!is_pos_inside_cluster(disk, dpos.cluster_pos))
                err = get_next_cluster(disk, &dpos, &dir->tcache, CO_NONE);
        }
        else
        {
            if (*pos >= disk->fat_root_dir_nents)
                err = EEOF;
        }

        if (err != ENOERR)
            break;

        err = read_dentry(disk, &dpos, dentry);
        if (err != ENOERR)
            return err;

        if (DENTRY_IS_ZERO(dentry))
        {
            // If we get a ZERO dir entry, we assume that
            // there are no more entries in current dir
            CYG_TRACE0(TDE, "ZERO dentry"); 
            err = EEOF;
            break;
        }
        else if (!DENTRY_IS_DELETED(dentry))
        {
            // Dir entry found
            CYG_TRACE3(TDE, "dentry_pos=%d cluster=%d pos=%d",
                            *pos, dpos.cluster, dpos.cluster_pos);
            break;
        }

        // Increment offset and position
        dpos.cluster_pos += DENTRY_SIZE;
        (*pos)++;
    }

    if (EEOF == err) CYG_TRACE0(TDE, "end of dir");
            
    // EEOF could be returned if there are no more entries in this
    // dir - this should be cought by the calling function 
    return err;
}

// -------------------------------------------------------------------------
// get_free_dentry()
// Gets free dir entry slot searching from given position.
// If an deleated entry is found, its clusters are freed and the
// entry is reused. 
// The directory is extended if needed.

static int
get_free_dentry(fatfs_disk_t     *disk, 
                fatfs_data_pos_t *dpos,
                fatfs_tcache_t   *tcache)
{
    fat_dir_entry_t  dentry;
    fatfs_data_pos_t cdpos;
    int err = ENOERR;
    
    CYG_TRACE3(TDE, "cluster=%d cluster_snum=%d cluster_pos=%d", 
                    dpos->cluster, dpos->cluster_snum, dpos->cluster_pos);
    cdpos = *dpos;

    while (true)
    {
        // Root dir check
        if (0 != cdpos.cluster) 
        {
            // Change cluster if needed
            if (!is_pos_inside_cluster(disk, cdpos.cluster_pos))
                err = get_next_cluster(disk, &cdpos, tcache, 
                                       CO_EXTEND | CO_ERASE_NEW);
        }
        else
        {
            if (cdpos.cluster_pos >= disk->fat_root_dir_size)
                err = ENOSPC;
        }

        if (err != ENOERR)
            return err;

        err = read_dentry(disk, &cdpos, &dentry);
        if (err != ENOERR)
            return err;

        if (DENTRY_IS_DELETED(&dentry))
        {
            CYG_TRACE3(TDE, "deleted dentry at cluster=%d cluster_snum=%d "
                            "cluster_pos=%d", cdpos.cluster, cdpos.cluster_snum,
                            cdpos.cluster_pos);

            // Retrun found dentry position
            *dpos = cdpos;
            return ENOERR;
        }
        else if (DENTRY_IS_ZERO(&dentry))
        {
            CYG_TRACE3(TDE, "zero dentry at cluster=%d cluster_snum=%d "
                            "cluster_pos=%d", cdpos.cluster, cdpos.cluster_snum,
                            cdpos.cluster_pos);

            // Retrun found dentry position
            *dpos = cdpos;
            return ENOERR;  
        }
       
        // Increment current position
        cdpos.cluster_pos += DENTRY_SIZE;
    }
}
 
// -------------------------------------------------------------------------
// dentry_to_node()
// Converts FAT dir entry to node. 
 
static void
dentry_to_node(fat_dir_entry_t *dentry, 
               fatfs_node_t    *node)
{
    get_dentry_filename(dentry, node->filename);

    if (DENTRY_IS_DIR(dentry))
        node->mode = __stat_mode_DIR;
    else
        node->mode = __stat_mode_REG;
    
    date_dos2unix(dentry->crt_time, dentry->crt_date, &node->ctime);
    date_dos2unix(0,                dentry->acc_date, &node->atime);
    date_dos2unix(dentry->wrt_time, dentry->wrt_date, &node->mtime);
    
    node->size       = dentry->size;
    node->priv_data  = dentry->nt_reserved;
    node->cluster    = dentry->cluster;
    node->dentry_pos = dentry->pos;
}

// -------------------------------------------------------------------------
// node_to_dentry()
// Converts node to FAT dir entry. 
 
static void
node_to_dentry(fatfs_node_t *node, fat_dir_entry_t *dentry)
{
    set_dentry_filename(dentry, node->filename, 0);

    if (node->mode == __stat_mode_DIR)
        dentry->attr = DENTRY_ATTR_DIR;
    else
        dentry->attr = DENTRY_ATTR_ARCHIVE;
        
    date_unix2dos(node->ctime, &dentry->crt_time, &dentry->crt_date);
    date_unix2dos(node->atime, NULL,              &dentry->acc_date);
    date_unix2dos(node->mtime, &dentry->wrt_time, &dentry->wrt_date);
    
    dentry->crt_sec_100 = 0; //FIXME
    dentry->size        = node->size;
    dentry->nt_reserved = node->priv_data;
    dentry->cluster     = node->cluster;
    dentry->cluster_HI  = 0;
    dentry->pos         = node->dentry_pos;
}

//==========================================================================
// FAT data functions 

// -------------------------------------------------------------------------
// read_data()
// Reads data from given position. 
 
static int
read_data(fatfs_disk_t     *disk,
          void             *data,
          cyg_uint32       *len,
          fatfs_data_pos_t *dpos,
          fatfs_tcache_t   *tcache)
{
    unsigned char *buf;
    cyg_uint32 apos;
    cyg_uint32 size;
    int err;

    // Initialize variables and get the absolute starting pos on disk
    buf   = (unsigned char *)data;
    size  = *len;
    apos  = get_data_disk_apos(disk, dpos->cluster, dpos->cluster_pos);
    err   = ENOERR;
  
    CYG_TRACE5(TDO, "len=%d cluster=%d cluster_pos=%d "
                    "cluster_snum=%d apos=%d", *len, dpos->cluster, 
                    dpos->cluster_pos, dpos->cluster_snum, apos);

    while (size > 0)
    {
        cyg_uint32 csize;

        // Check if we are still inside current cluster
        if (!is_pos_inside_cluster(disk, dpos->cluster_pos))
        {
            // Get next cluster of file and adjust absolute disk position 
            err = get_next_cluster(disk, dpos, tcache, CO_NONE);
            if (err != ENOERR)
                goto out;
            apos = get_data_disk_apos(disk, dpos->cluster, dpos->cluster_pos);
        }
        
        // Adjust the data chunk size to be read to the cluster boundary
        if (size > (disk->cluster_size - dpos->cluster_pos))
            csize = disk->cluster_size - dpos->cluster_pos;
        else
            csize = size;

        CYG_TRACE5(TDO, "-- len=%d cluster=%d cluster_pos=%d "
                        "cluster_snum=%d apos=%d", csize, dpos->cluster,
                        dpos->cluster_pos, dpos->cluster_snum, apos);

        // Read data chunk 
        err = cyg_blib_read(&disk->blib, (void*)buf, &csize, 0, apos);

        if (err != ENOERR)
            goto out;

        // Adjust running variables
        buf               += csize;
        dpos->cluster_pos += csize;
        apos              += csize;
        size              -= csize;    
    }
    
out:
    *len -= size;

    CYG_TRACE1(TDO, "total len=%d", *len);

    return err;
}

// -------------------------------------------------------------------------
// write_data()
// Writes data to given position. 
 
static int
write_data(fatfs_disk_t     *disk,
           void             *data,
           cyg_uint32       *len,
           fatfs_data_pos_t *dpos,
           fatfs_tcache_t   *tcache)
{
    unsigned char *buf;
    cyg_uint32 apos;
    cyg_uint32 size;
    int err;

    // Initialize variables and get the absolute starting pos on disk
    buf   = (unsigned char *)data;
    size  = *len;
    apos  = get_data_disk_apos(disk, dpos->cluster, dpos->cluster_pos);
    err   = ENOERR;
  
    CYG_TRACE5(TDO, "len=%d cluster=%d cluster_pos=%d "
                    "cluster_snum=%d apos=%d", *len, dpos->cluster, 
                    dpos->cluster_pos, dpos->cluster_snum, apos);

    while (size > 0)
    {
        cyg_uint32 csize;

        // Check if we are still inside current cluster
        if (!is_pos_inside_cluster(disk, dpos->cluster_pos))
        {
            // Get next cluster of file and adjust absolute disk position,
            // if at the last cluster try to extend the cluster chain 
            err = get_next_cluster(disk, dpos, tcache, CO_EXTEND);
            if (err != ENOERR)
                goto out;
            apos = get_data_disk_apos(disk, dpos->cluster, dpos->cluster_pos);
        }
        
        // Adjust the data chunk size to be read to the cluster boundary
        if (size > (disk->cluster_size - dpos->cluster_pos))
            csize = disk->cluster_size - dpos->cluster_pos;
        else
            csize = size;
 
        CYG_TRACE5(TDO, "-- len=%d cluster=%d cluster_pos=%d "
                        "cluster_snum=%d apos=%d", csize, dpos->cluster,
                        dpos->cluster_pos, dpos->cluster_snum, apos);

        // Write data chunk 
        err = cyg_blib_write(&disk->blib, (void*)buf, &csize, 0, apos);

        if (err != ENOERR)
            goto out;

        // Adjust running variables
        buf               += csize;
        dpos->cluster_pos += csize;
        apos              += csize;
        size              -= csize;    
    }
    
out:
    *len -= size;

    CYG_TRACE1(TDO, "total len=%d", *len);

    return err;
}

//==========================================================================
// Misc functions 

// -------------------------------------------------------------------------
// init_node()
// Initializes file attributes of a new node. 
 
static void
init_node_fattr(fatfs_node_t     *node, 
                const char       *name, 
                int               namelen,
                mode_t            mode,
                cyg_uint32        parent_cluster, 
                fatfs_data_pos_t *dentry_pos)
{
    if (namelen == 0)
        namelen = 12;
    
    strncpy(node->filename, name, namelen);
    node->filename[namelen] = '\0';
    
    node->mode  = mode;
    node->ctime = 
    node->atime =
    node->mtime = cyg_timestamp();

    node->priv_data      = 0;
    node->size           = 0;
    node->cluster        = 0;
    node->parent_cluster = parent_cluster;
    node->dentry_pos     = *dentry_pos;
}

// -------------------------------------------------------------------------
// is_node_root_node()
// Check if the given node is the root dir node 
 
static __inline__ bool
is_node_root_node(fatfs_node_t *node)
{
    return (node->filename[0] == '\0' && node->cluster == 0);
}

//==========================================================================
//==========================================================================
// Exported functions 

// -------------------------------------------------------------------------
// fatfs_get_disk_info()
// Gets disk info. 
 
int
fatfs_get_disk_info(fatfs_disk_t *disk)
{
    int err;
    cyg_uint32 sec_num, root_dir_sec_num;
    cyg_uint32 data_sec_num, data_clu_num;
    fat_boot_record_t boot_rec;

    CYG_CHECK_DATA_PTRC(disk);
 
    err = read_boot_record(disk, &boot_rec);
    if (err != ENOERR)    
        return err;

    // Check some known boot record values
    if (0x29 != boot_rec.ext_sig       ||
        0x55 != boot_rec.exe_marker[0] ||
        0xAA != boot_rec.exe_marker[1])
        return EINVAL;

    // Determine number of sectors
    if (boot_rec.sec_num_32 != 0)
        sec_num = boot_rec.sec_num_32;
    else
        sec_num = boot_rec.sec_num;
    
    // Number of sectors used by root directory 
    root_dir_sec_num = ((boot_rec.max_root_dents * DENTRY_SIZE) +
                        (boot_rec.bytes_per_sec - 1)) / boot_rec.bytes_per_sec;
        
    // Number of data sectors
    data_sec_num = sec_num - (boot_rec.res_sec_num + 
        (boot_rec.fat_tbls_num * boot_rec.sec_per_fat) + root_dir_sec_num);
    
    // Number of data clusters
    data_clu_num = data_sec_num / boot_rec.sec_per_clust;

    // Determine the type of FAT based on number of data clusters
    if (data_clu_num < 4085) 
        disk->fat_type = FATFS_FAT12;
    else if (data_clu_num < 65525) 
        disk->fat_type = FATFS_FAT16;
    else // FAT32
        return EINVAL;
    
    // Sector and cluster sizes 
    disk->sector_size       = boot_rec.bytes_per_sec;
    disk->sector_size_log2  = get_val_log2(disk->sector_size);
    disk->cluster_size      = boot_rec.bytes_per_sec * boot_rec.sec_per_clust;
    disk->cluster_size_log2 = get_val_log2(disk->cluster_size);

    // Sector and cluster size should always be a power of 2
    if (0 == disk->sector_size_log2 || 0 == disk->cluster_size_log2)
        return EINVAL;

    // FAT table and root dir sizes and position
    disk->fat_tbl_pos        = boot_rec.bytes_per_sec * boot_rec.res_sec_num;
    disk->fat_tbl_size       = boot_rec.bytes_per_sec * boot_rec.sec_per_fat;   
    disk->fat_tbl_nents      = data_clu_num + 2;
    disk->fat_tbls_num       = boot_rec.fat_tbls_num;
    disk->fat_root_dir_pos   = disk->fat_tbl_pos + 
                               disk->fat_tbls_num * disk->fat_tbl_size;
    disk->fat_root_dir_size  = boot_rec.max_root_dents * DENTRY_SIZE;
    disk->fat_root_dir_nents = boot_rec.max_root_dents;
    disk->fat_data_pos       = disk->fat_root_dir_pos + disk->fat_root_dir_size;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_get_root_node()
// Gets root dir node. 

void
fatfs_get_root_node(fatfs_disk_t *disk, fatfs_node_t *node)
{
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(node);
    
    node->mode           = __stat_mode_DIR;
    node->size           = disk->fat_root_dir_size;
    node->ctime          = 0;
    node->atime          = 0;
    node->mtime          = 0;
    node->filename[0]    = '\0';
    node->cluster        = 0;
    node->parent_cluster = 0;

    node->dentry_pos.cluster      = 0;
    node->dentry_pos.cluster_snum = 0;
    node->dentry_pos.cluster_pos  = 0;
}

// -------------------------------------------------------------------------
// fatfs_is_node_root_node()
// Check if the given node is root dir node. 

bool
fatfs_is_node_root_node(fatfs_node_t *node)
{
    return(is_node_root_node(node));
}

// -------------------------------------------------------------------------
// fatfs_get_dir_entry_node()
// Gets dir entry node at given position.
// If there is no dir entry at given position the next closest
// dir entry and its position are returned. 
// If EEOF error is returned, then there are no more dir
// entries in given dir. 

int
fatfs_get_dir_entry_node(fatfs_disk_t *disk,
                         fatfs_node_t *dir,
                         cyg_uint32   *pos,
                         fatfs_node_t *node)
{
    fat_dir_entry_t dentry;
    int err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir);
    CYG_CHECK_DATA_PTRC(pos);
    CYG_CHECK_DATA_PTRC(node);
 
    err = get_next_dentry(disk, dir, pos, &dentry);
  
    if (err != ENOERR)
       return err;

    dentry_to_node(&dentry, node);
    node->parent_cluster = dir->cluster;

    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_write_node()
// Writes node attributes to its dir entry (to disk).
 
int
fatfs_write_file_attr(fatfs_disk_t *disk, fatfs_node_t *node)
{
    fat_dir_entry_t dentry;
    int err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(node);
 
    node_to_dentry(node, &dentry);
    err = write_dentry(disk, &dentry.pos, &dentry);
    return err;
}

// -------------------------------------------------------------------------
// fatfs_delete_file()
// Marks file dir entry as deleted.
 
int
fatfs_delete_file(fatfs_disk_t *disk, fatfs_node_t *node)
{
    fat_dir_entry_t dentry;
    int err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(node);

    // Can't delete root
    if (is_node_root_node(node))
        return EINVAL;
    
    CYG_TRACE1(TDE, "filename='%s'", node->filename);

    node_to_dentry(node, &dentry);

    // Check if we are about to delete a directory
    if (DENTRY_IS_DIR(&dentry))
    {
        fat_dir_entry_t cdentry;
        cyg_uint32 pos = 0;
        int i = 0;
        
        CYG_TRACE0(TDE, "got directory");
        // Count number of entries in this dir    
        while (true)
        {
            err = get_next_dentry(disk, node, &pos, &cdentry);

            if (EEOF == err)
                break;
            else if (err != ENOERR)
                return err;
            i++; pos++;
        }
        CYG_TRACE1(TDE, "child count=%d", i);
        
        // Check if the dir is empty (except '.' and '..')
        if (i > 2)
            return EPERM; 
    }    
    
    // Free file clusters
    free_cluster_chain(disk, dentry.cluster);
    dentry_set_deleted(disk, &dentry);
    err = write_dentry(disk, &dentry.pos, &dentry);
    return err;
} 

// -------------------------------------------------------------------------
// fatfs_create_file()
// Creates a new file.
 
int
fatfs_create_file(fatfs_disk_t *disk, 
                  fatfs_node_t *dir, 
                  const char   *name,
                  int           namelen,
                  fatfs_node_t *node)
{
    fat_dir_entry_t dentry;
    int err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir);
    CYG_CHECK_DATA_PTRC(name);
    CYG_CHECK_DATA_PTRC(node);
 
    CYG_TRACE2(TDE, "filename='%s' parent='%s'", name, dir->filename);

    dentry.pos.cluster      = dir->cluster;
    dentry.pos.cluster_snum = 0;
    dentry.pos.cluster_pos  = 0;

    // Get free dir entry in parent dir
    err = get_free_dentry(disk, &dentry.pos, &dir->tcache);
    if (err != ENOERR)
        return err;

    // Set new file attributes
    init_node_fattr(node, name, namelen, __stat_mode_REG, 
                    dir->cluster, &dentry.pos); 

    // Write new dir dentry  
    node_to_dentry(node, &dentry);
    err = write_dentry(disk, &dentry.pos, &dentry);
    if (err != ENOERR)
       return err;
 
    return ENOERR;     
}

// -------------------------------------------------------------------------
// fatfs_create_dir()
// Creates a new directory.
 
int
fatfs_create_dir(fatfs_disk_t *disk, 
                 fatfs_node_t *dir, 
                 const char   *name,
                 int           namelen,
                 fatfs_node_t *node)
{
    fat_dir_entry_t dentry;
    fatfs_node_t    cnode;
    cyg_uint32 free_cluster;
    int err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir);
    CYG_CHECK_DATA_PTRC(name);
    CYG_CHECK_DATA_PTRC(node);
 
    CYG_TRACE2(TDE, "filename='%s' parent='%s'", name, dir->filename);

    // Get free cluster
    err = find_next_free_cluster(disk, 0, &free_cluster, 
                                 CO_MARK_LAST | CO_ERASE_NEW);
    if (err != ENOERR)
        return err;

    dentry.pos.cluster      = dir->cluster;
    dentry.pos.cluster_snum = 0;
    dentry.pos.cluster_pos  = 0;

    // Get free dir entry in parent dir
    err = get_free_dentry(disk, &dentry.pos, &dir->tcache);
    if (err != ENOERR)
        return err;

    // Set new dir attributes
    init_node_fattr(node, name, namelen, __stat_mode_DIR, 
                    dir->cluster, &dentry.pos); 
    node->cluster = free_cluster;

    // Write new dir dentry  
    node_to_dentry(node, &dentry);
    err = write_dentry(disk, &dentry.pos, &dentry);
    if (err != ENOERR)
       return err;

    // Starting position of new dir entries
    dentry.pos.cluster      = node->cluster;
    dentry.pos.cluster_snum = 0;
    dentry.pos.cluster_pos  = 0;

    CYG_TRACE0(TDE, "Creating '.' entry");
    // Set '.' dir attributes
    init_node_fattr(&cnode, ".", 0, __stat_mode_DIR, 
                    node->cluster, &dentry.pos); 
    cnode.cluster = node->cluster;

    // Write '.' dentry
    node_to_dentry(&cnode, &dentry); 
    err = write_dentry(disk, &dentry.pos, &dentry);
    if (err != ENOERR)
        return err;
          
    dentry.pos.cluster_pos += DENTRY_SIZE;

    CYG_TRACE0(TDE, "Creating '..' entry");
    // Set '..' dir attributes
    init_node_fattr(&cnode, "..", 0, __stat_mode_DIR,
                    node->cluster, &dentry.pos); 
    cnode.cluster = dir->cluster;
    
    // Write '..' dentry
    node_to_dentry(&cnode, &dentry); 
    err = write_dentry(disk, &dentry.pos, &dentry);
    if (err != ENOERR)
        return err;
  
    return ENOERR;     
}

// -------------------------------------------------------------------------
// fatfs_trunc_file()
// Truncates a file to zero length.

int
fatfs_trunc_file(fatfs_disk_t *disk, fatfs_node_t *node)
{
    fat_dir_entry_t dentry;
    int err;
    
    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(node);
 
    CYG_TRACE1(TDE, "file='%s'", node->filename);
  
    // Check for dir
    if (S_ISDIR(node->mode))
        return EINVAL; 

    // Trivial case check
    if (0 == node->size)
        return ENOERR;
   
    // Free cluster chain 
    err = free_cluster_chain(disk, node->cluster);
    if (err != ENOERR)
        return err;
    
    // Flush tcache
    fatfs_tcache_flush(disk, &node->tcache);
    
    // Update node attributes
    node->cluster = 0;
    node->size    = 0;
    node->mtime   =
    node->atime   = cyg_timestamp();

    // Write dentry
    node_to_dentry(node, &dentry);
    err = write_dentry(disk, &dentry.pos, &dentry);
    if (err != ENOERR)
        return err;
    
    return ENOERR;
}

// -------------------------------------------------------------------------
// fatfs_rename_file()
// Renames a file.
 
int
fatfs_rename_file(fatfs_disk_t *disk, 
                  fatfs_node_t *dir1, 
                  fatfs_node_t *node,
                  fatfs_node_t *dir2, 
                  const char   *name,
                  int           namelen)
{
    fat_dir_entry_t  dentry;
    fatfs_data_pos_t dpos;
    int err;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(dir1);
    CYG_CHECK_DATA_PTRC(node);
    CYG_CHECK_DATA_PTRC(dir2);
    CYG_CHECK_DATA_PTRC(name);
 
    // Can't rename root 
    if (is_node_root_node(node))
        return EINVAL;
 
    CYG_TRACE5(TDE, "dir1=%p '%s' to dir2=%p '%s' (%d)", 
                    dir1, node->filename, dir2, name, namelen);

    strncpy(node->filename, name, namelen);
    node->filename[namelen] = '\0';
   
    // Moving around in same dir
    if (dir1 == dir2)
    {
        CYG_TRACE0(TDE, "same dir"); 
        node_to_dentry(node, &dentry);
        err = write_dentry(disk, &dentry.pos, &dentry);
        return err;
    }
    
    CYG_TRACE0(TDE, "different dirs"); 
    
    // Moveing around in different dirs

    dpos.cluster      = dir2->cluster;
    dpos.cluster_snum = 0;
    dpos.cluster_pos  = 0;

    CYG_TRACE0(TDE, "writting to new dir"); 

    // Get free dir entry in target dir
    err = get_free_dentry(disk, &dpos, &dir2->tcache);
    if (err != ENOERR)
        return err;

    // Write node dentry to new location
    node_to_dentry(node, &dentry);
    err = write_dentry(disk, &dpos, &dentry);
    if (err != ENOERR)
        return err;
   
    CYG_TRACE0(TDE, "deleting from old dir"); 
    
    // Deleate dentry at old location
    dentry_set_deleted(disk, &dentry);
    dentry.size    = 0;
    dentry.cluster = 0;
    err = write_dentry(disk, &dentry.pos, &dentry);
    if (err != ENOERR)
        return err;
   
    // Set node new position and parent cluster
    node->dentry_pos     = dpos;
    node->parent_cluster = dir2->cluster;
 
    // If we moved a directory, we also have to correct the '..' entry  
    if (__stat_mode_DIR == node->mode)
    {
        fat_dir_entry_t cdentry;
        cyg_uint32 pos = 0;
        
        CYG_TRACE0(TDE, "moving directory - correcting '..' entry");

        while (true)
        {
            err = get_next_dentry(disk, node, &pos, &cdentry);

            if (EEOF == err)
                return EIO; // This dir doesn't have the '..' entry,
                            // that means something is very wrong
            else if (err != ENOERR)
                return err;

            if (0 == strncmp("..", cdentry.name, 2))
            {
                cdentry.cluster = dir2->cluster;
                err = write_dentry(disk, &cdentry.pos, &cdentry);
                if (err != ENOERR)
                    return err;
                break;
            }
            pos++;
        }
    }
   
    return ENOERR;     
}

// -------------------------------------------------------------------------
// fatfs_read_data()
// Reads data from disk. 
 
int
fatfs_read_data(fatfs_disk_t *disk,
                fatfs_node_t *node,
                void         *data,
                cyg_uint32   *len,
                cyg_uint32    off)
{
    int err;
    fatfs_data_pos_t dpos;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(node);
    CYG_CHECK_DATA_PTRC(data);
    CYG_CHECK_DATA_PTRC(len);
 
    err = get_data_position_from_off(disk, node->cluster, off, 
                                     &dpos, &node->tcache, CO_NONE);
    if (err != ENOERR)
        return err;

    err = read_data(disk, data, len, &dpos, &node->tcache);
    return err;
}

// -------------------------------------------------------------------------
// fatfs_write_data()
// Writes data to disk. 
 
int
fatfs_write_data(fatfs_disk_t *disk,
                 fatfs_node_t *node,
                 void         *data,
                 cyg_uint32   *len,
                 cyg_uint32    off)
{
    int err;
    fatfs_data_pos_t dpos;

    CYG_CHECK_DATA_PTRC(disk);
    CYG_CHECK_DATA_PTRC(node);
    CYG_CHECK_DATA_PTRC(data);
    CYG_CHECK_DATA_PTRC(len);

    // Check if this file has a zero size and no first cluster
    if (0 == node->size && 0 == node->cluster)
    {
        cyg_uint32 free_cluster;

        CYG_TRACE0(TDO, "new cluster for zero file");
        // Get free cluster
        err = find_next_free_cluster(disk, 0, &free_cluster, CO_MARK_LAST);
        if (err != ENOERR)
        {
            *len = 0;
            return err;
        }
        node->cluster = free_cluster;
    }

    err = get_data_position_from_off(disk, node->cluster, off, 
                                     &dpos, &node->tcache, 
                                     CO_MARK_LAST | CO_EXTEND);
    if (err != ENOERR)
    {
        *len = 0;
        return err;
    }
    
    err = write_data(disk, data, len, &dpos, &node->tcache);
    return err;
}

// -------------------------------------------------------------------------
// Support routines
// These enable the definition of local versions of certain routines
// if the given packages are not present.

#ifndef CYGPKG_LIBC_I18N

__externC int
toupper( int c )
{
    return (('a' <= c) && (c <= 'z')) ? c - 'a' + 'A' : c ;
}

#endif

#ifndef CYGFUN_LIBC_STRING_BSD_FUNCS

__externC int
strcasecmp( const char *s1, const char *s2 )
{
    int ret;
    CYG_REPORT_FUNCNAMETYPE( "strcasecmp", "returning %d" );
    CYG_REPORT_FUNCARG2( "s1=%08x, s2=%08x", s1, s2 );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    while (*s1 != '\0' && toupper(*s1) == toupper(*s2))
    {
        s1++;
        s2++;
    }

    ret = toupper(*(unsigned char *) s1) - toupper(*(unsigned char *) s2);
    CYG_REPORT_RETVAL( ret );
    return ret;
}

__externC int
strncasecmp( const char *s1, const char *s2, size_t n )
{
    int ret;
    CYG_REPORT_FUNCNAMETYPE( "strncasecmp", "returning %d" );
    CYG_REPORT_FUNCARG3( "s1=%08x, s2=%08x, n=%d", s1, s2, n );

    if (n == 0)
    {
        CYG_REPORT_RETVAL(0);
        return 0;
    }

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    while (n-- != 0 && toupper(*s1) == toupper(*s2))
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    ret = toupper(*(unsigned char *) s1) - toupper(*(unsigned char *) s2);
    CYG_REPORT_RETVAL( ret );
    return ret;
}

#endif

// -------------------------------------------------------------------------
// EOF fatfs_supp.c
