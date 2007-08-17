//==========================================================================
//
//        can_busload.c
//
//        CAN bus load test
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
// Date:          2007-06-26
// Description:   CAN bus load test
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

// We need two CAN channels
#if defined(CYGPKG_DEVS_CAN_LPC2XXX_CAN0) && defined(CYGPKG_DEVS_CAN_LPC2XXX_CAN1)


// The same baud rates are required because we send from one channel to the other one
#if CYGNUM_DEVS_CAN_LPC2XXX_CAN0_KBAUD == CYGNUM_DEVS_CAN_LPC2XXX_CAN1_KBAUD


// We need a large RX buffer
#ifdef CYGNUM_DEVS_CAN_LPC2XXX_CAN0_QUEUESIZE_RX_1024

#include "can_test_aux.inl" // include CAN test auxiliary functions
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
cyg_io_handle_t    hCAN_Tbl[2];


//===========================================================================
// Thread 0
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_uint32      len;
    cyg_can_message tx_msg;
    cyg_can_event   rx_event;
    cyg_uint32      i;
    cyg_uint32      rx_msg_cnt = 0;

    
    //
    // Prepeare message - we use a data length of 0 bytes here. Each received message
    // causes an iterrupt. The shortest message is a 0 data byte message. This will generate
    // the highest interrupt rate
    //
    CYG_CAN_MSG_SET_PARAM(tx_msg, 0, CYGNUM_CAN_ID_STD, 0, CYGNUM_CAN_FRAME_DATA);
    
    //
    // Now send 1024 CAN messages as fast as possible to stress the receiver of CAN
    // channel 1
    //
    for (i = 0; i< 1024; ++i)
    {
        tx_msg.id = i; 
        len = sizeof(tx_msg);
        if (ENOERR != cyg_io_write(hCAN_Tbl[1], &tx_msg, &len))
        {
            CYG_TEST_FAIL_FINISH("Error writing to channel 0");    
        }
    }
    
    //
    // Now try to receive all 1024 CAN messages. If all messages are received
    // and no overrun occured then the message processing is fast enought
    //
    while (1)
    {
        len = sizeof(rx_event);  
        //
        // First receive CAN event from real CAN hardware
        //
        len = sizeof(rx_event);
        if (ENOERR != cyg_io_read(hCAN_Tbl[0], &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from channel 1");   
        }
        
        if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
        {
            print_can_msg(&rx_event.msg, "RX chan 1:");
            rx_msg_cnt++;
            if (rx_msg_cnt == 1024)
            {
                CYG_TEST_PASS_FINISH("CAN load test OK");        
            }
        } // if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
        else
        {
            print_can_flags(rx_event.flags, "");  
            if (rx_event.flags & CYGNUM_CAN_EVENT_OVERRUN_RX)
            {
                CYG_TEST_FAIL_FINISH("RX overrun for channel 1");       
            }
            
            if (rx_event.flags & CYGNUM_CAN_EVENT_ERR_PASSIVE)
            {
                CYG_TEST_FAIL_FINISH("Channel 1 error passive event");       
            }
            
            if (rx_event.flags & CYGNUM_CAN_EVENT_BUS_OFF)
            {
                CYG_TEST_FAIL_FINISH("Channel 1 bus off event");       
            }
        }
    } // while (1)
}


//===========================================================================
// Entry point
//===========================================================================
void cyg_start(void)
{
    CYG_TEST_INIT();

    //
    // open CAN device driver channel 1
    //
    if (ENOERR != cyg_io_lookup(CYGPKG_DEVS_CAN_LPC2XXX_CAN0_NAME, &hCAN_Tbl[0])) 
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 0");
    }
    

    //
    // open CAN device driver channel 2
    //
    if (ENOERR != cyg_io_lookup(CYGPKG_DEVS_CAN_LPC2XXX_CAN1_NAME, &hCAN_Tbl[1])) 
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 1");
    }
   
    //
    // create the main thread
    //
    cyg_thread_create(5, can0_thread, 
                        (cyg_addrword_t) 0,
                        "can_tx_thread", 
                        (void *) can0_thread_data.stack, 
                        1024 * sizeof(long),
                        &can0_thread_data.hdl, 
                        &can0_thread_data.obj);
                        
    cyg_thread_resume(can0_thread_data.hdl);
    
    cyg_scheduler_start();
}
#else // CYGNUM_DEVS_CAN_LPC2XXX_CAN0_QUEUESIZE_RX_1024
#define N_A_MSG "Channel 0 needs RX buffer size for 1024 events"
#endif

#else // CYGNUM_DEVS_CAN_LPC2XXX_CAN0_KBAUD == CYGNUM_DEVS_CAN_LPC2XXX_CAN1_KBAUD
#define N_A_MSG "Baudrate of channel 0 and 1 need to be equal"
#endif

#else // defined(CYGPKG_DEVS_CAN_LPC2XXX_CAN0) && defined(CYGPKG_DEVS_CAN_LPC2XXX_CAN1)
#define N_A_MSG "Needs support for CAN channel 1 and 2"
#endif

#else // CYGFUN_KERNEL_API_C
#define N_A_MSG "Needs kernel C API"
#endif

#else // CYGPKG_IO_CAN && CYGPKG_KERNEL
#define N_A_MSG "Needs Kernel"
#endif

#ifdef N_A_MSG
void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG

//---------------------------------------------------------------------------
// EOF can_busload.c
