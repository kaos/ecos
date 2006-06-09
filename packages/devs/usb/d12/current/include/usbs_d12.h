#ifndef CYGONCE_USBS_D12_H
#define CYGONCE_USBS_D12_H
//==========================================================================
//
//      include/usbs_d12.h
//
//      The interface exported by the D12 USB device driver
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd
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
// Author(s):    Frank Pagliughi (fmp)
// Contributors: fmp
// Date:         2004-05-24
// Purpose:
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/io/usb/usbs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The Philips D12 is a full speed (12Mbps) USB peripheral controller
 * chip, with a parallel interface allowing it to be connected to nearly
 * any microcontroller or microprocessor. 
 */
extern usbs_control_endpoint    usbs_d12_ep0;

extern usbs_rx_endpoint         usbs_d12_rx_ep1;
extern usbs_tx_endpoint         usbs_d12_tx_ep1;
extern usbs_rx_endpoint         usbs_d12_rx_ep2;
extern usbs_tx_endpoint         usbs_d12_tx_ep2;
    
#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* CYGONCE_USBS_D12_H */
