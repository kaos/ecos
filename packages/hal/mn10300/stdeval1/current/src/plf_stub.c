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
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998, 1999 Cygnus Solutions.  
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
// We use serial1 on MN103002
#define SERIAL1_CR       ((volatile cyg_uint16 *)0x34000810)
#define SERIAL1_ICR      ((volatile cyg_uint8 *) 0x34000814)
#define SERIAL1_TXR      ((volatile cyg_uint8 *) 0x34000818)
#define SERIAL1_RXR      ((volatile cyg_uint8 *) 0x34000819)
#define SERIAL1_SR       ((volatile cyg_uint16 *)0x3400081c)

// Timer 1 provided baud rate divisor
#define TIMER1_MD       ((volatile cyg_uint8 *)0x34001001)
#define TIMER1_BR       ((volatile cyg_uint8 *)0x34001011)
#define TIMER1_CR       ((volatile cyg_uint8 *)0x34001021)

#define PORT3_MD        ((volatile cyg_uint8 *)0x36008025)

// Mystery register
#define TMPSCNT         ((volatile cyg_uint8 *)0x34001071)

#define SIO1_LSTAT_TRDY  0x20
#define SIO1_LSTAT_RRDY  0x10


//---------------------------------------------------------------------------

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
// This ISR is called from the interrupt handler. This should only
// happen when there is no serial driver, so the code shouldn't mess
// anything up.
int cyg_hal_gdb_isr(cyg_uint32 vector, target_register_t pc)
{
    if ( CYGNUM_HAL_INTERRUPT_SERIAL_1_RX == vector ) {
        cyg_uint8 c;

        HAL_READ_UINT8 (SERIAL1_RXR, c);
        HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SERIAL_1_RX);
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

int hal_stdeval1_interruptible(int state)
{
    if (state) {
        HAL_WRITE_UINT8 (SERIAL1_ICR, 0);
        HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SERIAL_1_RX)
        HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SERIAL_1_RX)
    } else {
        HAL_INTERRUPT_MASK (CYGNUM_HAL_INTERRUPT_SERIAL_1_RX)
    }
    return 0;
}

void hal_stdeval1_init_break_irq( void )
{
    // Enable serial receive interrupts.
    HAL_WRITE_UINT8 (SERIAL1_ICR, 0);
    HAL_INTERRUPT_ACKNOWLEDGE (CYGNUM_HAL_INTERRUPT_SERIAL_1_RX)
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SERIAL_1_RX)
    HAL_ENABLE_INTERRUPTS();
}
#endif

// Initialize the current serial port.
void hal_stdeval1_init_serial( void )
{
    // 48 translates to 38400 baud.
    HAL_WRITE_UINT8 (TIMER1_BR, 48);

    // Timer1 sourced from IOCLK
    HAL_WRITE_UINT8 (TIMER1_MD, 0x80);

    // Mode on PORT3, used for serial line controls.
    HAL_WRITE_UINT8 (PORT3_MD, 0x01);

    // No interrupts for now.
    HAL_WRITE_UINT8 (SERIAL1_ICR, 0x00);

    // Source from timer 1, 8bit chars, enable tx and rx
    HAL_WRITE_UINT16 (SERIAL1_CR, 0xc084);
}

// Write C to the current serial port.
void hal_stdeval1_put_char( int c )
{
    cyg_uint16 sr;

    do {
        HAL_READ_UINT16 (SERIAL1_SR, sr);
    } while ((sr & SIO1_LSTAT_TRDY) != 0);

    HAL_WRITE_UINT8 (SERIAL1_TXR, c);
}

// Read one character from the current serial port.
int hal_stdeval1_get_char( void )
{
    char c;
    cyg_uint16 sr;

    do {
        HAL_READ_UINT16 (SERIAL1_SR, sr);
    } while ((sr & SIO1_LSTAT_RRDY) == 0);

    HAL_READ_UINT8 (SERIAL1_RXR, c);

    return c;
}

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
