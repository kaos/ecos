//=============================================================================
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
// Author(s):    nickg, jskov
// Contributors: nickg, jskov
// Date:         1999-03-23
// Purpose:      HAL diagnostic output
// Description:  Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_diag.h>           // our header.

#include <cyg/infra/cyg_type.h>         // base types, externC
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // Calling interface
#include <cyg/hal/hal_intr.h>           // Interrupt macros
#include <cyg/hal/sh3_sci.h>            // driver API
#include <cyg/hal/hal_misc.h>           // Helper functions

#define SCI_BASE ((cyg_uint8*)0xfffffe80)

//-----------------------------------------------------------------------------


void
cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;

    if (initialized)
        return;

    initialized = 1;

    cyg_hal_plf_sci_init(0, 0, CYGNUM_HAL_INTERRUPT_SCI_RXI, SCI_BASE);
}

//=============================================================================
// Led control
//=============================================================================
#define LED_ON   0x04000000
#define LED_OFF  0x18000000

void
hal_diag_led_on( void )
{
    HAL_WRITE_UINT8(LED_ON, 0);
}

void
hal_diag_led_off( void )
{
    HAL_WRITE_UINT8(LED_OFF, 0);
}


//=============================================================================
// Compatibility with older stubs
//=============================================================================

#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

#define CYGPRI_HAL_SH_SH3_SCI_PRIVATE
#include <cyg/hal/sh3_sci.h>            // driver API

static channel_data_t channel = { (cyg_uint8*)SCI_BASE, 0, 0};

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#include <cyg/hal/hal_stub.h>           // hal_output_gdb_string
#endif

#if defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs)

#define CYG_HAL_DIAG_GDB

#elif defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

#define CYG_HAL_DIAG_GDB

#endif

//-----------------------------------------------------------------------------

void hal_diag_init(void)
{
    cyg_hal_plf_sci_init_channel(&channel);
}

void 
hal_diag_write_char_serial( char c )
{
    cyg_hal_plf_sci_putc(&channel, c);
}

void
hal_diag_read_char(char *c)
{
    *c = (char) cyg_hal_plf_sci_getc(&channel);
}

// Packet function
void 
hal_diag_write_char(char c)
{
#ifdef CYG_HAL_DIAG_GDB
    static char line[100];
    static int pos = 0;

    // No need to send CRs
    if( c == '\r' ) return;

    line[pos++] = c;

    if( c == '\n' || pos == sizeof(line) )
    {
        CYG_INTERRUPT_STATE old;

        // Disable interrupts. This prevents GDB trying to interrupt us
        // while we are in the middle of sending a packet. The serial
        // receive interrupt will be seen when we re-enable interrupts
        // later.
        
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION(old);
#else
        HAL_DISABLE_INTERRUPTS(old);
#endif
        
        while(1)
        {
            char c1;
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
        
            cyg_hal_plf_sci_putc(&channel, '$');
            cyg_hal_plf_sci_putc(&channel, 'O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                cyg_hal_plf_sci_putc(&channel, h);
                cyg_hal_plf_sci_putc(&channel, l);
                csum += h;
                csum += l;
            }
            cyg_hal_plf_sci_putc(&channel, '#');
            cyg_hal_plf_sci_putc(&channel, hex[(csum>>4)&0xF]);
            cyg_hal_plf_sci_putc(&channel, hex[csum&0xF]);

            // Wait for the ACK character '+' from GDB here and handle
            // receiving a ^C instead.
            c1 = (char) cyg_hal_plf_sci_getc(&channel);

            if( c1 == '+' )
                break;              // a good acknowledge

            // Check for user break.
            if( cyg_hal_is_break( &c1, 1 ) )
                cyg_hal_user_break( NULL );

            // otherwise, loop round again
        }
        
        pos = 0;

        // And re-enable interrupts
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION(old);
#else
        HAL_RESTORE_INTERRUPTS(old);
#endif
    }
#else // CYG_HAL_DIAG_GDB
    cyg_hal_plf_sci_putc(&channel, c);
#endif
}
#endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

//-----------------------------------------------------------------------------
// End of hal_diag.c
