//==========================================================================
//
//        can_callback.c
//
//        CAN driver test of callback on event
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
// Author(s):     Uwe Kindler, Alexey Shusharin
// Contributors:  Uwe Kindler, Alexey Shusharin
// Date:          2007-08-23
// Description:   CAN driver test of callback on event
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
#if defined(CYGOPT_IO_CAN_SUPPORT_CALLBACK)

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
thread_data_t      can_thread_data;

cyg_mutex_t can_lock;
cyg_cond_t can_wait;

cyg_io_handle_t    hCAN0;
cyg_io_handle_t    hCAN1;

//===========================================================================
//                          LOCAL FUNCTIONS
//===========================================================================
#include "can_test_aux.inl" // include CAN test auxiliary functions


//===========================================================================
//                           CALLBACK FUNCTION
//===========================================================================

static void callback_func(cyg_uint16 flags, CYG_ADDRWORD data)
{
    if (data == ((CYG_ADDRWORD) hCAN0) && (flags & CYGNUM_CAN_EVENT_RX))
    {
        // Wake up thread
        cyg_cond_signal(&can_wait);
    }
}

//===========================================================================
//                             READER THREAD 
//===========================================================================
void can_thread(cyg_addrword_t data)
{
    cyg_uint32              len;
    cyg_can_callback_cfg    callback_cfg;
    cyg_can_message         tx_msg;
    cyg_bool_t              wait_res;
    
    //
    // open CAN0 device driver
    //
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN0)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }
    
    //
    // open CAN1 device driver
    //
    if (ENOERR != cyg_io_lookup("/dev/can1", &hCAN1)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can1");
    }
    
    //
    // configure CAN0 callback
    //
    len = sizeof(callback_cfg);
    callback_cfg.flag_mask = 0xFFFF;
    callback_cfg.data = (CYG_ADDRWORD) hCAN0;
    callback_cfg.callback_func = callback_func;
    
    if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_CAN_CALLBACK,
            &callback_cfg, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    }
    
    //
    // transmit message from CAN1 to CAN0
    //
    tx_msg.id  = 0x001;
    tx_msg.ext = CYGNUM_CAN_ID_STD;
    tx_msg.rtr = CYGNUM_CAN_FRAME_DATA;
    tx_msg.dlc = 0;
    len = sizeof(tx_msg); 
    
    if (ENOERR != cyg_io_write(hCAN1, &tx_msg, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing message to /dev/can1");
    }
    
    //
    // Wait CAN0 callback
    //
    cyg_mutex_lock(&can_lock);
    
    wait_res = cyg_cond_timed_wait(&can_wait, 100);
    
    cyg_mutex_unlock(&can_lock);
    
    //
    // If result of wait is a signal operation, test is successed
    // If timeout - test is failed, because callback_func() hasn't been
    // called with correct parameters
    //
    if(wait_res)
    {
        CYG_TEST_PASS_FINISH("can_callback test OK");
    }
    else
    {
        CYG_TEST_FAIL_FINISH("can_callback test FAILED");
    }
}


void
cyg_start(void)
{
    CYG_TEST_INIT();
    
    cyg_mutex_init(&can_lock);
    cyg_cond_init(&can_wait, &can_lock);
    
    //
    // create the main thread
    //
    cyg_thread_create(4, can_thread, 
                        (cyg_addrword_t) 0,
                        "can_thread", 
                        (void *) can_thread_data.stack, 
                        1024 * sizeof(long),
                        &can_thread_data.hdl, 
                        &can_thread_data.obj);
                        
    cyg_thread_resume(can_thread_data.hdl);
    
    cyg_scheduler_start();
}

#else // #if defined(CYGOPT_IO_CAN_SUPPORT_CALLBACK)
#define N_A_MSG "Needs callback support"
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

// EOF can_callback.c
