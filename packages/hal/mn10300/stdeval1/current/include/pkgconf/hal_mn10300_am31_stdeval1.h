#ifndef CYGONCE_PKGCONF_HAL_MN10300_AM31_STDEVAL1_H
#define CYGONCE_PKGCONF_HAL_MN10300_AM31_STDEVAL1_H
// ====================================================================
//
//      pkgconf/hal_mn10300_am31_stdeval1.h
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
// Author(s):           bartv
// Contributors:        bartv
// Date:                1998-09-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_MN10300_AM31_STDEVAL1 {
       display  "stdeval1 evaluation board"
       type     radio
       parent   CYGPKG_HAL_MN10300
       requires CYGPKG_HAL_MN10300_AM31
       platform stdeval1
       description "
           The stdeval1 HAL package should be used when targetting the
           actual hardware. The same package can also be used when
           running on the full MN10300 simulator, since this provides
           an accurate simulation of the full hardware including I/O
           devices."
   }
   
   cdl_option CYGHWR_HAL_MN10300_AM31_STDEVAL1_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_MN10300_AM31_STDEVAL1
       #type             count
       type             enum
       legal_values     ram rom
       startup
       description      "
           When targetting the stdeval1 board it is possible to build
           the system for either RAM bootstrap or ROM bootstrap. The
           former generally requires that the board is equipped with
           ROMs containing the Cygmon ROM monitor or equivalent
           software that allows gdb to download the eCos application
           on to the board. The latter typically requires that the
           eCos application be blown into EPROMs or equivalent
           technology."
   }
   
   }}CFG_DATA */
#define CYGHWR_HAL_MN10300_AM31_STDEVAL1_STARTUP     ram

// Real-time clock/counter specifics

#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        150000


/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_MN10300_AM31_STDEVAL1_H */
/* EOF hal_mn10300_am31_stdeval1.h */
