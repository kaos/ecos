//==========================================================================
//
//      plf_misc.c
//
//      HAL platform miscellaneous functions
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jlarmour, jskov
// Date:         2000-05-15
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

#include <cyg/hal/hal_if.h>             // Calling interface definitions
#include <cyg/hal/hal_misc.h>           // Helper functions

//--------------------------------------------------------------------------

#define CYGARC_REG_SUPERIO_IREG     0xb4000398
#define CYGARC_REG_SUPERIO_DREG     0xb4000399

#define CYGARC_REG_SUPERIO_FER      0x00 // function enable
#define CYGARC_REG_SUPERIO_FAR      0x01 // function address
#define CYGARC_REG_SUPERIO_PTR      0x02 // power and test
#define CYGARC_REG_SUPERIO_CLK      0x51 // clock controller


#define CYGARC_REG_SUPERIO_FER_PAR  0x01
#define CYGARC_REG_SUPERIO_FER_SCC1 0x02
#define CYGARC_REG_SUPERIO_FER_SCC2 0x04

#define CYGARC_REG_SUPERIO_CLK_14M  0x00 // 14MHz source
#define CYGARC_REG_SUPERIO_CLK_CME  0x04 // clock multiplier enabled

#define CYGARC_REG_SUPERIO_PTR_PPEXT 0x80 // extended




static void write_super_io(cyg_uint8 offset, cyg_uint8 data)
{
    HAL_WRITE_UINT8(CYGARC_REG_SUPERIO_IREG, offset);
    HAL_WRITE_UINT8(CYGARC_REG_SUPERIO_DREG, data);
    HAL_WRITE_UINT8(CYGARC_REG_SUPERIO_DREG, data);
}

void hal_platform_init(void)
{
    hal_if_init();

    // Configure the SuperIO chip
    write_super_io(CYGARC_REG_SUPERIO_FER, 
                   CYGARC_REG_SUPERIO_FER_SCC1|CYGARC_REG_SUPERIO_FER_SCC2);
    write_super_io(CYGARC_REG_SUPERIO_FAR, 0x10);
    write_super_io(CYGARC_REG_SUPERIO_CLK, 
                   CYGARC_REG_SUPERIO_CLK_14M|CYGARC_REG_SUPERIO_CLK_CME);

    // Set up VSC320 interrupt controller. INT1 must merge INT2 and
    // INT3 according to the platform specification.
    HAL_WRITE_UINT32(CYGARC_REG_INT_CFG1,
                     CYGARC_REG_INT_CFG_INT2|CYGARC_REG_INT_CFG_INT3);

    // Unmask vectors which are entry points for interrupt controllers
    HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_V320USC_INT0);
    HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_V320USC_INT1);
    HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_IO);
}

//--------------------------------------------------------------------------
// End of plf_misc.c
