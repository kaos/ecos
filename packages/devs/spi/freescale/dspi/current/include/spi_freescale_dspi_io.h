#ifndef CYGONCE_DEVS_SPI_FREESCALE_DSPI_IO_H
#define CYGONCE_DEVS_SPI_FREESCALE_DSPI_IO_H
//=============================================================================
//
//      spi_freescale_dspi_io.h
//
//      IO definitions for Freescale DSPI.
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
// Date:        2011-11-03
// Purpose:     Freescale DSPI I/O definitions.
// Description:
// Usage:       #include <cyg/io/spi_freescale_dspi_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================


// ----------------------------------------------------------------------------
// DSPI

typedef volatile struct cyghwr_devs_freescale_dspi_s {
    cyg_uint32 mcr;    // Module Configuration Register
    cyg_uint32 reserved_0;
    cyg_uint32 tcr;    // Transfer Count Register
     // Clock and Transfer Attributes Register
    cyg_uint32 ctar[CYGHWR_DEVS_SPI_FREESCALE_DSPI_CTAR_NUM];
#if CYGHWR_DEVS_SPI_FREESCALE_DSPI_CTAR_NUM < 8
    cyg_uint32 reserved_1[8-CYGHWR_DEVS_SPI_FREESCALE_DSPI_CTAR_NUM];
#endif
    cyg_uint32 sr;     // Status Register
    cyg_uint32 rser;   // DMA/IRQ Request Select and Enable Register
    cyg_uint32 pushr;  // TX FIFO PUSH Register
    cyg_uint32 popr;   // RX FIFO POP Register
     // Transmit FIFO Registers
    cyg_uint32 txfr[CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE];
#if CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE < 16
    cyg_uint32 reserved_2[16-CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE];
#endif
    // Receive FIFO Registers
    cyg_uint32 rxfr[CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE];
#if CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE < 16
    cyg_uint32 reserved_3[16-CYGHWR_DEVS_SPI_FREESCALE_DSPI_FIFO_SIZE];
#endif
} cyghwr_devs_freescale_dspi_t;

// MCR Bit Fields
#define FREESCALE_DSPI_MCR_HALT_M                       0x1
#define FREESCALE_DSPI_MCR_HALT_S                       0
#define FREESCALE_DSPI_MCR_SMPL_PT_M                    0x300
#define FREESCALE_DSPI_MCR_SMPL_PT_S                    8
#define FREESCALE_DSPI_MCR_SMPL_PT(__val)               \
        VALUE_(FREESCALE_DSPI_MCR_SMPL_PT_S, __val)
#define FREESCALE_DSPI_MCR_CLR_RXF_M                    0x400
#define FREESCALE_DSPI_MCR_CLR_RXF_S                    10
#define FREESCALE_DSPI_MCR_CLR_TXF_M                    0x800
#define FREESCALE_DSPI_MCR_CLR_TXF_S                    11
#define FREESCALE_DSPI_MCR_DIS_RXF_M                    0x1000
#define FREESCALE_DSPI_MCR_DIS_RXF_S                    12
#define FREESCALE_DSPI_MCR_DIS_TXF_M                    0x2000
#define FREESCALE_DSPI_MCR_DIS_TXF_S                    13
#define FREESCALE_DSPI_MCR_MDIS_M                       0x4000
#define FREESCALE_DSPI_MCR_MDIS_S                       14
#define FREESCALE_DSPI_MCR_DOZE_M                       0x8000
#define FREESCALE_DSPI_MCR_DOZE_S                       15
#define FREESCALE_DSPI_MCR_PCSIS_M                      0x3F0000
#define FREESCALE_DSPI_MCR_PCSIS_S                      16
#define FREESCALE_DSPI_MCR_PCSIS(__val)                 \
        VALUE_(FREESCALE_DSPI_MCR_PCSIS_S, __val)
#define FREESCALE_DSPI_MCR_ROOE_M                       0x1000000
#define FREESCALE_DSPI_MCR_ROOE_S                       24
#define FREESCALE_DSPI_MCR_PCSSE_M                      0x2000000
#define FREESCALE_DSPI_MCR_PCSSE_S                      25
#define FREESCALE_DSPI_MCR_MTFE_M                       0x4000000
#define FREESCALE_DSPI_MCR_MTFE_S                       26
#define FREESCALE_DSPI_MCR_FRZ_M                        0x8000000
#define FREESCALE_DSPI_MCR_FRZ_S                        27
#define FREESCALE_DSPI_MCR_DCONF_M                      0x30000000
#define FREESCALE_DSPI_MCR_DCONF_S                      28
#define FREESCALE_DSPI_MCR_DCONF(__val)                 \
        VALUE_(FREESCALE_DSPI_MCR_DCONF_S, __val)
#define FREESCALE_DSPI_MCR_CONT_SCKE_M                  0x40000000
#define FREESCALE_DSPI_MCR_CONT_SCKE_S                  30
#define FREESCALE_DSPI_MCR_MSTR_M                       0x80000000
#define FREESCALE_DSPI_MCR_MSTR_S                       31
// TCR Bit Fields
#define FREESCALE_DSPI_TCR_DSPI_TCNT_M                  0xFFFF0000
#define FREESCALE_DSPI_TCR_DSPI_TCNT_S                  16
#define FREESCALE_DSPI_TCR_DSPI_TCNT(__val)             \
        VALUE_(FREESCALE_DSPI_TCR_DSPI_TCNT_S, __val)
// CTAR Bit Fields
#define FREESCALE_DSPI_CTAR_BR_M                        0xF
#define FREESCALE_DSPI_CTAR_BR_S                        0
#define FREESCALE_DSPI_CTAR_BR(__val)                   \
        VALUE_(FREESCALE_DSPI_CTAR_BR_S, __val)
#define FREESCALE_DSPI_CTAR_DT_M                        0xF0
#define FREESCALE_DSPI_CTAR_DT_S                        4
#define FREESCALE_DSPI_CTAR_DT(__val)                   \
        VALUE_(FREESCALE_DSPI_CTAR_DT_S, __val)
#define FREESCALE_DSPI_CTAR_ASC_M                       0xF00
#define FREESCALE_DSPI_CTAR_ASC_S                       8
#define FREESCALE_DSPI_CTAR_ASC(__val)                  \
        VALUE_(FREESCALE_DSPI_CTAR_ASC_S, __val)
#define FREESCALE_DSPI_CTAR_CSSCK_M                     0xF000
#define FREESCALE_DSPI_CTAR_CSSCK_S                     12
#define FREESCALE_DSPI_CTAR_CSSCK(__val)                \
        VALUE_(FREESCALE_DSPI_CTAR_CSSCK_S, __val)
#define FREESCALE_DSPI_CTAR_PBR_M                       0x30000
#define FREESCALE_DSPI_CTAR_PBR_S                       16
#define FREESCALE_DSPI_CTAR_PBR(__val)                  \
        VALUE_(FREESCALE_DSPI_CTAR_PBR_S, __val)
#define FREESCALE_DSPI_CTAR_PDT_M                       0xC0000
#define FREESCALE_DSPI_CTAR_PDT_S                       18
#define FREESCALE_DSPI_CTAR_PDT(__val)                  \
        VALUE_(FREESCALE_DSPI_CTAR_PDT_S, __val)
#define FREESCALE_DSPI_CTAR_PASC_M                      0x300000
#define FREESCALE_DSPI_CTAR_PASC_S                      20
#define FREESCALE_DSPI_CTAR_PASC(__val)                 \
        VALUE_(FREESCALE_DSPI_CTAR_PASC_S, __val)
#define FREESCALE_DSPI_CTAR_PCSSCK_S                    22
#define FREESCALE_DSPI_CTAR_PCSSCK(__val)               \
        VALUE_(FREESCALE_DSPI_CTAR_PCSSCK_S, __val)
#define FREESCALE_DSPI_CTAR_LSBFE_S                     24
#define FREESCALE_DSPI_CTAR_LSBFE_M                     0x1000000

#define FREESCALE_DSPI_CTAR_FMSZ_M                      0x78000000
#define FREESCALE_DSPI_CTAR_FMSZ_S                      27
#define FREESCALE_DSPI_CTAR_FMSZ(__val)                 \
        VALUE_(FREESCALE_DSPI_CTAR_FMSZ_S, __val)
#define FREESCALE_DSPI_CTAR_DBR_M                       0x80000000
#define FREESCALE_DSPI_CTAR_DBR_S                       31
// CTAR_SLAVE Bit Fields
#define FREESCALE_DSPI_CTAR_SLAVE_CPHA_M                0x2000000
#define FREESCALE_DSPI_CTAR_SLAVE_CPHA_S                25
#define FREESCALE_DSPI_CTAR_SLAVE_CPOL_M                0x4000000
#define FREESCALE_DSPI_CTAR_SLAVE_CPOL_S                26
#define FREESCALE_DSPI_CTAR_SLAVE_FMSZ_M                0xF8000000
#define FREESCALE_DSPI_CTAR_SLAVE_FMSZ_S                27
#define FREESCALE_DSPI_CTAR_SLAVE_FMSZ(__val)           \
        VALUE_(FREESCALE_DSPI_CTAR_SLAVE_FMSZ_S, __val)
// SR Bit Fields
#define FREESCALE_DSPI_SR_POPNXTPTR_M                   0xF
#define FREESCALE_DSPI_SR_POPNXTPTR_S                   0
#define FREESCALE_DSPI_SR_POPNXTPTR(__val)              \
        VALUE_(FREESCALE_DSPI_SR_POPNXTPTR_S, __val)
#define FREESCALE_DSPI_SR_RXCTR_M                       0xF0
#define FREESCALE_DSPI_SR_RXCTR_S                       4
#define FREESCALE_DSPI_SR_RXCTR(__val)                  \
        VALUE_(FREESCALE_DSPI_SR_RXCTR_S, __val)
#define FREESCALE_DSPI_SR_TXNXTPTR_M                    0xF00
#define FREESCALE_DSPI_SR_TXNXTPTR_S                    8
#define FREESCALE_DSPI_SR_TXNXTPTR(__val)               \
        VALUE_(FREESCALE_DSPI_SR_TXNXTPTR_S, __val)
#define FREESCALE_DSPI_SR_TXCTR_M                       0xF000
#define FREESCALE_DSPI_SR_TXCTR_S                       12
#define FREESCALE_DSPI_SR_TXCTR(__val)                  \
        VALUE_(FREESCALE_DSPI_SR_TXCTR_S, __val)
#define FREESCALE_DSPI_SR_RFDF_M                        0x20000
#define FREESCALE_DSPI_SR_RFDF_S                        17
#define FREESCALE_DSPI_SR_RFOF_M                        0x80000
#define FREESCALE_DSPI_SR_RFOF_S                        19
#define FREESCALE_DSPI_SR_TFFF_M                        0x2000000
#define FREESCALE_DSPI_SR_TFFF_S                        25
#define FREESCALE_DSPI_SR_TFUF_M                        0x8000000
#define FREESCALE_DSPI_SR_TFUF_S                        27
#define FREESCALE_DSPI_SR_EOQF_M                        0x10000000
#define FREESCALE_DSPI_SR_EOQF_S                        28
#define FREESCALE_DSPI_SR_TXRXS_M                       0x40000000
#define FREESCALE_DSPI_SR_TXRXS_S                       30
#define FREESCALE_DSPI_SR_TCF_M                         0x80000000
#define FREESCALE_DSPI_SR_TCF_S                         31

#define FREESCALE_DSPI_CLEAR_FIFOS (FREESCALE_DSPI_SR_TFUF_M |\
                                    FREESCALE_DSPI_SR_RFOF_M)

// RSER Bit Fields
#define FREESCALE_DSPI_RSER_RFDF_DIRS_M                 0x10000
#define FREESCALE_DSPI_RSER_RFDF_DIRS_S                 16
#define FREESCALE_DSPI_RSER_RFDF_RE_M                   0x20000
#define FREESCALE_DSPI_RSER_RFDF_RE_S                   17
#define FREESCALE_DSPI_RSER_RFOF_RE_M                   0x80000
#define FREESCALE_DSPI_RSER_RFOF_RE_S                   19
#define FREESCALE_DSPI_RSER_TFFF_DIRS_M                 0x1000000
#define FREESCALE_DSPI_RSER_TFFF_DIRS_S                 24
#define FREESCALE_DSPI_RSER_TFFF_RE_M                   0x2000000
#define FREESCALE_DSPI_RSER_TFFF_RE_S                   25
#define FREESCALE_DSPI_RSER_TFUF_RE_M                   0x8000000
#define FREESCALE_DSPI_RSER_TFUF_RE_S                   27
#define FREESCALE_DSPI_RSER_EOQF_RE_M                   0x10000000
#define FREESCALE_DSPI_RSER_EOQF_RE_S                   28
#define FREESCALE_DSPI_RSER_TCF_RE_M                    0x80000000
#define FREESCALE_DSPI_RSER_TCF_RE_S                    31
// PUSHR Bit Fields
#define FREESCALE_DSPI_PUSHR_TXDATA_M                   0xFFFF
#define FREESCALE_DSPI_PUSHR_TXDATA_S                   0
#define FREESCALE_DSPI_PUSHR_TXDATA(__val)              \
        VALUE_(FREESCALE_DSPI_PUSHR_TXDATA_S, __val)
#define FREESCALE_DSPI_PUSHR_PCS_M                      0x3F0000
#define FREESCALE_DSPI_PUSHR_PCS_S                      16
#define FREESCALE_DSPI_PUSHR_PCS(__val)                 VALUE_(FREESCALE_DSPI_PUSHR_PCS_S, __val)
#define FREESCALE_DSPI_PUSHR_CTCNT_M                    0x4000000
#define FREESCALE_DSPI_PUSHR_CTCNT_S                    26
#define FREESCALE_DSPI_PUSHR_EOQ_M                      0x8000000
#define FREESCALE_DSPI_PUSHR_EOQ_S                      27
#define FREESCALE_DSPI_PUSHR_CTAS_M                     0x70000000
#define FREESCALE_DSPI_PUSHR_CTAS_S                     28
#define FREESCALE_DSPI_PUSHR_CTAS(__val)                \
        VALUE_(FREESCALE_DSPI_PUSHR_CTAS_S, __val)
#define FREESCALE_DSPI_PUSHR_CONT_M                     0x80000000
#define FREESCALE_DSPI_PUSHR_CONT_S                     31
// PUSHR_SLAVE Bit Fields
#define FREESCALE_DSPI_PUSHR_SLAVE_TXDATA_M             0xFFFFFFFF
#define FREESCALE_DSPI_PUSHR_SLAVE_TXDATA_S             0
#define FREESCALE_DSPI_PUSHR_SLAVE_TXDATA(__val)        \
VALUE_(FREESCALE_DSPI_PUSHR_SLAVE_TXDATA_S, __val)

#define FREESCALE_DSPI_PUSHR_PCS_CLEAR(__val)\
CYG_MACRO_START\
    __val &= ~FREESCALE_DSPI_PUSHR_PCS_M; \
CYG_MACRO_END

// POPR Bit Fields
#define FREESCALE_DSPI_POPR_RXDATA_M                    0xFFFFFFFF
#define FREESCALE_DSPI_POPR_RXDATA_S                    0
#define FREESCALE_DSPI_POPR_RXDATA(__val)               \
        VALUE_(FREESCALE_DSPI_POPR_RXDATA_S, __val)
// TXFR Bit Fields
#define FREESCALE_DSPI_TXFR_TXDATA_M                    0xFFFF
#define FREESCALE_DSPI_TXFR_TXDATA_S                    0
#define FREESCALE_DSPI_TXFR_TXCMD_TXDATA_M              0xFFFF0000
#define FREESCALE_DSPI_TXFR_TXCMD_TXDATA_S              16
#define FREESCALE_DSPI_TXFR_TXCMD_TXDATA(__fr,__val)    \
        VALUE_(FREESCALE_DSPI_TXFR##__fr##_TXCMD_TXDATA_S, __val)
// RXFR Bit Fields
#define FREESCALE_DSPI_RXFR_RXDATA_M                    0xFFFFFFFF
#define FREESCALE_DSPI_RXFR_RXDATA_S                    0
#define FREESCALE_DSPI_RXFR_RXDATA(__fr,__val)          \
        VALUE_(FREESCALE_DSPI_RXFR##_fr##_RXDATA_S, __val)

// Borrow following macros from HAL

// CYGADDR_IO_SPI_FREESCALE_DSPI0_P
// CYGADDR_IO_SPI_FREESCALE_DSPI1_P
// CYGADDR_IO_SPI_FREESCALE_DSPI2_P

// CYGHWR_IO_FREESCALE_DSPI_PIN(__pin)

// CYGHWR_IO_FREESCALE_SPIx_PIN_SIN
// CYGHWR_IO_FREESCALE_SPIx_PIN_SOUT
// CYGHWR_IO_FREESCALE_SPIx_PIN_SCK

// CYGHWR_IO_FREESCALE_SPIx_PIN_CSn

//=============================================================================
#endif // CYGONCE_DEVS_SPI_FREESCALE_DSPI_IO_H
