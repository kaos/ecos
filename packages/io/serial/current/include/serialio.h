#ifndef CYGONCE_SERIALIO_H
#define CYGONCE_SERIALIO_H
// ====================================================================
//
//      serialio.h
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
// Purpose:     Special support for serial I/O devices
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// This file contains the user-level visible I/O interfaces

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/config_keys.h>

#ifdef __cplusplus
extern "C" {
#endif

// Supported baud rates
typedef enum {
    CYGNUM_SERIAL_BAUD_50 = 1,
    CYGNUM_SERIAL_BAUD_75,
    CYGNUM_SERIAL_BAUD_110,
    CYGNUM_SERIAL_BAUD_134_5,
    CYGNUM_SERIAL_BAUD_150,
    CYGNUM_SERIAL_BAUD_200,
    CYGNUM_SERIAL_BAUD_300,
    CYGNUM_SERIAL_BAUD_600,
    CYGNUM_SERIAL_BAUD_1200,
    CYGNUM_SERIAL_BAUD_1800,
    CYGNUM_SERIAL_BAUD_2400,
    CYGNUM_SERIAL_BAUD_3600,
    CYGNUM_SERIAL_BAUD_4800,
    CYGNUM_SERIAL_BAUD_7200,
    CYGNUM_SERIAL_BAUD_9600,
    CYGNUM_SERIAL_BAUD_14400,
    CYGNUM_SERIAL_BAUD_19200,
    CYGNUM_SERIAL_BAUD_38400,
    CYGNUM_SERIAL_BAUD_57600,
    CYGNUM_SERIAL_BAUD_115200,
    CYGNUM_SERIAL_BAUD_230400
} cyg_serial_baud_rate_t;
#define CYGNUM_SERIAL_BAUD_MIN CYGNUM_SERIAL_BAUD_50 
#define CYGNUM_SERIAL_BAUD_MAX CYGNUM_SERIAL_BAUD_230400

// Note: two levels of macro are required to get proper expansion.
#define _CYG_SERIAL_BAUD_RATE(n) CYGNUM_SERIAL_BAUD_##n
#define CYG_SERIAL_BAUD_RATE(n) _CYG_SERIAL_BAUD_RATE(n)

// Stop bit selections
typedef enum {
    CYGNUM_SERIAL_STOP_1 = 1,
    CYGNUM_SERIAL_STOP_1_5,
    CYGNUM_SERIAL_STOP_2
} cyg_serial_stop_bits_t;

// Parity modes
typedef enum {
    CYGNUM_SERIAL_PARITY_NONE = 0,
    CYGNUM_SERIAL_PARITY_EVEN,
    CYGNUM_SERIAL_PARITY_ODD,
    CYGNUM_SERIAL_PARITY_MARK,
    CYGNUM_SERIAL_PARITY_SPACE
} cyg_serial_parity_t;

// Word length
typedef enum {
    CYGNUM_SERIAL_WORD_LENGTH_5 = 5,
    CYGNUM_SERIAL_WORD_LENGTH_6,
    CYGNUM_SERIAL_WORD_LENGTH_7,
    CYGNUM_SERIAL_WORD_LENGTH_8
} cyg_serial_word_length_t;

typedef struct {
    cyg_serial_baud_rate_t   baud;
    cyg_serial_stop_bits_t   stop;
    cyg_serial_parity_t      parity;
    cyg_serial_word_length_t word_length;
    cyg_uint32               flags;
} cyg_serial_info_t;

// cyg_serial_info_t flags
#define CYGNUM_SERIAL_FLOW_NONE              (0)
// receive flow control, send xon/xoff when necessary:
#define CYGNUM_SERIAL_FLOW_XONXOFF_RX        (1<<0)  
// transmit flow control, act on received xon/xoff:
#define CYGNUM_SERIAL_FLOW_XONXOFF_TX        (1<<1)
// receive flow control, send RTS when necessary:
#define CYGNUM_SERIAL_FLOW_RTSCTS_RX         (1<<2)
// transmit flow control, act when not CTS:
#define CYGNUM_SERIAL_FLOW_RTSCTS_TX         (1<<3)
// receive flow control, send DTR when necessary:
#define CYGNUM_SERIAL_FLOW_DSRDTR_RX         (1<<4)
// transmit flow control, act when not DSR:
#define CYGNUM_SERIAL_FLOW_DSRDTR_TX         (1<<5)

// arguments for CYG_IO_SET_CONFIG_SERIAL_FLOW_CONTROL_FORCE
#define CYGNUM_SERIAL_FLOW_THROTTLE_RX       0
#define CYGNUM_SERIAL_FLOW_RESTART_RX        1
#define CYGNUM_SERIAL_FLOW_THROTTLE_TX       2
#define CYGNUM_SERIAL_FLOW_RESTART_TX        3

// arguments for CYG_IO_SET_CONFIG_SERIAL_HW_RX_FLOW_THROTTLE
#define CYGNUM_SERIAL_FLOW_HW_UNTHROTTLE     0
#define CYGNUM_SERIAL_FLOW_HW_THROTTLE       0
#define CYGNUM_SERIAL_FLOW_HW_UNTHROTTLE     0

typedef struct {  
    cyg_int32 rx_bufsize;
    cyg_int32 rx_count;
    cyg_int32 tx_bufsize;
    cyg_int32 tx_count;
} cyg_serial_buf_info_t;

#define CYG_SERIAL_INFO_INIT(_baud,_stop,_parity,_word_length,_flags) \
  { _baud, _stop, _parity, _word_length, _flags}

#ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS

# define CYGNUM_SERIAL_STATUS_FLOW          0
# define CYGNUM_SERIAL_STATUS_BREAK         1
# define CYGNUM_SERIAL_STATUS_FRAMEERR      2
# define CYGNUM_SERIAL_STATUS_PARITYERR     3
# define CYGNUM_SERIAL_STATUS_OVERRUNERR    4
# define CYGNUM_SERIAL_STATUS_CARRIERDETECT 5
# define CYGNUM_SERIAL_STATUS_RINGINDICATOR 6

typedef struct {
    cyg_uint32 which;        // one of CYGNUM_SERIAL_STATUS_* above
    cyg_uint32 value;        // and its value
} cyg_serial_line_status_t;

typedef void (*cyg_serial_line_status_callback_fn_t)(
                                                 cyg_serial_line_status_t *s,
                                                 CYG_ADDRWORD priv );
typedef struct {
    cyg_serial_line_status_callback_fn_t fn;
    CYG_ADDRWORD priv;
} cyg_serial_line_status_callback_t;

#endif // ifdef CYGOPT_IO_SERIAL_SUPPORT_LINE_STATUS

// Default configuration
#define CYG_SERIAL_BAUD_DEFAULT        CYGNUM_SERIAL_BAUD_38400
#define CYG_SERIAL_STOP_DEFAULT        CYGNUM_SERIAL_STOP_1
#define CYG_SERIAL_PARITY_DEFAULT      CYGNUM_SERIAL_PARITY_NONE
#define CYG_SERIAL_WORD_LENGTH_DEFAULT CYGNUM_SERIAL_WORD_LENGTH_8
#define CYG_SERIAL_FLAGS_DEFAULT       0

#ifdef CYGDAT_IO_SERIAL_FLOW_CONTROL_DEFAULT_XONXOFF
# define CYG_SERIAL_FLAGS_DEFAULT      CYG_SERIAL_FLOW_XONXOFF
#elif defined(CYGDAT_IO_SERIAL_FLOW_CONTROL_DEFAULT_RTSCTS)
# define CYG_SERIAL_FLAGS_DEFAULT      CYG_SERIAL_FLOW_RTSCTS
#elif defined(CYGDAT_IO_SERIAL_FLOW_CONTROL_DEFAULT_DSRDTR)
# define CYG_SERIAL_FLAGS_DEFAULT      CYG_SERIAL_FLOW_DSRDTR
#else 
# define CYG_SERIAL_FLAGS_DEFAULT      0
#endif

#ifdef __cplusplus
}
#endif

#endif  /* CYGONCE_SERIALIO_H */
/* EOF serialio.h */
