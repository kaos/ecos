#ifndef CYGONCE_PKGCONF_HAL_I386_PC_H
#define CYGONCE_PKGCONF_HAL_I386_PC_H
// ====================================================================
//
//      pkgconf/hal_i386_pc.h
//
//      HAL configuration file
//
// ====================================================================
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
