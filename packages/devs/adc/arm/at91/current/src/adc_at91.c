//==========================================================================
//
//      adc_at91.c
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
// Author(s):   Uwe Kindler <uwe_kindler@web.de>
//              Updated for Atmel AT91 device, ccoutand <ccoutand@stmi.com>
// Contributors:
// Date:         2010-02-15
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                                 INCLUDES
//==========================================================================
#include <pkgconf/system.h>
#include <pkgconf/devs_adc_arm_at91.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/adc.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#if CYGPKG_DEVS_ADC_ARM_AT91_DEBUG_LEVEL > 0
   #define at91_adc_printf(args...) diag_printf(args)
#else
   #define at91_adc_printf(args...)
#endif

#define AT91_ADC_CHER_CHx(_ch_)  (0x1 << _ch_)
#define AT91_ADC_CHER_CDRx(_ch_) (_ch_ << 2)

//==========================================================================
//                                  DATA TYPES
//==========================================================================
typedef struct at91_adc_info
{
    cyg_uint32              adc_base;          // base address of ADC peripheral
    cyg_uint8               adc_prescal;       // ADC prescal value
    cyg_uint8               adc_startup_time;  // ADC Startup Time value
    cyg_uint8               adc_shtim;         // ADC SHTIM value
    cyg_uint8               timer_id;          // select timer
    cyg_uint32              timer_base;        // base address of Timer peripheral
    cyg_uint32              tc_base;           // base address of Timer channel
    cyg_vector_t            timer_vector;      // interrupt vector number
    int                     timer_intprio;     // interrupt priority of ADC interrupt
    cyg_uint32              timer_cnt;         // Timer value
    cyg_uint8               timer_clk;         // Timer clock setting
    cyg_uint32              resolution;
    cyg_handle_t            int_handle;        // For initializing the interrupt
    cyg_interrupt           int_data;
    struct cyg_adc_channel *channel[AT91_MAX_ADC_CHAN]; // stores references to channel objects
    cyg_uint8               chan_mask;         // mask that indicates channels used
                                               // by ADC driver
} at91_adc_info;


//==========================================================================
//                               DECLARATIONS
//==========================================================================
static bool at91_adc_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo at91_adc_lookup(struct cyg_devtab_entry **tab,
                                    struct cyg_devtab_entry  *sub_tab,
                                    const char               *name);
static void at91_adc_enable( cyg_adc_channel *chan );
static void at91_adc_disable( cyg_adc_channel *chan );
static void at91_adc_set_rate( cyg_adc_channel *chan, cyg_uint32 rate );
static cyg_uint32 at91_adc_isr(cyg_vector_t vector, cyg_addrword_t data);
static void at91_adc_dsr(cyg_vector_t vector,
                            cyg_ucount32 count,
                            cyg_addrword_t data);

// -------------------------------------------------------------------------
// Driver functions:
CYG_ADC_FUNCTIONS( at91_adc_funs,
                   at91_adc_enable,
                   at91_adc_disable,
                   at91_adc_set_rate );


#include CYGDAT_DEVS_ADC_ARM_AT91_INL // Instantiate ADCs

//==========================================================================
// This function is called from the device IO infrastructure to initialize
// the device. It should perform any work needed to start up the device,
// short of actually starting the generation of samples. This function will
// be called for each channel, so if there is initialization that only needs
// to be done once, such as creating and interrupt object, then care should
// be taken to do this. This function should also call cyg_adc_device_init()
// to initialize the generic parts of the driver.
//==========================================================================
static bool at91_adc_init(struct cyg_devtab_entry *tab)
{
    cyg_adc_channel *chan   = (cyg_adc_channel *)tab->priv;
    cyg_adc_device *device  = chan->device;
    at91_adc_info *info     = device->dev_priv;
    cyg_uint32 regval;

    if (!info->int_handle)
    {
       cyg_drv_interrupt_create(info->timer_vector,
                                 info->timer_intprio,
                                (cyg_addrword_t)device,
                                &at91_adc_isr,
                                &at91_adc_dsr,
                                &(info->int_handle),
                                &(info->int_data));
       cyg_drv_interrupt_attach(info->int_handle);
       cyg_drv_interrupt_mask(info->timer_vector);

       // Reset ADC
       HAL_WRITE_UINT32((info->adc_base + AT91_ADC_CR), AT91_ADC_CR_SWRST);

       // Disable counter interrupts
       HAL_WRITE_UINT32(info->tc_base+AT91_TC_CCR, AT91_TC_CCR_CLKDIS);
       HAL_WRITE_UINT32(info->tc_base+AT91_TC_IDR, 0xffffffff);

       // Clear status bit
       HAL_READ_UINT32(info->tc_base + AT91_TC_SR, regval);

       // Enable peripheral clocks for TC
       HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCER,  \
             ((AT91_PMC_PCER_TC0) << info->timer_id));

       //
       // Disable all interrupts, all channels
       //
       HAL_WRITE_UINT32((info->adc_base + AT91_ADC_CHDR), \
                     AT91_ADC_CHER_CH0  |\
                     AT91_ADC_CHER_CH1  |\
                     AT91_ADC_CHER_CH2  |\
                     AT91_ADC_CHER_CH3  |\
                     AT91_ADC_CHER_CH4  |\
                     AT91_ADC_CHER_CH5  |\
                     AT91_ADC_CHER_CH6  |\
                     AT91_ADC_CHER_CH7);
       HAL_WRITE_UINT32((info->adc_base + AT91_ADC_IDR), \
                     AT91_ADC_CHER_CH0  |\
                     AT91_ADC_CHER_CH1  |\
                     AT91_ADC_CHER_CH2  |\
                     AT91_ADC_CHER_CH3  |\
                     AT91_ADC_CHER_CH4  |\
                     AT91_ADC_CHER_CH5  |\
                     AT91_ADC_CHER_CH6  |\
                     AT91_ADC_CHER_CH7);

       //
       // setup the default sample rate
       //
       at91_adc_set_rate(chan, chan->device->config.rate);

       // setup ADC mode
       HAL_WRITE_UINT32((info->adc_base + AT91_ADC_MR), \
                    ( ( info->adc_prescal  << AT91_ADC_MR_PRESCAL_SHIFT ) & \
                        AT91_ADC_MR_PRESCAL_MASK ) | \
                    ( ( info->adc_startup_time   << AT91_ADC_MR_STARTUP_SHIFT ) & \
                        AT91_ADC_MR_STARTUP_MASK ) | \
                    ( (  info->adc_shtim << AT91_ADC_MR_SHTIM_SHIFT ) & \
                        AT91_ADC_MR_SHTIM_MASK ) | \
                        AT91_ADC_MR_TRGSEL_TIOA0  | \
                        info->resolution);


    } // if (!info->int_handle)

    cyg_adc_device_init(device); // initialize generic parts of driver

    return true;
}


//==========================================================================
// This function is called when a client looks up or opens a channel. It
// should call cyg_adc_channel_init() to initialize the generic part of
// the channel. It should also perform any operations needed to start the
// channel generating samples.
//==========================================================================
static Cyg_ErrNo at91_adc_lookup(struct cyg_devtab_entry **tab,
                                    struct cyg_devtab_entry  *sub_tab,
                                    const char               *name)
{
    cyg_adc_channel  *chan     = (cyg_adc_channel *)(*tab)->priv;
    at91_adc_info *info     = chan->device->dev_priv;

    info->channel[chan->channel] = chan;
    cyg_adc_channel_init(chan); // initialize generic parts of channel

    //
    // The generic ADC manual says: When a channel is first looked up or
    // opened, then it is automatically enabled and samples start to
    // accumulate - so we start the channel now
    //
    chan->enabled = true;
    at91_adc_enable(chan);

    return ENOERR;
}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_ENABLE config operation.
// It should take any steps needed to start the channel generating samples
//==========================================================================
static void at91_adc_enable(cyg_adc_channel *chan)
{
    at91_adc_info *info      = chan->device->dev_priv;

    // Enable the channel
    HAL_WRITE_UINT32((info->adc_base + AT91_ADC_CHER), \
                      AT91_ADC_CHER_CHx(chan->channel));

    //
    // Unmask interrupt as soon as 1 channel is enable
    //
    if (!info->chan_mask)
    {
       cyg_drv_interrupt_unmask(info->timer_vector);

       // Enable timer interrupt
       HAL_WRITE_UINT32(info->tc_base+AT91_TC_IER, AT91_TC_IER_CPC);

       // Enable the clock
       HAL_WRITE_UINT32(info->tc_base+AT91_TC_CCR, AT91_TC_CCR_TRIG | AT91_TC_CCR_CLKEN);

       // Start timer
       HAL_WRITE_UINT32(info->tc_base+AT91_TC_CCR,  AT91_TC_CCR_TRIG);

       // Start ADC sampling
       HAL_WRITE_UINT32((info->adc_base + AT91_ADC_CR), AT91_ADC_CR_START);

    }

    info->chan_mask |= AT91_ADC_CHER_CHx(chan->channel);

}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_DISABLE config operation.
// It should take any steps needed to stop the channel generating samples.
//==========================================================================
static void at91_adc_disable(cyg_adc_channel *chan)
{
    at91_adc_info *info  = chan->device->dev_priv;
    cyg_uint32 sr;

    info->chan_mask &= ~ AT91_ADC_CHER_CHx(chan->channel);

    // Disable the channel
    HAL_WRITE_UINT32((info->adc_base + AT91_ADC_CHDR), \
                      AT91_ADC_CHER_CHx(chan->channel));

    //
    // If no channel is enabled the we disable interrupts now
    //
    if (!info->chan_mask)
    {
       cyg_drv_interrupt_mask(info->timer_vector);

       // Clear interrupt
       HAL_READ_UINT32(info->tc_base+AT91_TC_SR, sr);

       // Disable  timer interrupt
       HAL_WRITE_UINT32(info->tc_base+AT91_TC_IDR, AT91_TC_IER_CPC);

       // Disable the clock
       HAL_WRITE_UINT32(info->tc_base+AT91_TC_CCR, AT91_TC_CCR_CLKDIS);

    }
}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_RATE config operation.
// It should take any steps needed to change the sample rate of the channel,
// or of the entire device.
// We use a timer channel to generate the interrupts for sampling the
// analog channels
//==========================================================================
static void at91_adc_set_rate( cyg_adc_channel *chan, cyg_uint32 rate)
{
    cyg_adc_device   *device = chan->device;
    at91_adc_info *info   = (at91_adc_info *)device->dev_priv;
    cyg_uint8 timer_clk   = AT91_TC_CMR_CLKS_MCK2;
    cyg_uint32 tmr_period = CYGNUM_HAL_ARM_AT91_CLOCK_SPEED / ( rate << 1);

    if( tmr_period > 0xffff )
    {
       tmr_period = CYGNUM_HAL_ARM_AT91_CLOCK_SPEED / ( rate << 5);
       timer_clk  = AT91_TC_CMR_CLKS_MCK32;
    }

    if( tmr_period > 0xffff )
    {
       tmr_period = CYGNUM_HAL_ARM_AT91_CLOCK_SPEED / ( rate << 7);
       timer_clk  = AT91_TC_CMR_CLKS_MCK128;
    }

    if( tmr_period > 0xffff )
    {
       tmr_period = CYGNUM_HAL_ARM_AT91_CLOCK_SPEED / ( rate << 10);
       timer_clk  = AT91_TC_CMR_CLKS_MCK1024;
    }

    if( tmr_period > 0xffff )
    {
       tmr_period = 0xffff;
       timer_clk  = AT91_TC_CMR_CLKS_MCK1024;
       at91_adc_printf("AT91 ADC timer, rate too high!");
    }

    device->config.rate = rate;
    info->timer_clk = timer_clk;
    info->timer_cnt = tmr_period;

    // Set timer values
    HAL_WRITE_UINT32(info->tc_base+AT91_TC_CMR,  AT91_TC_CMR_CPCTRG | info->timer_clk);
    HAL_WRITE_UINT32(info->tc_base+AT91_TC_RC,  info->timer_cnt);

    at91_adc_printf("AT91 ADC Timer settings %d, %d", info->timer_clk, info->timer_cnt);

    return;
}


//==========================================================================
// This function is the ISR attached to the ADC device's interrupt vector.
// It is responsible for reading samples from the channels and passing them
// on to the generic layer. It needs to check each channel for data, and call
// cyg_adc_receive_sample() for each new sample available, and then ready the
// device for the next interrupt.
//==========================================================================
static cyg_uint32 at91_adc_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_adc_device   *device = (cyg_adc_device *) data;
    at91_adc_info *info   = (at91_adc_info *)device->dev_priv;
    cyg_uint32        regval,  adc_status;
    cyg_uint32        res = 0;
    cyg_adc_sample_t  adcdata;
    cyg_uint32 sr;

    cyg_uint8 active_channels = info->chan_mask;
    cyg_uint8 channel_no = 0;

    // Clear timer interrupt
    HAL_READ_UINT32(info->tc_base+AT91_TC_SR, sr);

    // Check on channel conversion done
    HAL_READ_UINT32(info->adc_base + AT91_ADC_SR, adc_status);

    while (active_channels)
    {
        if (active_channels & 0x01)
        {
            // If ADC conversion done, save sample
            if(adc_status & AT91_ADC_CHER_CHx(channel_no))
            {
              HAL_READ_UINT32((info->adc_base + AT91_ADC_CDR0 + AT91_ADC_CHER_CDRx(channel_no)), regval);
              adcdata = regval & 0x3FF;
              res |= CYG_ISR_HANDLED
                |  cyg_adc_receive_sample(info->channel[channel_no],
                                          adcdata);
            }
        } // if (active_channels & 0x01)
        active_channels >>= 1;
        channel_no++;
    } // while (active_channels)

    // Restart sampling
    HAL_WRITE_UINT32((info->adc_base + AT91_ADC_CR), AT91_ADC_CR_START);

    cyg_drv_interrupt_acknowledge(info->timer_vector);

    return res;
}


//==========================================================================
// This function is the DSR attached to the ADC device's interrupt vector.
// It is called by the kernel if the ISR return value contains the
// CYG_ISR_HANDLED bit. It needs to call cyg_adc_wakeup() for each channel
// that has its wakeup field set.
//==========================================================================
static void at91_adc_dsr(cyg_vector_t vector,
                            cyg_ucount32 count,
                            cyg_addrword_t data)
{
    cyg_adc_device   *device          = (cyg_adc_device *) data;
    at91_adc_info *info            = device->dev_priv;
    cyg_uint8         active_channels = info->chan_mask;
    cyg_uint8         chan_no         = 0;

    while (active_channels)
    {
        if (active_channels & 0x01)
        {
            if(info->channel[chan_no]->wakeup)
            {
                cyg_adc_wakeup(info->channel[chan_no]);
            }
        }
        chan_no++;
        active_channels >>= 1;
    }
}


//---------------------------------------------------------------------------
// eof adc_at91.c
