/*==========================================================================
//
//      hal_misc.c
//
//      Cortex-M exception vectors
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Author(s):    nickg
// Date:         2008-07-30
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header

#include <cyg/hal/drv_api.h>


#if defined(CYGPKG_KERNEL_INSTRUMENT) &&        \
    defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
#include <cyg/kernel/instrmnt.h>
#endif

//==========================================================================

typedef cyg_uint32 cyg_isr(cyg_uint32 vector, CYG_ADDRWORD data);

//==========================================================================
// External references

// VSRs in vectors.S
__externC void hal_default_exception_vsr( void );
__externC void hal_default_interrupt_vsr( void );
__externC void hal_default_svc_vsr( void );
__externC void hal_pendable_svc_vsr( void );
__externC void hal_switch_state_vsr( void );

// HAL and eCos functions
__externC void hal_system_init( void );
__externC void hal_variant_init( void );
__externC void hal_platform_init( void );
__externC void hal_ctrlc_isr_init( void );
__externC void initialize_stub( void );
__externC void cyg_hal_invoke_constructors( void );
__externC void cyg_start( void );
__externC void cyg_interrupt_post_dsr( CYG_ADDRWORD intr_obj );
__externC void interrupt_end(cyg_uint32 isr_ret, CYG_ADDRWORD intr, HAL_SavedRegisters *regs );

// DATA and BSS locations
__externC cyg_uint32 __ram_data_start;
__externC cyg_uint32 __ram_data_end;
__externC cyg_uint32 __rom_data_start;
__externC cyg_uint32 __sram_data_start;
__externC cyg_uint32 __sram_data_end;
__externC cyg_uint32 __srom_data_start;
__externC cyg_uint32 __bss_start;
__externC cyg_uint32 __bss_end;

// Scheduler lock
__externC volatile cyg_uint32 cyg_scheduler_sched_lock;

//==========================================================================
// Interrupt tables

volatile CYG_ADDRESS   hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRWORD  hal_interrupt_data    [CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRESS   hal_interrupt_objects [CYGNUM_HAL_ISR_COUNT];

//==========================================================================
// Main entry point
//
// Enter here from reset via slot 1 of VSR table. The stack pointer is
// already set to the value in VSR slot 0, usually the top of internal
// SRAM.

void hal_reset_vsr( void )
{
    // Call system init routine. This should do the minimum necessary
    // for the rest of the initialization to complete. For example set
    // up GPIO, the SRAM, power management etc. This routine is
    // usually supplied by the platform HAL. Calls to
    // hal_variant_init() and hal_platform_init() later will perform
    // the main initialization.
    
    hal_system_init();
    
    // Initialize vector table in base of SRAM.
    {
        register int i;

#if !defined(CYG_HAL_STARTUP_RAM)

        // Only install the exception vectors for non-RAM startup. For
        // RAM startup we want these to continue to point to the original
        // VSRs, which will belong to RedBoot or GDB stubs.
        
        for( i = 2; i < 15; i++ )
            hal_vsr_table[i] = (CYG_ADDRESS)hal_default_exception_vsr;

#endif
        // Always point SVC and PENDSVC vectors to our local versions
        
        hal_vsr_table[CYGNUM_HAL_VECTOR_SERVICE] = (CYG_ADDRESS)hal_default_svc_vsr;
        hal_vsr_table[CYGNUM_HAL_VECTOR_PENDSV] = (CYG_ADDRESS)hal_pendable_svc_vsr;

        // For all startup type, redirect interrupt vectors to our VSR.
        for( i = CYGNUM_HAL_VECTOR_SYS_TICK ;
             i < CYGNUM_HAL_VECTOR_SYS_TICK + CYGNUM_HAL_VSR_MAX;
             i++ )
            hal_vsr_table[i] = (CYG_ADDRESS)hal_default_interrupt_vsr;
    }


#if !defined(CYG_HAL_STARTUP_RAM)

    // Ensure that the CPU will use the vector table we have just set
    // up.

# if defined(CYGHWR_HAL_CORTEXM_M3)

    // On M3 parts, the NVIC contains a vector table base register. We
    // program this to relocate the vector table base to the base of
    // SRAM.
    
    HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_VTOR,
                      CYGARC_REG_NVIC_VTOR_TBLOFF(0)|
                      CYGARC_REG_NVIC_VTOR_TBLBASE_SRAM );
    
# else

#  error Unknown SRAM/VECTAB remap mechanism

# endif

    // Use SVC to switch our state to thread mode running on the PSP.
    // We don't need to do this for RAM startup since the ROM code
    // will have already done it.

    hal_vsr_table[CYGNUM_HAL_VECTOR_SERVICE] = (CYG_ADDRESS)hal_switch_state_vsr;    

    __asm__ volatile( "swi" );

    hal_vsr_table[CYGNUM_HAL_VECTOR_SERVICE] = (CYG_ADDRESS)hal_default_svc_vsr;
    
#endif // !defined(CYG_HAL_STARTUP_RAM)
    
#if defined(CYG_HAL_STARTUP_ROM)
    // Relocate data from ROM to RAM
    {
        register cyg_uint32 *p, *q;
        for( p = &__ram_data_start, q = &__rom_data_start;
             p < &__ram_data_end;
             p++, q++ )
            *p = *q;
    }

    // Relocate data from ROM to SRAM
    {
        register cyg_uint32 *p, *q;
        for( p = &__sram_data_start, q = &__srom_data_start;
             p < &__sram_data_end;
             p++, q++ )
            *p = *q;
    }
#endif

    // Clear BSS
    {
        register cyg_uint32 *p;
        for( p = &__bss_start; p < &__bss_end; p++ )
            *p = 0;
    }

    // Initialize interrupt vectors. Set the levels for all interrupts
    // to default values. Also set the default priorities of the
    // system handlers: all exceptions maximum priority except SVC and
    // PendSVC which are lowest priority.
    {
        register int i;
        
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SHPR0, 0x00000000 );
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SHPR1, 0xFF000000 );
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SHPR2, 0x00FF0000 );
        
        hal_interrupt_handlers[CYGNUM_HAL_INTERRUPT_SYS_TICK] = (CYG_ADDRESS)hal_default_isr;
        
        for( i = 1; i < CYGNUM_HAL_ISR_COUNT; i++ )
        {
            hal_interrupt_handlers[i] = (CYG_ADDRESS)hal_default_isr;
            HAL_WRITE_UINT8( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_PR(i-CYGNUM_HAL_INTERRUPT_EXTERNAL), 0x80 );
        }
    }

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
    // Enable DebugMonitor exceptions. This is needed to enable single
    // step. This only has an effect if no external JTAG device is
    // attached.
    {
        CYG_ADDRESS base = CYGARC_REG_DEBUG_BASE;
        cyg_uint32 demcr;

        HAL_READ_UINT32( base+CYGARC_REG_DEBUG_DEMCR, demcr );
        demcr |= CYGARC_REG_DEBUG_DEMCR_MON_EN;
        HAL_WRITE_UINT32( base+CYGARC_REG_DEBUG_DEMCR, demcr );
    }
#endif

#if !defined(CYG_HAL_STARTUP_RAM)    
    // Enable Usage, Bus and Mem fault handlers. Do this for ROM and
    // JTAG startups. For RAM startups, this will have already been
    // done by the ROM monitor.
    {
        CYG_ADDRESS base = CYGARC_REG_NVIC_BASE;
        cyg_uint32 shcsr;

        HAL_READ_UINT32( base+CYGARC_REG_NVIC_SHCSR, shcsr );
        shcsr |= CYGARC_REG_NVIC_SHCSR_USGFAULTENA;
        shcsr |= CYGARC_REG_NVIC_SHCSR_BUSFAULTENA;
        shcsr |= CYGARC_REG_NVIC_SHCSR_MEMFAULTENA;
        HAL_WRITE_UINT32( base+CYGARC_REG_NVIC_SHCSR, shcsr );
    }
#endif
    
    // Call variant and platform init routines
    hal_variant_init();
    hal_platform_init();

    // Start up the system clock
    HAL_CLOCK_INITIALIZE( CYGNUM_HAL_RTC_PERIOD );
            
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    
    initialize_stub();
    
#endif

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) || \
    defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
    
    hal_ctrlc_isr_init();
    
#endif

    // Run through static constructors
    cyg_hal_invoke_constructors();

    // Finally call into application
    cyg_start();
    for(;;);
}

//==========================================================================
// Handle Exceptions
//
// Exceptions are passed here from the initial VSR. We pass the
// exception on to GDB stubs or the kernel as appropriate.

__externC void __handle_exception (void);

__externC HAL_SavedRegisters *_hal_registers;
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
__externC void* volatile __mem_fault_handler;
#endif

void hal_deliver_exception( HAL_SavedRegisters *regs )
{
    // Special case handler for code which has chosen to take care
    // of data exceptions (i.e. code which expects them to happen)
    // This is common in discovery code, e.g. checking for a particular
    // device which may generate an exception when probing if the
    // device is not present
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    if (__mem_fault_handler )
    {
        regs->u.exception.pc = (unsigned long)__mem_fault_handler;
        return; // Caught an exception inside stubs        
    }
#endif

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

    _hal_registers = regs;
    __handle_exception();

#elif defined(CYGPKG_KERNEL_EXCEPTIONS)

    cyg_hal_deliver_exception( regs->u.exception.vector, (CYG_ADDRWORD)regs );

#else

    CYG_FAIL("Exception!!!");
    
#endif    
}

//==========================================================================
// Handle Interrupts
//
// Interrupts are passed here from the low-level VSR in vectors.S. We
// look up the ISR in the interrupt table, call it and if it requests
// it, post a DSR. If necessary we also then cause the pendable SVC to
// be requested.
//
// This function is also callable from ISR springboards that decode
// additional interrupts via external controllers to deliver an
// interrupt to a secondary vector.

void hal_deliver_interrupt( cyg_uint32 vector )
{
    register cyg_uint32 isr_result;
    register cyg_isr *isr;
    cyg_bool pendsvc = false;
    
#if defined(CYGPKG_KERNEL_INSTRUMENT) && \
    defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
    CYG_INSTRUMENT_INTR(RAISE, vector, 0);
#endif

    isr = (cyg_isr *)hal_interrupt_handlers[vector];
    
    // Call the ISR
    isr_result = isr( vector, hal_interrupt_data[vector] );


#if !defined(CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN)
    // If the ISR has returned the CALL_DSR bit, post the DSR and set
    // the pendable SVC exception pending.
    if( isr_result & CYG_ISR_CALL_DSR )
    {
        cyg_interrupt_post_dsr( hal_interrupt_objects[vector] );

        // Post the pendable SVC to call interrupt_end(). But only if
        // the scheduler lock is currently zero. If it is non zero
        // then interrupt_end will do nothing useful, so avoid calling
        // it.
        if( cyg_scheduler_sched_lock == 0 )
            pendsvc = true;
    }
#else
    // When chaining we don't know here whether the nested interrupt
    // has posted a DSR, so we have to run interrupt_end() regardless.
    // However, the same considerations as above regarding the
    // scheduler lock still apply.
    if( cyg_scheduler_sched_lock == 0 )
        pendsvc = true;
#endif

    // Post the pendable SVC if required.
    if( pendsvc )
    {
        cyg_uint32 icsr;
        HAL_READ_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_ICSR, icsr );
        icsr |= CYGARC_REG_NVIC_ICSR_PENDSVSET;
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_ICSR, icsr );
    }
}

//==========================================================================
// Call interrupt_end()
//
// This is called on the thread stack as a result of the pendable
// SVC. interrupt_end() decrements the scheduler lock, calls DSRs and
// optionally switches thread context. So before calling, we must
// increment the lock. The actual interrupt end processing has already
// been done above in hal_default_interrupt_vsr1(), so the arguments
// are zero.

__externC void hal_interrupt_end( void )
{
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT    
    cyg_scheduler_sched_lock++;
#endif
    
   interrupt_end(0,0,0);
}

//==========================================================================
// Interrupt masking and configuration

#ifndef HAL_VAR_INTERRUPT_MASK
#define HAL_VAR_INTERRUPT_MASK( __vector ) CYG_EMPTY_STATEMENT
#define HAL_VAR_INTERRUPT_UNMASK( __vector ) CYG_EMPTY_STATEMENT
#define HAL_VAR_INTERRUPT_SET_LEVEL( __vector, __level ) CYG_EMPTY_STATEMENT
#define HAL_VAR_INTERRUPT_ACKNOWLEDGE( __vector ) CYG_EMPTY_STATEMENT
#define HAL_VAR_INTERRUPT_CONFIGURE( __vector, __level, __up ) CYG_EMPTY_STATEMENT
#endif

//--------------------------------------------------------------------------

__externC void hal_interrupt_mask( cyg_uint32 vector )
{
    if( vector >= CYGNUM_HAL_INTERRUPT_EXTERNAL &&
        vector <= CYGNUM_HAL_INTERRUPT_NVIC_MAX )
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_CER(vector-CYGNUM_HAL_INTERRUPT_EXTERNAL),
                          CYGARC_REG_NVIC_IBIT(vector-CYGNUM_HAL_INTERRUPT_EXTERNAL) );
    else if( vector == CYGNUM_HAL_INTERRUPT_SYS_TICK )
    {
        cyg_uint32 csr;
        HAL_READ_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_CSR, csr );
        csr &= ~CYGARC_REG_SYSTICK_CSR_TICKINT;
        HAL_WRITE_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_CSR, csr );
    }
    HAL_VAR_INTERRUPT_MASK( vector );
}

//--------------------------------------------------------------------------

__externC void hal_interrupt_unmask( cyg_uint32 vector )
{
    if( vector >= CYGNUM_HAL_INTERRUPT_EXTERNAL &&
        vector <= CYGNUM_HAL_INTERRUPT_NVIC_MAX )
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SER(vector-CYGNUM_HAL_INTERRUPT_EXTERNAL),
                          CYGARC_REG_NVIC_IBIT(vector-CYGNUM_HAL_INTERRUPT_EXTERNAL) );
    else if( vector == CYGNUM_HAL_INTERRUPT_SYS_TICK )
    {
        cyg_uint32 csr;
        HAL_READ_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_CSR, csr );
        csr |= CYGARC_REG_SYSTICK_CSR_TICKINT;
        HAL_WRITE_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_CSR, csr );
    }
    HAL_VAR_INTERRUPT_UNMASK( vector );
}

//--------------------------------------------------------------------------

__externC void hal_interrupt_acknowledge( cyg_uint32 vector )
{
    HAL_VAR_INTERRUPT_ACKNOWLEDGE( vector );
}

//--------------------------------------------------------------------------

__externC void hal_interrupt_configure( cyg_uint32 vector, cyg_uint32 level, cyg_uint32 up )
{
    HAL_VAR_INTERRUPT_CONFIGURE( vector, level, up );
}

//--------------------------------------------------------------------------

__externC void hal_interrupt_set_level( cyg_uint32 vector, cyg_uint32 level )
{
    cyg_uint32 l = (level)+CYGNUM_HAL_CORTEXM_PRIORITY_MAX;
    if( l > 0xFF ) l = 0xFF; /* clamp to 0xFF */
    if( vector >= CYGNUM_HAL_INTERRUPT_EXTERNAL &&
        vector <= CYGNUM_HAL_INTERRUPT_NVIC_MAX )
    {
        HAL_WRITE_UINT8( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_PR(vector-CYGNUM_HAL_INTERRUPT_EXTERNAL),
                         l );
    }
    else if ( vector == CYGNUM_HAL_INTERRUPT_SYS_TICK )
    {
        cyg_uint32 shpr2;
        HAL_READ_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SHPR2, shpr2 );
        shpr2 &= ~0xFF000000;
        shpr2 |= (l)<<24;
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SHPR2, shpr2 );
    }
    HAL_VAR_INTERRUPT_SET_LEVEL( vector, level );
}

//==========================================================================
// Microsecond delay
//
// The system RTC is set up to tick at 1MHz so all we need to do here
// is count ticks.

__externC void hal_delay_us( cyg_int32 us )
{
    cyg_uint32 t0, t1;

    HAL_CLOCK_READ( &t0 );
    while ( us > 0 )
    {
        HAL_CLOCK_READ( &t1 );
        if( t1 < t0 )
            us -= (t1 + CYGNUM_HAL_RTC_PERIOD - t0);
        else
        	us -= t1 - t0;
        t0 = t1;
    }
}

//==========================================================================
// C++ support - run initial constructors

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

typedef void (*pfunc) (void);

extern pfunc __init_array_start__[];
extern pfunc __init_array_end__[];
#define CONSTRUCTORS_START  (__init_array_start__[0])
#define CONSTRUCTORS_END    (__init_array_end__)
#define NEXT_CONSTRUCTOR(c) ((c)++)

void
cyg_hal_invoke_constructors (void)
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    static pfunc *p = &CONSTRUCTORS_START;
    
    cyg_hal_stop_constructors = 0;
    for (; p != CONSTRUCTORS_END; NEXT_CONSTRUCTOR(p)) {
        (*p)();
        if (cyg_hal_stop_constructors) {
            NEXT_CONSTRUCTOR(p);
            break;
        }
    }
#else
    pfunc *p;

    for (p = &CONSTRUCTORS_START; p != CONSTRUCTORS_END; NEXT_CONSTRUCTOR(p))
        (*p)();
#endif
}

//==========================================================================
// Architecture default ISR

__externC cyg_uint32
hal_arch_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_TRACE1(true, "Interrupt: %d", vector);

    CYG_FAIL("Spurious Interrupt!!!");

    return 0;
}

//==========================================================================
// GDB support
//
// These functions translate between HAL saved contexts and GDB
// register dumps.

__externC void hal_get_gdb_registers( HAL_CORTEXM_GDB_Registers *gdbreg, HAL_SavedRegisters *regs )
{
    int i;
    cyg_uint32 *p = gdbreg->f0;
    
    switch( regs->u.type )
    {
    case HAL_SAVEDREGISTERS_THREAD:
        for( i = 0; i < 13; i++ )
            gdbreg->gpr[i] = regs->u.thread.r[i];
        gdbreg->gpr[13] = regs->u.thread.sp;
        gdbreg->gpr[14] = regs->u.thread.pc;
        gdbreg->gpr[15] = regs->u.thread.pc;
        gdbreg->ps = 0x01000000;
        break;
            
    case HAL_SAVEDREGISTERS_EXCEPTION:
        gdbreg->gpr[0] = regs->u.exception.r0;
        gdbreg->gpr[1] = regs->u.exception.r1;
        gdbreg->gpr[2] = regs->u.exception.r2;
        gdbreg->gpr[3] = regs->u.exception.r3;
        for( i = 0; i < 8; i++ )
            gdbreg->gpr[i+4] = regs->u.exception.r4_11[i];
        gdbreg->gpr[12] = regs->u.exception.r12;
        gdbreg->gpr[13] = ((cyg_uint32)regs)+sizeof(regs->u.exception);
        gdbreg->gpr[14] = regs->u.exception.lr;
        gdbreg->gpr[15] = regs->u.exception.pc;
        gdbreg->ps = regs->u.exception.psr;
        break;
                
    case HAL_SAVEDREGISTERS_INTERRUPT:
        gdbreg->gpr[0] = regs->u.interrupt.r0;
        gdbreg->gpr[1] = regs->u.interrupt.r1;
        gdbreg->gpr[2] = regs->u.interrupt.r2;
        gdbreg->gpr[3] = regs->u.interrupt.r3;
        gdbreg->gpr[12] = regs->u.interrupt.r12;
        gdbreg->gpr[13] = ((cyg_uint32)regs)+sizeof(regs->u.interrupt);
        gdbreg->gpr[14] = regs->u.interrupt.lr;
        gdbreg->gpr[15] = regs->u.interrupt.pc;
        gdbreg->ps = regs->u.interrupt.psr;
        break;
    }

    // Clear FP state, which we don't use
    for( i = 0; i < (8*3+1); i++ )
        p[i] = 0;
    
}

__externC void hal_set_gdb_registers( HAL_CORTEXM_GDB_Registers *gdbreg, HAL_SavedRegisters *regs )
{
    int i;
    
    switch( regs->u.type )
    {
    case HAL_SAVEDREGISTERS_THREAD:
        for( i = 0; i < 13; i++ )
            regs->u.thread.r[i] = gdbreg->gpr[i];
        regs->u.thread.sp = gdbreg->gpr[13];
        regs->u.thread.pc = gdbreg->gpr[14];
        regs->u.thread.pc = gdbreg->gpr[15];
        break;
            
    case HAL_SAVEDREGISTERS_EXCEPTION:
        regs->u.exception.r0 = gdbreg->gpr[0];
        regs->u.exception.r1 = gdbreg->gpr[1];
        regs->u.exception.r2 = gdbreg->gpr[2];
        regs->u.exception.r3 = gdbreg->gpr[3];
        for( i = 0; i < 8; i++ )
            regs->u.exception.r4_11[i] = gdbreg->gpr[i+4];
        regs->u.exception.r12 = gdbreg->gpr[12];
        regs->u.exception.lr = gdbreg->gpr[14];
        regs->u.exception.pc = gdbreg->gpr[15];
        regs->u.exception.psr = gdbreg->ps;
        break;
                
    case HAL_SAVEDREGISTERS_INTERRUPT:
        regs->u.interrupt.r0 = gdbreg->gpr[0];
        regs->u.interrupt.r1 = gdbreg->gpr[1];
        regs->u.interrupt.r2 = gdbreg->gpr[2];
        regs->u.interrupt.r3 = gdbreg->gpr[3];
        regs->u.interrupt.r12 = gdbreg->gpr[12];
        regs->u.interrupt.lr = gdbreg->gpr[14];
        regs->u.interrupt.pc = gdbreg->gpr[15];
        regs->u.interrupt.psr = gdbreg->ps;
        break;
    }
}

//==========================================================================
// When compiling C++ code with static objects the compiler
// inserts a call to __cxa_atexit() with __dso_handle as one of the
// arguments. __cxa_atexit() would normally be provided by glibc, and
// __dso_handle is part of crtstuff.c. eCos applications
// are linked rather differently, so either a differently-configured
// compiler is needed or dummy versions of these symbols should be
// provided. If these symbols are not actually used then providing
// them is still harmless, linker garbage collection will remove them.

void
__cxa_atexit(void (*arg1)(void*), void* arg2, void* arg3)
{
}

void*   __dso_handle = (void*) &__dso_handle;

//==========================================================================
// EOF hal_misc.c
