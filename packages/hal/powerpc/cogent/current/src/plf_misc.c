//=============================================================================
//
//      plf_misc.c
//
//      Platform miscellaneous code/data.
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
// Author(s):   hmt
// Contributors:hmt
// Date:        1999-06-08
// Purpose:     Platform specific code and data
// Description: Tables for per-platform initialization
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_mem.h>            // HAL memory definitions
#include <cyg/hal/hal_if.h>             // hal_if_init

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // interrupt vectors
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // HANDLED

// The memory map is weakly defined, allowing the application to redefine
// it if necessary. The regions defined below are the minimum requirements.
CYGARC_MEMDESC_TABLE CYGBLD_ATTRIB_WEAK = {
    // Mapping for the Cogent CMA101/102 boards.
    CYGARC_MEMDESC_NOCACHE( 0xfff00000, 0x00100000 ), // ROM region
    CYGARC_MEMDESC_NOCACHE( 0xff000000, 0x00100000 ), // MCP registers
    CYGARC_MEMDESC_NOCACHE( 0x0e000000, 0x01000000 ), // IO registers
    CYGARC_MEMDESC_CACHE(   0x00000000, 0x00800000 ), // Main memory

    CYGARC_MEMDESC_TABLE_END
};

void
hal_platform_init(void)
{
    hal_if_init();
}


//--------------------------------------------------------------------------
// Control C ISR support

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)

#if (1 == CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL)
#define CYG_DEVICE_SERIAL_RS232_16550_BASE CYG_DEVICE_SERIAL_RS232_16550_BASE_B
#else
#define CYG_DEVICE_SERIAL_RS232_16550_BASE CYG_DEVICE_SERIAL_RS232_16550_BASE_A
#endif

//-----------------------------------------------------------------------------
// There are two serial ports.
#define CYG_DEVICE_SERIAL_RS232_16550_BASE_A    0xe900047 // port A
#define CYG_DEVICE_SERIAL_RS232_16550_BASE_B    0xe900007 // port B

// receiver buffer register, read, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_RBR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x00)
// interrupt enable register, read/write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_IER \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x08)
// interrupt identification register, read, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_IIR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x10)
// line status register, read
#define CYG_DEVICE_SERIAL_RS232_16550_LSR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x28)


// The interrupt enable register bits.
#define SIO_IER_ERDAI   0x01            // enable received data available irq
#define SIO_IER_ETHREI  0x02            // enable THR empty interrupt
#define SIO_IER_ELSI    0x04            // enable receiver line status irq
#define SIO_IER_EMSI    0x08            // enable modem status interrupt

// The interrupt identification register bits.
#define SIO_IIR_IP      0x01            // 0 if interrupt pending
#define SIO_IIR_ID_MASK 0x0e            // mask for interrupt ID bits
#define ISR_Tx  0x02
#define ISR_Rx  0x04

// The line status register bits.
#define SIO_LSR_DR      0x01            // data ready
#define SIO_LSR_OE      0x02            // overrun error
#define SIO_LSR_PE      0x04            // parity error
#define SIO_LSR_FE      0x08            // framing error
#define SIO_LSR_BI      0x10            // break interrupt
#define SIO_LSR_THRE    0x20            // transmitter holding register empty
#define SIO_LSR_TEMT    0x40            // transmitter register empty
#define SIO_LSR_ERR     0x80            // any error condition

struct Hal_SavedRegisters *hal_saved_interrupt_state;

void hal_ctrlc_isr_init(void)
{
    // Enable serial receive interrupts.
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_IER, SIO_IER_ERDAI);

    HAL_INTERRUPT_SET_LEVEL(CYGHWR_HAL_GDB_PORT_VECTOR, 1);
    HAL_INTERRUPT_UNMASK(CYGHWR_HAL_GDB_PORT_VECTOR);
}

cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    cyg_uint8 _iir;


    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_IIR, _iir);
    _iir &= SIO_IIR_ID_MASK;

    if ( ISR_Rx == _iir ) {

        cyg_uint8 c, lsr;

        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
        if (lsr & SIO_LSR_DR) {

            HAL_READ_UINT8( CYG_DEVICE_SERIAL_RS232_16550_RBR, c );

            if( cyg_hal_is_break( &c , 1 ) )
                cyg_hal_user_break( (CYG_ADDRWORD *)hal_saved_interrupt_state );
        }

        // Acknowledge the interrupt
        HAL_INTERRUPT_ACKNOWLEDGE(CYGHWR_HAL_GDB_PORT_VECTOR);

        return CYG_ISR_HANDLED;
    }

    return 0;
}

#endif

// EOF plf_misc.c
