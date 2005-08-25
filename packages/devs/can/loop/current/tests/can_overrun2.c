//==========================================================================
//
//        can_overrun2.c
//
//        Test CAN device RX overrun events
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
// Description:   Simple read/write test of CAN driver
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

cyg_thread_entry_t can1_thread;
thread_data_t      can1_thread_data;


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
    cyg_uint8          i;
    cyg_uint32         len;
    cyg_uint32         rx_bufsize;
    cyg_can_buf_info_t tx_buf_info;
    cyg_can_event      rx_event;
    cyg_can_message    tx_msg =
    {
        0x000,                                               // CAN identifier
        {0x00, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7},    // 8 data bytes
        CYGNUM_CAN_ID_STD,                                   // standard frame
        CYGNUM_CAN_FRAME_DATA,                               // data frame
        2,                                                   // data length code
    };
    
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN0)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }
    
    len = sizeof(tx_buf_info);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&tx_buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    }
    
    //
    // Before we can write the CAN messages, we need to know the buffer size of the
    // receiver. The receiver will tell us this buffer size with one single CAN
    // message
    //
    len = sizeof(rx_event); 
            
    if (ENOERR != cyg_io_read(hCAN0, &rx_event, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
    }
    
    //
    // we expect a RX event here - we treat any other flag as an error
    //
    if (!(rx_event.flags & CYGNUM_CAN_EVENT_RX) || (rx_event.flags & !CYGNUM_CAN_EVENT_RX))
    {
        CYG_TEST_FAIL_FINISH("Unexpected RX event for /dev/can0");
    }
    
    rx_bufsize = *((cyg_uint32 *)rx_event.msg.data);
    
    //
    // now we send exactly one CAN message more than there is space in the receive buffer
    // this should cause an RX ovverun in receive buffer
    //
    diag_printf("/dev/can0: Sending %d CAN messages\n", rx_bufsize); 
    for (i = 0; i <= rx_bufsize; ++i)
    {
        //
        // we store the message number as CAN id and in first data byte so
        // a receiver can check this later
        //
        tx_msg.id = 0x000 + i;
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
    }  // for (i = 0; i <= rx_bufsize; ++i)
    
    cyg_thread_suspend(cyg_thread_self());                 
}


//===========================================================================
//                            READER THREAD
//===========================================================================
void can1_thread(cyg_addrword_t data)
{
    cyg_io_handle_t    hCAN1;
    cyg_uint8          i;
    cyg_uint32         len;
    cyg_can_buf_info_t rx_buf_info;
    cyg_can_event      rx_event;
    cyg_can_message    tx_msg;
    
    if (ENOERR != cyg_io_lookup("/dev/can1", &hCAN1)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can1");
    }
    
    len = sizeof(rx_buf_info);
    if (ENOERR != cyg_io_get_config(hCAN1, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&rx_buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can1");
    }
    
    //
    // first we send the size of our receive buffer to the writer
    // we setup tx message now
    //
    tx_msg.id  = 0x000;
    tx_msg.ext = CYGNUM_CAN_ID_STD;
    tx_msg.rtr = CYGNUM_CAN_FRAME_DATA;
    tx_msg.dlc = sizeof(rx_buf_info.rx_bufsize);
    
    //
    // we store size of rx buffer in CAN message. We do not need to care about
    // endianess here because this is a loopback driver test and we will receive
    // our own messages
    //
    *((cyg_uint32 *)tx_msg.data) = rx_buf_info.rx_bufsize;
    len = sizeof(tx_msg); 
    
    //
    // as soon as we send a CAN message, thread 0 will resume because it is waiting
    // for a message
    //
    diag_printf("/dev/can1: Sending size of RX buffer %d\n", rx_buf_info.rx_bufsize);        
    if (ENOERR != cyg_io_write(hCAN1, &tx_msg, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing to /dev/can1");
    }
    cyg_thread_delay(10); // let thread 0 run
       
    //
    // now we check if we received CAN messages  - if receive buffer is not full
    // the we have an error here because we expect a full receive buffer
    //
    len = sizeof(rx_buf_info);
    if (ENOERR != cyg_io_get_config(hCAN1, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&rx_buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can1");
    }
    
    if (rx_buf_info.rx_bufsize != rx_buf_info.rx_count)
    {
        CYG_TEST_FAIL_FINISH("RX buffer of /dev/can1 does not contain number of expected messages");
    }
    
    //
    // now we wait for messages from /dev/can0
    //
    diag_printf("/dev/can1: Receiving %d CAN messages\n", rx_buf_info.rx_count);
    for (i = 0; i < rx_buf_info.rx_count; ++i)
    {
        len = sizeof(rx_event);
        if (ENOERR != cyg_io_read(hCAN1, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }
        else
        {
            if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
            {
                print_can_msg(&rx_event.msg, "");
                if (rx_event.msg.data[0] != (i + 1))
                {
                    CYG_TEST_FAIL_FINISH("Received /dev/can1 RX event contains invalid data");
                }                    
            }
            else
            {
                CYG_TEST_FAIL_FINISH("Unexpected CAN event for /dev/can1");
            }
            
            //
            // now check if any other flag is set
            //
            if (rx_event.flags &  CYGNUM_CAN_EVENT_OVERRUN_RX)
            {
                diag_printf("RX queue overrun successfully indicated for /dev/can1\n");

//
// if TX events are supported then we have already a TX event in receive queue because
// we sent a message and the RX queue overrun will occur one message earlier
//
#if defined(CYGOPT_IO_CAN_TX_EVENT_SUPPORT)
                if (i < (rx_buf_info.rx_bufsize - 2))
#else
                if (i < (rx_buf_info.rx_bufsize - 1))
#endif
                {
                    CYG_TEST_FAIL_FINISH("RX queue overrun occured too early for /dev/can1");
                }
                else
                {
                    CYG_TEST_PASS_FINISH("can_overrun2 test OK"); 
                }
            } // if (rx_event.flags &  CYGNUM_CAN_EVENT_OVERRUN_RX)  
        }
        
    }
}



void
cyg_start(void)
{
    CYG_TEST_INIT();
    
    //
    // create the two threads which access the CAN device driver
    //
    cyg_thread_create(4, can0_thread, 
                        (cyg_addrword_t) 0,
		                "can0_thread", 
		                (void *) can0_thread_data.stack, 
		                1024 * sizeof(long),
		                &can0_thread_data.hdl, 
		                &can0_thread_data.obj);
		                
    cyg_thread_create(5, can1_thread, 
                        (cyg_addrword_t) can0_thread_data.hdl,
		                "can1_thread", 
		                (void *) can1_thread_data.stack, 
		                1024 * sizeof(long),
		                &can1_thread_data.hdl, 
		                &can1_thread_data.obj);
		                
    cyg_thread_resume(can0_thread_data.hdl);
    cyg_thread_resume(can1_thread_data.hdl);
    
    cyg_scheduler_start();
}

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
