#ifndef CYGONCE_PKGCONF_HAL_POWERPC_FADS_H
#define CYGONCE_PKGCONF_HAL_POWERPC_FADS_H
//=============================================================================
//
//      pkgconf/hal_powerpc_fads.h
//
//      HAL configuration file
//
//=============================================================================
// ####UNSUPPORTEDBEGIN####
//
// -------------------------------------------
// This source file has been contributed to eCos/Cygnus. It may have been
// changed slightly to provide an interface consistent with those of other 
// files.
//
// The functionality and contents of this file is supplied "AS IS"
// without any form of support and will not necessarily be kept up
// to date by Cygnus.
//
// All inquiries about this file, or the functionality provided by it,
// should be directed to the 'ecos-discuss' mailing list (see
// http://sourceware.cygnus.com/ecos/intouch.html for details).
//
// Maintained by:  <Unmaintained>
// -------------------------------------------
//
// ####UNSUPPORTEDEND####
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
// Author(s):           bartv
// Contributors:        bartv, khester
// Date:                1998-09-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
//=============================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_POWERPC_FADS {
       display  "Motorola FADS PowerPC evaluation board"
       type     radio
       parent   CYGPKG_HAL_POWERPC
       platform fads
       description "
           The FADS HAL package provides the support needed to run
           eCos on a Motorola FADS board equipped with a PowerPC processor."
   }

   cdl_option CYGHWR_HAL_POWERPC_FADS_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_POWERPC_FADS
       #type             count
       type             enum
       legal_values     ram rom
       startup
       description      "
           When targetting the FADS board for RAM bootstrap it is expected 
           that the image will be downloaded via the Motorola MPC8BUG tool."
   }

   }}CFG_DATA */

#define CYGHWR_HAL_POWERPC_FADS_STARTUP       ram

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_POWERPC_FADS_H */
/* EOF hal_powerpc_FADS.h */
