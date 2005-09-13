#ifndef CYGONCE_CAN_H
#define CYGONCE_CAN_H
// ====================================================================
//
//      can.h
//
//      Device I/O 
//
// ====================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: gthomas
// Date:         2005-12-05
// Purpose:      Internal interfaces for CAN I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


//==========================================================================
//                                INCLUDES
//==========================================================================
#include <pkgconf/system.h>
#include <pkgconf/io_can.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/io/io.h>
#include <cyg/io/canio.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/drv_api.h>


//===========================================================================
//                            FORWARD DECLARATIONS
//===========================================================================
typedef struct can_channel       can_channel;
typedef struct can_lowlevel_funs can_lowlevel_funs;


//===========================================================================
//                                DATA TYPES
//===========================================================================

//---------------------------------------------------------------------------
// Pointers into uper-level driver which ISRs and DSRs need
//
typedef struct 
{
    void (*can_init)(can_channel *chan);              // Initialize the can channel
    void (*xmt_msg)(can_channel *chan, void *pdata);  // transmit one single message
    void (*rcv_event)(can_channel *chan, void *pdata);// indicate can event occurance
} can_callbacks_t;


#define CAN_CALLBACKS(_l,_init,_xmt_msg,_rcv_event)  \
can_callbacks_t _l = {                               \
    _init,                                           \
    _xmt_msg,                                        \
    _rcv_event                                       \
};

extern can_callbacks_t cyg_io_can_callbacks;


//---------------------------------------------------------------------------
// Data buffer for receive and transmit FIFOs
//
typedef struct can_cbuf_st
{
    void                    *pdata;      // points to data buffer
    volatile int             put;
    volatile int             get;
    int                      len;
    volatile int             data_cnt;   // count of events or messages currently in buffer
    cyg_drv_cond_t           wait;
    cyg_drv_mutex_t          lock;
    bool                     waiting;
#if defined(CYGOPT_IO_CAN_SUPPORT_NONBLOCKING)
    bool                     blocking;   // true - timeouts will bes used, false - call blocks until data arrives
#endif    
    volatile bool            abort;      // Set by an outsider to kill processing
    volatile cyg_int32       pending;    // This many bytes waiting to be sent
#if defined(CYGOPT_IO_CAN_SUPPORT_TIMEOUTS)
    cyg_uint32               timeout;    // timeout value for reading data from buffer
#endif
} can_cbuf_t;


#ifdef CYGOPT_IO_CAN_SUPPORT_TIMEOUTS
#define _TX_TIMEOUT CYGNUM_IO_CAN_DEFAULT_TIMEOUT_WRITE
#define _RX_TIMEOUT CYGNUM_IO_CAN_DEFAULT_TIMEOUT_READ
#define CBUF_INIT(_data, _len, _timeout)   \
   {_data, 0, 0, _len, timeout : _timeout}
#else
#define _TX_TIMEOUT 0
#define _RX_TIMEOUT 0
#define CBUF_INIT(_data, _len, _timeout)   \
   {_data, 0, 0, _len, }
#endif // #ifdef CYGOPT_IO_CAN_SUPPORT_TIMEOUTS
 
   
//---------------------------------------------------------------------------
// Private data which describes this channel + initialisation macro
//
struct can_channel 
{
    can_lowlevel_funs  *funs;
    can_callbacks_t    *callbacks;
    void               *dev_priv;  // Whatever is needed by actual device routines
    cyg_can_info_t      config;    // Current configuration
    bool                init;      // true if driver is already initialized
    can_cbuf_t          out_cbuf;  // buffer for transmit can messages
    can_cbuf_t          in_cbuf;   // buffer with received can events
};



#define CAN_CHANNEL_USING_INTERRUPTS(_l,                                \
                       _funs,                                           \
                       _dev_priv,                                       \
                       _baud,                                           \
                       _out_buf, _out_buflen,                           \
                       _in_buf, _in_buflen)                             \
can_channel _l = {                                                      \
    &_funs,                                                             \
    &cyg_io_can_callbacks,                                              \
    &(_dev_priv),                                                       \
    CYG_CAN_INFO_INIT(_baud),                                           \
    false,                                                              \
    CBUF_INIT(_out_buf, _out_buflen, _TX_TIMEOUT),                      \
    CBUF_INIT(_in_buf, _in_buflen, _RX_TIMEOUT)                         \
};



//---------------------------------------------------------------------------
// Low level interface functions - these functions are required by the generic 
// CAN driver in order to access the low level hardware
//
struct can_lowlevel_funs 
{
    bool (*putmsg)(can_channel *priv, cyg_can_message *pmsg, void *pdata);   // send one can message - return true if consumed
    bool (*getevent)(can_channel *priv, cyg_can_event *pevent, void *pdata); // fetch one CAN event from device
    Cyg_ErrNo (*get_config)(can_channel    *priv,                            // query hardware configuration (baud rate, etc)
                            cyg_uint32      key, 
                            const void     *xbuf,
                            cyg_uint32     *len);
    Cyg_ErrNo (*set_config)(can_channel    *priv,                           // Change hardware configuration (baud rate, etc)
                            cyg_uint32      key, 
                            const void     *xbuf,
                            cyg_uint32     *len);
    void (*start_xmit)(can_channel *priv);                               // Enable the transmit channel and turn on transmit interrupts
    void (*stop_xmit)(can_channel *priv);                                // Disable the transmit channel and turn transmit interrupts off
};

#define CAN_LOWLEVEL_FUNS(_l,_putmsg,_getevent,_get_config,_set_config,_start_xmit,_stop_xmit)  \
can_lowlevel_funs _l = {                                                                        \
  _putmsg,                                                                                      \
  _getevent,                                                                                    \
  _get_config,                                                                                  \
  _set_config,                                                                                  \
  _start_xmit,                                                                                  \
  _stop_xmit                                                                                    \
};

extern cyg_devio_table_t cyg_io_can_devio;
//-------------------------------------------------------------------------
#endif // CYGONCE_SERIAL_H
