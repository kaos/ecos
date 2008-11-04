//==========================================================================
//
//      adc_lpc24xx.c
//
//      ADC driver for LPC24xx on chip ADC
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler <uwe_kindler@web.de>
// Contributors:
// Date:         2008-09-21
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
#include <pkgconf/devs_adc_arm_lpc24xx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/adc.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>



//==========================================================================
//                                DEFINES
//==========================================================================

//--------------------------------------------------------------------------
// Register definition
//
#define ADC_BASE        CYGARC_HAL_LPC24XX_REG_AD_BASE
#define ADC_CR         (ADC_BASE + 0x0000)
#define ADC_GDR        (ADC_BASE + 0x0004)
#define ADC_INTEN      (ADC_BASE + 0x000C)
#define ADC_DR(_chan_) (ADC_BASE + 0x0010 + ((_chan_) << 2))
#define ADC_STAT       (ADC_BASE + 0x0030)

#define DR_OVR         (0x01 << 30)
#define DR_DONE        (0x01 << 31)
#define CR_BURST       (0x01 << 16)
#define CR_PDN         (0x01 << 21)


#if CYGPKG_DEVS_ADC_ARM_LPC24XX_DEBUG_LEVEL > 0
   #define debug1_printf(args...) diag_printf(args)
#else
   #define debug1_printf(args...)
#endif

#define LPC2XXX_CHAN_CNT 8 // maximum number of channels for LPC2xxx device


//==========================================================================
//                                  DATA TYPES
//==========================================================================
typedef struct lpc2xxx_adc_info
{
    cyg_uint32              base;       // base address of ADC peripheral
    cyg_vector_t            vector;     // interrupt vector number
    int                     intprio;    // interrupt priority of ADC interrupt
    cyg_handle_t            int_handle; // For initializing the interrupt
    cyg_interrupt           int_data;
    struct cyg_adc_channel* channel[LPC2XXX_CHAN_CNT]; // stores references to 
                                                       // channel objects
#if CYGPKG_DEVS_ADC_ARM_LPC24XX_DEBUG_LEVEL > 1
    cyg_uint32              isr_cnt;     // number of ISR = number of samples
    cyg_uint32              zero_time;
#endif // CYGPKG_DEVS_ADC_ARM_LPC24XX_DEBUG_LEVEL > 1
    cyg_uint8               chan_mask;  // mask that indicates channels used 
                                        // by ADC driver
} lpc2xxx_adc_info;


//==========================================================================
//                               DECLARATIONS
//==========================================================================
static bool lpc2xxx_adc_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo lpc2xxx_adc_lookup(struct cyg_devtab_entry **tab,
                                    struct cyg_devtab_entry  *sub_tab,
                                    const char               *name);
static void lpc2xxx_adc_enable( cyg_adc_channel *chan );
static void lpc2xxx_adc_disable( cyg_adc_channel *chan );
static void lpc2xxx_adc_set_rate( cyg_adc_channel *chan, cyg_uint32 rate );
static cyg_uint32 lpc2xxx_adc_isr(cyg_vector_t vector, cyg_addrword_t data);
static void lpc2xxx_adc_dsr(cyg_vector_t vector,
                            cyg_ucount32 count,
                            cyg_addrword_t data);


//==========================================================================
// Instantiate data structures

// -------------------------------------------------------------------------
// Driver functions:
CYG_ADC_FUNCTIONS( lpc2xxx_adc_funs,
                   lpc2xxx_adc_enable,
                   lpc2xxx_adc_disable,
                   lpc2xxx_adc_set_rate );

// -------------------------------------------------------------------------
// Device instance:
static lpc2xxx_adc_info lpc2xxx_adc_info0 =
{
    .base               = CYGARC_HAL_LPC2XXX_REG_AD_BASE,
    .vector             = CYGNUM_HAL_INTERRUPT_TIMER1,
    .intprio            = CYGNUM_DEVS_ADC_ARM_LPC24XX_INTPRIO,
    .int_handle         = 0,
#if CYGPKG_DEVS_ADC_ARM_LPC24XX_DEBUG_LEVEL > 0
    .isr_cnt            = 0,
#endif
    .chan_mask          = 0
};

CYG_ADC_DEVICE( lpc2xxx_adc_device,
                &lpc2xxx_adc_funs,
                &lpc2xxx_adc_info0,
                CYGNUM_DEVS_ADC_ARM_LPC24XX_DEFAULT_RATE);

// -------------------------------------------------------------------------
// Channel instances:

#define LPC2XXX_ADC_CHANNEL( __chan )                                    \
CYG_ADC_CHANNEL( lpc2xxx_adc_channel##__chan,                            \
                 __chan,                                                 \
                 CYGDAT_DEVS_ADC_ARM_LPC24XX_CHANNEL##__chan##_BUFSIZE,  \
                 &lpc2xxx_adc_device );                                  \
                                                                         \
DEVTAB_ENTRY( lpc2xxx_adc_channel##__chan##_device,                      \
              CYGDAT_DEVS_ADC_ARM_LPC24XX_CHANNEL##__chan##_NAME,        \
              0,                                                         \
              &cyg_io_adc_devio,                                         \
              lpc2xxx_adc_init,                                          \
              lpc2xxx_adc_lookup,                                        \
              &lpc2xxx_adc_channel##__chan );

#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL0
LPC2XXX_ADC_CHANNEL(0);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL1
LPC2XXX_ADC_CHANNEL(1);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL2
LPC2XXX_ADC_CHANNEL(2);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL3
LPC2XXX_ADC_CHANNEL(3);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL4
LPC2XXX_ADC_CHANNEL(4);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL5
LPC2XXX_ADC_CHANNEL(5);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL6
LPC2XXX_ADC_CHANNEL(6);
#endif
#ifdef CYGPKG_DEVS_ADC_ARM_LPC24XX_CHANNEL7
LPC2XXX_ADC_CHANNEL(7);
#endif

//==========================================================================
// This function is called from the device IO infrastructure to initialize
// the device. It should perform any work needed to start up the device,
// short of actually starting the generation of samples. This function will
// be called for each channel, so if there is initialization that only needs
// to be done once, such as creating and interrupt object, then care should
// be taken to do this. This function should also call cyg_adc_device_init()
// to initialize the generic parts of the driver.
//==========================================================================
static bool lpc2xxx_adc_init(struct cyg_devtab_entry *tab)
{
    cyg_adc_channel  *chan   = (cyg_adc_channel *)tab->priv;
    cyg_adc_device   *device = chan->device;
    lpc2xxx_adc_info *info   = device->dev_priv;

    if (!info->int_handle)
    {
        cyg_drv_interrupt_create(info->vector,
                                 info->intprio,
                                (cyg_addrword_t)device,
                                &lpc2xxx_adc_isr,
                                &lpc2xxx_adc_dsr,
                                &(info->int_handle),
                                &(info->int_data));
        cyg_drv_interrupt_attach(info->int_handle);
        cyg_drv_interrupt_unmask(info->vector);

        //
        // The APB clock (PCLK) is divided by (this value plus one) to produce
        // the clock for the A/D converter, which should be less than or equal
        // to 4.5 MHz. Typically, software should program the smallest value in
        // this field that yields a clock of 4.5 MHz or slightly less, but in
        // certain cases (such as a high-impedance analog source) a slower
        // clock may be desirable.
        // Set clock division factor so ADC clock is <= 4.5 MHz
        //
        cyg_uint8 clkdiv = CYGNUM_HAL_ARM_LPC24XX_ADC_CLK / 4500001;

        //
        // Enable A/D converter and setup the configured sample size
        // The eCos ADC I/O manual says: Channels are initialized in a disabled
        // state and generate no samples - let's do this now
        // We initialize the device to operate in burst mode and we enable
        // conversion for all channels here
        //
        HAL_WRITE_UINT32(ADC_INTEN, 0);         // disables all interrupts
        HAL_WRITE_UINT32(ADC_CR, CR_BURST       // burst mode
                               | CR_PDN         // A/D converter is operational
                               | 0xFF           // enable all channels
                               | ((10 - CYGNUM_IO_ADC_SAMPLE_SIZE) << 17)
                               | (clkdiv << 8));// set clock divider

        //
        // setup the default sample rate
        //
        lpc2xxx_adc_set_rate(chan, chan->device->config.rate);
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
static Cyg_ErrNo lpc2xxx_adc_lookup(struct cyg_devtab_entry **tab,
                                    struct cyg_devtab_entry  *sub_tab,
                                    const char               *name)
{
    typedef struct adc_pin_cfg_st
    {
        cyg_uint8 port;
        cyg_uint8 pin;
        cyg_uint8 func;
    } adc_pin_cfg_t;
    static const adc_pin_cfg_t acd_pin_cfg_tbl[] =
    {
        {0, 23, 1},
        {0, 24, 1},
        {0, 25, 1},
        {0, 26, 1},
        {1, 30, 3},
        {1, 31, 3},
        {0, 12, 3},
        {0, 13, 3},
    };
    cyg_adc_channel  *chan     = (cyg_adc_channel *)(*tab)->priv;
    lpc2xxx_adc_info *info     = chan->device->dev_priv;
    adc_pin_cfg_t    *pin_cfg  = (adc_pin_cfg_t *)&acd_pin_cfg_tbl[chan->channel];

    //
    // This ADC driver is quite LP24xx specific. The pin function of each pin
    // is well defined in the LP24xx specification. Therefore we can setup
    // the pin function here. If someone decides that this driver can be used 
    // by other LPC2xxx or LPC3xxx variants too and that the driver should 
    // become more generic, then we might need to move the pin configuration 
    // out of this driver an into the variant / platform HAL
    //
    CYG_HAL_ARM_LPC24XX_PIN_CFG(pin_cfg->port, pin_cfg->pin, pin_cfg->func);
    info->channel[chan->channel] = chan;
    cyg_adc_channel_init(chan); // initialize generic parts of channel

    //
    // The generic ADC manual says: When a channel is first looked up or 
    // opened, then it is automatically enabled and samples start to
    // accumulate - so we start the channel now
    //
    chan->enabled = true;
    lpc2xxx_adc_enable(chan);

    return ENOERR;
}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_ENABLE config operation.
// It should take any steps needed to start the channel generating samples
//==========================================================================
static void lpc2xxx_adc_enable(cyg_adc_channel *chan)
{
    cyg_uint32        regval;
    lpc2xxx_adc_info *info      = chan->device->dev_priv;

    //
    // Enable interrupts for timer to start generation of samples in timer
    // ISR if this is the first channel that is enabled. If there are
    // already some channels enabled, then the interrupt is already enabled
    //
    if (!info->chan_mask)
    {
        HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                        CYGARC_HAL_LPC24XX_REG_TxMCR, regval);
#if CYGPKG_DEVS_ADC_ARM_LPC24XX_DEBUG_LEVEL > 0
        info->zero_time = cyg_current_time() * 10;
#endif
        regval |= CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_INT;
        HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                        CYGARC_HAL_LPC24XX_REG_TxMCR, regval);
    }
   
    info->chan_mask |= (0x01 << chan->channel);
}


//==========================================================================
// This function is called from the generic ADC package to enable the
// channel in response to a CYG_IO_SET_CONFIG_ADC_DISABLE config operation.
// It should take any steps needed to stop the channel generating samples.
//==========================================================================
static void lpc2xxx_adc_disable(cyg_adc_channel *chan)
{
    cyg_uint32        regval;
    lpc2xxx_adc_info *info  = chan->device->dev_priv;

    info->chan_mask &= ~(0x01 << chan->channel);
    
    //
    // If no channel is enabled the we disable interrupts now
    //
    if (!info->chan_mask)
    {
        HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                        CYGARC_HAL_LPC24XX_REG_TxMCR, regval);
        regval &= ~CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_INT;
        HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                        CYGARC_HAL_LPC24XX_REG_TxMCR, regval);
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
static void lpc2xxx_adc_set_rate( cyg_adc_channel *chan, cyg_uint32 rate)
{
    cyg_adc_device   *device = chan->device;
    cyg_uint32        regval;

    cyg_uint32 tmr_period = hal_lpc_get_pclk(CYNUM_HAL_LPC24XX_PCLK_TIMER1) /
                            rate;
    device->config.rate = rate;

    //
    // Disable and reset counter, set prescale register to 0 and
    // Set up match register
    //
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                     CYGARC_HAL_LPC24XX_REG_TxTCR,
                     CYGARC_HAL_LPC24XX_REG_TxTCR_CTR_RESET);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                     CYGARC_HAL_LPC24XX_REG_TxPR, 0);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                     CYGARC_HAL_LPC24XX_REG_TxMR0, tmr_period);
    //
    // Reset on match and Enable counter
    //
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                    CYGARC_HAL_LPC24XX_REG_TxMCR, regval);
    regval |= CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_RESET; // reset on match
    regval &= ~CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_STOP; // do not stop on match
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                     CYGARC_HAL_LPC24XX_REG_TxMCR, regval);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                     CYGARC_HAL_LPC24XX_REG_TxTCR,
                     CYGARC_HAL_LPC24XX_REG_TxTCR_CTR_ENABLE);
}


//==========================================================================
// This function is the ISR attached to the ADC device's interrupt vector.
// It is responsible for reading samples from the channels and passing them
// on to the generic layer. It needs to check each channel for data, and call
// cyg_adc_receive_sample() for each new sample available, and then ready the
// device for the next interrupt.
//==========================================================================
static cyg_uint32 lpc2xxx_adc_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_adc_device   *device = (cyg_adc_device *) data;
    lpc2xxx_adc_info *info   = (lpc2xxx_adc_info *)device->dev_priv;
    cyg_uint32        regval;
    cyg_uint32        res = 0;
    cyg_adc_sample_t  adcdata;

#if CYGPKG_DEVS_ADC_ARM_LPC24XX_DEBUG_LEVEL > 1
    //
    // Print debug information for channel 1 - this is the channel that
    // triggers the interrupt and that is used for measuring lost samples
    //
    if (!(++info->isr_cnt % device->config.rate))
    {
        cyg_uint32 current_time_ms = cyg_current_time() * 10;
        debug1_printf("ms %d smpl. %d\n",
                      current_time_ms - info->zero_time, info->isr_cnt);
        info->zero_time = current_time_ms;
    } // if (!(info->isr_count % device->config.rate))
#endif // CYGPKG_DEVS_ADC_ARM_LPC24XX_DEBUG_LEVEL > 1

    cyg_uint8 active_channels = info->chan_mask;
    cyg_uint8 channel_no = 0;
    while (active_channels)
    {
        if (active_channels & 0x01)
        {
            HAL_READ_UINT32(ADC_DR(channel_no), regval);
            adcdata = (regval >> 6) & 0x3FF;
            res |= CYG_ISR_HANDLED
                |  cyg_adc_receive_sample(info->channel[channel_no],
                                          adcdata);
        } // if (active_channels & 0x01)
        active_channels >>= 1;
        channel_no++;
    } // while (active_channels)

    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_TIMER1_BASE +
                     CYGARC_HAL_LPC24XX_REG_TxIR,
                     CYGARC_HAL_LPC24XX_REG_TxIR_MR0);  // Clear interrupt
    cyg_drv_interrupt_acknowledge(info->vector);
    return res;
}


//==========================================================================
// This function is the DSR attached to the ADC device's interrupt vector.
// It is called by the kernel if the ISR return value contains the
// CYG_ISR_HANDLED bit. It needs to call cyg_adc_wakeup() for each channel
// that has its wakeup field set.
//==========================================================================
static void lpc2xxx_adc_dsr(cyg_vector_t vector,
                            cyg_ucount32 count,
                            cyg_addrword_t data)
{
    cyg_adc_device   *device          = (cyg_adc_device *) data;
    lpc2xxx_adc_info *info            = device->dev_priv;
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
// eof adc_lpc24xx.c
