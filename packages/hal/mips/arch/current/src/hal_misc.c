//==========================================================================
//
//      hal_misc.c
//
//      HAL miscellaneous functions
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
// Author(s):    nickg
// Contributors: nickg, jlarmour
// Date:         1999-01-21
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling

/*------------------------------------------------------------------------*/
/* If required, define a variable to store the clock period.              */

#ifdef CYGHWR_HAL_CLOCK_PERIOD_DEFINED

CYG_WORD32 cyg_hal_clock_period;

#endif

/*------------------------------------------------------------------------*/
/* First level C exception handler.                                       */

externC void __handle_exception (void);

externC HAL_SavedRegisters *_hal_registers;

externC void exception_handler(HAL_SavedRegisters *regs)
{
#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

    // Set the pointer to the registers of the current exception
    // context. At entry the GDB stub will expand the
    // HAL_SavedRegisters structure into a (bigger) register array.
    _hal_registers = regs;
    __handle_exception();

#elif defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && defined(CYGPKG_HAL_EXCEPTIONS)

    // We should decode the vector and pass a more appropriate
    // value as the second argument. For now we simply pass a
    // pointer to the saved registers. We should also divert
    // breakpoint and other debug vectors into the debug stubs.
    
    cyg_hal_deliver_exception( regs->vector>>2, (CYG_ADDRWORD)regs );

#else
    
    CYG_FAIL("Exception!!!");
    
#endif    
    return;
}

/*------------------------------------------------------------------------*/
/* default ISR                                                            */

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) &&      \
    defined(CYGHWR_HAL_GDB_PORT_VECTOR) &&              \
    defined(HAL_CTRLC_ISR)

#ifndef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN    
    if( vector == CYGHWR_HAL_GDB_PORT_VECTOR )
#endif        
    {
        cyg_uint32 result = HAL_CTRLC_ISR( vector, data );
        if( result != 0 ) return result;
    }
    
#endif
    
    CYG_TRACE1(true, "Interrupt: %d", vector);
    CYG_FAIL("Spurious Interrupt!!!");
    return 0;
}

/*------------------------------------------------------------------------*/
/* data copy and bss zero functions                                       */

typedef void (CYG_SYM_ADDRESS)(void);

// All these must use this type of address to stop them being given relocations
// relative to $gp (i.e. assuming they would be in .sdata)
extern CYG_SYM_ADDRESS __ram_data_start;
extern CYG_SYM_ADDRESS __ram_data_end;
extern CYG_SYM_ADDRESS __rom_data_start;    

#ifdef CYG_HAL_STARTUP_ROM      
void hal_copy_data(void)
{
    char *p = (char *)&__ram_data_start;
    char *q = (char *)&__rom_data_start;
    
    while( p != (char *)&__ram_data_end )
        *p++ = *q++;
}
#endif

extern CYG_SYM_ADDRESS __bss_start;
extern CYG_SYM_ADDRESS __bss_end;

void hal_zero_bss(void)
{
    char *p = (char *)&__bss_start;

    while( p != (char *)&__bss_end )
        *p++ = 0;   
}

/*------------------------------------------------------------------------*/

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

typedef void (*pfunc) (void);
extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];

void
cyg_hal_invoke_constructors(void)
{
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
    
} // cyg_hal_invoke_constructors()

/*------------------------------------------------------------------------*/
/* Determine the index of the ls bit of the supplied mask.                */

cyg_uint32 hal_lsbit_index(cyg_uint32 mask)
{
    cyg_uint32 n = mask;

    static const signed char tab[64] =
    { -1, 0, 1, 12, 2, 6, 0, 13, 3, 0, 7, 0, 0, 0, 0, 14, 10,
      4, 0, 0, 8, 0, 0, 25, 0, 0, 0, 0, 0, 21, 27 , 15, 31, 11,
      5, 0, 0, 0, 0, 0, 9, 0, 0, 24, 0, 0 , 20, 26, 30, 0, 0, 0,
      0, 23, 0, 19, 29, 0, 22, 18, 28, 17, 16, 0
    };

    n &= ~(n-1UL);
    n = (n<<16)-n;
    n = (n<<6)+n;
    n = (n<<4)+n;

    return tab[n>>26];
}

/*------------------------------------------------------------------------*/
/* Determine the index of the ms bit of the supplied mask.                */

cyg_uint32 hal_msbit_index(cyg_uint32 mask)
{
    cyg_uint32 x = mask;    
    cyg_uint32 w;

    /* Phase 1: make word with all ones from that one to the right */
    x |= x >> 16;
    x |= x >> 8;
    x |= x >> 4;
    x |= x >> 2;
    x |= x >> 1;

    /* Phase 2: calculate number of "1" bits in the word        */
    w = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    w = (w & 0x33333333) + ((w >> 2) & 0x33333333);
    w = w + (w >> 4);
    w = (w & 0x000F000F) + ((w >> 8) & 0x000F000F);
    return (cyg_uint32)((w + (w >> 16)) & 0xFF);

}

/*------------------------------------------------------------------------*/
/* Idle thread action                                                     */

#include <cyg/infra/diag.h>

void hal_idle_thread_action( cyg_uint32 count )
{
#if 0 //def CYG_HAL_MIPS_SIM
    if( (count % 1000) == 0 )
    {
        // This code causes a fake interrupt.
        asm volatile (
            "xor    $24,$24,$24;"
            "mtc0   $24,$13;"
            "lui    $25,%%hi(1f);"
            "ori    $25,$25,%%lo(1f);"
            "j      other_vector;"
            "nop;"
            "1:"
            :
            :
            : "t8", "t9"
            );
    }
#endif
#if 0 //def CYG_HAL_MIPS_JMR3904

    if( (count % 100000 ) == 0 )
    {
//        cyg_uint32 tval, isr, imr, ilr;
          cyg_uint32 sr = 0, cr = 0, ctr = 0, cpr = 0;
//        HAL_CLOCK_READ( &tval );
//        HAL_READ_UINT32( 0xFFFFC000, isr );
//        HAL_READ_UINT32( 0xFFFFC004, imr );
//        HAL_READ_UINT32( 0xFFFFC01C, ilr );
//        CYG_TRACE2(1, "Timer value, ISR ",tval, isr);
//        CYG_TRACE2(1, "IMR ILR0 ", imr, ilr);

//        asm volatile (
//            "mfc0  %0,$12;"
//            "nop; nop; nop;"
//            "mfc0  %1,$13;"
//            "nop; nop; nop;"
//            "mfc0  %2,$9;"
//            "nop; nop; nop;"
//            "mfc0  %3,$11;"
//            "nop; nop; nop;"
//            : "=r"(sr), "=r"(cr), "=r"(ctr), "=r"(cpr)
//            );

        
//        diag_printf("Status %08x ", sr );
//       diag_printf("Cause %08x ", cr );
//        diag_printf("Counter %08x ", ctr );
//        diag_printf("Compare %08x\n", cpr);

#if 0
        asm volatile (
            "mfc0  %0,$12;"
            "nop; nop; nop;"
            : "=r"(sr)
            );
        diag_write_string("Status "); diag_write_hex( sr );

        asm volatile (
            "mfc0  %0,$13;"
            "nop; nop; nop;"
            : "=r"(cr)
            );
        diag_write_string(" Cause "); diag_write_hex( cr );

        asm volatile (
            "mfc0  %0,$9;"
            "nop; nop; nop;"
            : "=r"(ctr)
            );
        diag_write_string(" Counter "); diag_write_hex( ctr );

        asm volatile (
            "mfc0  %0,$11;"
            "nop; nop; nop;"
            : "=r"(cpr)
            );
        diag_write_string(" Compare "); diag_write_hex( cpr );
        diag_write_string( "\n" );
        
#endif
#if 1         
        asm volatile (
            "mfc0  %0,$12;"
            "nop; nop; nop;"
            : "=r"(sr)
            );

        asm volatile (
            "mfc0  %0,$13;"
            "nop; nop; nop;"
            : "=r"(cr)
            );

        CYG_INSTRUMENT_USER( 1, sr, cr );

        asm volatile (
            "mfc0  %0,$9;"
            "nop; nop; nop;"
            : "=r"(ctr)
            );

        asm volatile (
            "mfc0  %0,$11;"
            "nop; nop; nop;"
            : "=r"(cpr)
            );
        
        CYG_INSTRUMENT_USER( 2, ctr, cpr );
#endif
        
//        if( count == 4 )
//        {
//            HAL_ENABLE_INTERRUPTS();
//        }
        
//        if( count >= 10 )
//            for(;;);
    }
#endif
}

/*------------------------------------------------------------------------*/
/* End of hal_misc.c                                                      */
