#ifndef CYGONCE_PKGCONF_HAL_ARM_AEB_H
#define CYGONCE_PKGCONF_HAL_ARM_AEB_H
// ====================================================================
//
//      pkgconf/hal_arm_aeb.h
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
// Author(s):           gthomas
// Contributors:        gthomas
// Date:                1998-12-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_ARM_AEB {
       display  "ARM AEB-1 evaluation board"
       type     radio
       parent   CYGPKG_HAL_ARM
       description "
           The aeb HAL package provides the support needed to run
           eCos on a ARM AEB-1 eval board."
   }

   cdl_option CYGHWR_HAL_ARM_AEB_REVISION_B {
       display       "AEB board revision B"
       parent        CYGPKG_HAL_ARM_AEB
       platform      aeb
       type          radio
       description      "
            The AEB-1 eval board revision B has 128kB of memory and a flawed
            CPU which means caches must be disabled."
   }

   cdl_option CYGHWR_HAL_ARM_AEB_REVISION_C {
       display       "AEB board revision C"
       parent        CYGPKG_HAL_ARM_AEB
       platform      aebC
       type          radio
       description      "
            The AEB-1 eval board revision C has 256kB of memory and can
            run with caches enabled."
   }


   cdl_option CYGHWR_HAL_ARM_AEB_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_ARM_AEB
       #type             count
       type             enum
       legal_values     ram rom stubs
       startup
       description      "
           When targetting the AEB-1 eval board it is possible to build
           the system for either RAM bootstrap or ROM bootstrap(s). Select
           'ram' when building programs to load into RAM using onboard
           debug software such as Angel or eCos GDB stubs.  Select 'rom'
           when building a stand-alone application which will be put
           into ROM.  Selection of 'stubs' is for the special case of
           building the eCos GDB stubs themselves."
   }
   
   }}CFG_DATA */

#define CYGHWR_HAL_ARM_AEB_STARTUP       ram

// Real-time clock/counter specifics

#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        (240000/16)

// Board revision
#ifdef CYG_HAL_ARM_AEBC
#define CYGHWR_HAL_ARM_AEB_REVISION_C
#else
#define CYGHWR_HAL_ARM_AEB_REVISION_B
#endif

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_ARM_AEB_H */
/* EOF hal_arm_aeb.h */
