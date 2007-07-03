//==========================================================================
//
//        can_multichan_tx.c
//
//        CAN TX test for multiple CAN channels
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
// Description:   CAN TX test for multiple CAN controller channels
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
cyg_thread_entry_t can_thread;
thread_data_t      can0_thread_data;
cyg_io_handle_t    hCAN_Tbl[4];


//===========================================================================
//                             READER THREAD 
//===========================================================================
void can_thread(cyg_addrword_t data)
{
    cyg_uint32              len;
    cyg_can_message         tx_msg;
    cyg_can_info_t          can_info;
    cyg_can_baud_rate_t     baud;
    cyg_uint8               i = 0;
    cyg_uint8               j = 0;

    //
    // Check that all cannels have the same baudrate
    //
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN0
    len = sizeof(can_info);
    if (ENOERR != cyg_io_get_config(hCAN_Tbl[0], CYG_IO_GET_CONFIG_CAN_INFO, &can_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading baudrate of CAN channel 0");    
    } 
    else
    {
        baud = can_info.baud;
        ++i;
    }
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN1
    len = sizeof(can_info);
    if (ENOERR != cyg_io_get_config(hCAN_Tbl[1], CYG_IO_GET_CONFIG_CAN_INFO, &can_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading baudrate of CAN channel 1");    
    }
    else
    {
        if (i && (baud != can_info.baud))
        {
            CYG_TEST_FAIL_FINISH("Error - different baudrates for CAN channel 0 and 1");               
        }
        baud = can_info.baud;
        ++i;
    }
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN2
    len = sizeof(can_info);
    if (ENOERR != cyg_io_get_config(hCAN_Tbl[2], CYG_IO_GET_CONFIG_CAN_INFO, &can_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading baudrate of CAN channel 2");    
    } 
    else
    {
        if (i && (baud != can_info.baud))
        {
            CYG_TEST_FAIL_FINISH("Error - different baudrates for CAN channel 1 and 2");               
        }
        baud = can_info.baud;
        ++i;
    }
#endif
    
#ifdef CYGPKG_DEVS_CAN_LPC2XXX_CAN3
    len = sizeof(can_info);
    if (ENOERR != cyg_io_get_config(hCAN_Tbl[3], CYG_IO_GET_CONFIG_CAN_INFO, &can_info, &len))
    {
        CYG_TEST_FAIL_FINISH("Error reading baudrate of CAN channel 3");    
    }
    else
    {
        if (i && (baud != can_info.baud))
        {
            CYG_TEST_FAIL_FINISH("Error - different baudrates for CAN channel 2 and 3");               
        }
        baud = can_info.baud;
        ++i;
    }
#endif
    
    diag_printf("\n\nYou should no receive 4 CAN messages from each active CAN channel\n");
    
    //
    // Now each CAN channel sends 10 CAN messages
    //
    for (i = 0; i < 4; ++i)
    {
        if (hCAN_Tbl[i])
        {
            CYG_CAN_MSG_SET_PARAM(tx_msg, i * 0x100, CYGNUM_CAN_ID_STD, 4, CYGNUM_CAN_FRAME_DATA);
            tx_msg.data.dwords[0] = 0;
            tx_msg.data.dwords[1] = 0;
            char err_msg[64];
            diag_snprintf(err_msg, sizeof(err_msg), "Error sending TX using CAN channel %d", i);
            for (j = 0; j < 4; ++j)
            {
                tx_msg.id = i * 0x100 + j; 
                tx_msg.data.bytes[0] = j;
                len = sizeof(tx_msg);
                if (ENOERR != cyg_io_write(hCAN_Tbl[i], &tx_msg, &len))
                {
                    CYG_TEST_FAIL_FINISH(err_msg);     
                }
            }
        } //  if (hCAN_Tbl[i])
    } // for (i = 0; i < 4; ++i)
    
    CYG_TEST_PASS_FINISH("LPC2xxx CAN multi channel TX test OK");
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
// EOF can_multichan_tx.c
