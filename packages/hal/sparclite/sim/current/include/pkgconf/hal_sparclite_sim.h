#ifndef CYGONCE_PKGCONF_HAL_SPARCLITE_SIM_H
#define CYGONCE_PKGCONF_HAL_SPARCLITE_SIM_H
// ====================================================================
//
//      pkgconf/hal_sparclite_sim.h
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

   cdl_package CYGPKG_HAL_SPARCLITE_SIM {
       display  "Minimal simulator"
       type     radio
       parent   CYGPKG_HAL_SPARCLITE
       platform sim
       description "
           The minimal simulator HAL package is provided for when only a
           simple simulation of the processor architecture is desired, as
           opposed to detailed simulation of any specific board. To use
           this minimal simulator the command `target sim -nfp -sparclite
           -dumbio' should be used from inside gdb. It is not possible to
           use any of the eCos device drivers when the simulator is running
           in this mode"
       }
   
   }}CFG_DATA */


/* -------------------------------------------------------------------*/
// This is true for SPARClite:
#define CYGHWR_HAL_SPARCLITE_HAS_ASR17

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_SPARCLITE_SIM_H */
/* EOF hal_sparclite_sim.h */
