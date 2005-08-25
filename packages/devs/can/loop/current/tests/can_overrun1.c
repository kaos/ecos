//==========================================================================
//
//        can_overrun1.c
//
//        Test CAN device drivers
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
    cyg_uint8          i;
    cyg_uint32         len;
    cyg_can_buf_info_t buf_info;
    cyg_can_event      rx_event;
    cyg_can_message    tx_msg =
    {
        0x000,                                               // CAN identifier
        {0x00, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7},    // 8 data bytes
        CYGNUM_CAN_ID_STD,                                   // standard frame
        CYGNUM_CAN_FRAME_DATA,                               // data frame
        0,                                                   // data length code
    };
    
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN0)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }
    
    len = sizeof(buf_info);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    }
    
    if (buf_info.rx_count > 0)
    {
        CYG_TEST_FAIL_FINISH("Empty RX buffer expected for /dev/can0");
    }
    
    //
    // now we send exactly one CAN message more than it is space in left in buffer
    // Because each TX message will cause a TX event in receive queue we should
    // get a RX queue overrun
    //
    diag_printf("Sending %d CAN messages to /dev/can0\n", buf_info.rx_bufsize + 1);
    for (i = 0; i <= buf_info.rx_bufsize; ++i)
    {
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
    }
    
    //
    // now check if receive queue is completely filled - that means number of rx events should
    // be equal to RX queue size
    //
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    }
    
    if (buf_info.rx_bufsize != buf_info.rx_count)
    {
        CYG_TEST_FAIL_FINISH("Number of events in /dev/can0 RX queue differs from queue size");
    }
    
    //
    // Now read all events from receive queue - if everything is o.k. than the oldest
    // event should be overwritten by the latest sent message and the last event in
    // queue should indicate a RX overrun
    //
    diag_printf("\nReceiving %d CAN messages from /dev/can0\n", buf_info.rx_bufsize);
    for (i = 0; i < buf_info.rx_bufsize; ++i)
    {
        len = sizeof(rx_event);
        
        if (ENOERR != cyg_io_read(hCAN0, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can1");
        } 
        else
        {
            //
            // if we received a valid TX event then we can print the message
            //
            if (rx_event.flags & CYGNUM_CAN_EVENT_TX)
            {
                print_can_msg(&rx_event.msg, "");
                if (rx_event.msg.data[0] != (i + 1))
                {
                    CYG_TEST_FAIL_FINISH("Received /dev/can0 TX event contains invalid data");
                }                    
            }
            else
            {
                CYG_TEST_FAIL_FINISH("Unexpected CAN event for /dev/can0");
            }
            
            //
            // now check if any other flag is set
            //
            if (rx_event.flags &  CYGNUM_CAN_EVENT_OVERRUN_RX)
            {
                diag_printf("RX queue overrun successfully indicated for /dev/can0\n");
                if (i < (buf_info.rx_bufsize - 1))
                {
                    CYG_TEST_FAIL_FINISH("RX queue overrun occured too early /dev/can0");
                }
                else
                {
                    CYG_TEST_PASS_FINISH("can_overrun1 test OK"); 
                }
            } // if (rx_event.flags &  CYGNUM_CAN_EVENT_OVERRUN_RX)         
        }   
    } // for (i = 0; i < buf_info.rx_bufsize; ++i)
    
    CYG_TEST_FAIL_FINISH("RX overrun expected but not received for /dev/can0");
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
