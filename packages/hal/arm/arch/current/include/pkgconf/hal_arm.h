#ifndef CYGONCE_PKGCONF_HAL_ARM_H
#define CYGONCE_PKGCONF_HAL_ARM_H
// ====================================================================
//
//      pkgconf/hal_arm.h
//
//      HAL configuration file
//
// ====================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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

   cdl_package CYGPKG_HAL_ARM {
       display  "ARM architecture"
       type     radio
       parent   CYGPKG_HAL
       target   arm
       description "
           The ARM architecture HAL package provides generic
           support for this processor architecture. It is also
           necessary to select a specific target platform HAL
           package."
   }

   }}CFG_DATA */

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGHWR_HAL_ARM_DUMP_EXCEPTIONS {
       display          "Provide diagnostic dump for exceptions"
       parent           CYGPKG_HAL_ARM
       requires         !CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
       description      "
           Print messages about hardware exceptions, including
           raw exception frame dump and register contents."
   }
   
   }}CFG_DATA */
#undef  CYGHWR_HAL_ARM_DUMP_EXCEPTIONS

/* ---------------------------------------------------------------------
   {{CFG_DATA
   cdl_option CYGIMP_HAL_PROCESS_ALL_EXCEPTIONS {
       display          "Process all exceptions with the eCos application"
       parent           CYGPKG_HAL_ARM
       description      "
           Normal [RAM-based] programs which do not include GDB stubs 
           defer processing of the illegal instruction exception to GDB.
           Setting this options allows the program to explicitly handle
           the illegal instruction exception itself.  Note: this will
           prevent the use of GDB to debug the application as breakpoints
           will no longer work."
   }
   
   }}CFG_DATA */
#undef CYGIMP_HAL_PROCESS_ALL_EXCEPTIONS        


/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_ARM_H */
/* EOF hal_arm.h */
