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

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#include <cyg/hal/hal_stub.h>           // hal_output_gdb_string
#endif

#include <cyg/infra/cyg_type.h>         // base types, externC
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // Interrupt macros

#include <cyg/hal/sh_sci.inl>

void hal_diag_init(void)
{
    hal_sci_init_serial();
}

void 
hal_diag_write_char_serial( char c )
{
    hal_sci_put_char(c);
}

void
hal_diag_read_char(char *c)
{
    *c = (char) hal_sci_get_char();
}

externC cyg_bool cyg_hal_is_break(char *buf, int size);
externC void cyg_hal_user_break(CYG_ADDRWORD *regs);

// Packet function
void 
hal_diag_write_char(char c)
{

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
        
        HAL_DISABLE_INTERRUPTS(old);
        
        while(1)
        {
            char c1;
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
        
            hal_diag_write_char_serial('$');
            hal_diag_write_char_serial('O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                hal_diag_write_char_serial(h);
                hal_diag_write_char_serial(l);
                csum += h;
                csum += l;
            }
            hal_diag_write_char_serial('#');
            hal_diag_write_char_serial(hex[(csum>>4)&0xF]);
            hal_diag_write_char_serial(hex[csum&0xF]);

            // Wait for the ACK character '+' from GDB here and handle
            // receiving a ^C instead.
            hal_diag_read_char(&c1);

            if( c1 == '+' )
                break;              // a good acknowledge

            // Check for user break.
            if( cyg_hal_is_break( &c1, 1 ) )
                cyg_hal_user_break( NULL );

            // otherwise, loop round again
        }
        
        pos = 0;

        // And re-enable interrupts
        HAL_RESTORE_INTERRUPTS(old);
        
    }
}

//-----------------------------------------------------------------------------
// Led control
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

//-----------------------------------------------------------------------------
// End of hal_diag.c
