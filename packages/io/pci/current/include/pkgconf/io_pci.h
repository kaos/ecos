#ifndef CYGONCE_PKGCONF_IO_PCI_H
#define CYGONCE_PKGCONF_IO_PCI_H
// ====================================================================
//
//      pkgconf/io_pci.h
//
//      PCI Library configuration file
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
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-08-12
// Purpose:     To allow the user to edit PCI library configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_IO_PCI {
       display "PCI configuration library"
       type    bool
       parent  CYGPKG_IO
       description "
           The PCI configuration library provides initialization of devices
           on the PCI bus. Functions to find and access these devices are
	   also provided."
   }

   }}CFG_DATA */

#endif  // CYGONCE_PKGCONF_IO_PCI_H
// EOF io_pci.h
