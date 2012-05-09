#ifndef CYGONCE_FREESCALE_EDMA_H
#define CYGONCE_FREESCALE_EDMA_H
//===========================================================================
//
//      freescale_edma.h
//
//      Freescale eDMA specific registers
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
// Purpose:       Freescale eDMA specific registers
// Description:
// Usage:         #include <cyg/hal/freescale_edma.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <pkgconf/hal.h>
#if defined CYGHWR_HAL_EDMA_TCD_SECTION || defined CYGHWR_HAL_EDMA_MEM_SECTION
# include <cyg/infra/cyg_type.h>
#endif

// ----------------------------------------------------------------------------
// DMAMUX DMA Multiplexer

// DMAMUX - Peripheral register structure
typedef volatile struct cyghwr_hal_freescale_dmamux_s {
    cyg_uint8 chcfg[CYGNUM_HAL_FREESCALE_DMAMUX_CHAN_NUM];   // Channel Configuration Register
} cyghwr_hal_freescale_dmamux_t;

// DMAMUX - Peripheral instance base addresses - defined in HAL (typically var_io_devs.h)
// CYGHWR_IO_FREESCALE_DMAMUX0_P defined in HAL
// CYGHWR_IO_FREESCALE_DMAMUX1_P defined in HAL


// ----------------------------------------------------------------------------
// DMAMUX Register Masks

// CHCFG Bit Fields
#define FREESCALE_DMAMUX_CHCFG_SOURCE_M      0x3F
#define FREESCALE_DMAMUX_CHCFG_SOURCE(__val) \
            (__val & FREESCALE_DMAMUX_CHCFG_SOURCE_M)
#define FREESCALE_DMAMUX_CHCFG_TRIG_M        0x40
#define FREESCALE_DMAMUX_CHCFG_TRIG_S        6
#define FREESCALE_DMAMUX_CHCFG_ENBL_M        0x80
#define FREESCALE_DMAMUX_CHCFG_ENBL_S        7
#define FREESCALE_DMAMUX_CHCFG_ASIS          FREESCALE_DMAMUX_CHCFG_ENBL_M

// DMAMUX DMA request sources
// Provided by HAL (typically var_io_devs.h)
#define FREESCALE_DMAMUX_SRC(__src) (_src)

//---------------------------------------------------------------------------
// eDMA

// Indices for cyghwr_hal_freescale_edma_t::dchpri[]
enum {
    FREESCALE_DMA_PRI_CH3,  FREESCALE_DMA_PRI_CH2,
    FREESCALE_DMA_PRI_CH1,  FREESCALE_DMA_PRI_CH0,
    FREESCALE_DMA_PRI_CH7,  FREESCALE_DMA_PRI_CH6,
    FREESCALE_DMA_PRI_CH5,  FREESCALE_DMA_PRI_CH4,
    FREESCALE_DMA_PRI_CH11, FREESCALE_DMA_PRI_CH10,
    FREESCALE_DMA_PRI_CH9,  FREESCALE_DMA_PRI_CH8,
    FREESCALE_DMA_PRI_CH15, FREESCALE_DMA_PRI_CH14,
    FREESCALE_DMA_PRI_CH13, FREESCALE_DMA_PRI_CH12
#if CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM > 16
    ,
    FREESCALE_DMA_PRI_CH19, FREESCALE_DMA_PRI_CH18,
    FREESCALE_DMA_PRI_CH17, FREESCALE_DMA_PRI_CH16,
    FREESCALE_DMA_PRI_CH23, FREESCALE_DMA_PRI_CH22,
    FREESCALE_DMA_PRI_CH21, FREESCALE_DMA_PRI_CH20,
    FREESCALE_DMA_PRI_CH27, FREESCALE_DMA_PRI_CH26,
    FREESCALE_DMA_PRI_CH25, FREESCALE_DMA_PRI_CH24,
    FREESCALE_DMA_PRI_CH31, FREESCALE_DMA_PRI_CH30,
    FREESCALE_DMA_PRI_CH29, FREESCALE_DMA_PRI_CH28
#endif
};

// Transfer control descriptor
typedef volatile struct cyghwr_hal_freescale_edma_tcd_s
                           cyghwr_hal_freescale_edma_tcd_t;
#define CYGBLD_FREESCALE_EDMA_TCD_ALIGN CYGBLD_ATTRIB_ALIGN(32)
struct cyghwr_hal_freescale_edma_tcd_s {
    volatile void* saddr;             //  Source Address
    cyg_uint16 soff;         //  Signed Source Address Offset
    cyg_uint16 attr;         //  Transfer Attributes
    union {
        cyg_uint32 mlno;     //  Minor Byte Count (Minor Loop Dis)
        //  Signed Minor Loop Off:
        cyg_uint32 mloffyes; //    MinoL Eena and Off Dis
        cyg_uint32 mloffno;  //    Minor Loop and Off Ena
    } nbytes;
    cyg_uint32 slast;         //  Last Source Address Adjustment
    volatile void *daddr;              //  Destination Address
    cyg_uint16 doff;          //  Signed Destination Address Offset
    union {                   //  Current Minor Loop Link:
        cyg_uint16 elinkyes;  //  Major Loop Count (Ch Lnkng Ena)
        cyg_uint16 elinkno;   //  Major Loop Count (Ch Lnkng Dis)
    } citer;
    union {
        cyg_uint32 dlast;     //  Last Dst Addr Adj/Scat Gath Addr
        cyghwr_hal_freescale_edma_tcd_t *sga;  //  Last Dst Addr Adj/Scat Gath Addr
    };
    cyg_uint16 csr;           //  Control and Status
    union {                   //  Beginning Minor Loop Link:
        cyg_uint16 elinkno;   //  Major Loop Cnt (Ch Lnkng Dis)
        cyg_uint16 elinkyes;  //  Major Loop Cnt (Ch Lnkng Ena)
    } biter;
};

// DMA - Peripheral register structure
typedef volatile struct cyghwr_hal_freescale_edma_s {
    cyg_uint32 cr;                   // Control Register
    cyg_uint32 es;                   // Error Status Register
    cyg_uint32 reserved_0;
    cyg_uint32 erq;                  // Enable Request Register
    cyg_uint32 reserved_1;
    cyg_uint32 eei;                  // Enable Error Interrupt Register
    cyg_uint8  ceei;                 // Clear Enable Error Interrupt Register
    cyg_uint8  seei;                 // Set Enable Error Interrupt Register
    cyg_uint8  cerq;                 // Clear Enable Request Register
    cyg_uint8  serq;                 // Set Enable Request Register
    cyg_uint8  cdne;                 // Clear DONE Status Bit Register
    cyg_uint8  ssrt;                 // Set START Bit Register
    cyg_uint8  cerr;                 // Clear Error Register
    cyg_uint8  cint;                 // Clear Interrupt Request Register
    cyg_uint32 reserved_2;
    cyg_uint32 irq;                  // Interrupt Request Register
    cyg_uint32 reserved_3;
    cyg_uint32 err;                  // Error Register
    cyg_uint32 reserved_4;
    cyg_uint32 hrs;                  // Hardware Request Status Register
    cyg_uint8  reserved_5[0x8100 - (0x8034 + 4)];
    cyg_uint8  dchpri[CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM]; // Priorities
    cyg_uint8  reserved_6[0x9000 - 0x8100 - CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM];
    cyghwr_hal_freescale_edma_tcd_t
          tcd[CYGNUM_HAL_FREESCALE_EDMA_CHAN_NUM]; // Transfer control descriptors
} cyghwr_hal_freescale_edma_t;

// CYGHWR_IO_FREESCALE_EDMA0_P is defined by HAL
// #define CYGHWR_IO_FREESCALE_EDMA0_P

// ----------------------------------------------------------------------------
//  DMA Register Bits

// CR Bit Fields
#define FREESCALE_EDMA_CR_EDBG_M                        0x2
#define FREESCALE_EDMA_CR_EDBG_S                        1
#define FREESCALE_EDMA_CR_ERCA_M                        0x4
#define FREESCALE_EDMA_CR_ERCA_S                        2
#define FREESCALE_EDMA_CR_ERGA_M                        0x8
#define FREESCALE_EDMA_CR_ERGA_S                        3
#define FREESCALE_EDMA_CR_HOE_M                         0x10
#define FREESCALE_EDMA_CR_HOE_S                         4
#define FREESCALE_EDMA_CR_HALT_M                        0x20
#define FREESCALE_EDMA_CR_HALT_S                        5
#define FREESCALE_EDMA_CR_CLM_M                         0x40
#define FREESCALE_EDMA_CR_CLM_S                         6
#define FREESCALE_EDMA_CR_EMLM_M                        0x80
#define FREESCALE_EDMA_CR_EMLM_S                        7
#define FREESCALE_EDMA_CR_ECX_M                         0x10000
#define FREESCALE_EDMA_CR_ECX_S                         16
#define FREESCALE_EDMA_CR_CX_M                          0x20000
#define FREESCALE_EDMA_CR_CX_S                          17

#define FREESCALE_EDMA_GR_PRI(_gr_, _pr_) VALUE_((8 + 2 * _gr_), (_pr_ & 0x3))

// ES Bit Fields
#define FREESCALE_EDMA_ES_DBE_M                         0x1
#define FREESCALE_EDMA_ES_DBE_S                         0
#define FREESCALE_EDMA_ES_SBE_M                         0x2
#define FREESCALE_EDMA_ES_SBE_S                         1
#define FREESCALE_EDMA_ES_SGE_M                         0x4
#define FREESCALE_EDMA_ES_SGE_S                         2
#define FREESCALE_EDMA_ES_NCE_M                         0x8
#define FREESCALE_EDMA_ES_NCE_S                         3
#define FREESCALE_EDMA_ES_DOE_M                         0x10
#define FREESCALE_EDMA_ES_DOE_S                         4
#define FREESCALE_EDMA_ES_DAE_M                         0x20
#define FREESCALE_EDMA_ES_DAE_S                         5
#define FREESCALE_EDMA_ES_SOE_M                         0x40
#define FREESCALE_EDMA_ES_SOE_S                         6
#define FREESCALE_EDMA_ES_SAE_M                         0x80
#define FREESCALE_EDMA_ES_SAE_S                         7
#define FREESCALE_EDMA_ES_ERRCHN_M                      0xF00
#define FREESCALE_EDMA_ES_ERRCHN_S                      8
#define FREESCALE_EDMA_ES_ERRCHN(__val)                 \
            VALUE_(FREESCALE_EDMA_ES_ERRCHN_S, __val)
#define FREESCALE_EDMA_ES_CPE_M                         0x4000
#define FREESCALE_EDMA_ES_CPE_S                         14
#define FREESCALE_EDMA_ES_GPE_M                         0x8000
#define FREESCALE_EDMA_ES_GPE_S                         15
#define FREESCALE_EDMA_ES_ECX_M                         0x10000
#define FREESCALE_EDMA_ES_ECX_S                         16
#define FREESCALE_EDMA_ES_VLD_M                         0x80000000
#define FREESCALE_EDMA_ES_VLD_S                         31
// ERQ Bit Fields
#define FREESCALE_EDMA_ERQ(__rq)                        BIT(__rq)
// EEI Bit Fields
#define FREESCALE_EDMA_EEI(__rq)                        BIT(__rq)
#define FREESCALE_EDMA_CHAN_M                           0x1F
// CEEI Bit Fields
#define FREESCALE_EDMA_CEEI_CEEI(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_CEEI_CAEE_M                      0x40
#define FREESCALE_EDMA_CEEI_CAEE_S                      6
#define FREESCALE_EDMA_CEEI_NOP_M                       0x80
#define FREESCALE_EDMA_CEEI_NOP_S                       7
// SEEI Bit Fields
#define FREESCALE_EDMA_SEEI_SEEI(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_SEEI_SAEE_M                      0x40
#define FREESCALE_EDMA_SEEI_SAEE_S                      6
#define FREESCALE_EDMA_SEEI_NOP_M                       0x80
#define FREESCALE_EDMA_SEEI_NOP_S                       7
// CERQ Bit Fields
#define FREESCALE_EDMA_CERQ_CERQ(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_CERQ_CAER_M                      0x40
#define FREESCALE_EDMA_CERQ_CAER_S                      6
#define FREESCALE_EDMA_CERQ_NOP_M                       0x80
#define FREESCALE_EDMA_CERQ_NOP_S                       7
// SERQ Bit Fields
#define FREESCALE_EDMA_SERQ_SERQ(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_SERQ_SAER_M                      0x40
#define FREESCALE_EDMA_SERQ_SAER_S                      6
#define FREESCALE_EDMA_SERQ_NOP_M                       0x80
#define FREESCALE_EDMA_SERQ_NOP_S                       7
// CDNE Bit Fields
#define FREESCALE_EDMA_CDNE_CDNE(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_CDNE_CADN_M                      0x40
#define FREESCALE_EDMA_CDNE_CADN_S                      6
#define FREESCALE_EDMA_CDNE_NOP_M                       0x80
#define FREESCALE_EDMA_CDNE_NOP_S                       7
// SSRT Bit Fields
#define FREESCALE_EDMA_SSRT_SSRT(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_SSRT_SAST_M                      0x40
#define FREESCALE_EDMA_SSRT_SAST_S                      6
#define FREESCALE_EDMA_SSRT_NOP_M                       0x80
#define FREESCALE_EDMA_SSRT_NOP_S                       7
// CERR Bit Fields
#define FREESCALE_EDMA_CERR_CERR(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_CERR_CAEI_M                      0x40
#define FREESCALE_EDMA_CERR_CAEI_S                      6
#define FREESCALE_EDMA_CERR_NOP_M                       0x80
#define FREESCALE_EDMA_CERR_NOP_S                       7
// CINT Bit Fields
#define FREESCALE_EDMA_CINT_CINT(__val) (__val & FREESCALE_EDMA_CHAN_M)
#define FREESCALE_EDMA_CINT_CAIR_M                      0x40
#define FREESCALE_EDMA_CINT_CAIR_S                      6
#define FREESCALE_EDMA_CINT_NOP_M                       0x80
#define FREESCALE_EDMA_CINT_NOP_S                       7
// INT Bit Fields
#define FREESCALE_EDMA_INT(__ch)                        BIT(__ch)
// ERR Bit Fields
#define FREESCALE_EDMA_ERR(__ch)                        BIT(__ch)
// HRS Bit Fields
#define FREESCALE_EDMA_HRS(__ch)                        BIT(__ch)
// DCHPRI Bit Fields

#define FREESCALE_EDMA_DCHPRI_CHPRI_M                   0xF

#define FREESCALE_EDMA_DCHPRI_CHPRI(__val)              \
        (__val & FREESCALE_EDMA_DCHPRI_CHPRI_M)
#define FREESCALE_EDMA_DCHPRI_DPA_M                     0x40
#define FREESCALE_EDMA_DCHPRI_DPA_S                     6
#define FREESCALE_EDMA_DCHPRI_ECP_M                     0x80
#define FREESCALE_EDMA_DCHPRI_ECP_S                     7
#define FREESCALE_EDMA_DCHPRI_ASIS                      0x20
// SOFF Bit Fields
#define FREESCALE_EDMA_SOFF_SOFF_M                      0xFFFF
#define FREESCALE_EDMA_SOFF_SOFF(__val)                 \
        (__val & FREESCALE_EDMA_SOFF_SOFF_M)
// ATTR Bit Fields
#define FREESCALE_EDMA_ATTR_DSIZE_M                     0x7
#define FREESCALE_EDMA_ATTR_DSIZE(__val)                \
        (__val & FREESCALE_EDMA_ATTR_DSIZE_M)
#define FREESCALE_EDMA_ATTR_DMOD_M                      0xF8
#define FREESCALE_EDMA_ATTR_DMOD_S                      3
#define FREESCALE_EDMA_ATTR_DMOD(__val)                 \
        VALUE_(FREESCALE_EDMA_ATTR_DMOD_S, __val)
#define FREESCALE_EDMA_ATTR_SSIZE_M                     0x700
#define FREESCALE_EDMA_ATTR_SSIZE_S                     8
#define FREESCALE_EDMA_ATTR_SSIZE(__val)                \
        VALUE_(FREESCALE_EDMA_ATTR_SSIZE_S, __val)
#define FREESCALE_EDMA_ATTR_SMOD_M                      0xF800
#define FREESCALE_EDMA_ATTR_SMOD_S                      11
#define FREESCALE_EDMA_ATTR_SMOD(__val)                 \
        VALUE_(FREESCALE_EDMA_ATTR_SMOD_S, __val)
#define FREESCALE_EDMA_ATTR_SIZE_8       0
#define FREESCALE_EDMA_ATTR_SIZE_16      1
#define FREESCALE_EDMA_ATTR_SIZE_32      2
#define FREESCALE_EDMA_ATTR_SIZE_16B     4
// NBYTES_MLOFFNO Bit Fields
#define FREESCALE_EDMA_NBYTES_MLOFFNO_NBYTES_M          0x3FFFFFFF
#define FREESCALE_EDMA_NBYTES_MLOFFNO_NBYTES(__val)     \
        (__val & FREESCALE_EDMA_NBYTES_MLOFFNO_NBYTES_M)
#define FREESCALE_EDMA_NBYTES_MLOFFNO_DMLOE_M           0x40000000
#define FREESCALE_EDMA_NBYTES_MLOFFNO_DMLOE_S           30
#define FREESCALE_EDMA_NBYTES_MLOFFNO_SMLOE_M           0x80000000
#define FREESCALE_EDMA_NBYTES_MLOFFNO_SMLOE_S           31
// NBYTES_MLOFFYES Bit Fields
#define FREESCALE_EDMA_NBYTES_MLOFFYES_NBYTES_M         0x3FF
#define FREESCALE_EDMA_NBYTES_MLOFFYES_NBYTES(__val)    \
        (__val & FREESCALE_EDMA_NBYTES_MLOFFYES_NBYTES(__val))
#define FREESCALE_EDMA_NBYTES_MLOFFYES_MLOFF_M          0x3FFFFC00
#define FREESCALE_EDMA_NBYTES_MLOFFYES_MLOFF_S          10
#define FREESCALE_EDMA_NBYTES_MLOFFYES_MLOFF(__val)     \
        VALUE_(FREESCALE_EDMA_NBYTES_MLOFFYES_MLOFF_S, __val)
#define FREESCALE_EDMA_NBYTES_MLOFFYES_DMLOE_M          0x40000000
#define FREESCALE_EDMA_NBYTES_MLOFFYES_DMLOE_S          30
#define FREESCALE_EDMA_NBYTES_MLOFFYES_SMLOE_M          0x80000000
#define FREESCALE_EDMA_NBYTES_MLOFFYES_SMLOE_S          31
// DOFF Bit Fields
#define FREESCALE_EDMA_DOFF_DOFF_M                      0xFFFF
#define FREESCALE_EDMA_DOFF_DOFF(__val) (__val & FREESCALE_EDMA_DOFF_DOFF_M)
// CITER_ELINKYES Bit Fields
#define FREESCALE_EDMA_CITER_ELINKYES_CITER_M           0x1FF
#define FREESCALE_EDMA_CITER_ELINKYES_CITER(__val)      \
            (__val & FREESCALE_EDMA_CITER_ELINKYES_CITER_M)
#define FREESCALE_EDMA_CITER_ELINKYES_LINKCH_M          0x1E00
#define FREESCALE_EDMA_CITER_ELINKYES_LINKCH_S          9
#define FREESCALE_EDMA_CITER_ELINKYES_LINKCH(__val)     \
            VALUE_(FREESCALE_EDMA_CITER_ELINKYES_LINKCH_S, __val)
#define FREESCALE_EDMA_CITER_ELINKYES_ELINK_M           0x8000
#define FREESCALE_EDMA_CITER_ELINKYES_ELINK_S           15
// CITER_ELINKNO Bit Fields
#define FREESCALE_EDMA_CITER_ELINKNO_CITER_M            0x7FFF
#define FREESCALE_EDMA_CITER_ELINKNO_CITER(__val)       \
            (__val & FREESCALE_EDMA_CITER_ELINKNO_CITER_M)
#define FREESCALE_EDMA_CITER_ELINKNO_ELINK_M            0x8000
#define FREESCALE_EDMA_CITER_ELINKNO_ELINK_S            15
// CSR Bit Fields
#define FREESCALE_EDMA_CSR_START_M                      0x1
#define FREESCALE_EDMA_CSR_START_S                      0
#define FREESCALE_EDMA_CSR_INTMAJOR_M                   0x2
#define FREESCALE_EDMA_CSR_INTMAJOR_S                   1
#define FREESCALE_EDMA_CSR_INTHALF_M                    0x4
#define FREESCALE_EDMA_CSR_INTHALF_S                    2
#define FREESCALE_EDMA_CSR_DREQ_M                       0x8
#define FREESCALE_EDMA_CSR_DREQ_S                       3
#define FREESCALE_EDMA_CSR_ESG_M                        0x10
#define FREESCALE_EDMA_CSR_ESG_S                        4
#define FREESCALE_EDMA_CSR_MAJORELINK_M                 0x20
#define FREESCALE_EDMA_CSR_MAJORELINK_S                 5
#define FREESCALE_EDMA_CSR_ACTIVE_M                     0x40
#define FREESCALE_EDMA_CSR_ACTIVE_S                     6
#define FREESCALE_EDMA_CSR_DONE_M                       0x80
#define FREESCALE_EDMA_CSR_DONE_S                       7
#define FREESCALE_EDMA_CSR_MAJORLINKCH_M                0xF00
#define FREESCALE_EDMA_CSR_MAJORLINKCH_S                8
#define FREESCALE_EDMA_CSR_MAJORLINKCH(__val)           \
            VALUE_(FREESCALE_EDMA_CSR_MAJORLINKCH_S, __val)
#define FREESCALE_EDMA_CSR_BWC_M                        0xC000
#define FREESCALE_EDMA_CSR_BWC_S                        14
#define FREESCALE_EDMA_CSR_BWC(__val)                   \
        VALUE_(FREESCALE_EDMA_CSR_BWC_S, __val)
#define FREESCALE_EDMA_CSR_BWC_0      0
#define FREESCALE_EDMA_CSR_BWC_MEDIUM FREESCALE_EDMA_CSR_BWC(2)
#define FREESCALE_EDMA_CSR_BWC_NICE   FREESCALE_EDMA_CSR_BWC(3)

// BITER_ELINKNO Bit Fields
#define FREESCALE_EDMA_BITER_ELINKNO_BITER_M            0x7FFF
#define FREESCALE_EDMA_BITER_ELINKNO_BITER(__val)       \
            (__val FREESCALE_EDMA_BITER_ELINKNO_BITER_M)
#define FREESCALE_EDMA_BITER_ELINKNO_ELINK_M            0x8000
#define FREESCALE_EDMA_BITER_ELINKNO_ELINK_S            15
// BITER_ELINKYES Bit Fields
#define FREESCALE_EDMA_BITER_ELINKYES_BITER_M           0x1FF
#define FREESCALE_EDMA_BITER_ELINKYES_BITER(__val)      \
            (__val & FREESCALE_EDMA_BITER_ELINKYES_BITER_M)
#define FREESCALE_EDMA_BITER_ELINKYES_LINKCH_M          0x1E00
#define FREESCALE_EDMA_BITER_ELINKYES_LINKCH_S          9
#define FREESCALE_EDMA_BITER_ELINKYES_LINKCH(__val)     \
            VALUE_(FREESCALE_EDMA_BITER_ELINKYES_LINKCH_S, __val)
#define FREESCALE_EDMA_BITER_ELINKYES_ELINK_M           0x8000
#define FREESCALE_EDMA_BITER_ELINKYES_ELINK_S           15

// EDMA buffer descriptor memory section
#ifdef CYGHWR_HAL_EDMA_TCD_SECTION
# define EDMA_RAM_TCD_SECTION CYGBLD_ATTRIB_SECTION(CYGHWR_HAL_EDMA_TCD_SECTION)
#else
# define EDMA_RAM_MEM_SECTION
#endif // CYGHWR_HAL_EDMA_MEM_SECTION

// EDMA buffer memory section
#ifdef CYGHWR_HAL_EDMA_BUF_SECTION
# define EDMA_RAM_BUF_SECTION CYGBLD_ATTRIB_SECTION(CYGHWR_HAL_EDMA_BUF_SECTION)
#else
# define EDMA_RAM_BUF_SECTION
#endif // CYGHWR_HAL_EDMA_BUF_SECTION

//-----------------------------------------------------------------------------

// DMA Channel data
typedef struct cyghwr_hal_freescale_dma_chan_set_s {
    cyg_uint8 dma_src;      // Data source
    cyg_uint8 dma_chan_i;   // Channel index
    cyg_uint8 dma_prio;     // DMA channel priority
    cyg_uint8 isr_prio;     // Interrupt priority
    cyg_uint8 isr_num;      // Interrupt vector
    cyg_uint8 isr_ena;      // Interruot enable
} cyghwr_hal_freescale_dma_chan_set_t;

// DMA Channel set
typedef struct cyghwr_hal_freescale_dma_set_s {
    cyghwr_hal_freescale_edma_t* edma_p;
    const cyghwr_hal_freescale_dma_chan_set_t *chan_p;
    cyg_uint8 chan_n;
} cyghwr_hal_freescale_dma_set_t;


__externC void
hal_freescale_edma_init_chanset(cyghwr_hal_freescale_dma_set_t *inidat_p);

__externC void
hal_freescale_edma_diag(const cyghwr_hal_freescale_dma_set_t *inidat_p, cyg_uint32 mask);

__externC void
hal_freescale_edma_transfer_init(cyghwr_hal_freescale_edma_t *edma_p,
                                 cyg_uint8 chan_i,
                                 const cyghwr_hal_freescale_edma_tcd_t *tcd_cfg_p);
__externC void
hal_freescale_edma_tcd_diag(cyghwr_hal_freescale_edma_tcd_t *tcd_p, cyg_int32 chan_i, const char *prefix);

__externC void
hal_freescale_edma_transfer_diag (cyghwr_hal_freescale_edma_t *edma_p,
                                  cyg_uint8 chan_i, cyg_bool recurse);

__externC inline void
hal_freescale_edma_erq_enable(cyghwr_hal_freescale_edma_t *edma_p,
                              cyg_uint8 chan_i)
{
    edma_p->serq = chan_i;
}

__externC inline void
hal_freescale_edma_erq_disable(cyghwr_hal_freescale_edma_t *edma_p,
                               cyg_uint8 chan_i)
{
    edma_p->cerq = chan_i;
}

__externC inline void
hal_freescale_edma_cleardone(cyghwr_hal_freescale_edma_t *edma_p,
                              cyg_uint8 chan_i)
{
    edma_p->cdne = chan_i;
}

__externC inline void
hal_freescale_edma_irq_enable(cyghwr_hal_freescale_edma_t *edma_p,
                              cyg_uint8 chan_i)
{
    edma_p->seei = chan_i;
}

__externC inline void
hal_freescale_edma_irq_disable(cyghwr_hal_freescale_edma_t *edma_p,
                               cyg_uint8 chan_i)
{
    edma_p->ceei = chan_i;
}

__externC inline void
hal_freescale_edma_irq_clear(cyghwr_hal_freescale_edma_t *edma_p,
                               cyg_uint8 chan_i)
{
    edma_p->cint = chan_i;
}

__externC inline void
hal_freescale_edma_transfer_clear(cyghwr_hal_freescale_edma_t *edma_p,
                                  cyg_uint8 chan_i)
{
    edma_p->tcd[chan_i].csr &= ~FREESCALE_EDMA_CSR_DONE_M;
}

__externC inline void
hal_freescale_edma_transfer_start(cyghwr_hal_freescale_edma_t *edma_p,
                                  cyg_uint8 chan_i)
{
    edma_p->ssrt = chan_i;
}

#define HAL_DMA_TRANSFER_STOP(__edma,__chan)   \
        hal_freescale_edma_erq_disable(__edma, __chan)
#define HAL_DMA_TRANSFER_START(__edma,__chan)  \
        hal_freescale_edma_erq_enable(__edma, __chan)
#define HAL_DMA_TRANSFER_CLEAR(__edma,__chan)  \
        hal_freescale_edma_cleardone(__edma, __chan)

// end of var_io_dma.h
#endif // CYGONCE_FREESCALE_EDMA_H
