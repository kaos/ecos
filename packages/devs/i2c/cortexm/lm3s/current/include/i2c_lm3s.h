//==========================================================================
//
//      i2c_lm3s8xx.h
//
//      I2C driver for Stellaris Cortex M3 microcontroller
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2010, 2011 Free Software Foundation, Inc.
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
// Author(s):     ccoutand
// Contributors:  
// Date:          2011-01-18
// Original:      Bart Veer
//                I2C driver for motorola coldfire processor
// Description:   I2C driver for Stellaris Cortex M3 microcontroller
//####DESCRIPTIONEND####
//==========================================================================

#ifndef CYGONCE_I2C_LM3S_H
#define CYGONCE_I2C_LM3S_H

#include <pkgconf/devs_i2c_cortexm_lm3s.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>

typedef enum lm3s_i2c_xfer_mode {
    LM3S_I2C_XFER_MODE_INVALID = 0x00,
    LM3S_I2C_XFER_MODE_TX = 0x01,
    LM3S_I2C_XFER_MODE_RX = 0x02
} lm3s_i2c_xfer_mode;

typedef struct lm3s_i2c_extra {
    cyg_uint32      i2c_base;          // I2C base address
    cyg_uint32      i2c_periph;        // I2C peripheral bit mask
    cyg_uint8       i2c_owner;         // We have bus ownership
    cyg_uint8       i2c_lost_arb;      // Error condition leading to loss of
                                       // bus ownership
    cyg_uint8       i2c_send_nack;     // As per rx send_nack argument
    cyg_uint8       i2c_got_nack;      // The last tx resulted in a nack
    cyg_uint8       i2c_completed;     // Set by DSR, checked by thread

    union {
        const cyg_uint8 *i2c_tx_data;
        cyg_uint8      *i2c_rx_data;
    } i2c_data;                        // The current buffer for rx or tx
    cyg_uint32      i2c_count;         // Number of bytes left in buffer
    lm3s_i2c_xfer_mode i2c_mode;       // TX, RX, ...

    cyg_bool        send_stop;
    cyg_bool        send_start;
    cyg_drv_mutex_t i2c_lock;          // For synchronizing between DSR and
                                       // foreground
    cyg_drv_cond_t  i2c_wait;
    // For initializing the interrupt
    cyg_handle_t    i2c_interrupt_handle;
    cyg_interrupt   i2c_interrupt_data;
    cyg_uint32      i2c_isr_id;
    cyg_uint32      i2c_isr_pri;
} lm3s_i2c_extra;

externC void    lm3s_i2c_init(struct cyg_i2c_bus *);
externC cyg_uint32 lm3s_i2c_tx(const cyg_i2c_device *, cyg_bool,
                               const cyg_uint8 *, cyg_uint32, cyg_bool);
externC cyg_uint32 lm3s_i2c_rx(const cyg_i2c_device *, cyg_bool, cyg_uint8 *,
                               cyg_uint32, cyg_bool, cyg_bool);
externC void    lm3s_i2c_stop(const cyg_i2c_device *);

#define CYG_LM3S_I2C_BUS(                               \
        _name_,                                         \
        _init_fn_,                                      \
        _base_,                                         \
        _periph_,                                       \
        _isr_vec_,                                      \
        _isr_pri_,                                      \
        _fdr_)                                          \
    static lm3s_i2c_extra _name_ ## _extra = {          \
       .i2c_base = _base_,                              \
       .i2c_periph = _periph_,                          \
       .i2c_isr_id = _isr_vec_,                         \
       .i2c_isr_pri= _isr_pri_                          \
    };                                                  \
    CYG_I2C_BUS(_name_,                                 \
                _init_fn_,                              \
                lm3s_i2c_tx,                            \
                lm3s_i2c_rx,                            \
                lm3s_i2c_stop,                          \
                (void*) & ( _name_ ## _extra)) ;

#endif // CYGONCE_I2C_LM3S_H

// -------------------------------------------------------------------------
// EOF i2c_lm3s.h
