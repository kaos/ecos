//===========================================================================
//
//      hal_freescale_edma.c
//
//      Freescale eDMA support library
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Date:          2011-11-04
// Purpose:       Freescale eDMA specific functions
// Description:
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_freescale_edma.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/hal/hal_if.h>             // HAL header
#include <cyg/hal/freescale_edma.h>     // Freescale eDMA defs

// Channel priority register index
const cyg_uint8 const PRICHAN_I[CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM] =
{
    FREESCALE_DMA_PRI_CH0,  FREESCALE_DMA_PRI_CH1,
    FREESCALE_DMA_PRI_CH2,  FREESCALE_DMA_PRI_CH3,
    FREESCALE_DMA_PRI_CH4,  FREESCALE_DMA_PRI_CH5,
    FREESCALE_DMA_PRI_CH6,  FREESCALE_DMA_PRI_CH7,
    FREESCALE_DMA_PRI_CH8,  FREESCALE_DMA_PRI_CH9,
    FREESCALE_DMA_PRI_CH10, FREESCALE_DMA_PRI_CH11,
    FREESCALE_DMA_PRI_CH12, FREESCALE_DMA_PRI_CH13,
    FREESCALE_DMA_PRI_CH14, FREESCALE_DMA_PRI_CH15
#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE
    ,
    FREESCALE_DMA_PRI_CH16, FREESCALE_DMA_PRI_CH17,
    FREESCALE_DMA_PRI_CH18, FREESCALE_DMA_PRI_CH19,
    FREESCALE_DMA_PRI_CH20, FREESCALE_DMA_PRI_CH21,
    FREESCALE_DMA_PRI_CH22, FREESCALE_DMA_PRI_CH23,
    FREESCALE_DMA_PRI_CH24, FREESCALE_DMA_PRI_CH25,
    FREESCALE_DMA_PRI_CH26, FREESCALE_DMA_PRI_CH27,
    FREESCALE_DMA_PRI_CH28, FREESCALE_DMA_PRI_CH29,
    FREESCALE_DMA_PRI_CH30, FREESCALE_DMA_PRI_CH31
#endif
};

// Find an eDMA channel with given priority
static volatile cyg_uint8*
hal_freescale_edma_find_chan_with_pri(cyghwr_hal_freescale_edma_t *edma_p,
                                 cyg_uint32 pri, cyg_uint32 group_i)
{
    volatile cyg_uint8 *chan_p;

    for(chan_p = &edma_p->dchpri[group_i * CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE];
        chan_p < &edma_p->dchpri[group_i * CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE +
                                 CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE];
        chan_p++)
    {
        if((*chan_p & FREESCALE_EDMA_DCHPRI_CHPRI_M) == pri) break;
    }
#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE
    if(0 == group_i){
        if(chan_p >= &edma_p->dchpri[CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE])
            chan_p = NULL;
    }
#endif
    if(chan_p >= &edma_p->dchpri[CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM])
        chan_p = NULL;

    return chan_p;
}

// Initialize an eDMA channel
// If DMA prority change is required than old priority is assigned to the channel
// that before this call had requested priority.
void
hal_freescale_edma_init_1chan(
                           cyghwr_hal_freescale_edma_t *edma_p,
                           const cyghwr_hal_freescale_dma_chan_set_t *chan_p)
{
    cyg_uint8 oldprio;
    cyghwr_hal_freescale_dmamux_t *dmamux_p;
    volatile cyg_uint8 *prev_ch_reqprio_p; // Previous chan with req. prio.
    volatile cyg_uint8 *chcfg_p;
    cyg_uint32         group_i;

#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE
    dmamux_p = chan_p->dma_chan_i < CYGNUM_HAL_FREESCALE_DMAMUX_CHAN_NUM ?
                                         CYGHWR_IO_FREESCALE_DMAMUX0_P :
                                         CYGHWR_IO_FREESCALE_DMAMUX1_P;
#else
    dmamux_p = CYGHWR_IO_FREESCALE_DMAMUX0_P;
#endif
    chcfg_p = &dmamux_p->chcfg[chan_p->dma_chan_i % CYGNUM_HAL_FREESCALE_DMAMUX_CHAN_NUM];
    edma_p->cerq = chan_p->dma_chan_i;

    if(chan_p->dma_src & FREESCALE_DMAMUX_CHCFG_SOURCE_M) {
        *chcfg_p = chan_p->dma_src;
    } else if(!(chan_p->dma_src & FREESCALE_DMAMUX_CHCFG_ASIS)) {
        *chcfg_p = 0;
    }

    if((chan_p->dma_prio != FREESCALE_EDMA_DCHPRI_ASIS) &&
       (edma_p->dchpri[PRICHAN_I[chan_p->dma_chan_i]] != chan_p->dma_prio))
    {
        group_i = chan_p->dma_chan_i >= CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE ? 1 : 0;
        if((prev_ch_reqprio_p =
            hal_freescale_edma_find_chan_with_pri(edma_p, chan_p->dma_prio, group_i)))
        {
            oldprio = edma_p->dchpri[PRICHAN_I[chan_p->dma_chan_i]];
            edma_p->dchpri[PRICHAN_I[chan_p->dma_chan_i]] = chan_p->dma_prio;
            *prev_ch_reqprio_p = oldprio;
        }
    }
}

// Init DMA controller

const cyg_uint32 FREESCALE_EDMA_CR_INI = 0
#ifdef CYGOPT_HAL_FREESCALE_EDMA_EMLM
       | FREESCALE_EDMA_CR_EMLM_M
#endif
#ifdef CYGOPT_HAL_FREESCALE_EDMA_CLM
       | FREESCALE_EDMA_CR_CLM_M
#endif
#ifdef CYGOPT_HAL_FREESCALE_EDMA_ERCA
       | FREESCALE_EDMA_CR_ERCA_M
#endif
#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > 16
#ifdef CYGOPT_HAL_FREESCALE_EDMA_ERGA
       | FREESCALE_EDMA_CR_ERGA_M
#endif
       | FREESCALE_EDMA_GR_PRI(0, CYGNUM_HAL_FREESCALE_EDMA_GR0_PRIO)
       | FREESCALE_EDMA_GR_PRI(1, CYGNUM_HAL_FREESCALE_EDMA_GR1_PRIO)
#endif
      ;

void
hal_freescale_edma_init(cyghwr_hal_freescale_edma_t *edma_p)
{
    cyg_uint32 regval;

    regval = edma_p->cr;
#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > 16
    regval &= ~(FREESCALE_EDMA_GR_PRI(0, 3) | FREESCALE_EDMA_GR_PRI(1, 3));
#endif
    regval |= FREESCALE_EDMA_CR_INI;
    edma_p->cr = regval;
}

// Initialize a set of DMA channels
void
hal_freescale_edma_init_chanset(cyghwr_hal_freescale_dma_set_t *inidat_p)
{
    cyghwr_hal_freescale_edma_t *edma_p;
    const cyghwr_hal_freescale_dma_chan_set_t *chan_p;

    edma_p = inidat_p->edma_p = CYGHWR_HAL_FREESCALE_EDMA0_P;

    hal_freescale_edma_init(edma_p);

    for(chan_p = inidat_p->chan_p;
        chan_p < inidat_p->chan_p + inidat_p->chan_n;
        chan_p++)
    {
        hal_freescale_edma_init_1chan(edma_p, chan_p);
    }
    edma_p->es = 0;
}

#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM == 32
# define DMA_CHANMASK_FORMAT "0x%08x"
#else
# define DMA_CHANMASK_FORMAT "0x%04x"
#endif

#define EDMA_DIAG_PRINTF_FORMAT(__mf) "CR=0x%08x ES=0x%08x ERQ=" __mf \
        " INT=" __mf " ERR=" __mf " HRS=" __mf "\n"

// Display DMA configuration
void
hal_freescale_edma_diag(const cyghwr_hal_freescale_dma_set_t *inidat_p, cyg_uint32 mask)
{
    cyghwr_hal_freescale_edma_t *edma_p;
    cyghwr_hal_freescale_dmamux_t *dmamux_p;
    const cyghwr_hal_freescale_dma_chan_set_t *chan_p;
    cyg_uint8 chan_i;
    cyg_uint32 chan_p_i;

    edma_p = inidat_p->edma_p;
    diag_printf(EDMA_DIAG_PRINTF_FORMAT(DMA_CHANMASK_FORMAT),
                edma_p->cr, edma_p->es,
                edma_p->erq, edma_p->irq, edma_p->err, edma_p->hrs);

    for(chan_i = 0; chan_i < CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM; chan_i++){
        if(mask & 0x1){
            chan_p = inidat_p->chan_p;
#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE
            dmamux_p = chan_i < CYGNUM_HAL_FREESCALE_DMAMUX_CHAN_NUM ?
                                    CYGHWR_IO_FREESCALE_DMAMUX0_P :
                                    CYGHWR_IO_FREESCALE_DMAMUX1_P;
#else
            dmamux_p = CYGHWR_IO_FREESCALE_DMAMUX0_P;
#endif
            diag_printf("Chan %2d: CHCFG=0x%02x (%2d) DCHPRI=0x%02x dmamux[%c]=%p", chan_i,
                        dmamux_p->chcfg[chan_i % 16],
                        FREESCALE_DMAMUX_CHCFG_SOURCE(dmamux_p->chcfg[chan_i % 16]),
                        edma_p->dchpri[PRICHAN_I[chan_i]],
                        CYGHWR_IO_FREESCALE_DMAMUX0_P == dmamux_p ? '0' : (
#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > CYGNUM_HAL_FREESCALE_EDMA_GROUP_SIZE
                        CYGHWR_IO_FREESCALE_DMAMUX1_P == dmamux_p ? '1' :
#endif
                        '?'), dmamux_p);
            for(chan_p_i = 0; chan_p_i < inidat_p->chan_n; chan_p_i++){
                if(chan_p->dma_chan_i == chan_i){
                    diag_printf(" ISR_NUM=%2d[0x%02x] ISR_PRI=%3d[0x%02x]",
                                chan_p->isr_num, chan_p->isr_num,
                                chan_p->isr_prio, chan_p->isr_prio);
                }
                chan_p++;
            }
            diag_printf("\n");
        }
        mask >>= 1;
    }
}

// Initialize eDMA channel TCD
void
hal_freescale_edma_transfer_init(cyghwr_hal_freescale_edma_t *edma_p,
                                 cyg_uint8 chan_i,
                                 const cyghwr_hal_freescale_edma_tcd_t *tcd_cfg_p)
{
    HAL_DMA_TRANSFER_CLEAR(edma_p, chan_i);
    edma_p->tcd[chan_i] = *tcd_cfg_p;
}

// Show eDMA TCD
void hal_freescale_edma_tcd_diag(cyghwr_hal_freescale_edma_tcd_t *tcd_p, cyg_int32 chan_i, const char *prefix)
{
    if(chan_i < 0) {
        diag_printf("TCD %p chan %s:\n", tcd_p, prefix);
        prefix = "";
    } else {
        diag_printf("%sTCD %p chan %d:\n", "", tcd_p, chan_i);
    }

        diag_printf("%s    saddr=%p soff=0x%04x, attr=0x%04x\n", prefix,
                    tcd_p->saddr, tcd_p->soff, tcd_p->attr);
        diag_printf("%s    daddr=%p doff=0x%04x\n", prefix,
                    tcd_p->daddr, tcd_p->doff);
        diag_printf("%s    nbytes=%d [0x%08x], slast=%d [0x%08x]\n", prefix,
                    tcd_p->nbytes.mlno, tcd_p->nbytes.mlno,
                    tcd_p->slast, tcd_p->slast);
        diag_printf("%s    %s=%d [%p]\n", prefix,
                    (tcd_p->csr & FREESCALE_EDMA_CSR_ESG_M) ? "sga" : "dlast",
                    tcd_p->dlast, tcd_p->sga);
        diag_printf("%s    biter = %d, citer = %d\n", prefix,
                    tcd_p->biter.elinkno, tcd_p->citer.elinkno);
        diag_printf("%s    CSR=0x%04x\n", prefix, tcd_p->csr);
}

// Show eDMA TCD set
void hal_freescale_edma_transfer_diag(cyghwr_hal_freescale_edma_t
                                      *edma_p, cyg_uint8 chan_i, cyg_bool recurse)
{
    cyghwr_hal_freescale_edma_tcd_t *tcd_p;
    const char *prefix = "";

    for(tcd_p = &edma_p->tcd[chan_i]; tcd_p; tcd_p = tcd_p->sga){
        hal_freescale_edma_tcd_diag(tcd_p, chan_i, prefix);
        if(!(recurse && (tcd_p->csr & FREESCALE_EDMA_CSR_ESG_M)))
            break;
        prefix = "  ";
    }
}

// end of freescale_dma.h
