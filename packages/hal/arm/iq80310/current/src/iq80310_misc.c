//==========================================================================
//
//      iq80310_misc.c
//
//      HAL misc board support code for XScale IQ80310
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    msalter
// Contributors: msalter
// Date:         2000-10-10
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_stub.h>           // Stub macros
#include <cyg/hal/hal_if.h>             // calling interface API
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt names
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_iq80310.h>        // Hardware definitions
#include <cyg/infra/diag.h>             // diag_printf
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

static cyg_uint32 nfiq_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 nirq_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 nmi_mcu_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 nmi_patu_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 nmi_satu_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 nmi_pb_ISR(cyg_vector_t vector, cyg_addrword_t data);
static cyg_uint32 nmi_sb_ISR(cyg_vector_t vector, cyg_addrword_t data);

// Some initialization has already been done before we get here.
//
// Set up the interrupt environment.
// Set up the MMU so that we can use caches.
// Enable caches.
// - All done!

void hal_hardware_init(void)
{
    unsigned rtmp = 0;

    // Route INTA-INTD to IRQ pin
    //   The Yavapai manual is incorrect in that a '1' value
    //   routes to the IRQ line, not a '0' value.
    *PIRSR_REG = 0x0f;

    // Disable all interrupt sources:
    *IIMR_REG = 0x7f;
    *OIMR_REG = 0x7f; // don't mask INTD which is really xint3
    *X3MASK_REG = XINT3_TIMER | XINT3_ETHERNET | XINT3_UART_1 | \
	          XINT3_UART_2 | XINT3_PCI_INTD;

    // Let the timer run at a default rate (for delays)
    hal_clock_initialize(CYGNUM_HAL_RTC_PERIOD);

    // Set up eCos/ROM interfaces
    hal_if_init();

    // attach some builtin interrupt handlers
    HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_NIRQ, &nirq_ISR, CYGNUM_HAL_INTERRUPT_NIRQ, 0);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_NIRQ);

    HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_NFIQ, &nfiq_ISR, CYGNUM_HAL_INTERRUPT_NFIQ, 0);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_NFIQ);

    HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_MCU_ERR, &nmi_mcu_ISR, CYGNUM_HAL_INTERRUPT_MCU_ERR, 0);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_MCU_ERR);

    HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_PATU_ERR, &nmi_patu_ISR, CYGNUM_HAL_INTERRUPT_PATU_ERR, 0);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_PATU_ERR);

    HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_SATU_ERR, &nmi_satu_ISR, CYGNUM_HAL_INTERRUPT_SATU_ERR, 0);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SATU_ERR);

    HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_PBDG_ERR, &nmi_pb_ISR, CYGNUM_HAL_INTERRUPT_PBDG_ERR, 0);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_PBDG_ERR);

    HAL_INTERRUPT_ATTACH (CYGNUM_HAL_INTERRUPT_SBDG_ERR, &nmi_sb_ISR, CYGNUM_HAL_INTERRUPT_SBDG_ERR, 0);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SBDG_ERR);

    // Enable FIQ
#if 0
    asm volatile ("mrs %0,cpsr\n"
		  "bic %0,%0,#0x40\n"
		  "msr cpsr,%0\n"
		  : "=r"(rtmp) : );
#endif
}

#include CYGHWR_MEMORY_LAYOUT_H
typedef void code_fun(void);
void iq80310_program_new_stack(void *func)
{
    register CYG_ADDRESS stack_ptr asm("sp");
    register CYG_ADDRESS old_stack asm("r4");
    register code_fun *new_func asm("r0");
    old_stack = stack_ptr;
    stack_ptr = CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE - sizeof(CYG_ADDRESS);
    new_func = (code_fun*)func;
    new_func();
    stack_ptr = old_stack;
    return;
}

/*------------------------------------------------------------------------*/

//
// Memory layout
//

externC cyg_uint8 *
hal_arm_mem_real_region_top( cyg_uint8 *regionend )
{
    CYG_ASSERT( hal_dram_size > 0, "Didn't detect DRAM size!" );
    CYG_ASSERT( hal_dram_size <=  512<<20,
                "More than 512MB reported - that can't be right" );

    // is it the "normal" end of the DRAM region? If so, it should be
    // replaced by the real size
    if ( regionend ==
         ((cyg_uint8 *)CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE) ) {
        regionend = (cyg_uint8 *)CYGMEM_REGION_ram + hal_dram_size;
    }
    return regionend;
} // hal_arm_mem_real_region_top()


// -------------------------------------------------------------------------

// Clock can come from the PMU or from an external timer.
// The external timer is the preferred choice.

#if CYGNUM_HAL_INTERRUPT_RTC == CYGNUM_HAL_INTERRUPT_PMU_CCNT_OVFL

// Proper version that uses the clock counter in the PMU to do proper
// interrupts that require acknowledgement and all that good stuff.

static cyg_uint32 hal_clock_init_period; // The START value, it counts up

void hal_clock_initialize(cyg_uint32 period)
{
    // event types both zero; clear all 3 interrupts;
    // disable all 3 counter interrupts;
    // CCNT counts every processor cycle; reset all counters;
    // enable PMU.
    register cyg_uint32 init = 0x00000707;
    asm volatile (
        "mcr      p14,0,%0,c0,c0,0;" // write into PMNC
        :
        : "r"(init)
        /*:*/
        );
    // the CCNT in the PMU counts *up* then interrupts at overflow
    // ie. at 0x1_0000_0000 as it were.
    // So init to 0xffffffff - period + 1 to get the right answer.
    period = (~period) + 1;
    hal_clock_init_period = period;
    hal_clock_reset( 0, 0 );
}

// This routine is called during a clock interrupt.
// (before acknowledging the interrupt)
void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    asm volatile (
        "mrc      p14,0,r0,c1,c0,0;" // read from CCNT - how long since OVFL
        "add      %0, %0, r0;"       // synchronize with previous overflow
        "mcr      p14,0,%0,c1,c0,0;" // write into CCNT
        :
        : "r"(hal_clock_init_period)
        : "r0"
        );
}

// Read the current value of the clock, returning the number of hardware
// "ticks" that have occurred (i.e. how far away the current value is from
// the start)

void hal_clock_read(cyg_uint32 *pvalue)
{
    register cyg_uint32 now;
    asm volatile (
        "mrc      p14,0,%0,c1,c0,0;" // read from CCNT
        : "=r"(now)
        :
        /*:*/
        );
    *pvalue = now - hal_clock_init_period;
}

// Delay for some usecs.
void hal_delay_us(cyg_uint32 delay)
{
  int i;
  // the loop is going to take 3 ticks.  At 600 MHz, to give uS, multiply
  // by 600/3 = 200. No volatile is needed on i; gcc recognizes delay
  // loops and does NOT elide them.
  for ( i = 200 * delay; i ; i--)
    ;
}

#else // external timer

static cyg_uint32 _period;

void hal_clock_initialize(cyg_uint32 period)
{
    _period = period;

    // disable timer
    EXT_TIMER_INT_DISAB();
    EXT_TIMER_CNT_DISAB();

    *TIMER_LA0_REG_ADDR = period;
    *TIMER_LA1_REG_ADDR = period >> 8;
    *TIMER_LA2_REG_ADDR = period >> 16;

    EXT_TIMER_INT_ENAB();
    EXT_TIMER_CNT_ENAB();
}

// This routine is called during a clock interrupt.

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    // to clear the timer interrupt, clear the timer interrupt
    // enable, then re-set the int. enable bit
    EXT_TIMER_INT_DISAB();
    EXT_TIMER_INT_ENAB();
}

// Read the current value of the clock, returning the number of hardware
// "ticks" that have occurred (i.e. how far away the current value is from
// the start)

void hal_clock_read(cyg_uint32 *pvalue)
{
    cyg_uint8  cnt0, cnt1, cnt2, cnt3;
    cyg_uint32 timer_val;;

    // first read latches the count
    // Actually, it looks like there is a hardware problem where
    // invalid counts get latched. This do while loop appears
    // to get around the problem.
    do {
	cnt0 = *TIMER_LA0_REG_ADDR & TIMER_COUNT_MASK;
    } while (cnt0 == 0);
    cnt1 = *TIMER_LA1_REG_ADDR & TIMER_COUNT_MASK;
    cnt2 = *TIMER_LA2_REG_ADDR & TIMER_COUNT_MASK;
    cnt3 = *TIMER_LA3_REG_ADDR & 0xf;	/* only 4 bits in most sig. */

    /* now build up the count value */
    timer_val  =  ((cnt0 & 0x40) >> 1) | (cnt0 & 0x1f);
    timer_val |= (((cnt1 & 0x40) >> 1) | (cnt1 & 0x1f)) << 6;
    timer_val |= (((cnt2 & 0x40) >> 1) | (cnt2 & 0x1f)) << 12;
    timer_val |= cnt3 << 18;

    *pvalue = timer_val;
}

// Delay for some usecs.
void hal_delay_us(cyg_uint32 delay)
{
#define _CNT_MASK 0x3fffff
#define _TICKS_PER_USEC (EXT_TIMER_CLK_FREQ / 1000000)
    cyg_uint32 now, last, diff, ticks;

    hal_clock_read(&last);
    diff = ticks = 0;

    while (delay > ticks) {
	hal_clock_read(&now);

	if (now < last)
	    diff += ((_period - last) + now);
	else
	    diff += (now - last);

	last = now;

	if (diff >= _TICKS_PER_USEC) {
	    ticks += (diff / _TICKS_PER_USEC);
	    diff %= _TICKS_PER_USEC;
	}
    }
}

#endif

// -------------------------------------------------------------------------

typedef cyg_uint32 cyg_ISR(cyg_uint32 vector, CYG_ADDRWORD data);

extern void cyg_interrupt_post_dsr( CYG_ADDRWORD intr_obj );

static inline cyg_uint32
hal_call_isr (cyg_uint32 vector)
{
    cyg_ISR *isr;
    CYG_ADDRWORD data;
    cyg_uint32 isr_ret;

    isr = (cyg_ISR*) hal_interrupt_handlers[vector];
    data = hal_interrupt_data[vector];

    isr_ret = (*isr) (vector, data);

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    if (isr_ret & CYG_ISR_CALL_DSR) {
        cyg_interrupt_post_dsr (hal_interrupt_objects[vector]);
    }
#endif

    return isr_ret & ~CYG_ISR_CALL_DSR;
}

void _scrub_ecc(unsigned p)
{
    asm volatile ("ldrb r4, [%0]\n"
		  "strb r4, [%0]\n" : : "r"(p) );
}

static cyg_uint32 nmi_mcu_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 eccr_reg;

    // Read current state of ECC register
    eccr_reg = *ECCR_REG;

    // Turn off all ecc error reporting
    *ECCR_REG = 0x4;

    // Check for ECC Error 0
    if(*MCISR_REG & 0x1) {
	
#ifdef DEBUG_NMI
	diag_printf("ELOG0 = 0x%X\n", *ELOG0_REG);
	diag_printf("ECC Error Detected at Address 0x%X\n",*ECAR0_REG);		
#endif
	
	// Check for single-bit error
        if(!(*ELOG0_REG & 0x00000100)) {
	    // call ECC restoration function
	    _scrub_ecc(*ECAR0_REG);

	    // Clear the MCISR
	    *MCISR_REG = 0x1;
        } else {
#ifdef DEBUG_NMI
            diag_printf("Multi-bit or nibble error\n");
#endif
	}
    }

    // Check for ECC Error 1
    if(*MCISR_REG & 0x2) {

#ifdef DEBUG_NMI
	diag_printf("ELOG0 = 0x%X\n",*ELOG1_REG);
	diag_printf("ECC Error Detected at Address 0x%X\n",*ECAR1_REG);	
#endif
        
	// Check for single-bit error
        if(!(*ELOG1_REG & 0x00000100))  {
	    // call ECC restoration function
	    _scrub_ecc(*ECAR1_REG);
 
	    // Clear the MCISR
	    *MCISR_REG = 0x2;
	}
	else {
#ifdef DEBUG_NMI
            diag_printf("Multi-bit or nibble error\n");
#endif
	}
    }

    // Check for ECC Error N
    if(*MCISR_REG & 0x4) {
	// Clear the MCISR
	*MCISR_REG = 0x4;
	diag_printf("Uncorrectable error during RMW\n");
    }
    
    // Restore ECCR register
    *ECCR_REG = eccr_reg;

    // clear the interrupt condition
    *MCISR_REG = *MCISR_REG & 7;

    return CYG_ISR_HANDLED;
}

static cyg_uint32 nmi_patu_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 status;

    status = *PATUISR_REG;

#ifdef DEBUG_NMI
    if (status & 0x001) diag_printf ("PPCI Master Parity Error\n");
    if (status & 0x002) diag_printf ("PPCI Target Abort (target)\n");
    if (status & 0x004) diag_printf ("PPCI Target Abort (master)\n");
    if (status & 0x008) diag_printf ("PPCI Master Abort\n");
    if (status & 0x010) diag_printf ("Primary P_SERR# Detected\n");
    if (status & 0x080) diag_printf ("Internal Bus Master Abort\n");
    if (status & 0x100) diag_printf ("PATU BIST Interrupt\n");
    if (status & 0x200) diag_printf ("PPCI Parity Error Detected\n");
    if (status & 0x400) diag_printf ("Primary P_SERR# Asserted\n");
#endif

    *PATUISR_REG = status & 0x79f;
    *PATUSR_REG |= 0xf900;

    return CYG_ISR_HANDLED;
}


static cyg_uint32 nmi_satu_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 status;

    status = *SATUISR_REG;

#ifdef DEBUG_NMI
    if (status & 0x001) diag_printf ("SPCI Master Parity Error\n");
    if (status & 0x002) diag_printf ("SPCI Target Abort (target)\n");
    if (status & 0x004) diag_printf ("SPCI Target Abort (master)\n");
    if (status & 0x008) diag_printf ("SPCI Master Abort\n");
    if (status & 0x010) diag_printf ("Secondary P_SERR# Detected\n");
    if (status & 0x080) diag_printf ("Internal Bus Master Abort\n");
    if (status & 0x200) diag_printf ("SPCI Parity Error Detected\n");
    if (status & 0x400) diag_printf ("Secondary P_SERR# Asserted\n");
#endif

    *SATUISR_REG = status & 0x69f;
    *SATUSR_REG |= 0xf900;

    return CYG_ISR_HANDLED;
}

static cyg_uint32 nmi_pb_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 status;

    status = *PBISR_REG;

#ifdef DEBUG_NMI
    if (status & 0x001) diag_printf ("PPCI Master Parity Error\n");
    if (status & 0x002) diag_printf ("PPCI Target Abort (target)\n");
    if (status & 0x004) diag_printf ("PPCI Target Abort (master)\n");
    if (status & 0x008) diag_printf ("PPCI Master Abort\n");
    if (status & 0x010) diag_printf ("Primary P_SERR# Asserted\n");
    if (status & 0x020) diag_printf ("PPCI Parity Error Detected\n");
#endif

    *PBISR_REG = status & 0x3f;
    *PSR_REG |= 0xf900;

    return CYG_ISR_HANDLED;
}


static cyg_uint32 nmi_sb_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 status;

    status = *SBISR_REG;

    *SBISR_REG = status & 0x7f;
    *SSR_REG |= 0xf900;

    return CYG_ISR_HANDLED;
}


static cyg_uint32 nfiq_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 sources;
    int i, isr_ret;

    // Check NMI
    sources = *NISR_REG;
    for (i = 0; i < 12; i++) {
	if (sources & (1<<i)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_MCU_ERR + i);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	    return isr_ret;
	}
    }
    return 0;
}

static cyg_uint32 nirq_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
    cyg_uint32 sources;
    int i, isr_ret;

    // Check XINT3
    sources = *X3ISR_REG & ~(*X3MASK_REG);
    for (i = 0; i < 5; i++) {
	if (sources & (1 << i)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_TIMER + i);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	    return isr_ret;
	}
    }
    // What to do about S_INTA-S_INTC?

    // Check XINT6
    sources = *X6ISR_REG;
    for (i = 0; i < 3; i++) {
	// check DMA irqs
	if (sources & (1<<i)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_DMA_0 + i);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	    return isr_ret;
	}
    }
    if (sources & 0x10) {
	// performance monitor
	_80312_EMISR = *EMISR_REG;
	if (_80312_EMISR & 1) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_GTSC);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (_80312_EMISR & 0x7ffe) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_PEC);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	return 0;
    }
    if (sources & 0x20) {
	// Application Accelerator Unit
	isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_AAIP);
	CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	return isr_ret;
    }
    
    // Check XINT7
    sources = *X7ISR_REG;
    if (sources & 2) {
	// I2C Unit
	cyg_uint32 i2c_sources = *ISR_REG;
	
	if (i2c_sources & (1<<7)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_I2C_RX_FULL);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (i2c_sources & (1<<6)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_I2C_TX_EMPTY);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (i2c_sources & (1<<10)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_I2C_BUS_ERR);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (i2c_sources & (1<<4)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_I2C_STOP);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (i2c_sources & (1<<5)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_I2C_LOSS);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (i2c_sources & (1<<9)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_I2C_ADDRESS);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	return 0;
    }
    if (sources & 4) {
	// Messaging Unit
	cyg_uint32 inb_sources = *IISR_REG;

	if (inb_sources & (1<<0)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_MESSAGE_0);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (inb_sources & (1<<1)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_MESSAGE_1);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (inb_sources & (1<<2)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_DOORBELL);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (inb_sources & (1<<4)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_QUEUE_POST);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	if (inb_sources & (1<<6)) {
	    isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_INDEX_REGISTER);
	    CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
	}
	return 0;
    }
    if (sources & 8) {
	// BIST
	isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_BIST);
	CYG_ASSERT (isr_ret & CYG_ISR_HANDLED, "Interrupt not handled");
    }

    return 0;
}

// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.
int hal_IRQ_handler(void)
{
    int sources, masks;

    asm volatile ( // read the interrupt source reg INTSRC
        "mrc      p13,0,%0,c4,c0,0;"
        : "=r"(sources)
        : 
      /*:*/
        );
    asm volatile ( // read the interrupt control reg INTCTL
        "mrc      p13,0,%0,c0,c0,0;"
        : "=r"(masks)
        : 
      /*:*/
        );
    // is a source both unmasked and active?
    if ( (0 != (1 & masks)) && (0 != ((8 << 28) & sources)) )
        return CYGNUM_HAL_INTERRUPT_NFIQ;
    if ( (0 != (2 & masks)) && (0 != ((4 << 28) & sources)) )
        return CYGNUM_HAL_INTERRUPT_NIRQ;
    if ( (0 != (8 & masks)) && (0 != ((2 << 28) & sources)) )
        return CYGNUM_HAL_INTERRUPT_BCU_INTERRUPT;
    if ( (0 != (4 & masks)) && (0 != ((1 << 28) & sources)) ) {
        // more complicated; it's the PMU.
        asm volatile ( // read the PMNC perfmon control reg
            "mrc      p14,0,%0,c0,c0,0;"
            : "=r"(sources)
            : 
            /*:*/
            );
        // sources is now the PMNC performance monitor control register
        // enable bits are 4..6, status bits are 8..10
        sources = (sources >> 4) & (sources >> 8);
        if ( 1 & sources )
            return CYGNUM_HAL_INTERRUPT_PMU_PMN0_OVFL;
        if ( 2 & sources )
            return CYGNUM_HAL_INTERRUPT_PMU_PMN1_OVFL;
        if ( 4 & sources )
            return CYGNUM_HAL_INTERRUPT_PMU_CCNT_OVFL;
    }

    return CYGNUM_HAL_INTERRUPT_NONE; // This shouldn't happen!
}

//
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    int mask = 0;
    int submask = 0;
    switch ( vector ) {
    case CYGNUM_HAL_INTERRUPT_PMU_PMN0_OVFL:
    case CYGNUM_HAL_INTERRUPT_PMU_PMN1_OVFL:
    case CYGNUM_HAL_INTERRUPT_PMU_CCNT_OVFL:
        submask = vector - CYGNUM_HAL_INTERRUPT_PMU_PMN0_OVFL; // 0 to 2
        // select interrupt enable bit and also enable the perfmon per se
        submask = (1 << (submask + 4)); // bits 4-6 are masks
        asm volatile (
            "mrc      p14,0,r1,c0,c0,0;"
            "bic      r1, r1, #0x700;" // clear the overflow/interrupt flags
            "bic      r1, r1, #0x006;" // clear the reset bits
            "bic      %0, r1, %0;"     // preserve r1; better for debugging
            "tsts     %0, #0x070;"     // are all 3 sources now off?
            "biceq    %0, %0, #1;"     // if so, disable entirely.
            "mcr      p14,0,%0,c0,c0,0;"
            :
            : "r"(submask)
            : "r1"
            );
        mask = 4;
        break;
    case CYGNUM_HAL_INTERRUPT_BCU_INTERRUPT:
        // Nothing specific to do here
        mask = 8;
        break;
    case CYGNUM_HAL_INTERRUPT_NIRQ         :
        mask = 2;
        break;
    case CYGNUM_HAL_INTERRUPT_NFIQ         :
        mask = 1;
        break;
    case CYGNUM_HAL_INTERRUPT_GTSC:
        *GTMR_REG &= ~1;
        return;
    case CYGNUM_HAL_INTERRUPT_PEC:
        *ESR_REG &= ~(1<<16);
        return;
    case CYGNUM_HAL_INTERRUPT_AAIP:
        *ADCR_REG &= ~1;
        return;
    case CYGNUM_HAL_INTERRUPT_I2C_TX_EMPTY ... CYGNUM_HAL_INTERRUPT_I2C_ADDRESS:
        *ICR_REG &= ~(1<<(vector - CYGNUM_HAL_INTERRUPT_I2C_TX_EMPTY));
	return;
    case CYGNUM_HAL_INTERRUPT_MESSAGE_0 ... CYGNUM_HAL_INTERRUPT_INDEX_REGISTER:
        *IIMR_REG &= ~(1<<(vector - CYGNUM_HAL_INTERRUPT_MESSAGE_0));
	return;
    case CYGNUM_HAL_INTERRUPT_BIST:
        *ATUCR_REG &= ~(1<<3);
	return;
    case CYGNUM_HAL_INTERRUPT_P_SERR:  // FIQ
        *ATUCR_REG &= ~(1<<9);
	return;
    case CYGNUM_HAL_INTERRUPT_S_SERR:  // FIQ
        *ATUCR_REG &= ~(1<<10);
	return;
    case CYGNUM_HAL_INTERRUPT_TIMER ... CYGNUM_HAL_INTERRUPT_PCI_S_INTD:
        *X3MASK_REG |= (1<<(vector - CYGNUM_HAL_INTERRUPT_TIMER));
	return;

    // The hardware doesn't (yet?) provide masking or status for these
    // even though they can trigger cpu interrupts. ISRs will need to
    // poll the device to see if the device actually triggered the
    // interrupt.
    case CYGNUM_HAL_INTERRUPT_PCI_S_INTC:
    case CYGNUM_HAL_INTERRUPT_PCI_S_INTB:
    case CYGNUM_HAL_INTERRUPT_PCI_S_INTA:
    default:
        /* do nothing */
        return;
    }
    asm volatile (
        "mrc      p13,0,r1,c0,c0,0;"
        "bic      r1, r1, %0;"
        "mcr      p13,0,r1,c0,c0,0;"
        :
        : "r"(mask)
        : "r1"
        );
}

void hal_interrupt_unmask(int vector)
{
    int mask = 0;
    int submask = 0;
    switch ( vector ) {
    case CYGNUM_HAL_INTERRUPT_PMU_PMN0_OVFL:
    case CYGNUM_HAL_INTERRUPT_PMU_PMN1_OVFL:
    case CYGNUM_HAL_INTERRUPT_PMU_CCNT_OVFL:
        submask = vector - CYGNUM_HAL_INTERRUPT_PMU_PMN0_OVFL; // 0 to 2
        // select interrupt enable bit and also enable the perfmon per se
        submask = 1 + (1 << (submask + 4)); // bits 4-6 are masks
        asm volatile (
            "mrc      p14,0,r1,c0,c0,0;"
            "bic      r1, r1, #0x700;"   // clear the overflow/interrupt flags
            "bic      r1, r1, #0x006;"   // clear the reset bits
            "orr      %0, r1, %0;"       // preserve r1; better for debugging
            "mcr      p14,0,%0,c0,c0,0;"
            "mrc      p13,0,r2,c8,c0,0;" // steer PMU interrupt to IRQ
            "and      r2, r2, #2;"       // preserve the other bit (BCU steer)
            "mcr      p13,0,r2,c8,c0,0;"
            :
            : "r"(submask)
            : "r1","r2"
            );
        mask = 4;
        break;
    case CYGNUM_HAL_INTERRUPT_BCU_INTERRUPT:
         asm volatile (
            "mrc      p13,0,r2,c8,c0,0;" // steer BCU interrupt to IRQ
            "and      r2, r2, #1;"       // preserve the other bit (PMU steer)
            "mcr      p13,0,r2,c8,c0,0;"
            :
            : 
            : "r2"
            );
        mask = 8;
        break;
    case CYGNUM_HAL_INTERRUPT_NIRQ         :
        mask = 2;
        break;
    case CYGNUM_HAL_INTERRUPT_NFIQ         :
        mask = 1;
        break;
    case CYGNUM_HAL_INTERRUPT_GTSC:
        *GTMR_REG |= 1;
        return;
    case CYGNUM_HAL_INTERRUPT_PEC:
        *ESR_REG |= (1<<16);
        return;
    case CYGNUM_HAL_INTERRUPT_AAIP:
        *ADCR_REG |= 1;
        return;
    case CYGNUM_HAL_INTERRUPT_I2C_TX_EMPTY ... CYGNUM_HAL_INTERRUPT_I2C_ADDRESS:
        *ICR_REG |= (1<<(vector - CYGNUM_HAL_INTERRUPT_I2C_TX_EMPTY));
	return;
    case CYGNUM_HAL_INTERRUPT_MESSAGE_0 ... CYGNUM_HAL_INTERRUPT_INDEX_REGISTER:
        *IIMR_REG |= (1<<(vector - CYGNUM_HAL_INTERRUPT_MESSAGE_0));
	return;
    case CYGNUM_HAL_INTERRUPT_BIST:
        *ATUCR_REG |= (1<<3);
	return;
    case CYGNUM_HAL_INTERRUPT_P_SERR:  // FIQ
        *ATUCR_REG |= (1<<9);
	return;
    case CYGNUM_HAL_INTERRUPT_S_SERR:  // FIQ
        *ATUCR_REG |= (1<<10);
	return;
    case CYGNUM_HAL_INTERRUPT_TIMER ... CYGNUM_HAL_INTERRUPT_PCI_S_INTD:
        *X3MASK_REG &= ~(1<<(vector - CYGNUM_HAL_INTERRUPT_TIMER));
	return;
	
    // The hardware doesn't (yet?) provide masking or status for these
    // even though they can trigger cpu interrupts. ISRs will need to
    // poll the device to see if the device actually triggered the
    // interrupt.
    case CYGNUM_HAL_INTERRUPT_PCI_S_INTC:
    case CYGNUM_HAL_INTERRUPT_PCI_S_INTB:
    case CYGNUM_HAL_INTERRUPT_PCI_S_INTA:
    default:
        /* do nothing */
        return;
    }
    asm volatile (
        "mrc      p13,0,r1,c0,c0,0;"
        "orr      %0, r1, %0;"
        "mcr      p13,0,%0,c0,c0,0;"
        :
        : "r"(mask)
        : "r1"
        );
}

void hal_interrupt_acknowledge(int vector)
{
    int submask = 0;
    switch ( vector ) {
    case CYGNUM_HAL_INTERRUPT_PMU_PMN0_OVFL:
    case CYGNUM_HAL_INTERRUPT_PMU_PMN1_OVFL:
    case CYGNUM_HAL_INTERRUPT_PMU_CCNT_OVFL:
        submask = vector - CYGNUM_HAL_INTERRUPT_PMU_PMN0_OVFL; // 0 to 2
        // select interrupt enable bit and also enable the perfmon per se
        submask = (1 << (submask + 8)); // bits 8-10 are status; write 1 clr
        // Careful not to ack other interrupts or zero any counters:
        asm volatile (
            "mrc      p14,0,r1,c0,c0,0;"
            "bic      r1, r1, #0x700;" // clear the overflow/interrupt flags
            "bic      r1, r1, #0x006;" // clear the reset bits
            "orr      %0, r1, %0;"     // preserve r1; better for debugging
            "mcr      p14,0,%0,c0,c0,0;"
            :
            : "r"(submask)
            : "r1"
            );
        break;
    case CYGNUM_HAL_INTERRUPT_BCU_INTERRUPT:
    case CYGNUM_HAL_INTERRUPT_NIRQ         :
    case CYGNUM_HAL_INTERRUPT_NFIQ         :
    default:
        /* do nothing */
        return;
    }
}

void hal_interrupt_configure(int vector, int level, int up)
{
}

void hal_interrupt_set_level(int vector, int level)
{
}

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
/*------------------------------------------------------------------------*/
//  HW Debug support

static inline void set_ibcr0(unsigned x)
{
    asm volatile ("mcr p15,0,%0,c14,c8,0" : : "r"(x) );
}

static inline unsigned get_ibcr0(void)
{
    unsigned x;
    asm volatile ("mrc p15,0,%0,c14,c8,0" : "=r"(x) : );
    return x;
}

static inline void set_ibcr1(unsigned x)
{
    asm volatile ("mcr p15,0,%0,c14,c9,0" : : "r"(x) );
}

static inline unsigned get_ibcr1(void)
{
    unsigned x;
    asm volatile ("mrc p15,0,%0,c14,c9,0" : "=r"(x) : );
    return x;
}

static inline void set_dbr0(unsigned x)
{
    asm volatile ("mcr p15,0,%0,c14,c0,0" : : "r"(x) );
}

static inline unsigned get_dbr0(void)
{
    unsigned x;
    asm volatile ("mrc p15,0,%0,c14,c0,0" : "=r"(x) : );
    return x;
}

static inline void set_dbr1(unsigned x)
{
    asm volatile ("mcr p15,0,%0,c14,c3,0" : : "r"(x) );
}

static inline unsigned get_dbr1(void)
{
    unsigned x;
    asm volatile ("mrc p15,0,%0,c14,c3,0" : "=r"(x) : );
    return x;
}

static inline void set_dbcon(unsigned x)
{
    asm volatile ("mcr p15,0,%0,c14,c4,0" : : "r"(x) );
}

static inline unsigned get_dbcon(void)
{
    unsigned x;
    asm volatile ("mrc p15,0,%0,c14,c4,0" : "=r"(x) : );
    return x;
}

static inline void set_dcsr(unsigned x)
{
    asm volatile ("mcr p14,0,%0,c10,c0,0" : : "r"(x) );
}

static inline unsigned get_dcsr(void)
{
    unsigned x;
    asm volatile ("mrc p14,0,%0,c10,c0,0" : "=r"(x) : );
    return x;
}


int cyg_hal_plf_hw_breakpoint(int setflag, void *vaddr, int len)
{
    unsigned int addr = (unsigned)vaddr;

    if (setflag) {
	if (!(get_ibcr0() & 1))
	    set_ibcr0(addr | 1);
	else if (!(get_ibcr1() & 1))
	    set_ibcr1(addr | 1);
	else
	    return -1;
    } else {
	unsigned x = (addr | 1);
	if (get_ibcr0() == x)
	    set_ibcr0(0);
	else if (get_ibcr0() == x)
	    set_ibcr1(0);
	else
	    return -1;
    }
    return 0;
}

int cyg_hal_plf_hw_watchpoint(int setflag, void *vaddr, int len, int type)
{
    unsigned int mask, bit_nr, mode, addr = (unsigned)vaddr;
    unsigned dbcon = get_dbcon();

    mask = 0x80000000;
    bit_nr = 31;
    while (bit_nr) {
	if (len & mask)
	    break;
	bit_nr--;
	mask >>= 1;
    }
    mask = ~(0xffffffff << bit_nr);

    if (setflag) {
	/* set a watchpoint */
	if (type == 2)
	    mode = 1; // break on write
	else if (type == 3)
	    mode = 3; // break on read
	else if (type == 4)
	    mode = 2; // break on any access
	else
	    return 1;

	if (!(dbcon & 3)) {
	    set_dbr0(addr);
	    set_dbr1(mask);
	    set_dbcon(dbcon | mode | 0x100);
	} else
	    return 1;
    } else {
	/* clear a watchpoint */
	if (dbcon & 3)
	    set_dbcon(dbcon & ~3);
	else
	    return 1;
    }
    return 0;
}

// Return indication of whether or not we stopped because of a
// watchpoint or hardware breakpoint. If stopped by a watchpoint,
// also set '*data_addr_p' to the data address which triggered the
// watchpoint.
int cyg_hal_plf_is_stopped_by_hardware(void **data_addr_p)
{
    unsigned fsr, dcsr, dbcon, kind = 0;

    // Check for debug event
    asm volatile ("mrc p15,0,%0,c5,c0,0" : "=r"(fsr) : );
    if ((fsr & 0x200) == 0)
	return HAL_STUB_HW_STOP_NONE;

    // There was a debug event. Check the MOE for details
    dcsr = get_dcsr();
    switch ((dcsr >> 2) & 7) {
      case 1:  // HW breakpoint
      case 3:  // BKPT breakpoint
	return HAL_STUB_HW_STOP_BREAK;
      case 2:  // Watchpoint
	dbcon = get_dbcon();
	if (dbcon & 0x100) {
	    // dbr1 is used as address mask
	    kind = dbcon & 3;
	    *data_addr_p = (void *)get_dbr0();
	}
	if (kind == 1)
	    return HAL_STUB_HW_STOP_WATCH;
	if (kind == 2)
	    return HAL_STUB_HW_STOP_AWATCH;
	if (kind == 3)
	    return HAL_STUB_HW_STOP_RWATCH;
	// should never get here
	break;
    }
    return HAL_STUB_HW_STOP_NONE;
}
#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

/*------------------------------------------------------------------------*/
// EOF iq80310_misc.c
