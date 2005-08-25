//==========================================================================
//
//        can_txevent.c
//
//        Test CAN device driver timeouts
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Uwe Kindler
// Contributors:  Uwe Kindler
// Date:          2005-08-07
// Description:   Timeout test of CAN device driver
//####DESCRIPTIONEND####


//===========================================================================
//                                INCLUDES
//===========================================================================
#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>         // test macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/infra/diag.h>

// Package requirements
#if defined(CYGPKG_IO_CAN) && defined(CYGPKG_KERNEL)

#include <pkgconf/kernel.h>
#include <cyg/io/io.h>
#include <cyg/io/canio.h>

// Package option requirements
#if defined(CYGFUN_KERNEL_API_C)

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/kernel/kapi.h>

// tx event support required
#if defined(CYGOPT_IO_CAN_TX_EVENT_SUPPORT)


//===========================================================================
//                               DATA TYPES
//===========================================================================
typedef struct st_thread_data
{
    cyg_thread   obj;
    long         stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];
    cyg_handle_t hdl;
} thread_data_t;


//===========================================================================
//                              LOCAL DATA
//===========================================================================
cyg_thread_entry_t can0_thread;
thread_data_t      can0_thread_data;


//===========================================================================
//                          LOCAL FUNCTIONS
//===========================================================================
#include "can_test_aux.inl" // include CAN test auxiliary functions


//===========================================================================
//                             WRITER THREAD 
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_io_handle_t    hCAN0;
    cyg_uint32         len;
    cyg_can_buf_info_t buf_info;
    cyg_uint16         i;
    cyg_can_event      rx_event;
    cyg_can_message    tx_msg =
    {
        0x000,                                               // CAN identifier
        {0x00, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7},    // 8 data bytes
        CYGNUM_CAN_ID_STD,                                   // standard frame
        CYGNUM_CAN_FRAME_DATA,                               // data frame
        4,                                                   // data length code
    };
   
    
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN0)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }
    
    //
    // first we read the buffer info
    //
    len = sizeof(buf_info);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    }
    
    //
    // check if buffer is really empty
    //
    if (buf_info.rx_count != 0)
    {
        CYG_TEST_FAIL_FINISH("Receive buffer of /dev/can0 is not empty.");
    }
    
    //
    // now send messages - because TX events are supported each transmitted CAN message
    // will cause a TX event that is filled into receive queue
    //
    diag_printf("Sending %d CAN messages to /dev/can0 \n", buf_info.rx_bufsize);
    for (i = 0; i < buf_info.rx_bufsize; ++i)
    {
        tx_msg.id = i;
        tx_msg.data[0] = i;
        len = sizeof(tx_msg);
        
        if (ENOERR != cyg_io_write(hCAN0, &tx_msg, &len))
        {
            CYG_TEST_FAIL_FINISH("Error writing to /dev/can0");
        }
        else
        {
            print_can_msg(&tx_msg, "");
        }
    }
    
    //
    // now we read the buffer info - we expect a completely filled recieve queue
    //
    len = sizeof(buf_info);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    }
    
    //
    // if receive queue is not completely filled, then we have an error here
    //
    if (buf_info.rx_bufsize != buf_info.rx_count)
    {
        diag_printf("RX bufsize: %d    events in RX buffer: %d\n", buf_info.rx_bufsize, buf_info.rx_count);
        CYG_TEST_FAIL_FINISH("Receive queue of /dev/can0 not completely filled.");
    }
    
    //
    // now we read the receive queue
    //
    diag_printf("Receiving %d TX events from /dev/can0 \n", buf_info.rx_count);
    for (i = 0; i < buf_info.rx_count; ++i)
    {
        len = sizeof(rx_event);
        
        if (ENOERR != cyg_io_read(hCAN0, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }
        
        //
        // we expect only a set TX flag because no other events may arrive for the
        // loopback driver
        //
        if (!(rx_event.flags & CYGNUM_CAN_EVENT_TX) || (rx_event.flags & !CYGNUM_CAN_EVENT_TX))
        {
            CYG_TEST_FAIL_FINISH("Unexpected receive event flags.");
        }
        
        //
        // Now check if TX events contain valid data - we know that the ID and the first
        // data byte contain the message number
        //
        if ((rx_event.msg.id != i) || (rx_event.msg.data[0] != i))
        {
            CYG_TEST_FAIL_FINISH("Received invalid data in TX event");
        }
        else
        {
            print_can_msg(&rx_event.msg, "");
        }     
    } // for (i = 0; i < buf_info.rx_count; ++i)
           
    CYG_TEST_PASS_FINISH("can_txevent test OK");                     
}



void
cyg_start(void)
{
    CYG_TEST_INIT();
    
    //
    // create the thread which access the CAN device driver
    //
    cyg_thread_create(4, can0_thread, 
                        (cyg_addrword_t) 0,
		                "can0_thread", 
		                (void *) can0_thread_data.stack, 
		                1024 * sizeof(long),
		                &can0_thread_data.hdl, 
		                &can0_thread_data.obj);
		                
    cyg_thread_resume(can0_thread_data.hdl);
    
    cyg_scheduler_start();
}

#else // CYGOPT_IO_CAN_TX_EVENT_SUPPORT
#define N_A_MSG "TX event support required for IO/CAN"
#endif

#else // CYGFUN_KERNEL_API_C
#define N_A_MSG "Needs kernel C API"
#endif

#else // CYGPKG_IO_CAN && CYGPKG_KERNEL
#define N_A_MSG "Needs IO/CAN and Kernel"
#endif

#ifdef N_A_MSG
void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( N_A_MSG);
}
#endif // N_A_MSG

// EOF serial4.c
