/*==========================================================================
//
//      stm3210e_eval_eth_enc424j600.c
//
//      Setup for optional enc424j600 Ethernet over SPI
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2012 Free Software Foundation, Inc.
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
// Author(s):    Ilija Stanislevik
// Date:         2012-04-05
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <cyg/hal/hal_io.h>

#include <cyg/io/spi.h>
#include <cyg/io/spi_stm32.h>
#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>
#include <cyg/io/eth/enc424j600_eth.h>

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_ass.h>          // assertion macros

// Set to: 1 for diagnostic printouts; 0 for no diagnostic printouts.
#define DEBUG_ENC424J600_PLATFORM_INIT 0

// SPI device for communication with enc424j600 Ethernet chip
#define TRANS8B            false
#define CLK_IDLE_LOW        0
#define CLK_PHASE_RISING    0

#define AUX_SPI_CORTEXM_STM32_DEVICE(_name_, _bus_)     \
    CYG_DEVS_SPI_CORTEXM_STM32_DEVICE(                  \
        _name_##_spi,                                   \
        _bus_,                                          \
        CYGHWR_HAL_CORTEXM_STM32_SPIETH_SPI_CS,         \
        TRANS8B,                                        \
        CLK_IDLE_LOW,                                   \
        CLK_PHASE_RISING,                               \
        14000000,                                       \
        1,                                              \
        1,                                              \
        1                                               \
    )

AUX_SPI_CORTEXM_STM32_DEVICE
(
    CYGDAT_IO_ETH_ENC424J600_NAME,
    CYGHWR_HAL_CORTEXM_STM32_SPIETH_SPI_BUS
);

// Code to initialize enc424j600 driver data structure.

void cyg_devs_cortexm_stm3210e_enc424j600_init( struct cyg_netdevtab_entry * netdevtab_entry)
{
    struct eth_drv_sc *eth_inst = NULL;          // pointer to device instance
    enc424j600_priv_data_t *eth_inst_pd = NULL;  // device's private data
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_uint32 cr;
    cyg_uint32 backupr;
#endif

//  Assign SPI device to Ethernet device.
    eth_inst = netdevtab_entry->device_instance;
    eth_inst_pd = (enc424j600_priv_data_t *)eth_inst->driver_private;

#if DEBUG_ENC424J600_PLATFORM_INIT & 1
    diag_printf("%s(): Assigning SPI device to Ethernet device %s.\n",__FUNCTION__, netdevtab_entry->name);
#endif
    eth_inst_pd->spi_service_device = (cyg_spi_device *) &CYGDAT_IO_ETH_ENC424J600_NAME_spi_stm32;

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED

// Interrupt output from enc424j600 is connected to one of the pins from ports A-G.
// Here this pin is marshaled to External Interrupt Controller (EXTI).

#define ENC424J600_INTERRUPT_SOURCE_PIN  CYGHWR_HAL_SPIETH_INTERRUPT_PIN
#define ENC424J600_EXTICR                ((ENC424J600_INTERRUPT_SOURCE_PIN / 4) * 4 + 8)
#define ENC424J600_SHIFT_VALUE           ((ENC424J600_INTERRUPT_SOURCE_PIN % 4) * 4)

#if DEBUG_ENC424J600_PLATFORM_INIT & 1
    diag_printf("%s(): Mapping external interrupt from P%c%d.\n",__FUNCTION__,
                CYGHWR_HAL_CORTEXM_STM32_SPIETH_INTERRUPT_PORT,
                ENC424J600_INTERRUPT_SOURCE_PIN);
#endif

    // Is AFIO clock enabled?
    HAL_READ_UINT32(CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_APB2ENR , backupr );
    if (0 == (backupr & BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_AFIO)))
    {
        CYGHWR_HAL_STM32_CLOCK_ENABLE(CYGHWR_HAL_STM32_CLOCK(APB2,AFIO));
    }

    // Modify External Interrupt Control Register
    HAL_READ_UINT32(CYGHWR_HAL_STM32_AFIO + ENC424J600_EXTICR , cr );
    cr |= ((cyg_uint32)0xf << ENC424J600_SHIFT_VALUE);
    cr &= (((cyg_uint32)(CYGHWR_HAL_CORTEXM_STM32_SPIETH_INTERRUPT_PORT - 'A')
                            << ENC424J600_SHIFT_VALUE) & 0xffff);
    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_AFIO + ENC424J600_EXTICR , cr );

    // Restore AFIO clock
    if (0 == (backupr & BIT_(CYGHWR_HAL_STM32_RCC_APB2ENR_AFIO)))
    {
        CYGHWR_HAL_STM32_CLOCK_DISABLE(CYGHWR_HAL_STM32_CLOCK(APB2,AFIO));
    }

#endif  //  #ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
}

//==========================================================================
// EOF stm3210e_eval_eth_enc424j600.c
