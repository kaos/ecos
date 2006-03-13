//==========================================================================
//
//        flexcan_remote.c
//
//        FlexCAN remote response buffer test
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
    cyg_can_remote_buf     rtr_buf;
    cyg_can_remote_buf     rtr_buf2;
    cyg_can_msgbuf_info    msgbox_info;
    cyg_can_mode           mode; 
    cyg_can_state          state;
    
    //
    // before we start configuring the CAN hardware we stop the chip
    //
    mode = CYGNUM_CAN_MODE_STOP;
    len = sizeof(mode);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MODE ,&mode, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    //
    // now check if FlexCAN modul is really stopped
    //
    len = sizeof(state);
    if (ENOERR != cyg_io_get_config(hDrvFlexCAN, CYG_IO_GET_CONFIG_CAN_STATE ,&state, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    } 
    
    if (state != CYGNUM_CAN_STATE_STOPPED)
    {
        CYG_TEST_FAIL_FINISH("Error stopping FlexCAN /dev/can0");
    }

    //
    // We would like to setup 2 remote buffers - check if we have enough
    // free message buffers
    //
    len = sizeof(msgbox_info);
    if (ENOERR != cyg_io_get_config(hDrvFlexCAN, CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO ,&msgbox_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    } 
    else
    {
        diag_printf("Message boxes available: %d    free: %d\n", 
                    msgbox_info.count, msgbox_info.free);
    }
    
    if (msgbox_info.free < 2)
    {
        CYG_TEST_FAIL_FINISH("Not enough free message buffers/dev/can0");
    }

   
    //
    // Setup the first remote response buffer for resception of standard
    // remote frames
    //
    rtr_buf.cfg_id      = CYGNUM_CAN_MSGBUF_REMOTE_BUF_ADD;
    rtr_buf.msg.id      = 0x7FF;
    rtr_buf.msg.ext     = CYGNUM_CAN_ID_STD;
    rtr_buf.msg.rtr     = CYGNUM_CAN_FRAME_DATA;
    rtr_buf.msg.dlc     = 1;
    rtr_buf.msg.data[0] = 0xAB;
    
    len = sizeof(rtr_buf);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rtr_buf, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    //
    // setup the second remote response buffer for reception of extended
    // remote frames
    // 
    rtr_buf2.cfg_id      = CYGNUM_CAN_MSGBUF_REMOTE_BUF_ADD;
    rtr_buf2.msg.id      = 0x800;
    rtr_buf2.msg.ext     = CYGNUM_CAN_ID_EXT;
    rtr_buf2.msg.rtr     = CYGNUM_CAN_FRAME_DATA;
    rtr_buf2.msg.dlc     = 4;
    rtr_buf2.msg.data[0] = 0xAB;
    
    len = sizeof(rtr_buf2);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rtr_buf2, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    if (rtr_buf.handle == CYGNUM_CAN_MSGBUF_NA)
    {
        CYG_TEST_FAIL_FINISH("No free message buffer available for /dev/can0");
    }
    
    //
    // now configuration is finished and we can start chip again
    //
    mode = CYGNUM_CAN_MODE_START;
    len = sizeof(mode);
    if (ENOERR != cyg_io_set_config(hDrvFlexCAN, CYG_IO_SET_CONFIG_CAN_MODE ,&mode, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    } 
    
    diag_printf("Test of FlexCAN remote response buffer configuration\n"
                "If a CAN node sends a remote request with ID 0x7FF\n"
                "or 0x800 then the FlexCAN modul should respond with\n"
                "data frames.\n");
    diag_printf("!!! This test can be stopped by sending a data frame with ID 0x100 !!!\n\n");
    
    len = sizeof(msgbox_info);
    if (ENOERR != cyg_io_get_config(hDrvFlexCAN, CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO ,&msgbox_info, &len))
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
            
        if (ENOERR != cyg_io_read(hDrvFlexCAN, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }
        
        if (0x100 == rx_event.msg.id)
        {
            CYG_TEST_PASS_FINISH("flexcan_remote test OK"); 
        }
        else
        {
            print_can_flags(rx_event.flags, "");
            print_can_msg(&rx_event.msg, "");    
        }
    }

              
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
    
    //
    // setup CAN baudrate 250 KBaud
    // We do not setup baud rate and use the default baud rate instead
    /*
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

// EOF flexcan_remote.c
