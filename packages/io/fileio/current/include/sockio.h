#ifndef CYGONCE_SOCKIO_H
#define CYGONCE_SOCKIO_H
//=============================================================================
//
//      sockio.h
//
//      Socket IO header
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  nickg
// Date:          2000-05-25
// Purpose:       Socket IO header
// Description:   This header contains the external definitions of the general
//                socket IO subsystem for POSIX and EL/IX compatability.
//              
// Usage:
//              #include <sockio.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>

#include <cyg/infra/cyg_type.h>

#include <stddef.h>             // NULL, size_t
#include <limits.h>
#include <sys/types.h>

#include <cyg/fileio/fileio.h>

//=============================================================================
// Forward definitions

struct cyg_nstab_entry;
typedef struct cyg_nstab_entry cyg_nstab_entry;

struct cyg_sock_ops;
typedef struct cyg_sock_ops cyg_sock_ops;

struct sockaddr;
typedef struct sockaddr sockaddr;

struct msghdr;
typedef struct msghdr msghdr;

#ifndef CYGPKG_NET

typedef cyg_uint32      socklen_t;      /* length type for network syscalls */

#endif

//=============================================================================
// network stack entry

struct cyg_nstab_entry
{
    cyg_bool            valid;          // true if stack initialized
    cyg_uint32          syncmode;       // synchronization protocol
    char                *name;          // stack name
    char                *devname;       // hardware device name
    CYG_ADDRWORD        data;           // private data value

    int     (*init)( cyg_nstab_entry *nste );
    int     (*socket)( cyg_nstab_entry *nste, int domain, int type,
		       int protocol, cyg_file *file );
};

#define NSTAB_ENTRY( _l, _syncmode, _name, _devname, _data, _init, _socket )    \
struct cyg_nstab_entry _l CYG_HAL_TABLE_ENTRY(nstab) =                          \
{                                                                               \
    false,                                                                      \
    _syncmode,                                                                  \
    _name,                                                                      \
    _devname,                                                                   \
    _data,                                                                      \
    _init,                                                                      \
    _socket                                                                     \
};

//=============================================================================

struct cyg_sock_ops
{
    int (*bind)      ( cyg_file *fp, const sockaddr *sa, socklen_t len );
    int (*connect)   ( cyg_file *fp, const sockaddr *sa, socklen_t len );
    int (*accept)    ( cyg_file *fp, cyg_file *new_fp,
                       struct sockaddr *name, socklen_t *anamelen );
    int (*listen)    ( cyg_file *fp, int len );
    int (*getname)   ( cyg_file *fp, sockaddr *sa, socklen_t *len, int peer );
    int (*shutdown)  ( cyg_file *fp, int flags );
    int (*getsockopt)( cyg_file *fp, int level, int optname,
                       void *optval, socklen_t *optlen);
    int (*setsockopt)( cyg_file *fp, int level, int optname,
                       const void *optval, socklen_t optlen);
    int (*sendmsg)   ( cyg_file *fp, const struct msghdr *m,
                       int flags, ssize_t *retsize );
    int (*recvmsg)   ( cyg_file *fp, struct msghdr *m,
                       socklen_t *namelen, ssize_t *retsize );
};

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_SOCKIO_H
// End of sockio.h
