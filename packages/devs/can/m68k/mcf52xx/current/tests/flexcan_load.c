//==========================================================================
//
//        flexcan_load.c
//
//        FlexCAN load test
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
// Date:          2005-08-14
// Description:   FlexCAN load test
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

cyg_io_handle_t    hDrvFlexCAN;


//===========================================================================
//                          LOCAL FUNCTIONS
//===========================================================================
#include "can_test_aux.inl" // include CAN test auxiliary functions


//===========================================================================
//                             READER THREAD 
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_uint32             len;
    cyg_can_event          rx_event;
    cyg_can_timeout_info_t timeouts;

#if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)   
    //
    // setup large timeout values because we do not need timeouts here
    //
    timeouts.rx_timeout = 100000;
    timeouts.tx_timeout = 100000;
    
    len = sizeof(timeouts);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_TIMEOUT ,&timeouts, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    }
#endif // defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)  
    
    //
    // This thread simply receives all CAN events and prints the event flags and the
    // CAN message if it was a TX or RX event. You can use this test in order to check
    // when a RX overrun occurs
    //
    while (1)
    {
        len = sizeof(rx_event); 
            
        if (ENOERR != cyg_io_read(hDrvFlexCAN, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }
        else
        {
            print_can_flags(rx_event.flags, "");
            
            if ((rx_event.flags & CYGNUM_CAN_EVENT_RX) || (rx_event.flags & CYGNUM_CAN_EVENT_TX))
            {
                print_can_msg(&rx_event.msg, "");
            }
        }    
    }             
}


//===========================================================================
//                            WRITER THREAD
//===========================================================================
void can1_thread(cyg_addrword_t data)
{
    cyg_uint16      i = 0;
    cyg_uint32      len;
    cyg_can_message tx_msg =
    {
        0x000,                                               // CAN identifier
        {0x00, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7},    // 8 data bytes
        CYGNUM_CAN_ID_STD,                                   // standard frame
        CYGNUM_CAN_FRAME_DATA,                               // data frame
        8,                                                   // data length code
    };
    
    //
    // This thread simply sends CAN messages. It increments the ID for each new CAN messsage
    // and sends a remote frame after seven data frames. In the first byte of each data frame
    // the number (0 - 7) of the data frame is stored and the length of the data frame grows
    // from 1 - 8 data bytes.
    //
    // The received pattern should look like this way:
    // ID    Length    Data
    // ----------------------------------------------
    // 000   1         00
    // 001   2         01 F1
    // 002   3         02 F1 F2
    // 003   4         03 F1 F2 F3
    // 004   5         04 F1 F2 F3 F4
    // 005   6         05 F1 F2 F3 F4 F5
    // 006   7         06 F1 F2 F3 F4 F5 F6
    // 007   8         Remote Request
    // 008   1         00
    // 009   2         01 F1
    // 00A   3         02 F1 F2
    // ...
    //
    while (1)
    {
        tx_msg.id      = i;
        tx_msg.dlc     = (i % 8) + 1;
        tx_msg.data[0] = (i % 8);
        i = (i + 1) % 0x7FF;
        
        //
        // the 6th frame is a remote frame
        //
        if ((i % 8) == 6)
        {
            tx_msg.rtr =  CYGNUM_CAN_FRAME_RTR;
            tx_msg.ext =  CYGNUM_CAN_ID_STD;
        }
        //
        // the 7th frame is a extended frame
        //
        else if ((i % 8) == 7)
        {
            tx_msg.ext =  CYGNUM_CAN_ID_EXT;
            tx_msg.rtr = CYGNUM_CAN_FRAME_DATA;
        }
        //
        // the 8th frame is a extended remote frame
        //
        else if (!(i % 8))
        {
            tx_msg.ext =  CYGNUM_CAN_ID_EXT;
            tx_msg.rtr = CYGNUM_CAN_FRAME_RTR;
        }
        //
        // all other frames are standard data framse
        //
        else
        {
            tx_msg.ext =  CYGNUM_CAN_ID_STD;
            tx_msg.rtr =  CYGNUM_CAN_FRAME_DATA;
        }
        
        len = sizeof(tx_msg);
        if (ENOERR != cyg_io_write(hDrvFlexCAN, &tx_msg, &len))
        {
            CYG_TEST_FAIL_FINISH("Error writing to /dev/can0");
        }
        else
        {
            print_can_msg(&tx_msg, "TX: ");
        } 
        
        cyg_thread_delay(100);          
    } // while (1)
}



void
cyg_start(void)
{
    CYG_TEST_INIT();
    
    //
    // open flexcan device driver
    //
    if (ENOERR != cyg_io_lookup("/dev/can0", &hDrvFlexCAN)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }
    
    // We do not setup baudrate and take dafauklt baudrate from config tool instead
    /*
    //
    // setup CAN baudrate 250 KBaud
    //
    cyg_uint32     len;
    cyg_can_info_t can_cfg;
    can_cfg.baud = CYGNUM_CAN_KBAUD_250;
    len = sizeof(can_cfg);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_INFO ,&can_cfg, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    }
    */
    
    //
    // create the two threads which access the CAN device driver
    // a reader thread with a higher priority and a writer thread
    // with a lower priority
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

// EOF flexcan_load.c
