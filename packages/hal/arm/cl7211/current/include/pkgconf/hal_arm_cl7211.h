#ifndef CYGONCE_PKGCONF_HAL_ARM_CL7211_H
#define CYGONCE_PKGCONF_HAL_ARM_CL7211_H
// ====================================================================
//
//      pkgconf/hal_arm_cl7211.h
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
// Date:                1999-04-21
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_ARM_CL7211 {
       display  "Cirrus Logic CLxx11 evaluation boards"
       type     radio
       parent   CYGPKG_HAL_ARM
       description "
           The CLxx11 HAL package provides the support needed to run
           eCos on a Cirrus Logic CLxx11-1 eval board."
   }

   cdl_option CYGHWR_HAL_ARM_CLxx11_CL7111 {
       display "CL7111-1 variant"
       parent CYGPKG_HAL_ARM_CL7211
       platform cl7111
       type radio
       description "
           Cirrus Logic CL7111 board with ARM710C processor."
   }

   cdl_option CYGHWR_HAL_ARM_CLxx11_CL7211 {
       display "CL7211-1 variant"
       parent CYGPKG_HAL_ARM_CL7211
       platform cl7211
       type radio
       description "
           Cirrus Logic CL7211 board with ARM720 processor."
   }

   cdl_option CYGHWR_HAL_ARM_CL7211_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_ARM_CL7211
       type             enum
       legal_values     ram rom stubs
       startup
       description      "
           When targetting the CLxx11 eval boards it is possible to build
           the system for either RAM bootstrap or ROM bootstrap(s). Select
           'ram' when building programs to load into RAM using onboard
           debug software such as Angel or eCos GDB stubs.  Select 'rom'
           when building a stand-alone application which will be put
           into ROM.  Selection of 'stubs' is for the special case of
           building the eCos GDB stubs themselves."
   }
   
   }}CFG_DATA */

#define CYGHWR_HAL_ARM_CL7211_STARTUP       ram

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_CL7211_DIAG_PORT {
       display          "Diagnostic serial port"
       parent           CYGPKG_HAL_ARM_CL7211
       type             enum
       legal_values     0 1
       description      "
           The CLxx11 boards have two separate serial ports.  This option
           chooses which of these ports will be used."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_CL7211_DIAG_PORT     0

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_CL7211_DIAG_BAUD {
       display          "Diagnostic serial port baud rate"
       parent           CYGPKG_HAL_ARM_CL7211
       type             enum
       legal_values     9600 19200 38400 115200
       description      "
           This option selects the baud rate used for the diagnostic port.
           Note: this should match the value chosen for the GDB port if the
           diagnostic and GDB port are the same."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_CL7211_DIAG_BAUD     38400

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_CL7211_GDB_PORT {
       display          "GDB serial port"
       parent           CYGPKG_HAL_ARM_CL7211
       type             enum
       legal_values     0 1
       description      "
           The CLxx11 boards have two separate serial ports.  This option
           chooses which of these ports will be used to connect to a host
           running GDB."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_CL7211_GDB_PORT     0

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_CL7211_GDB_BAUD {
       display          "GDB serial port baud rate"
       parent           CYGPKG_HAL_ARM_CL7211
       type             enum
       legal_values     9600 19200 38400 115200
       description      "
           This option controls the baud rate used for the GDB connection."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_CL7211_GDB_BAUD     38400

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_CL7211_PROCESSOR_CLOCK {
       display          "Processor clock rate"
       parent           CYGPKG_HAL_ARM_CL7211
       type             enum
       legal_values     18432 36864 49152 73728
       description      "
           The processor on the CL7211 can run at various frequencies."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_CL7211_PROCESSOR_CLOCK 73728

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_CL7211_DRAM_SIZE {
       display          "DRAM size"
       parent           CYGPKG_HAL_ARM_CL7211
       type             enum
       legal_values     2 16
       description      "
           The CLxx11 boards can have various amounts of DRAM installed.
           The machine needs to be initialized differently, depending
           upon the amount installed."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_CL7211_DRAM_SIZE 16

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_option CYGHWR_HAL_ARM_CL7211_LCD_INSTALLED {
       display "LCD panel"
       type    bool
       parent   CYGPKG_HAL_ARM_CL7211
       description "
           If an LCD panel is installed, 128K of DRAM will be dedicated to the
           LCD buffer by the system intialization."
   }

   }}CFG_DATA */
#undef CYGHWR_HAL_ARM_CL7211_LCD_INSTALLED

// Real-time clock/counter specifics

#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        5120   // Assumes 512KHz clock

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_ARM_CL7211_H */
/* EOF hal_arm_cl7211.h */
