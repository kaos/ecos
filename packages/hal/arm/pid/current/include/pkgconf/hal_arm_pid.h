#ifndef CYGONCE_PKGCONF_HAL_ARM_PID_H
#define CYGONCE_PKGCONF_HAL_ARM_PID_H
// ====================================================================
//
//      pkgconf/hal_arm_pid.h
//
//      HAL configuration file
//
// ====================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
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

   cdl_package CYGPKG_HAL_ARM_PID {
       display  "ARM PID evaluation board"
       type     radio
       parent   CYGPKG_HAL_ARM
       description "
           The pid HAL package provides the support needed to run
           eCos on an ARM PID eval board."
   }

   cdl_option CYGHWR_HAL_ARM_PID_ARM7 {
       display       "ARM PID evaluation board with ARM7 CPU"
       parent        CYGPKG_HAL_ARM_PID
       platform      pid
       type          radio
       description      "
            The PID board comes with an ARM7 CPU as standard."
   }

   cdl_option CYGHWR_HAL_ARM_PID_ARM9 {
       display       "ARM PID evaluation board with ARM9 CPU"
       parent        CYGPKG_HAL_ARM_PID
       platform      pid9
       type          radio
       description      "
            The PID board can be upgraded to use an ARM9 CPU. This
            option enables extra ARM9 compiler optimizations."
   }

   cdl_option CYGHWR_HAL_ARM_PID_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_ARM_PID
       type             enum
       legal_values     ram rom stubs
       startup
       description      "
           When targetting the PID eval board it is possible to build
           the system for either RAM bootstrap or ROM bootstrap(s). Select
           'ram' when building programs to load into RAM using onboard
           debug software such as Angel or eCos GDB stubs.  Select 'rom'
           when building a stand-alone application which will be put
           into ROM.  Selection of 'stubs' is for the special case of
           building the eCos GDB stubs themselves."
   }
   
   }}CFG_DATA */

#define CYGHWR_HAL_ARM_PID_STARTUP       ram

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_PID_DIAG_PORT {
       display          "Diagnostic serial port"
       parent           CYGPKG_HAL_ARM_PID
       type             enum
       legal_values     0 1
       description      "
           The PID/7T board has two separate serial ports.  This option
           chooses which of these ports will be used."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_PID_DIAG_PORT     0

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_PID_DIAG_BAUD {
       display          "Diagnostic serial port baud rate"
       parent           CYGPKG_HAL_ARM_PID
       type             enum
       legal_values     9600 19200 38400 115200
       description      "
           This option selects the baud rate used for the diagnostic port.
           Note: this should match the value chosen for the GDB port if the
           diagnostic and GDB port are the same."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_PID_DIAG_BAUD     38400

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_PID_GDB_PORT {
       display          "GDB serial port"
       parent           CYGPKG_HAL_ARM_PID
       type             enum
       legal_values     0 1
       description      "
           The PID/7T board has two separate serial ports.  This option
           chooses which of these ports will be used to connect to a host
           running GDB."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_PID_GDB_PORT     0

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_PID_GDB_BAUD {
       display          "GDB serial port baud rate"
       parent           CYGPKG_HAL_ARM_PID
       type             enum
       legal_values     9600 19200 38400 115200
       description      "
           This option controls the baud rate used for the GDB connection."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_PID_GDB_BAUD     38400

// Real-time clock/counter specifics

#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        12500

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_ARM_PID_H */
/* EOF hal_arm_pid.h */
