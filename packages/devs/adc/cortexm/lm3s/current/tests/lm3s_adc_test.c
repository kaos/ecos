//==========================================================================
//
//      lm3s_adc_test.c
//
//      ADC driver for Stellaris Cortex M3 microcontroller
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2011 Free Software Foundation, Inc.
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
// Author(s):  Uwe Kindler <uwe_kindler@web.de>
//             Updated for Stellaris Cortex microcontroller, ccoutand
// Contributors:
// Date:         2011-01-11
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>        // test macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_arch.h>          // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <pkgconf/kernel.h>
#include <cyg/io/io.h>
#include <cyg/io/adc.h>
#include <pkgconf/devs_adc_cortexm_lm3s.h>

// Package option requirements
#if defined(CYGFUN_KERNEL_API_C)

# include <cyg/kernel/kapi.h>

# if CYGINT_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNELS > 0

#  define MAX_ADC_CHANNEL_TO_TEST 4

//===========================================================================
//                               DATA TYPES
//===========================================================================
typedef struct st_thread_data {
    cyg_thread      obj;
    int             stack[(CYGNUM_HAL_STACK_SIZE_MINIMUM / sizeof(int))];
    cyg_handle_t    hdl;
} thread_data_t;


//===========================================================================
//                              LOCAL DATA
//===========================================================================
cyg_thread_entry_t adc_thread;
thread_data_t   adc_thread_data;


//===========================================================================
//                             ADC THREAD
//===========================================================================
void
adc_thread(cyg_addrword_t data)
{
    int             res;
    cyg_io_handle_t handle[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    cyg_uint32      sample_cnt[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    cyg_uint32      cfg_data;
    cyg_uint32      len;
    cyg_uint32      start_time;
    cyg_uint32      end_time;
    int             i;
    cyg_uint8       seconds = 0;
    float           final_seconds;
    cyg_uint32      samples_expected;


    diag_printf("This test reads samples from all enabled ADC channels.\n"
                "Each second the number of already acquired samples\n"
                "will be printed. After 10 seconds all ADC channels\n"
                "will be stopped and each ADC buffer will be read until\n"
                "it is empty. If the number of acquired samples is much\n"
                "smaller than the number of expected samples, then you\n"
                "should lower the sample rate.\n\n");

    // Get a handle for ADC device 0 channel 0 - 3 (lookup also trigger a
    // channel enable)
    res = cyg_io_lookup("/dev/adc00", &handle[0]);
    res = cyg_io_lookup("/dev/adc01", &handle[1]);
    res = cyg_io_lookup("/dev/adc02", &handle[2]);
    res = cyg_io_lookup("/dev/adc03", &handle[3]);

    // Switch all channels to non blocking
    for (i = 0; i < MAX_ADC_CHANNEL_TO_TEST; ++i) {
        if (handle[i]) {
            cfg_data = 0;
            len = sizeof(cfg_data);
            res = cyg_io_set_config(handle[i],
                                    CYG_IO_SET_CONFIG_READ_BLOCKING,
                                    &cfg_data, &len);
            if (ENOERR != res) {
                CYG_TEST_FAIL_FINISH
                    ("Error switching ADC channel to non blocking");
            }
            sample_cnt[i] = 0;
        }
    }

    start_time = cyg_current_time();

    do {
        for (i = 0; i < MAX_ADC_CHANNEL_TO_TEST; ++i) {
            if (handle[i]) {
                cyg_adc_sample_t sample;

                // read a sample from the channel
                do {
                    cyg_uint32      len = sizeof(sample);
                    res = cyg_io_read(handle[i], &sample, &len);
                }
                while (-EAGAIN == res);
                if (ENOERR == res) {
                    sample_cnt[i]++;
                }
            }
        }

        end_time = cyg_current_time();

        // Print number of acquired samples - if one second is expired.
        // we expect that the number of acquired samples is nearly the
        // sample rate.
        if ((end_time - start_time) >= 100) {
            start_time = end_time;
            diag_printf("%d\t %d\t %d\t %d\n",
                        sample_cnt[0],
                        sample_cnt[1], sample_cnt[2], sample_cnt[3]);
            seconds++;
        }
    } while (seconds < 10);

    // Now stop all channels
    for (i = 0; i < MAX_ADC_CHANNEL_TO_TEST; ++i) {
        if (handle[i]) {
            res = cyg_io_set_config(handle[i],
                                    CYG_IO_SET_CONFIG_ADC_DISABLE, 0, 0);
            if (ENOERR != res) {
                CYG_TEST_FAIL_FINISH("Error disabling ADC channel");
            }
        }
    }

    end_time = cyg_current_time();
    end_time = seconds * 1000 + (end_time - start_time) * 10;
    final_seconds = end_time / 1000.0;

    // Now read all remaining samples from buffer
    for (i = 0; i < MAX_ADC_CHANNEL_TO_TEST; ++i) {
        if (handle[i]) {
            do {
                cyg_adc_sample_t sample;
                cyg_uint32      len = sizeof(sample);
                res = cyg_io_read(handle[i], &sample, &len);
                if (ENOERR == res) {
                    sample_cnt[i]++;
                }
            } while (ENOERR == res);
        }
    }

    diag_printf("\n\n----------------------------------------\n");
    samples_expected =
        final_seconds * CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_DEFAULT_RATE;
    diag_printf("Samples expected after %d milliseconds: %d\n", end_time,
                samples_expected);
    diag_printf("Samples read (per channel):\n");
    diag_printf("%d\t %d\t %d\t %d\n",
                sample_cnt[0], sample_cnt[1], sample_cnt[2], sample_cnt[3]);

    CYG_TEST_PASS_FINISH("ADC test OK");
}


void
cyg_start(void)
{
    CYG_TEST_INIT();

    // Create the main ADC test thread
    cyg_thread_create(4,
                      adc_thread,
                      (cyg_addrword_t)0,
                      "lm3s_adc_thread",
                      (void *)adc_thread_data.stack,
                      CYGNUM_HAL_STACK_SIZE_MINIMUM,
                      &adc_thread_data.hdl,
                      &adc_thread_data.obj
        );

    cyg_thread_resume(adc_thread_data.hdl);

    cyg_scheduler_start();
}
# else// CYGINT_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNELS > 0
#  define N_A_MSG "Needs at least one enabled ADC channel"
# endif

#else // CYGFUN_KERNEL_API_C
# define N_A_MSG "Needs kernel C API"
#endif

#ifdef N_A_MSG
void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG


//---------------------------------------------------------------------------
// EOF lm3s_adc_test.c
