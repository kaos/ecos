#ifndef CYGONCE_USBS_SA11X0_H
# define CYGONCE_USBS_SA11X0_H
//==========================================================================
//
//      include/usbs_sa11x0.h
//
//      The interface exported by the SA11X0 USB device driver
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.
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
// Purpose:
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/io/usb/usbs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The SA11x0 family comes with on-chip USB slave support. This
 * provides three endpoints. Endpoint 0 can only be used for control
 * messages. Endpoints 1 and 2 can only be used for bulk transfers,
 * host->slave for endpoint 1 and slave->host for endpoint 2.
 */
extern usbs_control_endpoint    usbs_sa11x0_ep0;
extern usbs_rx_endpoint         usbs_sa11x0_ep1;
extern usbs_tx_endpoint         usbs_sa11x0_ep2;
    
#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* CYGONCE_USBS_SA11X0_H */
