//=============================================================================
//
//      spi_freescale_dspi.c
//
//      SPI driver implementation for Freescale DSPI
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
// Date:        2011-11-04
// Purpose:     Freescale DSPI SPI driver implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <string.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include <cyg/io/spi.h>
#include <cyg/hal/hal_endian.h>

#include <pkgconf/devs_spi_freescale_dspi.h>
#if defined(CYGHWR_DEVS_SPI_FREESCALE_DSPI0) || \
    defined(CYGHWR_DEVS_SPI_FREESCALE_DSPI1) || \
    defined(CYGHWR_DEVS_SPI_FREESCALE_DSPI2)

#include <cyg/io/spi_freescale_dspi.h>


#define DEBUG_SPI CYGPKG_DEVS_SPI_FREESCALE_DSPI_DEBUG_LEVEL

#if DEBUG_SPI >= 3
# define DEBUG3_PRINTF(args...) diag_printf(args)
#else
# define DEBUG3_PRINTF(args...)
#endif

#if DEBUG_SPI >= 2
# define DEBUG2_PRINTF(args...) diag_printf(args)
#else
# define DEBUG2_PRINTF(args...)
#endif

#if DEBUG_SPI >= 1
# define DEBUG1_PRINTF(args...) diag_printf(args)
#else
# define DEBUG1_PRINTF(args...)
#endif

# define DEBUG0_PRINTF(args...) diag_printf(args)


//-----------------------------------------------------------------------------
// API function call forward references.

static void dspi_transaction_begin    (cyg_spi_device*);
static void dspi_transaction_transfer (cyg_spi_device*, cyg_bool, cyg_uint32,
                                       const cyg_uint8*, cyg_uint8*, cyg_bool);
static void dspi_transaction_tick     (cyg_spi_device*, cyg_bool, cyg_uint32);
static void dspi_transaction_end      (cyg_spi_device*);
static int  dspi_get_config           (cyg_spi_device*, cyg_uint32,
                                       void*, cyg_uint32*);
static int  dspi_set_config           (cyg_spi_device*, cyg_uint32,
                                       const void*, cyg_uint32*);

//-----------------------------------------------------------------------------
// Instantiate the bus state data structures.

#include <cyg/io/spi_freescale_dspi_buses.inl>

// Some hardware manipulation inline functions and macros

static inline void dspi_disable(cyghwr_devs_freescale_dspi_t* dspi_p)
{
    dspi_p->mcr |=  FREESCALE_DSPI_MCR_MDIS_M;
}

static inline void dspi_enable(cyghwr_devs_freescale_dspi_t* dspi_p)
{
    dspi_p->mcr &=  ~FREESCALE_DSPI_MCR_MDIS_M;
}

static inline void dspi_halt(cyghwr_devs_freescale_dspi_t* dspi_p)
{
    dspi_p->mcr |=  FREESCALE_DSPI_MCR_HALT_M;
}

static inline void dspi_tlah(cyghwr_devs_freescale_dspi_t* dspi_p)
{
    dspi_p->mcr &=  ~FREESCALE_DSPI_MCR_HALT_M;
}

static inline void
dspi_irq_enable(cyghwr_devs_freescale_dspi_t* dspi_p, cyg_uint32 irq_mask)
{
    dspi_p->rser |= irq_mask;
}

static inline void
dspi_irq_disable(cyghwr_devs_freescale_dspi_t* dspi_p, cyg_uint32 irq_mask)
{
    dspi_p->rser &= ~irq_mask;
}

static inline void
dspi_status_clear(cyghwr_devs_freescale_dspi_t* dspi_p, cyg_uint32 sr_mask)
{
    dspi_p->sr |= sr_mask;
}

static inline void
dspi_fifo_clear(cyghwr_devs_freescale_dspi_t* dspi_p)
{
    dspi_p->mcr |= FREESCALE_DSPI_MCR_CLR_RXF_M | FREESCALE_DSPI_MCR_CLR_TXF_M;
}

static inline void
dspi_fifo_drain(cyghwr_devs_freescale_dspi_t* dspi_p)
{
    dspi_p->sr |= FREESCALE_DSPI_SR_RFDF_M;
}


#define DSPI_IRQ_ENABLE(__dspi_p) \
        dspi_irq_enable(__dspi_p, FREESCALE_DSPI_RSER_EOQF_RE_M)
#define DSPI_IRQ_DISABLE(__dspi_p) \
        dspi_irq_disable(__dspi_p, FREESCALE_DSPI_RSER_EOQF_RE_M)
#define DSPI_EOQ_CLEAR(__dspi_p) \
        dspi_status_clear(__dspi_p, FREESCALE_DSPI_SR_EOQF_M | \
                                     FREESCALE_DSPI_SR_TCF_M | \
                                        FREESCALE_DSPI_SR_RFDF_M)
#define DSPI_TXRX_ENABLE(__dspi_p) \
        dspi_status_clear(__dspi_p, FREESCALE_DSPI_SR_TXRXS_M)

// Alternate clocking for spi_transaction_end()
// Used to initialize CTAR1.
static const cyg_freescale_dspi_clocking_t aux_clocking =
{
    .frame_size = 4,
    .cl_pol = 0,
    .cl_pha = 0,
    .cl_brate = CYGHWR_DEVS_FREESCALEDSPI_DSPI_CTAR1_AUX_SPEED,
    .cs_up_udly = CYGHWR_DEVS_FREESCALEDSPI_DSPI_CTAR1_AUX_CS_DELAY,
    .cs_dw_udly = CYGHWR_DEVS_FREESCALEDSPI_DSPI_CTAR1_AUX_CS_DELAY,
    .tr_bt_udly =CYGHWR_DEVS_FREESCALEDSPI_DSPI_CTAR1_AUX_CS_DELAY,
    .dl_unit = CYGNUM_DSPI_DELAY_UNIT(
                   CYGHWR_DEVS_FREESCALE_DSPI_DSPI_CTAR1_AUX_DELAY_UNIT),
    .cl_dbr = CYGHWR_DEVS_FREESCALEDSPI_DSPI_CTAR1_AUX_USE_DBR
};

//-----------------------------------------------------------------------------
// Implement DSPI ISRs.

// ISR for DSPI with DMA
// Disable DSPI IRQ and Tx DMA channel and schedule DSR.
static cyg_uint32 dspi_dma_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_spi_freescale_dspi_bus_t* dspi_bus_p =
          (cyg_spi_freescale_dspi_bus_t*) data;
    cyghwr_devs_freescale_dspi_t* dspi_p = dspi_bus_p->setup_p->dspi_p;
    cyghwr_hal_freescale_dma_set_t *dma_set_p = dspi_bus_p->setup_p->dma_set_p;
    cyghwr_hal_freescale_edma_t *edma_p;
    edma_p = dma_set_p->edma_p;

    cyg_drv_isr_lock();

    // Disable the Tx DMA channel and DSPI IRQ.
    hal_freescale_edma_erq_disable(edma_p, SPI_DMA_CHAN_I(dma_set_p, TX));
    DSPI_IRQ_DISABLE(dspi_p);

    cyg_drv_interrupt_acknowledge(vector);
    cyg_drv_isr_unlock();
    return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

// ISR for DSPI without DMA
// Disable DSPI IRQ and schedule DSR.
static cyg_uint32 dspi_nodma_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_spi_freescale_dspi_bus_t* dspi_bus_p =
          (cyg_spi_freescale_dspi_bus_t*) data;
    cyghwr_devs_freescale_dspi_t* dspi_p = dspi_bus_p->setup_p->dspi_p;

    cyg_drv_isr_lock();

    // Disable the DSPI IRQ.
    DSPI_IRQ_DISABLE(dspi_p);

    cyg_drv_interrupt_acknowledge(vector);
    cyg_drv_isr_unlock();
    return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

// DSPI DSR
static void dspi_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_spi_freescale_dspi_bus_t* dspi_bus = (cyg_spi_freescale_dspi_bus_t*) data;

    cyg_drv_dsr_lock();
    cyg_drv_cond_signal(&dspi_bus->transfer_done);
    cyg_drv_dsr_unlock();
}

//-----------------------------------------------------------------------------
// Calculate best fit CTAR baud rate setting (using some brute force).
// Best fit is considered the highest frequency that is not highe than set point.

static const cyg_uint16 ctar_br[16] = { 2/2, 4/2, 6/2, 8/2, 16/2, 32/2, 64/2,
    128/2, 256/2, 512/2, 1024/2, 2048/2, 4096/2, 8192/2, 16384/2, 32768/2 };
static const cyg_uint8 ctar_pbr[4] = { 2, 3, 5, 7 };

static const cyg_uint16 ctar_cssck[16] = { 2/2, 4/2, 8/2, 16/2, 32/2, 64/2,
    128/2, 256/2, 512/2, 1024/2, 2048/2, 4096/2, 8192/2, 16384/2, 32768/2,
    65536/2 };
static const cyg_uint8 ctar_pcssck[4] = { 1, 3, 5, 7 };

typedef struct ctar_br_s {
    cyg_uint8 valid;
    cyg_uint8 br;
    cyg_uint8 dbr;
    cyg_uint8 pbr;
} ctar_br_t;

static int dspi_ctar_brbf (const cyg_freescale_dspi_clocking_t* spi_cocking,
                              ctar_br_t* brs_p, const cyg_uint16* br_p,
                              const cyg_uint8* pbr_p, cyg_uint32* alt_brate,
                              cyg_uint32 sys_clk)
{
    cyg_uint32 pbr_i;
    cyg_uint32 br_i;
    cyg_uint32 dbr;
    cyg_uint32 clk;
    cyg_uint32 baud_bf = 0; // Best fit
    cyg_uint32 baud_tmp;
    cyg_uint32 baud_sp = alt_brate ? *alt_brate :
                         spi_cocking->cl_brate; // Set point

    // Calculate the maximal viable bus speed.
    clk = sys_clk;
     // Desired baud rate very high, use DBR if allowed
    dbr = (baud_sp > (clk / 4)) && spi_cocking->cl_dbr && (br_p == ctar_br);

    for(pbr_i = 0; pbr_i < 3; pbr_i++) {
        for(br_i = 0; br_i < 15; br_i++) {
            if((baud_tmp = (clk * (1+dbr)) / (pbr_p[pbr_i] * (2*br_p[br_i])))
               <= baud_sp)
            {
                if(baud_tmp > baud_bf) {
                    DEBUG3_PRINTF("DSPI Baud:"
                                  " SP=%d pbr=0x%x br=0x%x dbr=%d Temp=%d (SysClk=%d)\n",
                                  baud_sp, pbr_i, br_i, dbr, baud_tmp, clk);
                    brs_p->br = br_i;
                    brs_p->pbr = pbr_i;
                    brs_p->valid = 1;
                    baud_bf = baud_tmp;
                    if(baud_tmp == baud_sp)
                        goto baud_found;
                }
            }
        }
    }
    if(!brs_p->valid) {
        pbr_i = 3;
        br_i = 15;
        DEBUG1_PRINTF("DSPI Baud too low:"
                      " SP=%d pbr=0x%x br=0x%x Actual=%d (SysClk=%d)\n",
                      baud_sp, pbr_i, br_i,
                      (clk * (1+dbr)) / (pbr_p[pbr_i] * (2*br_p[br_i])), clk);
        brs_p->br = br_i;
        brs_p->pbr = pbr_i;

        return -1;
    } else {
baud_found:
        brs_p->dbr = dbr;
        DEBUG1_PRINTF("DSPI Baud found:"
                      " SP=%d pbr=0x%x br=0x%x dbr=%d Actual=%dHz (%dns) (SysClk=%d)\n",
                      baud_sp, brs_p->pbr, brs_p->br, dbr, baud_bf, 1000000000/baud_bf, clk);

        return 0;
    }
}

// Set Clock and Transfer Attributes Register
#define CYG_ASSERT_LOW_FREQ \
   CYG_ASSERT (false, "Freescale DSPI: Cannot run bus as slowly as requested.")
cyg_uint32
dspi_calc_ctar(const cyg_freescale_dspi_clocking_t* spi_clocking, cyg_uint32 sys_clk)
{
    cyg_uint32 regval;
    ctar_br_t brs;
    cyg_uint32 delay_brate;

    regval = FREESCALE_DSPI_CTAR_FMSZ(spi_clocking->frame_size - 1);
    if(spi_clocking->cl_pol)
        regval |= FREESCALE_DSPI_CTAR_SLAVE_CPOL_M;
    if(spi_clocking->cl_pha)
        regval |= FREESCALE_DSPI_CTAR_SLAVE_CPHA_M;
    if(spi_clocking->lsb_first)
        regval |= FREESCALE_DSPI_CTAR_LSBFE_M;

    // Get divider bits
    // Baud rate
    brs.dbr = 0;
    if (!dspi_ctar_brbf(spi_clocking, &brs, ctar_br, ctar_pbr, NULL, sys_clk))
    {
        regval |= FREESCALE_DSPI_CTAR_BR(brs.br) |
              FREESCALE_DSPI_CTAR_PBR(brs.pbr) |
              (brs.dbr ? FREESCALE_DSPI_CTAR_DBR_M : 0);
    } else
        CYG_ASSERT_LOW_FREQ;

    delay_brate = 100000000 / (spi_clocking->dl_unit * spi_clocking->cs_up_udly);
    if (!dspi_ctar_brbf(spi_clocking, &brs, ctar_cssck, ctar_pcssck,
                           &delay_brate, sys_clk))
    {
        regval |= FREESCALE_DSPI_CTAR_CSSCK(brs.br) |
              FREESCALE_DSPI_CTAR_PCSSCK(brs.pbr);
    } else
        CYG_ASSERT_LOW_FREQ;

    // Delay between clock and CS negation
    delay_brate = 100000000 / (spi_clocking->dl_unit * spi_clocking->cs_dw_udly);
    if (!dspi_ctar_brbf(spi_clocking, &brs, ctar_cssck, ctar_pcssck,
                           &delay_brate, sys_clk))
    {
        regval |= FREESCALE_DSPI_CTAR_ASC(brs.br) |
              FREESCALE_DSPI_CTAR_PASC(brs.pbr);
    } else
        CYG_ASSERT_LOW_FREQ;

    // Delay between clock and CS negation and assertion
    delay_brate = 100000000 / (spi_clocking->dl_unit * spi_clocking->tr_bt_udly);
    if (!dspi_ctar_brbf(spi_clocking, &brs, ctar_cssck, ctar_pcssck,
                           &delay_brate, sys_clk))
    {
        regval |= FREESCALE_DSPI_CTAR_DT(brs.br) |
              FREESCALE_DSPI_CTAR_PDT(brs.pbr);
    } else
        CYG_ASSERT_LOW_FREQ;
    return regval;
}

//----------------------------------------------------------------------------
//Set up SPI bus pins

static void dspi_pin_setup(const cyg_uint32* spi_pins_p,
                           const cyg_uint32* cs_pins_p, cyg_uint32 cs_pin_n)
{
    const cyg_uint32* pin_p;

    for(pin_p = spi_pins_p;
        pin_p < spi_pins_p + 3;
        CYGHWR_IO_FREESCALE_DSPI_PIN(*pin_p++));

    for(pin_p = cs_pins_p;
        pin_p < cs_pins_p + cs_pin_n;
        CYGHWR_IO_FREESCALE_DSPI_PIN(*pin_p++));
}

//-----------------------------------------------------------------------------
// Set up a new SPI bus on initialisation.

static void dspi_bus_setup(cyg_spi_freescale_dspi_bus_t* spi_bus_p)
{
    cyghwr_devs_freescale_dspi_t* dspi_p = spi_bus_p->setup_p->dspi_p;
    cyghwr_hal_freescale_dma_set_t* dma_set_p;
    cyghwr_hal_freescale_edma_t* edma_p;
    cyg_uint32 dma_chan_i;

    // Get the clock frequency from HAL.
    spi_bus_p->clock_freq = CYGHWR_IO_SPI_FREESCALE_DSPI_CLOCK;
    DEBUG1_PRINTF("DSPI BUS %p: SysClk=%d\n", spi_bus_p, spi_bus_p->clock_freq);

    // Set up the pins.
    dspi_pin_setup(spi_bus_p->setup_p->spi_pin_list_p,
                   spi_bus_p->setup_p->cs_pin_list_p,
                   spi_bus_p->setup_p->cs_pin_num);

    // Set up default SPI configuration.
    dspi_p->mcr = spi_bus_p->setup_p->mcr_opt | FREESCALE_DSPI_MCR_MSTR_M |
          FREESCALE_DSPI_MCR_CLR_RXF_M | FREESCALE_DSPI_MCR_CLR_TXF_M |
          FREESCALE_DSPI_MCR_MDIS_M;

    // Enable DSPI controller.
    dspi_enable(dspi_p);

    if((dma_set_p=spi_bus_p->setup_p->dma_set_p)) {
        // Initialize DMA channels
        hal_freescale_edma_init_chanset(dma_set_p);
#if DEBUG_SPI >= 1
        hal_freescale_edma_diag(dma_set_p, 0xffff);
        cyghwr_devs_freescale_dspi_diag(spi_bus_p);
#endif
        // Set up DMA transfer control descriptors
        edma_p = dma_set_p->edma_p;
        dma_chan_i = dma_set_p->chan_p[SPI_DMA_CHAN_TX_I].dma_chan_i;
        hal_freescale_edma_transfer_init(edma_p, dma_chan_i,
                                         spi_bus_p->tx_dma_tcd_ini_p);
#if DEBUG_SPI >= 1
        hal_freescale_edma_transfer_diag(edma_p, dma_chan_i, true);
#endif
        dma_chan_i = dma_set_p->chan_p[SPI_DMA_CHAN_RX_I].dma_chan_i;
        hal_freescale_edma_transfer_init(edma_p, dma_chan_i,
                                         spi_bus_p->rx_dma_tcd_ini_p);
#if DEBUG_SPI >= 1
        hal_freescale_edma_transfer_diag(edma_p, dma_chan_i, true);
#endif
        // Enable SPI DMA requests
        dspi_p->rser = FREESCALE_DSPI_RSER_TFFF_DIRS_M |
                       FREESCALE_DSPI_RSER_RFDF_DIRS_M |
                       FREESCALE_DSPI_RSER_TFFF_RE_M |
                       FREESCALE_DSPI_RSER_RFDF_RE_M;
    }
#if DEBUG_SPI >= 1
    cyghwr_devs_freescale_dspi_diag(spi_bus_p);
#endif
    // Initialise the synchronisation primitivies.
    cyg_drv_mutex_init (&spi_bus_p->transfer_mutex);
    cyg_drv_cond_init (&spi_bus_p->transfer_done, &spi_bus_p->transfer_mutex);

    // Hook up the ISR and DSR.
    cyg_drv_interrupt_create (spi_bus_p->setup_p->intr_num,
                              spi_bus_p->setup_p->intr_prio,
                              (cyg_addrword_t) spi_bus_p,
                              dma_set_p ? dspi_dma_ISR : dspi_nodma_ISR,
                              dspi_DSR, &spi_bus_p->intr_handle,
                              &spi_bus_p->intr_data);
    cyg_drv_interrupt_attach (spi_bus_p->intr_handle);

    dspi_p->ctar[1] = dspi_calc_ctar(&aux_clocking, spi_bus_p->clock_freq);

    // Call upper layer bus init.
    CYG_SPI_BUS_COMMON_INIT(&spi_bus_p->spi_bus);
}

//-----------------------------------------------------------------------------
// Set up Rx DMA channel
static void
rx_dma_channel_setup(cyghwr_hal_freescale_dma_set_t *dma_set_p,
                     cyg_uint8* data_buf, cyg_uint32 bus_16bit,
                     volatile cyghwr_hal_freescale_edma_tcd_t *tcd_p)
{
    cyg_uint32 step, sdsize;
    static cyg_uint8 popr_sink;

    // Set the correct transfer size.
    if(bus_16bit) {
        step = 2;
        sdsize = FREESCALE_EDMA_ATTR_SIZE_16;
    } else {
        step = 1;
        sdsize = FREESCALE_EDMA_ATTR_SIZE_8;
    }
    if(data_buf) {
        tcd_p->doff = step;
        tcd_p->daddr = data_buf;
    } else {
        tcd_p->doff = 0;
        tcd_p->daddr = &popr_sink;
    }
    tcd_p->nbytes.mlno = step;
    tcd_p->attr =  FREESCALE_EDMA_ATTR_SSIZE(sdsize) |
          FREESCALE_EDMA_ATTR_DSIZE(sdsize) |
          FREESCALE_EDMA_ATTR_SMOD(0) |
          FREESCALE_EDMA_ATTR_DMOD(0);
#if DEBUG_SPI >= 3
    hal_freescale_edma_tcd_diag(tcd_p, -1, "[DSPI Rx]");
#endif
}

//----------------------------------------------------------------------------
// Set up Tx FIFO queue
// Set up Tx FIFO command queue
//     DSPI requires sending command for every transfer.
// Used for transfers that fit within DSPI FIFO.

#if DEBUG_SPI >= 2
static const char debug_format[] = "BUFF %dbit %s: %p 0x%08x remain %d:\n";
#endif

static inline volatile cyg_uint32
fifo_pushque_fill(cyg_spi_freescale_dspi_bus_t* dspi_bus, cyg_uint8*
                  data_p, cyg_uint32 count, cyg_bool bus_16bit, cyg_uint32 pushr,
                  cyg_bool drop_cs)
{
    cyghwr_devs_freescale_dspi_t* dspi_p = dspi_bus->setup_p->dspi_p;
    cyg_uint32 txfifo_n = dspi_bus->txfifo_n;

    if(data_p) {
        if(!bus_16bit) {
            for(; count > 1; count--) {
                if(!(--txfifo_n)) {
                    dspi_p->pushr = pushr |= *data_p | FREESCALE_DSPI_PUSHR_EOQ_M;
                    count--;
                    DEBUG2_PRINTF(debug_format, 8, "FBK", &dspi_p->pushr,
                                  pushr | *data_p, count);
                    return count;
                }
                dspi_p->pushr = pushr | *data_p++;
                DEBUG3_PRINTF(debug_format, 8, "FAD", &dspi_p->pushr,
                              pushr | data_p[-1], count-1);
            }
            pushr |= *data_p;
        } else {
            cyg_uint16* data16_p = (cyg_uint16 *)data_p;
            cyg_uint16 data_word;

            for(; count > 2; count-=2) {
                if(!(--txfifo_n)) {
                    dspi_p->pushr = pushr |= *data16_p | FREESCALE_DSPI_PUSHR_EOQ_M;
                    count-=2;
                    DEBUG2_PRINTF(debug_format, 16, "FBK", &dspi_p->pushr,
                                  pushr, count);
                    return count;
                }
                data_word = *data16_p++;
                dspi_p->pushr = pushr | data_word;
                DEBUG3_PRINTF(debug_format, 16, "FAD", &dspi_p->pushr,
                              pushr | data_word, (count-1)*2);
            }
            data_word = *data16_p;
            pushr |= data_word;
        }
    } else {
        for(; count > 1; count--) {
            if(!(--txfifo_n)) {
                dspi_p->pushr = pushr |= FREESCALE_DSPI_PUSHR_EOQ_M;
                count--;
                DEBUG2_PRINTF(debug_format, 0, "FBK", &dspi_p->pushr,
                              pushr, count);
                return count;
            }
            dspi_p->pushr = pushr;
            DEBUG3_PRINTF(debug_format, 0, "FAD", &dspi_p->pushr, pushr,
                          count-1);
        }
    }
    if(drop_cs)
        pushr &= ~FREESCALE_DSPI_PUSHR_CONT_M;
    dspi_p->pushr = pushr |= FREESCALE_DSPI_PUSHR_EOQ_M;
    DEBUG3_PRINTF(debug_format, data_p ? (bus_16bit ? 16 :8) : pushr,
                  (drop_cs ? "FEN" : "FSG"), &dspi_p->pushr, pushr, 0);
    return 0;
}

//----------------------------------------------------------------------------
// Set up Tx
// Set up Tx DMA command queue
//     DSPI requires sending command for every transfer.
// Used for transfers larger than DSPI FIFO

static inline volatile cyg_uint32
dma_pushque_fill(cyg_spi_freescale_dspi_bus_t* dspi_bus, cyg_uint8* data_p,
                 cyg_uint32 count, cyg_bool bus_16bit, cyg_uint32 pushr,
                 cyg_bool drop_cs)
{
    volatile cyg_uint32* pushque_p;
    volatile cyg_uint32* pushque_end;

    pushque_p = dspi_bus->pushque_p;
    pushque_end = pushque_p + dspi_bus->pushque_n;
    pushque_p = dspi_bus->pushque_p;
    if(data_p) {
        if(!bus_16bit) {
            do {
                if(pushque_p == pushque_end) {
                    pushque_p[0] = pushr;
                    pushque_p[-1] |= FREESCALE_DSPI_PUSHR_EOQ_M;
                    DEBUG2_PRINTF(debug_format, 8, "BRK", pushque_p-1,
                                  pushque_p[-1], count);
                    return count;
                }
                *pushque_p++ = pushr | *data_p++;
                DEBUG3_PRINTF(debug_format, 8, "ADD", pushque_p-1,
                              pushque_p[-1], count-1);
            } while(--count > 1);
            pushr |= *data_p;
        } else {
            cyg_uint16* data16_p = (cyg_uint16 *)data_p;
            cyg_uint16 data_word;
            do {
                if(pushque_p == pushque_end) {
                    pushque_p[0] = pushr;
                    pushque_p[-1] |= FREESCALE_DSPI_PUSHR_EOQ_M;
                    DEBUG2_PRINTF(debug_format, 16, "BRK", pushque_p-1,
                                  pushque_p[-1], count);
                    return count;
                }
                data_word = *data16_p++;
                *pushque_p++ = pushr | data_word;
                DEBUG3_PRINTF(debug_format, 16, "ADD", pushque_p-1,
                              pushque_p[-1], count-2);
            } while((count -= 2) > 2);
            data_word = *data16_p;
            pushr |= data_word;
        }
    } else {
        do {
            if(pushque_p == pushque_end) {
                pushque_p[0] = pushr;
                pushque_p[-1] |= FREESCALE_DSPI_PUSHR_EOQ_M;
                DEBUG2_PRINTF(debug_format, 0, "BRK", pushque_p-1,
                              pushque_p[-1], count);
                return count;
            }
            *pushque_p++ = pushr;
            DEBUG3_PRINTF(debug_format, 0, "BRK", pushque_p-1, pushque_p[-1],
                          count-1);
        } while(--count > 1);
    }
    if(drop_cs) pushr &= ~FREESCALE_DSPI_PUSHR_CONT_M;
    pushque_p[1] = pushr;
    *pushque_p = pushr |= FREESCALE_DSPI_PUSHR_EOQ_M;
    DEBUG2_PRINTF(debug_format, data_p ? (bus_16bit ? 16 :8) : 0,
                  (drop_cs ? "END" : "SGM"), pushque_p, pushque_p[0], 0);
    return 0;
}

// Set up Tx DMA channel
// Used for transfers larger than DSPI FIFO

static inline cyg_uint32
tx_dma_channel_setup(cyg_spi_freescale_dspi_bus_t* dspi_bus,
                     cyg_uint8* data_buf, cyg_uint32 count,
                     cyg_bool bus_16bit,
                     cyg_uint32 pushr, cyg_bool drop_cs)
{
    cyghwr_hal_freescale_dma_set_t *dma_set_p = dspi_bus->setup_p->dma_set_p;
    cyghwr_hal_freescale_edma_t *edma_p =  dma_set_p->edma_p;
    volatile cyghwr_hal_freescale_edma_tcd_t *tcd_p;
    cyg_uint32 remain=0;
    cyg_uint32 dma_chan_i;

    remain = dma_pushque_fill(dspi_bus, data_buf, count, bus_16bit, pushr, drop_cs);
    dma_chan_i = SPI_DMA_CHAN_I(dma_set_p, TX);
    tcd_p = &edma_p->tcd[dma_chan_i];
    tcd_p->saddr = dspi_bus->pushque_p;
    DEBUG2_PRINTF("DSPI: Tx channel setup\n");
#if DEBUG_SPI >= 3
    hal_freescale_edma_transfer_diag(edma_p, dma_chan_i, true);
#endif
    return remain;
}

//-----------------------------------------------------------------------------
// Set SPI peripheral chip select.

static inline cyg_uint32
dspi_chip_select_set(cyg_int32 cs_i, cyg_bool pccse, cyg_bool assert)
{
    cyg_uint32 spi_pushr;

    if(cs_i < 0) {
        spi_pushr = 0;
    } else {
        if(pccse) {
            spi_pushr = cs_i;
        } else {
            if(cs_i < 5) {
                if(assert) {
                    cs_i = 1 << cs_i;
                    spi_pushr = FREESCALE_DSPI_PUSHR_PCS(cs_i);
                } else
                    spi_pushr = 0;
            } else {
                CYG_ASSERT(1, "DSPI: Peripheral Chip Select out of range.\n");
                spi_pushr = 1 << 5;
            }
        }
    }
    return spi_pushr;
}

//-----------------------------------------------------------------------------
// Execute SPI transaction.

static void spi_transaction_do (cyg_spi_device* device, cyg_bool tick_only,
                                 cyg_bool polled, cyg_uint32 count,
                                 const cyg_uint8* tx_data, cyg_uint8* rx_data,
                                 cyg_bool drop_cs)
{
    cyg_spi_freescale_dspi_bus_t* dspi_bus =
          (cyg_spi_freescale_dspi_bus_t*) device->spi_bus;
    cyg_spi_freescale_dspi_device_t* dspi_device =
          (cyg_spi_freescale_dspi_device_t*) device;
    cyg_bool bus_16bit = dspi_device->clocking.bus_16bit;
    cyghwr_devs_freescale_dspi_t* dspi_p = dspi_bus->setup_p->dspi_p;

    cyghwr_hal_freescale_dma_set_t* dma_set_p;
    cyghwr_hal_freescale_edma_t* edma_p = NULL;

    cyg_uint32 count_down;
    cyg_uint32 txfifo_n = dspi_bus->txfifo_n;
    cyg_uint32 pushr;
    cyg_uint32 pushque_n;
    cyg_uint32 dma_chan_rx_i = 0;
    cyg_uint32 dma_chan_tx_i = 0;

#if DEBUG_SPI >= 2
    cyg_uint32 first_turn = 1;
#endif

    DEBUG2_PRINTF("DSPI: transaction: count=%d drop_cs=%d\n", count, drop_cs);

    // Set up peripheral CS field. DSPI automatically asserts and deasserts CS
    pushr = dspi_chip_select_set(tick_only ? -1 : dspi_device->dev_num,
                                dspi_p->mcr & FREESCALE_DSPI_MCR_PCSSE_M, true);
    pushr |= FREESCALE_DSPI_PUSHR_CONT_M;

    dspi_fifo_clear(dspi_p);
    dspi_fifo_drain(dspi_p);

    pushque_n = dspi_bus->pushque_n;
    if(bus_16bit)
        txfifo_n *= 2;

    if((dma_set_p=dspi_bus->setup_p->dma_set_p)) {
        edma_p = dma_set_p->edma_p;
        // Set up the DMA channels.
        dma_chan_rx_i = SPI_DMA_CHAN_I(dma_set_p, RX);
        dma_chan_tx_i = SPI_DMA_CHAN_I(dma_set_p, TX);
        rx_dma_channel_setup(dma_set_p, (cyg_uint8*) rx_data,
                             bus_16bit, &edma_p->tcd[dma_chan_rx_i]);
        hal_freescale_edma_erq_enable(edma_p, dma_chan_rx_i);
    }

    if(!polled)
        cyg_drv_interrupt_unmask(dspi_bus->setup_p->intr_num);
    count_down = count;
    while(count_down) {
#if DEBUG_SPI >= 2
        if(first_turn) {
            if(dspi_bus->pushque_p)
                dspi_bus->pushque_p[0] |= FREESCALE_DSPI_PUSHR_CTCNT_M;
            first_turn = 0;
        }
#endif
        if(dma_set_p && (count_down > txfifo_n)) {
            // Transfer size is larger than DSPI FIFO
            // Use DMA Tx
            count_down = tx_dma_channel_setup(dspi_bus, (cyg_uint8*) tx_data,
                                              count_down, bus_16bit,
                                              pushr, drop_cs);
#if DEBUG_SPI >= 3
            hal_freescale_edma_transfer_diag(edma_p, dma_chan_rx_i, true);
#endif
            // Enable the Tx DMA / SPI controller.
            hal_freescale_edma_erq_enable(edma_p, dma_chan_tx_i);
            DSPI_EOQ_CLEAR(dspi_p);
        } else {
            // Transfer size fits within DSPI FIFO
            // No need for DMA Tx
            DSPI_EOQ_CLEAR(dspi_p);
            count_down = fifo_pushque_fill(dspi_bus, (cyg_uint8*) tx_data,
                                           count_down, bus_16bit,
                                           pushr, drop_cs);
#if DEBUG_SPI >= 3
            cyghwr_devs_freescale_dspi_diag(dspi_bus);
#endif
        }

        if(polled) {
            DEBUG2_PRINTF("DSPI Polled:\n");
            // Busy-wait for DSPI/DMA (polling for completion).
            while(!(dspi_p->sr & FREESCALE_DSPI_SR_EOQF_M));

            if(dma_set_p) // Disable the Tx DMA channel on completion.
                hal_freescale_edma_erq_disable(edma_p, dma_chan_tx_i);
        } else {
            // Wait for DSPI/DMA completion. (interrupt driven).
            cyg_drv_mutex_lock(&dspi_bus->transfer_mutex);
            cyg_drv_dsr_lock();

            DSPI_IRQ_ENABLE(dspi_p);
            DEBUG2_PRINTF("DSPI IRQ: Enabled\n");

            // Sit back and wait for the ISR/DSRs to signal completion.
            cyg_drv_cond_wait (&dspi_bus->transfer_done);

            cyg_drv_dsr_unlock();
            cyg_drv_mutex_unlock(&dspi_bus->transfer_mutex);
        }

        if(dma_set_p) {
            // Make sure that Rx has been drained by DMA.
            if(rx_data)
                while((dspi_p->sr & FREESCALE_DSPI_SR_RFDF_M));
        } else {
            // No DMA - "manually" drain Rx FIFO
            DEBUG2_PRINTF("DSPI FIFO: 'Manually' drain Rx fifo\n");
#if DEBUG_SPI >= 3
            cyghwr_devs_freescale_dspi_diag(dspi_bus);
#endif
            if(rx_data) {
                if(bus_16bit) {
                    cyg_uint16* rx_data16 = (cyg_uint16*) rx_data;
                    while(dspi_p->sr & FREESCALE_DSPI_SR_RXCTR_M)
                        *rx_data16++ = dspi_p->popr;
                    rx_data = (cyg_uint8*) rx_data16;
                } else {
                    while(dspi_p->sr & FREESCALE_DSPI_SR_RXCTR_M)
                        *rx_data++ = dspi_p->popr;
                }
            } else {
                dspi_fifo_drain(dspi_p);
            }
        }
        dspi_fifo_clear(dspi_p);
        // Prepare for next iteration
        if(tx_data) {
            tx_data += pushque_n;
            if(bus_16bit)
                tx_data += pushque_n;
        }
    }
    if(!polled)
        cyg_drv_interrupt_mask(dspi_bus->setup_p->intr_num);

    dspi_device->chip_sel = !drop_cs;
}

//-----------------------------------------------------------------------------
// Initialise SPI interfaces on startup.

static void CYGBLD_ATTRIB_C_INIT_PRI(CYG_INIT_BUS_SPI)
dspi_spi_init(void)
{
#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI0
  dspi_bus_setup (&cyg_spi_dspi_bus0);
#endif

#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI1
  dspi_bus_setup (&cyg_spi_dspi_bus1);
#endif

#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI2
  dspi_bus_setup (&cyg_spi_dspi_bus2);
#endif
}


//-----------------------------------------------------------------------------
// Start a SPI transaction.

static void dspi_transaction_begin(cyg_spi_device* device)
{
    cyg_spi_freescale_dspi_bus_t* dspi_bus =
          (cyg_spi_freescale_dspi_bus_t*) device->spi_bus;
    cyg_spi_freescale_dspi_device_t* dspi_device =
          (cyg_spi_freescale_dspi_device_t*) device;

    // On the first transaction, generate the values to be programmed into the
    // SPI configuration registers for this device and cache them. This avoids
    // having to recalculate the prescaler for every transaction.
    if(!(dspi_device->clocking.dspi_ctar))
        dspi_device->clocking.dspi_ctar = dspi_calc_ctar(&dspi_device->clocking,
                                                dspi_bus->clock_freq);
    // Set up the SPI controller.
    dspi_bus->setup_p->dspi_p->ctar[0] = dspi_device->clocking.dspi_ctar;
#if DEBUG_SPI >= 2
    cyghwr_devs_freescale_dspi_diag(dspi_bus);
#endif
}

//-----------------------------------------------------------------------------
// Run a transaction transfer.

static void dspi_transaction_transfer(cyg_spi_device* device, cyg_bool polled,
                                      cyg_uint32 count,
                                      const cyg_uint8* tx_data,
                                      cyg_uint8* rx_data,
                                      cyg_bool drop_cs)
{
    cyg_spi_freescale_dspi_device_t* dspi_device =
          (cyg_spi_freescale_dspi_device_t*) device;

    DEBUG3_PRINTF("Transaction tx_data = %p count=%d\n", tx_data, count);

    // Check for unsupported transactions.
    CYG_ASSERT (count > 0, "DSPI: Null transfer requested.");

    // We check that the buffers are half-word aligned and that count is a
    // multiple of two in order to carry out the 16-bit transfer.
    if (dspi_device->clocking.bus_16bit) {
        CYG_ASSERT (!(count & 1) && !((cyg_uint32) tx_data & 1) &&
                    !((cyg_uint32) rx_data & 1),
                    "DSPI: Misaligned data in 16-bit transfer.");
    }
    spi_transaction_do (device, false, polled, count, tx_data, rx_data, drop_cs);
}

//-----------------------------------------------------------------------------
// Carry out a bus tick operation - this drops chip select then pushes the
// required number of zeros onto the bus.

static void dspi_transaction_tick(cyg_spi_device* device, cyg_bool polled,
                                  cyg_uint32 count)
{
    cyg_spi_freescale_dspi_device_t* dspi_device =
          (cyg_spi_freescale_dspi_device_t*) device;

    // Check for unsupported transactions.
    CYG_ASSERT (count > 0, "DSPI: Null transfer requested.");

    // We check that count is a multiple of two in order
    // to carry out the 16-bit transfer.
    if (dspi_device->clocking.bus_16bit) {
        CYG_ASSERT (!(count & 1),
                    "DSPI: Misaligned data in 16-bit transfer.");
    }

    // Perform null transfer
    spi_transaction_do (device, true, polled, count, NULL, NULL, true);
}

//-----------------------------------------------------------------------------
// Terminate a SPI transaction, disabling the SPI controller.

static void dspi_transaction_end(cyg_spi_device* device)
{
    cyg_spi_freescale_dspi_bus_t* dspi_bus =
          (cyg_spi_freescale_dspi_bus_t*) device->spi_bus;
    cyg_spi_freescale_dspi_device_t* dspi_device =
          (cyg_spi_freescale_dspi_device_t*) device;

    cyghwr_hal_freescale_dma_set_t *dma_set_p = dspi_bus->setup_p->dma_set_p;
    cyghwr_hal_freescale_edma_t *edma_p;
    cyghwr_devs_freescale_dspi_t* dspi_p = dspi_bus->setup_p->dspi_p;

    if(dma_set_p) {
        edma_p = dma_set_p->edma_p;
        hal_freescale_edma_erq_disable(edma_p, SPI_DMA_CHAN_I(dma_set_p, TX));
        hal_freescale_edma_erq_disable(edma_p, SPI_DMA_CHAN_I(dma_set_p, RX));
    }

    if(dspi_device->chip_sel){
        // Clear peripheral CS by executing a dummy 4 bit transfer.
        dspi_p->pushr = FREESCALE_DSPI_PUSHR_EOQ_M | FREESCALE_DSPI_PUSHR_CTAS(1);
        DSPI_EOQ_CLEAR(dspi_p);
        while(!(dspi_p->sr & FREESCALE_DSPI_SR_EOQF_M));
        dspi_device->chip_sel = 0;
    }
}

//-----------------------------------------------------------------------------
// Get DSPI configuration parameter

static int dspi_get_config (cyg_spi_device* device, cyg_uint32 key,
                            void* buf, cyg_uint32* len)
{
    cyg_spi_freescale_dspi_bus_t* dspi_bus =
          (cyg_spi_freescale_dspi_bus_t*) device->spi_bus;
    cyg_spi_freescale_dspi_device_t* dspi_device =
          (cyg_spi_freescale_dspi_device_t*) device;
    cyg_uint32* data_p = buf;

    switch (key) {
    case CYG_IO_GET_CONFIG_SPI_CLOCKRATE :
        // Sanity check
        if (NULL == len) {
            CYG_ASSERT (false, "Freescale DSPI:"
                        " Null pointer as len argument for dspi_get_config().");
            return -1;
        } else if (sizeof(cyg_uint32) != *len) {
            CYG_ASSERT (false, "Freescale DSPI:"
                        " Invalid length with dspi_get_config().");
            return -1;
        } else if (NULL == buf) {
            CYG_ASSERT (false, "Freescale DSPI:"
                        " Null poiter as buf argument for dspi_get_config().");
            return -1;
        } else {
            cyg_uint32 ctar, dbr, br, pbr;

            ctar = dspi_device->clocking.dspi_ctar;
            dbr = (ctar & FREESCALE_DSPI_CTAR_DBR_M) >>
                  FREESCALE_DSPI_CTAR_DBR_S;
            br = (ctar & FREESCALE_DSPI_CTAR_BR_M) >>
                  FREESCALE_DSPI_CTAR_BR_S;
            pbr = (ctar & FREESCALE_DSPI_CTAR_PBR_M) >>
                  FREESCALE_DSPI_CTAR_PBR_S;
            *data_p = (dspi_bus->clock_freq * (1+dbr)) / (pbr * br);

            DEBUG2_PRINTF("DSPI Get Config: baud = %d\n", *data_p);

            return 0;
        }

    default :
        break;
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Change some SPI device configuration parameters

static int dspi_set_config(cyg_spi_device* device, cyg_uint32 key,
                           const void* buf, cyg_uint32* len)
{
    cyg_spi_freescale_dspi_device_t* dspi_device =
          (cyg_spi_freescale_dspi_device_t*) device;
    cyg_spi_freescale_dspi_bus_t* dspi_bus =
          (cyg_spi_freescale_dspi_bus_t*) device->spi_bus;

    cyg_uint32 regval;
    ctar_br_t brs;

    switch (key) {
    case CYG_IO_SET_CONFIG_SPI_CLOCKRATE :
        // Sanity check
        if (NULL == len) {
            CYG_ASSERT (false, "Freescale DSPI:"
                      " Null pointer as len argument for dspi_set_config().");
            return -1;
        } else if (sizeof(cyg_uint32) != *len) {
            CYG_ASSERT (false, "Freescale DSPI:"
                        " Invalid length with dspi_set_config().");
            return -1;
        } else if (NULL == buf) {
            CYG_ASSERT (false, "Freescale DSPI:"
                        " Null pointer as buf argument for dspi_set_config().");
            return -1;
        } else {
            // Get divider bits
            if (!dspi_ctar_brbf(&dspi_device->clocking, &brs,
                                   ctar_br, ctar_pbr, (cyg_uint32 *)buf, dspi_bus->clock_freq))
            {
                // Update the cache of the configuration register settings.
                regval = dspi_device->clocking.dspi_ctar;
                regval &= ~(FREESCALE_DSPI_CTAR_BR_M |
                            FREESCALE_DSPI_CTAR_PBR_M);
                regval |= FREESCALE_DSPI_CTAR_BR(brs.br) |
                      FREESCALE_DSPI_CTAR_PBR(brs.pbr);
                dspi_device->clocking.dspi_ctar = regval;

                return 0;

            } else {
                CYG_ASSERT (false, "Freescale DSPI:"
                            " Cannot run bus as slowly as requested.");
                return -1;
            }
        }
    default :
        break;
    }
    return -1;
}

#if DEBUG_SPI
//----------------------------------------------------------------------------
// Print out a DSPI array state
// Helper for cyghwr_devs_freescale_dspi_diag()

void cyghwr_devs_freescale_dspi_diag_array(char* name_p,
                                           volatile cyg_uint32* array_p,
                                           cyg_uint32 array_n)
{
    diag_printf("%s %p[%u]: ", name_p, array_p, array_n);
    for(; array_n; array_n--) {
        diag_printf(" 0x%08x", *array_p++);
    }
    diag_printf("\n");
}

//----------------------------------------------------------------------------
// Print out DSPI state

void cyghwr_devs_freescale_dspi_diag(cyg_spi_freescale_dspi_bus_t* dspi_bus_p)
{
    cyghwr_devs_freescale_dspi_t* dspi_p = dspi_bus_p->setup_p->dspi_p;

    diag_printf("DSPI %p\n", dspi_p);
    diag_printf("  MCR = 0x%08x TCR = 0x%08x\n", dspi_p->mcr, dspi_p->tcr);
    cyghwr_devs_freescale_dspi_diag_array("  CTAR",  dspi_p->ctar,
                                          CYGHWR_DEVS_SPI_FREESCALE_DSPI_CTAR_NUM);
    diag_printf("  SR = 0x%08x RSER = 0x%08x PUSHR = 0x%08x POPR = 0x%08x\n",
                dspi_p->sr, dspi_p->rser, dspi_p->pushr, dspi_p->popr);
    cyghwr_devs_freescale_dspi_diag_array("  TXFR", dspi_p->txfr, dspi_bus_p->txfifo_n);
    cyghwr_devs_freescale_dspi_diag_array("  RXFR", dspi_p->rxfr, dspi_bus_p->rxfifo_n);
}
#endif // DEBUG_SPI

#endif // defined(CYGHWR_DEVS_SPI_FREESCALE_DSPIx)

//=============================================================================
