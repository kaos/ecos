//==========================================================================
//
//      lpc2xxx_iap.c
//
//      LPC2XXX IAP (In Application Programming) interface
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2006 Free Software Foundation, Inc.
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
// Author(s):    Sergei Gavrikov
// Contributors: Sergei Gavrikov
// Date:         2007-09-20
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal_arm_lpc2xxx.h>
#include <cyg/hal/hal_intr.h>

#include <cyg/hal/lpc2xxx_iap.h>

// ----------------------------------------------------------------------------
// In most cases NXP IAP (In Appplication Programming) interface uses itself to
// manage on-chip flash memory. Two simple examples are provided the below as
// commented out C code sniplets just to demo how to use the IAP call there.
// ----------------------------------------------------------------------------
#if 0
void
iap_call_demo1 (void *data)     // Read part id
{
    flash_data_t   *id = (flash_data_t *) data;

    hal_lpc2xxx_iap_call (HAL_LPC2XXX_IAP_COMMAND_READ_PART_ID, 0, 0, 0, 0,
                          (cyg_uint32 *) id);
    // ...
}

void
iap_call_demo2 (void)           // Format sectors 7, 8 of on-chip flash
{
    int             rc;

    rc = hal_lpc2xxx_iap_call (HAL_LPC2XXX_IAP_COMMAND_PREPARE_SECTORS,
                               7, 8, 0, 0, 0);
    if (rc != HAL_LPC2XXX_IAP_RETURN_CMD_SUCCESS) {
        // ...
    }
    rc = hal_lpc2xxx_iap_call (HAL_LPC2XXX_IAP_COMMAND_ERASE_SECTORS, 7,
                               8, CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED / 1000,
                               0, 0);
    if (rc != HAL_LPC2XXX_IAP_RETURN_CMD_SUCCESS) {
        // ...
    }
}
#endif
// ----------------------------------------------------------------------------

typedef void    (*lpc_iap_entry_t) (cyg_uint32[], cyg_uint32[]);

static lpc_iap_entry_t lpc_iap_entry =
    (lpc_iap_entry_t) HAL_LPC2XXX_IAP_ENTRY_DEFAULT;


static void     (*precall) (void) = NULL;
static void     (*postcall) (void) = NULL;

static int initialized = 0;

static void
hal_lpc2xxx_iap_init (void) {
#ifdef CYGFUN_HAL_ARM_LPC2XXX_IAP_CALL_WRAP
# ifdef CYGDAT_HAL_ARM_LPC2XXX_IAP_PRE_CALL
    if (!precall) {
      precall = CYGDAT_HAL_ARM_LPC2XXX_IAP_PRE_CALL;
    }
# endif
# ifdef CYGDAT_HAL_ARM_LPC2XXX_IAP_POST_CALL
    if (!postcall) {
      postcall = CYGDAT_HAL_ARM_LPC2XXX_IAP_POST_CALL;
    }
# endif
#endif
    initialized = 1;
}

// ----------------------------------------------------------------------------
// hal_lpc2xxx_iap_call --
//
// Arguments: 'cmd' - code of NXP IAP command, 'par0'...'par3' - parameters,
// and the last argument 'ret' is a pointer on an array of cyg_uint32 values
// (at the least room for two elemets in the array must be reserved) for the
// returned data.
//
// Returns 0 (HAL_LPC2XXX_IAP_RETURN_CMD_SUCCESS) on success, otherwise error
// code (see <cyg/hal/lpc2xxx_iap.h> for details).
//
cyg_uint32
hal_lpc2xxx_iap_call (cyg_uint32 cmd, cyg_uint32 par0, cyg_uint32 par1,
                      cyg_uint32 par2, cyg_uint32 par3, cyg_uint32 * ret)
{
    cyg_uint32      command[5] = { cmd, par0, par1, par2, par3 };
    cyg_uint32      results[2];

#ifdef CYGNUM_HAL_ARM_LPC2XXX_IAP_CALL_SAFE
    cyg_uint32      oldints;
#endif

    if (!initialized)
        hal_lpc2xxx_iap_init ();

    if (precall)
        precall ();

#ifdef CYGNUM_HAL_ARM_LPC2XXX_IAP_CALL_SAFE
    HAL_DISABLE_INTERRUPTS (oldints);
#endif

    lpc_iap_entry (command, results);

#ifdef CYGNUM_HAL_ARM_LPC2XXX_IAP_CALL_SAFE
    HAL_RESTORE_INTERRUPTS (oldints);
#endif

    if (postcall)
        postcall ();

    if (ret != NULL) {
        ret[0] = results[0];    // return code
        ret[1] = results[1];    // fill in data
    }

    return results[0];
}

// indent: -i4 -br -nut -di16 -ce; vim: expandtab tabstop=4 shiftwidth=4:
//--------------------------------------------------------------------------
// EOF lpc2xxx_iap.c
