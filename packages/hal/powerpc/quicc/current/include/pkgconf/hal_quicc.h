#ifndef CYGONCE_PKGCONF_HAL_QUICC_H
#define CYGONCE_PKGCONF_HAL_QUICC_H
//=============================================================================
//
//      pkgconf/hal_quicc.h
//
//      HAL configuration file
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           hmt
// Contributors:        hmt
// Date:                1999-06-08
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
//=============================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_QUICC {
       display  "Motorola MBX PowerQUICC support"
       type     bool
       parent   CYGPKG_HAL_POWERPC
       description "
           The QUICC package provides some of the support needed
           to run eCos on a Motorola MBX board, using the QUICC
           feature of the MPC860 and MPC821 CPUs.
           Currently only serial IO via SMC1 is provided by
           this package."
   }

   }}CFG_DATA */

/* -------------------------------------------------------------------*/

#endif  /* CYGONCE_PKGCONF_HAL_QUICC_H */
/* EOF hal_quicc.h */
