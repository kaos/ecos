#ifndef CYGONCE_ISO_STAT_H
#define CYGONCE_ISO_STAT_H
/*========================================================================
//
//      stat.h
//
//      POSIX file characteristics
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-05-08
// Purpose:       This file provides the macros, types and functions
//                for file characteristics required by POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <sys/stat.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

#ifdef CYGBLD_ISO_STAT_DEFS_HEADER
# include CYGBLD_ISO_STAT_DEFS_HEADER
#else

#include <sys/types.h>   /* ino_t, dev_t, etc. */
#include <time.h>        /* time_t */

#define __stat_mode_DIR    (1<<0)
#define __stat_mode_CHR    (1<<1)
#define __stat_mode_BLK    (1<<2)
#define __stat_mode_REG    (1<<3)
#define __stat_mode_FIFO   (1<<4)
#define __stat_mode_MQ     (1<<5)
#define __stat_mode_SEM    (1<<6)
#define __stat_mode_SHM    (1<<7)

#define S_ISDIR(__mode)  ((__mode) & __stat_mode_DIR )
#define S_ISCHR(__mode)  ((__mode) & __stat_mode_CHR )
#define S_ISBLK(__mode)  ((__mode) & __stat_mode_BLK )
#define S_ISREG(__mode)  ((__mode) & __stat_mode_REG )
#define S_ISFIFO(__mode) ((__mode) & __stat_mode_FIFO )

#define S_TYPEISMQ(__buf)   ((__buf)->st_mode & __stat_mode_MQ )
#define S_TYPEISSEM(__buf)  ((__buf)->st_mode & __stat_mode_SEM )
#define S_TYPEISSHM(__buf)  ((__buf)->st_mode & __stat_mode_SHM )


#define S_IRUSR  (1<<8)
#define S_IWUSR  (1<<9)
#define S_IXUSR  (1<<10)
#define S_IRWXU  (S_IRUSR|S_IWUSR|S_IXUSR)

#define S_IRGRP  (1<<11)
#define S_IWGRP  (1<<12)
#define S_IXGRP  (1<<13)
#define S_IRWXG  (S_IRGRP|S_IWGRP|S_IXGRP)

#define S_IROTH  (1<<14)
#define S_IWOTH  (1<<15)
#define S_IXOTH  (1<<16)
#define S_IRWXO  (S_IROTH|S_IWOTH|S_IXOTH)

#define S_ISUID  (1<<17)
#define S_ISGID  (1<<18)


struct stat {
    mode_t  st_mode;     /* File mode */
    ino_t   st_ino;      /* File serial number */
    dev_t   st_dev;      /* ID of device containing file */
    nlink_t st_nlink;    /* Number of hard links */
    uid_t   st_uid;      /* User ID of the file owner */
    gid_t   st_gid;      /* Group ID of the file's group */
    off_t   st_size;     /* File size (regular files only) */
    time_t  st_atime;    /* Last access time */
    time_t  st_mtime;    /* Last data modification time */
    time_t  st_ctime;    /* Last file status change time */
}; 

#endif /* ifndef CYGBLD_ISO_STAT_DEFS_HEADER */

/* PROTOTYPES */

__externC int stat( const char *path, struct stat *buf );

__externC int fstat( int fd, struct stat *buf );

__externC int mkdir(const char *path, mode_t mode);

#endif /* CYGONCE_ISO_STAT_H multiple inclusion protection */

/* EOF stat.h */
