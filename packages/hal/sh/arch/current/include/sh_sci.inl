#ifndef CYGONCE_HAL_SH_SCI_H
#define CYGONCE_HAL_SH_SCI_H
//=============================================================================
//
//      sh_sci.inl
//
//      Simple driver for the SH Serial Communication Interface (SCI)
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
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-05-17
// Description: Simple driver for the SH Serial Communication Interface
//              Clients of this file can configure the behavior with:
//              CYG_SCI_PORT:   serial port to use (0 or 1)
//              CYG_SCI_PUBLIC: define to export init, get, and put functions.
//              CYG_SCI_STUB:   define to get extra stub functions
//              CYG_SCI_NAME_PREFIX: Function prefix. Default is hal_sci_
//                                   Using different prefixes can be required
//                                   if both serial ports are used from the
//                                   same source file.
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/sh_regs.h>            // serial register definitions

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/sh_stub.h>            // target_register_t
#endif

// Unless explicitly asked to export functions, keep them local.
#ifndef CYG_SCI_PUBLIC
#define CYG_SCI_PUBLIC static
#endif

// Function names are prefix with hal_sci_ unless something else is specified.
#ifndef CYG_SCI_NAME_PREFIX
#define CYG_SCI_NAME_PREFIX hal_sci_
#endif

//-----------------------------------------------------------------------------
// Name Conversion magic.
#define Str(x, y) x ## y
#define Xstr(x, y) Str(x, y)
#define NC(_name)  Xstr(CYG_SCI_NAME_PREFIX, _name)

//-----------------------------------------------------------------------------
// The exported functions.
// Base:
CYG_SCI_PUBLIC void NC(init_serial)( void );
CYG_SCI_PUBLIC int  NC(get_char)( void );
CYG_SCI_PUBLIC void NC(put_char)( int c );
// GDB BREAK support:
#if defined(CYG_SCI_STUB) && defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
int  NC(interruptible)( int state );
void NC(init_break_irq)( void );
int  cyg_hal_gdb_isr( target_register_t pc ); // called from vectors.S
#endif

//-----------------------------------------------------------------------------
// The minimal init, get and put functions. All by polling.

CYG_SCI_PUBLIC void NC(init_serial)( void )
{
    // Disable interrupts.
    // Note: This affects out all SCI sources, not only RXI.
    HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_SCI_RXI);

    // Disable Tx/Rx interrupts, but enable Tx/Rx
    HAL_WRITE_UINT8(CYGARC_REG_SCSCR, CYGARC_REG_SCSCR_TE|CYGARC_REG_SCSCR_RE);

    // 8-1-no parity.
    HAL_WRITE_UINT8(CYGARC_REG_SCSMR, 0);

#ifndef CYG_HAL_STARTUP_RAM
    {
        // Set speed to 38400 - but only if configured for ROM; if
        // configured for RAM, we could be hosted by either the eCos
        // stub (running 38400, already initialized) or the Hitachi
        // ROM (running 9600 - reinitializing to 38400 would cause GDB
        // to lose the connection).
        cyg_uint8 tmp;
        HAL_READ_UINT8(CYGARC_REG_SCSMR, tmp);
        tmp &= ~CYGARC_REG_SCSMR_CKSx_MASK;
        tmp |= CYGARC_REG_CKSx_38400;
        HAL_WRITE_UINT8(CYGARC_REG_SCSMR, tmp);
        HAL_WRITE_UINT8(CYGARC_REG_SCBRR, CYGARC_REG_SCBRR_38400);
    }
#endif
}

CYG_SCI_PUBLIC int NC(get_char)( void )
{
    cyg_uint8 c, sr;

    do {
        HAL_READ_UINT8(CYGARC_REG_SCSSR, sr);
    } while ((sr & CYGARC_REG_SCSSR_RDRF) == 0);

    HAL_READ_UINT8(CYGARC_REG_SCRDR, c);

    // Clear buffer full flag.
    HAL_WRITE_UINT8(CYGARC_REG_SCSSR, sr & ~CYGARC_REG_SCSSR_RDRF);

    return c;
}

CYG_SCI_PUBLIC void NC(put_char)( int c )
{
    cyg_uint8 sr;

    do {
        HAL_READ_UINT8(CYGARC_REG_SCSSR, sr);
    } while ((sr & CYGARC_REG_SCSSR_TDRE) == 0);

    HAL_WRITE_UINT8(CYGARC_REG_SCTDR, c);

    // Clear empty flag.
    HAL_WRITE_UINT8(CYGARC_REG_SCSSR, sr & ~CYGARC_REG_SCSSR_TDRE);

    // Hang around until the character has been safely sent.
    do {
        HAL_READ_UINT8(CYGARC_REG_SCSSR, sr);
    } while ((sr & CYGARC_REG_SCSSR_TDRE) == 0);
}


//-----------------------------------------------------------------------------
// Additional functions required to provide GDB BREAK support.

#if defined(CYG_SCI_STUB) && defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
int NC(interruptible)( int state )
{
    if (state) {
        // Note: This affects out all SCI sources, not only RXI.
        HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_SCI_RXI);
    } else {
        // Note: This affects out all SCI sources, not only RXI.
        HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_SCI_RXI);
    }

    return 0;
}

void NC(init_break_irq)( void )
{
    cyg_uint8 scr;

    hal_diag_led_off();

    NC(interruptible)(0);

    // Enable serial receive interrupts.

    // Enable Rx interrupts
    HAL_READ_UINT8(CYGARC_REG_SCSCR, scr);
    scr |= CYGARC_REG_SCSCR_RIE;
    HAL_WRITE_UINT8(CYGARC_REG_SCSCR, scr);
    HAL_ENABLE_INTERRUPTS();

    NC(interruptible)(1);
}

// This ISR is called from the interrupt handler.
int cyg_hal_gdb_isr( target_register_t pc )
{
    cyg_uint8 c, sr;

    HAL_READ_UINT8(CYGARC_REG_SCSSR, sr);
    if (sr & CYGARC_REG_SCSSR_RDRF) {
        HAL_READ_UINT8(CYGARC_REG_SCRDR, c);

        // Clear buffer full flag.
        HAL_WRITE_UINT8(CYGARC_REG_SCSSR, 
                        CYGARC_REG_SCSSR_CLEARMASK & ~CYGARC_REG_SCSSR_RDRF);

        if ( 3 == c )
        {
            // Ctrl-C: set a breakpoint at PC so GDB will display the
            // correct program context when stopping rather than the
            // interrupt handler.
            cyg_hal_gdb_interrupt( pc );
        }

        // Interrupt handled. Don't call ISR proper. At return
        // from the VSR, execution will stop at the breakpoint
        // just set.
        return 0;
    }

    // Not caused by GDB. Call ISR proper.
    return 1;
}

#endif

//-----------------------------------------------------------------------------
// Undefine all macros used in this file.
#undef CYG_SCI_PORT
#undef CYG_SCI_PUBLIC
#undef CYG_SCI_STUB
#undef CYG_SCI_NAME_PREFIX
#undef NC
#undef Str
#undef Xstr

//-----------------------------------------------------------------------------
// end of sh_sci.inl
#endif // CYGONCE_HAL_SH_SCI_INL
