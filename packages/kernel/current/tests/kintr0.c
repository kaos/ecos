/*=================================================================
//
//        kintr0.c
//
//        Kernel C API Intr test 0
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
// Author(s):     dsm
// Contributors:  dsm, jlarmour
// Date:          1999-02-16
// Description:   Very basic test of interrupt objects
// Options:
//     CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE
//     CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE_MAX
//     CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST
//####DESCRIPTIONEND####
*/

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_intr.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include "testaux.h"

static cyg_interrupt intr_obj[2];

static cyg_handle_t intr0, intr1;


static cyg_ISR_t isr0, isr1;
static cyg_DSR_t dsr0, dsr1;

static cyg_uint32 isr0(cyg_vector_t vector, cyg_addrword_t data)
{
    CYG_UNUSED_PARAM(cyg_addrword_t, data);

    cyg_interrupt_acknowledge(vector);
    return 0;
}

static void dsr0(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    CYG_UNUSED_PARAM(cyg_vector_t, vector);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(cyg_addrword_t, data);
}

static cyg_uint32 isr1(cyg_vector_t vector, cyg_addrword_t data)
{
    CYG_UNUSED_PARAM(cyg_vector_t, vector);
    CYG_UNUSED_PARAM(cyg_addrword_t, data);
    return 0;
}

static void dsr1(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    CYG_UNUSED_PARAM(cyg_vector_t, vector);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(cyg_addrword_t, data);
}

static bool flash( void )
{
    cyg_handle_t handle;
    cyg_interrupt intr;

    cyg_interrupt_create(0, 0, (cyg_addrword_t)333, isr0, dsr0, &handle, &intr );
    cyg_interrupt_delete(handle);

    return true;
}

static cyg_VSR_t vsr0;

static void vsr0()
{
}

void kintr0_main( void )
{
    cyg_vector_t v = 11 % CYGNUM_HAL_VSR_COUNT;
    cyg_vector_t v1;
    cyg_vector_t lvl1 = 1 % (CYGNUM_HAL_ISR_COUNT);
    cyg_vector_t lvl2 = 15 % (CYGNUM_HAL_ISR_COUNT);
    int in_use;

    cyg_VSR_t *old_vsr, *new_vsr;

    CYG_TEST_INIT();
 
#ifdef CYGPKG_HAL_MIPS_TX39    
    // This can be removed when PR 17831 is fixed
    if ( cyg_test_is_simulator )
        v1 = 12 % CYGNUM_HAL_ISR_COUNT;
    else /* NOTE TRAILING ELSE... */
#endif
    v1 = 6 % CYGNUM_HAL_ISR_COUNT;

    CHECK(flash());
    CHECK(flash());

    // Make sure the chosen levels are not already in use.
    HAL_INTERRUPT_IN_USE( lvl1, in_use );
    intr0 = 0;
    if (!in_use)
        cyg_interrupt_create(lvl1, 1, (cyg_addrword_t)777, isr0, dsr0, 
                             &intr0, &intr_obj[0]);
    
    HAL_INTERRUPT_IN_USE( lvl2, in_use );
    intr1 = 0;
    if (!in_use && lvl1 != lvl2)
        cyg_interrupt_create(lvl2, 1, 888, isr1, dsr1, &intr1, &intr_obj[1]);

    // Check these functions at least exist

    cyg_interrupt_enable();
    cyg_interrupt_disable();

    if (intr0)
        cyg_interrupt_attach(intr0);
    if (intr1)
        cyg_interrupt_attach(intr1);
    if (intr0)
        cyg_interrupt_detach(intr0);
    if (intr1)
        cyg_interrupt_detach(intr1);

    // If this attaching interrupt replaces the previous interrupt
    // instead of adding to it we could be in a big mess if the
    // vector is being used by something important.
        
    cyg_interrupt_get_vsr( v, &old_vsr );
    cyg_interrupt_set_vsr( v, vsr0 );
    cyg_interrupt_get_vsr( v, &new_vsr );
    CHECK( vsr0 == new_vsr );

    new_vsr = NULL;
    cyg_interrupt_get_vsr( v, &new_vsr );
    cyg_interrupt_set_vsr( v, old_vsr );
    CHECK( new_vsr == vsr0 );

    cyg_interrupt_set_vsr( v, new_vsr );
    new_vsr = NULL;
    cyg_interrupt_get_vsr( v, &new_vsr );       
    CHECK( vsr0 == new_vsr );

    cyg_interrupt_set_vsr( v, old_vsr );
    CHECK( vsr0 == new_vsr );
    new_vsr = NULL;
    cyg_interrupt_get_vsr( v, &new_vsr );
    CHECK( old_vsr == new_vsr );
        
    CHECK( NULL != vsr0 );

    cyg_interrupt_mask(v1);
    cyg_interrupt_unmask(v1);
        
    cyg_interrupt_configure(v1, true, true);

    CYG_TEST_PASS_FINISH("Kernel C API Intr 0 OK");
}

externC void
cyg_start( void )
{ 
    kintr0_main();
}

#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

/* EOF kintr0.c */
