//==========================================================================
//
//      socket.cxx
//
//      Fileio socket operations
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
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-05-25
// Purpose:             Fileio socket operations
// Description:         These are the functions that operate on sockets,
//                      such as socket(), bind(), accept() etc.
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>

#include <cyg/hal/hal_tables.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <stdarg.h>                     // for fcntl()

#include <cyg/io/file.h>                // struct iovec

#include "fio.h"                       // Private header

#include <cyg/kernel/mutex.hxx>        // mutex definitions

#ifdef CYGPKG_NET

#include <sys/socket.h>                 // struct msghdr

#endif

//==========================================================================
// Forward definitions

static void cyg_ns_lock( cyg_nstab_entry *ns );
static void cyg_ns_unlock( cyg_nstab_entry *ns );
static void cyg_sock_lock( cyg_file *fp );
static void cyg_sock_unlock( cyg_file *fp );

//==========================================================================
// Local entry/return macros

#define SOCKET_ENTRY() FILEIO_ENTRY()

#define SOCKET_RETURN(err) FILEIO_RETURN(err)

#define SOCKET_RETURN_VALUE(val) FILEIO_RETURN_VALUE(val)

//==========================================================================
// Locking protocols

#define LOCK_NS( _n ) cyg_ns_lock( _n )

#define UNLOCK_NS( _n ) cyg_ns_unlock( _n )

#define LOCK_SOCKET( _fp ) cyg_sock_lock( _fp )

#define UNLOCK_SOCKET( _fp ) cyg_sock_unlock( _fp )

//==========================================================================
// Tables and local variables

// Array of network stacks installed
__externC cyg_nstab_entry nstab[];
CYG_HAL_TABLE_BEGIN( nstab, nstab );

// End of array marker
__externC cyg_nstab_entry nstab_end;
CYG_HAL_TABLE_END( nstab_end, nstab );

Cyg_Mutex nstab_lock[CYGNUM_FILEIO_NSTAB_MAX];

//==========================================================================
// Initialization

__externC void cyg_nstab_init()
{
    cyg_nstab_entry *n;

    for( n = &nstab[0]; n != &nstab_end; n++ )
    {
        // stop if there are more than the configured maximum
        if( n-&nstab[0] >= CYGNUM_FILEIO_NSTAB_MAX )
            break;

        if( n->init( n ) == 0 )
        {
            n->valid = true;
        }
    }
}

//==========================================================================
// Socket API calls

// -------------------------------------------------------------------------

__externC int	socket (int domain, int type, int protocol)
{
    SOCKET_ENTRY();

    int err = 0;
    int fd;
    cyg_file *file;

    fd = cyg_fd_alloc(0);

    if( fd < 0 )
        SOCKET_RETURN(EMFILE);
    
    file = cyg_file_alloc();

    if( file == NULL )
    {
        cyg_fd_free(fd);
        SOCKET_RETURN(ENFILE);
    }

    cyg_nstab_entry *n;

    for( n = &nstab[0]; n != &nstab_end; n++ )
    {
        LOCK_NS( n );
        
        err = n->socket( n, domain, type, protocol, file );

        UNLOCK_NS( n );
        
        if( err == 0 ) break;
    }    

    if( err != 0 )
    {
        cyg_fd_free(fd);
        cyg_file_free(file);        
        SOCKET_RETURN( err );
    }

    file->f_syncmode = n->syncmode;
    file->f_mte = (cyg_mtab_entry *)n;
    
    cyg_fd_assign( fd, file );

    SOCKET_RETURN_VALUE(fd);
}


// -------------------------------------------------------------------------

__externC int	accept (int s, struct sockaddr *sa, socklen_t *addrlen)
{
    SOCKET_ENTRY();

    int err = 0;
    int fd;
    cyg_file *fp;
    cyg_file *new_fp;

    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    fd = cyg_fd_alloc(0);

    if( fd < 0 )
    {
        cyg_fp_free( fp );
        SOCKET_RETURN(EMFILE);
    }
    
    new_fp = cyg_file_alloc();

    if( new_fp == NULL )
    {
        cyg_fp_free( fp );
        cyg_fd_free(fd);
        SOCKET_RETURN(ENFILE);
    }

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        err = ops->accept( fp, new_fp, sa, addrlen );

        UNLOCK_SOCKET( fp );

    }
    else err = EBADF;

    if( err != 0 )
    {
        cyg_fp_free( fp );
        cyg_fd_free(fd);
        cyg_file_free(new_fp);        
        SOCKET_RETURN( err );
    }

    new_fp->f_syncmode = fp->f_syncmode;
    new_fp->f_mte = fp->f_mte;
    
    cyg_fd_assign( fd, new_fp );

    cyg_fp_free( fp );
    
    SOCKET_RETURN_VALUE(fd);
}


// -------------------------------------------------------------------------

__externC int	bind (int s, const struct sockaddr *sa, unsigned int len)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->bind( fp, sa, len );

        UNLOCK_SOCKET( fp );

    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}


// -------------------------------------------------------------------------

__externC int	connect (int s, const struct sockaddr *sa, socklen_t len)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->connect( fp, sa, len );

        UNLOCK_SOCKET( fp );

    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}


// -------------------------------------------------------------------------

__externC int	getpeername (int s, struct sockaddr *sa, socklen_t *len)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->getname( fp, sa, len, 1 );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}


// -------------------------------------------------------------------------

__externC int	getsockname (int s, struct sockaddr *sa, socklen_t *len)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->getname( fp, sa, len, 0 );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}


// -------------------------------------------------------------------------

__externC int	getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->getsockopt( fp, level, optname, optval, optlen );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}


// -------------------------------------------------------------------------

__externC int	listen (int s, int len)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->listen( fp, len );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}


// -------------------------------------------------------------------------

__externC ssize_t recvmsg (int s, struct msghdr *msg, int flags)
{
    SOCKET_ENTRY();

    ssize_t ret = 0;
    int error = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        msg->msg_flags = flags;
        
        error = ops->recvmsg( fp, msg, NULL, &ret );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );

    if( error != ENOERR )
        SOCKET_RETURN(error);

    SOCKET_RETURN_VALUE( ret );
}

// -------------------------------------------------------------------------

__externC ssize_t recvfrom (int s, void *buf, size_t len, int flags,
                          struct sockaddr *from, socklen_t *fromlen)
{
    SOCKET_ENTRY();

    struct msghdr msg;
    struct iovec iov;
    ssize_t ret = 0;
    int error = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        // Set up a message header...
	msg.msg_name = (caddr_t)from;
        msg.msg_namelen = *fromlen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	iov.iov_base = buf;
	iov.iov_len = len;
	msg.msg_control = 0;
	msg.msg_flags = flags;        

        LOCK_SOCKET( fp );

        error = ops->recvmsg( fp, &msg, fromlen, &ret );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );

    if( error != ENOERR )
        SOCKET_RETURN(error);

    SOCKET_RETURN_VALUE( ret );

}

// -------------------------------------------------------------------------

__externC ssize_t recv (int s, void *buf, size_t len, int flags)
{
    return recvfrom( s, buf, len, flags, NULL, NULL );
}

// -------------------------------------------------------------------------

__externC ssize_t sendmsg (int s, const struct msghdr *msg, int flags)
{
    SOCKET_ENTRY();

    ssize_t ret = 0;
    int error = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        error = ops->sendmsg( fp, msg, flags, &ret );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    if( error != ENOERR )
        SOCKET_RETURN(error);

    SOCKET_RETURN_VALUE( ret );
}


// -------------------------------------------------------------------------

__externC ssize_t sendto (int s, const void *buf,
	    size_t len, int flags, const struct sockaddr *to, socklen_t tolen)
{
    SOCKET_ENTRY();

    ssize_t ret = 0;
    int error = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

    
        struct msghdr msg;
        struct iovec iov;

        msg.msg_name = (caddr_t)to;
        msg.msg_namelen = tolen;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = 0;
        msg.msg_flags = 0;
        iov.iov_base = (char *)buf;
        iov.iov_len = len;

        LOCK_SOCKET( fp );

        error = ops->sendmsg( fp, &msg, flags, &ret );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    if( error != ENOERR )
        SOCKET_RETURN(error);

    SOCKET_RETURN_VALUE( ret );    
}

// -------------------------------------------------------------------------

__externC ssize_t send (int s, const void *buf, size_t len, int flags)
{
    return sendto( s, buf, len, flags, NULL, 0 );
}


// -------------------------------------------------------------------------

__externC int	setsockopt (int s, int level, int optname,
                            const void *optval, socklen_t optlen)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->setsockopt( fp, level, optname, optval, optlen );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}

// -------------------------------------------------------------------------

__externC int	shutdown (int s, int how)
{
    SOCKET_ENTRY();

    int ret = 0;
    cyg_file *fp;
    
    fp = cyg_fp_get( s );

    if( fp == NULL )
        FILEIO_RETURN(EBADF);

    if( fp->f_type == CYG_FILE_TYPE_SOCKET )
    {
        cyg_sock_ops *ops = (cyg_sock_ops *)fp->f_xops;

        LOCK_SOCKET( fp );

        ret = ops->shutdown( fp, how );

        UNLOCK_SOCKET( fp );
    }
    
    cyg_fp_free( fp );
    
    SOCKET_RETURN(ret);
}

//==========================================================================
// Locking protocol

static void cyg_ns_lock( cyg_nstab_entry *ns )
{
    if( ns->syncmode & CYG_SYNCMODE_SOCK_NETSTACK )
    {
        nstab_lock[ns-&nstab[0]].lock();
    }
}

static void cyg_ns_unlock( cyg_nstab_entry *ns )
{
    if( ns->syncmode & CYG_SYNCMODE_SOCK_NETSTACK )
    {
        nstab_lock[ns-&nstab[0]].unlock();
    }
}

static void cyg_sock_lock( cyg_file *fp )
{
    cyg_nstab_entry *ns = (cyg_nstab_entry *)fp->f_mte;

    if( fp->f_syncmode & CYG_SYNCMODE_SOCK_NETSTACK )
        nstab_lock[ns-&nstab[0]].lock();

    cyg_file_lock( fp, fp->f_syncmode>>CYG_SYNCMODE_SOCK_SHIFT);
}

static void cyg_sock_unlock( cyg_file *fp )
{
    cyg_nstab_entry *ns = (cyg_nstab_entry *)fp->f_mte;

    if( fp->f_syncmode & CYG_SYNCMODE_SOCK_NETSTACK )
        nstab_lock[ns-&nstab[0]].unlock();

    cyg_file_unlock( fp, fp->f_syncmode>>CYG_SYNCMODE_SOCK_SHIFT);
}


// -------------------------------------------------------------------------
// EOF socket.cxx
