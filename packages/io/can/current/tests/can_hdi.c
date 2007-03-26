//==========================================================================
//
//        can_hdi.c
//
//        CAN hardware description information test
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
// Date:          2007-03-22
// Description:   CAN hardware desciption information test
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
    cyg_can_hdi            hdi;
    cyg_can_msgbuf_info    msgbox_info;

    
    //
    // Query information about hardware of CAN controller
    //
    len = sizeof(hdi);
    if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_HDI ,&hdi, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
    } 
    
    //
    // Check type of CAN controller - type of supported CAN frames
    //
    diag_printf("\n\nSupported message formats:\n");
    if (hdi.support_flags & CYGNUM_CAN_HDI_FRAMETYPE_STD)
    {
        diag_printf("  Standard CAN (2.0A):         yes\n");
        diag_printf("  Extended CAN (2.0B):         no\n");
    } 
    else if (hdi.support_flags & CYGNUM_CAN_HDI_FRAMETYPE_EXT_PASSIVE)
    {
    	diag_printf("  Standard CAN (2.0A):         yes\n");
        diag_printf("  Extended CAN (2.0B):         passive\n"); 
    }
    else if (hdi.support_flags & CYGNUM_CAN_HDI_FRAMETYPE_EXT_ACTIVE)
    {
    	diag_printf("  Standard CAN (2.0A):         yes\n");
        diag_printf("  Extended CAN (2.0B):         yes\n"); 
    }
    
    //
    // Check if this is a FullCAN controller
    //
    diag_printf("\nController type:               ");
    if (hdi.support_flags & CYGNUM_CAN_HDI_FULLCAN)
    {
        diag_printf("FullCAN\n");
        //
        // FullCAN means the controller supports a number of message buffers.
        // Now query number of available and free message buffers
        //
        len = sizeof(msgbox_info);
        if (ENOERR != cyg_io_get_config(hCAN0, CYG_IO_GET_CONFIG_CAN_MSGBUF_INFO ,&msgbox_info, &len))
        {
             CYG_TEST_FAIL_FINISH("Error reading config of /dev/can0");
        } 
    
        diag_printf("  Message buffers:             %d\n", msgbox_info.count);
        diag_printf("  Message buffers free:        %d\n", msgbox_info.free);
    }
    else
    {
    	diag_printf("BasicCAN\n");	
    }
    	
    //
    // Check if automatic baudrate detection is supported
    //
    if (hdi.support_flags & CYGNUM_CAN_HDI_AUTBAUD)
    {
    	diag_printf("\nAutomatic baudrate detection:  supported\n"); 
    }
    
    //
    // Check if driver supports timestamps
    //
    if (hdi.support_flags & CYGNUM_CAN_HDI_TIMESTAMP)
    {
    	diag_printf("Timestamps:                    supported\n"); 
    }
    
    diag_printf("\n\n"); 
    CYG_TEST_PASS_FINISH("can_hdi test OK");
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

//---------------------------------------------------------------------------
// EOF can_hdi.c
