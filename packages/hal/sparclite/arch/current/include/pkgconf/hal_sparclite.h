#ifndef CYGONCE_PKGCONF_HAL_SPARCLITE_H
#define CYGONCE_PKGCONF_HAL_SPARCLITE_H
// ====================================================================
//
//      pkgconf/hal_sparclite.h
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
// Contributors:        bartv, hmt
// Date:                1998-09-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_SPARCLITE {
       display "SPARClite architecture"
       type    radio
       parent  CYGPKG_HAL
       target  sparclite
       description "
           The SPARClite architecture HAL package provides generic support
           for this processor architecture. It is also necessary to select
           a specific target platform HAL package."
   }

   }}CFG_DATA */

/* -------------------------------------------------------------------*/

#endif  /* CYGONCE_PKGCONF_HAL_SPARCLITE_H */
/* EOF hal_sparclite.h */
