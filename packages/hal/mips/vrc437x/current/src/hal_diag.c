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

#include <cyg/hal/plf_stub.h>


#if defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs)

#define CYG_KERNEL_DIAG_GDB

#endif

/*---------------------------------------------------------------------------*/

void hal_diag_init()
{
    cyg_hal_plf_comms_init();
}

/*---------------------------------------------------------------------------*/

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
        
            cyg_hal_plf_serial_putc(NULL, '$');
            cyg_hal_plf_serial_putc(NULL, 'O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                cyg_hal_plf_serial_putc(NULL, h);
                cyg_hal_plf_serial_putc(NULL, l);
                csum += h;
                csum += l;
            }
            cyg_hal_plf_serial_putc(NULL, '#');
            cyg_hal_plf_serial_putc(NULL, hex[(csum>>4)&0xF]);
            cyg_hal_plf_serial_putc(NULL, hex[csum&0xF]);

            c1 = cyg_hal_plf_serial_getc( NULL );

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
#ifdef CYGSEM_HAL_MIPS_VR4300_VRC437X_DIAG_ACKS_INT_0
        HAL_INTERRUPT_ACKNOWLEDGE( CYGNUM_HAL_INTERRUPT_VRC437X );
#endif
        
        // And re-enable interrupts
        HAL_RESTORE_INTERRUPTS( oldstate );
        
    }
#else
    cyg_hal_plf_serial_putc(NULL, c);
#endif    
}

/*---------------------------------------------------------------------------*/

void hal_diag_read_char(char *c)
{
    *c = cyg_hal_plf_serial_getc(NULL);
}

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */
