#ifndef CYGONCE_ISO_SYS_SELECT_H
#define CYGONCE_ISO_SYS_SELECT_H
/*========================================================================
//
//      sys/select.h
//
//      POSIX definitions for select()
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2001-07-26
// Purpose:       This file provides the macros, types and functions
//                required by POSIX 1003.1.
// Description:   Much of the real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <sys/select.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */


/* ------------------------------------------------------------------- */

#if !defined(_POSIX_SOURCE)

#ifdef CYGINT_ISO_SELECT
# ifdef CYGBLD_ISO_SELECT_HEADER
#  include CYGBLD_ISO_SELECT_HEADER
# else

#define	NBBY	8		/* number of bits in a byte */

/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

typedef unsigned int	fd_mask;
#define __NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#ifndef __howmany
#define	__howmany(__x, __y)	(((__x) + ((__y) - 1)) / (__y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[__howmany(FD_SETSIZE, __NFDBITS)];
} fd_set;

#define	FD_SET(__n, __p)   ((__p)->fds_bits[(__n)/__NFDBITS] |= (1 << ((__n) % __NFDBITS)))
#define	FD_CLR(__n, __p)   ((__p)->fds_bits[(__n)/__NFDBITS] &= ~(1 << ((__n) % __NFDBITS)))
#define	FD_ISSET(__n, __p) ((__p)->fds_bits[(__n)/__NFDBITS] & (1 << ((__n) % __NFDBITS)))

#define	FD_COPY(__f, __t)                                       \
{                                                               \
    unsigned int _i;                                            \
    for( _i = 0; _i < __howmany(FD_SETSIZE, __NFDBITS) ; _i++ ) \
        (__t)->fds_bits[_i] = (__f)->fds_bits[_i];              \
}

#define	FD_ZERO(__p)                                            \
{                                                               \
    unsigned int _i;                                            \
    for( _i = 0; _i < __howmany(FD_SETSIZE, __NFDBITS) ; _i++ ) \
        (__p)->fds_bits[_i] = 0;                                \
}

#  ifdef __cplusplus
extern "C" {
#  endif

struct timeval;
extern int
select( int /* nfd */, fd_set * /* in */, fd_set * /* out */,
        fd_set * /* ex */, struct timeval * /* tv */ );
        
#  ifdef __cplusplus
}   /* extern "C" */
#  endif
# endif
#endif

#endif /* if !defined(_POSIX_SOURCE) */
/* ------------------------------------------------------------------- */
#endif /* CYGONCE_ISO_SYS_SELECT_H multiple inclusion protection */

/* EOF sys/select.h */
