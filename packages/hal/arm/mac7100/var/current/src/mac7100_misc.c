/*==========================================================================
//
//      mac7100_misc.c
//
//      HAL misc board support code for Freescale MAC7100
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (c) 2006 eCosCentric Ltd
//
// Ecos is free software; you can redistribute it and/or modify it under
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
// Author(s):    Ilija Koco <ilijak@siva.com.mk>
// Contributors: 
// Date:         2006-04-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================
*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // necessary?
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_if.h>             // calling interface
#include <cyg/hal/hal_misc.h>           // helper functions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif
#include <cyg/hal/var_io.h>             // platform registers
#include <cyg/devs/ser_esci.h>            // ESCI registers
#include <pkgconf/io_serial_freescale_esci.h>

// -------------------------------------------------------------------------
// Clock support

static cyg_uint32 _period;

void hal_clock_initialize(cyg_uint32 period)
{
    CYG_ADDRESS pit_base = MAC7100_PIT_BASE;
    cyg_uint32 pit_en;

    CYG_ASSERT(period < 0x10000, "Invalid clock period");

    // Disable counter
    HAL_READ_UINT32(MAC7100_PIT_EN(pit_base), pit_en);
    pit_en &= ~MAC7100_PIT_EN_PEN(CYGNUM_PIT_CHAN_CLOCK);
    HAL_WRITE_UINT32(MAC7100_PIT_EN(pit_base), pit_en);

    // Set registers
    _period=period;
    HAL_WRITE_UINT32(MAC7100_PIT_TLVAL(pit_base, CYGNUM_PIT_CHAN_CLOCK), 
                     period);

    // Start timer
    pit_en |= MAC7100_PIT_EN_PEN(CYGNUM_PIT_CHAN_CLOCK);
    HAL_WRITE_UINT32(MAC7100_PIT_EN(pit_base), pit_en);

    // Enable timer interrupt    
    HAL_READ_UINT32(MAC7100_PIT_INTEN(pit_base), pit_en);
    pit_en |= MAC7100_PIT_EN_PEN(CYGNUM_PIT_CHAN_CLOCK);
    HAL_WRITE_UINT32(MAC7100_PIT_INTEN(pit_base), pit_en);

    HAL_READ_UINT32(MAC7100_PIT_INTSEL(pit_base), pit_en);
    pit_en |= MAC7100_PIT_EN_PEN(CYGNUM_PIT_CHAN_CLOCK);
    HAL_WRITE_UINT32(MAC7100_PIT_INTSEL(pit_base), pit_en);
}

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    HAL_WRITE_UINT32(MAC7100_PIT_FLG(MAC7100_PIT_BASE), 
                     MAC7100_PIT_FLAG_TIF(CYGNUM_PIT_CHAN_CLOCK));
}


void hal_clock_read(cyg_uint32 *pvalue)
{
    cyg_uint32 val;

    HAL_READ_UINT32(MAC7100_PIT_TLVAL(MAC7100_PIT_BASE,CYGNUM_PIT_CHAN_CLOCK),
                    val);
    *pvalue = _period-val;
}

// -------------------------------------------------------------------------
//
// Delay for some number of micro-seconds
//

#if CYGNUM_PIT_CHAN_US!=0
#define CYGNUM_1_US (CYGNUM_HAL_ARM_MAC7100_CLOCK_SPEED/2000000)
#else
#define CYGNUM_1_US (CYGNUM_HAL_ARM_MAC7100_F_OSC/1000000)
#endif

void hal_delay_us(cyg_int32 usecs)
{
    CYG_ADDRESS pit_base = MAC7100_PIT_BASE;
    cyg_uint32 pit_en;
    
    
    // Clear flag
    HAL_WRITE_UINT32(MAC7100_PIT_FLG(pit_base), 
                     MAC7100_PIT_FLAG_TIF(CYGNUM_PIT_CHAN_US));

    // Set timer
    HAL_WRITE_UINT32(MAC7100_PIT_TLVAL(pit_base, CYGNUM_PIT_CHAN_US), 
                     usecs*CYGNUM_1_US-1);
    HAL_READ_UINT32(MAC7100_PIT_EN(pit_base), pit_en);
    pit_en |= MAC7100_PIT_EN_PEN(CYGNUM_PIT_CHAN_US);
    HAL_WRITE_UINT32(MAC7100_PIT_EN(pit_base), pit_en);

    do {
      HAL_READ_UINT32(MAC7100_PIT_FLG(pit_base), pit_en); 
    }while (!(pit_en & MAC7100_PIT_FLAG_TIF(CYGNUM_PIT_CHAN_US)));
    
    // Disable counter
    HAL_READ_UINT32(MAC7100_PIT_EN(pit_base), pit_en);
    pit_en &= ~MAC7100_PIT_EN_PEN(CYGNUM_PIT_CHAN_US);
    HAL_WRITE_UINT32(MAC7100_PIT_EN(pit_base), pit_en);
}


// -------------------------------------------------------------------------
// Hardware init

void hal_intc_init(void);
void hal_pit_init(void);
static void hal_mac7100_esci_pins(cyg_uint32);
void hal_hardware_init(void)
{
    // Reset all interrupts
    //

    // Flush internal priority level stack
    //
    
    // Set up eCos/ROM interfaces
#if (defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_A) || \
    (CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS > 0)
    hal_mac7100_esci_pins(FREESCALE_ESCI_A_I);
#endif
#if (defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_B) || \
    (CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS > 1)
    hal_mac7100_esci_pins(FREESCALE_ESCI_B_I);
#endif
#if defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_C
    hal_mac7100_esci_pins(FREESCALE_ESCI_C_I);
#endif
#if defined CYGPKG_IO_SERIAL_FREESCALE_ESCI_D
    hal_mac7100_esci_pins(FREESCALE_ESCI_D_I);
#endif
    
    hal_intc_init();    // Initialize interrupt controller
    hal_pit_init();     // Initilaize CLOCK for channels 1..10
    hal_if_init();
}

void hal_pit_init(void)
{
    cyg_uint32 pit_ctrl;
    
    CYG_ADDRESS pit_base=MAC7100_PIT_BASE;
    
    HAL_READ_UINT32(MAC7100_PIT_CTRL(pit_base), pit_ctrl);
    pit_ctrl &= ~MAC7100_PIT_MDIS;
    HAL_WRITE_UINT32(MAC7100_PIT_CTRL(pit_base), pit_ctrl);
}


// -------------------------------------------------------------------------
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.

typedef struct ClSlMask_T {
  cyg_uint8 ClMask;
  cyg_uint8 SlMask;
} ClSlMask_T;

static ClSlMask_T ClSlMasks[CYGNUM_HAL_ISR_COUNT];

void hal_intc_init(void)
{
    cyg_uint8 iconfig;
    CYG_ADDRESS intc_base=MAC7100_INTC_BASE;

    iconfig = (CYGHWR_HAL_ARM_MAC7100_INTC_FIQDEF | 
               CYGHWR_HAL_ARM_MAC7100_INTC_EMASK);
    HAL_WRITE_UINT8(MAC7100_INTC_ICONFIG(intc_base), iconfig);
}

int hal_IRQ_handler(void)
{
    CYG_ADDRESS intc_base=MAC7100_INTC_BASE;
    cyg_int8 irq_num;
    ClSlMask_T clslmask;
        
    HAL_READ_UINT8(MAC7100_INTC_CLMASK(intc_base), clslmask.ClMask);
    HAL_READ_UINT8(MAC7100_INTC_SLMASK(intc_base), clslmask.SlMask);
#ifdef CYGHWR_HAL_ARM_MAC7100_FIQ
    HAL_READ_UINT8(MAC7100_FIQIACK(intc_base), irq_num);
    if((irq_num-=CYGNUM_HAL_ISR_COUNT)<0){
#endif //CYGHWR_HAL_ARM_MAC7100_FIQ
        HAL_READ_UINT8(MAC7100_INTC_IRQIACK(intc_base), irq_num);
        if((irq_num-=CYGNUM_HAL_ISR_COUNT)<0){
            irq_num=CYGNUM_HAL_INTERRUPT_NONE;
        }else{
            ClSlMasks[irq_num]=clslmask;
        }
    return irq_num;
#ifdef CYGHWR_HAL_ARM_MAC7100_FIQ
    }
#endif // CYGHWR_HAL_ARM_MAC7100_FIQ
}

// -------------------------------------------------------------------------
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    
    HAL_WRITE_UINT8(MAC7100_INTC_SIMR(MAC7100_INTC_BASE), vector);
}

void hal_interrupt_unmask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    
    HAL_WRITE_UINT8(MAC7100_INTC_CIMR(MAC7100_INTC_BASE), vector);
}

void hal_interrupt_acknowledge(int vector)
{
    // ?? No check for valid vector here! Spurious interrupts
    // ?? must be acknowledged, too.
    
    if(vector>=0)
        HAL_WRITE_UINT8(MAC7100_INTC_CLMASK(MAC7100_INTC_BASE), 
                        ClSlMasks[vector].ClMask);
}

void hal_interrupt_configure(int vector, int level, int up)
{
    // TO DO
}

void hal_interrupt_set_level(int vector, int level)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    CYG_ASSERT(level >=0 level <= 15, "Invalid level");
    
    HAL_WRITE_UINT8(MAC7100_INTC_ICR(MAC7100_INTC_BASE,vector), 
                    MAC7100_INTC_INT_LEVEL(level));
}

void hal_show_IRQ(int vector, int data, int handler)
{
//    UNDEFINED(__FUNCTION__);  // FIXME
}


void hal_mac7100_reset_cpu(void)
{
    // TO DO use watchdog to reset cpu 
}

// Set ESCI channel pins in perpheral mode

void
hal_mac7100_esci_pins(cyg_uint32 i_esci){
    cyg_uint16 *p_pim_config=0;
    
    switch(i_esci){
    case FREESCALE_ESCI_A_I:
        p_pim_config=
          (cyg_uint16 *)MAC7100_PIM_CONFIG(MAC7100_PORT_G_OFFSET,2);
        break;
    case FREESCALE_ESCI_B_I:
        p_pim_config=(cyg_uint16 *)MAC7100_PIM_CONFIG(MAC7100_PORT_G_OFFSET,0);
        break;
    case FREESCALE_ESCI_C_I:
        p_pim_config=
          (cyg_uint16 *)MAC7100_PIM_CONFIG(MAC7100_PORT_G_OFFSET,14);
        break;
    case FREESCALE_ESCI_D_I:
        p_pim_config=
          (cyg_uint16 *)MAC7100_PIM_CONFIG(MAC7100_PORT_G_OFFSET,15);
        break;
    }
    HAL_WRITE_UINT16(p_pim_config++, MAC7100_PIM_MODE_PERIPHERAL);
    HAL_WRITE_UINT16(p_pim_config, MAC7100_PIM_MODE_PERIPHERAL);
}


//--------------------------------------------------------------------------
// EOF mac7100_misc.c
