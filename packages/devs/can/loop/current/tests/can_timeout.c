//==========================================================================
//
//        can_timeout.c
//
//        Test CAN device driver timeouts
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
// Description:   Timeout test of CAN device driver
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

// timeout support required
#if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)


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
// Measure timeout time and compare it with RX timeout value
//===========================================================================
void check_timeout(cyg_io_handle_t hCAN, cyg_uint32 timeout)
{
    cyg_can_event          rx_event;
    cyg_can_timeout_info_t timeout_info;
    cyg_uint32             len;
    cyg_uint32             current_time_old;
    cyg_uint32             current_time_new;
    cyg_int32              diff_time;   
    
    

    timeout_info.tx_timeout = timeout;
    timeout_info.rx_timeout = timeout;
    if (ENOERR != cyg_io_set_config(hCAN, CYG_IO_SET_CONFIG_CAN_TIMEOUT ,&timeout_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error writing config of /dev/can0");
    }
    
    diag_printf("read timeout:  %d\n", timeout);
    
    //
    // if the return code is -EINTR then we know that a timout occured. Normally the
    // receive queue should be empty so we can expect a timeout here
    //
    current_time_old = (cyg_uint32)cyg_current_time();
    len = sizeof(rx_event);
    if (-EINTR == cyg_io_read(hCAN, &rx_event, &len))
    {
        current_time_new = (cyg_uint32)cyg_current_time();    
        diff_time = current_time_new -  current_time_old;
        diag_printf("measured time: %d\n\n", diff_time);
        diff_time = diff_time - timeout;
        
        //
        // If the measured value is smaller then the timeout value or if
        // the difference is > 2 the we treat this as an error
        //
        if ((diff_time < 0) || (diff_time > 2))
        {
            CYG_TEST_FAIL_FINISH("Measured timeout time differs from /dev/can0 RX timeout value.");  
        }
    }
    else
    {
        CYG_TEST_FAIL_FINISH("Timeout expected for /dev/can0");  
    }
}

//===========================================================================
//                             WRITER THREAD 
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_io_handle_t        hCAN0;
    cyg_uint32             len;
    cyg_uint32             blocking;
    cyg_can_timeout_info_t timeout_info;
   
    
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN0)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }
    
    //
    // the first thing we need to do is to switch to nonblocking calls because
    // function calls with timeout require nonblocking calls
    //
    blocking = 0;
    len = sizeof(blocking);
    if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_READ_BLOCKING ,&blocking, &len))
    {
        CYG_TEST_FAIL_FINISH("Error changing config of /dev/can0");
    }
      
    //
    // first we read the timeout configuration
    //
    len = sizeof(timeout_info);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_TIMEOUT ,&timeout_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    }
    
    //
    // now we check the default timeout and a number of different timeout values
    //
    check_timeout(hCAN0, timeout_info.rx_timeout);
    check_timeout(hCAN0, 10);
    check_timeout(hCAN0, 20);
    check_timeout(hCAN0, 50);
    check_timeout(hCAN0, 100);
    check_timeout(hCAN0, 200);
    check_timeout(hCAN0, 500);

        
    CYG_TEST_PASS_FINISH("can_timeout test OK");                     
}




void
cyg_start(void)
{
    CYG_TEST_INIT();
    
    //
    // create the thread which access the CAN device driver
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

#else // CYGOPT_IO_CAN_SUPPORT_TIMEOUTS
#define N_A_MSG "Timeout support required for IO/CAN"
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

// EOF can_timeout.c
