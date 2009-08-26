//==========================================================================
//
//      adc_stm32.c
//
//      ADC driver for STM32 on chip ADC
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
// Author(s):    Simon Kallweit <simon.kallweit@intefo.ch>
// Contributors:
// Date:         2009-02-24
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_adc_cortexm_stm32.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/io/adc.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

//-----------------------------------------------------------------------------
// Diagnostic support
// Switch the #if to 1 to generate some diagnostic messages.

#if 0
#include <cyg/infra/diag.h>
#define adc_diag( __fmt, ... ) diag_printf("ADC: %30s[%4d]: " __fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__ );
#else
#define adc_diag( __fmt, ... ) 
#endif


//-----------------------------------------------------------------------------
// STM32 ADC device setup

typedef struct stm32_adc_setup {
    CYG_ADDRESS         adc_base;       // ADC registers base address
    CYG_ADDRESS         dma_base;       // DMA registers base address
    cyg_vector_t        dma_int_vector; // DMA interrupt vector
    cyg_priority_t      dma_int_pri;    // DMA interrupt priority
    cyg_uint8           dma_channel;    // DMA channel to use
    CYG_ADDRESS         tim_base;       // Timer registers base address
    const cyg_uint32    *pins;          // ADC associated GPIO pins
    cyg_uint8           extsel;         // ADC EXTSEL value (timer event)
    cyg_uint32          sample_time;    // ADC sampling time in us
} stm32_adc_setup;

//-----------------------------------------------------------------------------
// STM32 ADC device

typedef struct stm32_adc_info {
    const stm32_adc_setup   *setup;         // ADC setup
    cyg_handle_t            dma_int_handle; // DMA interrupt handle
    cyg_interrupt           dma_int_data;   // DMA interrupt data
    cyg_uint16              *dma_buf;       // DMA buffer
    cyg_adc_channel         *chan[18];      // Channel references by channel no
    cyg_uint32              chan_mask;      // Channel mask
} stm32_adc_info;

//-----------------------------------------------------------------------------
// API function call forward references

static bool stm32_adc_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo stm32_adc_lookup(struct cyg_devtab_entry **tab,
                                  struct cyg_devtab_entry *sub_tab,
                                  const char *name);

static void stm32_adc_enable(cyg_adc_channel *chan);
static void stm32_adc_disable(cyg_adc_channel *chan);
static void stm32_adc_set_rate(cyg_adc_channel *chan, cyg_uint32 rate);

static cyg_uint32 stm32_dma_isr(cyg_vector_t vector, cyg_addrword_t data);
static void stm32_dma_dsr(cyg_vector_t vector, cyg_ucount32 count,
                          cyg_addrword_t data);

static void stm32_adc_init_clock(void);
static void stm32_adc_init_device(cyg_adc_device *device);
static void stm32_adc_update_sequence(cyg_adc_device *device);

CYG_ADC_FUNCTIONS(stm32_adc_funs,
                  stm32_adc_enable,
                  stm32_adc_disable,
                  stm32_adc_set_rate);

//-----------------------------------------------------------------------------
// STM32 ADC channel instance macro

#define STM32_ADC_CHANNEL(_device_, _chan_)                                 \
CYG_ADC_CHANNEL(                                                            \
    stm32_adc##_device_##_channel##_chan_,                                  \
    _chan_,                                                                 \
    CYGDAT_DEVS_ADC_CORTEXM_STM32_ADC##_device_##_CHANNEL##_chan_##_BUFSIZE,\
    &stm32_adc_device##_device_                                             \
);                                                                          \
DEVTAB_ENTRY(                                                               \
    stm32_adc##_device_##_channel##_chan_##_device,                         \
    CYGDAT_DEVS_ADC_CORTEXM_STM32_ADC##_device_##_CHANNEL##_chan_##_NAME,   \
    0,                                                                      \
    &cyg_io_adc_devio,                                                      \
    stm32_adc_init,                                                         \
    stm32_adc_lookup,                                                       \
    &stm32_adc##_device_##_channel##_chan_                                  \
);

//-----------------------------------------------------------------------------
// STM32 ADC device instances

#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC1
#include "adc1.inl"
#endif

#ifdef CYGHWR_DEVS_ADC_CORTEXM_STM32_ADC3
#include "adc3.inl"
#endif

static cyg_bool initialized;
static cyg_uint32 adc_clock;

__externC cyg_uint32 hal_stm32_pclk1;
__externC cyg_uint32 hal_stm32_pclk2;

//-----------------------------------------------------------------------------
// This function is called from the device IO infrastructure to initialize the
// device. It should perform any work needed to start up the device, short of
// actually starting the generation of samples. This function will be called
// for each channel, so if there is initialization that only needs to be done
// once, such as creating and interrupt object, then care should be taken to do
// this. This function should also call cyg_adc_device_init() to initialize the
// generic parts of the driver.

static bool
stm32_adc_init(struct cyg_devtab_entry *tab)
{
    cyg_adc_channel *chan = (cyg_adc_channel *) tab->priv;
    cyg_adc_device *device = chan->device;
    stm32_adc_info *info = device->dev_priv;
    
    adc_diag("Initializing device\n");
    
    // Initialize ADC clock
    if (!initialized) {
        stm32_adc_init_clock();
        initialized = true;
    }
    
    // Keep reference to channel
    info->chan[chan->channel] = chan;

    if (!info->dma_int_handle) {
        // Initialize ADC device
        stm32_adc_init_device(device);
        
        // Set default rate
        stm32_adc_set_rate(chan, chan->device->config.rate);
        
        // Initialize DMA interrupt
        cyg_drv_interrupt_create(info->setup->dma_int_vector,
                                 info->setup->dma_int_pri,
                                (cyg_addrword_t) device,
                                &stm32_dma_isr,
                                &stm32_dma_dsr,
                                &info->dma_int_handle,
                                &info->dma_int_data);
        cyg_drv_interrupt_attach(info->dma_int_handle);
        cyg_drv_interrupt_unmask(info->setup->dma_int_vector);
    }
    
    // Initialize generic parts of ADC device
    cyg_adc_device_init(device);
    
    return true;
}

//-----------------------------------------------------------------------------
// This function is called when a client looks up or opens a channel. It should
// call cyg_adc_channel_init() to initialize the generic part of the channel.
// It should also perform any operations needed to start the channel generating
// samples.

static Cyg_ErrNo
stm32_adc_lookup(struct cyg_devtab_entry **tab,
                 struct cyg_devtab_entry *sub_tab,
                 const char *name)
{
    cyg_adc_channel *chan = (cyg_adc_channel *) (*tab)->priv;
    stm32_adc_info *info = chan->device->dev_priv;
    cyg_uint32 cr;

    adc_diag("Opening device\n");
    
    // Configure the input pin, if available
    if (info->setup->pins[chan->channel] != CYGHWR_HAL_STM32_GPIO_NONE)
        CYGHWR_HAL_STM32_GPIO_SET(info->setup->pins[chan->channel]);
    
    // Activate temperature and VREF if necessary
    if (chan->channel >= 16) {
        HAL_READ_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
        cr |= CYGHWR_HAL_STM32_ADC_CR2_TSVREFE;
        HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    }

    // Initialize generic parts of the channel
    cyg_adc_channel_init(chan);

    // The generic ADC manual says: When a channel is first looked up or 
    // opened, then it is automatically enabled and samples start to
    // accumulate - so we start the channel now
    chan->enabled = true;
    stm32_adc_enable(chan);

    return ENOERR;
}

//-----------------------------------------------------------------------------
// This function is called from the generic ADC package to enable the channel
// in response to a CYG_IO_SET_CONFIG_ADC_ENABLE config operation. It should
// take any steps needed to start the channel generating samples

static void
stm32_adc_enable(cyg_adc_channel *chan)
{
    stm32_adc_info *info = chan->device->dev_priv;
    cyg_uint32 cr;
    cyg_bool start;
    
    adc_diag("Enabling channel\n");

    start = !info->chan_mask;

    // Update the scanning sequence
    info->chan_mask |= (1 << chan->channel);
    stm32_adc_update_sequence(chan->device);
    
    // Start scanning when first channel was activated
    if (start) {
        // Enable timer
        adc_diag("Starting scanning\n");
        HAL_READ_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_CR1, cr);
        cr |= CYGHWR_HAL_STM32_TIM_CR1_CEN;
        HAL_WRITE_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_CR1, cr);
    }
}

//-----------------------------------------------------------------------------
// This function is called from the generic ADC package to enable the channel
// in response to a CYG_IO_SET_CONFIG_ADC_DISABLE config operation. It should
// take any steps needed to stop the channel generating samples.

static void
stm32_adc_disable(cyg_adc_channel *chan)
{
    stm32_adc_info *info = chan->device->dev_priv;
    cyg_uint32 cr;
    
    adc_diag("Disabling channel\n");
    
    // Update scanning sequence
    info->chan_mask &= ~(1 << chan->channel);
    stm32_adc_update_sequence(chan->device);
    
    // Stop scanning when no channel is active
    if (!info->chan_mask) {
        // Disable timer
        adc_diag("Stopping scanning\n");
        HAL_READ_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_CR1, cr);
        cr &= ~CYGHWR_HAL_STM32_TIM_CR1_CEN;
        HAL_WRITE_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_CR1, cr);
    }
}

//-----------------------------------------------------------------------------
// This function is called from the generic ADC package to enable the channel
// in response to a CYG_IO_SET_CONFIG_ADC_RATE config operation. It should take
// any steps needed to change the sample rate of the channel, or of the entire
// device. We use a timer channel to generate the interrupts for sampling the
// analog channels

static void
stm32_adc_set_rate( cyg_adc_channel *chan, cyg_uint32 rate)
{
    cyg_adc_device *device = chan->device;
    stm32_adc_info *info = device->dev_priv;
    cyg_uint32 clock;
    cyg_uint32 period, prescaler;
    cyg_uint32 cr;
    
    adc_diag("Setting rate to %d\n", rate);

    device->config.rate = rate;
    
    clock = hal_stm32_timer_clock(info->setup->tim_base);
    
    period = clock / rate;
    prescaler = (period / 0x10000) + 1;
    period = period / prescaler;
    
    HAL_WRITE_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_PSC,
                     prescaler - 1);
    HAL_WRITE_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_ARR,
                     period - 1);
    
    // Reinitialize timer
    cr = CYGHWR_HAL_STM32_TIM_EGR_UG;
    HAL_WRITE_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_EGR, cr);
}

//-----------------------------------------------------------------------------
// This function is the ISR attached to the ADC device's DMA channel interrupt
// vector. It is responsible for reading samples from the DMA buffer and
// passing them on to the generic layer.

static cyg_uint32
stm32_dma_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_adc_device *device = (cyg_adc_device *) data;
    stm32_adc_info *info = (stm32_adc_info *) device->dev_priv;
    cyg_uint32 chan_active = info->chan_mask;
    cyg_uint16 *sample = info->dma_buf;
    cyg_adc_channel **chan = info->chan;
    cyg_uint32 isr;
    cyg_uint32 res = CYG_ISR_HANDLED;
    
    HAL_READ_UINT32(info->setup->dma_base + CYGHWR_HAL_STM32_DMA_ISR, isr);
    if (!(isr & CYGHWR_HAL_STM32_DMA_ISR_MASK(info->setup->dma_channel)))
        return 0;
    
    while (chan_active) {
        if (chan_active & 0x1)
            res |= cyg_adc_receive_sample(*chan, *sample++ & 0xfff);
        chan_active >>= 1;
        chan++;
    }

    HAL_WRITE_UINT32(info->setup->dma_base + CYGHWR_HAL_STM32_DMA_IFCR,
                     CYGHWR_HAL_STM32_DMA_IFCR_MASK(info->setup->dma_channel));
    
    cyg_drv_interrupt_acknowledge(vector);
    
    return res;
}

//-----------------------------------------------------------------------------
// This function is the DSR attached to the ADC device's DMA channel interrupt
// vector. It is called by the kernel if the ISR return value contains the
// CYG_ISR_CALL_DSR bit. It needs to call cyg_adc_wakeup() for each channel
// that has its wakeup field set.

static void
stm32_dma_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_adc_device *device = (cyg_adc_device *) data;
    stm32_adc_info *info = (stm32_adc_info *) device->dev_priv;
    cyg_uint32 chan_active = info->chan_mask;
    cyg_adc_channel **chan = info->chan;
    
    while (chan_active) {
        if (chan_active & 0x1)
            if ((*chan)->wakeup)
                cyg_adc_wakeup(*chan);
        chan_active >>= 1;
        chan++;
    }
}

//-----------------------------------------------------------------------------
// Initializes the ADC system clock.

static void
stm32_adc_init_clock(void)
{
    CYG_ADDRESS rcc = CYGHWR_HAL_STM32_RCC;
    cyg_uint32 cfgr;
    
    adc_diag("Initializing ADC system clock\n");
    
    HAL_READ_UINT32(rcc + CYGHWR_HAL_STM32_RCC_CFGR, cfgr);
    cfgr &= ~CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_XXX;

#if CYGNUM_DEVS_ADC_CORTEXM_STM32_CLOCK_DIV == 2
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_2;
    adc_clock = hal_stm32_pclk2 / 2;
#elif CYGNUM_DEVS_ADC_CORTEXM_STM32_CLOCK_DIV == 4
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_4;
    adc_clock = hal_stm32_pclk2 / 4;
#elif CYGNUM_DEVS_ADC_CORTEXM_STM32_CLOCK_DIV == 6
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_6;
    adc_clock = hal_stm32_pclk2 / 6;
#elif CYGNUM_DEVS_ADC_CORTEXM_STM32_CLOCK_DIV == 8
    cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_8;
    adc_clock = hal_stm32_pclk2 / 8;
#endif

    HAL_READ_UINT32(rcc + CYGHWR_HAL_STM32_RCC_CFGR, cfgr);
}

//-----------------------------------------------------------------------------
// Initializes an ADC device.

static void
stm32_adc_init_device(cyg_adc_device *device)
{
    stm32_adc_info *info = device->dev_priv;
    cyg_uint32 cr;
    cyg_uint64 tmp;
    cyg_uint32 cycles;
    cyg_uint32 smpr;
    int i;
    
    static const cyg_uint32 cycles_table[] = 
        { 15, 75, 135, 285, 415, 555, 715, 2395 };
    
    // Make sure ADC is powered on
    cr = CYGHWR_HAL_STM32_ADC_CR2_ADON;
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    
    // Reset calibration
    cr |= CYGHWR_HAL_STM32_ADC_CR2_RSTCAL;
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    do {
        HAL_READ_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    } while (cr & CYGHWR_HAL_STM32_ADC_CR2_RSTCAL);
    
    // Do calibration
    cr |= CYGHWR_HAL_STM32_ADC_CR2_CAL;
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    do {
        HAL_READ_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    } while (cr & CYGHWR_HAL_STM32_ADC_CR2_CAL);
    
    // Power off ADC 
    cr &= CYGHWR_HAL_STM32_ADC_CR2_ADON;
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    
    // Enable external triggering and DMA
    cr |= CYGHWR_HAL_STM32_ADC_CR2_DMA |
          CYGHWR_HAL_STM32_ADC_CR2_EXTTRIG |
          CYGHWR_HAL_STM32_ADC_CR2_EXTSEL(info->setup->extsel);
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    
    // Enable scanning
    cr = CYGHWR_HAL_STM32_ADC_CR1_SCAN;
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR1, cr);


    // Set timer direction = down, clock divider = 1
    cr = CYGHWR_HAL_STM32_TIM_CR1_DIR | CYGHWR_HAL_STM32_TIM_CR1_CKD_1;
    HAL_WRITE_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_CR1, cr);

    // Enable generation of TRGO event
    cr = CYGHWR_HAL_STM32_TIM_CR2_MMS_UPDATE;
    HAL_WRITE_UINT32(info->setup->tim_base + CYGHWR_HAL_STM32_TIM_CR2, cr);
    

    // Setup DMA channel
    HAL_WRITE_UINT32(info->setup->dma_base + 
                     CYGHWR_HAL_STM32_DMA_CPAR(info->setup->dma_channel),
                     info->setup->adc_base + CYGHWR_HAL_STM32_ADC_DR);
    HAL_WRITE_UINT32(info->setup->dma_base +
                     CYGHWR_HAL_STM32_DMA_CMAR(info->setup->dma_channel),
                     (CYG_ADDRESS) info->dma_buf);
    HAL_WRITE_UINT32(info->setup->dma_base +
                     CYGHWR_HAL_STM32_DMA_CNDTR(info->setup->dma_channel),
                     0);
    HAL_WRITE_UINT32(info->setup->dma_base +
                     CYGHWR_HAL_STM32_DMA_CCR(info->setup->dma_channel),
                     CYGHWR_HAL_STM32_DMA_CCR_TCIE |
                     CYGHWR_HAL_STM32_DMA_CCR_TEIE |
                     CYGHWR_HAL_STM32_DMA_CCR_CIRC |
                     CYGHWR_HAL_STM32_DMA_CCR_MINC |
                     CYGHWR_HAL_STM32_DMA_CCR_PSIZE16 |
                     CYGHWR_HAL_STM32_DMA_CCR_MSIZE16);

    // Compute duration of a single cycle in pico-seconds
    tmp = 1000000000000LL / adc_clock;
    // Compute tenths of cycles for target sample time
    tmp = (info->setup->sample_time * 1000000 * 10) / tmp;
    cycles = tmp;
    
    adc_diag("Setting ADC sample time to %d us (%d.%d cycles)\n",
             info->setup->sample_time, cycles / 10, cycles % 10);
    
    // Find best matching SMPR value
    if (cycles > cycles_table[7]) {
        adc_diag("ADC sample time too long\n");
        smpr = 7;
    } else {
        for (smpr = 7; smpr > 0; smpr--)
            if (cycles > cycles_table[smpr])
                break;
    }
    
    // Expand SMPR value to all channels
    for (i = 0; i < 10; i++)
        smpr |= smpr << 3;
    
    // Set sampling time
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_SMPR1, smpr);
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_SMPR2, smpr);
}

//-----------------------------------------------------------------------------
// Updates the sequence for the regular group. ADC and DMA are disabled during
// the update. The sequence registers and DMA count registers are rewritten.
// Note: As the regular group consists of 16 channels max, we cannot activate
// the theoretical maximum of 18 channels (analog ins + temperature/VREF).

static void
stm32_adc_update_sequence(cyg_adc_device *device)
{
    stm32_adc_info *info = device->dev_priv;
    int i;
    int count = 0;
    cyg_uint32 cr;
    cyg_uint32 sqr1 = 0;
    cyg_uint32 sqr2 = 0;
    cyg_uint32 sqr3 = 0;
    
    adc_diag("Updateing regular group\n");
    
    // Disable ADC
    HAL_READ_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    cr &= ~CYGHWR_HAL_STM32_ADC_CR2_ADON;
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);

    // Disable DMA
    HAL_READ_UINT32(info->setup->dma_base +
                    CYGHWR_HAL_STM32_DMA_CCR(info->setup->dma_channel), cr);
    cr &= ~CYGHWR_HAL_STM32_DMA_CCR_EN;
    HAL_WRITE_UINT32(info->setup->dma_base +
                     CYGHWR_HAL_STM32_DMA_CCR(info->setup->dma_channel), cr);

    // Initialize scanning sequence (regular group)
    for (i = 0; i < 18; i++) {
        if (!(info->chan_mask & (1 << i)))
            continue;
        
        if (count < 6) {
            sqr3 |= CYGHWR_HAL_STM32_ADC_SQRx_SQ(count, i);
        } else if (count < 12) {
            sqr2 |= CYGHWR_HAL_STM32_ADC_SQRx_SQ(count - 6, i);
        } else if (count < 16) {
            sqr1 |= CYGHWR_HAL_STM32_ADC_SQRx_SQ(count - 12, i);
        } else {
            CYG_FAIL("Too many active channels\n");
        }
        count++;
    }
    
    sqr1 |= CYGHWR_HAL_STM32_ADC_SQR1_L(count - 1);
    
    adc_diag("sqr1: %p sqr2: %p sqr3: %p\n",
             (void *) sqr1, (void *) sqr2, (void *) sqr3);
    
    // Write sequence registers
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_SQR1, sqr1);
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_SQR2, sqr2);
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_SQR3, sqr3);
    
    // Update DMA
    HAL_WRITE_UINT32(info->setup->dma_base +
                     CYGHWR_HAL_STM32_DMA_CNDTR(info->setup->dma_channel),
                     count);
    
    // Enable DMA
    HAL_READ_UINT32(info->setup->dma_base +
                    CYGHWR_HAL_STM32_DMA_CCR(info->setup->dma_channel), cr);
    cr |= CYGHWR_HAL_STM32_DMA_CCR_EN;
    HAL_WRITE_UINT32(info->setup->dma_base +
                     CYGHWR_HAL_STM32_DMA_CCR(info->setup->dma_channel), cr);
    
    // Enable ADC
    HAL_READ_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
    cr |= CYGHWR_HAL_STM32_ADC_CR2_ADON;
    HAL_WRITE_UINT32(info->setup->adc_base + CYGHWR_HAL_STM32_ADC_CR2, cr);
}

//-----------------------------------------------------------------------------
// End of adc_stm32.c
