#ifndef CYGONCE_PKGCONF_HAL_I386_PC_H
#define CYGONCE_PKGCONF_HAL_I386_PC_H
// ====================================================================
//
//      pkgconf/hal_i386_pc.h
//
//      HAL configuration file
//
// ====================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           jskov
// Contributors:        jskov
// Date:                1999-01-12      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// pjo, 27 Sep 1999: Copied from the pc for PC.


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_I386_PC {
       display  "i386 PC Target"
       type     radio
       parent   CYGPKG_HAL_I386
       platform pc
       description "
           The i386 PC Target HAL package provides the 
           support needed to run eCos binaries on an i386 PC."
   }

   }}CFG_DATA */

#define CYGHWR_HAL_I386_PC_STARTUP       ram
#undef  CYGSEM_HAL_I386_PC_REAL_TIME

// Real-time clock/counter specifics

/*
Not really sure exactly how these are used...  but the RTC period is based
	on the clock input to the 8254, which is 1193180 Hz.  CYGNUM_HAL_RTC_PERIOD
	is set for 100 ticks per second.

Note also that the real-time clock stuff is a bit misleading: this only
	configures the interval timer; we have another actual real time clock
	instead.
*/
#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        11932

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_I386_PC_H */
/* EOF hal_i386_pc.h */
