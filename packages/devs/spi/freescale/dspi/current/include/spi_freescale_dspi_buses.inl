#ifndef SPI_FREESCALE_DSPI_BUSES_INL
#define SPI_FREESCALE_DSPI_BUSES_INL
//=============================================================================
//
//      spi_freescale_dspi_buses.inl
//
//      SPI bus instantiation for Freescale DSPI
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.
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
// Author(s):   ilijak
// Date:        2011-11-09
// Purpose:     Freescale DSPI DSPI bus instantiation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#define PUSHQUE_ALIGN CYGBLD_ATTRIB_ALIGN(4)

#define DSPI_DMA_BANDWIDTH FREESCALE_EDMA_CSR_BWC_0

//-----------------------------------------------------------------------------
// Instantiate the bus state data structures.


// DSPI0 BUS =================================================================
#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI0

#define  CYGBLD_DSPI0_TCD_SECTION \
    CYGBLD_ATTRIB_SECTION(CYGOPT_DEVS_SPI_FREESCALE_DSPI0_TCD_SECTION)

extern cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus0;

// Pinonomy -------------------------------------------------------------------

#ifdef CYHGWR_DEVS_SPI_FREESCALE_DSPI0_PCSS
#define CYGHWR_FREESCALE_DSPI0_MCR_PCSSE FREESCALE_DSPI_MCR_PCSSE_M
#else
#define CYGHWR_FREESCALE_DSPI0_MCR_PCSSE 0
#endif

static const cyg_uint32 spi0_pins[] = {
    CYGHWR_IO_FREESCALE_SPI0_PIN_SIN,
    CYGHWR_IO_FREESCALE_SPI0_PIN_SOUT,
    CYGHWR_IO_FREESCALE_SPI0_PIN_SCK
};

// SPI chip select pins.
static const cyg_uint32 spi0_cs_pins[] = {
#ifdef CYGHWR_FREESCALE_DSPI0_CS0
    CYGHWR_IO_FREESCALE_SPI0_PIN_CS0,
#endif
#ifdef CYGHWR_FREESCALE_DSPI0_CS1
    CYGHWR_IO_FREESCALE_SPI0_PIN_CS1,
#endif
#ifdef CYGHWR_FREESCALE_DSPI0_CS2
    CYGHWR_IO_FREESCALE_SPI0_PIN_CS2,
#endif
#ifdef CYGHWR_FREESCALE_DSPI0_CS3
    CYGHWR_IO_FREESCALE_SPI0_PIN_CS3,
#endif
#ifdef CYGHWR_FREESCALE_DSPI0_CS4
    CYGHWR_IO_FREESCALE_SPI0_PIN_CS4,
#endif
#ifdef CYGHWR_FREESCALE_DSPI0_CS5
    CYGHWR_IO_FREESCALE_SPI0_PIN_CS5
#endif
};

#ifdef CYGINT_DEVS_SPI_DSPI0_USES_DMA
static volatile
cyg_uint32 dspi0_pushque[CYGNUM_DEVS_SPI_FREESCALE_DSPI0_PUSHQUE_SIZE+4] PUSHQUE_ALIGN;

static cyghwr_hal_freescale_dma_chan_set_t dspi0_dma_chan[] =
{
    {
        .dma_src = FREESCALE_DMAMUX_SRC_SPI0_TX
        | FREESCALE_DMAMUX_CHCFG_ENBL_M,
        .dma_chan_i = CYGHWR_DEVS_SPI_FREESCALE_DSPI0_TX_DMA_CHAN,
        .dma_prio = CYGNUM_DEVS_SPI_FREESCALE_DSPI0_TX_DMA_PRI,
    },
    {
        .dma_src = FREESCALE_DMAMUX_SRC_SPI0_RX
        | FREESCALE_DMAMUX_CHCFG_ENBL_M,
        .dma_chan_i = CYGHWR_DEVS_SPI_FREESCALE_DSPI0_RX_DMA_CHAN,
        .dma_prio = CYGNUM_DEVS_SPI_FREESCALE_DSPI0_RX_DMA_PRI,
    }
};

static cyghwr_hal_freescale_dma_set_t dspi0_dma_set = {
    .chan_p = dspi0_dma_chan,
    .chan_n = 2
};

static const cyghwr_hal_freescale_edma_tcd_t dspi0_dma_tcd_tx_ini =
{
        .saddr =  (cyg_uint32 *) dspi0_pushque,
        .soff = 4,
        .attr = FREESCALE_EDMA_ATTR_SSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_DSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_SMOD(0) |
                FREESCALE_EDMA_ATTR_DMOD(0),
        .daddr = (cyg_uint32 *)&CYGADDR_IO_SPI_FREESCALE_DSPI0_P->pushr,
        .doff = 0,
        .nbytes.mlno = 4,
        .slast = 0,
        .citer.elinkno = 1,
        {.dlast = 0},
        .biter.elinkno = 1,
        .csr = DSPI_DMA_BANDWIDTH
};


static const cyghwr_hal_freescale_edma_tcd_t dspi0_dma_tcd_rx_ini =
{
        .saddr = (cyg_uint32 *)&CYGADDR_IO_SPI_FREESCALE_DSPI0_P->popr,
        .soff = 0,
        .attr = FREESCALE_EDMA_ATTR_SSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_DSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_SMOD(0) |
                FREESCALE_EDMA_ATTR_DMOD(0),
        .daddr = NULL,
        .doff = 4,
        .nbytes.mlno = 4,
        .slast = 0,
        .citer.elinkno = 1,
        {.dlast = 0},
        .biter.elinkno = 1,
        .csr = DSPI_DMA_BANDWIDTH
};

#endif // CYGINT_DEVS_SPI_DSPI0_USES_DMA

static const cyg_spi_freescale_dspi_bus_setup_t dspi0_setup = {
    .dspi_p          = CYGADDR_IO_SPI_FREESCALE_DSPI0_P,
#ifdef CYGINT_DEVS_SPI_DSPI0_USES_DMA
    .dma_set_p       = &dspi0_dma_set,
#else
    .dma_set_p       = NULL,
#endif
    .intr_num        = CYGNUM_HAL_INTERRUPT_SPI0,
    .intr_prio       = CYGNUM_DEVS_SPI_FREESCALE_DSPI0_ISR_PRI,
    .spi_pin_list_p  = spi0_pins,
    .cs_pin_list_p   = spi0_cs_pins,
    .cs_pin_num      = sizeof(spi0_cs_pins)/sizeof(spi0_cs_pins[0]),
    .mcr_opt         = CYGHWR_FREESCALE_DSPI0_MCR_PCSSE |
                       FREESCALE_DSPI_MCR_PCSIS(CYGHWR_DEVS_SPI_FREESCALE_DSPI0_PCSIS)
};

cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus0 = {
    .spi_bus.spi_transaction_begin    = dspi_transaction_begin,
    .spi_bus.spi_transaction_transfer = dspi_transaction_transfer,
    .spi_bus.spi_transaction_tick     = dspi_transaction_tick,
    .spi_bus.spi_transaction_end      = dspi_transaction_end,
    .spi_bus.spi_get_config           = dspi_get_config,
    .spi_bus.spi_set_config           = dspi_set_config,
    .setup_p                          = &dspi0_setup,
#ifdef CYGINT_DEVS_SPI_DSPI0_USES_DMA
    .tx_dma_tcd_p = &CYGHWR_IO_FREESCALE_EDMA0_P->
                     tcd[CYGHWR_DEVS_SPI_FREESCALE_DSPI0_TX_DMA_CHAN],
    .rx_dma_tcd_p = &CYGHWR_IO_FREESCALE_EDMA0_P->
    tcd[CYGHWR_DEVS_SPI_FREESCALE_DSPI0_RX_DMA_CHAN],
    .tx_dma_tcd_ini_p = &dspi0_dma_tcd_tx_ini,
    .rx_dma_tcd_ini_p = &dspi0_dma_tcd_rx_ini,
    .pushque_p = dspi0_pushque,
#else // CYGINT_DEVS_SPI_DSPI0_USES_DMA
    .tx_dma_tcd_p = NULL,
    .rx_dma_tcd_p = NULL,
    .tx_dma_tcd_ini_p = NULL,
    .rx_dma_tcd_ini_p = NULL,
    .pushque_p = NULL,
#endif // CYGINT_DEVS_SPI_DSPI0_USES_DMA
    .pushque_n = CYGNUM_DEVS_SPI_FREESCALE_DSPI0_PUSHQUE_SIZE,
    .txfifo_n = CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE,
    .rxfifo_n = CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE
};
#endif

// DSPI1 BUS =================================================================
#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI1

#define  CYGBLD_DSPI1_TCD_SECTION \
    CYGBLD_ATTRIB_SECTION(CYGOPT_DEVS_SPI_FREESCALE_DSPI1_TCD_SECTION)

extern cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus1;

// Pinonomy -------------------------------------------------------------------

#ifdef CYHGWR_DEVS_SPI_FREESCALE_DSPI1_PCSS
#define CYGHWR_FREESCALE_DSPI1_MCR_PCSSE FREESCALE_DSPI_MCR_PCSSE_M
#else
#define CYGHWR_FREESCALE_DSPI1_MCR_PCSSE 0
#endif

static const cyg_uint32 spi1_pins[] = {
    CYGHWR_IO_FREESCALE_SPI1_PIN_SIN,
    CYGHWR_IO_FREESCALE_SPI1_PIN_SOUT,
    CYGHWR_IO_FREESCALE_SPI1_PIN_SCK
};

// SPI chip select pins.
static const cyg_uint32 spi1_cs_pins[] = {
#ifdef CYGHWR_FREESCALE_DSPI1_CS0
    CYGHWR_IO_FREESCALE_SPI1_PIN_CS0,
#endif
#ifdef CYGHWR_FREESCALE_DSPI1_CS1
    CYGHWR_IO_FREESCALE_SPI1_PIN_CS1,
#endif
#ifdef CYGHWR_FREESCALE_DSPI1_CS2
    CYGHWR_IO_FREESCALE_SPI1_PIN_CS2,
#endif
#ifdef CYGHWR_FREESCALE_DSPI1_CS3
    CYGHWR_IO_FREESCALE_SPI1_PIN_CS3,
#endif
#ifdef CYGHWR_FREESCALE_DSPI1_CS4
    CYGHWR_IO_FREESCALE_SPI1_PIN_CS4,
#endif
#ifdef CYGHWR_FREESCALE_DSPI1_CS5
    CYGHWR_IO_FREESCALE_SPI1_PIN_CS5
#endif
};

#ifdef CYGINT_DEVS_SPI_DSPI1_USES_DMA
static volatile
cyg_uint32 dspi1_pushque[CYGNUM_DEVS_SPI_FREESCALE_DSPI1_PUSHQUE_SIZE+4]
           PUSHQUE_ALIGN EDMA_RAM_BUF_SECTION;

static const cyghwr_hal_freescale_dma_chan_set_t dspi1_dma_chan[2] =
{
    {
        .dma_src = FREESCALE_DMAMUX_SRC_SPI1_TX
        | FREESCALE_DMAMUX_CHCFG_ENBL_M,
        .dma_chan_i = CYGHWR_DEVS_SPI_FREESCALE_DSPI1_TX_DMA_CHAN,
        .dma_prio = CYGNUM_DEVS_SPI_FREESCALE_DSPI1_TX_DMA_PRI,
    },
    {
        .dma_src = FREESCALE_DMAMUX_SRC_SPI1_RX
        | FREESCALE_DMAMUX_CHCFG_ENBL_M,
        .dma_chan_i = CYGHWR_DEVS_SPI_FREESCALE_DSPI1_RX_DMA_CHAN,
        .dma_prio = CYGNUM_DEVS_SPI_FREESCALE_DSPI1_RX_DMA_PRI,
    }
};

static cyghwr_hal_freescale_dma_set_t dspi1_dma_set = {
    .chan_p = dspi1_dma_chan,
    .chan_n = 2
};

static const cyghwr_hal_freescale_edma_tcd_t dspi1_dma_tcd_tx_ini =
{
        .saddr =  (cyg_uint32 *) dspi1_pushque,
        .soff = 4,
        .attr = FREESCALE_EDMA_ATTR_SSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_DSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_SMOD(0) |
                FREESCALE_EDMA_ATTR_DMOD(0),
        .daddr = (cyg_uint32 *)&CYGADDR_IO_SPI_FREESCALE_DSPI1_P->pushr,
        .doff = 0,
        .nbytes.mlno = 4,
        .slast = 0,
        .citer.elinkno = 1,
        {.dlast = 0},
        .biter.elinkno = 1,
        .csr = DSPI_DMA_BANDWIDTH
};


static const cyghwr_hal_freescale_edma_tcd_t dspi1_dma_tcd_rx_ini =
{
        .saddr = (cyg_uint32 *)&CYGADDR_IO_SPI_FREESCALE_DSPI1_P->popr,
        .soff = 0,
        .attr = FREESCALE_EDMA_ATTR_SSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_DSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_SMOD(0) |
                FREESCALE_EDMA_ATTR_DMOD(0),
        .daddr = NULL,
        .doff = 4,
        .nbytes.mlno = 4,
        .slast = 0,
        .citer.elinkno = 1,
        {.dlast = 0},
        .biter.elinkno = 1,
        .csr = DSPI_DMA_BANDWIDTH
};

#endif // CYGINT_DEVS_SPI_DSPI1_USES_DMA

static const cyg_spi_freescale_dspi_bus_setup_t dspi1_setup = {
    .dspi_p          = CYGADDR_IO_SPI_FREESCALE_DSPI1_P,
#ifdef CYGINT_DEVS_SPI_DSPI1_USES_DMA
    .dma_set_p       = &dspi1_dma_set,
#else
    .dma_set_p       = NULL,
#endif
    .intr_num        = CYGNUM_HAL_INTERRUPT_SPI1,
    .intr_prio       = CYGNUM_DEVS_SPI_FREESCALE_DSPI1_ISR_PRI,
    .spi_pin_list_p  = spi1_pins,
    .cs_pin_list_p   = spi1_cs_pins,
    .cs_pin_num      = sizeof(spi1_cs_pins)/sizeof(spi1_cs_pins[0]),
    .mcr_opt         = CYGHWR_FREESCALE_DSPI1_MCR_PCSSE |
                       FREESCALE_DSPI_MCR_PCSIS(CYGHWR_DEVS_SPI_FREESCALE_DSPI1_PCSIS)
};

cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus1 = {
    .spi_bus.spi_transaction_begin    = dspi_transaction_begin,
    .spi_bus.spi_transaction_transfer = dspi_transaction_transfer,
    .spi_bus.spi_transaction_tick     = dspi_transaction_tick,
    .spi_bus.spi_transaction_end      = dspi_transaction_end,
    .spi_bus.spi_get_config           = dspi_get_config,
    .spi_bus.spi_set_config           = dspi_set_config,
    .setup_p                          = &dspi1_setup,
#ifdef CYGINT_DEVS_SPI_DSPI1_USES_DMA
    .tx_dma_tcd_p = &CYGHWR_IO_FREESCALE_EDMA0_P->
                     tcd[CYGHWR_DEVS_SPI_FREESCALE_DSPI1_TX_DMA_CHAN],
    .rx_dma_tcd_p = &CYGHWR_IO_FREESCALE_EDMA0_P->
    tcd[CYGHWR_DEVS_SPI_FREESCALE_DSPI1_RX_DMA_CHAN],
    .tx_dma_tcd_ini_p = &dspi1_dma_tcd_tx_ini,
    .rx_dma_tcd_ini_p = &dspi1_dma_tcd_rx_ini,
    .pushque_p = dspi1_pushque,
#else // CYGINT_DEVS_SPI_DSPI1_USES_DMA
    .tx_dma_tcd_p = NULL,
    .rx_dma_tcd_p = NULL,
    .tx_dma_tcd_ini_p = NULL,
    .rx_dma_tcd_ini_p = NULL,
    .pushque_p = NULL,
#endif // CYGINT_DEVS_SPI_DSPI1_USES_DMA
    .pushque_n = CYGNUM_DEVS_SPI_FREESCALE_DSPI1_PUSHQUE_SIZE,
    .txfifo_n = CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE,
    .rxfifo_n = CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE
};
#endif

// DSPI2 BUS =================================================================
#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI2

#define  CYGBLD_DSPI2_TCD_SECTION \
    CYGBLD_ATTRIB_SECTION(CYGOPT_DEVS_SPI_FREESCALE_DSPI2_TCD_SECTION)

extern cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus2;

// Pinonomy -------------------------------------------------------------------

#ifdef CYHGWR_DEVS_SPI_FREESCALE_DSPI2_PCSS
#define CYGHWR_FREESCALE_DSPI2_MCR_PCSSE FREESCALE_DSPI_MCR_PCSSE_M
#else
#define CYGHWR_FREESCALE_DSPI2_MCR_PCSSE 0
#endif

static const cyg_uint32 spi2_pins[] = {
    CYGHWR_IO_FREESCALE_SPI2_PIN_SIN,
    CYGHWR_IO_FREESCALE_SPI2_PIN_SOUT,
    CYGHWR_IO_FREESCALE_SPI2_PIN_SCK
};

// SPI chip select pins.
static const cyg_uint32 spi2_cs_pins[] = {
#ifdef CYGHWR_FREESCALE_DSPI2_CS0
    CYGHWR_IO_FREESCALE_SPI2_PIN_CS0,
#endif
#ifdef CYGHWR_FREESCALE_DSPI2_CS1
    CYGHWR_IO_FREESCALE_SPI2_PIN_CS1,
#endif
#ifdef CYGHWR_FREESCALE_DSPI2_CS2
    CYGHWR_IO_FREESCALE_SPI2_PIN_CS2,
#endif
#ifdef CYGHWR_FREESCALE_DSPI2_CS3
    CYGHWR_IO_FREESCALE_SPI2_PIN_CS3,
#endif
#ifdef CYGHWR_FREESCALE_DSPI2_CS4
    CYGHWR_IO_FREESCALE_SPI2_PIN_CS4,
#endif
#ifdef CYGHWR_FREESCALE_DSPI2_CS5
    CYGHWR_IO_FREESCALE_SPI2_PIN_CS5
#endif
};

#ifdef CYGINT_DEVS_SPI_DSPI2_USES_DMA
static volatile
cyg_uint32 dspi2_pushque[CYGNUM_DEVS_SPI_FREESCALE_DSPI2_PUSHQUE_SIZE+4] PUSHQUE_ALIGN;

static const cyghwr_hal_freescale_dma_chan_set_t dspi2_dma_chan[] =
{
    {
        .dma_src = FREESCALE_DMAMUX_SRC_SPI2_TX
        | FREESCALE_DMAMUX_CHCFG_ENBL_M,
        .dma_chan_i = CYGHWR_DEVS_SPI_FREESCALE_DSPI2_TX_DMA_CHAN,
        .dma_prio = CYGNUM_DEVS_SPI_FREESCALE_DSPI2_TX_DMA_PRI,
    },
    {
        .dma_src = FREESCALE_DMAMUX_SRC_SPI2_RX
        | FREESCALE_DMAMUX_CHCFG_ENBL_M,
        .dma_chan_i = CYGHWR_DEVS_SPI_FREESCALE_DSPI2_RX_DMA_CHAN,
        .dma_prio = CYGNUM_DEVS_SPI_FREESCALE_DSPI2_RX_DMA_PRI,
    }
};

static cyghwr_hal_freescale_dma_set_t dspi2_dma_set = {
    .chan_p = dspi2_dma_chan,
    .chan_n = 2
};

static const cyghwr_hal_freescale_edma_tcd_t dspi2_dma_tcd_tx_ini =
{
        .saddr =  (cyg_uint32 *) dspi2_pushque,
        .soff = 4,
        .attr = FREESCALE_EDMA_ATTR_SSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_DSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_SMOD(0) |
                FREESCALE_EDMA_ATTR_DMOD(0),
        .daddr = (cyg_uint32 *)&CYGADDR_IO_SPI_FREESCALE_DSPI2_P->pushr,
        .doff = 0,
        .nbytes.mlno = 4,
        .slast = 0,
        .citer.elinkno = 1,
        {.dlast = 0},
        .biter.elinkno = 1,
        .csr = DSPI_DMA_BANDWIDTH
};


static const cyghwr_hal_freescale_edma_tcd_t dspi2_dma_tcd_rx_ini =
{
        .saddr = (cyg_uint32 *)&CYGADDR_IO_SPI_FREESCALE_DSPI2_P->popr,
        .soff = 0,
        .attr = FREESCALE_EDMA_ATTR_SSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_DSIZE(FREESCALE_EDMA_ATTR_SIZE_32) |
                FREESCALE_EDMA_ATTR_SMOD(0) |
                FREESCALE_EDMA_ATTR_DMOD(0),
        .daddr = NULL,
        .doff = 4,
        .nbytes.mlno = 4,
        .slast = 0,
        .citer.elinkno = 1,
        {.dlast = 0},
        .biter.elinkno = 1,
        .csr = DSPI_DMA_BANDWIDTH
};

#endif // CYGINT_DEVS_SPI_DSPI2_USES_DMA

static const cyg_spi_freescale_dspi_bus_setup_t dspi2_setup = {
    .dspi_p          = CYGADDR_IO_SPI_FREESCALE_DSPI2_P,
#ifdef CYGINT_DEVS_SPI_DSPI2_USES_DMA
    .dma_set_p       = &dspi2_dma_set,
#else
    .dma_set_p       = NULL,
#endif
    .intr_num        = CYGNUM_HAL_INTERRUPT_SPI2,
    .intr_prio       = CYGNUM_DEVS_SPI_FREESCALE_DSPI2_ISR_PRI,
    .spi_pin_list_p  = spi2_pins,
    .cs_pin_list_p   = spi2_cs_pins,
    .cs_pin_num      = sizeof(spi2_cs_pins)/sizeof(spi2_cs_pins[0]),
    .mcr_opt         = CYGHWR_FREESCALE_DSPI2_MCR_PCSSE |
                       FREESCALE_DSPI_MCR_PCSIS(CYGHWR_DEVS_SPI_FREESCALE_DSPI2_PCSIS)
};

cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus2 = {
    .spi_bus.spi_transaction_begin    = dspi_transaction_begin,
    .spi_bus.spi_transaction_transfer = dspi_transaction_transfer,
    .spi_bus.spi_transaction_tick     = dspi_transaction_tick,
    .spi_bus.spi_transaction_end      = dspi_transaction_end,
    .spi_bus.spi_get_config           = dspi_get_config,
    .spi_bus.spi_set_config           = dspi_set_config,
    .setup_p                          = &dspi2_setup,
#ifdef CYGINT_DEVS_SPI_DSPI2_USES_DMA
    .tx_dma_tcd_p = &CYGHWR_IO_FREESCALE_EDMA0_P->
                     tcd[CYGHWR_DEVS_SPI_FREESCALE_DSPI2_TX_DMA_CHAN],
    .rx_dma_tcd_p = &CYGHWR_IO_FREESCALE_EDMA0_P->
    tcd[CYGHWR_DEVS_SPI_FREESCALE_DSPI2_RX_DMA_CHAN],
    .tx_dma_tcd_ini_p = &dspi2_dma_tcd_tx_ini,
    .rx_dma_tcd_ini_p = &dspi2_dma_tcd_rx_ini,
    .pushque_p = dspi2_pushque,
#else // CYGINT_DEVS_SPI_DSPI2_USES_DMA
    .tx_dma_tcd_p = NULL,
    .rx_dma_tcd_p = NULL,
    .tx_dma_tcd_ini_p = NULL,
    .rx_dma_tcd_ini_p = NULL,
    .pushque_p = NULL,
#endif // CYGINT_DEVS_SPI_DSPI2_USES_DMA
    .pushque_n = CYGNUM_DEVS_SPI_FREESCALE_DSPI2_PUSHQUE_SIZE,
    .txfifo_n = CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE,
    .rxfifo_n = CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE
};
#endif


//=============================================================================
#endif // SPI_FREESCALE_DSPI_BUSES_INL
