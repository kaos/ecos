#ifndef CYGONCE_DEVS_SPI_ARM_LPC2XXX_H
#define CYGONCE_DEVS_SPI_ARM_LPC2XXX_H

//==========================================================================
//
//      spi_lpc2xxx.h
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
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>

struct spi_dev {
  volatile cyg_uint32 spcr;
  volatile cyg_uint32 spsr;
  volatile cyg_uint32 spdr;
  volatile cyg_uint32 spccr;
  cyg_uint32 d1, d2, d3;
  volatile cyg_uint32 spint;
};

typedef struct {
  cyg_spi_bus     spi_bus;
  
  cyg_interrupt   spi_intr;
  cyg_handle_t    spi_hand;
  cyg_vector_t    spi_vect;
  cyg_drv_mutex_t spi_lock;
  cyg_drv_cond_t  spi_wait;
  
  struct spi_dev *spi_dev;
  
  volatile cyg_uint32       count;
  volatile const cyg_uint8 *tx;
  volatile cyg_uint8       *rx;
} cyg_spi_lpc2xxx_bus_t;

typedef struct {
  cyg_spi_device  spi_device;
  
  cyg_uint8       spi_cpha;
  cyg_uint8       spi_cpol;
  cyg_uint8       spi_lsbf;
  cyg_uint32      spi_baud;
  
  void            (*spi_cs)(int);
} cyg_spi_lpc2xxx_dev_t;

#ifdef CYGPKG_DEVS_SPI_ARM_LPC2XXX_BUS0
externC cyg_spi_lpc2xxx_bus_t cyg_spi_lpc2xxx_bus0;
#endif

#ifdef CYGPKG_DEVS_SPI_ARM_LPC2XXX_BUS1
externC cyg_spi_lpc2xxx_bus_t cyg_spi_lpc2xxx_bus1;
#endif

#endif
