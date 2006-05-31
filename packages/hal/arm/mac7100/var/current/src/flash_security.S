/*==========================================================================
//
//      flash_security.S
//
//      MAC7100 Flash security area
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 eCosCentric Ltd
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Ilija Koco <ilijak@siva.com.mk>
// Contributors: 
// Date:         2006-04-11
// Purpose:      HAL board support
// Description:  MAC7100 Flash security area.           
//
//####DESCRIPTIONEND####
//
//========================================================================*/

/////////////////////////////////////////////////////////////////////////////
//    This section contains Common Flash Module (CFM) configuration field
//        For detail description see Common Flash Module (CFM) chapter
//        @ MAC7100 Microcontroller Familly  Reference Manual
//
//    Deafault factorry setting:
//        0xffffffff for All words except Flash Security Word
//        0xfffffffe for Flash Security Word
/////////////////////////////////////////////////////////////////////////////

    .section ".mac7100_flash_security", "a"
    .code 32
    .global VAE_MAC7100_FlashSecurity
VAE_MAC7100_FlashSecurity:
    .long 0xffffffff  // Backdoor Comparison Key bit 63-32
    .long 0xffffffff  // Backdoor Comparison key bit 31-0
    .long 0xffffffff  // Program FLASH protection Bytes 
    .long 0xffffffff  // Program FLASH SUPV Access Bytes  
    .long 0xffffffff  // Program FLASH DATA Access Bytes  
    .long 0xfffffffe  // Flash Security Word
    .long 0xffffffff  // Data FLASH protection, SUPV access, DATA access
     
    .long 0xe0f00420  // Just a placeholder (round to 0x420)
    .end
    
//--------------------------------------------------------------------------
// EOF flash_security.S
