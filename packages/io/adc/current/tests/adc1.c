//==========================================================================
//
//        adc1.c
//
//        ADC test
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2009 Free Software Foundation, Inc.                        
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
// Author(s):     Simon Kallweit <simon.kallweit@intefo.ch>
// Contributors:
// Date:          2009-03-02
// Description:   ADC test
//####DESCRIPTIONEND####

#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_arch.h>

// Package requirements
#if defined(CYGPKG_IO_ADC) && defined(CYGPKG_KERNEL)

#include <pkgconf/kernel.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/adc.h>

// Package option requirements
#if defined(CYGFUN_KERNEL_API_C)

#include <cyg/kernel/kapi.h>

#define TEST_RATE                   100 // Data acquisition rate for test
#define TEST_SAMPLES                10  // Number of samples to acquire

// Thread data
cyg_handle_t thread_handle;
cyg_thread thread_data;
cyg_uint8 thread_stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];

static void
test_channel(const char *dev, cyg_io_handle_t channel)
{
    Cyg_ErrNo res;
    cyg_uint32 cfg_data;
    cyg_uint32 len;
    cyg_adc_sample_t sample;
    int count;
    
    diag_printf("\nTesting ADC channel '%s'\n", dev);
    
    // Disable channel
    res = cyg_io_set_config(channel, CYG_IO_SET_CONFIG_ADC_DISABLE, 0, 0);
    if (res != ENOERR)
        CYG_TEST_FAIL_FINISH("Failed to disable ADC channel");
    
    // Make channel non-blocking
    cfg_data = 0;
    len = sizeof(cfg_data);
    res = cyg_io_set_config(channel, CYG_IO_SET_CONFIG_READ_BLOCKING,
                            &cfg_data, &len);
    if (res != ENOERR)
        CYG_TEST_FAIL_FINISH("Failed to make ADC channel non-blocking");

    // Set channel sampling rate
    cfg_data = TEST_RATE;
    len = sizeof(cfg_data);
    res = cyg_io_set_config(channel, CYG_IO_SET_CONFIG_ADC_RATE,
                            &cfg_data, &len);
    if (res != ENOERR)
        CYG_TEST_FAIL_FINISH("Failed to set ADC channel sampling rate");
    
    // Flush channel
    do {
        len = sizeof(sample);
        res = cyg_io_read(channel, &sample, &len);
    } while (res == ENOERR);
    
    // Enable channel
    res = cyg_io_set_config(channel, CYG_IO_SET_CONFIG_ADC_ENABLE, 0, 0);
    if (res != ENOERR)
        CYG_TEST_FAIL_FINISH("Failed to enable ADC channel");
    
    // Read from channel
    count = 0;
    while (count < TEST_SAMPLES) {
        len = sizeof(sample);
        res = cyg_io_read(channel, &sample, &len);
        if (res == ENOERR) {
            diag_printf("%d\n", sample);
            count++;
        }
    }
    
    // Disable channel
    res = cyg_io_set_config(channel, CYG_IO_SET_CONFIG_ADC_DISABLE, 0, 0);
    if (res != ENOERR)
        CYG_TEST_FAIL_FINISH("Failed to disable ADC channel");
}

static void
adc_thread(cyg_addrword_t data)
{
    cyg_io_handle_t channel;
    cyg_devtab_entry_t *t;
    
    CYG_TEST_INFO("ADC test");
    
    for (t = &__DEVTAB__[0]; t != &__DEVTAB_END__; t++) {
        if (t->handlers != &cyg_io_adc_devio)
            continue;
        if (cyg_io_lookup(t->name, &channel) == ENOERR) {
            test_channel(t->name, channel);
        } else {
            CYG_TEST_FAIL_FINISH("Cannot open ADC channel");
        }
    }
    
    CYG_TEST_PASS_FINISH("ADC test OK");
}


void
cyg_start(void)
{
    CYG_TEST_INIT();

    // Create the main ADC test thread
    cyg_thread_create(
        4,
        adc_thread,
        (cyg_addrword_t) 0,
        "adc1",
        thread_stack,
        sizeof(thread_stack),
        &thread_handle,
        &thread_data
    );
    cyg_thread_resume(thread_handle);
    cyg_scheduler_start();
}

#else // CYGFUN_KERNEL_API_C
#define N_A_MSG "Needs kernel C API"
#endif

#else // CYGPKG_IO_ADC && CYGPKG_KERNEL
#define N_A_MSG "Needs Kernel and ADC support"
#endif

#ifdef N_A_MSG
void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG
