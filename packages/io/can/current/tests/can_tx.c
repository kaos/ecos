//==========================================================================
//
//        can_tx.c
//
//        Simple CAN TX test
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
// Date:          2007-01-08
// Description:   Simple write test of CAN driver
//####DESCRIPTIONEND####


//===========================================================================
//                                INCLUDES
//===========================================================================
#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>         // test macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_diag.h>

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


//===========================================================================
//                          LOCAL FUNCTIONS
//===========================================================================
#include "can_test_aux.inl" // include CAN test auxiliary functions


//===========================================================================
//                             WRITER THREAD 
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_io_handle_t hCAN0;
    cyg_uint32      i;
    cyg_uint32      len;
    CYG_CAN_MSG_INIT(tx_msg, 0x001, CYGNUM_CAN_ID_STD, 1, CYGNUM_CAN_FRAME_DATA);
    
    //
    // Open device and check return value
    //
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN0)) 
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/can0");
    }

#ifdef CYGOPT_IO_CAN_STD_CAN_ID    
    //
    // Now send 1000 messages with standard identifier.
    // Each message contains the message number in its identifier 
    // and in the first data byte
    //
    for (i = 0; i < 1000; ++i)
    {
        CYG_CAN_MSG_SET_STD_ID(tx_msg, i);
        CYG_CAN_MSG_SET_DATA(tx_msg, 0, i);
        len = sizeof(tx_msg);
        
        //
        // Each message with an odd identifier should be a remote request message
        //
        if (i % 2)
        {
            CYG_CAN_MSG_SET_FRAME_TYPE(tx_msg, CYGNUM_CAN_FRAME_RTR); 
        }
        else
        {
            CYG_CAN_MSG_SET_FRAME_TYPE(tx_msg, CYGNUM_CAN_FRAME_DATA); 
        }
        
        //
        // Sending CAN messages is blocking so the thread waits until there is
        // space in the tx queue
        //
        if (ENOERR != cyg_io_write(hCAN0, &tx_msg, &len))
        {
            CYG_TEST_FAIL_FINISH("Error writing to /dev/can0");
        }    
    }
#endif

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID    
    //
    // Now send 1000 messages with extended identifier.
    // Each message contains the message number in its identifier 
    // and in the first data byte
    //
    for (i = 0; i < 1000; ++i)
    {
        CYG_CAN_MSG_SET_EXT_ID(tx_msg, i + 0x800);
        CYG_CAN_MSG_SET_DATA(tx_msg, 0, i);
        CYG_CAN_MSG_SET_DATA_LEN(tx_msg, 8);
        len = sizeof(tx_msg);
        
        //
        // Each message with an odd identifier should be a remote request message
        //
        if (i % 2)
        {
            CYG_CAN_MSG_SET_FRAME_TYPE(tx_msg, CYGNUM_CAN_FRAME_RTR); 
        }
        else
        {
            CYG_CAN_MSG_SET_FRAME_TYPE(tx_msg, CYGNUM_CAN_FRAME_DATA); 
        }
        
        //
        // Sending CAN messages is blocking so the thread waits until there is
        // space in the tx queue
        //
        if (ENOERR != cyg_io_write(hCAN0, &tx_msg, &len))
        {
            CYG_TEST_FAIL_FINISH("Error writing to /dev/can0");
        }    
    }
#endif

    
    CYG_TEST_PASS_FINISH("CAN TX test OK");
}


void
cyg_start(void)
{
    CYG_TEST_INIT();
    
    //
    // create the threads that access the CAN device driver
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

// EOF can_tx.c
