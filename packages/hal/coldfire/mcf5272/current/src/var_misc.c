//==========================================================================
//
//      var_misc.c
//
//      Miscellaneous functions specific to the processor variant
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
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
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       Miscellaneous functions specific to the MCF5272 processor.
//
//####DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>

#include CYGHWR_MEMORY_LAYOUT_H

// -------------------------------------------------------------------------
// VSR table

// For the MCF5272, we can point the VBR directly to the VSR table.
// However, the table must be on a 1 MB boundary. Locate the VSR table where
// the linker tells us to.

volatile CYG_ADDRESS cyg_hal_vsr_table[CYGNUM_HAL_VSR_COUNT]
                    __attribute__ ((section (".ramvec")));

// -------------------------------------------------------------------------
// Function prototypes

static void hal_update_interrupt_controller(int vector);

// -------------------------------------------------------------------------
// Interrupt controller management
//
// With the MCF5272 interrupt controller, it is not possible to mask an
// interrupt while retaining its associated priority. Moreover, if we enabled
// the use of interrupts with different priorities, we don't have a means to
// retrieve the priority of the current interrupt, after having raised the
// IPL to the maximum, in the first instruction of the HAL ISR handler.
// So, we use an auxiliary table (cyg_hal_ILVL_table) that records all the
// priorities set for the various interrupts.
// The purpose of the cyg_hal_IMASK_table table is to record wether an
// interrupt is currently masked (0) or not (1).

volatile cyg_uint8 cyg_hal_ILVL_table[CYGNUM_HAL_ISR_COUNT];
volatile cyg_uint8 cyg_hal_IMASK_table[CYGNUM_HAL_ISR_COUNT];


// Update priority table and interrupt controller
void hal_interrupt_set_level(int vector, int level)
{
    cyg_uint32 index;
    
    CYG_ASSERT((0 <= (level) && 7 >= (level)), "Illegal level");
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= (vector)
                && CYGNUM_HAL_ISR_MAX >= (vector)), "Illegal vector");

    HAL_TRANSLATE_VECTOR(vector, index);
    cyg_hal_ILVL_table[index] = (cyg_uint8) level;

    hal_update_interrupt_controller(vector);
}

// Update mask table and interrupt controller
void hal_interrupt_mask(int vector)
{
    cyg_uint32 index;
    
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= (vector)
        && CYGNUM_HAL_ISR_MAX >= (vector)), "Illegal vector");
    
    HAL_TRANSLATE_VECTOR(vector, index);
    cyg_hal_IMASK_table[index] = 0;
    
    hal_update_interrupt_controller(vector);
}

// Update mask table and interrupt controller
void hal_interrupt_unmask(int vector)
{
    cyg_uint32 index;

    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= (vector)
    && CYGNUM_HAL_ISR_MAX >= (vector)), "Illegal vector");
    
    HAL_TRANSLATE_VECTOR(vector, index);
    cyg_hal_IMASK_table[index] = 1;
         
    hal_update_interrupt_controller(vector);
}


// Set the priority in the interrupt control register.
// Disable all interrupts while we access the hardware registers.
static void hal_update_interrupt_controller(int vector)
{
    cyg_uint32 index;
    cyg_uint8 level;
    cyg_uint32 vec_offset;
    cyg_uint32 icr, icr_msk_offset, icr_msk, icr_val, icr_oldval;
    CYG_INTERRUPT_STATE intr_state;
    
    HAL_TRANSLATE_VECTOR(vector, index); 
    level = cyg_hal_IMASK_table[index] ? cyg_hal_ILVL_table[index] : 0;

    vec_offset = (vector) - HAL_PROG_INT_VEC_BASE - 1;
    icr = vec_offset / 8;
    icr_msk_offset = ((8-1)*4) - (vec_offset % 8) * 4;
    icr_msk = 0x0F << (icr_msk_offset);
    icr_val = (0x08 | (level & 0x07)) << icr_msk_offset;
    
    HAL_DISABLE_INTERRUPTS(intr_state);
    HAL_READ_UINT32(&MCF5272_DEVS->intc.icr[icr], icr_oldval);
    icr_val |= icr_oldval & 0x77777777 & ~icr_msk;
    HAL_WRITE_UINT32(&MCF5272_DEVS->intc.icr[icr], icr_val);
    HAL_RESTORE_INTERRUPTS(intr_state);
}
// -------------------------------------------------------------------------
// End of var_misc.c
