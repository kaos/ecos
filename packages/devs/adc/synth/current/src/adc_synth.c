//==========================================================================
//
//      adc_synth.c
//
//      ADC driver for Synthetic ADC
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
// Date:         2009-02-27
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>
#include <pkgconf/devs_adc_synth.h>

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

#define NUM_CHANNELS    16

#define MODE_CONST      0
#define MODE_RANDOM     1
#define MODE_FILE       2

#define SAMPLE_BITS     ((1 << CYGNUM_DEVS_ADC_SYNTH_SAMPLE_SIZE) - 1)

//-----------------------------------------------------------------------------
// Synthetic ADC channel

typedef struct synth_adc_channel_info {
    cyg_uint32 mode;            // Channel mode
    cyg_uint32 const_value;     // Const sample value
    char *filename;             // Sampling data filename

    int fd;                     // File descriptor of sample file
    cyg_uint32 num_samples;     // Number of samples in the file
    cyg_adc_sample_t *base;     // Base address of mapped sample file
    cyg_adc_sample_t *sample;   // Current sample
    
    cyg_adc_sample_t (*get_sample)(cyg_adc_channel *chan);
} synth_adc_channel_info;

//-----------------------------------------------------------------------------
// Synthetic ADC device

typedef struct synth_adc_info {
    synth_adc_channel_info  *chan_info;     // Channel infos
    cyg_adc_channel *chan[NUM_CHANNELS];    // Channel references
    cyg_uint32 chan_mask;                   // Active channels
    cyg_handle_t alarm_handle;              // Alarm handle
    cyg_alarm alarm_data;                   // Alarm data
    cyg_tick_count_t alarm_interval;        // Alarm interval in ticks
    cyg_uint32 alarm_samples;               // Number of samples per tick
} synth_adc_info;

//-----------------------------------------------------------------------------
// API function call forward references

static bool synth_adc_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo synth_adc_lookup(struct cyg_devtab_entry **tab,
                                  struct cyg_devtab_entry *sub_tab,
                                  const char *name);

static void synth_adc_enable(cyg_adc_channel *chan);
static void synth_adc_disable(cyg_adc_channel *chan);
static void synth_adc_set_rate(cyg_adc_channel *chan, cyg_uint32 rate);

static void alarm_handler(cyg_handle_t alarm, cyg_addrword_t data);

static cyg_adc_sample_t synth_adc_get_sample_const(cyg_adc_channel *chan);
static cyg_adc_sample_t synth_adc_get_sample_random(cyg_adc_channel *chan);
static cyg_adc_sample_t synth_adc_get_sample_file(cyg_adc_channel *chan);

static cyg_uint32 rand(void);

CYG_ADC_FUNCTIONS(synth_adc_funs,
                  synth_adc_enable,
                  synth_adc_disable,
                  synth_adc_set_rate);

//-----------------------------------------------------------------------------
// Synthetic ADC channel info macro

#define SYNTH_ADC_CHANNEL_INFO(_chan_)                                      \
{                                                                           \
    .mode           = CYGNUM_DEVS_ADC_SYNTH_CHANNEL##_chan_##_MODE,         \
    .const_value    = CYGNUM_DEVS_ADC_SYNTH_CHANNEL##_chan_##_CONST_VALUE,  \
    .filename       = CYGDAT_DEVS_ADC_SYNTH_CHANNEL##_chan_##_FILENAME,     \
}

//-----------------------------------------------------------------------------
// Synthetic ADC channel instance macro

#define SYNTH_ADC_CHANNEL(_chan_)                                           \
CYG_ADC_CHANNEL(                                                            \
    synth_adc_channel##_chan_,                                              \
    _chan_,                                                                 \
    CYGNUM_DEVS_ADC_SYNTH_CHANNEL##_chan_##_BUFSIZE,                        \
    &synth_adc_device                                                       \
);                                                                          \
DEVTAB_ENTRY(                                                               \
    synth_adc_channel##_chan_##_device,                                     \
    CYGDAT_DEVS_ADC_SYNTH_CHANNEL##_chan_##_NAME,                           \
    0,                                                                      \
    &cyg_io_adc_devio,                                                      \
    synth_adc_init,                                                         \
    synth_adc_lookup,                                                       \
    &synth_adc_channel##_chan_                                              \
);

//-----------------------------------------------------------------------------
// Synthetic ADC device instance

static synth_adc_channel_info synth_adc_channel_infos[NUM_CHANNELS] = {
    SYNTH_ADC_CHANNEL_INFO(0),
    SYNTH_ADC_CHANNEL_INFO(1),
    SYNTH_ADC_CHANNEL_INFO(2),
    SYNTH_ADC_CHANNEL_INFO(3),
    SYNTH_ADC_CHANNEL_INFO(4),
    SYNTH_ADC_CHANNEL_INFO(5),
    SYNTH_ADC_CHANNEL_INFO(6),
    SYNTH_ADC_CHANNEL_INFO(7),
    SYNTH_ADC_CHANNEL_INFO(8),
    SYNTH_ADC_CHANNEL_INFO(9),
    SYNTH_ADC_CHANNEL_INFO(10),
    SYNTH_ADC_CHANNEL_INFO(11),
    SYNTH_ADC_CHANNEL_INFO(12),
    SYNTH_ADC_CHANNEL_INFO(13),
    SYNTH_ADC_CHANNEL_INFO(14),
    SYNTH_ADC_CHANNEL_INFO(15),
};

static synth_adc_info synth_adc_info0 = {
    .chan_info = synth_adc_channel_infos,
};

CYG_ADC_DEVICE(synth_adc_device,
               &synth_adc_funs,
               &synth_adc_info0,
               CYGNUM_DEVS_ADC_SYNTH_DEFAULT_RATE);

SYNTH_ADC_CHANNEL(0)
SYNTH_ADC_CHANNEL(1)
SYNTH_ADC_CHANNEL(2)
SYNTH_ADC_CHANNEL(3)
SYNTH_ADC_CHANNEL(4)
SYNTH_ADC_CHANNEL(5)
SYNTH_ADC_CHANNEL(6)
SYNTH_ADC_CHANNEL(7)
SYNTH_ADC_CHANNEL(8)
SYNTH_ADC_CHANNEL(9)
SYNTH_ADC_CHANNEL(10)
SYNTH_ADC_CHANNEL(11)
SYNTH_ADC_CHANNEL(12)
SYNTH_ADC_CHANNEL(13)
SYNTH_ADC_CHANNEL(14)
SYNTH_ADC_CHANNEL(15)


//-----------------------------------------------------------------------------
// This function is called from the device IO infrastructure to initialize the
// device. It should perform any work needed to start up the device, short of
// actually starting the generation of samples. This function will be called
// for each channel, so if there is initialization that only needs to be done
// once, such as creating and interrupt object, then care should be taken to do
// this. This function should also call cyg_adc_device_init() to initialize the
// generic parts of the driver.

static bool
synth_adc_init(struct cyg_devtab_entry *tab)
{
    static cyg_bool initialized = false;
    cyg_adc_channel *chan = (cyg_adc_channel *) tab->priv;
    cyg_adc_device *device = chan->device;
    synth_adc_info *info = device->dev_priv;
    synth_adc_channel_info *chan_info = &info->chan_info[chan->channel];
    cyg_handle_t counter;
    
    adc_diag("Initializing device\n");
    
    // Initialize channel
    info->chan[chan->channel] = chan;
    switch (chan_info->mode) {
    case MODE_CONST:
        chan_info->get_sample = synth_adc_get_sample_const;
        break;
    case MODE_RANDOM:
        chan_info->get_sample = synth_adc_get_sample_random;
        break;
    case MODE_FILE:
        chan_info->get_sample = synth_adc_get_sample_file;
        break;
    }

    // Set default rate
    if (!initialized) {
        // Initialize alarm
        cyg_clock_to_counter(cyg_real_time_clock(), &counter);
        cyg_alarm_create(counter, alarm_handler, (cyg_addrword_t) device,
                         &info->alarm_handle, &info->alarm_data);
        
        synth_adc_set_rate(chan, chan->device->config.rate);
        initialized = true;
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
synth_adc_lookup(struct cyg_devtab_entry **tab,
                 struct cyg_devtab_entry *sub_tab,
                 const char *name)
{
    cyg_adc_channel *chan = (cyg_adc_channel *) (*tab)->priv;
    synth_adc_info *info = chan->device->dev_priv;
    synth_adc_channel_info *chan_info = &info->chan_info[chan->channel];

    adc_diag("Opening device\n");
    
    // When this channel is in file mode, initialize file access
    if (chan_info->mode == MODE_FILE) {
        struct cyg_hal_sys_new_stat stat;
        
        // Open the file
        chan_info->fd = cyg_hal_sys_open(chan_info->filename,
                                         CYG_HAL_SYS_O_RDONLY, 0);
        if (chan_info->fd == -ENOENT) {
            adc_diag("Cannot open sampling file '%s' for channel '%s'\n", 
                     chan_info->filename, (*tab)->name);
            CYG_FAIL("Cannot open sampling file\n");
        }
        
        // Get file size
        if (cyg_hal_sys_newfstat(chan_info->fd, &stat) != 0) {
            CYG_FAIL("Cannot stat sampling file\n");
        }
        chan_info->num_samples = stat.st_size / sizeof(cyg_adc_sample_t);
        if (chan_info->num_samples <= 0)
            CYG_FAIL("Sampling file too small\n");
        
        // Memory map
        chan_info->base = (cyg_adc_sample_t *) cyg_hal_sys_mmap(
            NULL,
            chan_info->num_samples * sizeof(cyg_adc_sample_t),
            CYG_HAL_SYS_PROT_READ,
            CYG_HAL_SYS_MAP_SHARED,
            chan_info->fd,
            0);
        if (chan_info->base == (void *) -1)
            CYG_FAIL("Cannot memory map sampling file\n");
        chan_info->sample = chan_info->base;

        adc_diag("Mapped to %p\n", chan_info->base);
    }
    
    // Initialize generic parts of the channel
    cyg_adc_channel_init(chan);

    // The generic ADC manual says: When a channel is first looked up or 
    // opened, then it is automatically enabled and samples start to
    // accumulate - so we start the channel now
    chan->enabled = true;
    synth_adc_enable(chan);

    return ENOERR;
}

//-----------------------------------------------------------------------------
// This function is called from the generic ADC package to enable the channel
// in response to a CYG_IO_SET_CONFIG_ADC_ENABLE config operation. It should
// take any steps needed to start the channel generating samples

static void
synth_adc_enable(cyg_adc_channel *chan)
{
    synth_adc_info *info = chan->device->dev_priv;
    cyg_bool start;
    
    adc_diag("Enabling channel\n");

    start = !info->chan_mask;
    info->chan_mask |= (1 << chan->channel);
    
    // Start scanning when first channel was activated
    if (start) {
        // Enable timer
        adc_diag("Starting scanning\n");
        cyg_alarm_initialize(info->alarm_handle,
                             cyg_current_time() + info->alarm_interval,
                             info->alarm_interval);
    }
}

//-----------------------------------------------------------------------------
// This function is called from the generic ADC package to enable the channel
// in response to a CYG_IO_SET_CONFIG_ADC_DISABLE config operation. It should
// take any steps needed to stop the channel generating samples.

static void
synth_adc_disable(cyg_adc_channel *chan)
{
    synth_adc_info *info = chan->device->dev_priv;
    
    adc_diag("Disabling channel\n");
    
    info->chan_mask &= ~(1 << chan->channel);
    
    // Stop scanning when no channel is active
    if (!info->chan_mask) {
        // Disable timer
        adc_diag("Stopping scanning\n");
        cyg_alarm_disable(info->alarm_handle);
    }
}

//-----------------------------------------------------------------------------
// This function is called from the generic ADC package to enable the channel
// in response to a CYG_IO_SET_CONFIG_ADC_RATE config operation. It should take
// any steps needed to change the sample rate of the channel, or of the entire
// device. We use a timer channel to generate the interrupts for sampling the
// analog channels

static void
synth_adc_set_rate(cyg_adc_channel *chan, cyg_uint32 rate)
{
    cyg_adc_device *device = chan->device;
    synth_adc_info *info = device->dev_priv;
    cyg_uint64 interval;
    
    adc_diag("Setting rate to %d\n", rate);
        
    interval = 1000000000000LL / rate;
    interval /= (CYGNUM_HAL_RTC_NUMERATOR / CYGNUM_HAL_RTC_DENOMINATOR);
    
    if (interval > 1000) {
        info->alarm_interval = interval / 1000;
        info->alarm_samples = 1;
    } else {
        info->alarm_interval = 1;
        info->alarm_samples = 1000 / interval;
    }
    
    if (info->chan_mask)
        cyg_alarm_initialize(info->alarm_handle,
                             cyg_current_time() + info->alarm_interval,
                             info->alarm_interval);

    device->config.rate = rate;
}

static void
alarm_handler(cyg_handle_t alarm, cyg_addrword_t data)
{
    cyg_adc_device *device = (cyg_adc_device *) data;
    synth_adc_info *info = device->dev_priv;
    cyg_adc_channel *chan;
    synth_adc_channel_info *chan_info;
    cyg_uint32 active_mask;
    int i, j;
   
    if (!info->chan_mask)
        return;
    
    active_mask = info->chan_mask;
    
    for (i = 0; i < NUM_CHANNELS; i++) {
        if (active_mask & 0x01) {
            chan = info->chan[i];
            chan_info = &info->chan_info[chan->channel];
            for (j = 0; j < info->alarm_samples; j++)
                cyg_adc_receive_sample(chan, chan_info->get_sample(chan));
            cyg_adc_wakeup(info->chan[i]);
        }
        active_mask >>= 1;
    }
}

static cyg_adc_sample_t
synth_adc_get_sample_const(cyg_adc_channel *chan)
{
    synth_adc_info *info = chan->device->dev_priv;
    synth_adc_channel_info *chan_info = &info->chan_info[chan->channel];
    
    return chan_info->const_value;
}

static cyg_adc_sample_t
synth_adc_get_sample_random(cyg_adc_channel *chan)
{
    return rand() & SAMPLE_BITS;
}

static cyg_adc_sample_t
synth_adc_get_sample_file(cyg_adc_channel *chan)
{
    synth_adc_info *info = chan->device->dev_priv;
    synth_adc_channel_info *chan_info = &info->chan_info[chan->channel];
    cyg_adc_sample_t sample;
    
    sample = *chan_info->sample++;
    if (chan_info->sample >= chan_info->base + chan_info->num_samples)
        chan_info->sample = chan_info->base;
    
    return sample;
}

//-----------------------------------------------------------------------------
// Simple random number generator

static cyg_uint32 rand(void)
{
    static cyg_uint32 seed;
    
    seed = (seed * 1103515245) + 12345; // permutate seed
    
    return seed;
}
 
