//==========================================================================
//
//      hal_startup.c
//
//      ColdFire architecture HAL startup code
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       Architecture startup code.
// Description:   This module contains code that sets up the hardware and the
//                memory sections. All the code must be contained in the
//                section called ".boot", in order for the ROMRAM startup
//                to work properly.
//
//####DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_startup.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/hal/hal_if.h>         // hal_if_init
#include <cyg/hal/hal_intr.h>       // Interrupt definitions
#include <cyg/hal/hal_stub.h>       // initialize_stub

externC void cyg_start(void);
externC void hal_ctrlc_isr_init(void);

static void hal_vsr_init(void) __attribute__ ((section (".boot")));
static void hal_isr_init(void) __attribute__ ((section (".boot")));
static void hal_init_ram_sections(void) __attribute__ ((section (".boot")));
static void cyg_hal_invoke_constructors(void) __attribute__ ((section (".boot")));

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

externC unsigned char __romram_copy_source[];
externC unsigned char __romram_copy_dest[];
externC unsigned char __romram_copy_length[];

#endif


// -------------------------------------------------------------------------
// Reset vector routine.

void hal_reset(void)
{
    // Do any variant-specific reset initialization
    var_reset();

    // Do any platform-specific reset initialization
    plf_reset();

    // Initialize the RAM sections that the rest of the C code requires
    hal_init_ram_sections();

    // All program sections are now in place

    // Make sure that every instruction above this one has been output by
    // the compiler
    HAL_REORDER_BARRIER();

    // Now it is safe to use a stack in RAM
    asm volatile ("lea cyg_interrupt_stack, %sp");
   
    // It is now safe to call C functions which may rely on initialized
    // data
    hal_vsr_init();
    hal_isr_init();

    // Initialize variant HAL private data
    var_init_data();

    // Initialize platform HAL private data
    plf_init_data();

    // Initialize the virtual vector table
    hal_if_init();

    // Call C++ constructors
    cyg_hal_invoke_constructors();

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    initialize_stub();
#endif

    // Init Ctrl-C debug ISR
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
    hal_ctrlc_isr_init();
#endif

    // Call cyg_start. This routine should not return.
    cyg_start();
}


// -------------------------------------------------------------------------
// Initialize the vector table.

static void hal_vsr_init(void)
{
    unsigned int i;
    
    // If we are starting up from ROM, or we are starting in
    // RAM and NOT using a ROM monitor, initialize the VSR and ISR tables.
#if defined(CYG_HAL_STARTUP_ROM) ||         \
    defined(CYG_HAL_STARTUP_ROMRAM) ||      \
        (defined(CYG_HAL_STARTUP_RAM) &&     \
        !defined(CYGSEM_HAL_USE_ROM_MONITOR))

    // Initialize the HAL's vector table with the ROM vector table
    for (i = 0; i < CYGNUM_HAL_VSR_COUNT; i++)
        cyg_hal_vsr_table[i] = rom_vsr_table[i];

#elif defined(CYG_HAL_STARTUP_RAM) && defined(CYGSEM_HAL_USE_ROM_MONITOR)

    // We only take control of the interrupt vectors,
    // the rest are left to the ROM for now.
    cyg_hal_vsr_table[CYGNUM_HAL_VECTOR_UNINITINT] =
        rom_vsr_table[CYGNUM_HAL_VECTOR_UNINITINT];

    cyg_hal_vsr_table[CYGNUM_HAL_VECTOR_SPURINT] =
        rom_vsr_table[CYGNUM_HAL_VECTOR_SPURINT];

    for(i = 0; i < CYGNUM_HAL_NUMAUTOVEC; i++)
        cyg_hal_vsr_table[CYGNUM_HAL_VECTOR_AUTOVEC1 + i] =
            rom_vsr_table[CYGNUM_HAL_VECTOR_AUTOVEC1 + i];
    
    for(i = 0; i < CYGNUM_HAL_NUMUSERINTR; i++)
        cyg_hal_vsr_table[CYGNUM_HAL_VECTOR_USERINTRFIRST + i] =
            rom_vsr_table[CYGNUM_HAL_VECTOR_USERINTRFIRST + i];

#endif
}


// -------------------------------------------------------------------------
// Initialize the ISRs.

static void hal_isr_init(void)
{
    int i;

    // Initialize all ISR entries to default
    for (i = 0; i < CYGNUM_HAL_ISR_COUNT; i++)
    {
        cyg_hal_interrupt_handlers[i] = (CYG_ADDRESS) &hal_default_isr;
        cyg_hal_interrupt_data[i] = (CYG_ADDRWORD) 0;
        cyg_hal_interrupt_objects[i] = (CYG_ADDRESS) 0;
    }
}


// -------------------------------------------------------------------------
// Initialize the RAM sections
// For an efficient copy, we suppose that the sections are aligned at a
// 4-byte boundary and are a multiple of 4 bytes. Linker scripts should
// guarantee this.

static void hal_init_ram_sections(void)
{
    cyg_uint32 *m;

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
    {
        // Initialize the RAM data section from the ROM image of the data
        // section.
        cyg_uint32 *p = (cyg_uint32 *) __romram_copy_dest;
        cyg_uint32 *q = (cyg_uint32 *) __romram_copy_source;
        cyg_uint32 length = 0;

        while (length < (cyg_uint32) __romram_copy_length)
        {
            *p++ = *q++;
            length += 4;
        }
    }       
#endif

    // Initialize the bss sections to zero
    m = (cyg_uint32 *) __bss_start;
    while (m != (cyg_uint32 *) __bss_end)
        *m++ = 0x0;
}


// -------------------------------------------------------------------------
// Call static constructors.

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

typedef void (*pfunc) (void);
extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];

static void cyg_hal_invoke_constructors(void)
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
}
