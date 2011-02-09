//==========================================================================
//
//      adc_lm3s.inl
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
// Author(s):    ccoutand
//              
// Contributors:
// Date:         2011-01-08
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef CYGONCE_DEVS_ADC_CORTEXM_LM3S_INL
#define CYGONCE_DEVS_ADC_CORTEXM_LM3S_INL

// Declare ADC0
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0

static lm3s_adc_info lm3s_adc0_info =
{
    .timer_base       = CYGHWR_HAL_LM3S_GPTIM0 + ( 0x1000 * CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_SELECT_TIMER ),
    .adc_intprio      = CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_INTPRIO,
    .adc_vector       = CYGNUM_HAL_INTERRUPT_ADC0_S0,
    .adc_base         = CYGHWR_HAL_LM3S_ADC0,
    .adc_periph       = CYGHWR_HAL_LM3S_P_ADC0,
    .timer_periph     = (CYGHWR_HAL_LM3S_SC_RCGC1_TIMER0 << CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_SELECT_TIMER),
#if   CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_AVERAGING == 0
    .adc_avg          = 0,
#elif CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_AVERAGING == 2
    .adc_avg          = 1,
#elif CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_AVERAGING == 4
    .adc_avg          = 2,
#elif CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_AVERAGING == 8
    .adc_avg          = 3,
#elif CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_AVERAGING == 16
    .adc_avg          = 4,
#elif CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_AVERAGING == 32
    .adc_avg          = 5,
#elif CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_AVERAGING == 64
    .adc_avg          = 6,
#endif
#if defined(CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_TEMP_SENSOR)
    .sensor_channel   = CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_TEMP_SENSOR_CHANNEL,
#else
    .sensor_channel   = 0xff,
#endif
    .max_channel      = CYGINT_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNELS,
    .int_handle       = 0,
    .chan_mask        = 0
};

CYG_ADC_DEVICE(                                                 \
         lm3s_adc0_device,                                      \
         &lm3s_adc_funs,                                        \
         &lm3s_adc0_info,                                       \
         CYGNUM_DEVS_ADC_CORTEXM_LM3S_ADC0_DEFAULT_RATE);

#define LM3S_ADC0_CHANNEL( __chan )                                      \
CYG_ADC_CHANNEL(                                                         \
         lm3s_adc0_channel##__chan,                                      \
         __chan,                                                         \
         CYGDAT_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL##__chan##_BUFSIZE,    \
         &lm3s_adc0_device );                                            \
DEVTAB_ENTRY(                                                            \
         lm3s_adc0_channel##__chan##_device,                             \
         CYGDAT_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL##__chan##_NAME,       \
         0,                                                              \
         &cyg_io_adc_devio,                                              \
         lm3s_adc_init,                                                  \
         lm3s_adc_lookup,                                                \
         &lm3s_adc0_channel##__chan );

#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL0
LM3S_ADC0_CHANNEL(0);
#endif
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL1
LM3S_ADC0_CHANNEL(1);
#endif
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL2
LM3S_ADC0_CHANNEL(2);
#endif
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL3
LM3S_ADC0_CHANNEL(3);
#endif
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL4
LM3S_ADC0_CHANNEL(4);
#endif
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL5
LM3S_ADC0_CHANNEL(5);
#endif
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL6
LM3S_ADC0_CHANNEL(6);
#endif
#ifdef CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0_CHANNEL7
LM3S_ADC0_CHANNEL(7);
#endif

#endif // CYGPKG_DEVS_ADC_CORTEXM_LM3S_ADC0

#endif // CYGONCE_DEVS_ADC_CORTEXM_LM3S_INL
