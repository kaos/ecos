//==========================================================================
//
//      usb_serial_echo.c
//
//      Example application for the USB serial layer in eCos.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2010 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Frank M. Pagliughi (fmp), SoRo Systems, Inc.
// Contributors: 
// Date:         2008-06-02
// Description:  USB serial example application.
//
//####DESCRIPTIONEND####
//===========================================================================

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <pkgconf/kernel.h>
#include <cyg/io/usb/usbs_serial.h>

#include <stdio.h>
#include <stdlib.h>

#define DEBUG_OUTPUT

#if defined(DEBUG_OUTPUT)
#define DBG diag_printf
#else
#define DBG (1) ? (void)0 : diag_printf
#endif

typedef unsigned char byte;

// --------------------------------------------------------------------------
//                                                         Data & Callback(s)
// --------------------------------------------------------------------------

#define BUF_SIZE        4096

static byte     rx_buf[2][BUF_SIZE+1], tx_buf[BUF_SIZE+1];

// --------------------------------------------------------------------------
//                                                              Main Routine
// --------------------------------------------------------------------------

int main(void)
{
  int             n;
  unsigned        ibuf, next_buf;
  
  // ----- Start USB subsystem -----
  
  usbs_serial_start();
  
  // ----- Get data from host and send it back -----
  
  while (1) {
    ibuf = 0;
    
    // ----- Wait for the host to configure -----
    
    usbs_serial_wait_until_configured();
    cyg_thread_delay((cyg_tick_count_t) 10);
    
    // ----- While configured read data & print to screen -----
    
    usbs_serial_start_rx(&usbs_ser0, rx_buf[ibuf], BUF_SIZE);
    
    while (usbs_serial_is_configured()) {
      
      n = usbs_serial_wait_for_rx(&usbs_ser0);
      next_buf = ibuf ^ 1;
      
      usbs_serial_start_rx(&usbs_ser0, rx_buf[next_buf], BUF_SIZE);
      
      if (n < 0) {
        DBG("*** I/O Error: %d ***\n", n);
      }
      else {
        memcpy(tx_buf, rx_buf[ibuf], n);
        usbs_serial_tx(&usbs_ser0, tx_buf, n);
        rx_buf[ibuf][n] = '\0';
        DBG("%s", rx_buf[ibuf]);
      }
      
      ibuf = next_buf;
    }
  }
  
  return 0;
}

