//==========================================================================
//
//        i2s_audio_test.c
//
//        Cirrus CL7209 eval board CS4340 audio test
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
// Date:          2000-01-03
// Description:   Tool used to test AUDIO interface
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_edb7xxx.h>  // Board definitions

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>            // HAL ISR support

#if 1
#include "long_audio_left.h"
#include "long_audio_right.h"
#else
#include "short_audio_left.h"
#include "short_audio_right.h"
#endif

extern void i2s_FIQ(void);  // Actually the FIQ handler

// Number of data samples to play
#define NUM_PCM_SAMPLES (sizeof(left_channel)/sizeof(left_channel[0]))
// Play the data in 8 "chunks", truncated to multiple of 4 samples
#define CHUNK_LENGTH (((NUM_PCM_SAMPLES/8)/4)*4)

// This structure is used by the assembly coded interrupt routine
struct audio_buf {
    void *left_chan_ptr;
    void *right_chan_ptr;
    int   length;
};
volatile struct audio_buf cur_buf;
volatile struct audio_buf next_buf;

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
I2Sreset(void)
{
    // Enable i2s interface instead of SSI#2
    *(volatile cyg_uint32 *)SYSCON3 |= SYSCON3_I2SSEL | 0x200;

    // Configure the i2s audio interface
    //   External clock, interrupt on right channel FIFO needs service
    *(volatile cyg_uint32 *)I2S_CTL = I2S_CTL_FLAG | I2S_CTL_ECS | I2S_CTL_RCTM;

    // Clear out status register
    *(volatile cyg_uint32 *)I2S_STAT = 0xFFFFFFFF;  // 1's reset

    // Now enable the interface
    *(volatile cyg_uint32 *)I2S_CTL |= I2S_CTL_EN;

    // Turn on the FIFOs
    while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0) ;  // Wait   
    *(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;  // Reset flag
    *(volatile cyg_uint32 *)I2S_FIFO_CTL = I2S_FIFO_CTL_RIGHT_ENABLE;
    while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0) ;  // Wait   
    *(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;  // Reset flag
    *(volatile cyg_uint32 *)I2S_FIFO_CTL = I2S_FIFO_CTL_LEFT_ENABLE;
    while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0) ;  // Wait   
    *(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;  // Reset flag

    // Clear out status register
    *(volatile cyg_uint32 *)I2S_STAT = 0xFFFFFFFF;  // 1's reset

    // Enable interrupt
    *(volatile cyg_uint32 *)INTMR3 |= INTSR3_I2SINT;
}

static void
I2Sdisable(void)
{
    // Turn off the FIFOs
    while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0) ;  // Wait   
    *(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;  // Reset flag
    *(volatile cyg_uint32 *)I2S_FIFO_CTL = I2S_FIFO_CTL_RIGHT_DISABLE;
    while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0) ;  // Wait   
    *(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;  // Reset flag
    *(volatile cyg_uint32 *)I2S_FIFO_CTL = I2S_FIFO_CTL_LEFT_DISABLE;
    while ((*(volatile cyg_uint32 *)I2S_STAT & I2S_STAT_FIFO) == 0) ;  // Wait   
    *(volatile cyg_uint32 *)I2S_STAT = I2S_STAT_FIFO;  // Reset flag

    // Disable interrupt
    *(volatile cyg_uint32 *)INTMR3 &= ~INTSR3_I2SINT;

    // Disable the interface
    *(volatile cyg_uint32 *)I2S_CTL = 0;
}

static void
idle(void)
{
    // Do nothing
}

static void
audio_exercise(cyg_addrword_t p)
{
    int sample, len;
    bool started;

    diag_printf("AUDIO test here!\n");

    // Enable my special FIQ handler
    HAL_VSR_SET(CYGNUM_HAL_VECTOR_FIQ, i2s_FIQ, NULL);

    while (TRUE) {
        diag_printf("... Starting over\n");

        // Set up pointers
        cur_buf.length = 0;
        next_buf.length = 0;
        sample = 0;
        started = false;

        // Send out the data
        while (sample != NUM_PCM_SAMPLES) {
            if (next_buf.length == 0) {
                // Move some data into the "next" buffer
                next_buf.left_chan_ptr = &left_channel[sample];
                next_buf.right_chan_ptr = &right_channel[sample];
                len = NUM_PCM_SAMPLES - sample;
                if (len > CHUNK_LENGTH) len = CHUNK_LENGTH;
                next_buf.length = len;

                idle();

                if (!started) {
                    started = true;
                    // Initialize audio interface
                    I2Sreset();
                }
                sample += len;
            }
        }

        // Wait for all data to be sent
        while (cur_buf.length != 0) {
        }
        
        // Shut off audio interface
        I2Sdisable();
    }

    diag_printf("All done!\n");
    cyg_test_exit();
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(11,                // Priority - just a number
                      audio_exercise,    // entry
                      0,                 // initial parameter
                      "AUDIO_thread",    // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    // Let 'em fly...
    cyg_scheduler_start();
} // cyg_package_start()

