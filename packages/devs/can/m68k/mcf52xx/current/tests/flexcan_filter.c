//==========================================================================
//
//        flexcan_filter.c
//
//        FlexCAN message filter test
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
// Date:          2005-09-10
// Description:   FlexCAN hardware filter test
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
    cyg_can_mode           mode;
    cyg_uint8              i;
    cyg_can_hdi            hdi;
    cyg_can_msgbuf_info    msgbox_info;
    cyg_can_msgbuf_cfg     msgbox_cfg;

    
    len = sizeof(hdi);
    if (ENOERR != cyg_io_get_config(hDrvFlexCAN, CYG_IO_GET_CONFIG_CAN_HDI ,&hdi, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    } 
    
    //
    // Normally the FlexCAN modul should support message filters. So the
    // FULLCAN flag should be set - if it is not, we treat this as an error
    //
    if (!(hdi.support_flags & CYGNUM_CAN_HDI_FULLCAN))
    {
        CYG_TEST_FAIL_FINISH("/dev/can0 does not support message buffers");
    }
    
    //
    // We have free message boxes available and now we can setup the message 
    // filters, during this configuration process we set the FlexCAN modul into
    // stopped state
    //
    mode = CYGNUM_CAN_MODE_STOP;
    len = sizeof(mode);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MODE ,&mode, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    }
    
    //
    // Now reset message buffer configuration - this is mandatory bevore adding
    //
    msgbox_cfg.cfg_id = CYGNUM_CAN_MSGBUF_RESET_ALL;
    len = sizeof(msgbox_cfg);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&msgbox_cfg, &len))
    {
        CYG_TEST_FAIL_FINISH("Error restting message buffer configuration of /dev/can0");
    } 
    
    //
    // Now query number of available and free message boxes
    //
    len = sizeof(msgbox_info);
    if (ENOERR != cyg_io_get_config(hDrvFlexCAN, CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO ,&msgbox_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    } 
    
    //
    // if there are no free message boxes available then this is a failure
    //
    if (!msgbox_info.free)
    {
        CYG_TEST_FAIL_FINISH("No free message boxes available for /dev/can0");
    }
    
    //
    // Now device is stopped an we can setup all free message buffers
    // we setup as many standard CAN message filters as there are free
    // message buffers available.
    //
    for (i = 0; i < msgbox_info.free; ++i)
    {
        cyg_can_filter rx_filter;
        
        rx_filter.cfg_id  = CYGNUM_CAN_MSGBUF_RX_FILTER_ADD;
        rx_filter.msg.id  = i;
        
        if (i % 2)
        {
            rx_filter.msg.ext = CYGNUM_CAN_ID_EXT;
        }
        else
        {
            rx_filter.msg.ext = CYGNUM_CAN_ID_STD;
        }
    
        len = sizeof(rx_filter); 
        if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rx_filter, &len))
        {
            CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
        }
        else if (CYGNUM_CAN_MSGBUF_NA == rx_filter.handle)
        {
            CYG_TEST_FAIL_FINISH("Error setting up message filter for /dev/can0");
        }
    }
    
    //
    // Now message filters have been setup and we can start the FlexCAN modul again
    //
    mode = CYGNUM_CAN_MODE_START;
    len = sizeof(mode);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MODE ,&mode, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    diag_printf("Now try to send CAN messages. The device should only\n"
                "receive messages identifiers in the range of 0x00 to 0x%X.\n"
                "All CAN filters with an odd identifier will receive\n"
                "extended CAN messages. As soon as a standard message\n"
                "with ID 0 arrives, all message filters will be cleared\n\n", (msgbox_info.free - 1));
    
    //
    // Now receive messages until a message arrives with largest ID of all
    // available message filters
    //
    rx_event.msg.id = 1;
    while(rx_event.msg.id != 0)
    {
        len = sizeof(rx_event); 
            
        if (ENOERR != cyg_io_read(hDrvFlexCAN, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }      
        else if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
        {
            print_can_msg(&rx_event.msg, "");    
        } // if (ENOERR != cyg_io_read(hDrvFlexCAN, &rx_event, &len))
    } // while(1)
    
    //
    // Stop device in order to clear all message filters and to activate
    // the receive all message box
    //
    mode = CYGNUM_CAN_MODE_STOP;
    len = sizeof(mode);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MODE ,&mode, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    //
    // Now enable reception of all available CAN messages
    //
    cyg_can_filter rx_filter;
    rx_filter.cfg_id  = CYGNUM_CAN_MSGBUF_RX_FILTER_ALL;
    len = sizeof(rx_filter);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MSGBUF , &rx_filter, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    }
    
    //
    // Now we can start FlexCAN modul again
    //
    mode = CYGNUM_CAN_MODE_START;
    len = sizeof(mode);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MODE ,&mode, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    diag_printf("\nAll message filters have been cleared an now the device\n"
                "will receive any available CAN message identifier.\n"
                "Send a CAN message with ID 0x100 to stop this test.\n\n");
    
    //
    // Now receive messages until a message arrives with largest ID of all
    // available message filters
    //
    rx_event.msg.id = 1;
    while(rx_event.msg.id != 0x100)
    {
        len = sizeof(rx_event); 
            
        if (ENOERR != cyg_io_read(hDrvFlexCAN, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }      
        else if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
        {
            print_can_msg(&rx_event.msg, "");    
        } // if (ENOERR != cyg_io_read(hDrvFlexCAN, &rx_event, &len))
    } // while(1)
    
    CYG_TEST_PASS_FINISH("flexcan_filter test OK");
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
    }*/
    
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
		                
    cyg_thread_resume(can0_thread_data.hdl);
    
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

// EOF flexcan_filter.c
