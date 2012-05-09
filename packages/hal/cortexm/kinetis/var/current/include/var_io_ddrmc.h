#ifndef CYGONCE_HAL_VAR_IO_DDRMC_H
#define CYGONCE_HAL_VAR_IO_DDRMC_H
//===========================================================================
//
//      var_io_ddrmc.h
//
//      Kinetis DDRam controller specific registers
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
// Date:          2012-03-08
// Purpose:       Kinetis variant specific registers
// Description:
// Usage:         #include <cyg/hal/var_io_ddrmc.h>  // var_io.h includes this file
//
//####DESCRIPTIONEND####
//
//===========================================================================

//----------------------------------------------------------------------------
// DDRMC - SDRAM controller

# define CYGNUM_HAL_KINETIS_DDRMC_CR_N 64  // Number of DDRMC control registers

typedef volatile struct cyghwr_hal_kinetis_ddrmc_s {
    cyg_uint32 cr[CYGNUM_HAL_KINETIS_DDRMC_CR_N]; // Control registers
    cyg_uint32 reserved1[32];
    cyg_uint32 rcr;                               // RCR control register
    cyg_uint32 reserved2[10];
    cyg_uint32 pad_ctrl;                          // I/O Pad control register
} cyghwr_hal_kinetis_ddrmc_t;

# define CYGHWR_HAL_KINETIS_DDRMC_P  ((cyghwr_hal_kinetis_ddrmc_t *)0x400ae000)

// RCR
# define CYGHWR_HAL_KINETIS_DDRMC_RCR_RST_M 0x40000000
// PAD CTRL
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT_CS0_M 0x03000000
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT_CS0_S 20
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT(_x_)  \
        VALUE_(CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT_CS0_S, _x_)
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT_DIS 0
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT_50  3
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT_75  1
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_ODT_150 2

# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY_M 0x0000000f
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY_S 0
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY(_x_)  \
        VALUE_(CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY_S, _x_)
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY_0BUF  0
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY_4BUF  1
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY_7BUF  2
# define CYGHWR_HAL_KINETIS_DDRMC_PADCTRL_SPDLY_10BUF 3

# define CYGHWR_HAL_KINETIS_DDRMC_CR00_START 0x1
# define CYGHWR_HAL_KINETIS_DDRMC_CR30_DRAM_INIT_CPL 0x400

# define CYGHWR_HAL_KINETIS_SIM_MCR_DDR_SETUP_M 0x0000ffff

# ifndef CYGHWR_HAL_KINETIS_SIM_MCR_DDR_SETUP
#  define CYGHWR_HAL_KINETIS_SIM_MCR_DDR_SETUP \
          (CYGHWR_HAL_KINETIS_SIM_MCR_DDRCFG(CYGHWR_HAL_KINETIS_SIM_MCR_DDRBUS)| \
          (CYGHWR_HAL_KINETIS_SIM_MCR_DDRDQSDIS_M * 0)                         | \
          (CYGHWR_HAL_KINETIS_SIM_MCR_DDRPEN_M * 1)                            | \
          (CYGHWR_HAL_KINETIS_SIM_MCR_DDRS_M * 0)                              | \
          (CYGHWR_HAL_KINETIS_SIM_MCR_DDRSREN_M * 0 ))
# endif

__externC void hal_cortexm_kinetis_ddrmc_init(const cyg_uint32 src[], cyg_uint32 num);
# define HAL_CORTEXM_KINETIS_DDRMC_INIT(__inidat) \
        hal_cortexm_kinetis_ddrmc_init(__inidat, ((sizeof(__inidat)/2)/sizeof(__inidat[0])))
__externC void hal_cortexm_kinetis_ddrmc_diag(void);

//-----------------------------------------------------------------------------
// end of var_io_ddrmc.h
#endif // CYGONCE_HAL_VAR_IO_DDRMC_H
