//==========================================================================
//
//      io/can/common/can.c
//
//      High level CAN driver
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2005-05-12
// Purpose:      Top level CAN driver
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                              INCLUDES
//==========================================================================
#include <pkgconf/io.h>
#include <pkgconf/io_can.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/can.h>
#include <cyg/infra/cyg_ass.h>      // assertion support
#include <cyg/infra/diag.h>         // diagnostic output


//==========================================================================
//                                MACROS
//==========================================================================
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMEOUTS
#define CYG_DRV_COND_WAIT(_cond, _time) cyg_cond_timed_wait(_cond, cyg_current_time() + (_time))
#else
#define CYG_DRV_COND_WAIT(_cond, _time) cyg_drv_cond_wait(_cond)
#endif


//==========================================================================
//                            LOCAL FUNCTIONS
//==========================================================================
//
// Device I/O functions
//
static Cyg_ErrNo can_write(cyg_io_handle_t handle, const void *_buf, cyg_uint32 *len);
static Cyg_ErrNo can_read(cyg_io_handle_t handle, void *_buf, cyg_uint32 *len);
static cyg_bool  can_select(cyg_io_handle_t handle, cyg_uint32 which, CYG_ADDRWORD info);
static Cyg_ErrNo can_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *xbuf, cyg_uint32 *len);
static Cyg_ErrNo can_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *xbuf, cyg_uint32 *len);

//
// Callback functions into upper layer driver
//
static void can_init(can_channel *chan);    
static void can_rcv_event(can_channel *chan, void *pdata);
static void can_xmt_msg(can_channel *chan, void *pdata);

//
// Device I/O table
//
DEVIO_TABLE(cyg_io_can_devio,
            can_write,
            can_read,
            can_select,
            can_get_config,
            can_set_config
    );


//
// Callbacks into upper layer driver
//
CAN_CALLBACKS(cyg_io_can_callbacks, 
              can_init, 
              can_xmt_msg,
              can_rcv_event);


//===========================================================================
// Initialize CAN driver
//===========================================================================
static void can_init(can_channel *chan)
{
    can_cbuf_t *cbuf;
    
    if (chan->init) 
    {
        return;
    }
    
    cbuf = &chan->in_cbuf;
    cbuf->waiting  = false;
    cbuf->abort    = false;
#ifdef CYGOPT_IO_CAN_SUPPORT_NONBLOCKING
    cbuf->blocking = true;
#endif
    cyg_drv_mutex_init(&cbuf->lock);
    cyg_drv_cond_init(&cbuf->wait, &cbuf->lock);
    
    cbuf = &chan->out_cbuf;
    cbuf->waiting  = false;
    cbuf->abort    = false;
#ifdef CYGOPT_IO_CAN_SUPPORT_NONBLOCKING
    cbuf->blocking = true;
#endif
    cyg_drv_mutex_init(&cbuf->lock);
    cyg_drv_cond_init(&cbuf->wait, &cbuf->lock);
    
    chan->init = true;
}


//===========================================================================
// Write exactly one CAN message to CAN bus
//===========================================================================
static Cyg_ErrNo can_write(cyg_io_handle_t handle, const void *_buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t      = (cyg_devtab_entry_t *)handle;
    can_channel        *chan   = (can_channel *)t->priv;
    can_lowlevel_funs  *funs   = chan->funs;
    Cyg_ErrNo           res    = ENOERR;
    can_cbuf_t         *cbuf   = &chan->out_cbuf;
    cyg_uint32          size   = *len;
    
    //
    // the user need to provide a can message buffer
    //
    if (*len != sizeof(cyg_can_message))
    {
        return -EINVAL;
    }
    
    cyg_drv_mutex_lock(&cbuf->lock);
    cbuf->abort = false;
    cyg_drv_dsr_lock(); // avoid race condition while testing pointers
    
    while (size > 0)
    {
        if (cbuf->data_cnt == cbuf->len) 
        {
            cbuf->waiting = true;      // Buffer full - wait for space
            funs->start_xmit(chan);    // Make sure xmit is running
            //
            // Check flag: 'start_xmit' may have obviated the need
            // to wait
            //
            if (cbuf->waiting) 
            {
                cbuf->pending += size;  // Have this much more to send [eventually]
#if defined(CYGOPT_IO_CAN_SUPPORT_NONBLOCKING)
#if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)
                //
                // If timeouts are enabled and we use nonblocking calls then we
                // can use the timeout values
                //
                if (!cbuf->blocking)
                {
                    if(!CYG_DRV_COND_WAIT(&cbuf->wait, cbuf->timeout))
                    {
                        cbuf->abort = true;
                    }
                } // if (!cbuf->blocking)#
                else
#else // #if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)
                //
                // if this is a nonblocking call then we return immediatelly
                //
                if (!cbuf->blocking)
                {
                    *len = 0;
                    res = -EAGAIN;
                    break;
                }
                else
#endif // #if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)
#endif //defined(CYGOPT_IO_CAN_SUPPORT_NONBLOCKING)
                {
                    if(!cyg_drv_cond_wait(&cbuf->wait))
                    {
                        cbuf->abort = true;
                    }
                }
                cbuf->pending -= size;
                if (cbuf->abort) 
                {
                    // Give up!
                    *len -= size;   // number of characters actually sent
                     cbuf->abort = false;
                     cbuf->waiting = false;
                     res = -EINTR;
                     break;
                } // if (cbuf->abort) 
            } // if (cbuf->waiting)
        } // if (cbuf->data_cnt == cbuf->len) 
        else
        {           
            //
            // there is enougth space left so we can store additional data
            //
            cyg_can_message *ptxbuf       = (cyg_can_message *)cbuf->pdata;
            cyg_can_message *pbuf_message = &ptxbuf[cbuf->put];
            cyg_can_message *pmessage     = (cyg_can_message *)_buf;
            
            *pbuf_message = *pmessage; // copy message
   
            cbuf->put = (cbuf->put + 1) % cbuf->len;
            cbuf->data_cnt++;   
            size -= sizeof(cyg_can_message);   
        }
    } // while (size > 0)
    
    (funs->start_xmit)(chan);  // Start output as necessary
    cyg_drv_dsr_unlock();            
    cyg_drv_mutex_unlock(&cbuf->lock);
    return res;
}


//===========================================================================
// Read one single CAN event from hw
//===========================================================================
static Cyg_ErrNo can_read(cyg_io_handle_t handle, void *_buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t      = (cyg_devtab_entry_t *)handle;
    can_channel        *chan   = (can_channel *)t->priv;
    can_cbuf_t         *cbuf   = &chan->in_cbuf;
    cyg_uint32          size   = 0;
    Cyg_ErrNo           res    = ENOERR;
    
    //
    // the user need to provide a can event buffer
    //
    if (*len != sizeof(cyg_can_event))
    {
        return -EINVAL;
    }
    
    cyg_drv_mutex_lock(&cbuf->lock);
    cbuf->abort = false;
    
    cyg_drv_dsr_lock();  // avoid race conditions
    while (size < *len)
    {
        //
        // if message buffer contains at least one message then read the
        // oldest message from buffer and return
        //
        if (cbuf->data_cnt > 0)
        {
            cyg_can_event *prxbuf     = (cyg_can_event *)cbuf->pdata;   
            cyg_can_event *pbuf_event = &prxbuf[cbuf->get];
            cyg_can_event *pevent     = (cyg_can_event *)_buf;

            *pevent = *pbuf_event; // copy event
            
            cbuf->get = (cbuf->get + 1) % cbuf->len;
            cbuf->data_cnt--; 
            size += sizeof(cyg_can_event);
        }
        else
        {
            //
            // if messaeg buffer does not contain any message, then wait until
            // a message arrives or return immediatelly if nonblocking calls are
            // supported
            //
            cbuf->waiting = true;
#if defined(CYGOPT_IO_CAN_SUPPORT_NONBLOCKING)
#if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)
            //
            // If timeouts are enabled and we use nonblocking calls then we
            // can use the timeout values
            //
            if (!cbuf->blocking)
            {
                if(!CYG_DRV_COND_WAIT(&cbuf->wait, cbuf->timeout))
                {
                    cbuf->abort = true;
                }
            } // if (!cbuf->blocking)#
            else
#else // #if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)
            //
            // if this is a nonblocking call then we return immediatelly
            //
            if (!cbuf->blocking)
            {
                *len = 0;
                res = -EAGAIN;
                break;
            }
            else
#endif // #if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)
#endif // #if defined(CYGOPT_IO_CAN_SUPPORT_NONBLOCKING)
            {
                if(!cyg_drv_cond_wait(&cbuf->wait))
                {
                    cbuf->abort = true;
                }
            }
            
            if (cbuf->abort)
            {
                *len = size;
                cbuf->abort = false;
                cbuf->waiting = false;
                res = -EINTR;
                break;
            }
        }
    } // while (size < *len)
    cyg_drv_dsr_unlock();
    
    cyg_drv_mutex_unlock(&cbuf->lock);
    
    return res;
}


//===========================================================================
// Query CAN channel configuration data
//===========================================================================
static Cyg_ErrNo can_get_config(cyg_io_handle_t handle, 
               cyg_uint32      key, 
               void           *xbuf,
               cyg_uint32     *len)
{
    cyg_devtab_entry_t *t         = (cyg_devtab_entry_t *)handle;
    can_channel        *chan      = (can_channel *)t->priv;
    Cyg_ErrNo           res       = ENOERR;
    cyg_can_info_t     *pcan_info = (cyg_can_info_t *)xbuf;
    can_cbuf_t         *out_cbuf  = &chan->out_cbuf;
    can_cbuf_t         *in_cbuf   = &chan->in_cbuf;
    can_lowlevel_funs  *funs      = chan->funs;
    
    switch (key)
    {
        //
        // query about CAN configuration like baud rate
        //
        case CYG_IO_GET_CONFIG_CAN_INFO :
             if (*len < sizeof(cyg_can_info_t)) 
             {
                 return -EINVAL;
             }
             *pcan_info = chan->config;
             *len       = sizeof(chan->config);
             break;
        //
        // return rx/tx buffer sizes and counts 
        //    
        case CYG_IO_GET_CONFIG_CAN_BUFFER_INFO :
             {
                 cyg_can_buf_info_t *pbuf_info;
                 if (*len < sizeof(cyg_can_buf_info_t))
                 {
                     return -EINVAL; 
                 } 
          
                *len = sizeof(cyg_can_buf_info_t);
                pbuf_info = (cyg_can_buf_info_t *)xbuf;
                pbuf_info->rx_bufsize = in_cbuf->len;
                if (pbuf_info->rx_bufsize)
                {
                    pbuf_info->rx_count = in_cbuf->data_cnt ;
                }
                else
                {
                    pbuf_info->rx_count = 0;
                }            
                pbuf_info->tx_bufsize = out_cbuf->len;
                if (pbuf_info->tx_bufsize)
                {
                    pbuf_info->tx_count = out_cbuf->data_cnt;
                }
                else
                {
                    pbuf_info->tx_count = 0;
                }
            }
            break; // case CYG_IO_GET_CONFIG_CAN_BUFFER_INFO

#ifdef CYGOPT_IO_CAN_SUPPORT_TIMEOUTS            
            //
            // return current timeouts
            //
            case CYG_IO_GET_CONFIG_CAN_TIMEOUT :
                 {
                     cyg_can_timeout_info_t *ptimeout_info;
                     if (*len < sizeof(cyg_can_timeout_info_t))
                     {
                         return -EINVAL; 
                     } 
                     
                     *len = sizeof(cyg_can_timeout_info_t);
                      ptimeout_info = (cyg_can_timeout_info_t *)xbuf;
                      
                      ptimeout_info->rx_timeout = in_cbuf->timeout;
                      ptimeout_info->tx_timeout = out_cbuf->timeout;
                 }
                 break; // case CYG_IO_GET_CONFIG_CAN_TIMEOUT_INFO
#endif // CYGOPT_IO_CAN_SUPPORT_TIMEOUTS

#ifdef CYGOPT_IO_CAN_SUPPORT_NONBLOCKING
            //
            // check if blocking calls are enabled
            //
            case CYG_IO_GET_CONFIG_READ_BLOCKING:
                 {
                     if (*len < sizeof(cyg_uint32)) 
                     {
                         return -EINVAL;
                     }
                     *(cyg_uint32*)xbuf = (in_cbuf->blocking) ? 1 : 0;
                 }
                 break;

            //
            // check if nonblocking calls are enabled
            // 
            case CYG_IO_GET_CONFIG_WRITE_BLOCKING:
                 {
                     if (*len < sizeof(cyg_uint32))
                     {
                         return -EINVAL;
                     }
                     *(cyg_uint32*)xbuf = (out_cbuf->blocking) ? 1 : 0;
                 }
                 break;
#endif // CYGOPT_IO_CAN_SUPPORT_NONBLOCKING
        
        //
        // return hardware description interface
        //
        case CYG_IO_GET_CONFIG_CAN_HDI :
             {
                 cyg_can_hdi *hdi = (cyg_can_hdi *)xbuf;
                 if (*len != sizeof(cyg_can_hdi)) 
                 {
                     return -EINVAL;
                 }
                 hdi = hdi; // avoid compiler warnings
                 *len = sizeof(cyg_can_hdi);  
                 //
                 // pass down to low level to gather more information about
                 // CAN hardware
                 //
                 res = (funs->get_config)(chan, key, xbuf, len); 
             }
             break;
        
        default:
            res = (funs->get_config)(chan, key, xbuf, len);
    } // switch (key)
    
    return res;
}


//===========================================================================
// Set CAN channel configuration
//===========================================================================
static Cyg_ErrNo can_set_config(cyg_io_handle_t handle, 
               cyg_uint32      key,
               const void     *xbuf, 
               cyg_uint32     *len)
{
    cyg_devtab_entry_t *t         = (cyg_devtab_entry_t *)handle;
    can_channel        *chan      = (can_channel *)t->priv;
    Cyg_ErrNo           res       = ENOERR;
    can_lowlevel_funs  *funs      = chan->funs;
    can_cbuf_t         *out_cbuf  = &chan->out_cbuf;
    can_cbuf_t         *in_cbuf   = &chan->in_cbuf;
    
    switch (key)
    {
#ifdef CYGOPT_IO_CAN_SUPPORT_NONBLOCKING
        //
        // Set calls to read function to blocking / nonblocking mode
        //
        case CYG_IO_SET_CONFIG_READ_BLOCKING:
             {
                 if (*len < sizeof(cyg_uint32) || 0 == in_cbuf->len) 
                 {
                     return -EINVAL;
                 }
                 in_cbuf->blocking = (1 == *(cyg_uint32*)xbuf) ? true : false;
             }
             break;
        
        //
        // set calls to write functions to blocking / nonblocking mode
        //     
        case CYG_IO_SET_CONFIG_WRITE_BLOCKING:
             {
                 if (*len < sizeof(cyg_uint32) || 0 == out_cbuf->len) 
                 {
                     return -EINVAL;
                 }
                 out_cbuf->blocking = (1 == *(cyg_uint32*)xbuf) ? true : false;
             }
             break;
#endif // CYGOPT_IO_CAN_SUPPORT_NONBLOCKING

#ifdef CYGOPT_IO_CAN_SUPPORT_TIMEOUTS            
        //
        // return current timeouts
        //
        case CYG_IO_SET_CONFIG_CAN_TIMEOUT :
             {
                 cyg_can_timeout_info_t *ptimeout_info;
                 if (*len < sizeof(cyg_can_timeout_info_t))
                 {
                     return -EINVAL; 
                 } 
                   
                 *len = sizeof(cyg_can_timeout_info_t);
                  ptimeout_info = (cyg_can_timeout_info_t *)xbuf;
                   
                  in_cbuf->timeout = ptimeout_info->rx_timeout;
                  out_cbuf->timeout = ptimeout_info->tx_timeout;
             }
             break; // case CYG_IO_GET_CONFIG_CAN_TIMEOUT_INFO
#endif // CYGOPT_IO_CAN_SUPPORT_TIMEOUTS

        case CYG_IO_SET_CONFIG_CAN_INPUT_FLUSH:
             {
                 //
                 // Flush any buffered input
                 //
                 if (in_cbuf->len == 0)
                 {
                     break;  // Nothing to do if not buffered
                 }
                 cyg_drv_mutex_lock(&in_cbuf->lock);  // Stop any further input processing
                 cyg_drv_dsr_lock();
                 if (in_cbuf->waiting) 
                 {
                     in_cbuf->abort = true;
                     cyg_drv_cond_broadcast(&in_cbuf->wait);
                     in_cbuf->waiting = false;
                 }
                 in_cbuf->get = in_cbuf->put = in_cbuf->data_cnt = 0; // Flush buffered input
                 
                 //
                 // Pass to the hardware driver in case it wants to flush FIFOs etc.
                 //
                 (funs->set_config)(chan,
                                    CYG_IO_SET_CONFIG_CAN_INPUT_FLUSH,
                                    NULL, NULL);
                 cyg_drv_dsr_unlock();
                 cyg_drv_mutex_unlock(&in_cbuf->lock);
            } // CYG_IO_SET_CONFIG_CAN_INPUT_FLUSH:
        
        //
        // flush any buffered output
        //    
        case CYG_IO_SET_CONFIG_CAN_OUTPUT_FLUSH:
             {
                  // Throw away any pending output
                  if (out_cbuf->len == 0) 
                  {
                      break;  // Nothing to do if not buffered
                  }
                  cyg_drv_mutex_lock(&out_cbuf->lock);  // Stop any further output processing
                  cyg_drv_dsr_lock();
                  if (out_cbuf->data_cnt > 0) 
                  {
                      out_cbuf->get = out_cbuf->put = out_cbuf->data_cnt = 0;  // Empties queue!
                     (funs->stop_xmit)(chan);  // Done with transmit
                  }
                  
                  //
                  // Pass to the hardware driver in case it wants to flush FIFOs etc.
                  //
                  (funs->set_config)(chan,
                                     CYG_IO_SET_CONFIG_CAN_OUTPUT_FLUSH,
                                     NULL, NULL);
                  if (out_cbuf->waiting) 
                  {
                      out_cbuf->abort = true;
                      cyg_drv_cond_broadcast(&out_cbuf->wait);
                      out_cbuf->waiting = false;
                  }// if (out_cbuf->waiting) 
                  cyg_drv_dsr_unlock();
                  cyg_drv_mutex_unlock(&out_cbuf->lock);
              }
              break; // CYG_IO_GET_CONFIG_CAN_OUTPUT_FLUSH:
        
        //
        // wait until all messages in outbut buffer are sent
        //      
        case CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN:
             {
                 // Wait for any pending output to complete
                 if (out_cbuf->len == 0) 
                 {
                     break;  // Nothing to do if not buffered
                 }
                 cyg_drv_mutex_lock(&out_cbuf->lock);  // Stop any further output processing
                 cyg_drv_dsr_lock();
                 while (out_cbuf->pending || (out_cbuf->data_cnt > 0)) 
                 {
                     out_cbuf->waiting = true;
                     if(!cyg_drv_cond_wait(&out_cbuf->wait))
                     {
                         res = -EINTR;
                     }
                 }
                 cyg_drv_dsr_unlock();
                 cyg_drv_mutex_unlock(&out_cbuf->lock);
             }
             break;// CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN:
             
        //     
        // Abort any outstanding I/O, including blocked reads
        // Caution - assumed to be called from 'timeout' (i.e. DSR) code  
        //   
        case CYG_IO_SET_CONFIG_CAN_ABORT :
             {
                 in_cbuf->abort = true;
                 cyg_drv_cond_broadcast(&in_cbuf->wait);
                 
                 out_cbuf->abort = true;
                 cyg_drv_cond_broadcast(&out_cbuf->wait);
             }
             break;
        
        default:
            //
            // pass down to lower layers
            //
            res = (funs->set_config)(chan, key, xbuf, len);
    } // switch (key)
    
    return res;
}


//===========================================================================
// Select support for CAN channel
//===========================================================================
static cyg_bool can_select(cyg_io_handle_t handle, cyg_uint32 which, CYG_ADDRWORD info)
{
    //
    // do nothing here because we currently do not support select
    //
    return true;
}


//===========================================================================
// Callback for received events
//===========================================================================
static void can_rcv_event(can_channel *chan, void *pdata)
{
    can_cbuf_t     *cbuf   = &chan->in_cbuf;
    cyg_can_event  *prxbuf = (cyg_can_event *)cbuf->pdata;
    
    //
    // cbuf is a ring buffer - if the buffer is full, then we overwrite the
    // oldest message in buffer so the user will always get the actual and
    // last state of the external hardware that is connected to the
    // CAN bus. We need to call cyg_drv_dsr_lock() here because this function
    // may be called from different message box interrupts and so we have to
    // protect data access here
    //
    cyg_drv_dsr_lock();
    prxbuf[cbuf->put].flags = 0; // clear flags because it is a new event
    if (chan->funs->getevent(chan, &prxbuf[cbuf->put], pdata))
    {
        if (cbuf->data_cnt < cbuf->len)
        {
            cbuf->data_cnt++;
        }
        else
        {
            //
            // the buffer is full but a new message arrived. We store this new
            // message and overwrite the oldest one, but at least we tell the user
            // that there is an overrun in RX queue
            //
            prxbuf[cbuf->put].flags |= CYGNUM_CAN_EVENT_OVERRUN_RX;
            cbuf->get = (cbuf->get + 1) % cbuf->len;
        }
        
        cbuf->put = (cbuf->put + 1) % cbuf->len;
        
        if (cbuf->waiting) 
        {
            cbuf->waiting = false;
            cyg_drv_cond_broadcast(&cbuf->wait);
        }
    }
    
    cyg_drv_dsr_unlock();
}


//===========================================================================
// Callback function for transmit events
//===========================================================================
static void can_xmt_msg(can_channel *chan, void *pdata)
{
    can_cbuf_t        *cbuf    = &chan->out_cbuf;
    can_lowlevel_funs *funs    = chan->funs;  
    cyg_can_message   *ptxbuf  = (cyg_can_message *)cbuf->pdata;
    cyg_can_message   *pbuf_txmsg;

    //
    // transmit messages as long as there are messages in the buffer 
    //
    while (cbuf->data_cnt > 0)
    {
        pbuf_txmsg = &ptxbuf[cbuf->get];
        
        if (funs->putmsg(chan, pbuf_txmsg, pdata))
        {
            cbuf->get = (cbuf->get + 1) % cbuf->len;
            cbuf->data_cnt--;
        }
        else
        {
            //
            // we are here because the hardware is busy at the moment and
            // we can't send another message - now we check if there is already
            // some space in buffer so we can wakeup the writer
            //
            if ((cbuf->len - cbuf->data_cnt) > 0)
            {
                if (cbuf->waiting)
                {
                    cbuf->waiting = false;
                    cyg_drv_cond_broadcast(&cbuf->wait);
                }
            }
            return;
        }
    } // while (cbuf->data_cnt > 0)
    funs->stop_xmit(chan);  // Done with transmit
    
    if (cbuf->waiting)
    {
        cbuf->waiting = false;
        cyg_drv_cond_broadcast(&cbuf->wait);
    }            
}


//---------------------------------------------------------------------------
// end of can.c
