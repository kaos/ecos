#ifndef CYGONCE_HAL_VAR_IO_FLEXBUS_H
#define CYGONCE_HAL_VAR_IO_FLEXBUS_H
//===========================================================================
//
//      var_io_flexbus.h
//
//      Kinetis FlexBus specific registers
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
// Date:          2011-02-05
// Purpose:       Kinetis variant specific registers
// Description:
// Usage:         #include <cyg/hal/var_io.h>  // var_io.h includes this file
//
//####DESCRIPTIONEND####
//
//===========================================================================


//---------------------------------------------------------------------------
// FlexBus

// FlexBus chip select control registers
typedef struct cyghwr_hal_kinetis_fbcs_s{
    cyg_uint32 csar;    // Chip select address register
    cyg_uint32 csmr;    // Chip select mask register
    cyg_uint32 cscr;    // Chip select control register
} cyghwr_hal_kinetis_fbcs_t;

#define CYGNUM_HAL_KINETIS_FBCS_N 6 // Kinetis has up to 6 chip selects

// FlexBus control
typedef volatile struct cyghwr_hal_kinetis_fb_s {
  cyghwr_hal_kinetis_fbcs_t csel[CYGNUM_HAL_KINETIS_FBCS_N]; //Chip Selects
  cyg_uint8 reserved[24];
  cyg_uint32 cspmcr;      //Chip select port multiplexing control register
} cyghwr_hal_kinetis_fb_t;

#define CYGHWR_HAL_KINETIS_FB_P ((cyghwr_hal_kinetis_fb_t *) 0x4000C000)

// CSAR - Chip Select Address Register
// CSAR Bit Fields
#define CYGHWR_HAL_FB_CSAR_BA_M        0xFFFF0000
#define CYGHWR_HAL_FB_CSAR_BA_S        16
#define CYGHWR_HAL_FB_CS_AR_BA(__val)  VALUE_(CYGHWR_HAL_FB_CSAR_BA_S, __val)

// CSMR - Chup Select Mask Register
// CSMR Bit Fields
#define CYGHWR_HAL_FB_CSMR_V_M         0x1
#define CYGHWR_HAL_FB_CSMR_V_S         0
#define CYGHWR_HAL_FB_CSMR_WP_M        0x100
#define CYGHWR_HAL_FB_CSMR_WP_S        8
#define CYGHWR_HAL_FB_CSMR_BAM_M       0xFFFF0000
#define CYGHWR_HAL_FB_CSMR_BAM_S       16
#define CYGHWR_HAL_FB_CS_MR_BAM(__val) VALUE_(CYGHWR_HAL_FB_CSMR_BAM_S, __val)

// CSCR - Chip Select Control register
// CSCR Bit Fields
#define CYGHWR_HAL_FB_CSCR_BSTW_M      0x8
#define CYGHWR_HAL_FB_CSCR_BSTW_S      3
#define CYGHWR_HAL_FB_CSCR_BSTR_M      0x10
#define CYGHWR_HAL_FB_CSCR_BSTR_S      4
#define CYGHWR_HAL_FB_CSCR_BEM_M       0x20
#define CYGHWR_HAL_FB_CSCR_BEM_S       5
#define CYGHWR_HAL_FB_CSCR_PS_M        0xC0
#define CYGHWR_HAL_FB_CSCR_PS_S        6
#define CYGHWR_HAL_FB_CSCR_AA_M        0x100
#define CYGHWR_HAL_FB_CSCR_AA_S        8
#define CYGHWR_HAL_FB_CSCR_BLS_M       0x200
#define CYGHWR_HAL_FB_CSCR_BLS_S       9
#define CYGHWR_HAL_FB_CSCR_WS_M        0xFC00
#define CYGHWR_HAL_FB_CSCR_WS_S        10
#define CYGHWR_HAL_FB_CSCR_WRAH_M      0x30000
#define CYGHWR_HAL_FB_CSCR_WRAH_S      16
#define CYGHWR_HAL_FB_CSCR_RDAH_M      0xC0000
#define CYGHWR_HAL_FB_CSCR_RDAH_S      18
#define CYGHWR_HAL_FB_CSCR_ASET_M      0x300000
#define CYGHWR_HAL_FB_CSCR_ASET_S      20
#define CYGHWR_HAL_FB_CSCR_EXALE_M     0x400000
#define CYGHWR_HAL_FB_CSCR_EXALE_S     22
#define CYGHWR_HAL_FB_CSCR_SWSEN_M     0x800000
#define CYGHWR_HAL_FB_CSCR_SWSEN_S     23
#define CYGHWR_HAL_FB_CSCR_SWS_M       0xFC000000
#define CYGHWR_HAL_FB_CSCR_SWS_S       26

// CSPMCR Bit Fields
#define CYGHWR_HAL_FB_CSPMCR_G5_M  0xF000
#define CYGHWR_HAL_FB_CSPMCR_G5_S  12
#define CYGHWR_HAL_FB_CSPMCR_G4_M  0xF0000
#define CYGHWR_HAL_FB_CSPMCR_G4_S  16
#define CYGHWR_HAL_FB_CSPMCR_G3_M  0xF00000
#define CYGHWR_HAL_FB_CSPMCR_G3_S  20
#define CYGHWR_HAL_FB_CSPMCR_G2_M  0xF000000
#define CYGHWR_HAL_FB_CSPMCR_G2_S  24
#define CYGHWR_HAL_FB_CSPMCR_G1_M  0xF0000000
#define CYGHWR_HAL_FB_CSPMCR_G1_S  28

// FlexBus control pin multiplexing
#define CYGHWR_HAL_FB_CSPMCR(__group, __val) VALUE_(__group, __val)

#define CYGHWR_HAL_FB_CSPMCR_G1_ALE \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G1_S, 0)
#define CYGHWR_HAL_FB_CSPMCR_G1_CS1 \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G1_S, 1)
#define CYGHWR_HAL_FB_CSPMCR_G1_TS  \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G1_S, 2)

#define CYGHWR_HAL_FB_CSPMCR_G2_CS4      \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G2_S, 0)
#define CYGHWR_HAL_FB_CSPMCR_G2_TSIZ0    \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G2_S, 1)
#define CYGHWR_HAL_FB_CSPMCR_G2_BE_31_24 \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G2_S, 2)

#define CYGHWR_HAL_FB_CSPMCR_G3_CS5      \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G3_S, 0)
#define CYGHWR_HAL_FB_CSPMCR_G3_TSIZ1    \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G3_S, 1)
#define CYGHWR_HAL_FB_CSPMCR_G3_BE_23_16 \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G3_S, 2)

#define CYGHWR_HAL_FB_CSPMCR_G4_TST      \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G4_S, 0)
#define CYGHWR_HAL_FB_CSPMCR_G4_CS2      \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G4_S, 1)
#define CYGHWR_HAL_FB_CSPMCR_G4_BE_15_8  \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G4_S, 2)

#define CYGHWR_HAL_FB_CSPMCR_G5_TA       \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G5_S, 0)
#define CYGHWR_HAL_FB_CSPMCR_G5_CS3      \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G5_S, 1)
#define CYGHWR_HAL_FB_CSPMCR_G5_BE_7_0   \
            CYGHWR_HAL_FB_CSPMCR(CYGHWR_HAL_FB_CSPMCR_G5_S, 2)

//-----------------------------------------------------------------------------
// end of var_io_flexbus.h
#endif // CYGONCE_HAL_VAR_IO_FLEXBUS_H
