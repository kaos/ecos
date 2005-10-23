#ifndef CYGONCE_ISO_SYS_TIME_H
#define CYGONCE_ISO_SYS_TIME_H
/*========================================================================
//
//      sys/time.h
//
//      struct timeval and gettimeofday()
//
//========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005  eCosCentric Limited
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Alexander Neundorf <neundorf@kde.org>
// Contributors:  
// Date:          2005-10-04
// Purpose:       This file provides the time macros, types and functions
//                required by ISO C and POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <sys/time.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

/* This is the "standard" way to get NULL and size_t from stddef.h,
 * which is the canonical location of the definitions.
 */
#define __need_NULL
#define __need_size_t
#include <stddef.h>

#include <time.h>
#ifdef CYGBLD_ISO_STRUCTTIMEVAL_HEADER
# include CYGBLD_ISO_STRUCTTIMEVAL_HEADER
#else

/*
 * Structure returned by gettimeofday(2),
 * and used in other calls such as select(2).
 */
struct timeval {
	time_t	tv_sec;		/* seconds */
	time_t	tv_usec;	/* and microseconds */
};

#endif


#endif /* CYGONCE_ISO_SYS_TIME_H multiple inclusion protection */

/* EOF sys/time.h */
