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
// Author(s):   jskov
// Contributors:nickg, jskov
// Date:        2000-05-10
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_diag.h>

#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_misc.h>           // Helper functions

#include <cyg/hal/hal_io.h>

#include <cyg/hal/hal_if.h>             // ROM calling interface

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#include <cyg/hal/hal_stub.h>           // CYG_HAL_GDB_x_CRITICAL_IO_REGION
#endif

externC void cyg_hal_plf_serial_init_channel(void* __ch_data);
externC void cyg_hal_plf_serial_putc(void* __ch_data, cyg_uint8 __ch);
externC cyg_uint8 cyg_hal_plf_serial_getc(void* __ch_data);

//-----------------------------------------------------------------------------
// Hit the hardware directly. Either wrap in GDB O-packets, or write
// data raw to device.

// Decide when to wrap text output as GDB O-packets
#if (CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL \
     == CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL)

#if defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs) \
    || defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

#define CYG_DIAG_GDB

#endif

#endif

//-----------------------------------------------------------------------------
// There are two serial ports. Hardwire to use one of them.
#define CYG_DEVICE_SERIAL_SCC1    0xb40003f8 // port 1
#define CYG_DEVICE_SERIAL_SCC2    0xb40002f8 // port 2

typedef struct {
    cyg_uint8* base;
    cyg_int32 msec_timeout;
    int isr_vector;
} channel_data_t;

#if (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL == 0)
#define BASE ((cyg_uint8*)CYG_DEVICE_SERIAL_SCC1)
#else
#define BASE ((cyg_uint8*)CYG_DEVICE_SERIAL_SCC2)
#endif

static channel_data_t channel = { BASE, 0, 0};

#ifdef CYG_DIAG_GDB

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
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION(old);
#else
        HAL_DISABLE_INTERRUPTS(old);
#endif
        
        while(1)
        {
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
            char c1;
        
            cyg_hal_plf_serial_putc(&channel, '$');
            cyg_hal_plf_serial_putc(&channel, 'O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                cyg_hal_plf_serial_putc(&channel, h);
                cyg_hal_plf_serial_putc(&channel, l);
                csum += h;
                csum += l;
            }
            cyg_hal_plf_serial_putc(&channel, '#');
            cyg_hal_plf_serial_putc(&channel, hex[(csum>>4)&0xF]);
            cyg_hal_plf_serial_putc(&channel, hex[csum&0xF]);

            c1 = cyg_hal_plf_serial_getc(&channel);

            if( c1 == '+' ) break;
        }
        
        pos = 0;
        // And re-enable interrupts
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION(old);
#else
        HAL_RESTORE_INTERRUPTS(old);
#endif
    }
}

void 
hal_diag_read_char(char *c)
{
    *c = cyg_hal_plf_serial_getc(&channel);
}

#else // CYG_DIAG_GDB

// Hit the hardware directly, no GDB translation

void 
hal_diag_read_char(char *c)
{
    *c = cyg_hal_plf_serial_getc(&channel);
}

void 
hal_diag_write_char(char c)
{
    cyg_hal_plf_serial_putc(&channel, c);
}


#endif

// Reagardless whether encoding or not we alway initialize the device.

void 
hal_diag_init(void)
{
    // Init serial device
    cyg_hal_plf_serial_init_channel((void*)&channel);
}

#endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

//-----------------------------------------------------------------------------
// End of hal_diag.c
