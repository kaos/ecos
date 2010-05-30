//==========================================================================
//
//      adc_at91.inl
//
//      ADC driver for AT91 on chip ADC
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
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
// Author(s):    Atmel AT91 on-chip ADC device driver, ccoutand
//              
// Contributors:
// Date:         2010-05-27
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef CYGONCE_DEVS_ADC_ARM_AT91_INL
#define CYGONCE_DEVS_ADC_ARM_AT91_INL

// Some AT91 HAL are defining the timer 0 vector as TC0 and other as TIMER0
#ifndef CYGNUM_HAL_INTERRUPT_TC0
#define CYGNUM_HAL_INTERRUPT_TC0 CYGNUM_HAL_INTERRUPT_TIMER0
#endif

// Declare ADC0
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0

static at91_adc_info at91_adc0_info =
{
    .adc_base         = AT91_ADC,
    .timer_base       = AT91_TC,
    .tc_base          = AT91_TC + (AT91_TC_TC_SIZE * CYGNUM_DEVS_ADC_ARM_AT91_ADC0_SELECT_TIMER),
    .timer_vector     = CYGNUM_HAL_INTERRUPT_TC0 + CYGNUM_DEVS_ADC_ARM_AT91_ADC0_SELECT_TIMER,
    .timer_intprio    = CYGNUM_DEVS_ADC_ARM_AT91_ADC0_INTPRIO,
    .timer_id         = CYGNUM_DEVS_ADC_ARM_AT91_ADC0_SELECT_TIMER,
    .int_handle       = 0,
    .adc_prescal      = CYGNUM_DEVS_ADC_ARM_AT91_ADC0_PRESCAL,
    .adc_startup_time = CYGNUM_DEVS_ADC_ARM_AT91_ADC0_STARTUP_TIME,
    .adc_shtim        = CYGNUM_DEVS_ADC_ARM_AT91_ADC0_SHTIM,
#if CYGNUM_IO_ADC_SAMPLE_SIZE > 8
     .resolution      = AT91_ADC_MR_LOWREC_10BITS,
#else
     .resolution      = AT91_ADC_MR_LOWRES_8BITS,
#endif
    .chan_mask        = 0
};

CYG_ADC_DEVICE( at91_adc0_device,
                &at91_adc_funs,
                &at91_adc0_info,
                CYGNUM_DEVS_ADC_ARM_AT91_ADC0_DEFAULT_RATE);

#define AT91_ADC0_CHANNEL( __chan )                                        \
CYG_ADC_CHANNEL( at91_adc0_channel##__chan,                                \
                 __chan,                                                   \
                 CYGDAT_DEVS_ADC_ARM_AT91_ADC0_CHANNEL##__chan##_BUFSIZE,  \
                 &at91_adc0_device );                                       \
                                                                           \
DEVTAB_ENTRY( at91_adc0_channel##__chan##_device,                          \
              CYGDAT_DEVS_ADC_ARM_AT91_ADC0_CHANNEL##__chan##_NAME,        \
              0,                                                           \
              &cyg_io_adc_devio,                                           \
              at91_adc_init,                                               \
              at91_adc_lookup,                                             \
              &at91_adc0_channel##__chan );

#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL0
AT91_ADC0_CHANNEL(0);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL1
AT91_ADC0_CHANNEL(1);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL2
AT91_ADC0_CHANNEL(2);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL3
AT91_ADC0_CHANNEL(3);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL4
AT91_ADC0_CHANNEL(4);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL5
AT91_ADC0_CHANNEL(5);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL6
AT91_ADC0_CHANNEL(6);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC0_CHANNEL7
AT91_ADC0_CHANNEL(7);
#endif

#endif // CYGPKG_DEVS_ADC_ARM_AT91_ADC0



// Declare ADC1
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1

static at91_adc_info at91_adc1_info =
{
    .adc_base         = AT91_ADC1,
    .timer_base       = AT91_TC,
    .tc_base          = AT91_TC + (AT91_TC_TC_SIZE * CYGNUM_DEVS_ADC_ARM_AT91_ADC1_SELECT_TIMER),
    .timer_vector     = CYGNUM_HAL_INTERRUPT_TC0 + CYGNUM_DEVS_ADC_ARM_AT91_ADC1_SELECT_TIMER,
    .timer_intprio    = CYGNUM_DEVS_ADC_ARM_AT91_ADC1_INTPRIO,
    .timer_id         = CYGNUM_DEVS_ADC_ARM_AT91_ADC1_SELECT_TIMER,
    .int_handle       = 0,
    .adc_prescal      = CYGNUM_DEVS_ADC_ARM_AT91_ADC1_PRESCAL,
    .adc_startup_time = CYGNUM_DEVS_ADC_ARM_AT91_ADC1_STARTUP_TIME,
    .adc_shtim        = CYGNUM_DEVS_ADC_ARM_AT91_ADC1_SHTIM,
#if CYGNUM_IO_ADC_SAMPLE_SIZE > 8
     .resolution      = AT91_ADC_MR_LOWREC_10BITS,
#else
     .resolution      = AT91_ADC_MR_LOWRES_8BITS,
#endif
    .chan_mask        = 0
};
CYG_ADC_DEVICE( at91_adc1_device,
                &at91_adc_funs,
                &at91_adc1_info,
                CYGNUM_DEVS_ADC_ARM_AT91_ADC1_DEFAULT_RATE);

#define AT91_ADC1_CHANNEL( __chan )                                        \
CYG_ADC_CHANNEL( at91_adc1_channel##__chan,                                \
                 __chan,                                                   \
                 CYGDAT_DEVS_ADC_ARM_AT91_ADC1_CHANNEL##__chan##_BUFSIZE,  \
                 &at91_adc1_device );                                       \
                                                                           \
DEVTAB_ENTRY( at91_adc1_channel##__chan##_device,                          \
              CYGDAT_DEVS_ADC_ARM_AT91_ADC1_CHANNEL##__chan##_NAME,        \
              0,                                                           \
              &cyg_io_adc_devio,                                           \
              at91_adc_init,                                               \
              at91_adc_lookup,                                             \
              &at91_adc1_channel##__chan );

#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL0
AT91_ADC1_CHANNEL(0);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL1
AT91_ADC1_CHANNEL(1);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL2
AT91_ADC1_CHANNEL(2);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL3
AT91_ADC1_CHANNEL(3);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL4
AT91_ADC1_CHANNEL(4);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL5
AT91_ADC1_CHANNEL(5);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL6
AT91_ADC1_CHANNEL(6);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_AT91_ADC1_CHANNEL7
AT91_ADC1_CHANNEL(7);
#endif

#endif // CYGPKG_DEVS_ADC_ARM_AT91_ADC1

#endif // CYGONCE_DEVS_ADC_ARM_AT91_INL
