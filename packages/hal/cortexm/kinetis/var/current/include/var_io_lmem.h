#ifndef CYGONCE_HAL_VAR_IO_LMEM_H
#define CYGONCE_HAL_VAR_IO_LMEM_H
//===========================================================================
//
//      var_io_lmem.h
//
//      Kinetis Local memory controller specific registers
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Date:          2012-04-28
// Purpose:       Kinetis variant specific registers
// Description:
// Usage:         #include <cyg/hal/var_io_lmem.h>  // var_io.h includes this file
//
//####DESCRIPTIONEND####
//
//===========================================================================

//----------------------------------------------------------------------------
// LMEM - Local memory controller

typedef volatile struct cyghwr_hal_kinetis_lmem_s {
    cyg_uint32 ccr;    // Cache control register
    cyg_uint32 clcr;   // Cache line control register
    void *csar;        // Cache search address register
    cyg_uint32 cvr;    // Cache read/write value register
    cyg_uint8  reserved[16];
    cyg_uint32 rmr;    // Cache regions mode register
} cyghwr_hal_kinetis_lmem_t;

#define CYGHWR_HAL_KINETIS_LMEM_PC_P  ((cyghwr_hal_kinetis_lmem_t *) 0xE0082000)
#define CYGHWR_HAL_KINETIS_LMEM_PS_P  ((cyghwr_hal_kinetis_lmem_t *) 0xE0082800)

// CCR Bit Fields
#define CYGHWR_HAL_KINETIS_LMEM_CCR_ENCACHE_M          0x1
#define CYGHWR_HAL_KINETIS_LMEM_CCR_ENWRBUF_M          0x2
#define CYGHWR_HAL_KINETIS_LMEM_CCR_INVW0_M            0x1000000
#define CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW0_M           0x2000000
#define CYGHWR_HAL_KINETIS_LMEM_CCR_INVW1_M            0x4000000
#define CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW1_M           0x8000000
#define CYGHWR_HAL_KINETIS_LMEM_CCR_GO_M               0x80000000

//CLCR Bit Fields
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LGO_M             0x1
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_CACHEADDR_M       0xFFC
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_CACHEADDR_S       2
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_CACHEADDR(_ca_) \
            (_ca_ << CYGHWR_HAL_KINETIS_LMEM_CLCR_CACHEADDR_S)
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_WSEL_M            0x4000
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_TDSEL_M           0x10000
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LCIVB_M           0x100000
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LCIMB_M           0x200000
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LCWAY_M           0x400000
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LCMD_S            24
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LCMD(_cmd_) \
            (_cmd_ << CYGHWR_HAL_KINETIS_LMEM_CLCR_LCMD_S)
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LADSEL_M          0x4000000
#define CYGHWR_HAL_KINETIS_LMEM_CLCR_LACC_M            0x8000000

// CSAR Bit Fields
#define CYGHWR_HAL_KINETIS_LMEM_CSAR_LGO_M             0x1
#define CYGHWR_HAL_KINETIS_LMEM_CSAR_PHYADDR_M         0xFFFFFFFC
#define CYGHWR_HAL_KINETIS_LMEM_CSAR_PHYADDR_S         2
#define CYGHWR_HAL_KINETIS_LMEM_CSAR_PHYADDR(_adr_) \
            (_adr_ << CYGHWR_HAL_KINETIS_LMEM_CSAR_PHYADDR_S)

// CCVR Bit Fields
#define CYGHWR_HAL_KINETIS_LMEM_CCVR_DATA_M            0xFFFFFFFF

// PCCRMR Bit Fields

#define CYGHWR_HAL_KINETIS_LMEM_CRMR_REGION_M           0x3
#define CYGHWR_HAL_KINETIS_LMEM_CRMR_REGION(_region_,_mask_) \
            (_mask_ << ((15 - _region_) * 2))

#define CYGHWR_HAL_KINETIS_LMEM_CRMR_REGION_NC_M 0
#define CYGHWR_HAL_KINETIS_LMEM_CRMR_REGION_WT_M 2
#define CYGHWR_HAL_KINETIS_LMEM_CRMR_REGION_WB_M 3

#define CYGHWR_HAL_KINETIS_LMEM_R_FLASH_0000   0
#define CYGHWR_HAL_KINETIS_LMEM_R_DRAM_0800    1
#define CYGHWR_HAL_KINETIS_LMEM_R_FLEXNVM_1000 2
#define CYGHWR_HAL_KINETIS_LMEM_R_FLEXBUS_1800 3
#define CYGHWR_HAL_KINETIS_LMEM_R_SRAM_L       4
#define CYGHWR_HAL_KINETIS_LMEM_R_SRAM_U       5
#define CYGHWR_HAL_KINETIS_LMEM_FLEXBUS_6000   6
#define CYGHWR_HAL_KINETIS_LMEM_DRAM_7000      7
#define CYGHWR_HAL_KINETIS_LMEM_DRAM_8000      8
#define CYGHWR_HAL_KINETIS_LMEM_FLEXBUS_9000   9


#define HAL_CORTEXM_KINETIS_CACHE_PC_ENABLE() \
            hal_cortexm_kinetis_cache_enable(CYGHWR_HAL_KINETIS_LMEM_PC_P)

#define HAL_CORTEXM_KINETIS_CACHE_PS_ENABLE() \
            hal_cortexm_kinetis_cache_enable(CYGHWR_HAL_KINETIS_LMEM_PS_P)

#define HAL_CORTEXM_KINETIS_CACHE_PC_DISABLE() \
            hal_cortexm_kinetis_cache_disable(CYGHWR_HAL_KINETIS_LMEM_PC_P)

#define HAL_CORTEXM_KINETIS_CACHE_PS_DISABLE() \
            hal_cortexm_kinetis_cache_disable(CYGHWR_HAL_KINETIS_LMEM_PS_P)

#define HAL_CORTEXM_KINETIS_CACHE_PC_INVALL() \
            hal_cortexm_kinetis_cache_inval(CYGHWR_HAL_KINETIS_LMEM_PC_P)

#define HAL_CORTEXM_KINETIS_CACHE_PS_INVALL() \
            hal_cortexm_kinetis_cache_inval(CYGHWR_HAL_KINETIS_LMEM_PS_P)

#define HAL_CORTEXM_KINETIS_CACHE_PC_SYNC() \
            hal_cortexm_kinetis_cache_sync(CYGHWR_HAL_KINETIS_LMEM_PC_P)

#define HAL_CORTEXM_KINETIS_CACHE_PS_SYNC() \
            hal_cortexm_kinetis_cache_sync(CYGHWR_HAL_KINETIS_LMEM_PS_P)

#define HAL_CORTEXM_KINETIS_CACHE_PC_CLEAR() \
            hal_cortexm_kinetis_cache_clear(CYGHWR_HAL_KINETIS_LMEM_PC_P)

#define HAL_CORTEXM_KINETIS_CACHE_PS_CLEAR() \
            hal_cortexm_kinetis_cache_clear(CYGHWR_HAL_KINETIS_LMEM_PS_P)

#define HAL_CORTEXM_KINETIS_CACHE_PC_IS_ENABLED() \
            hal_cortexm_kinetis_cache_is_enabled(CYGHWR_HAL_KINETIS_LMEM_PC_P)

#define HAL_CORTEXM_KINETIS_CACHE_PS_IS_ENABLED() \
            hal_cortexm_kinetis_cache_is_enabled(CYGHWR_HAL_KINETIS_LMEM_PS_P)

#define HAL_CORTEXM_KINETIS_CACHE_PS_INVALIDATE(_base, _size_) \
        hal_cortexm_kinetis_cache_invalidate(CYGHWR_HAL_KINETIS_LMEM_PS_P, _base, _size_)

#define HAL_CORTEXM_KINETIS_CACHE_PC_INVALIDATE(_base, _size_) \
        hal_cortexm_kinetis_cache_invalidate(CYGHWR_HAL_KINETIS_LMEM_PC_P, _base, _size_)

__externC void inline
hal_cortexm_kinetis_cache_enable(cyghwr_hal_kinetis_lmem_t* lmem_p)
{
    lmem_p->ccr = (CYGHWR_HAL_KINETIS_LMEM_CCR_GO_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_INVW0_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_INVW1_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_ENCACHE_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_ENWRBUF_M
              );
}

__externC void inline
hal_cortexm_kinetis_cache_disable(cyghwr_hal_kinetis_lmem_t* lmem_p)
{
    lmem_p->ccr = (CYGHWR_HAL_KINETIS_LMEM_CCR_GO_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW0_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW1_M
              );
}

__externC void inline
hal_cortexm_kinetis_cache_inval(cyghwr_hal_kinetis_lmem_t* lmem_p)
{
    lmem_p->ccr |= ( CYGHWR_HAL_KINETIS_LMEM_CCR_GO_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_INVW0_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_INVW1_M
              );
}

__externC void inline
hal_cortexm_kinetis_cache_sync(cyghwr_hal_kinetis_lmem_t* lmem_p)
{
    lmem_p->ccr |= ( CYGHWR_HAL_KINETIS_LMEM_CCR_GO_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW0_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW1_M
              );
}

__externC void inline
hal_cortexm_kinetis_cache_clear(cyghwr_hal_kinetis_lmem_t* lmem_p)
{
    lmem_p->ccr |= ( CYGHWR_HAL_KINETIS_LMEM_CCR_GO_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_INVW0_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_INVW1_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW0_M |
              CYGHWR_HAL_KINETIS_LMEM_CCR_PUSHW1_M
              );
}

__externC bool inline
hal_cortexm_kinetis_cache_is_enabled(cyghwr_hal_kinetis_lmem_t* lmem_p)
{
    return lmem_p->ccr & CYGHWR_HAL_KINETIS_LMEM_CCR_ENCACHE_M;
}


__externC void inline
hal_cortexm_kinetis_cache_invalidate(cyghwr_hal_kinetis_lmem_t* lmem_p,
                                        cyg_uint8* addr_p, cyg_uint32 size)
{
    size = (((cyg_uint32)addr_p & 0xf) + size)/HAL_DCACHE_LINE_SIZE + 1;
    while(size--){
        lmem_p->csar = addr_p;
        lmem_p->clcr = CYGHWR_HAL_KINETIS_LMEM_CLCR_LADSEL_M |
              CYGHWR_HAL_KINETIS_LMEM_CLCR_LCMD(1) |
              CYGHWR_HAL_KINETIS_LMEM_CLCR_LGO_M;
        addr_p += HAL_DCACHE_LINE_SIZE;
         while(lmem_p->clcr & CYGHWR_HAL_KINETIS_LMEM_CLCR_LGO_M);
    }
}

# define HAL_CORTEXM_KINETIS_DDRMC_INIT(__inidat) \
        hal_cortexm_kinetis_ddrmc_init(__inidat, ((sizeof(__inidat)/2)/sizeof(__inidat[0])))
__externC void hal_cortexm_kinetis_ddrmc_diag(void);

//-----------------------------------------------------------------------------
// end of var_io_ddrmc.h
#endif // CYGONCE_HAL_VAR_IO_LMEM_H
