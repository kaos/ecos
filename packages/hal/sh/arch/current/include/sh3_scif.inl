#ifndef CYGONCE_HAL_SH3_SCIF_H
#define CYGONCE_HAL_SH3_SCIF_H
//=============================================================================
//
//      sh3_scif.inl
//
//      Simple driver for the SH3 Serial Communication Interface with FIFO
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
// Date:        2000-03-30
// Description: Simple driver for the SH Serial Communication Interface
//              The driver can be used for either the SCIF or the IRDA
//              modules (the latter can act as the former).
//              Clients of this file can configure the behavior with:
//              CYG_SCIF_IRDA:   if used for IrDA instead of SCIF
//              CYG_SCIF_PUBLIC: define to export init, get, and put functions.
//              CYG_SCIF_STUB:   define to get extra stub functions
//              CYG_SCIF_NAME_PREFIX: Function prefix. Default is hal_sci_
//                                    Using different prefixes can be required
//                                    if both serial ports are used from the
//                                    same source file.
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
#ifndef CYG_SCIF_PUBLIC
#define CYG_SCIF_PUBLIC static
#endif

// Function names are prefix with hal_sci_ unless something else is specified.
#ifndef CYG_SCIF_NAME_PREFIX
#define CYG_SCIF_NAME_PREFIX hal_scif_
#endif

//-----------------------------------------------------------------------------
// Register base.
// Can be used to drive either the IrDA controller or the SCIF controller
#ifdef CYG_SCIF_IRDA
# define SC_BASE (CYGARC_REG_SCSMR1)
#else
# define SC_BASE (CYGARC_REG_SCSMR2)
#endif

#define _SCSMR  (SC_BASE+0)
#define _SCBRR  (SC_BASE+2)
#define _SCSCR  (SC_BASE+4)
#define _SCFTDR (SC_BASE+6)
#define _SCSSR  (SC_BASE+8)
#define _SCFRDR (SC_BASE+10)
#define _SCFCR  (SC_BASE+12)
#define _SCFDR  (SC_BASE+14)

//-----------------------------------------------------------------------------
// Name Conversion magic.
#define Str(x, y) x ## y
#define Xstr(x, y) Str(x, y)
#define NC(_name)  Xstr(CYG_SCIF_NAME_PREFIX, _name)

//-----------------------------------------------------------------------------
// The exported functions.
// Base:
CYG_SCIF_PUBLIC void NC(init_serial)( void );
CYG_SCIF_PUBLIC int  NC(get_char)( void );
CYG_SCIF_PUBLIC void NC(put_char)( int c );
// GDB BREAK support:
#if defined(CYG_SCIF_STUB) && defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
int  NC(interruptible)( int state );
void NC(init_break_irq)( void );
int  cyg_hal_gdb_isr( target_register_t pc ); // called from vectors.S
#endif

//-----------------------------------------------------------------------------
// The minimal init, get and put functions. All by polling.

CYG_SCIF_PUBLIC void NC(init_serial)( void )
{
    // Disable interrupts.
#ifdef CYG_SCIF_IRDA
    // Note: This affects out all IRDA sources, not only RXI1.
    HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_IRDA_RXI1);
#else
    // Note: This affects out all SCIF sources, not only RXI2.
    HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_SCIF_RXI2);
#endif

    // Disable everything.
    HAL_WRITE_UINT8(_SCSCR, 0);

    // Reset FIFO.
    HAL_WRITE_UINT8(_SCFCR, CYGARC_REG_SCFCR2_TFRST|CYGARC_REG_SCFCR2_RFRST);

#ifdef CYG_SCIF_IRDA
    // 8-1-no parity, clear IRMOD.
    HAL_WRITE_UINT8(_SCSMR, 0);
#else
    // 8-1-no parity.
    HAL_WRITE_UINT8(_SCSMR, 0);
#endif

#ifndef CYG_HAL_STARTUP_RAM
    {
        // Set speed to 38400 - but only if configured for ROM; if
        // configured for RAM, we could be hosted by either the eCos
        // stub (running 38400, already initialized) or the Hitachi
        // ROM (running 9600 - reinitializing to 38400 would cause GDB
        // to lose the connection).
        cyg_uint8 tmp;
        HAL_READ_UINT8(_SCSMR, tmp);
        tmp &= ~CYGARC_REG_SCSMR2_CKSx_MASK;
        tmp |= CYGARC_SCBRR2_CKSx(38400);
        HAL_WRITE_UINT8(_SCSMR, tmp);
        HAL_WRITE_UINT8(_SCBRR, CYGARC_SCBRR2_N(38400));
    }
#endif

    // Let things settle: Here we should should wait the equivalent of
    // one bit interval, i.e. 1/38400 second, but until we have
    // something like the Linux jiffies, it's hard to do reliably. So
    // just move on and hope for the best (this is unlikely to cause
    // problems since the CPU has just come out of reset anyway).

    // Bring FIFO out of reset and set to trigger on every char in
    // FIFO (or C-c input would not be processed).
    HAL_WRITE_UINT8(_SCFCR, CYGARC_REG_SCFCR2_RTRG_1|CYGARC_REG_SCFCR2_TTRG_1);

    // Leave Tx/Rx interrupts disabled, but enable Tx/Rx
    HAL_WRITE_UINT8(_SCSCR, CYGARC_REG_SCSCR2_TE|CYGARC_REG_SCSCR2_RE);
}

CYG_SCIF_PUBLIC int NC(get_char)( void )
{
    cyg_uint8 c;
    cyg_uint16 fdr;

    do {
        HAL_READ_UINT16(_SCFDR, fdr);
    } while ((fdr & CYGARC_REG_SCFDR2_RCOUNT_MASK) == 0);

    HAL_READ_UINT8(_SCFRDR, c);

    // Clear FIFO full flag.
    HAL_WRITE_UINT8(_SCSSR,
                    CYGARC_REG_SCSSR2_CLEARMASK & ~CYGARC_REG_SCSSR2_RDF);

    return c;
}

CYG_SCIF_PUBLIC void NC(put_char)( int c )
{
    cyg_uint16 fdr;

    do {
        HAL_READ_UINT16(_SCFDR, fdr);
    } while ((fdr & CYGARC_REG_SCFDR2_TCOUNT_MASK >> CYGARC_REG_SCFDR2_TCOUNT_shift) < 16);

    HAL_WRITE_UINT8(_SCFTDR, c);

    // Clear FIFO-empty/transmit end flags.
    HAL_WRITE_UINT8(_SCSSR, CYGARC_REG_SCSSR2_CLEARMASK   
                    & ~(CYGARC_REG_SCSSR2_TDFE | CYGARC_REG_SCSSR2_TEND ));

    // Hang around until the character has been safely sent.
    do {
        HAL_READ_UINT16(_SCFDR, fdr);
    } while ((fdr & CYGARC_REG_SCFDR2_TCOUNT_MASK) == 0);
}


//-----------------------------------------------------------------------------
// Additional functions required to provide GDB BREAK support.

#if defined(CYG_SCIF_STUB) && defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
int NC(interruptible)( int state )
{
    if (state) {
#ifdef CYG_SCIF_IRDA
        // Note: This affects out all IRDA sources, not only RXI1.
        HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_IRDA_RXI1);
#else
        // Note: This affects out all SCIF sources, not only RXI2.
        HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_SCIF_RXI2);
#endif
    } else {
#ifdef CYG_SCIF_IRDA
        // Note: This affects out all IRDA sources, not only RXI1.
        HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_IRDA_RXI1);
#else
        // Note: This affects out all SCIF sources, not only RXI2.
        HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_SCIF_RXI2);
#endif
    }

    return 0;
}

void NC(init_break_irq)( void )
{
    cyg_uint8 scr;

    NC(interruptible)(0);

    // Enable serial receive interrupts.

    // Enable Rx interrupts
    HAL_READ_UINT8(_SCSCR, scr);
    scr |= CYGARC_REG_SCSCR2_RIE;
    HAL_WRITE_UINT8(_SCSCR, scr);
    HAL_ENABLE_INTERRUPTS();

    NC(interruptible)(1);
}

// This ISR is called from the interrupt handler.
int cyg_hal_gdb_isr( target_register_t pc )
{
    cyg_uint8 c;
    cyg_uint16 fdr;

    HAL_READ_UINT16(_SCFDR, fdr);
    if ((fdr & CYGARC_REG_SCFDR2_RCOUNT_MASK) != 0) {
        HAL_READ_UINT8(_SCFRDR, c);

        // Clear buffer full flag.
        HAL_WRITE_UINT8(_SCSSR, 
                        CYGARC_REG_SCSSR2_CLEARMASK & ~CYGARC_REG_SCSSR2_RDF);

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
#undef CYG_SCIF_IRDA
#undef CYG_SCIF_PUBLIC
#undef CYG_SCIF_STUB
#undef CYG_SCIF_NAME_PREFIX
#undef NC
#undef Str
#undef Xstr

//-----------------------------------------------------------------------------
// end of sh3_scif.inl
#endif // CYGONCE_HAL_SH3_SCIF_INL
