//==========================================================================
//
//      adc_lm3s.c
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
// Author(s):   Uwe Kindler <uwe_kindler@web.de>
//              Updated for Stellaris device, ccoutand
// Contributors:
// Date:         2011-01-08
// Purpose:
// Description:
//
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                                 INCLUDES
//==========================================================================
#include <pkgconf/system.h>
#include <pkgconf/devs_adc_cortexm_lm3s.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/adc.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#if CYGPKG_DEVS_ADC_CORTEXM_LM3S_DEBUG_LEVEL > 0
# define lm3s_adc_diag(args...) diag_printf(args)
#else
# define lm3s_adc_diag(args...)
#endif

#define CYGHWR_HAL_LM3S_ADC_MAX_CHAN 8

//==========================================================================
//                                  DATA TYPES
//==========================================================================
typedef struct lm3s_adc_info {
    cyg_uint32      adc_base;          // ADC base address
    cyg_uint32      adc_periph;        // ADC peripheral mask
    cyg_vector_t    adc_vector;        // Interrupt vector number
    cyg_priority_t  adc_intprio;       // Interrupt priority of ADC interrupt
    cyg_uint32      timer_base;        // Base address of Timer peripheral
    cyg_uint32      timer_interval;    // Timer value
    cyg_uint32      timer_periph;      // Timer peripheral mask
    cyg_uint8       sensor_channel;    // Temperature sensor channel if any
    cyg_uint8       max_channel;       // Number of ADC channel
    cyg_handle_t    int_handle;        // For initializing the interrupt
    cyg_interrupt   int_data;
    cyg_uint8       adc_avg;           // Sample averaging
    // Stores references to channel objects
    struct cyg_adc_channel *channel[CYGHWR_HAL_LM3S_ADC_MAX_CHAN];
    cyg_uint8       chan_mask;         // Mask that indicates channels used
                                       // by ADC driver
} lm3s_adc_info;


//==========================================================================
//                               DECLARATIONS
//==========================================================================
static bool     lm3s_adc_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo lm3s_adc_lookup(struct cyg_devtab_entry **tab,
                                 struct cyg_devtab_entry *sub_tab,
                                 const char *name);
static void     lm3s_adc_enable(cyg_adc_channel * chan);
static void     lm3s_adc_disable(cyg_adc_channel * chan);
static void     lm3s_adc_set_rate(cyg_adc_channel * chan, cyg_uint32 rate);
static cyg_uint32 lm3s_adc_isr(cyg_vector_t vector, cyg_addrword_t data);
static void     lm3s_adc_dsr(cyg_vector_t vector,
                             cyg_ucount32 count, cyg_addrword_t data);

static void     lm3s_adc_disable_sequencer0(cyg_uint32);
static void     lm3s_adc_enable_sequencer0(cyg_uint32);
static void     lm3s_adc_flush(cyg_uint32);
static void     lm3s_adc_update_sequencer0(cyg_adc_channel *);

// -------------------------------------------------------------------------
// Driver functions:
CYG_ADC_FUNCTIONS( lm3s_adc_funs,
                   lm3s_adc_enable,
                   lm3s_adc_disable,
                   lm3s_adc_set_rate );


#include CYGDAT_DEVS_ADC_CORTEXM_LM3S_INL       // Instantiate ADCs


//==========================================================================
//
// The eCos Sellaris ADC drivers uses a single sequencer ( sequencer 0 ).
// The same sequencer is used to sample all channels.
// Sampling of the different channel is triggered from a timer interrupt.
// The ADC driver flexibility does not allow to trigger sampling from
// external GPIO or analog comparator event. It should be noted that enabling
// / disabling an ADC channel disturbs the sampling of other channels since it
// requires to stop sampling to re-organize the sequencer. Also the FIFO
// is flushed to ensure correct sample order out of the sequencer FIFO.
//
//==========================================================================
static bool
lm3s_adc_init(struct cyg_devtab_entry *tab)
{
    cyg_adc_channel *chan = (cyg_adc_channel *) tab->priv;
    cyg_adc_device *device = chan->device;
    lm3s_adc_info  *info = device->dev_priv;

    lm3s_adc_diag("ADC: Init\n");

    if (!info->int_handle) {
        lm3s_adc_diag("ADC: IRQ vect %d, pri %d\n",
                      info->adc_vector, info->adc_intprio);

        cyg_drv_interrupt_create(info->adc_vector,
                                 info->adc_intprio,
                                 (cyg_addrword_t)device,
                                 &lm3s_adc_isr,
                                 &lm3s_adc_dsr,
                                 &(info->int_handle), &(info->int_data));
        cyg_drv_interrupt_attach(info->int_handle);
        cyg_drv_interrupt_mask(info->adc_vector);

        // Enable ADC and sampling timer peripheral
        CYGHWR_HAL_LM3S_PERIPH_SET(info->adc_periph, 1);
        CYGHWR_HAL_LM3S_PERIPH_SET((CYGHWR_HAL_LM3S_PERIPH_GC1 | info->
                                    timer_periph), 1);

        // Disable timer
        HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_CTL, 0);

        // Disable / reset sequencer
        HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_ACTSS, 0);
        HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_SS_MUX0, 0);
        HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_SS_CTL0, 0);

        // Trigger sampling from timer
        HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_EMUX,
                         CYGHWR_HAL_LM3S_ADC_EMUX_EM_TIMER(0));

        // Set Averaging
        HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_SAC,
                         info->adc_avg);

        // Setup timer
        HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_CFG,
                         CYGHWR_HAL_LM3S_GPTIM_CFG_32BIT);
        HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_TAMR,
                         CYGHWR_HAL_LM3S_GPTIM_TAMR_PERIODIC);

        // Setup the default sample rate
        lm3s_adc_set_rate(chan, chan->device->config.rate);

    }

    // Initialize generic parts of driver
    cyg_adc_device_init(device);

    return true;
}


//==========================================================================
// This function is called when a client looks up or opens a channel. It
// should call cyg_adc_channel_init() to initialize the generic part of
// the channel. It should also perform any operations needed to start the
// channel generating samples.
//==========================================================================
static Cyg_ErrNo
lm3s_adc_lookup(struct cyg_devtab_entry **tab,
                struct cyg_devtab_entry *sub_tab, const char *name)
{
    cyg_adc_channel *chan = (cyg_adc_channel *) (*tab)->priv;
    lm3s_adc_info  *info = chan->device->dev_priv;

    lm3s_adc_diag("ADC: Opening channel %d\n", chan->channel);

    if (chan->channel > info->max_channel)
        return ENOENT;

    info->channel[chan->channel] = chan;

    // Initialize generic parts of channel
    cyg_adc_channel_init(chan);

    // The generic ADC manual says: When a channel is first looked up or
    // opened, then it is automatically enabled and samples start to
    // accumulate - so we start the channel now
    chan->enabled = true;
    lm3s_adc_enable(chan);

    return ENOERR;
}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_ENABLE config operation.
// It should take any steps needed to start the channel generating samples
//==========================================================================
static void
lm3s_adc_enable(cyg_adc_channel * chan)
{
    lm3s_adc_info  *info = chan->device->dev_priv;
    cyg_uint32      ctl =
        CYGHWR_HAL_LM3S_GPTIM_CTL_TAEN | CYGHWR_HAL_LM3S_GPTIM_CTL_TAOTE;
    cyg_uint32      start = !info->chan_mask;

    // Disable ADC sequencer 0 and timer
    HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_CTL, 0);
    lm3s_adc_disable_sequencer0(info->adc_base);

    // Update sequencer
    info->chan_mask |= (1 << chan->channel);
    lm3s_adc_update_sequencer0(chan);

    // Unmask interrupt as soon as 1 channel is enable
    if (start) {
        cyg_drv_interrupt_unmask(info->adc_vector);
    }
    // Enable sequencer and timer
    lm3s_adc_enable_sequencer0(info->adc_base);
    HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_CTL, ctl);
}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_DISABLE config operation.
// It should take any steps needed to stop the channel generating samples.
//==========================================================================
static void
lm3s_adc_disable(cyg_adc_channel * chan)
{
    lm3s_adc_info  *info = chan->device->dev_priv;
    cyg_uint32      ctl =
        CYGHWR_HAL_LM3S_GPTIM_CTL_TAEN | CYGHWR_HAL_LM3S_GPTIM_CTL_TAOTE;

    // Disable ADC sequencer 0 and timer
    HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_CTL, 0);
    lm3s_adc_disable_sequencer0(info->adc_base);

    // Update sequencer
    info->chan_mask &= ~(1 << chan->channel);
    lm3s_adc_update_sequencer0(chan);

    // Stop scanning when no channel is active
    if (!info->chan_mask) {
        cyg_drv_interrupt_mask(info->adc_vector);
        return;
    }
    // Enable sequencer and timer
    lm3s_adc_enable_sequencer0(info->adc_base);
    HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_CTL, ctl);
}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_RATE config operation.
// It should take any steps needed to change the sample rate of the channel,
// or of the entire device.
// We use a timer channel to generate the interrupts for sampling the
// analog channels
//==========================================================================
static void
lm3s_adc_set_rate(cyg_adc_channel * chan, cyg_uint32 rate)
{
    cyg_adc_device *device = chan->device;
    lm3s_adc_info  *info = (lm3s_adc_info *) device->dev_priv;

    info->timer_interval = hal_lm3s_timer_clock() / rate;

    lm3s_adc_diag("ADC: Timer interval %d\n", info->timer_interval);

    HAL_WRITE_UINT32(info->timer_base + CYGHWR_HAL_LM3S_GPTIM_TAILR,
                     info->timer_interval);
}


//==========================================================================
// This function is the ISR attached to the ADC device's interrupt vector.
// It is responsible for reading samples from the channels and passing them
// on to the generic layer. It needs to check each channel for data, and call
// cyg_adc_receive_sample() for each new sample available, and then ready the
// device for the next interrupt.
//==========================================================================
static cyg_uint32
lm3s_adc_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_adc_device *device = (cyg_adc_device *) data;
    lm3s_adc_info  *info = (lm3s_adc_info *) device->dev_priv;
    cyg_uint32      regval;
    cyg_uint32      res = 0;
    cyg_adc_sample_t adcdata;
    cyg_uint32      sr;

    cyg_uint8       active_channels = info->chan_mask;
    cyg_uint8       channel_no = 0;

    while (active_channels) {
        HAL_READ_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_SS_FIFO0_SR, sr);
        // Check FIFO Full
        if ((sr & CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_FULL)) {
            lm3s_adc_diag("ADC: FIFO Full\n");
        }
        // Check FIFO Empty
        if ((sr & CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_EMPTY)) {
            lm3s_adc_diag("ADC: FIFO Empty\n");
        }
        if (active_channels & 0x01) {
            // If ADC conversion done, save sample
            if (!(sr & CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_EMPTY)) {
                HAL_READ_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_SS_FIFO0,
                                regval);
                adcdata = regval & 0x3FF;
                res |= CYG_ISR_HANDLED
                    | cyg_adc_receive_sample(info->channel[channel_no],
                                             adcdata);
            }
        }
        active_channels >>= 1;
        channel_no++;
    }

    HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_ISCR,
                     CYGHWR_HAL_LM3S_ADC_ISCR_IN(0));

    cyg_drv_interrupt_acknowledge(info->adc_vector);

    return res;
}


//==========================================================================
// This function is the DSR attached to the ADC device's interrupt vector.
// It is called by the kernel if the ISR return value contains the
// CYG_ISR_HANDLED bit. It needs to call cyg_adc_wakeup() for each channel
// that has its wakeup field set.
//==========================================================================
static void
lm3s_adc_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_adc_device *device = (cyg_adc_device *) data;
    lm3s_adc_info  *info = device->dev_priv;
    cyg_uint8       active_channels = info->chan_mask;
    cyg_uint8       chan_no = 0;

    while (active_channels) {
        if (active_channels & 0x01) {
            if (info->channel[chan_no]->wakeup) {
                cyg_adc_wakeup(info->channel[chan_no]);
            }
        }
        chan_no++;
        active_channels >>= 1;
    }
}


static void
lm3s_adc_disable_sequencer0(cyg_uint32 base)
{
    cyg_uint32      reg;

    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_ADC_IMR, 0);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_ADC_ISCR,
                     CYGHWR_HAL_LM3S_ADC_ISCR_IN(0));
    HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_ADC_ACTSS, reg);
    reg &= ~(CYGHWR_HAL_LM3S_ADC_ACTSS_ASEN(0));
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_ADC_ACTSS, reg);
}


static void
lm3s_adc_enable_sequencer0(cyg_uint32 base)
{
    cyg_uint32      reg;

    HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_ADC_ACTSS, reg);
    reg |= (CYGHWR_HAL_LM3S_ADC_ACTSS_ASEN(0));
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_ADC_ACTSS, reg);
    HAL_WRITE_UINT32(base + CYGHWR_HAL_LM3S_ADC_IMR,
                     CYGHWR_HAL_LM3S_ADC_IMR_MASK(0));
}


static void
lm3s_adc_flush(cyg_uint32 base)
{
    volatile cyg_uint32 d;
    volatile cyg_uint32 i;

    HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_ADC_SS_FIFO0_SR, i);
    while (!(i & CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_EMPTY)) {
        HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_ADC_SS_FIFO0, d);
        HAL_READ_UINT32(base + CYGHWR_HAL_LM3S_ADC_SS_FIFO0_SR, i);
    }
}


static void
lm3s_adc_update_sequencer0(cyg_adc_channel * chan)
{
    lm3s_adc_info  *info = chan->device->dev_priv;
    cyg_uint8       i;
    cyg_uint8       cnt = 0;
    cyg_uint32      mux = 0;
    cyg_uint32      ctl = 0;

    lm3s_adc_diag("ADC: Update sequencer for channel %d\n", chan->channel);

    // Update sequencer
    for (i = 0; i < info->max_channel; i++) {
        if (!(info->chan_mask & (1 << i)))
            continue;

        // Clear and update MUX register
        mux &= ~(CYGHWR_HAL_LM3S_ADC_SS_MUX0_M(cnt));
        mux |= CYGHWR_HAL_LM3S_ADC_SS_MUX0_V(i, cnt);

        // Temperature sensor channel
        if (i == info->sensor_channel) {
            ctl |= CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS(cnt);
            lm3s_adc_diag("ADC: Channel %d mapped to temperature sensor\n",
                          i);
        }

        cnt++;
    }

    lm3s_adc_diag("ADC: MUX0 Register: 0x%x\n", mux);
    HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_SS_MUX0, mux);

    if (info->chan_mask) {
        ctl |= CYGHWR_HAL_LM3S_ADC_SS_CTL0_END((cnt - 1));
        ctl |= CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE((cnt - 1));
    }

    lm3s_adc_diag("ADC: CTL0 Register: 0x%x\n", ctl);
    HAL_WRITE_UINT32(info->adc_base + CYGHWR_HAL_LM3S_ADC_SS_CTL0, ctl);

    lm3s_adc_flush(info->adc_base);
}


//---------------------------------------------------------------------------
// EOF adc_lm3s.c
