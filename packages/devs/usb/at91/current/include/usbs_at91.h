#ifndef CYGONCE_USBS_AT91_H
#define CYGONCE_USBS_AT91_H
//==========================================================================
//
//      include/usbs_at91.h
//
//      The interface exported by the AT91 USB device driver
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 eCosCentric Ltd
// Copyright (C) 2007 Andrew Lunn
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Oliver Munz
// Contributors: bartv
// Date:         2006-02-25
// Purpose:
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/io/usb/usbs.h>
#include <pkgconf/devs_usb_at91.h>

#define AT91_USB_ENDPOINTS 4
 
extern usbs_control_endpoint    usbs_at91_ep0;
extern usbs_rx_endpoint         usbs_at91_ep1;
extern usbs_rx_endpoint         usbs_at91_ep2;
extern usbs_rx_endpoint         usbs_at91_ep3;

extern void usbs_at91_endpoint_init(usbs_rx_endpoint * pep, 
                                    cyg_uint8 endpoint_type, cyg_bool enable);
#endif /* CYGONCE_USBS_AT91_H */
