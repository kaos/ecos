//==========================================================================
//
//      i2c_lpc2xxx.c
//
//      I2C driver for LPC2xxx
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Hans Rosenfeld <rosenfeld@grumpf.hope-2000.org>
// Contributors: 
// Date:         2007-07-12
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_i2c_arm_lpc2xxx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

/* 
 * According to the Users Manual the LPC2xxx I2C module is very
 * similar to the I2C module of the Philips 8xC552/556 controllers. I
 * guess it is used in other Philips/NXP controllers, too. Using these
 * macros should make it easier to split off the common parts of the
 * driver once it's necessary.
 */

#define I2C_XFER        8
#define I2C_INTR        CYGNUM_HAL_INTERRUPT_I2C
#define I2C_FREQ        (CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED / \
                         CYGNUM_HAL_ARM_LPC2XXX_VPBDIV)
#define I2C_BASE        CYGARC_HAL_LPC2XXX_REG_I2_BASE

#define I2C_CONSET      CYGARC_HAL_LPC2XXX_REG_I2CONSET
#define I2C_CONCLR      CYGARC_HAL_LPC2XXX_REG_I2CONCLR
#define I2C_CON         I2C_CONSET
#define I2C_STAT        CYGARC_HAL_LPC2XXX_REG_I2STAT
#define I2C_DAT         CYGARC_HAL_LPC2XXX_REG_I2DAT
#define I2C_ADR         CYGARC_HAL_LPC2XXX_REG_I2ADR
#define I2C_SCLH        CYGARC_HAL_LPC2XXX_REG_I2SCLH
#define I2C_SCLL        CYGARC_HAL_LPC2XXX_REG_I2SCLL

#define I2C_R8(r, x)    HAL_READ_UINT8  (I2C_BASE + (r), (x))
#define I2C_W8(r, x)    HAL_WRITE_UINT8 (I2C_BASE + (r), (x))
#define I2C_R16(r, x)   HAL_READ_UINT16 (I2C_BASE + (r), (x))
#define I2C_W16(r, x)   HAL_WRITE_UINT16(I2C_BASE + (r), (x))

/* Special case for setting/clearing bits in I2C_CON */
#define SET_CON(x)      I2C_W8(I2C_CONSET, (x))
#define CLR_CON(x)      I2C_W8(I2C_CONCLR, (x))

#define CON_EN          CYGARC_HAL_LPC2XXX_REG_I2CONSET_I2EN
#define CON_STA         CYGARC_HAL_LPC2XXX_REG_I2CONSET_STA
#define CON_STO         CYGARC_HAL_LPC2XXX_REG_I2CONSET_STO
#define CON_SI          CYGARC_HAL_LPC2XXX_REG_I2CONSET_SI
#define CON_AA          CYGARC_HAL_LPC2XXX_REG_I2CONSET_AA

static cyg_uint8        i2c_addr;
static cyg_uint32       i2c_count = 0;
static const cyg_uint8* i2c_txbuf = NULL;
static cyg_uint8*       i2c_rxbuf = NULL;
static cyg_bool         i2c_rxnak;

volatile
static cyg_uint32       i2c_flag = 0;
static cyg_uint32       i2c_delay;

static cyg_drv_mutex_t  i2c_lock;
static cyg_drv_cond_t   i2c_wait;
static cyg_handle_t     i2c_hand;
static cyg_interrupt    i2c_data;

#define I2C_FLAG_FINISH 1       /* transfer finished                        */
#define I2C_FLAG_ACT    2       /* bus still active, no STOP condition sent */
#define I2C_FLAG_ERROR  (1<<31) /* one of the following errors occured:     */
#define I2C_FLAG_ADDR   (1<<30) /* - address was not ACKed                  */
#define I2C_FLAG_DATA   (1<<29) /* - data was not ACKed                     */
#define I2C_FLAG_LOST   (1<<28) /* - bus arbitration was lost               */
#define I2C_FLAG_BUF    (1<<27) /* - no buffer for reading or writing       */
#define I2C_FLAG_UNK    (1<<26) /* - unknown I2C status                     */

/*
 * set up I2C bus timing
 * I2C clock period is in PCLK ticks
 */
static void
i2c_lpc2xxx_delay(cyg_uint32 delay)
{
  cyg_uint32 period;
  
  if(delay == i2c_delay)
    return;
  
  period = I2C_FREQ / (1000000000 / delay);
  period /= 2;
  
  I2C_W16(I2C_SCLL, period);
  I2C_W16(I2C_SCLH, period);
  i2c_delay = delay;
}

/*
 * The ISR does the actual work. It is not that much work to justify
 * putting it in the DSR, and it is also not clear whether this would
 * even work.  If an error occurs we try to leave the bus in the same
 * state as we would if there was no error.
 */
static cyg_uint32
i2c_lpc2xxx_isr(cyg_vector_t vec, cyg_addrword_t data)
{
  cyg_uint8 status;
  I2C_R8(I2C_STAT, status);
  
  switch(status) {
    case 0x08: /* START sent, send Addr+R/W   */
    case 0x10: /* ReSTART sent, send Addr+R/W */
      CLR_CON(CON_STA);
      I2C_W8(I2C_DAT, i2c_addr);
      break;
      
    case 0x18: /* Addr ACKed, send data       */
    case 0x28: /* Data ACKed, send more       */
      if(i2c_count == 0) {
        i2c_flag = I2C_FLAG_FINISH;
        cyg_drv_interrupt_mask_intunsafe(vec);
        cyg_drv_interrupt_acknowledge(vec);
        return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
      }
      
      if(i2c_txbuf == NULL) {
        i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_BUF;
        cyg_drv_interrupt_mask_intunsafe(vec);
        cyg_drv_interrupt_acknowledge(vec);
        return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
      }
      
      I2C_W8(I2C_DAT, *i2c_txbuf);
      i2c_txbuf++;
      i2c_count--;
      break;
      
    case 0x50: /* Data ACKed, receive more    */
    case 0x58: /* Data not ACKed, end reception */
      if(i2c_rxbuf == NULL) {
        i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_BUF;
        cyg_drv_interrupt_mask_intunsafe(vec);
        cyg_drv_interrupt_acknowledge(vec);
        return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
      }
      
      I2C_R8(I2C_DAT, *i2c_rxbuf);
      i2c_rxbuf++;
      i2c_count--;
    case 0x40: /* Addr ACKed, receive data    */
      if(status == 0x58 || i2c_count == 0) {
        i2c_flag = I2C_FLAG_FINISH;
        cyg_drv_interrupt_mask_intunsafe(vec);
        cyg_drv_interrupt_acknowledge(vec);
        return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
      }
      
      if(i2c_count == 1 && i2c_rxnak)
        CLR_CON(CON_AA);
      else    
        SET_CON(CON_AA);
      break;
      
    case 0x20: /* Addr not ACKed */
    case 0x48:
      i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_ADDR;
      cyg_drv_interrupt_mask_intunsafe(vec);
      cyg_drv_interrupt_acknowledge(vec);
      return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
      break;
    case 0x30: /* Data not ACKed */
      i2c_count++;
      i2c_txbuf--;
      i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_DATA;
      cyg_drv_interrupt_mask_intunsafe(vec);
      cyg_drv_interrupt_acknowledge(vec);
      return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
      break;
    case 0x38: /* Arbitration lost */
      i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_LOST;
      break;
    default: /* lots of unused states... */
      i2c_flag = I2C_FLAG_ERROR | I2C_FLAG_UNK;
      break;
  }
  
  CLR_CON(CON_SI);
  cyg_drv_interrupt_acknowledge(vec);
  return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}

static void
i2c_lpc2xxx_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
  if(i2c_flag)
    cyg_drv_cond_signal(&i2c_wait);
}

/*
 * Initialize driver & hardware state
 */
static void
i2c_lpc2xxx_init(struct cyg_i2c_bus *bus)
{
  cyg_uint32 addr, tmp;
  
  /* enable I2C pins */
  addr = CYGARC_HAL_LPC2XXX_REG_PIN_BASE + CYGARC_HAL_LPC2XXX_REG_PINSEL0;
  HAL_READ_UINT32(addr, tmp);
  tmp |= 0x50;
  HAL_WRITE_UINT32(addr, tmp);
  
  cyg_drv_mutex_init(&i2c_lock);
  cyg_drv_cond_init(&i2c_wait, &i2c_lock);
  cyg_drv_interrupt_create(I2C_INTR, 0, (cyg_addrword_t) 0, &i2c_lpc2xxx_isr,
                           &i2c_lpc2xxx_dsr, &i2c_hand, &i2c_data);
  cyg_drv_interrupt_attach(i2c_hand);
  
  CLR_CON(CON_EN | CON_STA | CON_SI | CON_AA);
  I2C_W8(I2C_ADR, 0);
  SET_CON(CON_EN);
}

/*
 * transmit a buffer to a device
 */
static cyg_uint32
i2c_lpc2xxx_tx(const cyg_i2c_device *dev, 
               cyg_bool send_start, 
               const cyg_uint8 *tx_data, 
               cyg_uint32 count, 
               cyg_bool send_stop)
{
  i2c_lpc2xxx_delay(dev->i2c_delay);
  
  i2c_addr  = dev->i2c_address << 1;
  i2c_count = count;
  i2c_txbuf = tx_data;
  
  /*
   * for a repeated start the SI bit has to be reset
   * if we continue a previous transfer, load the next byte
   */
  if(send_start && i2c_flag == I2C_FLAG_ACT) {
    SET_CON(CON_STA);
    CLR_CON(CON_SI);
  } else if(send_start) {
    SET_CON(CON_STA);
  } else {
    I2C_W8(I2C_DAT, *i2c_txbuf);
    i2c_txbuf++;
    i2c_count--;
    CLR_CON(CON_SI);
  }
  
  i2c_flag  = 0;
  
  /*
   * the isr will do most of the work, and the dsr will signal when an
   * error occured or the transfer finished
   */
  cyg_drv_mutex_lock(&i2c_lock);
  cyg_drv_dsr_lock();
  cyg_drv_interrupt_unmask(I2C_INTR);
  while(!(i2c_flag & (I2C_FLAG_FINISH|I2C_FLAG_ERROR)))
    cyg_drv_cond_wait(&i2c_wait);
  cyg_drv_interrupt_mask(I2C_INTR);
  cyg_drv_dsr_unlock();
  cyg_drv_mutex_unlock(&i2c_lock);
  
  /* too bad we have no way to tell the caller */
  if(i2c_flag & I2C_FLAG_ERROR)
    diag_printf("I2C TX error flag: %x\n", i2c_flag);
  
  if(send_stop) {
    SET_CON(CON_STO);
    CLR_CON(CON_SI | CON_STA);
  } else  i2c_flag = I2C_FLAG_ACT;
  
  count -= i2c_count;
  
  i2c_addr  = 0;
  i2c_count = 0;
  i2c_txbuf = NULL;
  
  return count;
}

/*
 * receive into a buffer from a device
 */
static cyg_uint32
i2c_lpc2xxx_rx(const cyg_i2c_device *dev,
               cyg_bool send_start,
               cyg_uint8 *rx_data,
               cyg_uint32 count,
               cyg_bool send_nak,
               cyg_bool send_stop)
{
  i2c_lpc2xxx_delay(dev->i2c_delay);
  
  i2c_addr  = dev->i2c_address << 1 | 1;
  i2c_count = count;
  i2c_rxbuf = rx_data;
  i2c_rxnak = send_nak;
  
  /*
   * for a repeated start the SI bit has to be reset
   * if we continue a previous transfer, start reception
   */
  if(send_start && i2c_flag == I2C_FLAG_ACT) {
    SET_CON(CON_STA);
    CLR_CON(CON_SI);
  } else if(send_start)
    SET_CON(CON_STA);
  
  i2c_flag  = 0;
  
  /*
   * the isr will do most of the work, and the dsr will signal when an
   * error occured or the transfer finished
   */
  cyg_drv_mutex_lock(&i2c_lock);
  cyg_drv_dsr_lock();
  cyg_drv_interrupt_unmask(I2C_INTR);
  while(!(i2c_flag & (I2C_FLAG_FINISH|I2C_FLAG_ERROR)))
    cyg_drv_cond_wait(&i2c_wait);
  cyg_drv_interrupt_mask(I2C_INTR);
  cyg_drv_dsr_unlock();
  cyg_drv_mutex_unlock(&i2c_lock);
  
  /* too bad we have no way to tell the caller */
  if(i2c_flag & I2C_FLAG_ERROR)
    diag_printf("I2C RX error flag: %x\n", i2c_flag);
  
  if(send_stop) {
    SET_CON(CON_STO);
    CLR_CON(CON_SI | CON_STA);
  } else  i2c_flag = I2C_FLAG_ACT;
  
  count -= i2c_count;
  
  i2c_addr  = 0;
  i2c_count = 0;
  i2c_rxbuf = NULL;
  
  return count;
  
}


/*
 * generate a STOP
 */
static void
i2c_lpc2xxx_stop(const cyg_i2c_device *dev)
{
  SET_CON(CON_STO);
}

CYG_I2C_BUS(cyg_i2c_lpc2xxx_bus,
            &i2c_lpc2xxx_init,
            &i2c_lpc2xxx_tx,
            &i2c_lpc2xxx_rx,
            &i2c_lpc2xxx_stop,
            (void *) 0);
