//==========================================================================
//
//      i2c_lpc2xxx.c
//
//      I2C driver for LPC2xxx
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
// Author(s):    Hans Rosenfeld <rosenfeld@grumpf.hope-2000.org>
// Contributors: Uwe Kindler <uwe_kindler@web.de>
// Date:         2007-07-12
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                                 INCLUDES
//==========================================================================
#include <pkgconf/system.h>
#include <pkgconf/devs_i2c_arm_lpc2xxx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/io/i2c_lpc2xxx.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

//
// According to the Users Manual the LPC2xxx I2C module is very
// similar to the I2C module of the Philips 8xC552/556 controllers. I
// guess it is used in other Philips/NXP controllers, too. Using these
// macros should make it easier to split off the common parts of the
// driver once it's necessary.
//
// Optimize for the case of a single bus device, while still allowing
// multiple devices.
//
#ifndef CYGHWR_DEVS_I2C_ARM_LPC2XXX_MULTIPLE_BUSES
# define    I2C_BASE(_extra_)       (cyg_uint8*)HAL_LPC2XXX_I2C_SINGLETON_BASE
# define    I2C_ISRVEC(_extra_)     HAL_LPC2XXX_I2C_SINGLETON_ISRVEC
# define    I2C_ISRPRI(_extra_)     HAL_LPC2XXX_I2C_SINGLETON_ISRPRI
# define    I2C_CLK(_extra_)        HAL_LPC2XXX_I2C_SINGLETON_CLK
# define    I2C_BUS_FREQ(_extra_)   HAL_LPC2XXX_I2C_SINGLETON_BUS_FREQ
#else
# define    I2C_BASE(_extra_)       ((_extra_)->i2c_base)
# define    I2C_ISRVEC(_extra_)     ((_extra_)->i2c_isrvec)
# define    I2C_ISRPRI(_extra_)     ((_extra_)->i2c_isrpri)
# define    I2C_CLK(_extra_)        ((_extra_)->i2c_pclk)
# define    I2C_BUS_FREQ(_extra_)   ((_extra_)->i2c_bus_freq)
#endif // CYGHWR_DEVS_I2C_ARM_LPC2XXX_MULTIPLE_BUSES

#define I2C_XFER             8

#define I2C_CONSET(_extra_)  (I2C_BASE(_extra_) + 0x0000)
#define I2C_CON(_extra_)      I2C_CONSET(_extra_)
#define I2C_STAT(_extra_)    (I2C_BASE(_extra_) + 0x0004)
#define I2C_DAT(_extra_)     (I2C_BASE(_extra_) + 0x0008)
#define I2C_ADR(_extra_)     (I2C_BASE(_extra_) + 0x000C)
#define I2C_SCLH(_extra_)    (I2C_BASE(_extra_) + 0x0010)
#define I2C_SCLL(_extra_)    (I2C_BASE(_extra_) + 0x0014)
#define I2C_CONCLR(_extra_)  (I2C_BASE(_extra_) + 0x0018)

#define I2C_R8(r, x)          HAL_READ_UINT8  ((r), (x))
#define I2C_W8(r, x)          HAL_WRITE_UINT8 ((r), (x))
#define I2C_R16(r, x)         HAL_READ_UINT16 ((r), (x))
#define I2C_W16(r, x)         HAL_WRITE_UINT16((r), (x))

// Special case for setting/clearing bits in I2C_CON
#define SET_CON(_extra_, x)   I2C_W8(I2C_CONSET(_extra_), (x))
#define CLR_CON(_extra_, x)   I2C_W8(I2C_CONCLR(_extra_), (x))

// I2C_CONSET register bits
#define CON_AA   (1<<2)
#define CON_SI   (1<<3)
#define CON_STO  (1<<4)
#define CON_STA  (1<<5)
#define CON_EN   (1<<6)


#define I2C_FLAG_FINISH  1       // transfer finished                       
#define I2C_FLAG_ACT     2       // bus still active, no STOP condition send
#define I2C_FLAG_ERROR  (1<<31)  // one of the following errors occured:    
#define I2C_FLAG_ADDR   (1<<30)  // - address was not ACKed                 
#define I2C_FLAG_DATA   (1<<29)  // - data was not ACKed                    
#define I2C_FLAG_LOST   (1<<28)  // - bus arbitration was lost              
#define I2C_FLAG_BUF    (1<<27)  // - no buffer for reading or writing      
#define I2C_FLAG_UNK    (1<<26)  // - unknown I2C status                   
#define I2C_FLAG_BUS    (1<<25)  // - bus error

#if CYGPKG_DEVS_I2C_ARM_LPC2XXX_DEBUG_LEVEL > 0
   #define debug1_printf(args...) diag_printf(args)
#else
   #define debug1_printf(args...)
#endif
#if CYGPKG_DEVS_I2C_ARM_LPC2XXX_DEBUG_LEVEL > 1
   #define debug2_printf(args...) diag_printf(args)
#else
   #define debug2_printf(args...)
#endif

//==========================================================================
// The ISR does the actual work. It is not that much work to justify
// putting it in the DSR, and it is also not clear whether this would
// even work.  If an error occurs we try to leave the bus in the same
// state as we would if there was no error.
//==========================================================================
static cyg_uint32 lpc2xxx_i2c_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    cyg_lpc2xxx_i2c_extra* extra = (cyg_lpc2xxx_i2c_extra*)data;
    cyg_uint8  status;
    
    I2C_R8(I2C_STAT(extra), status);
    switch(status) 
    {
    case 0x00: // bus error, stop transfer
         SET_CON(extra, CON_STO);
         extra->i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_BUS;
         break;
         
    case 0x08: // START sent, send Addr+R/W
    case 0x10: // ReSTART sent, send Addr+R/W
         CLR_CON(extra, CON_STA);
         I2C_W8(I2C_DAT(extra), extra->i2c_addr);
         break;
      
    case 0x18: // Addr ACKed, send data
         if(extra->i2c_txbuf == NULL) 
         {
             extra->i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_BUF;
             cyg_drv_interrupt_mask_intunsafe(vec);
             cyg_drv_interrupt_acknowledge(vec);
             return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
         }
         I2C_W8(I2C_DAT(extra), *extra->i2c_txbuf);
         extra->i2c_txbuf++;
         break;
         
    case 0x28: // Data ACKed, send more
         extra->i2c_count--;     
         if(extra->i2c_count == 0) 
         {
             extra->i2c_flag = I2C_FLAG_FINISH;
             cyg_drv_interrupt_mask_intunsafe(vec);
             cyg_drv_interrupt_acknowledge(vec);
             return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
         }
         I2C_W8(I2C_DAT(extra), *extra->i2c_txbuf);
         extra->i2c_txbuf++;
         break;
      
    case 0x50: // Data ACKed, receive more
    case 0x58: // Data not ACKed, end reception
         if(extra->i2c_rxbuf == NULL) 
         {
             extra->i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_BUF;
             cyg_drv_interrupt_mask_intunsafe(vec);
             cyg_drv_interrupt_acknowledge(vec);
             return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
         }
      
         I2C_R8(I2C_DAT(extra), *extra->i2c_rxbuf);
         extra->i2c_rxbuf++;
         extra->i2c_count--;
         // fall through
         
    case 0x40: // Addr ACKed, receive data
         if(status == 0x58 || extra->i2c_count == 0) 
         {
             extra->i2c_flag = I2C_FLAG_FINISH;
             cyg_drv_interrupt_mask_intunsafe(vec);
             cyg_drv_interrupt_acknowledge(vec);
             return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
         }
      
         if((extra->i2c_count == 1) && extra->i2c_rxnak)
         {
             CLR_CON(extra, CON_AA);
         }
         else 
         {   
             SET_CON(extra, CON_AA);
         }
         break;
      
    case 0x20: // Addr not ACKed
    case 0x48: // Addr not ACKed
         SET_CON(extra, CON_STO); // tranfer failed - force stop
         extra->i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_ADDR;
         cyg_drv_interrupt_mask_intunsafe(vec);
         cyg_drv_interrupt_acknowledge(vec);
         break;
         
    case 0x30: // Data not ACKed
         SET_CON(extra, CON_STO); // tranfer failed - force stop
         extra->i2c_count++;
         extra->i2c_txbuf--;
         extra->i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_DATA;
         cyg_drv_interrupt_mask_intunsafe(vec);
         cyg_drv_interrupt_acknowledge(vec);
         return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
         break;
         
    case 0x38: // Arbitration lost
         extra->i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_LOST;
         break;
         
    default: // lots of unused states
         extra->i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_UNK;
         break;
    } // switch(status)
  
    CLR_CON(extra, CON_SI);
    cyg_drv_interrupt_acknowledge(vec);
    
    //
    // We need to call the DSR only if there is really something to signal,
    // that means only if extra->i2c_flag != 0
    //
    if (extra->i2c_flag)
    {
        return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
    }
    else
    {
        return CYG_ISR_HANDLED; 
    }
}


//==========================================================================
// DSR signals data
//==========================================================================
static void
lpc2xxx_i2c_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_lpc2xxx_i2c_extra* extra = (cyg_lpc2xxx_i2c_extra*)data;
    if(extra->i2c_flag)
    {
        cyg_drv_cond_signal(&extra->i2c_wait);
    }
}


//==========================================================================
// Initialize driver & hardware state
//==========================================================================
void cyg_lpc2xxx_i2c_init(struct cyg_i2c_bus *bus)
{
    cyg_lpc2xxx_i2c_extra* extra = (cyg_lpc2xxx_i2c_extra*)bus->i2c_extra;
    cyg_uint16             duty_cycle;
  
    cyg_drv_mutex_init(&extra->i2c_lock);
    cyg_drv_cond_init(&extra->i2c_wait, &extra->i2c_lock);
    cyg_drv_interrupt_create(I2C_ISRVEC(extra),
                             I2C_ISRPRI(extra),
                             (cyg_addrword_t) extra,
                             &lpc2xxx_i2c_isr,
                             &lpc2xxx_i2c_dsr,
                             &(extra->i2c_interrupt_handle),
                             &(extra->i2c_interrupt_data));
    cyg_drv_interrupt_attach(extra->i2c_interrupt_handle);
  
   
    CLR_CON(extra, CON_EN | CON_STA | CON_SI | CON_AA);
    HAL_WRITE_UINT8(I2C_ADR(extra), 0);
    
    //
    // Setup I2C bus frequency
    //
    duty_cycle = (I2C_CLK(extra) / I2C_BUS_FREQ(extra)) / 2;
    HAL_WRITE_UINT16(I2C_SCLL(extra), duty_cycle);
    HAL_WRITE_UINT16(I2C_SCLH(extra), duty_cycle);
    
    SET_CON(extra, CON_EN);
}


//==========================================================================
// transmit a buffer to a device
//==========================================================================
cyg_uint32 cyg_lpc2xxx_i2c_tx(const cyg_i2c_device *dev, 
                              cyg_bool              send_start, 
                              const cyg_uint8      *tx_data, 
                              cyg_uint32            count, 
                              cyg_bool              send_stop)
{
    cyg_lpc2xxx_i2c_extra* extra = 
                           (cyg_lpc2xxx_i2c_extra*)dev->i2c_bus->i2c_extra;
    extra->i2c_addr  = dev->i2c_address << 1;
    extra->i2c_count = count;
    extra->i2c_txbuf = tx_data;
  
    //
    // for a repeated start the SI bit has to be reset
    // if we continue a previous transfer, load the next byte
    //
    if(send_start) 
    {
        SET_CON(extra, CON_STA);
        if (I2C_FLAG_ACT == extra->i2c_flag)
        {
            CLR_CON(extra, CON_SI);
        }
    } 
    else 
    {
        HAL_WRITE_UINT8(I2C_DAT(extra), *(extra->i2c_txbuf));
        extra->i2c_txbuf++;
        CLR_CON(extra, CON_SI);
    }
  
    extra->i2c_flag  = 0;
  
    //
    // the isr will do most of the work, and the dsr will signal when an
    // error occured or the transfer finished
    //
    cyg_drv_mutex_lock(&extra->i2c_lock);
    cyg_drv_dsr_lock();
    cyg_drv_interrupt_unmask(I2C_ISRVEC(extra));
    while(!(extra->i2c_flag & (I2C_FLAG_FINISH | I2C_FLAG_ERROR)))
    {
        cyg_drv_cond_wait(&extra->i2c_wait);
    }
    cyg_drv_interrupt_mask(I2C_ISRVEC(extra));
    cyg_drv_dsr_unlock();
    cyg_drv_mutex_unlock(&extra->i2c_lock);
  
    // too bad we have no way to tell the caller
    if(extra->i2c_flag & I2C_FLAG_ERROR)
    {
        debug1_printf("I2C TX error flag: %x\n", extra->i2c_flag);
        extra->i2c_flag = 0;
    }
    else
    {
        if(send_stop) 
        {
            SET_CON(extra, CON_STO);
            CLR_CON(extra, CON_SI | CON_STA);
            extra->i2c_flag = 0;
        } 
        else  
        {
            extra->i2c_flag = I2C_FLAG_ACT;
        }
    }
   
    count -= extra->i2c_count;
  
    extra->i2c_addr  = 0;
    extra->i2c_count = 0;
    extra->i2c_txbuf = NULL;
  
    return count;
}

 
//==========================================================================
// receive into a buffer from a device
//==========================================================================
cyg_uint32 cyg_lpc2xxx_i2c_rx(const cyg_i2c_device *dev,
                              cyg_bool              send_start,
                              cyg_uint8            *rx_data,
                              cyg_uint32            count,
                              cyg_bool              send_nak,
                              cyg_bool              send_stop)
{
    cyg_lpc2xxx_i2c_extra* extra = 
                           (cyg_lpc2xxx_i2c_extra*)dev->i2c_bus->i2c_extra;
    extra->i2c_addr  = (dev->i2c_address << 1) | 0x01;
    extra->i2c_count = count;
    extra->i2c_rxbuf = rx_data;
    extra->i2c_rxnak = send_nak;
  
    //
    // for a repeated start the SI bit has to be reset
    // if we continue a previous transfer, start reception
    //
    if(send_start) 
    {
        SET_CON(extra, CON_STA);
        if (I2C_FLAG_ACT == extra->i2c_flag)
        {
            CLR_CON(extra, CON_SI);
        }
    } 
  
    extra->i2c_flag  = 0;
  
    //
    // the isr will do most of the work, and the dsr will signal when an
    // error occurred or the transfer finished
    //
    cyg_drv_mutex_lock(&extra->i2c_lock);
    cyg_drv_dsr_lock();
    cyg_drv_interrupt_unmask(I2C_ISRVEC(extra));
    while(!(extra->i2c_flag & (I2C_FLAG_FINISH | I2C_FLAG_ERROR)))
    {
        cyg_drv_cond_wait(&extra->i2c_wait);
    }
    cyg_drv_interrupt_mask(I2C_ISRVEC(extra));
    cyg_drv_dsr_unlock();
    cyg_drv_mutex_unlock(&extra->i2c_lock);
  
    // too bad we have no way to tell the caller 
    if (extra->i2c_flag & I2C_FLAG_ERROR)
    {
        diag_printf("I2C RX error flag: %x\n", extra->i2c_flag);
        extra->i2c_flag = 0;
    }
    else
    {
        if(send_stop) 
        {
            SET_CON(extra, CON_STO);
            CLR_CON(extra, CON_SI | CON_STA);
            extra->i2c_flag = 0;
        } 
        else  
        {
            extra->i2c_flag = I2C_FLAG_ACT;
        }
    }
    
    count -= extra->i2c_count;
  
    extra->i2c_addr  = 0;
    extra->i2c_count = 0;
    extra->i2c_rxbuf = NULL;
  
    return count; 
}


//==========================================================================
//  generate a STOP
//==========================================================================
void cyg_lpc2xxx_i2c_stop(const cyg_i2c_device *dev)
{
    cyg_lpc2xxx_i2c_extra* extra = 
                           (cyg_lpc2xxx_i2c_extra*)dev->i2c_bus->i2c_extra;
    extra = extra; // avoid compiler warning in case of singleton
    SET_CON(extra, CON_STO);
    extra->i2c_flag  = 0;
    extra->i2c_count = 0;
}

//---------------------------------------------------------------------------
// eof i2c_lpc2xxx.c
