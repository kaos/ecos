//=================================================================
//
//        iram.c
//
//        Test support for on-chip memory
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2008-01-14
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/system.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_ass.h>

#if   !defined(CYGMEM_REGION_iram)
# define NA_MSG     "No IRAM memory region defined"
#elif defined(CYGMEM_REGION_ram) && (CYGMEM_REGION_iram == CYGMEM_REGION_ram)
# define NA_MSG     "IRAM is used as the main RAM memory bank"
#endif

#ifdef NA_MSG

externC void
cyg_start( void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(NA_MSG);
}

#else   // NA_MSG

// These are provided by the linker script
externC cyg_uint8   _hal_iram_section_start_vma[];
externC cyg_uint8   _hal_iram_bss_section_start[];
externC cyg_uint8   _hal_iram_section_end_vma[];

// The external declarations which allow the attributes to be
// specified.
externC void    onchip_fn1(void)    CYGBLD_ATTRIB_SECTION(".iram_text.onchip_fn1");
externC int     onchip_fn2(int)     CYGBLD_ATTRIB_SECTION(".iram_text.onchip_fn2");
extern int      onchip_data1[8]     CYGBLD_ATTRIB_SECTION(".iram_data.onchip_data1");
extern char     onchip_data2        CYGBLD_ATTRIB_SECTION(".iram_data.onchip_data2");
extern int      onchip_data3[1024]  CYGBLD_ATTRIB_SECTION(".iram_data.onchip_data3");
extern int      onchip_bss1         CYGBLD_ATTRIB_SECTION(".iram_bss.onchip_bss1");
extern int      onchip_bss2[42]     CYGBLD_ATTRIB_SECTION(".iram_bss.onchip_bss2");

// Then the definitions. data3 should be garbage collected by the linker.
int      onchip_data1[8]     = { 1, 2, 3, 4, 5, 6, 7, 8 };
char     onchip_data2        = 42;
int      onchip_data3[1024]  = { 1 } ;
int      onchip_bss1;
int      onchip_bss2[42];

void
onchip_fn1(void)
{
    int i, data1_sum, fn2_result;

    diag_printf("PASS:<onchip_fn1 running>\n");

    // Check again that the data has been correctly initialized,
    // that there are no addressing funnies, and that the data can
    // be overwritten.
    for (i = 0, data1_sum = 0; i < 8; i++) {
        data1_sum       += onchip_data1[i];
        onchip_data1[i] -= i;
    }
    if (36 != data1_sum) {
        diag_printf("FAIL:<onchip data1 array should add up to 36>\n");
    }

    for (i = 0, data1_sum = 0; i < 8; i++) {
        data1_sum   += onchip_data1[i];
    }
    if (8 != data1_sum) {
        diag_printf("FAIL:<onchip data1 array should now add up to 8>\n");
    }
    // Call one on-chip function from another.
    fn2_result = onchip_fn2(data1_sum);
    if (861 != fn2_result) {
        diag_printf("FAIL:<onchip_fn2 should return 903>\n");
    }
}

int
onchip_fn2(int arg)
{
    int i;
    int result;
    diag_printf("PASS:<onchip_fn2 running>\n");

    // Make sure that the on-chip bss is correctly zero-initialized
    // and writable.
    for (i = 0; i < 42; i++) {
        if (0 != onchip_bss2[i]) {
            diag_printf("FAIL:<onchip bss2 should be initialized to 0>\n");
        }
        onchip_bss2[i]  = i;
    }
    for (i = 0, result = 0; i < 42; i++) {
        result  += onchip_bss2[i];
    }
    return result;
}

void
check_addr(void* where)
{
    if ((where < (void*)_hal_iram_section_start_vma) || (where >= (void*)_hal_iram_section_end_vma)) {
        diag_printf("FAIL:<ptr %p is not in IRAM region\n", where);
    }
}
    
externC void
cyg_start( void)
{
    int i;
    int data1_sum;
    
    CYG_TEST_INIT();
    // For human inspection
    diag_printf("INFO:<IRAM usage : %p -> %p\n", _hal_iram_section_start_vma, _hal_iram_section_end_vma);
    diag_printf("INFO:<IRAM bss   @ %p\n", _hal_iram_bss_section_start);
    diag_printf("INFO:<RAM        : %p -> %p\n", (void*)CYGMEM_REGION_ram, (void*)(CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE));
    diag_printf("INFO:<onchip_fn1   @ %p>\n", &onchip_fn1);
    diag_printf("INFO:<onchip_fn2   @ %p>\n", &onchip_fn2);
    diag_printf("INFO:<onchip_data1 @ %p>\n", &onchip_data1[0]);

    // Make sure that IRAM is really separate from main memory.
    if ((_hal_iram_section_start_vma >= (cyg_uint8*)CYGMEM_REGION_ram) &&
        (_hal_iram_section_start_vma <  (cyg_uint8*)(CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE))) {
        CYG_TEST_FAIL("IRAM start overlaps SDRAM");
    }
    if ((_hal_iram_section_end_vma >= (cyg_uint8*)CYGMEM_REGION_ram) &&
        (_hal_iram_section_end_vma <  (cyg_uint8*)(CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE))) {
        CYG_TEST_FAIL("IRAM end overlaps SDRAM");
    }

    // Make sure that various objects are correctly placed.
    check_addr(&onchip_fn1);
    check_addr(&onchip_fn2);
    check_addr(&onchip_data1[0]);
    check_addr(&onchip_data2);
    // Not data3, we want that one to be garbage collected.
    check_addr(&onchip_bss1);
    check_addr(&onchip_bss2[0]);

    // Check that on-chip data is correctly initialized.
    CYG_TEST_CHECK( 42 == onchip_data2, "onchip_data2 should be the answer");
    for (i = 0, data1_sum = 0; i < 8; i++) {
        data1_sum   += onchip_data1[i];
    }
    CYG_TEST_CHECK( 36 == data1_sum, "onchip data1 array should add up to 36");

    // Make sure we can call code located in iram.
    {
        void (*onchip_fn1_ptr)(void) = &onchip_fn1;
        (*onchip_fn1_ptr)();
    }

    CYG_TEST_PASS_FINISH("IRAM test");
}

#endif  // NA_MSG
