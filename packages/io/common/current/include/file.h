//==========================================================================
//
//      io/common/include/file.h
//
//      Defines for high level file I/O
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


#ifndef _CYG_IO_FILE_H_
#define _CYG_IO_FILE_H_

#include <pkgconf/system.h>

//==========================================================================
// If the fileio package is loaded, we need to go through that to do all
// basic IO operations. This code redefines the tags on the structures so
// that they have the names expected by BSD based code.

#ifdef CYGPKG_IO_FILEIO

#include <pkgconf/io_fileio.h>

#define CYG_IOVEC_TAG iovec
#define CYG_UIO_TAG uio
#define CYG_FILEOPS_TAG fileops
#define CYG_FILE_TAG file
#define CYG_SELINFO_TAG selinfo

#include <cyg/fileio/fileio.h>

// File states
#define FREAD      CYG_FREAD
#define FWRITE     CYG_FWRITE
#define FNONBLOCK  CYG_FNONBLOCK
#define FASYNC     CYG_FASYNC

// Type of "file"
#define	DTYPE_VNODE	CYG_FILE_TYPE_FILE	/* file */
#define	DTYPE_SOCKET	CYG_FILE_TYPE_SOCKET	/* communications endpoint */

//==========================================================================
// Otherwise define all the structs here...

#else // CYGPKG_IO_FILEIO

// High-level file I/O interfaces
// Derived [in part] from OpenBSD <sys/file.h>, <sys/uio.h>, <sys/fcntl.h>

#include <pkgconf/io.h>
#include <cyg/infra/cyg_type.h>

#define NFILE CYGPKG_IO_NFILE

struct iovec {
    void           *iov_base;   /* Base address. */
    CYG_ADDRWORD   iov_len;     /* Length. */
};

enum	uio_rw { UIO_READ, UIO_WRITE };

/* Segment flag values. */
enum uio_seg {
    UIO_USERSPACE,		/* from user data space */
    UIO_SYSSPACE		/* from system space */
};

struct uio {
    struct	iovec *uio_iov;	/* pointer to array of iovecs */
    int	uio_iovcnt;	/* number of iovecs in array */
    CYG_ADDRWORD	uio_offset;	/* offset into file this uio corresponds to */
    CYG_ADDRWORD	uio_resid;	/* residual i/o count */
    enum	uio_seg uio_segflg; /* see above */
    enum	uio_rw uio_rw;	/* see above */
};

/*
 * Limits
 */
#define UIO_SMALLIOV	8		/* 8 on stack, else malloc */

// Description of open file
struct file {
    short	f_flag;		/* file state */
    short	f_type;		/* descriptor type */
    struct	fileops {
        int	(*fo_read)(struct file *fp, struct uio *uio);
        int	(*fo_write)(struct file *fp, struct uio *uio);
        int	(*fo_ioctl)(struct file *fp, CYG_ADDRWORD com,
                            CYG_ADDRWORD data);
        int	(*fo_select)(struct file *fp, int which);
        int	(*fo_close)(struct file *fp);
    } *f_ops;
    CYG_ADDRWORD	f_offset;
    CYG_ADDRWORD	f_data;		/* vnode or socket */
};

// File states
#define FREAD      0x01
#define FWRITE     0x02
#define FNONBLOCK  0x10
#define FASYNC     0x20
#define FALLOC     0x80         // File is "busy", i.e. allocated

// Type of "file"
#define	DTYPE_VNODE	1	/* file */
#define	DTYPE_SOCKET	2	/* communications endpoint */
#define	DTYPE_PIPE	3	/* pipe */

externC cyg_bool getfp(int fdes, struct file **fp);
externC int falloc(struct file **fp, int *fd);
externC void ffree(struct file *fp);

//==========================================================================

#endif // CYGPKG_IO_FILEIO

//==========================================================================
#endif // _CYG_IO_FILE_H_
