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
// Author(s):   nickg, jskov (based on the old tx39 hal_stub.c)
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
#include <cyg/hal/hal_diag.h>           // diag output. FIXME

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/plf_z8530.h>

/*---------------------------------------------------------------------------*/

#if CYGHWR_HAL_MIPS_VR4300_VRC4373_GDB_PORT == 0
#define DUART_CHAN      DUART_A
#else
#define DUART_CHAN      DUART_B
#endif

/*---------------------------------------------------------------------------*/

static unsigned char _diag_init[] = {
    0x00, /* Register 0 */
    0x00, /* Register 1 - no interrupts */
    0x00, /* Register 2 */
    0xC1, /* Register 3 - Rx enable, 8 data */
    0x44, /* Register 4 - x16 clock, 1 stop, no parity */
    0x68, /* Register 5 - Tx enable, 8 data */
    0x00, /* Register 6 */
    0x00, /* Register 7 */
    0x00, /* Register 8 */
    0x00, /* Register 9 */
    0x00, /* Register 10 */
    0x56, /* Register 11 - Rx, Tx clocks from baud rate generator */
    0x00, /* Register 12 - baud rate LSB */
    0x00, /* Register 13 - baud rate MSB */
    0x03, /* Register 14 - enable baud rate generator */
    0x00  /* Register 15 */
};

#define BRTC(brate) (( ((unsigned) DUART_CLOCK) / (2*(brate)*SCC_CLKMODE_TC)) - 2)
#define DUART_CLOCK          4915200         /* Z8530 duart */
#define SCC_CLKMODE_TC       16              /* Always run x16 clock for async modes */

void hal_plf_init_serial()
{
    unsigned short brg = BRTC(CYGHWR_HAL_MIPS_VR4300_VRC4373_DIAG_BAUD);
    int i;

    _diag_init[12] = brg & 0xFF;
    _diag_init[13] = brg >> 8;
    for (i = 1;  i < 16;  i++) {
        HAL_DUART_WRITE_CR(DUART_CHAN, i, _diag_init[i]);
    }    
}

/*---------------------------------------------------------------------------*/

void hal_plf_put_char( int c)
{
    cyg_uint8 rr0;
    do
    {
        HAL_DUART_READ_CR(DUART_CHAN, 0, rr0 );
    } while( (rr0 & 0x04) == 0 );

    HAL_DUART_WRITE_TR( DUART_CHAN, c );

    HAL_INTERRUPT_ACKNOWLEDGE( CYGNUM_HAL_INTERRUPT_DUART );    
}

/*---------------------------------------------------------------------------*/

int hal_plf_get_char( void )
{
    char c;
    cyg_uint8 rr0;

    do
    {
        HAL_DUART_READ_CR(DUART_CHAN, 0, rr0 );
    } while( (rr0 & 0x01) == 0 );

    HAL_DUART_READ_RR( DUART_CHAN, c );

    HAL_INTERRUPT_ACKNOWLEDGE( CYGNUM_HAL_INTERRUPT_DUART );
    
    return c;
}

//-----------------------------------------------------------------------------
// Stub init

void hal_plf_stub_init(void)
{
    extern CYG_ADDRESS hal_virtual_vector_table[64];
    extern void init_thread_syscall( void *);
    extern void install_async_breakpoint(void *epc);
    void (*oldvsr)(void);
    extern void __default_exception_vsr(void);

    // Ensure that the breakpoint VSR points to the default VSR. This will pass
    // it on to the stubs.
    HAL_VSR_SET( CYGNUM_HAL_VECTOR_BREAKPOINT, __default_exception_vsr, &oldvsr );

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
