//=============================================================================
//
//      spi_a2fxxx.c
//
//      SPI driver implementation for Smartfusion Cortex-M3
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   ccoutand, updated for Smartfusion Cortex-M3
// Original(s): Chris Holgate
// Date:        2011-04-25
// Purpose:     Smartfusion Cortex-M3 SPI driver implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include <cyg/io/spi.h>
#include <cyg/io/spi_a2fxxx.h>

#include <pkgconf/devs_spi_cortexm_a2fxxx.h>

#include <string.h>

#if defined(CYGHWR_DEVS_SPI_CORTEXM_A2FXXX_BUS1) || \
      defined(CYGHWR_DEVS_SPI_CORTEXM_A2FXXX_BUS2)

#ifdef CYGDBG_DEVS_SPI_CORTEXM_A2FXXX_TRACE
# define SPI_TRACE(args...) diag_printf(args)
#else
# define SPI_TRACE(args...)
#endif

//-----------------------------------------------------------------------------
// API function call forward references.

static void a2fxxx_transaction_begin    (cyg_spi_device*);
static void a2fxxx_transaction_transfer (cyg_spi_device*, cyg_bool, cyg_uint32, const cyg_uint8*, cyg_uint8*, cyg_bool);
static void a2fxxx_transaction_tick     (cyg_spi_device*, cyg_bool, cyg_uint32);
static void a2fxxx_transaction_end      (cyg_spi_device*);
static int  a2fxxx_get_config           (cyg_spi_device*, cyg_uint32, void*, cyg_uint32*);
static int  a2fxxx_set_config           (cyg_spi_device*, cyg_uint32, const void*, cyg_uint32*);

//-----------------------------------------------------------------------------
// Instantiate the bus state data structures.

#ifdef CYGHWR_DEVS_SPI_CORTEXM_A2FXXX_BUS1

static cyg_haladdress rx1_dma_null = 0x0;
static cyg_haladdress tx1_dma_null = 0x0;

static const cyg_spi_cortexm_a2fxxx_bus_setup_t bus1_setup = {
    .spi_reg_base                     = CYGHWR_HAL_A2FXXX_SPI0,
    .dma_reg_base                     = CYGHWR_HAL_A2FXXX_DMA,
    .dma_tx_channel                   = CYGNUM_DEVS_SPI_CORTEXM_A2FXXX_BUS1_TX_DMA,
    .dma_rx_channel                   = CYGNUM_DEVS_SPI_CORTEXM_A2FXXX_BUS1_RX_DMA,
#ifdef CYGDAT_DEVS_SPI_CORTEXM_A2FXXX_BUS1_TX_DMA_PRI_HIGH
    .dma_tx_pri                       = true,
#else
    .dma_tx_pri                       = false,
#endif
#ifdef CYGDAT_DEVS_SPI_CORTEXM_A2FXXX_BUS1_RX_DMA_PRI_HIGH
    .dma_rx_pri                       = true,
#else
    .dma_rx_pri                       = false,
#endif
    .rx_dma_null                      = &rx1_dma_null,
    .tx_dma_null                      = &tx1_dma_null,
};

cyg_spi_cortexm_a2fxxx_bus_t cyg_spi_a2fxxx_bus1 = {
    .spi_bus.spi_transaction_begin    = a2fxxx_transaction_begin,
    .spi_bus.spi_transaction_transfer = a2fxxx_transaction_transfer,
    .spi_bus.spi_transaction_tick     = a2fxxx_transaction_tick,
    .spi_bus.spi_transaction_end      = a2fxxx_transaction_end,
    .spi_bus.spi_get_config           = a2fxxx_get_config,
    .spi_bus.spi_set_config           = a2fxxx_set_config,
    .setup                            = &bus1_setup,
    .cs_up                            = false
};
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_A2FXXX_BUS2

static cyg_haladdress rx2_dma_null = 0x0;
static cyg_haladdress tx2_dma_null = 0x0;

static const cyg_spi_cortexm_a2fxxx_bus_setup_t bus2_setup = {
    .spi_reg_base                     = CYGHWR_HAL_A2FXXX_SPI1,
    .dma_reg_base                     = CYGHWR_HAL_A2FXXX_DMA,
    .dma_tx_channel                   = CYGNUM_DEVS_SPI_CORTEXM_A2FXXX_BUS2_TX_DMA,
    .dma_rx_channel                   = CYGNUM_DEVS_SPI_CORTEXM_A2FXXX_BUS2_RX_DMA,
#ifdef CYGDAT_DEVS_SPI_CORTEXM_A2FXXX_BUS2_TX_DMA_PRI_HIGH
    .dma_tx_pri                       = true,
#else
    .dma_tx_pri                       = false,
#endif
#ifdef CYGDAT_DEVS_SPI_CORTEXM_A2FXXX_BUS2_RX_DMA_PRI_HIGH
    .dma_rx_pri                       = true,
#else
    .dma_rx_pri                       = false,
#endif
    .rx_dma_null                      = &rx2_dma_null,
    .tx_dma_null                      = &tx2_dma_null,
};

cyg_spi_cortexm_a2fxxx_bus_t cyg_spi_a2fxxx_bus2 = {
    .spi_bus.spi_transaction_begin    = a2fxxx_transaction_begin,
    .spi_bus.spi_transaction_transfer = a2fxxx_transaction_transfer,
    .spi_bus.spi_transaction_tick     = a2fxxx_transaction_tick,
    .spi_bus.spi_transaction_end      = a2fxxx_transaction_end,
    .spi_bus.spi_get_config           = a2fxxx_get_config,
    .spi_bus.spi_set_config           = a2fxxx_set_config,
    .setup                            = &bus2_setup,
    .cs_up                            = false
};
#endif


//-----------------------------------------------------------------------------
// Implement DMA ISRs.  These clear the DMA channel interrupts and
// schedule their respective DSRs.

static cyg_uint32 a2fxxx_tx_ISR
  (cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                                  (cyg_spi_cortexm_a2fxxx_bus_t*) data;
    cyg_uint8 chan = a2fxxx_bus->setup->dma_tx_channel;
    SPI_TRACE("SPI : Handle TX ISR\n");
    a2fxxx_dma_clear_interrupt ( chan );
    return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

static cyg_uint32 a2fxxx_rx_ISR
  (cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                                  (cyg_spi_cortexm_a2fxxx_bus_t*) data;
    cyg_uint8 chan = a2fxxx_bus->setup->dma_rx_channel;
    SPI_TRACE("SPI : Handle RX ISR\n");
    a2fxxx_dma_clear_interrupt ( chan );
    return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

//-----------------------------------------------------------------------------
// Implement DMA DSRs.  These clear down the interrupt conditions and assert
// their respective 'transaction complete' flags.

static void a2fxxx_tx_DSR
  (cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                                  (cyg_spi_cortexm_a2fxxx_bus_t*) data;
    SPI_TRACE("SPI : Handle TX DSR\n");
    cyg_drv_dsr_lock ();
    a2fxxx_bus->tx_dma_done = true;
    cyg_drv_cond_signal (&a2fxxx_bus->condvar);
    cyg_drv_dsr_unlock ();
}

static void a2fxxx_rx_DSR
  (cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                                  (cyg_spi_cortexm_a2fxxx_bus_t*) data;
    SPI_TRACE("SPI : Handle RX DSR\n");
    cyg_drv_dsr_lock ();
    a2fxxx_bus->rx_dma_done = true;
    cyg_drv_cond_signal (&a2fxxx_bus->condvar);
    cyg_drv_dsr_unlock ();
}

//-----------------------------------------------------------------------------
// Set up a new SPI bus on initialization.

static void a2fxxx_spi_bus_setup
  (cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus)
{
    cyg_uint8 dma_tx_type, dma_rx_type;

    if( a2fxxx_bus->setup->spi_reg_base == CYGHWR_HAL_A2FXXX_SPI1 ){
        CYGHWR_HAL_A2FXXX_GPIO_SET( CYGHWR_HAL_A2FXXX_SPI1_DO );
        CYGHWR_HAL_A2FXXX_GPIO_SET( CYGHWR_HAL_A2FXXX_SPI1_DI );
        CYGHWR_HAL_A2FXXX_GPIO_SET( CYGHWR_HAL_A2FXXX_SPI1_CLK );
        dma_tx_type = CYGHWR_HAL_A2FXXX_DMA_XFER(TO_SPI1);
        dma_rx_type = CYGHWR_HAL_A2FXXX_DMA_XFER(FROM_SPI1);
    }
    else {
        CYGHWR_HAL_A2FXXX_GPIO_SET( CYGHWR_HAL_A2FXXX_SPI0_DO );
        CYGHWR_HAL_A2FXXX_GPIO_SET( CYGHWR_HAL_A2FXXX_SPI0_DI );
        CYGHWR_HAL_A2FXXX_GPIO_SET( CYGHWR_HAL_A2FXXX_SPI0_CLK );
        dma_tx_type = CYGHWR_HAL_A2FXXX_DMA_XFER(TO_SPI0);
        dma_rx_type = CYGHWR_HAL_A2FXXX_DMA_XFER(FROM_SPI0);
    }

    // Initialize the synchronization primitives.
    cyg_drv_mutex_init (&a2fxxx_bus->mutex);
    cyg_drv_cond_init (&a2fxxx_bus->condvar, &a2fxxx_bus->mutex);

    // Setup DMA channel
    a2fxxx_dma_ch_setup(a2fxxx_bus->setup->dma_tx_channel, dma_tx_type,
             CYGHWR_HAL_A2FXXX_DMA_OUTBOUND, sizeof(cyg_uint8), 0,
             a2fxxx_bus->setup->dma_tx_pri, 9);
    a2fxxx_dma_ch_setup(a2fxxx_bus->setup->dma_rx_channel, dma_rx_type,
             CYGHWR_HAL_A2FXXX_DMA_INBOUND, 0, sizeof(cyg_uint8),
             a2fxxx_bus->setup->dma_rx_pri, 9);

    // Register ISR /DSR
    a2fxxx_dma_ch_attach(a2fxxx_bus->setup->dma_tx_channel,
              a2fxxx_tx_ISR, a2fxxx_tx_DSR, (cyg_addrword_t) a2fxxx_bus);

    a2fxxx_dma_ch_attach(a2fxxx_bus->setup->dma_rx_channel,
              a2fxxx_rx_ISR, a2fxxx_rx_DSR, (cyg_addrword_t) a2fxxx_bus);

    // Call upper layer bus init.
    CYG_SPI_BUS_COMMON_INIT(&a2fxxx_bus->spi_bus);
}

//-----------------------------------------------------------------------------
// Drive a GPIO pin as a SPI chip select line.

static inline void a2fxxx_spi_chip_select
  (cyg_spi_device* device, cyg_bool assert)
{
    cyg_spi_cortexm_a2fxxx_device_t* a2fxxx_device =
                  (cyg_spi_cortexm_a2fxxx_device_t*) device;
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                  (cyg_spi_cortexm_a2fxxx_bus_t*) device->spi_bus;

    // CS is driven from GPIO
    if( a2fxxx_device->cs_gpio == true) {
        if (assert == true)
            CYGHWR_HAL_A2FXXX_GPIO_OUT( a2fxxx_device->cs_gpio_n, 0 );
        else
            CYGHWR_HAL_A2FXXX_GPIO_OUT( a2fxxx_device->cs_gpio_n, 1 );
    }
    // CS is driven from controller
    else {
        cyg_uint32 reg_addr = a2fxxx_bus->setup->spi_reg_base +
                                        CYGHWR_HAL_A2FXXX_SPI_SLAVE_SEL;
        cyg_uint32 reg_data;
        HAL_READ_UINT32 (reg_addr, reg_data);
        if (assert == true)
            reg_data |= CYGHWR_HAL_A2FXXX_SPI_CS_SEL(a2fxxx_device->dev_num);
        else
            reg_data &= ~CYGHWR_HAL_A2FXXX_SPI_CS_SEL(a2fxxx_device->dev_num);
        HAL_WRITE_UINT32 (reg_addr, reg_data);
    }
}

//-----------------------------------------------------------------------------
// Configure build-in SPI chip select line

static inline void a2fxxx_spi_set_ss
  (cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus, cyg_uint8 ss_num)
{
    cyg_uint32 ss;

    if( a2fxxx_bus->setup->spi_reg_base == CYGHWR_HAL_A2FXXX_SPI0 ){
        if ( ss_num == 0 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI0_SS0;
        } else if( ss_num == 1 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI0_SS1;
        } else if( ss_num == 2 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI0_SS2;
        } else if( ss_num == 3 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI0_SS3;
        } else {
            CYG_ASSERT (ss < 4, "SPI : SPI0 SS out of range.");
        }
    }
    else {
        if ( ss_num == 0 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS0;
        } else if( ss_num == 1 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS1;
        } else if( ss_num == 2 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS2;
        } else if( ss_num == 3 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS3;
        } else if( ss_num == 4 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS4;
        } else if( ss_num == 5 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS5;
        } else if( ss_num == 6 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS6;
        } else if( ss_num == 7 ) {
            ss = CYGHWR_HAL_A2FXXX_SPI1_SS7;
        } else {
            CYG_ASSERT (ss < 8, "SPI : SPI1 SS out of range.");
        }
    }

    CYGHWR_HAL_A2FXXX_GPIO_SET( ss );
}

//-----------------------------------------------------------------------------
// Initiate a DMA transfer over the SPI interface.

static void spi_transaction_dma
  (cyg_spi_device* device, cyg_bool tick_only, cyg_bool polled, cyg_uint32 count,
  const cyg_uint8* tx_data, cyg_uint8* rx_data, cyg_bool drop_cs)
{
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                (cyg_spi_cortexm_a2fxxx_bus_t*) device->spi_bus;
    cyg_spi_cortexm_a2fxxx_device_t* a2fxxx_device =
                (cyg_spi_cortexm_a2fxxx_device_t*) device;
    cyg_haladdress reg_addr;
    cyg_uint32 spi_cr_val = a2fxxx_device->spi_cr_val;
    cyg_haladdress spi_reg_base = a2fxxx_bus->setup->spi_reg_base;

    SPI_TRACE("SPI : Transfer start\n");

    // Drive chip select low, either from GPIO or build in SS signal
    if( !a2fxxx_bus->cs_up && !tick_only ){
        SPI_TRACE("SPI : Setup CS\n");
        CYGACC_CALL_IF_DELAY_US (a2fxxx_device->tr_bt_udly);
        a2fxxx_spi_chip_select (device, true);
        CYGACC_CALL_IF_DELAY_US (a2fxxx_device->cs_up_udly);
        a2fxxx_bus->cs_up = true;
    }

    // Disable the SPI controller.
    reg_addr = spi_reg_base + CYGHWR_HAL_A2FXXX_SPI_CTRL;
    HAL_WRITE_UINT32 (reg_addr, a2fxxx_device->spi_cr_val);

    // Write down transfer count
    spi_cr_val |= CYGHWR_HAL_A2FXXX_SPI_CTRL_COUNT( count );
    reg_addr = spi_reg_base + CYGHWR_HAL_A2FXXX_SPI_CTRL;
    HAL_WRITE_UINT32 (reg_addr, spi_cr_val);

    //
    // Setup DMA channels
    // For null source or destination pointer, replace the buffer with the SPI driver
    // dummy buffer and set the DMA address increment to 0
    //
    if(tx_data == NULL)
        a2fxxx_dma_update_incr(a2fxxx_bus->setup->dma_tx_channel, false, 0);
    else
        a2fxxx_dma_update_incr(a2fxxx_bus->setup->dma_tx_channel, false,
                                                          sizeof(cyg_uint8));

    if(rx_data == NULL)
        a2fxxx_dma_update_incr(a2fxxx_bus->setup->dma_rx_channel, true, 0);
    else
        a2fxxx_dma_update_incr(a2fxxx_bus->setup->dma_rx_channel, true,
                                                          sizeof(cyg_uint8));

    a2fxxx_bus->tx_dma_done = false;
    a2fxxx_dma_xfer (a2fxxx_bus->setup->dma_tx_channel, polled, count,
         ((tx_data == NULL) ? (cyg_uint8*) a2fxxx_bus->setup->tx_dma_null :
          (cyg_uint8*) tx_data),
          (cyg_uint8*)(a2fxxx_bus->setup->spi_reg_base + CYGHWR_HAL_A2FXXX_SPI_TX));

    a2fxxx_bus->rx_dma_done = false;
    a2fxxx_dma_xfer (a2fxxx_bus->setup->dma_rx_channel, polled, count,
          (cyg_uint8*)(a2fxxx_bus->setup->spi_reg_base + CYGHWR_HAL_A2FXXX_SPI_RX),
         ((rx_data == NULL) ? (cyg_uint8*) a2fxxx_bus->setup->rx_dma_null :
          (cyg_uint8*) rx_data));

    //
    // Run the DMA (polling for completion).
    //
    if (polled)
    {
        // Enable the SPI controller.
        SPI_TRACE("SPI : Control Register 0x%x, 0x%x, %d\n", reg_addr,
                   spi_cr_val, count);
        HAL_WRITE_UINT32 (reg_addr, spi_cr_val | CYGHWR_HAL_A2FXXX_SPI_CTRL_EN);

        // Wait transfer completed.
        do {
            if(a2fxxx_dma_get_comp_flag(a2fxxx_bus->setup->dma_rx_channel))
                a2fxxx_bus->rx_dma_done = true;
            if(a2fxxx_dma_get_comp_flag(a2fxxx_bus->setup->dma_tx_channel))
                a2fxxx_bus->tx_dma_done = true;
        } while ( !(a2fxxx_bus->tx_dma_done && a2fxxx_bus->rx_dma_done) );

        // Acknowledge transfer
        a2fxxx_dma_clear_interrupt (a2fxxx_bus->setup->dma_rx_channel);
        a2fxxx_dma_clear_interrupt (a2fxxx_bus->setup->dma_tx_channel);

    } else {
        cyg_drv_mutex_lock (&a2fxxx_bus->mutex);
        cyg_drv_dsr_lock ();

        // Enable the SPI controller.
        SPI_TRACE("SPI : Control Register 0x%x, 0x%x, %d\n", reg_addr,
                   spi_cr_val, count);
        HAL_WRITE_UINT32 (reg_addr, spi_cr_val | CYGHWR_HAL_A2FXXX_SPI_CTRL_EN);

        // Sit back and wait for the ISR/DSRs to signal completion.
        do {
            cyg_drv_cond_wait (&a2fxxx_bus->condvar);
        } while (!(a2fxxx_bus->tx_dma_done && a2fxxx_bus->rx_dma_done));

        cyg_drv_dsr_unlock ();
        cyg_drv_mutex_unlock (&a2fxxx_bus->mutex);
    }

    SPI_TRACE("SPI : Transfer completed\n");

    if (drop_cs && !tick_only) {
        SPI_TRACE("SPI : Release chip select\n");
        CYGACC_CALL_IF_DELAY_US (a2fxxx_device->cs_dw_udly);
        a2fxxx_spi_chip_select (device, false);
        a2fxxx_bus->cs_up = false;
    }
}

//-----------------------------------------------------------------------------
// Initialize SPI interfaces on startup.

static void CYGBLD_ATTRIB_C_INIT_PRI(CYG_INIT_BUS_SPI)
a2fxxx_spi_init(void)
{
    hal_dma_init();

#ifdef CYGHWR_DEVS_SPI_CORTEXM_A2FXXX_BUS1
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE( CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(SPI0) );
    a2fxxx_spi_bus_setup (&cyg_spi_a2fxxx_bus1);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_A2FXXX_BUS2
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE( CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(SPI1) );
    a2fxxx_spi_bus_setup (&cyg_spi_a2fxxx_bus2);
#endif
}

//-----------------------------------------------------------------------------
// Start a SPI transaction.

static void a2fxxx_transaction_begin
  (cyg_spi_device* device)
{
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                  (cyg_spi_cortexm_a2fxxx_bus_t*) device->spi_bus;
    cyg_spi_cortexm_a2fxxx_device_t* a2fxxx_device =
                  (cyg_spi_cortexm_a2fxxx_device_t*) device;

    cyg_haladdress reg_addr;
    cyg_uint32 reg_data, divided_clk, br, cs_gpio_n;

    SPI_TRACE("SPI : Transfer begin\n");
    SPI_TRACE("SPI : Device baud rate = %d\n",      a2fxxx_device->cl_brate);
    SPI_TRACE("SPI : Device clock polarity = %d\n", a2fxxx_device->cl_pol);
    SPI_TRACE("SPI : Device clock phase = %d\n",    a2fxxx_device->cl_pha);

    // On the first transaction, generate the values to be programmed into the
    // SPI configuration registers for this device and cache them.  This avoids
    // having to recalculate the values for every transaction.
    if (!a2fxxx_device->spi_cr_val) {

        // SPI bus protocol
        switch( a2fxxx_device->proto ) {
        case A2FXXX_SPI_TI_SYNC_SERIAL:
            reg_data = CYGHWR_HAL_A2FXXX_SPI_CTRL_PROTO_TI;
            break;
        case A2FXXX_SPI_NS_MICROWIRE:
            reg_data = CYGHWR_HAL_A2FXXX_SPI_CTRL_PROTO_NS;
            break;
        default:
            reg_data = CYGHWR_HAL_A2FXXX_SPI_CTRL_PROTO_MOTOROLA;
            break;
        }

        // Bus polarity
        if (a2fxxx_device->cl_pol)
            reg_data |= CYGHWR_HAL_A2FXXX_SPI_CTRL_SPO;
        if (a2fxxx_device->cl_pha)
            reg_data |= CYGHWR_HAL_A2FXXX_SPI_CTRL_SPH;

        // Master mode
        reg_data |= CYGHWR_HAL_A2FXXX_SPI_CTRL_MASTER;

        // Calculate the maximum viable bus speed.
        divided_clk = hal_a2fxxx_spi_clock(a2fxxx_bus->setup->spi_reg_base) >> 1;
        for (br = 0; (br < 7) && (divided_clk > a2fxxx_device->cl_brate); br++)
            divided_clk >>= 1;

        CYG_ASSERT (divided_clk <= a2fxxx_device->cl_brate,
            "A2FXXX SPI : Cannot run bus slowly enough for peripheral.");

        SPI_TRACE("SPI : Clock divider = %d\n", divided_clk);

        reg_addr = a2fxxx_bus->setup->spi_reg_base +
                   CYGHWR_HAL_A2FXXX_SPI_CLK_GEN;
        HAL_WRITE_UINT32 (reg_addr, br);

        // Set transfer size (byte mode)
        reg_addr = a2fxxx_bus->setup->spi_reg_base +
                   CYGHWR_HAL_A2FXXX_SPI_TXRXDF_SIZE;
        HAL_WRITE_UINT32 (reg_addr, 8);

        // Cache the configuration register settings.
        a2fxxx_device->spi_cr_val = reg_data;

        // Configure GPIO as chip select
        if(a2fxxx_device->cs_gpio == true) {
            cs_gpio_n = a2fxxx_device->cs_gpio_n;
            SPI_TRACE("SPI : Setup GPIO for CS => %d\n", cs_gpio_n);
            a2fxxx_device->cs_gpio_n =
                CYGHWR_HAL_A2FXXX_CS_GPIO( cs_gpio_n,  OUT, cs_gpio_n, DISABLE );
            CYGHWR_HAL_A2FXXX_GPIO_SET( a2fxxx_device->cs_gpio_n );
            CYGHWR_HAL_A2FXXX_GPIO_OUT( a2fxxx_device->cs_gpio_n, 1);
        }
        // Configure build-in SPI chip select line
        else {
           SPI_TRACE("SPI : Setup SPI SS => %d\n", a2fxxx_device->dev_num);
           a2fxxx_spi_set_ss(a2fxxx_bus, a2fxxx_device->dev_num);
        }
    }

    // Set up the SPI controller.
    reg_addr = a2fxxx_bus->setup->spi_reg_base + CYGHWR_HAL_A2FXXX_SPI_CTRL;
    HAL_WRITE_UINT32 (reg_addr, a2fxxx_device->spi_cr_val);
}

//-----------------------------------------------------------------------------
// Run a transaction transfer.

static void a2fxxx_transaction_transfer
  (cyg_spi_device* device, cyg_bool polled, cyg_uint32 count,
  const cyg_uint8* tx_data, cyg_uint8* rx_data, cyg_bool drop_cs)
{
    // Check for unsupported transactions.
    CYG_ASSERT (count > 0, "A2FXXX SPI : Null transfer requested.");

    // Perform transfer
    spi_transaction_dma (device, false, polled, count, tx_data, rx_data, drop_cs);
}

//-----------------------------------------------------------------------------
// Carry out a bus tick operation - this just pushes the required number of
// zeros onto the bus, leaving the chip select in its current state.

static void a2fxxx_transaction_tick
  (cyg_spi_device* device, cyg_bool polled, cyg_uint32 count)
{
    // Check for unsupported transactions.
    CYG_ASSERT (count > 0, "A2FXXX SPI : Null transfer requested.");

    SPI_TRACE("SPI : Transfer tick\n");

    // Perform null transfer.
    spi_transaction_dma (device, true, polled, count, NULL, NULL, false);
}

//-----------------------------------------------------------------------------
// Terminate a SPI transaction, disabling the SPI controller.

static void a2fxxx_transaction_end
  (cyg_spi_device* device)
{
    cyg_spi_cortexm_a2fxxx_bus_t* a2fxxx_bus =
                             (cyg_spi_cortexm_a2fxxx_bus_t*) device->spi_bus;
    cyg_spi_cortexm_a2fxxx_device_t* a2fxxx_device =
                                   (cyg_spi_cortexm_a2fxxx_device_t*) device;
    cyg_haladdress reg_addr;

    SPI_TRACE("SPI : Transfer end\n");

    // De-assert chip select
    if( a2fxxx_bus->cs_up == true ){
        SPI_TRACE("SPI : Release chip select\n");
        CYGACC_CALL_IF_DELAY_US (a2fxxx_device->cs_dw_udly);
        a2fxxx_spi_chip_select (device, false);
        a2fxxx_bus->cs_up = false;
    }

    // Disable controller
    reg_addr = a2fxxx_bus->setup->spi_reg_base + CYGHWR_HAL_A2FXXX_SPI_CTRL;
    HAL_WRITE_UINT32 (reg_addr, a2fxxx_device->spi_cr_val);
}

//-----------------------------------------------------------------------------
// Note that no dynamic configuration options are currently defined.

static int a2fxxx_get_config
  (cyg_spi_device* dev, cyg_uint32 key, void* buf, cyg_uint32* len)
{
    return -1;
}

static int a2fxxx_set_config
  (cyg_spi_device* dev, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    return -1;
}

#endif

//=============================================================================
