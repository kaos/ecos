//==========================================================================
//
//      e2fs.h
//
//      Second extended filesystem defines.
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
// Date:         2001-06-25
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef CYGONCE_REDBOOT_E2FS_H
#define CYGONCE_REDBOOT_E2FS_H

//
// Structure of the super block
//
struct e2fs_super_block {
    cyg_uint32  inodes_count;
    cyg_uint32  blocks_count;
    cyg_uint32  r_blocks_count;
    cyg_uint32  free_blocks_count;
    cyg_uint32  free_inodes_count;
    cyg_uint32  first_data_block;
    cyg_uint32  log_block_size;
    cyg_int32   log_frag_size;
    cyg_uint32  blocks_per_group;
    cyg_uint32  frags_per_group;
    cyg_uint32  inodes_per_group;
    cyg_uint32  mtime;
    cyg_uint32  wtime;
    cyg_uint16  mnt_count;
    cyg_int16   max_mnt_count;
    cyg_uint16  magic;
    cyg_uint16  state;
    cyg_uint16  errors;
    cyg_uint16  minor_rev_level;
    cyg_uint32  lastcheck;
    cyg_uint32  checkinterval;
    cyg_uint32  creator_os;
    cyg_uint32  rev_level;
    cyg_uint16  def_resuid;
    cyg_uint16  def_resgid;
};

#define E2FS_PRE_02B_MAGIC	0xEF51
#define E2FS_SUPER_MAGIC	0xEF53

#define E2FS_PTRS_PER_BLOCK(e)   ((e)->blocksize / sizeof(cyg_uint32))

#define E2FS_BLOCK_SIZE(s)	  (E2FS_MIN_BLOCK_SIZE << SWAB_LE32((s)->log_block_size))
#define	E2FS_ADDR_PER_BLOCK(s)	  (E2FS_BLOCK_SIZE(s) / sizeof(unsigned int))
#define E2FS_BLOCK_SIZE_BITS(s)	  (SWAB_LE32((s)->log_block_size) + 10)

#define	E2FS_NR_DIR_BLOCKS	12

#define	E2FS_IND_BLOCK		E2FS_NR_DIR_BLOCKS
#define	E2FS_DIND_BLOCK		(E2FS_IND_BLOCK + 1)
#define	E2FS_TIND_BLOCK		(E2FS_DIND_BLOCK + 1)

#define	E2FS_N_BLOCKS		(E2FS_TIND_BLOCK + 1)


// Structure of an inode on the disk
//
typedef struct e2fs_inode {
    cyg_uint16   mode;
    cyg_uint16   uid;
    cyg_uint32   size;
    cyg_uint32   atime;
    cyg_uint32   ctime;
    cyg_uint32   mtime;
    cyg_uint32   dtime;
    cyg_uint16   gid;
    cyg_uint16   links_count;
    cyg_uint32   blocks;
    cyg_uint32   flags;
    cyg_uint32   reserved1;
    cyg_uint32   block[E2FS_N_BLOCKS];
    cyg_uint32   version;
    cyg_uint32   file_acl;
    cyg_uint32   dir_acl;
    cyg_uint32   faddr;
    cyg_uint8    frag;
    cyg_uint8    fsize;
    cyg_uint16   pad1;
    cyg_uint32   reserved2[2];
} e2fs_inode_t;


#define	E2FS_INODES_PER_BLOCK(e)  ((e)->blocksize / sizeof (struct e2fs_inode))

#define E2FS_MIN_BLOCK_SIZE	  1024
#define	E2FS_MAX_BLOCK_SIZE	  4096

// Special inode numbers
//
#define	E2FS_BAD_INO		 1
#define E2FS_ROOT_INO		 2

typedef struct e2fs_dir_entry {
    cyg_uint32 inode;
    cyg_uint16 reclen;
    cyg_uint8  namelen;
    cyg_uint8  filetype;
    char       name[2];
} e2fs_dir_entry_t;

#define E2FS_FTYPE_UNKNOWN  0
#define E2FS_FTYPE_REG_FILE 1
#define E2FS_FTYPE_DIR      2
#define E2FS_FTYPE_CHRDEV   3
#define E2FS_FTYPE_BLKDEV   4
#define E2FS_FTYPE_FIFO     5
#define E2FS_FTYPE_SOCK     6
#define E2FS_FTYPE_SYMLINK  7

typedef struct e2fs_group
{
    cyg_uint32 block_bitmap;	   // blocks bitmap block
    cyg_uint32 inode_bitmap;	   // inodes bitmap block
    cyg_uint32 inode_table;	   // inodes table block
    cyg_uint16 free_blocks_count;
    cyg_uint16 free_inodes_count;
    cyg_uint16 used_dirs_count;
    cyg_uint16 pad;
    cyg_uint32 reserved[3];
} e2fs_group_t;

#define E2FS_BLOCKS_PER_GROUP(s)  (SWAB_LE32((s)->blocks_per_group))
#define E2FS_INODES_PER_GROUP(s)  (SWAB_LE32((s)->inodes_per_group))

#define	E2FS_GDESC_PER_BLOCK(e)	  ((e)->blocksize / sizeof (struct e2fs_e2fs_group_desc))
#define E2FS_GDESC_PER_SECTOR     (SECTOR_SIZE/sizeof(e2fs_group_t))
#define E2FS_GDESC_CACHE_SIZE     (E2FS_GDESC_PER_SECTOR * 1)
#define E2FS_GDESC_PER_SECTOR     (SECTOR_SIZE/sizeof(e2fs_group_t))

typedef struct e2fs_desc {
    partition_t  *part;     		// partition holding this filesystem
    cyg_uint32   blocksize;		// fs blocksize
    cyg_uint32   ngroups;		// number of groups in fs
    cyg_uint32   blocks_per_group;
    cyg_uint32   inodes_per_group;
    cyg_uint32   gdesc_block;           // block nr of group descriptors
    cyg_int32    gdesc_first;           // which gdesc is first in cache
    e2fs_group_t gdesc_cache[E2FS_GDESC_CACHE_SIZE];
    cyg_uint32   nr_ind_blocks;
    cyg_uint32   nr_dind_blocks;
    cyg_uint32   nr_tind_blocks;
} e2fs_desc_t;

#define E2FS_BLOCK_TO_SECTOR(e,b)  ((b) * ((e)->blocksize / SECTOR_SIZE))

extern fs_funs_t redboot_e2fs_funs;

#endif  // CYGONCE_REDBOOT_E2FS_H
