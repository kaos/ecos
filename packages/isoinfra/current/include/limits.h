#ifndef CYGONCE_ISO_LIMITS_H
#define CYGONCE_ISO_LIMITS_H
/*========================================================================
//
//      limits.h
//
//      ISO standard limits
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
// Date:          2000-04-14
// Purpose:       This file provides the limits properties
//                required by ISO C and POSIX 1003.1
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation), as well as
//                being partially provided by the compiler.
//
// Usage:         #include <limits.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* DEFINES */

/*-----------------------------------------------------------------------------
 * Minimum values from POSIX.1 tables 2-3, 2-7 and 2-7a.

 * These are the standard-mandated minimum values.
 * These values do not vary with the implementation - they may
 * simply be defined
 */

/* Minimum number of operations in one list I/O call.  */
#define _POSIX_AIO_LISTIO_MAX	2

/* Minimal number of outstanding asynchronous I/O operations.  */
#define _POSIX_AIO_MAX		1

/* Maximum length of arguments to `execve', including environment.  */
#define	_POSIX_ARG_MAX		4096

/* Maximum simultaneous processes per real user ID.  */
#define	_POSIX_CHILD_MAX	6

/* Minimal number of timer expiration overruns.  */
#define _POSIX_DELAYTIMER_MAX	32

/* Maximum link count of a file.  */
#define	_POSIX_LINK_MAX		8

/* Size of storage required for a login name */
#define _POSIX_LOGIN_NAME_MAX   9

/* Number of bytes in a terminal canonical input queue.  */
#define	_POSIX_MAX_CANON	255

/* Number of bytes for which space will be
   available in a terminal input queue.  */
#define	_POSIX_MAX_INPUT	255

/* Maximum number of message queues open for a process.  */
#define _POSIX_MQ_OPEN_MAX	8

/* Maximum number of supported message priorities.  */
#define _POSIX_MQ_PRIO_MAX	32

/* Number of bytes in a filename.  */
#define	_POSIX_NAME_MAX		14

/* Number of simultaneous supplementary group IDs per process.  */
#define	_POSIX_NGROUPS_MAX	0

/* Number of files one process can have open at once.  */
#define	_POSIX_OPEN_MAX		16

/* Number of bytes in a pathname.  */
#define	_POSIX_PATH_MAX		255

/* Number of bytes than can be written atomically to a pipe.  */
#define	_POSIX_PIPE_BUF		512

/* Minimal number of realtime signals reserved for the application.  */
#define _POSIX_RTSIG_MAX	8

/* Number of semaphores a process can have.  */
#define _POSIX_SEM_NSEMS_MAX	256

/* Maximal value of a semaphore.  */
#define _POSIX_SEM_VALUE_MAX	32767

/* Number of pending realtime signals.  */
#define _POSIX_SIGQUEUE_MAX	32

/* Largest value of a `ssize_t'.  */
#define	_POSIX_SSIZE_MAX	32767

/* Number of streams a process can have open at once.  */
#define	_POSIX_STREAM_MAX	8

/* Controlling the iterations of destructors for thread-specific data.  */
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS	4

/* The number of data keys per process.  */
#define _POSIX_THREAD_KEYS_MAX	128

/* The number of threads per process.  */
#define _POSIX_THREAD_THREADS_MAX	64

/* Maximum number of characters in a tty name.  */
#define	_POSIX_TTY_NAME_MAX	9

/* Number of timer for a process.  */
#define _POSIX_TIMER_MAX	32

/* Maximum length of a timezone name (element of `tzname').  */
#define	_POSIX_TZNAME_MAX	3

/* Maximum clock resolution in nanoseconds.  */
#define _POSIX_CLOCKRES_MIN	20000000


#ifdef CYGBLD_ISO_SSIZET_HEADER
# include CYGBLD_ISO_SSIZET_HEADER
#else
# ifndef __STRICT_ANSI__
#  define SSIZE_MAX LONG_MAX
# endif
#endif

/* INCLUDES */

#ifdef CYGBLD_ISO_OPEN_MAX_HEADER
# include CYGBLD_ISO_OPEN_MAX_HEADER
#else
# ifndef __STRICT_ANSI__
#  define OPEN_MAX _POSIX_OPEN_MAX
# endif
#endif

#ifdef CYGBLD_ISO_LINK_MAX_HEADER
# include CYGBLD_ISO_LINK_MAX_HEADER
#else
# ifndef __STRICT_ANSI__
#  define LINK_MAX _POSIX_LINK_MAX
# endif
#endif

#ifdef CYGBLD_ISO_NAME_MAX_HEADER
# include CYGBLD_ISO_NAME_MAX_HEADER
#else
# ifndef __STRICT_ANSI__
#  define NAME_MAX _POSIX_NAME_MAX
# endif
#endif

#ifdef CYGBLD_ISO_PATH_MAX_HEADER
# include CYGBLD_ISO_PATH_MAX_HEADER
#else
# ifndef __STRICT_ANSI__
#  define PATH_MAX _POSIX_PATH_MAX
# endif
#endif

#if CYGINT_ISO_POSIX_LIMITS
# ifdef CYGBLD_ISO_POSIX_LIMITS_HEADER
#  include CYGBLD_ISO_POSIX_LIMITS_HEADER
# endif
#endif

/* When using a crosscompiler targeting linux, the next limits.h file
   in the include sequence may be the glibc header - which breaks our
   world. So skip it by defining _LIBC_LIMITS_H_ */
#define _LIBC_LIMITS_H_

#include_next <limits.h>

#endif /* CYGONCE_ISO_LIMITS_H multiple inclusion protection */

/* EOF limits.h */
