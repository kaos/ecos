#ifndef DEV_FLASH_SYNTHV2_SYNTH_H
#define DEV_FLASH_SYNTHV2_SYNTH_H
//==========================================================================
//
//      synth.h
//
//      Synthetic Flash driver header
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2004 Free Software Foundation, Inc.                        
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
// Author(s):    Andrew Lunn
// Contributors: Andrew Lunn
// Date:         2004-07-03
// Purpose:      Header file for the synthetic flash device
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

// Structure of data private to each flash device
struct cyg_flash_synth_priv 
{
    // Configuration parameters,
    size_t                      block_size;
    cyg_uint32                  blocks;
    size_t                      boot_block_size;
    cyg_uint32                  boot_blocks;
    cyg_bool                    boot_block_bottom;
    char *                      filename;
    // Run-time data
    int                         flashfd;
    // Space for the block layout
    struct cyg_flash_block_info block_info[2];
};

extern const struct cyg_flash_dev_funs cyg_flash_synth_funs;

#endif

  
