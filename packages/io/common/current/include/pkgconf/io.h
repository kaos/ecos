#ifndef CYGONCE_PKGCONF_IO_H
#define CYGONCE_PKGCONF_IO_H
// ====================================================================
//
//      pkgconf/io.h
//
//      Device configuration file
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
// Author(s):   gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     To allow the user to edit device configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_IO {
       display "I/O sub-system"
       type    bool
       requires CYGPKG_ERROR
       description "
           The eCos system is supplied with a number of different
           device drivers.  This option enables the basic I/O system
           support which is the basis for all drivers."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_option CYGDBG_IO_INIT {
       display "Debug I/O sub-system"
       type    bool
       parent   CYGPKG_IO
       description "
           This option enables verbose messages to be displayed on the
           system 'diag' device during I/O system initialization."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */

#undef CYGDBG_IO_INIT

#endif  /* CYGONCE_PKGCONF_IO_H */
/* EOF io.h */
