/*==========================================================================
//
//      adc.c
//
//      Generic ADC driver layer
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

#include <cyg/io/adc.h>

//==========================================================================
// Diagnostic support
//
// Switch the #if to 1 to generate some diagnostic messages.

#if 0

#include <cyg/infra/diag.h>

#define adc_diag( __fmt, ... ) diag_printf("ADC: %30s[%4d]: " __fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__ );
#define adc_dump_buf( __buf, __size )  diag_dump_buf( __buf, __size )
#else
#define adc_diag( __fmt, ... ) 
#define adc_dump_buf( __buf, __size )
#endif

//==========================================================================
// Main device table entry functions

static Cyg_ErrNo adc_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo adc_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static cyg_bool adc_select(cyg_io_handle_t handle, cyg_uint32 which, CYG_ADDRWORD info);
static Cyg_ErrNo adc_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo adc_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

DEVIO_TABLE(cyg_io_adc_devio,
            adc_write,
            adc_read,
            adc_select,
            adc_get_config,
            adc_set_config
    );

//==========================================================================
// Device interface functions

//--------------------------------------------------------------------------
// Device write
//
// Not supported

static Cyg_ErrNo adc_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len)
{
    adc_diag("write not supported\n");
    return -EDEVNOSUPP;
}

//--------------------------------------------------------------------------
// Device Read
//
// Returns a whole number of samples from a channel

static Cyg_ErrNo adc_read(cyg_io_handle_t handle, void *abuf, cyg_uint32 *len)
{
    Cyg_ErrNo res = ENOERR;
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    cyg_adc_channel *chan = (cyg_adc_channel *)t->priv;
    cyg_adc_device *dev = chan->device;
    
    cyg_adc_sample_t *buf = (cyg_adc_sample_t *)abuf;
    cyg_uint32 size = 0;

    adc_diag("chan %d buf %p len %u\n", chan->channel, abuf, *len);
    
    // Check that supplied buffer address is aligned to sample size.
    if( (((CYG_ADDRESS)buf) & (sizeof(cyg_adc_sample_t)-1)) != 0 )
        return -EIO;

    // Check that channel is enabled
    if( !chan->enabled )
        return -EIO;
    
    cyg_drv_mutex_lock( &dev->lock );
    cyg_drv_dsr_lock();
    {
        while( (*len-size) >= sizeof(cyg_adc_sample_t) )
        {
            if( chan->put != chan->get )
            {
                // A sample is available, transfer it to the buffer.
                
                int next = chan->get+1;
                cyg_drv_isr_lock();
                {
                    *buf++ = chan->buf[chan->get];
                    if( next == chan->len )
                        next = 0;
                    chan->get = next;
                }
                cyg_drv_isr_unlock();
                size += sizeof(cyg_adc_sample_t);

                adc_diag("chan %d sample %d -> %04x\n", chan->channel, size, buf[-1] );
            }
            else
            {
                // If there is no data available, either wait or
                // return EAGAIN.
                
                if( !chan->blocking )
                {
                    // If non-blocking, return what we have. If
                    // nothing, return EAGAIN.
                    *len = size;
                    if( size == 0 )
                        res = -EAGAIN;
                    break;
                }
                // Otherwise, we must wait for data to arrive.

                adc_diag("wait for sample\n");
                chan->waiting = true;
                if( !cyg_drv_cond_wait( &chan->wait ) )
                {
                    // Abort the wait, return EINTR
                    adc_diag("abort\n");
                    *len = size;
                    res = -EINTR;
                    break;
                }

                adc_diag("wake up\n");
                // Loop back round to pick up any samples now
                // available.
            }
        }
    }
    cyg_drv_dsr_unlock();    
    cyg_drv_mutex_unlock( &dev->lock );
    
    return res;
}

//--------------------------------------------------------------------------
// Select support
//
// If select support is enabled, check for pending samples.

static cyg_bool adc_select(cyg_io_handle_t handle, cyg_uint32 which, CYG_ADDRWORD info)
{
#ifdef CYGPKG_IO_ADC_SELECT_SUPPORT
    
    cyg_bool res = false;
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    cyg_adc_channel *chan = (cyg_adc_channel *)t->priv;
    cyg_adc_device *dev = chan->device;

    // Check that channel is enabled. If not, return false.
    if( !chan->enabled )
        return false;
    
    // Only read select is supported
    if( which == CYG_FREAD )
    {
        cyg_drv_mutex_lock( &dev->lock );
        cyg_drv_dsr_lock();
        {
            cyg_drv_isr_lock();            
            if( chan->put == chan->get )
            {
                // There is no data in the buffer, register the select.

                cyg_selrecord( info, &chan->selinfo );
            }
            else
            {
                res = true;
            }
            cyg_drv_isr_unlock();            
        }
        cyg_drv_dsr_unlock();
        cyg_drv_mutex_unlock( &dev->lock );
        
    }

    return res;
    
#else
    
    return true;

#endif
}

//--------------------------------------------------------------------------
// Get config
//
// Return various configuration options.

static Cyg_ErrNo adc_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *xbuf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    cyg_adc_channel *chan = (cyg_adc_channel *)t->priv;
    cyg_adc_info_t *buf = (cyg_adc_info_t *)xbuf;
    Cyg_ErrNo res = ENOERR;

    adc_diag("chan %d\n", chan->channel );
    switch( key )
    {
    case CYG_IO_GET_CONFIG_ADC_RATE:
        *buf = chan->device->config;
        break;

    case CYG_IO_GET_CONFIG_READ_BLOCKING:
        if (*len < sizeof(cyg_uint32))
            return -EINVAL;
        *(cyg_uint32*)xbuf = (chan->blocking) ? 1 : 0;
        break;

        
    default:
        res = -EINVAL;
    }

    return res;
}

//--------------------------------------------------------------------------
// Set config
//
// Set configuration options.

static Cyg_ErrNo adc_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *xbuf, cyg_uint32 *len)
{

    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    cyg_adc_channel *chan = (cyg_adc_channel *)t->priv;
    cyg_adc_info_t *buf = (cyg_adc_info_t *)xbuf;
    Cyg_ErrNo res = ENOERR;

    adc_diag("chan %d\n", chan->channel );
    
    switch( key )
    {
    case CYG_IO_SET_CONFIG_ADC_RATE:
        chan->device->config = *buf;
        chan->device->funs->set_rate( chan, buf->rate );
        break;

    case CYG_IO_SET_CONFIG_ADC_ENABLE:
        chan->enabled = true;
        chan->device->funs->enable( chan );
        break;        

    case CYG_IO_SET_CONFIG_ADC_DISABLE:
        chan->enabled = false;        
        chan->device->funs->disable( chan );
        break;        

    case CYG_IO_SET_CONFIG_ADC_DATA_FLUSH:
        cyg_drv_isr_lock();
        chan->put = 0;
        chan->get = 0;
        cyg_drv_isr_unlock();
        break;      

    case CYG_IO_SET_CONFIG_READ_BLOCKING:
        if (*len < sizeof(cyg_uint32))
            return -EINVAL;

        chan->blocking = (1 == *(cyg_uint32*)xbuf) ? true : false;
        break;
        
    default:
        res = -EINVAL;
    }

    return res;
}

//==========================================================================
// Callbacks from hardware driver to generic layer

//--------------------------------------------------------------------------
// Initialize generic device structure

__externC void cyg_adc_device_init( cyg_adc_device *device )
{
    if( device->init )
        return;

    adc_diag("\n");
    
    device->init = true;
    cyg_drv_mutex_init( &device->lock );
}

//--------------------------------------------------------------------------
// Initialize generic channel structure

__externC void cyg_adc_channel_init(cyg_adc_channel *chan)
{
    if( chan->init )
        return;

    adc_diag("chan %d\n", chan->channel );
    
    chan->init = true;
    cyg_drv_cond_init( &chan->wait, &chan->device->lock );
    chan->enabled = false;
    chan->waiting = false;
    chan->wakeup = false;
    chan->blocking = true;
    chan->overflow = 0;
    
#ifdef CYGPKG_IO_ADC_SELECT_SUPPORT
        cyg_selinit( &chan->selinfo );
#endif        
    
}

//--------------------------------------------------------------------------
// Add a new sample to the buffer
//
// This function is called from the ISR to store a sample in the
// buffer.

__externC cyg_uint32 cyg_adc_receive_sample(cyg_adc_channel *chan, cyg_adc_sample_t sample)
{
    cyg_uint32 res = 0;
    int next = chan->put+1;

    adc_diag("chan %d sample %04x\n", chan->channel, sample );

    // Ignore this sample if the channel is not enabled.
    if( !chan->enabled )
        return 0;
    
    if( next == chan->len )
        next = 0;

    if( next == chan->get )
    {
        // The buffer is full. Record an overflow and arrange to
        // replace the oldest sample with the new one.
        
        adc_diag("chan %d overflow\n", chan->channel);
        chan->overflow++;

        chan->get++;
        if( chan->get == chan->len )
            chan->get = 0;
    }

    // If this is the first sample into the buffer, wake any waiting
    // selectors. Also do this if there are any threads waiting for
    // data.
    if( chan->put == chan->get || chan->waiting )
        res |= CYG_ISR_CALL_DSR, chan->wakeup = true;

    // Store the new sample
    chan->buf[chan->put] = sample;
    chan->put = next;

    return res;
}

//--------------------------------------------------------------------------
// Wakeup a channel
//
// This function is called from the DSR if the channel wakeup field is
// set. It satisfies any select operations or any waiting readers.

__externC void cyg_adc_wakeup(cyg_adc_channel *chan )
{
    adc_diag("chan %d\n", chan->channel );
    
#ifdef CYGPKG_IO_ADC_SELECT_SUPPORT
    cyg_selwakeup( &chan->selinfo );
#endif

    chan->waiting = false;
    cyg_drv_cond_signal( &chan->wait );

    chan->wakeup = false;    
}

//==========================================================================
// End of adc.c
