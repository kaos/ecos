/*==========================================================================
//
//      at91_misc.c
//
//      HAL misc board support code for Atmel AT91
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jskov, nickg, tkoeller
// Date:         2001-07-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

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

// -------------------------------------------------------------------------
// Hardware init

void hal_hardware_init(void)
{
    unsigned i;

    // Reset all interrupts
    HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_IDCR, 0xFFFFFFFF);

    // Flush internal priority level stack
    for (i = 0; i < 8; ++i)
        HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_EOI, 0xFFFFFFFF);

#ifdef HAL_PLF_HARDWARE_INIT
    // Perform any platform specific initializations
    HAL_PLF_HARDWARE_INIT();
#endif

    // Set up eCos/ROM interfaces
    hal_if_init();
}

#if CYGINT_HAL_ARM_AT91_SYS_INTERRUPT
// Decode a system interrupt. Not all systems have all interrupts. So
// code will only be generated for those interrupts which have a
// defined value.
static int sys_irq_handler(void)
{
  cyg_uint32 sr, mr;

#ifdef CYGNUM_HAL_INTERRUPT_PITC
  // Periodic Interrupt Timer Controller
  HAL_READ_UINT32((AT91_PITC+AT91_PITC_PISR), sr);
  if (sr & AT91_PITC_PISR_PITS) {
    return CYGNUM_HAL_INTERRUPT_PITC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_DBG
  // Debug Unit
  HAL_READ_UINT32((AT91_DBG + AT91_DBG_CSR), sr);
  HAL_READ_UINT32((AT91_DBG + AT91_DBG_IMR), mr);
  if (sr & mr) {
    return CYGNUM_HAL_INTERRUPT_DBG;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_RTTC
  /* Real Time Timer. Check the interrupt is enabled, not that just
     the status indicates there is an interrupt. It takes a while for
     the status bit to clear. */
  HAL_READ_UINT32((AT91_RTTC+AT91_RTTC_RTSR), sr);
  HAL_READ_UINT32((AT91_RTTC+AT91_RTTC_RTMR), mr);
  if (((mr & AT91_RTTC_RTMR_ALMIEN) &&
       (sr & AT91_RTTC_RTSR_ALMS)) ||
      ((mr & AT91_RTTC_RTMR_RTTINCIEN) &&
       (sr & AT91_RTTC_RTSR_RTTINC))) {
    return CYGNUM_HAL_INTERRUPT_RTTC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_PMC
  // Power Management Controller
  HAL_READ_UINT32((AT91_PMC+AT91_PMC_IMR), mr);
  HAL_READ_UINT32((AT91_PMC+AT91_PMC_SR), sr);
  if ((sr & mr) & 
      (AT91_PMC_SR_MOSCS   |
       AT91_PMC_SR_LOCK    |
       AT91_PMC_SR_MCKRDY  |
       AT91_PMC_SR_PCK0RDY |
       AT91_PMC_SR_PCK1RDY |
       AT91_PMC_SR_PCK2RDY |
       AT91_PMC_SR_PCK3RDY)) {
    return CYGNUM_HAL_INTERRUPT_PMC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_MC
  // Memory controller
  HAL_READ_UINT32((AT91_MC+AT91_MC_FMR), mr);
  HAL_READ_UINT32((AT91_MC+AT91_MC_FSR), sr);
  if ((sr & mr) & 
      (AT91_MC_FSR_FRDY  |
       AT91_MC_FSR_LOCKE |
       AT91_MC_FSR_PROGE)) {
    return CYGNUM_HAL_INTERRUPT_MC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_WDTC
  // Watchdog Timer Controller
  HAL_READ_UINT32((AT91_WDTC+AT91_WDTC_WDSR), sr);
  HAL_READ_UINT32((AT91_WDTC+AT91_WDTC_WDMR), mr);
  if ((mr & AT91_WDTC_WDMR_FIEN) &&
      sr & (AT91_WDTC_WDSR_UNDER |
            AT91_WDTC_WDSR_ERROR)) {
    return CYGNUM_HAL_INTERRUPT_WDTC;
  }
#endif

#ifdef CYGNUM_HAL_INTERRUPT_RSTC
  // Reset Controller
  HAL_READ_UINT32((AT91_RST + AT91_RST_RSR), sr);
  HAL_READ_UINT32((AT91_RST + AT91_RST_RMR), mr);
  if (((mr & AT91_RST_RMR_URSTIEN) && (sr & AT91_RST_RSR_USER)) ||  
      ((mr & AT91_RST_RMR_BODIEN) && (sr & AT91_RST_RSR_BROWN)))
    return CYGNUM_HAL_INTERRUPT_RSTC;
#endif
  
  return CYGNUM_HAL_INTERRUPT_NONE;
}
#endif

// -------------------------------------------------------------------------
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.

int hal_IRQ_handler(void)
{
    cyg_uint32 irq_num;
    cyg_uint32 ivr;
#ifdef CYGHWR_HAL_ARM_AT91_FIQ
    // handle fiq interrupts as irq 
    cyg_uint32 ipr,imr;

    HAL_READ_UINT32(AT91_AIC+AT91_AIC_IPR, ipr);
    HAL_READ_UINT32(AT91_AIC+AT91_AIC_IMR, imr);

    if (imr & ipr & (1 << CYGNUM_HAL_INTERRUPT_FIQ)) {
      HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_ICCR, (1 << CYGNUM_HAL_INTERRUPT_FIQ));
      return CYGNUM_HAL_INTERRUPT_FIQ;
    }
#endif
    // Calculate active interrupt (updates ISR)
    HAL_READ_UINT32(AT91_AIC+AT91_AIC_IVR, ivr);

    HAL_READ_UINT32(AT91_AIC+AT91_AIC_ISR, irq_num);

#if CYGINT_HAL_ARM_AT91_SYS_INTERRUPT
    if (irq_num == CYGNUM_HAL_INTERRUPT_SYS) {
      // determine the source of the system interrupt
      irq_num = sys_irq_handler();
    }
#endif
    // An invalid interrupt source is treated as a spurious interrupt    
    if (irq_num < CYGNUM_HAL_ISR_MIN || irq_num > CYGNUM_HAL_ISR_MAX)
      irq_num = CYGNUM_HAL_INTERRUPT_NONE;
    
    return irq_num;
}

// -------------------------------------------------------------------------
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

#if CYGINT_HAL_ARM_AT91_SYS_INTERRUPT
    if (vector >= 32) {
      HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_IDCR, 
                       (1 << CYGINT_HAL_ARM_AT91_SYS_INTERRUPT));
      return;
    }
#endif
    HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_IDCR, (1<<vector));
}

void hal_interrupt_unmask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

#if CYGINT_HAL_ARM_AT91_SYS_INTERRUPT
    if (vector >= 32) {
      hal_interrupt_configure(CYGINT_HAL_ARM_AT91_SYS_INTERRUPT, true, true);
      HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_IECR, 
                       (1 <<CYGINT_HAL_ARM_AT91_SYS_INTERRUPT));
      return;
    }
#endif
    HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_IECR, (1<<vector));
}

void hal_interrupt_acknowledge(int vector)
{
    // No check for valid vector here! Spurious interrupts
    // must be acknowledged, too.
    HAL_WRITE_UINT32(AT91_AIC+AT91_AIC_EOI, 0xFFFFFFFF);  
}

void hal_interrupt_configure(int vector, int level, int up)
{
    cyg_uint32 mode;

    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

#if CYGINT_HAL_ARM_AT91_SYS_INTERRUPT
    if (vector >= 32) 
      return;
#endif
    if (level) {
        if (up) {
            mode = AT91_AIC_SMR_LEVEL_HI;
        } else {
            mode = AT91_AIC_SMR_LEVEL_LOW;
        }
    } else {
        if (up) {
            mode = AT91_AIC_SMR_EDGE_POS;
        } else {
            mode = AT91_AIC_SMR_EDGE_NEG;
        }
    }
    mode |= 7;  // Default priority
    HAL_WRITE_UINT32(AT91_AIC+(AT91_AIC_SMR0+(vector*4)), mode);
}

void hal_interrupt_set_level(int vector, int level)
{
    cyg_uint32 mode;

    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    CYG_ASSERT(level >= 0 && level <= 7, "Invalid level");

#if CYGINT_HAL_ARM_AT91_SYS_INTERRUPT
    if (vector >= 32) 
      return;
#endif

    HAL_READ_UINT32(AT91_AIC+(AT91_AIC_SMR0+(vector*4)), mode);
    mode = (mode & ~AT91_AIC_SMR_PRIORITY) | level;
    HAL_WRITE_UINT32(AT91_AIC+(AT91_AIC_SMR0+(vector*4)), mode);
}

void hal_show_IRQ(int vector, int data, int handler)
{
//    UNDEFINED(__FUNCTION__);  // FIXME
}


#ifndef AT91_RST
/* Use the watchdog to generate a reset */
void hal_at91_reset_cpu(void)
{
    HAL_WRITE_UINT32(AT91_WD + AT91_WD_OMR, AT91_WD_OMR_OKEY);
    HAL_WRITE_UINT32(AT91_WD + AT91_WD_CMR, AT91_WD_CMR_CKEY);
    HAL_WRITE_UINT32(AT91_WD + AT91_WD_CR, AT91_WD_CR_RSTKEY);
    HAL_WRITE_UINT32(AT91_WD + AT91_WD_OMR, 
                     (AT91_WD_OMR_OKEY | 
                      AT91_WD_OMR_RSTEN |
                      AT91_WD_OMR_WDEN));
    while(1) CYG_EMPTY_STATEMENT;
}
#else
/* Use the Reset Controller to generate a reset */
void hal_at91_reset_cpu(void)
{
  HAL_WRITE_UINT32(AT91_RST + AT91_RST_RCR,
                   AT91_RST_RCR_PROCRST |                   
                   AT91_RST_RCR_ICERST  |
                   AT91_RST_RCR_PERRST  |
                   AT91_RST_RCR_KEY);
  while(1) CYG_EMPTY_STATEMENT;
}
#endif
//--------------------------------------------------------------------------
// EOF at91_misc.c
