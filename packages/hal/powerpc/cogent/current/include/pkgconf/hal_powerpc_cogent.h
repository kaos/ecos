#ifndef CYGONCE_PKGCONF_HAL_POWERPC_COGENT_H
#define CYGONCE_PKGCONF_HAL_POWERPC_COGENT_H
// ====================================================================
//
//      pkgconf/hal_powerpc_cogent.h
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

   cdl_package CYGPKG_HAL_POWERPC_COGENT {
       display  "Cogent PowerPC evaluation board"
       type     radio
       parent   CYGPKG_HAL_POWERPC
       platform cogent
       description "
           The cogent HAL package provides the support needed to run
           eCos on a Cogent board equipped with a PowerPC processor."
   }

   cdl_option CYGHWR_HAL_POWERPC_COGENT_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_POWERPC_COGENT
       #type             count
       type             enum
       legal_values     ram rom stubs
       startup
       description      "
           When targetting the cogent board it is possible to build
           the system for either RAM bootstrap, ROM bootstrap, or STUB
           bootstrap. RAM bootstrap generally requires that the board
           is equipped with ROMs containing a suitable ROM monitor or
           equivalent software that allows GDB to download the eCos
           application on to the board. The ROM bootstrap typically
           requires that the eCos application be blown into EPROMs or
           equivalent technology. STUB bootstrap is for configurations
           of eCos used only as GDB stub ROMs; the use of memory is
           limited to the top 1MB."
  }
  
#### There will be a flood of PRs if the config tool is allowed to fiddle
#### with these two... *sigh*
##  cdl_option CYGHWR_HAL_POWERPC_COGENT_GDB_PORT {
##      display          "GDB Serial Port"
##      parent           CYGPKG_HAL_POWERPC_COGENT
##      type             enum
##      legal_values     1 0
##      description      "
##          The Cogent board has two separate serial ports.  This option
##          chooses which of these ports will be used by the GDB stub. 
##          On the CMA101 board, 0 and 1 are the P11 and P12
##          connectors respectively. On the CMA102 board, 0 and 1 are
##          the P2 and P3 connectors respectively."
##  }
##
##  cdl_option CYGHWR_HAL_POWERPC_COGENT_DIAG_PORT {
##      display          "Diag Serial Port"
##      parent           CYGPKG_HAL_POWERPC_COGENT
##      type             enum
##      legal_values     1 0
##      description      "
##          The Cogent board has two separate serial ports.  This option
##          chooses which of these ports will be used for diag output.
##          On the CMA101 board, 0 and 1 are the P11 and P12
##          connectors respectively. On the CMA102 board, 0 and 1 are
##          the P2 and P3 connectors respectively."
##  }

  
  }}CFG_DATA */

#define CYGHWR_HAL_POWERPC_COGENT_STARTUP       ram
#define CYGHWR_HAL_POWERPC_COGENT_GDB_PORT      1
#define CYGHWR_HAL_POWERPC_COGENT_DIAG_PORT     1

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_POWERPC_COGENT_H */
/* EOF hal_powerpc_cogent.h */
