//==========================================================================
//
//        can_remote.c
//
//        CAN remote response buffer test
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Uwe Kindler
// Contributors:  Uwe Kindler
// Date:          2005-08-14
// Description:   CAN load test
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

// Package option requirements
#if defined(CYGOPT_IO_CAN_RUNTIME_MBOX_CFG)

// Package option requirements
#if defined(CYGOPT_IO_CAN_REMOTE_BUF)


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
cyg_io_handle_t    hCAN0;


//===========================================================================
//                          LOCAL FUNCTIONS
//===========================================================================
#include "can_test_aux.inl" // include CAN test auxiliary functions


//===========================================================================
// Main thread
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_uint32             len;
    cyg_can_event          rx_event;
    cyg_can_remote_buf     rtr_buf;
    cyg_can_filter         rx_filter;
    cyg_can_msgbuf_info    msgbox_info; 
    cyg_can_msgbuf_cfg     msgbox_cfg;

    //
    // We would like to setup 2 remote buffers - check if we have enough
    // free message buffers
    //
    len = sizeof(msgbox_info);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO ,&msgbox_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    } 
    else
    {
        diag_printf("\n\n\nMessage boxes available: %d    free: %d\n", 
                    msgbox_info.count, msgbox_info.free);
    }
    
    //
    // We have not enougth free message buffers, so we clear all message buffers now
    // and try again
    //
    if (msgbox_info.free < 2)
    {
        msgbox_cfg.cfg_id = CYGNUM_CAN_MSGBUF_RESET_ALL;
        len = sizeof(msgbox_cfg);
        if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_CAN_MSGBUF, &msgbox_cfg, &len))
        {
            CYG_TEST_FAIL_FINISH("Error clearing message buffers of /dev/can0");    
        }
        
        //
        // Now query number of free message boxes again. We need 3 free message boxes.
        // 2 message boxes for setup of remote response buffers and 1 message box for
        // setup of receive message box for CAN identifier 0x100
        //
        len = sizeof(msgbox_info);
        if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO ,&msgbox_info, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
        } 
        else
        {
            diag_printf("Message boxes available: %d    free: %d\n", 
                        msgbox_info.count, msgbox_info.free);    
        }
        
        if (msgbox_info.free < 3)
        {
            CYG_TEST_FAIL_FINISH("Not enough free message buffers available for /dev/can0");    
        }
        else
        {
            rx_filter.cfg_id = CYGNUM_CAN_MSGBUF_RX_FILTER_ADD;
            CYG_CAN_MSG_SET_STD_ID(rx_filter.msg, 0x100);
            
            len = sizeof(rx_filter);
            if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rx_filter, &len))
            {
                CYG_TEST_FAIL_FINISH("Error adding rx filter for CAN ID 0x100 for /dev/can0");
            } 
        } // if (msgbox_info.free < 3)
    } // if (msgbox_info.free < 2)
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
    //
    // Setup the first remote response buffer for resception of standard
    // remote frames
    //
    rtr_buf.cfg_id      = CYGNUM_CAN_MSGBUF_REMOTE_BUF_ADD;
    CYG_CAN_MSG_SET_PARAM(rtr_buf.msg, 0x7FF, CYGNUM_CAN_ID_STD, 1, CYGNUM_CAN_FRAME_DATA);
    CYG_CAN_MSG_SET_DATA(rtr_buf.msg, 0, 0xAB);
   
    len = sizeof(rtr_buf);
    if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rtr_buf, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
#endif

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    cyg_can_remote_buf     rtr_buf2;
    //
    // setup the second remote response buffer for reception of extended
    // remote frames
    // 
    rtr_buf2.cfg_id      = CYGNUM_CAN_MSGBUF_REMOTE_BUF_ADD;
    CYG_CAN_MSG_SET_PARAM(rtr_buf2.msg, 0x800, CYGNUM_CAN_ID_EXT, 4, CYGNUM_CAN_FRAME_DATA);
    CYG_CAN_MSG_SET_DATA(rtr_buf2.msg, 0, 0xCD);
   
    len = sizeof(rtr_buf2);
    if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rtr_buf2, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    if (rtr_buf.handle == CYGNUM_CAN_MSGBUF_NA)
    {
        CYG_TEST_FAIL_FINISH("No free message buffer available for /dev/can0");
    }
#endif
      
    diag_printf("\nTest of CAN remote response buffer configuration\n"
                "If a CAN node sends a remote request with ID 0x7FF (std. ID)\n"
                "or 0x800 (ext. ID) then the CAN driver should respond with\n"
                "data frames.\n\n");
    diag_printf("!!! This test can be stopped by sending a data frame\n"
                "with ID 0x100 !!!\n\n");
    
    len = sizeof(msgbox_info);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO ,&msgbox_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    else
    {
        diag_printf("Message boxes available: %d    free: %d\n", 
                    msgbox_info.count, msgbox_info.free);
    }
    
    while (1)
    {
        len = sizeof(rx_event); 
            
        if (ENOERR != cyg_io_read(hCAN0, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }
        
        if (0x100 == rx_event.msg.id)
        {
            CYG_TEST_PASS_FINISH("can_remote test OK"); 
        }
        else
        {
            print_can_flags(rx_event.flags, "");
            
            if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
            {
                print_can_msg(&rx_event.msg, "");   
            }
        }
    }         
}


void
cyg_start(void)
{
    CYG_TEST_INIT();
    
    //
    // open CAN device driver
    //
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN0)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }
    
    //
    // create the thread that accesses the CAN device driver
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

#else // CYGOPT_IO_CAN_REMOTE_BUF
#define N_A_MSG "Needs support for CAN remote response buffers"
#endif

#else // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
#define N_A_MSG "Needs support for CAN message buffer runtime configuration"
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

// EOF can_remote.c
