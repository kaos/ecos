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
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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

//-----------------------------------------------------------------------------
// Serial definitions.
#define DIAG_BASE       0xfffff300
#define DIAG_SLCR       (DIAG_BASE+0x00)
#define DIAG_SLSR       (DIAG_BASE+0x04)
#define DIAG_SLDICR     (DIAG_BASE+0x08)
#define DIAG_SLDISR     (DIAG_BASE+0x0C)
#define DIAG_SFCR       (DIAG_BASE+0x10)
#define DIAG_SBRG       (DIAG_BASE+0x14)
#define DIAG_TFIFO      (DIAG_BASE+0x20)
#define DIAG_RFIFO      (DIAG_BASE+0x30)

#define BRG_T0          0x0000
#define BRG_T2          0x0100
#define BRG_T4          0x0200
#define BRG_T5          0x0300

//-----------------------------------------------------------------------------

// Initialize the current serial port.
void hal_jmr_init_serial( void )
{
//hal_diag_led(0x10);    
    HAL_WRITE_UINT16( DIAG_SLCR , 0x0020 );

    HAL_WRITE_UINT16( DIAG_SLDICR , 0x0000 );
    
    HAL_WRITE_UINT16( DIAG_SFCR , 0x0000 );
    
//    HAL_WRITE_UINT16( DIAG_SBRG , BRG_T2 | 20 );
//    HAL_WRITE_UINT16( DIAG_SBRG , BRG_T2 | 10 );
    HAL_WRITE_UINT16( DIAG_SBRG , BRG_T2 | 5 );    

//hal_diag_led(0x10);
}

// Write C to the current serial port.
void hal_jmr_put_char( int c )
{
    CYG_WORD16 disr;
    
//hal_diag_led(0x20);

    for(;;)
    {
        HAL_READ_UINT16( DIAG_SLDISR , disr );

        if( disr & 0x0002 ) break;
    }

    disr = disr & ~0x0002;
    
    HAL_WRITE_UINT8( DIAG_TFIFO, c );

    HAL_WRITE_UINT16( DIAG_SLDISR , disr );    

//hal_diag_led(0x20);
//    HAL_DIAG_WRITE_CHAR( c );
}

// Read one character from the current serial port.
int hal_jmr_get_char( void )
{
#ifdef CYG_HAL_MIPS_SIM
    // FIXME: ask nickg if this can go to /dev/null
    static char input[] = "+$s#73";
    static int i = 0;
    return input[i++];
#else
    char c;

    CYG_WORD16 disr;

//hal_diag_led(0x40);        
    for(;;)
    {
        
        HAL_READ_UINT16( DIAG_SLDISR , disr );

        if( disr & 0x0001 ) break;
    }

    disr = disr & ~0x0001;
    
    HAL_READ_UINT8( DIAG_RFIFO, c );
    
    HAL_WRITE_UINT16( DIAG_SLDISR , disr );    

//hal_diag_led(0x40);
    
//    HAL_DIAG_READ_CHAR(c);
//    diag_printf("<%02x:%c>",c);

    return c;
    
#endif    
}

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
