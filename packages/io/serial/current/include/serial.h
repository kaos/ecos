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
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
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
#include <pkgconf/io_serial.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/io/io.h>
#include <cyg/io/serialio.h>
#include <cyg/hal/drv_api.h>

#ifdef CYGPKG_IO_SERIAL_SELECT_SUPPORT
#include <cyg/fileio/fileio.h>
#endif

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
#if CYGINT_IO_SERIAL_BLOCK_TRANSFER
    // Request space for input characters
    bool (*data_rcv_req)(serial_channel *chan, int avail, 
                         int* space_avail, unsigned char** space);
    // Receive operation completed
    void (*data_rcv_done)(serial_channel *chan);
    // Request characters for transmission
    bool (*data_xmt_req)(serial_channel *chan, int space,
                         int* chars_avail, unsigned char** chars);
    // Transmit operation completed
    void (*data_xmt_done)(serial_channel *chan);
#endif
#if defined(CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS)
    void (*indicate_status)(serial_channel *chan, cyg_serial_line_status_t *s );
#endif
} serial_callbacks_t;

#if CYGINT_IO_SERIAL_BLOCK_TRANSFER
# ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
#  define SERIAL_CALLBACKS(_l,_init,_xmt_char,_rcv_char, _data_rcv_req, _data_rcv_done, _data_xmt_req, _data_xmt_done, _indicate_status)  \
serial_callbacks_t _l = {                               \
    _init,                                              \
    _xmt_char,                                          \
    _rcv_char,                                          \
    _data_rcv_req,                                      \
    _data_rcv_done,                                     \
    _data_xmt_req,                                      \
    _data_xmt_done,                                     \
    _indicate_status                                    \
};
# else
#  define SERIAL_CALLBACKS(_l,_init,_xmt_char,_rcv_char, _data_rcv_req, _data_rcv_done, _data_xmt_req, _data_xmt_done)  \
serial_callbacks_t _l = {                               \
    _init,                                              \
    _xmt_char,                                          \
    _rcv_char,                                          \
    _data_rcv_req,                                      \
    _data_rcv_done,                                     \
    _data_xmt_req,                                      \
    _data_xmt_done                                      \
};
# endif
#else
# ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
#  define SERIAL_CALLBACKS(_l,_init,_xmt_char,_rcv_char,_indicate_status)  \
serial_callbacks_t _l = {                               \
    _init,                                              \
    _xmt_char,                                          \
    _rcv_char,                                          \
    _indicate_status                                    \
};
# else
#  define SERIAL_CALLBACKS(_l,_init,_xmt_char,_rcv_char)  \
serial_callbacks_t _l = {                               \
    _init,                                              \
    _xmt_char,                                          \
    _rcv_char                                           \
};
# endif
#endif

extern serial_callbacks_t cyg_io_serial_callbacks;

typedef struct {
    unsigned char           *data;
    volatile int             put;
    volatile int             get;
    int                      len;
    int                      nb;          // count of bytes currently in buffer
    int                      low_water;   // For tx: min space in buffer before restart
                                          // For rx: max buffer used before flow unthrottled
#ifdef CYGPKG_IO_SERIAL_FLOW_CONTROL
    int                      high_water;  // For tx: unused
                                          // For rx: min buffer used before throttle
#endif
    cyg_drv_cond_t           wait;
    cyg_drv_mutex_t          lock;
    bool                     waiting;
#ifdef CYGOPT_IO_SERIAL_SUPPORT_NONBLOCKING
    bool                     blocking;
#endif
    volatile bool            abort;       // Set by an outsider to kill processing
    volatile cyg_int32       pending;     // This many bytes waiting to be sent
#ifdef CYGPKG_IO_SERIAL_SELECT_SUPPORT    
    struct CYG_SELINFO_TAG   selinfo;     // select info
#endif
} cbuf_t;

#define CBUF_INIT(_data, _len) \
   {_data, 0, 0, _len}

#ifdef CYGPKG_IO_SERIAL_FLOW_CONTROL
typedef struct {
    cyg_uint32 flags;
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_SOFTWARE
    cyg_uint8  xchar;
#endif
} flow_desc_t;
#endif

// Private data which describes this channel
struct serial_channel {
    serial_funs        *funs;
    serial_callbacks_t *callbacks;
    void               *dev_priv;  // Whatever is needed by actual device routines
    cyg_serial_info_t   config;    // Current configuration
    bool                init;
    cbuf_t              out_cbuf;
    cbuf_t              in_cbuf;
#ifdef CYGPKG_IO_SERIAL_FLOW_CONTROL
    flow_desc_t         flow_desc;
#endif
#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS
    cyg_serial_line_status_callback_fn_t status_callback;
    CYG_ADDRWORD             status_callback_priv;
#endif
};

// Flow descriptor flag values
#define CYG_SERIAL_FLOW_OUT_THROTTLED     (1<<0)
#define CYG_SERIAL_FLOW_IN_THROTTLED      (1<<1)

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
    Cyg_ErrNo (*set_config)(serial_channel *priv, cyg_uint32 key, const void *xbuf,
                            cyg_uint32 *len);
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
