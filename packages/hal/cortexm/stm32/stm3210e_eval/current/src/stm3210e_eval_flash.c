/*==========================================================================
//
//      stm3210e_eval_flash.c
//
//      Cortex-M3 STM3210E EVAL Flash setup
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 eCosCentric Limited.
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-07-30
// Description:  
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_stm32.h>
#include <pkgconf/hal_cortexm_stm32_stm3210e_eval.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include <cyg/hal/hal_io.h>


//--------------------------------------------------------------------------
// Internal flash

#include <cyg/io/stm32_flash.h>

cyg_stm32_flash_dev hal_stm32_flash_priv;

CYG_FLASH_DRIVER(hal_stm32_flash,
                 &cyg_stm32_flash_funs,
                 0,
                 0x08000000,
                 0,
                 0,
                 0,
                 &hal_stm32_flash_priv
);

//--------------------------------------------------------------------------
// There is a Spansion S29GL128P90FFIR20 or a NUMONYX equivalent.
// These are AMD compatible and with CFI can all be handled by the AMD
// driver.


#include <cyg/io/am29xxxxx_dev.h>

static const CYG_FLASH_FUNS(hal_stm3210e_flash_amd_funs,
                            &cyg_am29xxxxx_init_cfi_16,
                            &cyg_flash_devfn_query_nop,
                            &cyg_am29xxxxx_erase_16,
                            &cyg_am29xxxxx_program_16,
                            (int (*)(struct cyg_flash_dev*, const cyg_flashaddr_t, void*, size_t))0,
                            &cyg_flash_devfn_lock_nop,
                            &cyg_flash_devfn_unlock_nop);

static const cyg_am29xxxxx_dev hal_stm3210e_flash_priv;

CYG_FLASH_DRIVER(hal_stm3210e_flash,
                 &hal_stm3210e_flash_amd_funs,
                 0,
                 0x64000000,
                 0,
                 0,
                 hal_stm3210e_flash_priv.block_info,
                 &hal_stm3210e_flash_priv);

//--------------------------------------------------------------------------
// EOF stm3210e_eval_flash.c
