/*=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1998-03-02
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_diag.h>

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/hal_io.h>

#include <cyg/hal/plf_z8530.h>

/*---------------------------------------------------------------------------*/

#if defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs)

#define CYG_KERNEL_DIAG_GDB

#endif

#if CYGHWR_HAL_MIPS_VR4300_VRC4373_DIAG_PORT == 0
#define DUART_CHAN      DUART_A
#else
#define DUART_CHAN      DUART_B
#endif

/*---------------------------------------------------------------------------*/

extern int pmon_outbyte(char c);
extern int pmon_write( int fd, char *buf, int size);
extern int pmon_open( char *name, int mode, int perms);
extern int pmon_close( int fd );
extern int pmon_ioctl( int fd, int op, int *argp );
int fd;

static unsigned char _diag_init[] = {
    0x00, /* Register 0 */
#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT)
    0x10, /* Register 1 - rx interrupts only */
#else    
    0x00, /* Register 1 - no interrupts */
#endif    
    0x00, /* Register 2 */
    0xC1, /* Register 3 - Rx enable, 8 data */
    0x44, /* Register 4 - x16 clock, 1 stop, no parity */
    0x68, /* Register 5 - Tx enable, 8 data */
    0x00, /* Register 6 */
    0x00, /* Register 7 */
    0x00, /* Register 8 */
#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT)
    0x0a, /* Register 9 */
#else    
    0x00, /* Register 9 */
#endif    
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

void hal_duart_init(void)
{
    unsigned short brg = BRTC(CYGHWR_HAL_MIPS_VR4300_VRC4373_DIAG_BAUD);
    int i;
#ifdef CYGSEM_HAL_USE_ROM_MONITOR_PMON
    // We close down all of PMON's IO streams. This prevents it
    // printing out some silly error messages that are caused by
    // us changing the value of the counter/compare registers. By
    // default they came out of the debug serial line, disrupting
    // GDB session. 
    pmon_close(0);
    pmon_close(1);
    pmon_close(2);
//    fd = pmon_open( "/dev/tty1", 0 , 0);
//    fd = pmon_open( "/dev/tty1", 0 , 0);
//    fd = pmon_open( "/dev/tty1", 0 , 0);
#endif
    _diag_init[12] = brg & 0xFF;
    _diag_init[13] = brg >> 8;
    for (i = 1;  i < 16;  i++) {
        HAL_DUART_WRITE_CR(DUART_CHAN, i, _diag_init[i]);
    }
}

/*---------------------------------------------------------------------------*/

void hal_duart_write_char(char c)
{
    cyg_uint8 rr0;
    do
    {
        HAL_DUART_READ_CR(DUART_CHAN, 0, rr0 );
    } while( (rr0 & 0x04) == 0 );

    HAL_DUART_WRITE_TR( DUART_CHAN, c );

#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT) && \
    defined(CYGHWR_HAL_GDB_PORT_VECTOR)
    // Ensure that this write does not provoke a spurious interrupt.
    HAL_INTERRUPT_ACKNOWLEDGE( CYGHWR_HAL_GDB_PORT_VECTOR );
#endif    

}

/*---------------------------------------------------------------------------*/

void hal_duart_read_char(char *c)
{
    cyg_uint8 rr0;
    do
    {
        HAL_DUART_READ_CR(DUART_CHAN, 0, rr0 );
    } while( (rr0 & 0x01) == 0 );

    HAL_DUART_READ_RR( DUART_CHAN, *c );

#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT) && \
    defined(CYGHWR_HAL_GDB_PORT_VECTOR)
    // Ensure that this read does not provoke a spurious interrupt.
    HAL_INTERRUPT_ACKNOWLEDGE( CYGHWR_HAL_GDB_PORT_VECTOR );
#endif    

}

/*---------------------------------------------------------------------------*/

void hal_diag_init()
{
    hal_duart_init();
}

/*---------------------------------------------------------------------------*/

extern cyg_bool cyg_hal_is_break(char *buf, int size);
extern void cyg_hal_user_break(CYG_ADDRWORD *regs);

void hal_diag_write_char(char c)
{
#ifdef CYG_KERNEL_DIAG_GDB    
    static char line[100];
    static int pos = 0;

    // No need to send CRs
    if( c == '\r' ) return;

    line[pos++] = c;

    if( c == '\n' || pos == sizeof(line) )
    {

        // Disable interrupts. This prevents GDB trying to interrupt us
        // while we are in the middle of sending a packet. The serial
        // receive interrupt will be seen when we re-enable interrupts
        // later.
        CYG_INTERRUPT_STATE oldstate;
        HAL_DISABLE_INTERRUPTS(oldstate);
        
        while(1)
        {
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
            char c1;
        
            hal_duart_write_char('$');
            hal_duart_write_char('O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                hal_duart_write_char(h);
                hal_duart_write_char(l);
                csum += h;
                csum += l;
            }
            hal_duart_write_char('#');
            hal_duart_write_char(hex[(csum>>4)&0xF]);
            hal_duart_write_char(hex[csum&0xF]);

            hal_duart_read_char( &c1 );

            if( c1 == '+' ) break;

            if( cyg_hal_is_break( &c1 , 1 ) )
                cyg_hal_user_break( NULL );    

        }
        
        pos = 0;

        // Wait for all data from serial line to drain
        // and clear ready-to-send indication.
//        hal_diag_drain_serial0();

        // Disabling the interrupts for an extended period of time
        // can provoke a spurious interrupt 0. FIXME - why?
#ifdef CYGSEM_HAL_MIPS_VR4300_VRC4373_DIAG_ACKS_INT_0 
        HAL_INTERRUPT_ACKNOWLEDGE( CYGNUM_HAL_INTERRUPT_VRC4373 );
#endif
        
        // And re-enable interrupts
        HAL_RESTORE_INTERRUPTS( oldstate );
        
    }
#else
    hal_duart_write_char(c);
#endif    
}

/*---------------------------------------------------------------------------*/

void hal_diag_read_char(char *c)
{
    hal_duart_read_char(c);
}

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */
