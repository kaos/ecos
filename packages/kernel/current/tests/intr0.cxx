//=================================================================
//
//        intr0.cxx
//
//        Interrupt test 0
//
//=================================================================
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-06-15
// Description:   Very basic test of interrupt objects
// Options:
//     CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE
//     CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE_SIZE
//     CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/intr.hxx>
#include <cyg/hal/hal_intr.h>

#include <cyg/infra/testcase.h>

#include "testaux.hxx"

static cyg_ISR isr0, isr1;
static cyg_DSR dsr0, dsr1;

static cyg_uint32 isr0(cyg_vector vector, CYG_ADDRWORD data)
{
    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);

    Cyg_Interrupt::acknowledge_interrupt(vector);
    return 0;
}

static void dsr0(cyg_vector vector, cyg_ucount32 count, CYG_ADDRWORD data)
{
    CYG_UNUSED_PARAM(cyg_vector, vector);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);
}

static cyg_uint32 isr1(cyg_vector vector, CYG_ADDRWORD data)
{
    CYG_UNUSED_PARAM(cyg_vector, vector);
    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);
    return 0;
}

static void dsr1(cyg_vector vector, cyg_ucount32 count, CYG_ADDRWORD data)
{
    CYG_UNUSED_PARAM(cyg_vector, vector);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);
}

static bool flash( void )
{
    Cyg_Interrupt intr0 = Cyg_Interrupt( 0, 0, (CYG_ADDRWORD)333, isr0, dsr0 );

    return true;
}

static cyg_VSR vsr0;

static void vsr0()
{
}

void intr0_main( void )
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());

    Cyg_Interrupt intr0 = 
	Cyg_Interrupt( 1 % (CYG_ISR_COUNT), 1, (CYG_ADDRWORD)777, isr0, dsr0 );
    
    Cyg_Interrupt intr1 = 
	Cyg_Interrupt( 15 % (CYG_ISR_COUNT), 1, 888, isr1, dsr1 );
    

    // Check these functions at least exist

    Cyg_Interrupt::enable_interrupts();
    Cyg_Interrupt::disable_interrupts();

    intr0.attach();
    intr1.attach();
    intr0.detach();
    intr1.detach();

    // If this attaching interrupt replaces the previous interrupt
    // instead of adding to it we could be in a big mess if the
    // vector is being used by something important.
	
    cyg_vector v = 11 % CYG_VSR_COUNT;
    cyg_VSR *old_vsr, *new_vsr;
    Cyg_Interrupt::set_vsr( v, vsr0, &old_vsr );
    Cyg_Interrupt::get_vsr( v, &new_vsr );
    CHECK( vsr0 == new_vsr );

    new_vsr = NULL;
    Cyg_Interrupt::set_vsr( v, old_vsr, &new_vsr );
    CHECK( new_vsr == vsr0 );

    Cyg_Interrupt::set_vsr( v, new_vsr );
    new_vsr = NULL;
    Cyg_Interrupt::get_vsr( v, &new_vsr );	
    CHECK( vsr0 == new_vsr );

    Cyg_Interrupt::set_vsr( v, old_vsr );
    CHECK( vsr0 == new_vsr );
    new_vsr = NULL;
    Cyg_Interrupt::get_vsr( v, &new_vsr );
    CHECK( old_vsr == new_vsr );
	
    CHECK( NULL != vsr0 );

    cyg_vector v1 = 6 % CYG_ISR_COUNT;
    
    Cyg_Interrupt::mask_interrupt(v1);
    Cyg_Interrupt::unmask_interrupt(v1);

    Cyg_Interrupt::configure_interrupt(v1, true, true);

    CYG_TEST_PASS_FINISH("Intr 0 OK");
}

externC void
cyg_start( void )
{
    intr0_main();
}
// EOF intr0.cxx
