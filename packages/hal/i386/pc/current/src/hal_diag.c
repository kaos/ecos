//=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
//
//=============================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:proven
// Date:        1998-10-05
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types

#include <cyg/hal/hal_diag.h>

#include <cyg/hal/plf_misc.h>

//-----------------------------------------------------------------------------
// New Hal_Diag init to comply with the eCos/ROM Calling Interface.

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT

#include <cyg/hal/hal_arch.h>           // basic machine info
#include <cyg/hal/hal_intr.h>           // interrupt macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_misc.h>           // Helper functions

#include <cyg/hal/pcmb_serial.h>

//=============================================================================

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG) \
    || defined(CYGPRI_HAL_IMPLEMENTS_IF_SERVICES)

channel_data_t pc_ser_channels[CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS];

void
cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;
    int num_serial;

    if (initialized)
        return;

    initialized = 1;

    num_serial = CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS;
#ifdef CYGSEM_HAL_I386_PC_DIAG_SCREEN
    --num_serial;
#endif
    if (num_serial > 0) {
	// COM1
	pc_ser_channels[0].base = 0x3F8;
	pc_ser_channels[0].msec_timeout = 1000;
	pc_ser_channels[0].isr_vector = 36;
    }
    if (num_serial > 1) {
	// COM2
	pc_ser_channels[1].base = 0x2F8;
	pc_ser_channels[1].msec_timeout = 1000;
	pc_ser_channels[1].isr_vector = 35;
    }

    cyg_hal_plf_serial_init();

#ifdef CYGSEM_HAL_I386_PC_DIAG_SCREEN
    
    pc_ser_channels[num_serial].base = 0x060;
    pc_ser_channels[num_serial].msec_timeout = 1000;
    pc_ser_channels[num_serial].isr_vector = 33;

    cyg_hal_plf_screen_init();

#endif    
}

//=============================================================================

#endif  //defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)
	//  || defined(CYGPRI_HAL_IMPLEMENTS_IF_SERVICES)

#endif

//=============================================================================

#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

// TODO: add stand-alone code

#endif // CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT

//-----------------------------------------------------------------------------
// End of hal_diag.c

