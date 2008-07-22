//==========================================================================
//
//        can_extended_cfg.c
//
//        Test of extended CAN configuration keys for LPC2xxx CAN driver
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
// Date:          2006-06-20
// Description:   LPC2xxx CAN extended configuration test
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
#include "can_test_aux.inl"

#if defined(CYGOPT_DEVS_CAN_LPC2XXX_EXTENDED_CFG_KEYS)
#include <cyg/io/can_lpc2xxx.h>

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
//                             READER THREAD 
//===========================================================================
void can0_thread(cyg_addrword_t data)
{
    cyg_uint32              len;
    cyg_can_event           rx_event;
    cyg_can_filtergroup_cfg acc_filt_grp;
    cyg_can_msgbuf_cfg      msgbox_cfg;
    
    //
    // First we reset message buffer configuration - this is mandatory bevore starting
    // message buffer runtime configuration. This call clears/frees all message buffers
    // The CAN controller cannot receive any further CAN message after this call
    //
    msgbox_cfg.cfg_id = CYGNUM_CAN_MSGBUF_RESET_ALL;
    len = sizeof(msgbox_cfg);
    if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_CAN_MSGBUF ,&msgbox_cfg, &len))
    {
        CYG_TEST_FAIL_FINISH("Error resetting message buffer configuration of /dev/can0");
    } 
    
    //
    // Now we setup two different acceptance filter groups. Acceptance filter
    // groups are not part of the CAN I/O layer and are a LPC2xxx specific
    // feature. You should not use appcetance filter groups if you would like
    // to code portable eCos CAN applications
    //
#ifdef CYGOPT_IO_CAN_STD_CAN_ID    
    acc_filt_grp.ext            = CYGNUM_CAN_ID_STD;
    acc_filt_grp.lower_id_bound = 0x100;
    acc_filt_grp.upper_id_bound = 0x110;
    len = sizeof(acc_filt_grp);
    
    if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_LPC2XXX_ACCFILT_GROUP ,&acc_filt_grp, &len))
    {
        CYG_TEST_FAIL_FINISH("Error adding filter group to /dev/can0");
    } 
#endif // CYGOPT_IO_CAN_STD_CAN_ID
    
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID   
    acc_filt_grp.ext            = CYGNUM_CAN_ID_EXT;
    acc_filt_grp.lower_id_bound = 0x2000;
    acc_filt_grp.upper_id_bound = 0x2200;
    len = sizeof(acc_filt_grp);
    
    if (ENOERR != cyg_io_set_config(hCAN0, CYG_IO_SET_CONFIG_LPC2XXX_ACCFILT_GROUP ,&acc_filt_grp, &len))
    {
        CYG_TEST_FAIL_FINISH("Error adding filter group to /dev/can0");
    } 
#endif // CYGOPT_IO_CAN_STD_CAN_ID
    
    diag_printf("\n\nNow try to send CAN messages. The device should only\n"
                    "receive standard messages identifiers in the range of 0x100\n"
                    "to 0x110 and/or extended identifiers in the range 0x2000 to\n"
                    "0x2200. As soon as a standard message with ID 0x110 or an\n"
                    "extended message with ID 0x2200 arrives, the test finishes\n\n");
    
    while (1)
    {
        len = sizeof(rx_event); 
            
        if (ENOERR != cyg_io_read(hCAN0, &rx_event, &len))
        {
            CYG_TEST_FAIL_FINISH("Error reading from /dev/can0");
        }
        else
        {
            print_can_flags(rx_event.flags, "");
            
            if (rx_event.flags & CYGNUM_CAN_EVENT_RX)
            {
                print_can_msg(&rx_event.msg, "");
#ifdef CYGOPT_IO_CAN_STD_CAN_ID
                if (rx_event.msg.id == 0x110)
                {
                    CYG_TEST_PASS_FINISH("LPC2xxx CAN message filter group test OK");
                }
#endif // CYGOPT_IO_CAN_STD_CAN_ID

#ifdef CYGOPT_IO_CAN_EXT_CAN_ID          
                if (rx_event.msg.id == 0x2200)
                {
                    CYG_TEST_PASS_FINISH("LPC2xxx CAN message filter group test OK");
                }
#endif // CYGOPT_IO_CAN_EXT_CAN_ID  
                
                if (((rx_event.msg.id > 0x110) && (rx_event.msg.id < 0x2000))
                   || (rx_event.msg.id > 0x2200))
                {
                    CYG_TEST_FAIL_FINISH("Received CAN identifier outside filter group bounds");
                }
            }
        } 
    } // while (1)
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

#else // CYGOPT_DEVS_CAN_LPC2XXX_EXTENDED_CFG_KEYS
#define N_A_MSG "Needs support for extended LPC2xxx CAN configuration keys" 
#endif

#else // CYGOPT_IO_CAN_RUNTIME_MBOX_CFG
#define N_A_MSG "Needs CAN message box runtime confuguration support"
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

// EOF flexcan_remote.c
