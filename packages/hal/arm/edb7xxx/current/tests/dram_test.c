//==========================================================================
//
//        dram_test.c
//
//        eCos generic DRAM test code
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
// Date:          1999-10-22
// Description:   Tool used to test DRAM on eval boards
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

#include <cyg/hal/hal_arch.h>
#include CYGHWR_MEMORY_LAYOUT_H  // Memory layout

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_MINIMUM
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

extern char __bss_end;
int total_errors;
int run_errors;
int error_count;

#define MAX_ERRORS   5
#define NUM_RUNS     4
#define REFRESH_TIME 5

int decay_time[] = { 50, 100, 200, 500, 1000 };
#define NUM_DECAY sizeof(decay_time)/sizeof(decay_time[0])

// FUNCTIONS

static void
new_test(void)
{
    error_count = 0;
}

static void
report_error(void *addr, cyg_uint32 actual, cyg_uint32 expected)
{
    total_errors++;
    run_errors++;
    if (++error_count > MAX_ERRORS) return;
    diag_printf("   0x%08x: expected - 0x%08x, actual - 0x%08x\n", 
                addr, expected, actual);
}

// Fill longwords with their own address and verify

static void
addr_test(cyg_uint32 start, cyg_uint32 end)
{
    cyg_uint32 *mp;

    new_test();
    diag_printf("-- Address test\n");
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)mp;
    }
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)mp) {
            report_error(mp, *mp, (cyg_uint32)mp);
        }
    }
}

// Fill longwords with zeros

static void
zeros_test(cyg_uint32 start, cyg_uint32 end)
{
    cyg_uint32 *mp;

    new_test();
    diag_printf("-- Zeros test\n");
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)0;
    }
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)0) {
            report_error(mp, *mp, (cyg_uint32)0);
        }
    }
}

// Fill longwords with all ones

static void
ones_test(cyg_uint32 start, cyg_uint32 end)
{
    cyg_uint32 *mp;

    new_test();
    diag_printf("-- Ones test\n");
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)0xFFFFFFFF;
    }
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)0xFFFFFFFF) {
            report_error(mp, *mp, (cyg_uint32)0xFFFFFFFF);
        }
    }
}

// Fill longwords with a "walking" bit

static void
walking_bit_test(cyg_uint32 start, cyg_uint32 end)
{
    cyg_uint32 *mp;
    cyg_uint32 bit;

    new_test();
    diag_printf("-- Walking test\n");
    bit = 1;
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)bit;
        bit <<= 1;
        if (bit == 0) bit = 1;
    }
    bit = 1;
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)bit) {
            report_error(mp, *mp, (cyg_uint32)bit);
        }
        bit <<= 1;
        if (bit == 0) bit = 1;
    }
}

// Fill longwords with an alternating pattern

static void
pattern_test(cyg_uint32 start, cyg_uint32 end)
{
    cyg_uint32 *mp;
    cyg_uint32 pat;

    new_test();
    diag_printf("-- Pattern test\n");
    pat = 0x55555555;
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)pat;
    }
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)pat) {
            report_error(mp, *mp, (cyg_uint32)pat);
        }
    }
    pat = 0xAAAAAAAA;
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)pat;
    }
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)pat) {
            report_error(mp, *mp, (cyg_uint32)pat);
        }
    }
}

// Verify if refresh works

static void
refresh_test(cyg_uint32 start, cyg_uint32 end)
{
    cyg_uint32 *mp;
    cyg_uint32 pat;

    new_test();
    diag_printf("-- Refresh test\n");
    pat = 0x55555555;
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)pat;
    }
    cyg_thread_delay(REFRESH_TIME*100);
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)pat) {
            report_error(mp, *mp, (cyg_uint32)pat);
        }
    }
    pat = 0xAAAAAAAA;
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        *mp = (cyg_uint32)pat;
    }
    cyg_thread_delay(REFRESH_TIME*100);
    for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
        if (*mp != (cyg_uint32)pat) {
            report_error(mp, *mp, (cyg_uint32)pat);
        }
    }
}

// See how long we can "sleep" before refresh fails

static void
decay_test(cyg_uint32 start, cyg_uint32 end)
{
    cyg_uint32 *mp;
    cyg_uint32 pat;
    int i;

    diag_printf("-- Decay test\n");
    for (i = 0;  i < NUM_DECAY;  i++) {
        diag_printf("    ... %d.%02d sec delay\n", decay_time[i]/100, decay_time[i]%100);
        new_test();
        pat = 0x55555555;
        for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
            *mp = (cyg_uint32)pat;
        }
        cyg_thread_delay(decay_time[i]);
        for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
            if (*mp != (cyg_uint32)pat) {
                report_error(mp, *mp, (cyg_uint32)pat);
            }
        }
        pat = 0xAAAAAAAA;
        for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
            *mp = (cyg_uint32)pat;
        }
        cyg_thread_delay(decay_time[i]);
        for (mp = (cyg_uint32 *)start;  mp < (cyg_uint32 *)end;  mp++) {
            if (*mp != (cyg_uint32)pat) {
                report_error(mp, *mp, (cyg_uint32)pat);
            }
        }
        diag_printf("    ... %d errors\n", error_count);
    }
}


static void
dram_exercise(cyg_addrword_t p)
{
    cyg_uint32 start_DRAM, end_DRAM;
    int i;

    CYG_TEST_INIT();

    end_DRAM = CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE;
    start_DRAM = ((cyg_uint32)&__bss_end + 0x00000FFF) & 0xFFFFF000;
    diag_printf("DRAM test - start: 0x%08x, end: 0x%08x\n", start_DRAM, end_DRAM);

    for (i = 0;  i < NUM_RUNS;  i++) {
        diag_printf("\n*** Start run #%d of %d\n", i+1, NUM_RUNS);
        run_errors = 0;
        addr_test(start_DRAM, end_DRAM);
        ones_test(start_DRAM, end_DRAM);
        zeros_test(start_DRAM, end_DRAM);
        walking_bit_test(start_DRAM, end_DRAM);
        pattern_test(start_DRAM, end_DRAM);
        refresh_test(start_DRAM, end_DRAM);
        decay_test(start_DRAM, end_DRAM);
        diag_printf("\n*** End run, %d errors, %d total errors\n", run_errors, total_errors);
    }

    CYG_TEST_PASS_FINISH("DRAM testing complete");
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      dram_exercise,     // entry
                      0,                 // entry parameter
                      "DRAM test",       // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
} // cyg_package_start()

