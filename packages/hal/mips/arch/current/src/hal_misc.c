//=============================================================================
//
//	hal_misc.c
//
//	HAL miscellaneous functions
//
//=============================================================================
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-02-05
// Purpose:	HAL miscellaneous functions
// Description: This file contains miscellaneous functions provided by the
//              HAL.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling

/*---------------------------------------------------------------------------*/
/* First level C exception handler.                                          */

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
    
    deliver_exception( regs->vector>>2, (CYG_ADDRWORD)regs );

#else
    
    CYG_FAIL("Exception!!!");
    
#endif    
    return;
}

/*---------------------------------------------------------------------------*/
/* default ISR                                                               */

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_TRACE1(true, "Interrupt: %d", vector);
    CYG_FAIL("Spurious Interrupt!!!");
    return 0;
}

/*---------------------------------------------------------------------------*/
/* data copy and bss zero functions                                          */

typedef void (CYG_ROM_ADDRESS)(void);

#ifdef CYG_HAL_STARTUP_ROM	
void hal_copy_data(void)
{
    extern char __ram_data_start;
    extern char __ram_data_end;
    extern CYG_ROM_ADDRESS __rom_data_start;    
    char *p = &__ram_data_start;
    char *q = (char *)&__rom_data_start;
    
    while( p != (char *)&__ram_data_end )
        *p++ = *q++;
}
#endif

void hal_zero_bss(void)
{
    extern CYG_ROM_ADDRESS __bss_start;
    extern CYG_ROM_ADDRESS _end;
    char *p = (char *)&__bss_start;

    while( p != (char *)&_end )
        *p++ = 0;   
}

/*---------------------------------------------------------------------------*/

void fake_ctor(void) {};

void
cyg_hal_invoke_constructors (void)
{
#ifdef CYG_KERNEL_USE_INIT_PRIORITY

    typedef void (*pfunc) (void);
    extern pfunc __CTOR_LIST__[];
    long i = (long)(__CTOR_LIST__[0]);
    pfunc p;
    
    for ( ; i > 0; i-- )
    {
        p = __CTOR_LIST__[i];
        p ();
    }

#else

    typedef void (CYG_CODE_ADDRESS)();
    extern CYG_CODE_ADDRESS __sched_text_start;
    extern CYG_CODE_ADDRESS __sched_text_end;
    extern CYG_CODE_ADDRESS __clock_text_start;
    extern CYG_CODE_ADDRESS __clock_text_end;
    extern CYG_CODE_ADDRESS __thread_text_start;
    extern CYG_CODE_ADDRESS __thread_text_end;
    
    typedef void (*pfunc) ();
    extern pfunc __CTOR_LIST__[];
    pfunc *p;
    
    for (p = __CTOR_LIST__+1; *p != 0; p++)
    {
        CYG_ADDRESS *pp = (CYG_ADDRESS *)*p;
        
        if( pp > (CYG_ADDRESS *)&__sched_text_start &&
            pp < (CYG_ADDRESS *)&__sched_text_end )
        {
            (*p) ();
            *p = fake_ctor;
        }
    }
    
    for (p = __CTOR_LIST__+1; *p != 0; p++)
    {
        CYG_ADDRESS *pp = (CYG_ADDRESS *)*p;
        
        if( pp > (CYG_ADDRESS *)&__clock_text_start &&
            pp < (CYG_ADDRESS *)&__clock_text_end )
        {
            (*p) ();
            *p = fake_ctor;
        }
    }
    
    for (p = __CTOR_LIST__+1; *p != 0; p++)
    {
        CYG_ADDRESS *pp = (CYG_ADDRESS *)*p;
        
        if( pp > (CYG_ADDRESS *)&__thread_text_start &&
            pp < (CYG_ADDRESS *)&__thread_text_end )
        {
            (*p) ();
            *p = fake_ctor;
        }
    }
    
    for (p = __CTOR_LIST__+1; *p != 0; p++)
    {            
        (*p) ();
        *p = fake_ctor;
    }

#endif

#ifdef CYG_HAL_MIPS_TX3904

    // On the real hardware we also enable the cache.
    // doing this here is a temporary measure until we
    // have a proper platform specific place to do it.
    
    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();

#endif
    
#if defined(CYG_HAL_USE_ROM_MONITOR) && \
    !defined(CYGDBG_INFRA_DIAG_USE_DEVICE)    && \
        defined(CYG_HAL_TX39_JMR3904)

        {
            static void hal_init_ctrlc_intr(void);
            hal_init_ctrlc_intr();
        }

#endif
#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)   && \
    defined(CYG_HAL_USE_ROM_MONITOR)            && \
    defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
    {
        extern void patch_dbg_syscalls(void * vector);
#define DBG_SYSCALL_VEC_NUM 15
        patch_dbg_syscalls( (void *)(&hal_vsr_table[DBG_SYSCALL_VEC_NUM]) );
    }
#endif
}

/*---------------------------------------------------------------------------*/
/* Determine the index of the ls bit of the supplied mask.                   */

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

/*---------------------------------------------------------------------------*/
/* Determine the index of the ms bit of the supplied mask.                   */

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

/*---------------------------------------------------------------------------*/
/* Idle thread action                                                        */

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

    {
//        cyg_uint32 tval, isr, imr, ilr;
          cyg_uint32 sr, cr;
//        HAL_CLOCK_READ( &tval );
//        HAL_READ_UINT32( 0xFFFFC000, isr );
//        HAL_READ_UINT32( 0xFFFFC004, imr );
//        HAL_READ_UINT32( 0xFFFFC01C, ilr );
//        CYG_TRACE2(1, "Timer value, ISR ",tval, isr);
//        CYG_TRACE2(1, "IMR ILR0 ", imr, ilr);

        asm volatile (
            "mfc0  %0,$12;"
            "mfc0  %1,$13;"
            : "=r"(sr), "=r"(cr)
            );

        CYG_TRACE2(1, "Status Cause ", sr, cr);

//        if( count == 4 )
//        {
//            HAL_ENABLE_INTERRUPTS();
//        }
        
//        if( count >= 10 )
//            for(;;);
    }
#endif
}

/*---------------------------------------------------------------------------*/
/* This code installs an interrupt handler to capture Ctrl-C.                */
/* It's here for lack of anywhere more suitable to put it.                   */
  
#if defined(CYG_HAL_USE_ROM_MONITOR) && \
    !defined(CYGDBG_INFRA_DIAG_USE_DEVICE)    && \
    defined(CYG_HAL_TX39_JMR3904)

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
    
static cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    char c;
    CYG_WORD16 disr;
    
    HAL_READ_UINT16( DIAG_SLDISR , disr );

    if( disr & 0x0001 )
    {

        disr = disr & ~0x0001;
    
        HAL_READ_UINT8( DIAG_RFIFO, c );
    
        HAL_WRITE_UINT16( DIAG_SLDISR , disr );
    
#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
        if( c == 3 )
        {
            // Ctrl-C: breakpoint.
            extern void breakpoint(void);
            breakpoint();
        }
#elif defined(CYG_HAL_USE_ROM_MONITOR) && defined(CYG_HAL_USE_ROM_MONITOR_GDB_STUBS)
        if( c == 3 )
        {
            // Ctrl-C: breakpoint.
//                HAL_BREAKPOINT(_breakinst);
            typedef void bpt_fn(void);
            bpt_fn *bfn = ((bpt_fn **)0x80000100)[61];

            bfn();
        }
#elif defined(CYG_HAL_USE_ROM_MONITOR) && defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
        if( c == 3 )
        {
            // Ctrl-C: breakpoint.
            HAL_BREAKPOINT(_breakinst1);
        }            
#endif
    }

    return 0;
}

    
static void hal_init_ctrlc_intr(void)
{
    CYG_WORD16 dicr;
    
    HAL_INTERRUPT_ATTACH( CYG_VECTOR_SIO_0, hal_ctrlc_isr, NULL, NULL );

    HAL_INTERRUPT_UNMASK( CYG_VECTOR_SIO_0 );

    HAL_READ_UINT16( DIAG_SLDICR , dicr );
    dicr = 0x0001;
    HAL_WRITE_UINT16( DIAG_SLDICR , dicr );    
}

#endif

/*---------------------------------------------------------------------------*/
/* End of hal_misc.c                                                         */
