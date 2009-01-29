/*==========================================================================
//
//      at91sam7xek_misc.c
//
//      HAL misc board support code for Atmel AT91SAM7X EK board
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2006 Free Software Foundation, Inc.                        
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
// Author(s):    andrew lunn
// Contributors: Andrew Lunn, John Eigelaar
// Date:         2006-06-20
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/hal/hal_io.h>             // IO macros

// The development board has four LEDs
void 
hal_at91_led (int val)
{
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB19, AT91_PIN_OUT);
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB20, AT91_PIN_OUT);
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB21, AT91_PIN_OUT);
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB22, AT91_PIN_OUT);
  
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PB19, AT91_PIN_PULLUP_DISABLE);
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PB20, AT91_PIN_PULLUP_DISABLE);
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PB21, AT91_PIN_PULLUP_DISABLE);
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PB22, AT91_PIN_PULLUP_DISABLE);

  // Set the bits. The logic is inverted 
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PB19, !(val & 1));
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PB20, !(val & 2));
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PB21, !(val & 4));
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PB22, !(val & 8));
}

void hal_plf_eth_init(void)
{
   cyg_uint32 stat;

   /* Enable the PIOB Clock */
   HAL_WRITE_UINT32(AT91_PMC + AT91_PMC_PCER, AT91_PMC_PCER_PIOB);
  
   /* RXDV / Testmode select */
   HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB15, AT91_PIN_IN);
   HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PB15, AT91_PIN_PULLUP_DISABLE);

   //TODO: The errata reports that the RMII mode for the SAM7X does not work.
   //      It would probably still be a good idea to use the RMII/MII CDL 
   //      configuration to select the appropriate mode here
   /* COL / !MII select */
   HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB16, AT91_PIN_IN);
   HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PB16, AT91_PIN_PULLUP_DISABLE);

   /* TXCLK / ISOLATE */
   HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB0, AT91_PIN_IN);
   HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PB0, AT91_PIN_PULLUP_DISABLE);

   /* Power Down Mode */
   HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PB18, AT91_PIN_OUT);
   HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PB18,0);

   /* All the lines setup correctly. Now do a external reset and let the phy 
      start up in the correct mode */
   HAL_WRITE_UINT32(AT91_RST+AT91_RST_RMR,AT91_RST_RMR_KEY|(1<<0x8));
   HAL_WRITE_UINT32(AT91_RST+AT91_RST_RCR,AT91_RST_RCR_KEY|AT91_RST_RCR_EXTRST);

   do {
     HAL_READ_UINT32(AT91_RST+AT91_RST_RSR,stat);
   } while (!(stat&AT91_RST_RSR_NRST_SET));
}

//--------------------------------------------------------------------------
// EOF at91sam7sek_misc.c
