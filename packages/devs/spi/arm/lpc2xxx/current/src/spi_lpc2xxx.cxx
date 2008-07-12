//==========================================================================
//
//      spi_lpc2xxx.cxx
//
//      SPI driver for LPC2xxx
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

#include <pkgconf/hal.h>
#include <pkgconf/io_spi.h>
#include <pkgconf/devs_spi_arm_lpc2xxx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/spi.h>
#include <cyg/io/spi_lpc2xxx.h>
#include <cyg/error/codes.h>

#define SPI_SPCR_SPIE 0x80
#define SPI_SPCR_LSBF 0x40
#define SPI_SPCR_MSTR 0x20
#define SPI_SPCR_CPOL 0x10
#define SPI_SPCR_CPHA 0x08

#define SPI_SPSR_SPIF 0x80
#define SPI_SPSR_WCOL 0x40
#define SPI_SPSR_ROVR 0x20
#define SPI_SPSR_MODF 0x10
#define SPI_SPSR_ABRT 0x08

#define SPI_SPINT     0x01

#ifdef CYGPKG_DEVS_SPI_ARM_LPC2XXX_BUS0
cyg_spi_lpc2xxx_bus_t cyg_spi_lpc2xxx_bus0;
CYG_SPI_DEFINE_BUS_TABLE(cyg_spi_lpc2xxx_dev_t, 0);
#endif
#ifdef CYGPKG_DEVS_SPI_ARM_LPC2XXX_BUS1
cyg_spi_lpc2xxx_bus_t cyg_spi_lpc2xxx_bus1;
CYG_SPI_DEFINE_BUS_TABLE(cyg_spi_lpc2xxx_dev_t, 1);
#endif

/*
 * Interrupt routine
 * read & write the next byte until count reaches zero
 */
static cyg_uint32
spi_lpc2xxx_isr(cyg_vector_t vec, cyg_addrword_t data)
{
  cyg_spi_lpc2xxx_bus_t *bus = (cyg_spi_lpc2xxx_bus_t *) data;
  cyg_uint8 tmp;
  
  tmp = bus->spi_dev->spsr;
  
  if(tmp & SPI_SPSR_MODF)
    bus->spi_dev->spcr = bus->spi_dev->spcr | SPI_SPCR_MSTR;
  
  tmp = bus->spi_dev->spdr;
  
  if(bus->count) {
    if(bus->rx)
      *bus->rx++ = tmp;
    if(--bus->count) {
      bus->spi_dev->spint = SPI_SPINT;
      bus->spi_dev->spdr = bus->tx ? *bus->tx++ : 0;
      cyg_drv_interrupt_acknowledge(bus->spi_vect);
      return CYG_ISR_HANDLED;
    }
  }
  
  bus->count = 0;
  bus->tx = NULL;
  bus->rx = NULL;
  
  bus->spi_dev->spint = SPI_SPINT;
  cyg_drv_interrupt_acknowledge(bus->spi_vect);
  return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}

static void 
spi_lpc2xxx_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
  cyg_drv_cond_signal(&((cyg_spi_lpc2xxx_bus_t *) data)->spi_wait);
}


/*
 * Configure bus for a specific baud rate
 */
static void
spi_lpc2xxx_baud(cyg_spi_lpc2xxx_bus_t *bus, cyg_uint32 baud)
{
  cyg_uint32 ccr = 8;
  
  if(baud) {
    ccr = (CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED 
           / CYGNUM_HAL_ARM_LPC2XXX_VPBDIV) / baud;
    if(((CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED 
         / CYGNUM_HAL_ARM_LPC2XXX_VPBDIV) / ccr) > baud)
      ccr++;
    ccr++;
    ccr &= 0xfe;
  }
  
  bus->spi_dev->spccr = ccr < 8 ? 8 : ccr;
}

/*
 * get/set configuration
 */
static int
spi_lpc2xxx_get_config(cyg_spi_device *device, cyg_uint32 key, void *buf, 
                       cyg_uint32 *len)
{
  cyg_spi_lpc2xxx_dev_t *dev = (cyg_spi_lpc2xxx_dev_t *) device;
  
  switch(key) {
    case CYG_IO_GET_CONFIG_SPI_CLOCKRATE:
      if(*len == sizeof(cyg_uint32)) {
        cyg_uint32 *b = (cyg_uint32 *) buf;
        *b = dev->spi_baud;
      } else return -EINVAL;
      break;
    default:
      return -EINVAL;
  }
  
  return ENOERR;
}

static int
spi_lpc2xxx_set_config(cyg_spi_device *device, cyg_uint32 key, const void *buf, 
                       cyg_uint32 *len)
{
  cyg_spi_lpc2xxx_dev_t *dev = (cyg_spi_lpc2xxx_dev_t *) device;
  
  switch(key) {
    case CYG_IO_GET_CONFIG_SPI_CLOCKRATE:
      if(*len == sizeof(cyg_uint32)) {
        dev->spi_baud = * (cyg_uint32 *) buf;
        spi_lpc2xxx_baud((cyg_spi_lpc2xxx_bus_t *) dev->spi_device.spi_bus, 
                         dev->spi_baud);
      }
      else return -EINVAL;
      break;
    default:
      return -EINVAL;
  }
  
  return ENOERR;
}


/*
 * Begin transaction
 * configure bus for device and drive CS by calling device cs() function
 */
static void
spi_lpc2xxx_begin(cyg_spi_device *device)
{
  cyg_spi_lpc2xxx_dev_t *dev = (cyg_spi_lpc2xxx_dev_t *) device;
  cyg_spi_lpc2xxx_bus_t *bus = 
    (cyg_spi_lpc2xxx_bus_t *) dev->spi_device.spi_bus;
  
  cyg_uint8 cr = 
    (dev->spi_cpha ? SPI_SPCR_CPHA : 0) |
    (dev->spi_cpol ? SPI_SPCR_CPOL : 0) |
    (dev->spi_lsbf ? SPI_SPCR_LSBF : 0);
  
  bus->spi_dev->spcr = SPI_SPCR_MSTR | cr;
  
  spi_lpc2xxx_baud(bus, dev->spi_baud);
  
  dev->spi_cs(1);
}


/*
 * Transfer a buffer to a device,
 * fill another buffer with data from the device
 */
static void
spi_lpc2xxx_transfer(cyg_spi_device *device, cyg_bool polled, cyg_uint32 count,
                     const cyg_uint8 *tx_data, cyg_uint8 *rx_data,
                     cyg_bool drop_cs)
{
  cyg_spi_lpc2xxx_dev_t *dev = (cyg_spi_lpc2xxx_dev_t *) device;
  cyg_spi_lpc2xxx_bus_t *bus = 
    (cyg_spi_lpc2xxx_bus_t *) dev->spi_device.spi_bus;
  cyg_uint8 tmp;
  
  if(!count) return;
  
  if(!polled) {
    bus->count = count;
    bus->tx = tx_data;
    bus->rx = rx_data;
    
    bus->spi_dev->spcr |= SPI_SPCR_SPIE;
    bus->spi_dev->spdr = bus->tx ? *bus->tx++ : 0;
    
    cyg_drv_mutex_lock(&bus->spi_lock);
    cyg_drv_dsr_lock();
    cyg_drv_interrupt_unmask(bus->spi_vect);
    while(bus->count)
      cyg_drv_cond_wait(&bus->spi_wait);
    cyg_drv_interrupt_mask(bus->spi_vect);
    cyg_drv_dsr_unlock();
    cyg_drv_mutex_unlock(&bus->spi_lock);
  } else do {
      bus->spi_dev->spdr = tx_data ? *tx_data++ : 0;
      while(!(bus->spi_dev->spsr & SPI_SPSR_SPIF));
      tmp = bus->spi_dev->spdr;
      if(rx_data)
        *rx_data++ = tmp;
      count--;
    } while(count);
  
  if(drop_cs)
    dev->spi_cs(0);
  
  return;
}


/*
 * Tick
 */
static void
spi_lpc2xxx_tick(cyg_spi_device *device, cyg_bool polled, cyg_uint32 count)
{
  spi_lpc2xxx_transfer(device, polled, count, NULL, NULL, false);
}


/*
 * End transaction
 * disable SPI bus, drop CS, reset transfer variables
 */
static void
spi_lpc2xxx_end(cyg_spi_device *device)
{
  cyg_spi_lpc2xxx_dev_t *dev = (cyg_spi_lpc2xxx_dev_t *) device;
  cyg_spi_lpc2xxx_bus_t *bus = 
    (cyg_spi_lpc2xxx_bus_t *) dev->spi_device.spi_bus;
  
  bus->spi_dev->spcr = 0;
  dev->spi_cs(0);
  
  bus->count = 0;
  bus->tx = NULL;
  bus->rx = NULL;
}


/*
 * Driver & bus initialization
 */
static void 
spi_lpc2xxx_init_bus(cyg_spi_lpc2xxx_bus_t *bus, 
                     cyg_addrword_t dev,
                     cyg_vector_t vec)
{
  bus->spi_bus.spi_transaction_begin    = spi_lpc2xxx_begin;
  bus->spi_bus.spi_transaction_transfer = spi_lpc2xxx_transfer;
  bus->spi_bus.spi_transaction_tick     = spi_lpc2xxx_tick;
  bus->spi_bus.spi_transaction_end      = spi_lpc2xxx_end;
  bus->spi_bus.spi_get_config           = spi_lpc2xxx_get_config;
  bus->spi_bus.spi_set_config           = spi_lpc2xxx_set_config;
  CYG_SPI_BUS_COMMON_INIT(&bus->spi_bus);
  
  cyg_drv_mutex_init(&bus->spi_lock);
  cyg_drv_cond_init(&bus->spi_wait, &bus->spi_lock);
  
  bus->spi_dev = (struct spi_dev *) dev;
  bus->spi_vect = vec;
  cyg_drv_interrupt_create(
                           vec, 0, (cyg_addrword_t) bus,
                           &spi_lpc2xxx_isr, &spi_lpc2xxx_dsr,
                           &bus->spi_hand, &bus->spi_intr);
  cyg_drv_interrupt_attach(bus->spi_hand);
}

/*
 * initialization class
 */
class cyg_spi_lpc2xxx_init_class {
public:
  cyg_spi_lpc2xxx_init_class(void) {
    cyg_uint32 addr, tmp;
    
#ifdef CYGPKG_DEVS_SPI_ARM_LPC2XXX_BUS0
    addr = (CYGARC_HAL_LPC2XXX_REG_PIN_BASE
            + CYGARC_HAL_LPC2XXX_REG_PINSEL0);
    HAL_READ_UINT32(addr, tmp);
    tmp |= 0x5500;
    HAL_WRITE_UINT32(addr, tmp);
    
    spi_lpc2xxx_init_bus(&cyg_spi_lpc2xxx_bus0,
                         CYGARC_HAL_LPC2XXX_REG_SPI0_BASE,
                         CYGNUM_HAL_INTERRUPT_SPI0);
#endif
#ifdef CYGPKG_DEVS_SPI_ARM_LPC2XXX_BUS1
    addr = (CYGARC_HAL_LPC2XXX_REG_PIN_BASE
            + CYGARC_HAL_LPC2XXX_REG_PINSEL1);
    HAL_READ_UINT32(addr, tmp);
    tmp |= 0x2a8;
    HAL_WRITE_UINT32(addr, tmp);
    spi_lpc2xxx_init_bus(&cyg_spi_lpc2xxx_bus1,
                         CYGARC_HAL_LPC2XXX_REG_SPI1_BASE,
                         CYGNUM_HAL_INTERRUPT_SPI1);
#endif
  }
};

static cyg_spi_lpc2xxx_init_class spi_lpc2xxx_init 
    CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_IO);

