//==========================================================================
//
//      usbs_sa11x0.c
//
//      Static data for the SA11x0 USB device driver
//
//==========================================================================
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
// Copyright (C) 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    bartv
// Contributors: bartv
// Date:         2000-10-04
//
// This file contains various objects that should go into extras.o
// rather than libtarget.a, e.g. devtab entries that would normally
// be eliminated by the selective linking.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/io/devtab.h>
#include <cyg/io/usb/usbs_sa11x0.h>
#include <pkgconf/devs_usb_sa11x0.h>

// ----------------------------------------------------------------------------
// Initialization. The goal here is to call usbs_sa11x0_init()
// early on during system startup, to take care of things like
// registering interrupt handlers etc. which are best done
// during system init.
//
// If the endpoint 0 devtab entry is available then its init()
// function can be used to take care of this. However the devtab
// entries are optional so an alternative mechanism must be
// provided. Unfortunately although it is possible to give
// a C function the constructor attribute, it cannot be given
// an initpri attribute. Instead it is necessary to define a
// dummy C++ class.

extern "C" void usbs_sa11x0_init(void);

#ifndef CYGVAR_DEVS_USB_SA11X0_EP0_DEVTAB_ENTRY
class usbs_sa11x0_initialization {
  public:
    usbs_sa11x0_initialization() {
        usbs_sa11x0_init();
    }
};

static usbs_sa11x0_initialization usbs_sa11x0_init_object CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_IO);
#endif

// ----------------------------------------------------------------------------
// The devtab entries. Each of these is optional, many applications
// will want to use the lower-level API rather than go via
// open/read/write/ioctl.

#ifdef CYGVAR_DEVS_USB_SA11X0_EP0_DEVTAB_ENTRY

// For endpoint 0 the only legal operations are get_config() and
// set_config(), and these are provided by the common package.

static bool
usbs_sa11x0_devtab_ep0_init(struct cyg_devtab_entry* tab)
{
    CYG_UNUSED_PARAM(struct cyg_devtab_entry*, tab);
    usbs_sa11x0_init();
    return true;
}

static CHAR_DEVIO_TABLE(usbs_sa11x0_ep0_devtab_functions,
                        &cyg_devio_cwrite,
                        &cyg_devio_cread,
                        &cyg_devio_select,
                        &usbs_devtab_get_config,
                        &usbs_devtab_set_config);

static CHAR_DEVTAB_ENTRY(usbs_sa11x0_ep0_devtab_entry,
                         CYGDAT_DEVS_USB_SA11X0_DEVTAB_BASENAME "0c",
                         0,
                         &usbs_sa11x0_ep0_devtab_functions,
                         &usbs_sa11x0_devtab_ep0_init,
                         0,
                         (void*) &usbs_sa11x0_ep0);
#endif

// ----------------------------------------------------------------------------
// Common routines for ep1 and ep2.
#if defined(CYGVAR_DEVS_USB_SA11X0_EP1_DEVTAB_ENTRY) || defined(CYGVAR_DEVS_USB_SA11X0_EP2_DEVTAB_ENTRY)
static bool
usbs_sa11x0_devtab_dummy_init(struct cyg_devtab_entry* tab)
{
    CYG_UNUSED_PARAM(struct cyg_devtab_entry*, tab);
    return true;
}
#endif

// ----------------------------------------------------------------------------
// ep1 devtab entry. This can only be used for host->slave, so only the
// cread() function makes sense.

#ifdef CYGVAR_DEVS_USB_SA11X0_EP1_DEVTAB_ENTRY

static CHAR_DEVIO_TABLE(usbs_sa11x0_ep1_devtab_functions,
                        &cyg_devio_cwrite,
                        &usbs_devtab_cread,
                        &cyg_devio_select,
                        &usbs_devtab_get_config,
                        &usbs_devtab_set_config);

static CHAR_DEVTAB_ENTRY(usbs_sa11x0_ep1_devtab_entry,
                         CYGDAT_DEVS_USB_SA11X0_DEVTAB_BASENAME "1r",
                         0,
                         &usbs_sa11x0_ep1_devtab_functions,
                         &usbs_sa11x0_devtab_dummy_init,
                         0,
                         (void*) &usbs_sa11x0_ep1);
#endif

// ----------------------------------------------------------------------------
// ep2 devtab entry. This can only be used for slave->host, so only
// the cwrite() function makes sense.

#ifdef CYGVAR_DEVS_USB_SA11X0_EP2_DEVTAB_ENTRY

static CHAR_DEVIO_TABLE(usbs_sa11x0_ep2_devtab_functions,
                        &usbs_devtab_cwrite,
                        &cyg_devio_cread,
                        &cyg_devio_select,
                        &usbs_devtab_get_config,
                        &usbs_devtab_set_config);

static DEVTAB_ENTRY(usbs_sa11x0_ep2_devtab_entry,
                    CYGDAT_DEVS_USB_SA11X0_DEVTAB_BASENAME "2w",
                    0,
                    &usbs_sa11x0_ep2_devtab_functions,
                    &usbs_sa11x0_devtab_dummy_init,
                    0,
                    (void*) &usbs_sa11x0_ep2);

#endif

