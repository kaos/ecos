#ifndef CYGONCE_SERIAL_H
#define CYGONCE_SERIAL_H
// ====================================================================
//
//      serial.h
//
//      Device I/O 
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Serial I/O interfaces

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/io.h>
#include <cyg/io/serialio.h>
#include <cyg/hal/drv_api.h>

typedef struct serial_channel serial_channel;
typedef struct serial_funs serial_funs;

// Pointers into upper-level driver which interrupt handlers need
typedef struct {
    // Initialize the channel
    void (*serial_init)(serial_channel *chan);
    // Cause an additional character to be output if one is available
    void (*xmt_char)(serial_channel *chan);
    // Consume an input character
    void (*rcv_char)(serial_channel *chan, unsigned char c);
} serial_callbacks_t;

#define SERIAL_CALLBACKS(_l,_init,_xmt_char,_rcv_char)  \
serial_callbacks_t _l = {                               \
    _init,                                              \
    _xmt_char,                                          \
    _rcv_char                                           \
};

extern serial_callbacks_t cyg_io_serial_callbacks;

typedef struct {
    unsigned char           *data;
    volatile int             put;
    volatile int             get;
    int                      len;
    int                      low_water;   // Min space in buffer before restart
    cyg_drv_cond_t           wait;
    cyg_drv_mutex_t          lock;
    bool                     waiting;
    volatile bool            abort;       // Set by an outsider to kill processing
    volatile cyg_int32       pending;     // This many bytes waiting to be sent
} cbuf_t;

#define CBUF_INIT(_data, _len) \
   {_data, 0, 0, _len}

// Private data which describes this channel
struct serial_channel {
    serial_funs        *funs;
    serial_callbacks_t *callbacks;
    void               *dev_priv;  // Whatever is needed by actual device routines
    cyg_serial_info_t   config;    // Current configuration
    bool                init;
    cbuf_t              out_cbuf;
    cbuf_t              in_cbuf;
};

// Initialization macro for serial channel
#define SERIAL_CHANNEL(_l,                                              \
                       _funs,                                           \
                       _dev_priv,                                       \
                       _baud, _stop, _parity, _word_length, _flags)     \
serial_channel _l = {                                                   \
    &_funs,                                                             \
    &cyg_io_serial_callbacks,                                           \
    &(_dev_priv),                                                       \
    CYG_SERIAL_INFO_INIT(_baud, _stop, _parity, _word_length, _flags),  \
};

#define SERIAL_CHANNEL_USING_INTERRUPTS(_l,                             \
                       _funs,                                           \
                       _dev_priv,                                       \
                       _baud, _stop, _parity, _word_length, _flags,     \
                       _out_buf, _out_buflen,                           \
                       _in_buf, _in_buflen)                             \
serial_channel _l = {                                                   \
    &_funs,                                                             \
    &cyg_io_serial_callbacks,                                           \
    &(_dev_priv),                                                       \
    CYG_SERIAL_INFO_INIT(_baud, _stop, _parity, _word_length, _flags),  \
    false,                                                              \
    CBUF_INIT(_out_buf, _out_buflen),                                   \
    CBUF_INIT(_in_buf, _in_buflen)                                      \
};

// Low level interface functions
struct serial_funs {
    // Send one character to the output device, return true if consumed
    bool (*putc)(serial_channel *priv, unsigned char c);
    // Fetch one character from the device
    unsigned char (*getc)(serial_channel *priv);    
    // Change hardware configuration (baud rate, etc)
    bool (*set_config)(serial_channel *priv, cyg_serial_info_t *config);
    // Enable the transmit channel and turn on transmit interrupts
    void (*start_xmit)(serial_channel *priv);
    // Disable the transmit channel and turn transmit interrupts off
    void (*stop_xmit)(serial_channel *priv);
};

#define SERIAL_FUNS(_l,_putc,_getc,_set_config,_start_xmit,_stop_xmit)  \
serial_funs _l = {                                                      \
  _putc,                                                                \
  _getc,                                                                \
  _set_config,                                                          \
  _start_xmit,                                                          \
  _stop_xmit                                                            \
};

extern cyg_devio_table_t cyg_io_serial_devio;

#endif // CYGONCE_SERIAL_H
