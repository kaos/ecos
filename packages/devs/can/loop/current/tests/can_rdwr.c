//==========================================================================
//
//        can_rdwr.c
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
    cyg_can_buf_info_t tx_buf_info;
    cyg_can_message    tx_msg =
    {
        0x000,                                               // CAN identifier
        {0x00, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7},    // 8 data bytes
        CYGNUM_CAN_ID_STD,                                   // standard frame
        CYGNUM_CAN_FRAME_DATA,                               // data frame
        8,                                                   // data length code
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
    
    if (tx_buf_info.tx_bufsize < 10)
    {
        CYG_TEST_FAIL_FINISH("TX quesize of /dev/can0 too small for 10 CAN messages");
    }
    
    while (1)
    {
        //
        // now we simply send 10 CAN messages and then suspend the thread
        //
        CYG_TEST_INFO("Thread0: Writing 10 CAN messages");
        for (i = 0; i < 10; ++i)
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
        }
        
        //
        // Now we we give the reader thread a chance to run and to read
        // the messages      
        //
        cyg_thread_delay(100);
        CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");                      
    }
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
    
    if (ENOERR != cyg_io_lookup("/dev/can1", &hCAN1)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can1");
    }
    
    len = sizeof(rx_buf_info);
    if (ENOERR != cyg_io_get_config(hCAN1, CYG_IO_GET_CONFIG_CAN_BUFFER_INFO ,&rx_buf_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can1");
    }
    
    if (rx_buf_info.rx_bufsize < 10)
    {
        CYG_TEST_FAIL_FINISH("RX quesize of /dev/can1 too small for 10 CAN events");
    }
    
    while (1)
    {
        //
        // now we try to read the 10 CAN messages that was sent by writer 
        // thread previously
        //
        CYG_TEST_INFO("Thread1: Reading 10 CAN messages");
        for (i = 0; i < 10; ++i)
        {
            len = sizeof(rx_event); 
            
            if (ENOERR != cyg_io_read(hCAN1, &rx_event, &len))
            {
                CYG_TEST_FAIL_FINISH("Error reading from /dev/can1");
            }
            
            //
            // we expect only RX events in this test case - so all other events
            // cause a test fail
            //
            if (!(rx_event.flags & CYGNUM_CAN_EVENT_RX))
            {
                CYG_TEST_FAIL_FINISH("Received wrong CAN event type");
            }
            
            //
            // The writer thread stored the message number in CAN id and first
            // data byte so we can check now if we received valid data
            //
            if ((rx_event.msg.id != i) || (rx_event.msg.data[0] != i))
            {
                CYG_TEST_FAIL_FINISH("Received CAN message contains unexpected data");
            }
            else
            {
                print_can_msg(&rx_event.msg, "");
            }
        } //for (i = 0; i < 10; ++i)
        
        CYG_TEST_PASS_FINISH("can_rdwr test OK");
    } // while (1)
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
