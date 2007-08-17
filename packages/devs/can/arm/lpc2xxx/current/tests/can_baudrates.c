//==========================================================================
//
//        can_baudrates.c
//
//        CAN test of all supported baudrates
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
// Description:   CAN LPC2xxx baudrate test
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
cyg_io_handle_t    hCAN;


//
// The table of baudrates to test
//
static cyg_can_baud_rate_t baudrate_tbl[9] =
{
    CYGNUM_CAN_KBAUD_10,
    CYGNUM_CAN_KBAUD_20,
    CYGNUM_CAN_KBAUD_50,
    CYGNUM_CAN_KBAUD_100,
    CYGNUM_CAN_KBAUD_125,
    CYGNUM_CAN_KBAUD_250,
    CYGNUM_CAN_KBAUD_500,
    CYGNUM_CAN_KBAUD_800,
    CYGNUM_CAN_KBAUD_1000,
};

//
// String table forprinting supported baudrates
//
static char* baudrate_strings_tbl[9] =
{
    "10",
    "20",
    "50",
    "100",
    "125",
    "250",
    "500",
    "800",
    "1000",
};


//===========================================================================
// Thread 0
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_uint32          len;
    cyg_can_event       rx_event;
    cyg_uint32          i;
    cyg_can_info_t      can_info;

    diag_printf("\n\nWhen the LPC2xxx driver selects a new baudrate then you need\n"
                "to setup your hardware to the new baudrate and send one CAN\n"
                "single CAN standard message.\n");
    //
    // Test all supported baudrates
    //
    for (i = 0; i < 9; ++i)
    {
        diag_printf("\n\nBaudrate: %s Kbaud\n", baudrate_strings_tbl[i]);
        can_info.baud = baudrate_tbl[i];
        len = sizeof(can_info);
        if (ENOERR != cyg_io_set_config(hCAN, CYG_IO_SET_CONFIG_CAN_INFO, &can_info, &len))
        {
            diag_printf("not supported\n");
            continue;
        } 
        else
        {
            diag_printf("waiting for CAN message...\n");    
        }
        
        len = sizeof(rx_event);  
        //
        // First receive CAN event from real CAN hardware
        //
        len = sizeof(rx_event);
        if (ENOERR != cyg_io_read(hCAN, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from channel 0");   
        }
        
        if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
        {
            print_can_msg(&rx_event.msg, "RX chan 1:");
        } // if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
        else  
        {
            print_can_flags(rx_event.flags, ""); 
            CYG_TEST_FAIL_FINISH("Rx message expected");   
        }
    } // for (i = 0; i < 9; ++i)
    
    CYG_TEST_PASS_FINISH("CAN baudrate test OK");    
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
    if (ENOERR != cyg_io_lookup(CYGPKG_DEVS_CAN_LPC2XXX_CAN0_NAME, &hCAN)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 0");
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
