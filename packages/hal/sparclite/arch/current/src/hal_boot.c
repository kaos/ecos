//==========================================================================
//
//      hal_boot.c
//
//      SPARClite Architecture specific interrupt dispatch tables
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt
// Contributors: hmt
// Date:         1998-12-10
// Purpose:      Interrupt handler tables for SPARClite.
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

/*------------------------------------------------------------------------*/
/* calling this is our raison d'etre: */
extern void cyg_start( void );

/*------------------------------------------------------------------------*/
/* data copy and bss zero functions                                       */

typedef void (CYG_ROM_ADDRESS)(void);

#ifdef CYG_HAL_STARTUP_ROM      
void hal_copy_data(void)
{
    extern char __ram_data_start;
    extern char __ram_data_end;
    extern CYG_ROM_ADDRESS __rom_data_start;    
    long *p = (long *)&__ram_data_start;
    long *q = (long *)&__rom_data_start;
    
    while( p <= (long *)&__ram_data_end )
        *p++ = *q++;
}
#endif

void hal_zero_bss(void)
{
    extern CYG_ROM_ADDRESS __bss_start;
    extern CYG_ROM_ADDRESS __bss_end;

    register long long zero = 0;
    register long long *end = (long long *)&__bss_end;
    register long long *p = (long long *)&__bss_start;

    while( p <= end )
        *p++ = zero;   
}

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

void
cyg_hal_invoke_constructors (void)
{
    typedef void (*pfunc) (void);
    extern pfunc __CTOR_LIST__[];
    extern pfunc __CTOR_END__[];

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    static pfunc *p = &__CTOR_END__[-1];
    
    cyg_hal_stop_constructors = 0;
    for (; p >= __CTOR_LIST__; p--) {
        (*p) ();
        if (cyg_hal_stop_constructors) {
            p--;
            break;
        }
    }
#else
    pfunc *p;

    for (p = &__CTOR_END__[-1]; p >= __CTOR_LIST__; p--)
        (*p) ();
#endif
}

// Override any __gccmain the compiler might generate. We don't want
// constructors to be called twice.
void  __gccmain(void) {}

/*------------------------------------------------------------------------*/
/*   CYG_HAL_START - pre-main-entrypoint                                  */

#ifdef CYG_HAL_SPARCLITE_SLEB
#define SLEB_LED (*(volatile char *)(0x02000003))
#define LED( _x_ ) SLEB_LED = (char)(0xff & ~(_x_))
#else
#define LED( _x_ ) CYG_EMPTY_STATEMENT
#endif

extern void hal_board_prestart( void );
extern void hal_board_poststart( void );

// This is called with traps enabled, but interrupts masked out:
// Be sure to enable them in hal_board_poststart() at the latest.

void cyg_hal_start( void )
{
    /* Board specific prestart that's best done in C */
    hal_board_prestart();

    LED( 0xd0 );

#ifdef CYG_HAL_STARTUP_ROM
    /* Copy data from ROM to RAM */
    hal_copy_data();
#endif
                
    LED( 0xd4 );

    /* Zero BSS */
    hal_zero_bss();

    LED( 0xd8 );

    /* Call constructors */
    cyg_hal_invoke_constructors();

    LED( 0xdc );

    /* Board specific late startup that's best done in C */
    hal_board_poststart();

    LED( 0xf8 );

    /* Call cyg_start */
    cyg_start(); /* does not return */
}

// EOF hal_boot.c
