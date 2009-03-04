//==========================================================================
//
//        adc2.c
//
//        ADC performance test
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
// Description:   ADC performance test
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

#define MAX_CHANNELS                64  // Max channels to test

#define TICKS_PER_SECOND            \
    (1000000000 / (CYGNUM_HAL_RTC_NUMERATOR / CYGNUM_HAL_RTC_DENOMINATOR))

// ADC test channel
typedef struct test_channel {
    const char *dev;            // ADC channel device name
    cyg_io_handle_t handle;     // ADC channel handle
    cyg_uint32 count;           // Sample counter
} test_channel;

// ADC test channels
static test_channel test_channels[MAX_CHANNELS];

// Thread data
cyg_handle_t thread_handle;
cyg_thread thread_data;
cyg_uint8 thread_stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];

static void
adc_thread(cyg_addrword_t data)
{
    cyg_uint32 num = 0;
    cyg_devtab_entry_t *t;
    test_channel *chan;
    cyg_adc_sample_t sample;
    Cyg_ErrNo res;
    cyg_uint32 cfg_data;
    cyg_uint32 len;
    cyg_uint32 start_time;
    cyg_uint32 end_time;
    int i;
    cyg_uint8 seconds = 0;
    float final_seconds;
    cyg_uint32 samples_expected;
    
    CYG_TEST_INFO("ADC performance test");
    
    // This test reads samples from all enabled ADC channels. Each second, the
    // number of already acquired samples is printed. After 10 seconds, all ADC
    // channels are stpped and each ADC buffer is read until empty. If the
    // number of acquired samples is much smaller than the number of expected
    // samples, the rate is too high.

    CYG_TEST_INFO("Opening available ADC channels");
    
    for (t = &__DEVTAB__[0]; t != &__DEVTAB_END__; t++) {
        if (t->handlers != &cyg_io_adc_devio)
            continue;
        chan = &test_channels[num++];
        chan->dev = t->name;
        if (cyg_io_lookup(chan->dev, &chan->handle) != ENOERR)
            CYG_TEST_FAIL_FINISH("Cannot open ADC channel");
    }
    
    diag_printf("Opened %d ADC channels\n", num);
    
    CYG_TEST_INFO("Preparing ADC channels for test");
    
    for (i = 0; i < num; i++) {
        chan = &test_channels[i];
        
        // Disable channel
        res = cyg_io_set_config(chan->handle, CYG_IO_SET_CONFIG_ADC_DISABLE, 0, 0);
        if (res != ENOERR)
            CYG_TEST_FAIL_FINISH("Failed to disable ADC channel");
        
        // Make channel non-blocking
        cfg_data = 0;
        len = sizeof(cfg_data);
        res = cyg_io_set_config(chan->handle, 
                                CYG_IO_SET_CONFIG_READ_BLOCKING,
                                &cfg_data, &len);
        if (res != ENOERR)
            CYG_TEST_FAIL_FINISH("Failed to make ADC channel non-blocking");

        // Set channel sampling rate
        cfg_data = CYGNUM_IO_ADC_PERFORMANCE_TEST_RATE;
        len = sizeof(cfg_data);
        res = cyg_io_set_config(chan->handle,
                                CYG_IO_SET_CONFIG_ADC_RATE,
                                &cfg_data, &len);
        if (res != ENOERR)
            CYG_TEST_FAIL_FINISH("Failed to set ADC channel sampling rate");
        
        // Flush channel
        do {
            len = sizeof(sample);
            res = cyg_io_read(chan->handle, &sample, &len);
        } while (res == ENOERR);
        chan->count = 0;
    }
    
    CYG_TEST_INFO("Starting measurement");
    
    for (i = 0; i < num; i++) {
        chan = &test_channels[i];
        
        // Enable channel
        res = cyg_io_set_config(chan->handle, CYG_IO_SET_CONFIG_ADC_ENABLE, 0, 0);
        if (res != ENOERR)
            CYG_TEST_FAIL_FINISH("Failed to enabled ADC channel");
    }

    start_time = cyg_current_time();
    do {
        for (i = 0; i < num; i++) {
            chan = &test_channels[i];
            
            // Read & count samples
            do {
                cyg_uint32 len = sizeof(sample);
                res = cyg_io_read(chan->handle, &sample, &len);
                if (res == ENOERR)
                    chan->count++;
            } while (res == ENOERR);
        }
        
        // Print number of acquired samples - if one second is expired. We
        // expect that the number of acquired samples is nearly the sample rate
        end_time = cyg_current_time();
        if ((end_time - start_time) >= TICKS_PER_SECOND) {
            start_time = end_time;
            diag_printf("\n");
            for (i = 0; i < num; i++) {
                chan = &test_channels[i];
                diag_printf("%-20s\t= %d\n", chan->dev, chan->count);
            }
            seconds++;
        }
    } while (seconds < 10);

    for (i = 0; i < num; i++) {
        chan = &test_channels[i];
        
        // Disable channel
        res = cyg_io_set_config(chan->handle, CYG_IO_SET_CONFIG_ADC_DISABLE, 0, 0);
        if (res != ENOERR)
            CYG_TEST_FAIL_FINISH("Failed to disable ADC channel");
    }
    
    end_time = cyg_current_time();
    
    for (i = 0; i < num; i++) {
        chan = &test_channels[i];
        
        // Count remainding samples
        do {
            len = sizeof(sample);
            res = cyg_io_read(chan->handle, &sample, &len);
            if (res == ENOERR)
                chan->count++;
        } while (res == ENOERR);
    }
    
    CYG_TEST_INFO("Finished measurement");
    
    diag_printf("\n\n----------------------------------------\n");
    
    final_seconds = (end_time - start_time) + (seconds * TICKS_PER_SECOND);
    final_seconds /= TICKS_PER_SECOND;
    samples_expected = final_seconds * CYGNUM_IO_ADC_PERFORMANCE_TEST_RATE;
    diag_printf("Samples expected after %d milliseconds: %d\n",
                (unsigned int) (final_seconds * 1000), samples_expected);
    
    diag_printf("Samples read (per channel):\n");
    for (i = 0; i < num; i++) {
        chan = &test_channels[i];
        diag_printf("%-20s\t= %d\n", chan->dev, chan->count);
    }

    CYG_TEST_PASS_FINISH("ADC performance test OK");
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
        "adc2",
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
