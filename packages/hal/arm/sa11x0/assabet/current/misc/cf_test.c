//==========================================================================
//
//        cf_test.c
//
//        SA1110/Assabet - Compact Flash test
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          2000-06-05
// Description:   Tool used to test CF stuff
//####DESCRIPTIONEND####

#include <pkgconf/system.h>
#ifdef CYGPKG_IO_PCMCIA
#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros

#include <cyg/io/pcmcia.h>

#include <cyg/hal/hal_sa11x0.h>  // Board definitions
#include <cyg/hal/assabet.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define STACK_SIZE 4096
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

// FUNCTIONS

static void
cyg_test_exit(void)
{
    while (TRUE) ;
}

static void
cf_test(cyg_addrword_t p)
{
    int len, ptr;
    unsigned char buf[256];
    unsigned char *en0;
    unsigned long cor = 0, regs;
    struct cf_cftable cftable;
    struct cf_config config;
    struct cf_slot *slot;

    diag_printf("CF test here\n");
    cf_init();
    slot = cf_get_slot(0);
    en0 = slot->io;
    while (1) {
        if (slot->state == CF_SLOT_STATE_Inserted) {
            diag_printf("Compact Flash card inserted!\n");
            cf_change_state(slot, CF_SLOT_STATE_Ready);
            diag_dump_buf(slot->attr, slot->attr_length);
            len = sizeof(buf);
            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_MANFID, buf, &len, &ptr)) {
                diag_printf("CIS 0x20:\n");
                diag_dump_buf(buf, len);
            }
            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_VERS_1, buf, &len, &ptr)) {
                diag_printf("CIS 0x15:\n");
                diag_dump_buf(buf, len);
            }
            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_CONFIG, buf, &len, &ptr)) {
                diag_printf("CIS 0x1A:\n");
                diag_dump_buf(buf, len);
            }
            if (cf_parse_config(buf, len, &config)) {
                cor = config.base;
                diag_printf("COR = %x\n", cor);
            }
            ptr = 0;
            while (cf_get_CIS(slot, CF_CISTPL_CFTABLE_ENTRY, buf, &len, &ptr)) {
                diag_printf("CIS 0x1B:\n");
                diag_dump_buf(buf, len);
                if (cf_parse_cftable(buf, len, &cftable)) {
                    regs = cftable.io_space.base[0];
                    cf_set_COR(slot, cor, cftable.cor);
                    en0[regs+0x00] = 0x21;
                    en0[regs+0x0E] = 0x49;
                    en0[regs+0x00] = 0x61;
                    diag_dump_buf(en0+regs, 0x10);
                    en0[regs+0x00] = 0x21;
                    diag_dump_buf(en0+regs, 0x10);
                }
            }
            break;
        }
    }
    cyg_test_exit();
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      cf_test,           // entry
                      0,                 // entry parameter
                      "CompactFlash test",  // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
} // cyg_package_start()
#endif
