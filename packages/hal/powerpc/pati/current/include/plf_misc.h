#ifndef CYGONCE_PLF_MISC_H
#define CYGONCE_PLF_MISC_H

//==========================================================================
//
//      plf_misc.h
//
//      HAL platform miscellaneous functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Author(s):    Steven Clugston
// Contributors: Steven Clugston
// Date:         2008-05-08
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous defines provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/
#define CYGHWR_CFG_SDRAM_BASE	                (0x00400000) // (CS2)
#define CYGHWR_PCI_EPLD_BASE_ADDRESS            (0x02800000) // (CS3)
#define CYGPLF_REG_PLD_REGISTER_BASE  (CYGHWR_PCI_EPLD_BASE_ADDRESS + 0x1000)

#define CYGPLF_REG_PLD_PART_ID	      (CYGPLF_REG_PLD_REGISTER_BASE + 0x00)
#define CYGPLF_REG_PLD_BOARD_TIMING   (CYGPLF_REG_PLD_REGISTER_BASE + 0x04)
#define CYGPLF_REG_PLD_HW_CONFIG      (CYGPLF_REG_PLD_REGISTER_BASE + 0x08)
#define CYGPLF_REG_PLD_MISC_CONFIG    (CYGPLF_REG_PLD_REGISTER_BASE + 0x0C)
#define CYGPLF_REG_PLD_RESET	      (CYGPLF_REG_PLD_REGISTER_BASE + 0x10)

#define CYGPLF_REG_PLD_MISC_CPU_VPP   (0x00000008)
#define CYGPLF_REG_PLD_MISC_EXT_VPP   (0x00000004)
#define CYGPLF_REG_PLD_MISC_EXT_WP    (0x00000002)

/*------------------------------------------------------------------------*/
/* End of plf_misc.h                                                      */
#endif //CYGONCE_PLF_MISC_H
