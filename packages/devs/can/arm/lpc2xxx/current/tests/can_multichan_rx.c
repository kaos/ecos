//==========================================================================
//
//        can_multichan_rx.c
//
//        CAN RX test for multiple CAN channels
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
// Description:   CAN RX test for multiple CAN controller channels
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
#if defined(CYGOPT_IO_CAN_SUPPORT_NONBLOCKING)

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
cyg_thread_entry_t can_thread;
thread_data_t      can0_thread_data;
cyg_io_handle_t    hCAN_Tbl[4];


//===========================================================================
// Setup acceptance filter
//===========================================================================
void can_setup_channel(cyg_io_handle_t hCAN, unsigned char Channel)
{
    cyg_uint32              len;
    cyg_can_msgbuf_cfg      msgbox_cfg;
    cyg_uint8               i;
    cyg_uint32              blocking;
    
    //
    // First we reset message buffer configuration - this is mandatory bevore starting
    // message buffer runtime configuration. This call clears/frees all message buffers
    // The CAN controller cannot receive any further CAN message after this call
    //
    msgbox_cfg.cfg_id = CYGNUM_CAN_MSGBUF_RESET_ALL;
    len = sizeof(msgbox_cfg);
    if (ENOERR != cyg_io_set_config(hCAN, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&msgbox_cfg, &len))
    {
        CYG_TEST_FAIL_FINISH("Error resetting message buffer configuration of /dev/can0");
    } 
    
    //
    // Now setup 10 message filters for this channel
    //
    for (i = 0; i < 10; ++i)
    {
        cyg_can_filter rx_filter;    
        
        rx_filter.cfg_id  = CYGNUM_CAN_MSGBUF_RX_FILTER_ADD;
        rx_filter.msg.id  = Channel * 0x100 + i;
        rx_filter.msg.ext = CYGNUM_CAN_ID_STD;
  
        len = sizeof(rx_filter); 
        if (ENOERR != cyg_io_set_config(hCAN, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rx_filter, &len))
        {
            CYG_TEST_FAIL_FINISH("Error writing config");
        }
        else if (CYGNUM_CAN_MSGBUF_NA == rx_filter.handle)
        {
            CYG_TEST_FAIL_FINISH("Error setting up message filter");
        }        
    }
    
    //
    // Now set driver into nonblocking mode because the receiver thread will
    // receive the messages from all channels
    //
    blocking = 0;
    len = sizeof(blocking);
    if (ENOERR != cyg_io_set_config(hCAN, CYG_IO_SET_CONFIG_READ_BLOCKING ,&blocking, &len))
    {
        CYG_TEST_FAIL_FINISH("Error setting channel into nonblocking mode");
    } 
    
    //
    // If timeouts are supported we need to setup a timeout value of 0 because
    // the driver should return immediatelly if no message is available
    //
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMEOUTS
    cyg_can_timeout_info_t timeouts;
    
    timeouts.rx_timeout = 0;
    timeouts.tx_timeout = 0;
    len = sizeof(timeouts);
    if (ENOERR != cyg_io_set_config(hCAN, CYG_IO_SET_CONFIG_CAN_TIMEOUT ,&timeouts, &len))
    {
        CYG_TEST_FAIL_FINISH("Error setting timeout for channel");
    } 
#endif
}


//===========================================================================
//                             READER THREAD 
//===========================================================================
void can_thread(cyg_addrword_t data)
{
    cyg_uint32              len;
    cyg_uint8               i = 0;

    //
    // Check that all cannels have the same baudrate
    //
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN0
    can_setup_channel(hCAN_Tbl[0], 0);
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN1
    can_setup_channel(hCAN_Tbl[1], 1);
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN2
    can_setup_channel(hCAN_Tbl[2], 2);
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN3
    can_setup_channel(hCAN_Tbl[3], 3);
#endif
    
    diag_printf("\n\nThis test uses all available CAN channels for reception\n"
                "of CAN standard messages. The following messages will be received:\n\n");
    
    for (i = 0; i < 4; ++i)
    {
        if (hCAN_Tbl[i])
        {
            diag_printf("CAN channel %d: msg: 0x%03x - 0x%03x\n", i, i * 0x100, i * 0x100 + 9);
        }
    }
    
    diag_printf("\n\nYou can stop this test by sending a message with ID 0xX09\n");
    
    while (1)
    {
        for (i = 0; i < 4; ++i)
        {
            if (hCAN_Tbl[i])
            {
                Cyg_ErrNo     ret;
                cyg_can_event rx_event; 
                
                len = sizeof(rx_event);
                ret = cyg_io_read(hCAN_Tbl[i], &rx_event, &len);
                if ((ret == -EAGAIN) || (ret == -EINTR))
                {
                    continue;    
                }
                
                if (ENOERR != ret)
                {
                    CYG_TEST_FAIL_FINISH("Error reading from channel");
                }
                else
                {
                    diag_printf("Channel %d events: ", i);
                    print_can_flags(rx_event.flags, "");
                    if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
                    {
                        print_can_msg(&rx_event.msg, "");
                        if ((rx_event.msg.id & 9) == 9)
                        {
                            CYG_TEST_PASS_FINISH("LPC2xxx CAN multi channel RX test OK");    
                        }
                    } // if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
                }
            } // if (hCAN_Tbl[i])
        } // for (i = 0; i < 4; ++i)
    } // while (1)
}



void
cyg_start(void)
{
    CYG_TEST_INIT();

#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN0
    //
    // open CAN device driver
    //
    if (ENOERR != cyg_io_lookup(CYGPKG_DEVS_CAN_LPC2XXX_CAN0_NAME, &hCAN_Tbl[0])) 
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 0");
    }
#else
    hCAN_Tbl[0] = 0;  
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN1
    //
    // open CAN device driver
    //
    if (ENOERR != cyg_io_lookup(CYGPKG_DEVS_CAN_LPC2XXX_CAN1_NAME, &hCAN_Tbl[1])) 
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 1");
    }
#else
    hCAN_Tbl[1] = 0;  
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN2
    //
    // open CAN device driver
    //
    if (ENOERR != cyg_io_lookup(CYGPKG_DEVS_CAN_LPC2XXX_CAN2_NAME, &hCAN_Tbl[2])) 
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 2");
    }
#else
    hCAN_Tbl[2] = 0;  
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN3
    //
    // open CAN device driver
    //
    if (ENOERR != cyg_io_lookup(CYGPKG_DEVS_CAN_LPC2XXX_CAN3_NAME, &hCAN_Tbl[3])) 
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 3");
    }
#else
    hCAN_Tbl[3] = 0;  
#endif
    
    //
    // create the thread that accesses the CAN device driver
    //
    cyg_thread_create(4, can_thread, 
                        (cyg_addrword_t) 0,
                        "can0_thread", 
                        (void *) can0_thread_data.stack, 
                        1024 * sizeof(long),
                        &can0_thread_data.hdl, 
                        &can0_thread_data.obj);
                        
    cyg_thread_resume(can0_thread_data.hdl);
    
    cyg_scheduler_start();
}
#else // CYGOPT_IO_CAN_SUPPORT_NONBLOCKING
#define N_A_MSG "Needs support for nonblocking calls"
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
// EOF can_multichan_rx.c
