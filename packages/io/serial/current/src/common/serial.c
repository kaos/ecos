//==========================================================================
//
//      io/serial/common/serial.c
//
//      High level serial driver
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   gthomas
// Contributors:  gthomas
// Date:        1999-02-04
// Purpose:     Top level serial driver
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>

static Cyg_ErrNo serial_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len);
static Cyg_ErrNo serial_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
static Cyg_ErrNo serial_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo serial_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len);

DEVIO_TABLE(cyg_io_serial_devio,
            serial_write,
            serial_read,
            serial_get_config,
            serial_set_config
    );

static void serial_init(serial_channel *chan);
static void serial_xmt_char(serial_channel *chan);
static void serial_rcv_char(serial_channel *chan, unsigned char c);
SERIAL_CALLBACKS(cyg_io_serial_callbacks, 
                 serial_init, 
                 serial_xmt_char, 
                 serial_rcv_char);

static void
serial_init(serial_channel *chan)
{
    if (chan->init) return;
    if (chan->out_cbuf.len != 0) {
#ifdef CYGDBG_IO_INIT
        diag_printf("Set output buffer - buf: %x len: %d\n", chan->out_cbuf.data, chan->out_cbuf.len);
#endif
        chan->out_cbuf.waiting = false;
        chan->out_cbuf.abort = false;
        chan->out_cbuf.pending = 0;
        cyg_drv_mutex_init(&chan->out_cbuf.lock);
        cyg_drv_cond_init(&chan->out_cbuf.wait, &chan->out_cbuf.lock);
        chan->out_cbuf.low_water = chan->out_cbuf.len / 4;
    }
    if (chan->in_cbuf.len != 0) {
#ifdef CYGDBG_IO_INIT
        diag_printf("Set input buffer - buf: %x len: %d\n", chan->in_cbuf.data, chan->in_cbuf.len);
#endif
        chan->in_cbuf.waiting = false;
        chan->in_cbuf.abort = false;
        cyg_drv_mutex_init(&chan->in_cbuf.lock);
        cyg_drv_cond_init(&chan->in_cbuf.wait, &chan->in_cbuf.lock);
    }
    chan->init = true;
}

static Cyg_ErrNo 
serial_write(cyg_io_handle_t handle, const void *_buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    serial_channel *chan = (serial_channel *)t->priv;
    serial_funs *funs = chan->funs;
    cyg_int32 size = *len;
    cyg_uint8 *buf = (cyg_uint8 *)_buf;
    int next;
    cbuf_t *cbuf = &chan->out_cbuf;
    Cyg_ErrNo res = ENOERR;

    cbuf->abort = false;
    cyg_drv_mutex_lock(&cbuf->lock);
    if (cbuf->len == 0) {
        // Non interrupt driven (i.e. polled) operation
        while (size-- > 0) {
            while ((funs->putc)(chan, *buf) == false) ;  // Ignore full, keep trying
            buf++;
        }
    } else {
        cyg_drv_dsr_lock();  // Avoid race condition testing pointers
        while (size > 0) {       
            next = cbuf->put + 1;
            if (next == cbuf->len) next = 0;
            if (next == cbuf->get) {                
                cbuf->waiting = true;
                // Buffer full - wait for space
                (funs->start_xmit)(chan);  // Make sure xmit is running
                if (cbuf->waiting) {
                    // Note: 'start_xmit' may have obviated the need to wait :-)
                    cbuf->pending += size;  // Have this much more to send [eventually]
                    cyg_drv_cond_wait(&cbuf->wait);
                    cbuf->pending -= size;
                }
                if (cbuf->abort) {
                    // Give up!
                    cbuf->abort = false;
                    cbuf->waiting = false;
                    res = -EINTR;
                    break;
                }
            } else {
                cbuf->data[cbuf->put++] = *buf++;
                cbuf->put = next;
                size--;  // Only count if actually sent!
            }
        }
        cyg_drv_dsr_unlock();
        (funs->start_xmit)(chan);  // Start output as necessary
    }
    cyg_drv_mutex_unlock(&cbuf->lock);
    return res;
}

static Cyg_ErrNo 
serial_read(cyg_io_handle_t handle, void *_buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    serial_channel *chan = (serial_channel *)t->priv;
    serial_funs *funs = chan->funs;
    cyg_uint8 *buf = (cyg_uint8 *)_buf;
    cyg_int32 size = 0;
    cbuf_t *cbuf = &chan->in_cbuf;
    Cyg_ErrNo res = ENOERR;
#ifdef XX_CYGDBG_DIAG_BUF
            extern int enable_diag_uart;
            int _enable = enable_diag_uart;
            int _time, _stime;
            externC cyg_tick_count_t cyg_current_time(void);
#endif // CYGDBG_DIAG_BUF

    cbuf->abort = false;
    cyg_drv_mutex_lock(&cbuf->lock);
    if (cbuf->len == 0) {
        // Non interrupt driven (i.e. polled) operation
        while (size++ < *len) {
            *buf++ = (funs->getc)(chan);
        }
    } else {
        cyg_drv_dsr_lock();  // Avoid races
        while (size < *len) {
            if (cbuf->get != cbuf->put) {
                *buf++ = cbuf->data[cbuf->get];
                if (++cbuf->get == cbuf->len) cbuf->get = 0;
                size++;
            } else {
                cbuf->waiting = true;
#ifdef XX_CYGDBG_DIAG_BUF
            enable_diag_uart = 0;
            HAL_CLOCK_READ(&_time);
            _stime = (int)cyg_current_time();
            diag_printf("READ wait - get: %d, put: %d, time: %x.%x\n", cbuf->get, cbuf->put, _stime, _time);
            enable_diag_uart = _enable;
#endif // CYGDBG_DIAG_BUF
                cyg_drv_cond_wait(&cbuf->wait);
#ifdef XX_CYGDBG_DIAG_BUF
            enable_diag_uart = 0;
            HAL_CLOCK_READ(&_time);
            _stime = (int)cyg_current_time();
            diag_printf("READ continue - get: %d, put: %d, time: %x.%x\n", cbuf->get, cbuf->put, _stime, _time);
            enable_diag_uart = _enable;
#endif // CYGDBG_DIAG_BUF
                if (cbuf->abort) {
                    // Give up!
                    cbuf->abort = false;
                    cbuf->waiting = false;
                    res = -EINTR;
                    break;
                }
            }
        }
        cyg_drv_dsr_unlock();
    }
    cyg_drv_isr_lock();
#ifdef XX_CYGDBG_DIAG_BUF
            enable_diag_uart = 0;
            HAL_CLOCK_READ(&_time);
            _stime = (int)cyg_current_time();
            diag_printf("READ done - size: %d, len: %d, time: %x.%x\n", size, *len, _stime, _time);
            enable_diag_uart = _enable;
#endif // CYGDBG_DIAG_BUF
    cyg_drv_isr_unlock();
    cyg_drv_mutex_unlock(&cbuf->lock);
    return res;
}

static Cyg_ErrNo 
serial_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *xbuf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    serial_channel *chan = (serial_channel *)t->priv;
    cyg_serial_info_t *buf = (cyg_serial_info_t *)xbuf;
    Cyg_ErrNo res = ENOERR;
    cbuf_t *cbuf = &chan->out_cbuf;
    serial_funs *funs = chan->funs;

    switch (key) {
    case CYG_IO_GET_CONFIG_SERIAL_INFO:
        if (*len < sizeof(cyg_serial_info_t)) {
            return -EINVAL;
        }
        *buf = chan->config;
        *len = sizeof(chan->config);
        break;       
    case CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN:
// Wait for any pending output to complete
        if (cbuf->len == 0) break;  // Nothing to do if not buffered
        cyg_drv_mutex_lock(&cbuf->lock);  // Stop any further output processing
        cyg_drv_dsr_lock();
        while (cbuf->pending || (cbuf->get != cbuf->put)) {
            cbuf->waiting = true;
            cyg_drv_cond_wait(&cbuf->wait);
        }
        cyg_drv_dsr_unlock();
        cyg_drv_mutex_unlock(&cbuf->lock);
        break;
    case CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH:
        // Flush any buffered input
        cbuf = &chan->in_cbuf;
        if (cbuf->len == 0) break;  // Nothing to do if not buffered
        cyg_drv_mutex_lock(&cbuf->lock);  // Stop any further input processing
        cyg_drv_dsr_lock();
        if (cbuf->waiting) {
            cbuf->abort = true;
            cyg_drv_cond_signal(&cbuf->wait);
            cbuf->waiting = false;
        }
        cbuf->get = cbuf->put;  // Flush buffered input
        cyg_drv_dsr_unlock();
        cyg_drv_mutex_unlock(&cbuf->lock);
        break;
    case CYG_IO_GET_CONFIG_SERIAL_ABORT:
        // Abort any outstanding I/O, including blocked reads
        // Caution - assumed to be called from 'timeout' (i.e. DSR) code
        cbuf = &chan->in_cbuf;
        if (cbuf->len != 0) {
            cbuf->abort = true;
            cyg_drv_cond_signal(&cbuf->wait);
        }
        cbuf = &chan->out_cbuf;
        if (cbuf->len != 0) {
            cbuf->abort = true;
            cyg_drv_cond_signal(&cbuf->wait);
        }
        break;
    case CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH:
// Throw away any pending output
        if (cbuf->len == 0) break;  // Nothing to do if not buffered
        cyg_drv_mutex_lock(&cbuf->lock);  // Stop any further output processing
        cyg_drv_dsr_lock();
        if (cbuf->get != cbuf->put) {
            cbuf->get = cbuf->put;  // Empties queue!
            (funs->stop_xmit)(chan);  // Done with transmit
        }
        if (cbuf->waiting) {
            cbuf->abort = true;
            cyg_drv_cond_signal(&cbuf->wait);
            cbuf->waiting = false;
        }
        cyg_drv_dsr_unlock();
        cyg_drv_mutex_unlock(&cbuf->lock);
        break;
    default:
        res = -EINVAL;
    }
    return res;
}

static Cyg_ErrNo 
serial_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *xbuf, cyg_uint32 *len)
{
    Cyg_ErrNo res = ENOERR;
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    serial_channel *chan = (serial_channel *)t->priv;
    serial_funs *funs = chan->funs;
    cyg_serial_info_t *buf = (cyg_serial_info_t *)xbuf;

    switch (key) {
    case CYG_IO_SET_CONFIG_SERIAL_INFO:
        if (*len != sizeof(cyg_serial_info_t)) {
            return -EINVAL;
        }
        if ((funs->set_config)(chan, buf) != true) {
            // Device is not happy with paramters or they cannot be set
            res = -EINVAL;
        }
        break;
    default:
        res = -EINVAL;
    }
    return res;
}

static void
serial_xmt_char(serial_channel *chan)
{
    cbuf_t *cbuf = &chan->out_cbuf;
    serial_funs *funs = chan->funs;
    unsigned char c;
    int space;

    while (cbuf->get != cbuf->put) {
        c = cbuf->data[cbuf->get];
        if ((funs->putc)(chan, c)) {
            cbuf->get++;
            if (cbuf->get == cbuf->len) cbuf->get = 0;
            if (cbuf->waiting) {
                // See if there is now enough room to restart writer
                space = (cbuf->len + cbuf->get) - cbuf->put;
                if (space > cbuf->len) space -= cbuf->len;
                if (space >= cbuf->low_water) {
                    cbuf->waiting = false;
                    cyg_drv_cond_broadcast(&cbuf->wait);
                }
            }
        } else {
            return;  // Need to wait for more space
        }
    }
    (funs->stop_xmit)(chan);  // Done with transmit
    if (cbuf->waiting) {
        cbuf->waiting = false;
        cyg_drv_cond_signal(&cbuf->wait);
    }
}

static void
serial_rcv_char(serial_channel *chan, unsigned char c)
{
    cbuf_t *cbuf = &chan->in_cbuf;

    cbuf->data[cbuf->put++] = c;
    if (cbuf->put == cbuf->len) cbuf->put = 0;
    if (cbuf->waiting) {
#ifdef XX_CYGDBG_DIAG_BUF
            extern int enable_diag_uart;
            int _enable = enable_diag_uart;
            int _time, _stime;
            externC cyg_tick_count_t cyg_current_time(void);
            enable_diag_uart = 0;
            HAL_CLOCK_READ(&_time);
            _stime = (int)cyg_current_time();
            diag_printf("Signal reader - time: %x.%x\n", _stime, _time);
            enable_diag_uart = _enable;
#endif // CYGDBG_DIAG_BUF
        cbuf->waiting = false;
        cyg_drv_cond_signal(&cbuf->wait);
    }
}

