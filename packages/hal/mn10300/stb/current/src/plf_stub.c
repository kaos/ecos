//=============================================================================
//
//      plf_stub.c
//
//      Platform specific code for GDB stub support.
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, jskov (based on the old mn10300 hal_stub.c)
// Contributors:nickg, jskov
// Date:        1999-02-12
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>

#include <cyg/hal/hal_io.h>             // HAL IO macros
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros

//---------------------------------------------------------------------------
// MN10300 Serial line

// We use serial0 on AM33
#define SERIAL0_CR       ((volatile cyg_uint16 *)0xd4002000)
#define SERIAL0_ICR      ((volatile cyg_uint8 *) 0xd4002004)
#define SERIAL0_TXR      ((volatile cyg_uint8 *) 0xd4002008)
#define SERIAL0_RXR      ((volatile cyg_uint8 *) 0xd4002009)
#define SERIAL0_SR       ((volatile cyg_uint16 *)0xd400200c)

// Timer 2 provides baud rate divisor
#define TIMER0_MD       ((volatile cyg_uint8 *)0xd4003002)
#define TIMER0_BR       ((volatile cyg_uint8 *)0xd4003012)
#define TIMER0_CR       ((volatile cyg_uint8 *)0xd4003022)

#define SIO1_LSTAT_TRDY  0x20
#define SIO1_LSTAT_RRDY  0x10


//---------------------------------------------------------------------------

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
// This ISR is called from the interrupt handler. This should only
// happen when there is no serial driver, so the code shouldn't mess
// anything up.
int cyg_hal_gdb_isr(cyg_uint32 vector, target_register_t pc)
{
    if ( CYGNUM_HAL_INTERRUPT_SERIAL_0_RX == vector ) {
        cyg_uint8 c;

        HAL_READ_UINT8 (SERIAL0_RXR, c);
        HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SERIAL_0_RX);

        if( 3 == c )
        {
            // Ctrl-C: set a breakpoint at PC so GDB will display the
            // correct program context when stopping rather than the
            // interrupt handler.
            cyg_hal_gdb_interrupt (pc);

            // Interrupt handled. Don't call ISR proper. At return
            // from the VSR, execution will stop at the breakpoint
            // just set.
            return 0;
        }
    }

    // Not caused by GDB. Call ISR proper.
    return 1;
}

int hal_stb_interruptible(int state)
{
    if (state) {
        HAL_WRITE_UINT8 (SERIAL0_ICR, 0);
        HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SERIAL_0_RX)
        HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SERIAL_0_RX)
    } else {
        HAL_INTERRUPT_MASK (CYGNUM_HAL_INTERRUPT_SERIAL_0_RX)
    }
    return 0;
}

void hal_stb_init_break_irq( void )
{
    // Enable serial receive interrupts.
    HAL_WRITE_UINT8 (SERIAL0_ICR, 0);
    HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SERIAL_0_RX)
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SERIAL_0_RX)
    HAL_ENABLE_INTERRUPTS();
}
#endif

// Initialize the current serial port.
void hal_stb_init_serial( void )
{
    // 99 translates to 38400 baud.
    HAL_WRITE_UINT8 (TIMER0_BR, 99);

    // Timer0 sourced from IOCLK
    HAL_WRITE_UINT8 (TIMER0_MD, 0x80);

    // No interrupts for now.
    HAL_WRITE_UINT8 (SERIAL0_ICR, 0x00);

    // Source from timer 1, 8bit chars, enable tx and rx
    HAL_WRITE_UINT16 (SERIAL0_CR, 0xc085);

}

// Write C to the current serial port.
void hal_stb_put_char( int c )
{
    cyg_uint16 sr;

    do {
        HAL_READ_UINT16 (SERIAL0_SR, sr);
    } while ((sr & SIO1_LSTAT_TRDY) != 0);

    HAL_WRITE_UINT8 (SERIAL0_TXR, c);
}

// Read one character from the current serial port.
int hal_stb_get_char( void )
{
    char c;
    cyg_uint16 sr;

    do {
        HAL_READ_UINT16 (SERIAL0_SR, sr);
    } while ((sr & SIO1_LSTAT_RRDY) == 0);

    HAL_READ_UINT8 (SERIAL0_RXR, c);

    return c;
}

//-----------------------------------------------------------------------------

void hal_stb_platform_init(void)
{
    extern CYG_ADDRESS hal_virtual_vector_table[64];
    extern void init_thread_syscall( void *);
    extern void install_async_breakpoint(void *epc);
//    void (*oldvsr)(void);
    extern void __default_exception_vsr(void);

    // Ensure that the breakpoint VSR points to the default VSR. This will pass
    // it on to the stubs.
//    HAL_VSR_SET( CYGNUM_HAL_VECTOR_BREAKPOINT, __default_exception_vsr, &oldvsr );

    // Install async breakpoint handler into vector table.
    hal_virtual_vector_table[35] = (CYG_ADDRESS)install_async_breakpoint;

#if !defined(CYGPKG_KERNEL) && defined(CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT)
    // Only include this code if we do not have a kernel. Otherwise
    // the kernel supplies the functionality for the app we are linked
    // with.

    // Prepare for application installation of thread info function in
    // vector table.
    hal_virtual_vector_table[15] = 0;
    init_thread_syscall( (void *)&hal_virtual_vector_table[15] );

#endif
}


#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
