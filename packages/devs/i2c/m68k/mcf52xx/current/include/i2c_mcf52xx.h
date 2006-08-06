//==========================================================================
//
//      devs/i2c/m68k/mcf52xx/current/src/i2c_mcf52xx.h
//
//      I2C driver for Motorola coldfire processors
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005, 2006 eCosCentric Ltd.
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Bart Veer
// Contributors:  
// Date:          2005-11-20
// Description:   I2C driver for motorola coldfire processor
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/devs_i2c_mcf52xx.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>

typedef enum cyg_mcf52xx_i2c_xfer_mode {
    CYG_MCF52xx_I2C_XFER_MODE_INVALID   = 0x00,
    CYG_MCF52xx_I2C_XFER_MODE_TX        = 0x01,
    CYG_MCF52xx_I2C_XFER_MODE_RX        = 0x02,
    CYG_MCF52xx_I2C_XFER_MODE_STARTRX   = 0x03
} cyg_mcf52xx_i2c_xfer_mode;

typedef struct cyg_mcf52xx_i2c_extra {
#ifdef CYGHWR_DEVS_I2C_MCF52xx_MULTIPLE_BUSES    
    // Put statically initialized fields first.
    cyg_uint8*                  i2c_base;       // Per-bus h/w details
    cyg_vector_t                i2c_isrvec;
    int                         i2c_isrpri;
    int                         i2c_fdr;
#endif    

    cyg_uint8                   i2c_owner;      // We have bus ownership
    cyg_uint8                   i2c_lost_arb;   // Error condition leading to loss of bus ownership
    cyg_uint8                   i2c_send_nack;  // As per rx send_nack argument
    cyg_uint8                   i2c_got_nack;   // The last tx resulted in a nack
    cyg_uint8                   i2c_completed;  // Set by DSR, checked by thread
    
    union {
        const cyg_uint8*        i2c_tx_data;
        cyg_uint8*              i2c_rx_data;
    } i2c_data;                                 // The current buffer for rx or tx
    cyg_uint32                  i2c_count;      // Number of bytes left in buffer
    cyg_mcf52xx_i2c_xfer_mode   i2c_mode;       // TX, RX, ...


    cyg_drv_mutex_t             i2c_lock;       // For synchronizing between DSR and foreground
    cyg_drv_cond_t              i2c_wait;
    cyg_handle_t                i2c_interrupt_handle;   // For initializing the interrupt
    cyg_interrupt               i2c_interrupt_data;
} cyg_mcf52xx_i2c_extra;

externC void        cyg_mcf52xx_i2c_init(struct cyg_i2c_bus*);
externC cyg_uint32  cyg_mcf52xx_i2c_tx(const cyg_i2c_device*, cyg_bool, const cyg_uint8*, cyg_uint32, cyg_bool);
externC cyg_uint32  cyg_mcf52xx_i2c_rx(const cyg_i2c_device*, cyg_bool, cyg_uint8*, cyg_uint32, cyg_bool, cyg_bool);
externC void        cyg_mcf52xx_i2c_stop(const cyg_i2c_device*);

#ifdef CYGHWR_DEVS_I2C_MCF52xx_MULTIPLE_BUSES
# define CYG_MCF52xx_I2C_BUS(_name_, _init_fn_, _base_, _isr_vec_, _isr_pri_, _fdr_)    \
    static cyg_mcf52xx_i2c_extra _name_ ## _extra = {                                   \
        _base_,                                                                         \
        _isr_vec_,                                                                      \
        _isr_pri_,                                                                      \
        _fdr_                                                                           \
    } ;                                                                                 \
    CYG_I2C_BUS(_name_,                                                                 \
                _init_fn_,                                                              \
                &cyg_mcf52xx_i2c_tx,                                                    \
                &cyg_mcf52xx_i2c_rx,                                                    \
                &cyg_mcf52xx_i2c_stop,                                                  \
                (void*) & ( _name_ ## _extra)) ;

#else
# define CYG_MCF52xx_I2C_BUS(_name_, _init_fn_, _base_, _isr_vec_, _isr_pri_, _fdr_)    \
    static cyg_mcf52xx_i2c_extra _name_ ## _extra;                                      \
    CYG_I2C_BUS(_name_,                                                                 \
                _init_fn_,                                                              \
                cyg_mcf52xx_i2c_tx,                                                     \
                cyg_mcf52xx_i2c_rx,                                                     \
                cyg_mcf52xx_i2c_stop,                                                   \
                (void*) & ( _name_ ## _extra)) ;
#endif

