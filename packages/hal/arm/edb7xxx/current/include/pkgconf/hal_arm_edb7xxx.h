#ifndef CYGONCE_PKGCONF_HAL_ARM_EDB7XXX_H
#define CYGONCE_PKGCONF_HAL_ARM_EDB7XXX_H
// ====================================================================
//
//      pkgconf/hal_arm_edb7xxx.h
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
// Date:                1999-04-21
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_ARM_EDB7XXX {
       display  "Cirrus Logic EDB7XXX evaluation boards"
       type     radio
       parent   CYGPKG_HAL_ARM
       description "
           The EDB7XXX HAL package provides the support needed to run
           eCos on a Cirrus Logic EDB7XXX eval boards."
   }

   cdl_option CYGHWR_HAL_ARM_EDB7XXX_CL7111 {
       display "CL7111-1 variant"
       parent CYGPKG_HAL_ARM_EDB7XXX
       platform cl7111
       type radio
       description "
           Cirrus Logic CL7111 board with ARM710C processor."
   }

   cdl_option CYGHWR_HAL_ARM_EDB7XXX_EDB7211 {
       display "EDB7211-1 variant"
       parent CYGPKG_HAL_ARM_EDB7XXX
       platform edb7211
       type radio
       description "
           Cirrus Logic EDB7211 board with EDB7XXX (ARM720) processor."
   }

   cdl_option CYGHWR_HAL_ARM_EDB7XXX_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_ARM_EDB7XXX
       type             enum
       legal_values     ram rom stubs
       startup
       description      "
           When targetting the EDB7XXX eval boards it is possible to build
           the system for either RAM bootstrap or ROM bootstrap(s). Select
           'ram' when building programs to load into RAM using onboard
           debug software such as Angel or eCos GDB stubs.  Select 'rom'
           when building a stand-alone application which will be put
           into ROM.  Selection of 'stubs' is for the special case of
           building the eCos GDB stubs themselves."
   }
   
   }}CFG_DATA */

#define CYGHWR_HAL_ARM_EDB7XXX_STARTUP       ram


/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_EDB7XXX_DIAG_PORT {
       display          "Diagnostic serial port"
       parent           CYGPKG_HAL_ARM_EDB7XXX
       type             enum
       legal_values     0 1
       description      "
           The EDB7XXX boards have two separate serial ports.  This option
           chooses which of these ports will be used."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_EDB7XXX_DIAG_PORT     0

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_EDB7XXX_DIAG_BAUD {
       display          "Diagnostic serial port baud rate"
       parent           CYGPKG_HAL_ARM_EDB7XXX
       type             enum
       legal_values     9600 19200 38400 115200
       description      "
           This option selects the baud rate used for the diagnostic port.
           Note: this should match the value chosen for the GDB port if the
           diagnostic and GDB port are the same."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_EDB7XXX_DIAG_BAUD     38400

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_EDB7XXX_GDB_PORT {
       display          "GDB serial port"
       parent           CYGPKG_HAL_ARM_EDB7XXX
       type             enum
       legal_values     0 1
       description      "
           The EDB7XXX boards have two separate serial ports.  This option
           chooses which of these ports will be used to connect to a host
           running GDB."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_EDB7XXX_GDB_PORT     0

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_EDB7XXX_GDB_BAUD {
       display          "GDB serial port baud rate"
       parent           CYGPKG_HAL_ARM_EDB7XXX
       type             enum
       legal_values     9600 19200 38400 115200
       description      "
           This option controls the baud rate used for the GDB connection."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_EDB7XXX_GDB_BAUD     38400

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_EDB7XXX_PROCESSOR_CLOCK {
       display          "Processor clock rate"
       parent           CYGPKG_HAL_ARM_EDB7XXX
       type             enum
       legal_values     18432 36864 49152 73728
       description      "
           The processor on the EDB7XXX can run at various frequencies."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_EDB7XXX_PROCESSOR_CLOCK 73728

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_EDB7XXX_DRAM_SIZE {
       display          "DRAM size"
       parent           CYGPKG_HAL_ARM_EDB7XXX
       type             enum
       legal_values     2 16
       description      "
           The EDB7XXX boards can have various amounts of DRAM installed.
           The machine needs to be initialized differently, depending
           upon the amount installed."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_ARM_EDB7XXX_DRAM_SIZE 16

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_option CYGHWR_HAL_ARM_EDB7XXX_SOFTWARE_DRAM_REFRESH {
       display "Perform DRAM refresh in software"
       type    bool
       parent   CYGPKG_HAL_ARM_EDB7XXX
       description "
           This option will add code that refreshes the DRAM by
           touching all of DRAM during the system clock interrupt
           processing."
   }

   }}CFG_DATA */

#undef CYGHWR_HAL_ARM_EDB7XXX_SOFTWARE_DRAM_REFRESH

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_option CYGHWR_HAL_ARM_EDB7XXX_LCD_INSTALLED {
       display "LCD panel"
       type    bool
       parent   CYGPKG_HAL_ARM_EDB7XXX
       description "
           If an LCD panel is installed, 128K of DRAM will be dedicated to the
           LCD buffer by the system intialization."
   }

   }}CFG_DATA */
#define CYGHWR_HAL_ARM_EDB7XXX_LCD_INSTALLED

// Real-time clock/counter specifics

#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        5120   // Assumes 512KHz clock

//---------------------------------------------------------------------------
// Platform information - descriptive only

#ifdef __CL7111
#define HAL_PLATFORM_CPU    "ARM 710C"
#define HAL_PLATFORM_BOARD  "CL-PS7111"
#else
#define HAL_PLATFORM_CPU    "ARM 720T"
#define HAL_PLATFORM_BOARD  "EDB7211"
#endif
#define HAL_PLATFORM_EXTRA  ""

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_ARM_EDB7XXX_H */
/* EOF hal_arm_edb7xxx.h */
