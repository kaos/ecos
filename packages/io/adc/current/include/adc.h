#ifndef CYGONCE_ADC_H
#define CYGONCE_ADC_H
/*==========================================================================
//
//      adc.h
//
//      Generic ADC driver layer header
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
// Author(s):    nickg
// Date:         2008-03-31
// Description:  Implements generic layer of ADC drivers.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/io_adc.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/config_keys.h>
#include <cyg/hal/drv_api.h>


#ifdef CYGPKG_IO_ADC_SELECT_SUPPORT
#include <cyg/fileio/fileio.h>
#endif

//==========================================================================
// Configuration information structure

typedef struct
{
    cyg_uint32          rate;           // Sample rate
} cyg_adc_info_t;

//==========================================================================
// Sample size type.
//
// Define sample size type depending on hardware capability.

#if CYGNUM_IO_ADC_SAMPLE_SIZE > 16
typedef cyg_int32 cyg_adc_sample_t;
#elif CYGNUM_IO_ADC_SAMPLE_SIZE > 8
typedef cyg_int16 cyg_adc_sample_t;
#else
typedef cyg_int8 cyg_adc_sample_t;
#endif

//==========================================================================
// Forward type definitions.

typedef struct cyg_adc_device cyg_adc_device;
typedef struct cyg_adc_channel cyg_adc_channel;
typedef struct cyg_adc_functions cyg_adc_functions;

//==========================================================================
// Callbacks from hardware drivers to generic driver.

__externC void cyg_adc_device_init( cyg_adc_device *device );

__externC void cyg_adc_channel_init(cyg_adc_channel *chan);

__externC cyg_uint32 cyg_adc_receive_sample(cyg_adc_channel *chan, cyg_adc_sample_t sample);

__externC void cyg_adc_wakeup(cyg_adc_channel *chan );

//==========================================================================
// Device table functions

__externC cyg_devio_table_t cyg_io_adc_devio;

//==========================================================================
// ADC device
//
// A single device may support several channels which share interrupt
// vectors and sample rate settings.

struct cyg_adc_device
{
    cyg_adc_functions   *funs;          // Hardware device functions
    void                *dev_priv;      // Hardware device private data
    cyg_adc_info_t      config;         // Current configuration

    cyg_bool            init;           // Initialized ?
    cyg_drv_mutex_t     lock;           // Device lock
};

#define CYG_ADC_DEVICE(__name, __funs, __dev_priv, __rate )     \
cyg_adc_device __name =                                         \
{                                                               \
    .funs               = __funs,                               \
    .dev_priv           = __dev_priv,                           \
    .config.rate        = __rate,                               \
    .init               = false                                 \
};


//==========================================================================
// ADC channel
//
// Each device may support several channels, each providing a separate
// stream of samples.

struct  cyg_adc_channel
{
    int                 channel;        // Channel number
    
    cyg_adc_sample_t    *buf;           // Sample data buffer
    int                 len;            // Buffer length in samples
    volatile int        put;            // Sample insert index
    volatile int        get;            // Sample extract index

    cyg_adc_device      *device;        // Controlling device
    
    cyg_bool            init;           // Initialized ?
    cyg_drv_cond_t      wait;           // Readers wait here for data
    cyg_bool            waiting;        // True if any threads waiting
    cyg_bool            wakeup;         // True if wakeup needed
    
    cyg_bool            enabled;        // Channel enabled?
    cyg_bool            blocking;       // Blocking IO
    int                 overflow;       // Overflow counter
    
#ifdef CYGPKG_IO_ADC_SELECT_SUPPORT    
    struct CYG_SELINFO_TAG   selinfo;   // Select info
#endif

};

#define CYG_ADC_CHANNEL( __name, __channel, __bufsize, __device )       \
static cyg_adc_sample_t __name##_buf[__bufsize];                        \
cyg_adc_channel __name =                                                \
{                                                                       \
    .channel            = __channel,                                    \
    .buf                = __name##_buf,                                 \
    .len                = __bufsize,                                    \
    .put                = 0,                                            \
    .get                = 0,                                            \
    .device             = __device,                                     \
    .init               = false                                         \
};
              
//==========================================================================
// Device functions
//
// These are the functions exported by the hardware device to the
// generic layer.

struct cyg_adc_functions
{
    void (*enable)( cyg_adc_channel *chan );
    void (*disable)( cyg_adc_channel *chan );

    void (*set_rate)( cyg_adc_channel *chan, cyg_uint32 rate );
};

#define CYG_ADC_FUNCTIONS( __name, __enable, __disable, __set_rate )    \
cyg_adc_functions __name =                                              \
{                                                                       \
    .enable             = __enable,                                     \
    .disable            = __disable,                                    \
    .set_rate           = __set_rate                                    \
};

//==========================================================================
#endif // CYGONCE_ADC_H

