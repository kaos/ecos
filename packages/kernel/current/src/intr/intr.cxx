//==========================================================================
//
//      intr/intr.cxx
//
//      Interrupt class implementations
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
// Contributors: nickg
// Date:         1999-02-17
// Purpose:      Interrupt class implementation
// Description:  This file contains the definitions of the interrupt
//               class.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>             // base kernel types
#include <cyg/infra/cyg_trac.h>           // tracing macros
#include <cyg/infra/cyg_ass.h>            // assertion macros
#include <cyg/kernel/instrmnt.h>           // instrumentation

#include <cyg/kernel/intr.hxx>             // our header

#include <cyg/kernel/sched.hxx>            // scheduler

#include <cyg/kernel/sched.inl>

// -------------------------------------------------------------------------
// Statics

// Interrupt disable counter. Note that this is initialized to 1 since
// enable_interrupts() is called in Cyg_Scheduler::start().

cyg_int32 Cyg_Interrupt::disable_counter = 1;

// -------------------------------------------------------------------------

Cyg_Interrupt::Cyg_Interrupt(
    cyg_vector      vec,                // Vector to attach to
    cyg_priority    pri,                // Queue priority
    CYG_ADDRWORD    d,                  // Data pointer
    cyg_ISR         *ir,                // Interrupt Service Routine
    cyg_DSR         *dr                 // Deferred Service Routine
    )
{
    CYG_REPORT_FUNCTION();
    CYG_REPORT_FUNCARG5("vector=%d, priority=%d, data=%08x, isr=%08x, "
                        "dsr=%08x", vec, pri, d, ir, dr);
    
    vector      = vec;
    priority    = pri;
    isr         = ir;
    dsr         = dr;
    data        = d;

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST
    
    dsr_count   = 0;
    next_dsr    = NULL;

#endif

#ifdef CYGIMP_KERNEL_INTERRUPTS_CHAIN

    next        = NULL;
    
#endif

    CYG_REPORT_RETURN();
    
};

// -------------------------------------------------------------------------

Cyg_Interrupt::~Cyg_Interrupt()
{
    CYG_REPORT_FUNCTION();
    detach();
    CYG_REPORT_RETURN();
};

// -------------------------------------------------------------------------
// DSR handling statics:

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE

Cyg_Interrupt *
Cyg_Interrupt::dsr_table[CYGNUM_KERNEL_INTERRUPTS_DSRS_TABLE_SIZE];

cyg_ucount32 Cyg_Interrupt::dsr_table_head = 0;

cyg_ucount32 Cyg_Interrupt::dsr_table_tail = 0;

#endif

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST

Cyg_Interrupt *Cyg_Interrupt::dsr_list = NULL;

#endif

// -------------------------------------------------------------------------
// Call any pending DSRs

void
Cyg_Interrupt::call_pending_DSRs_inner(void)
{
//    CYG_REPORT_FUNCTION();

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE    

    while( dsr_table_head != dsr_table_tail )
    {
        Cyg_Interrupt *intr = dsr_table[dsr_table_head];

        dsr_table_head++;
        if( dsr_table_head >= CYGNUM_KERNEL_INTERRUPTS_DSRS_TABLE_SIZE )
            dsr_table_head = 0;

        CYG_INSTRUMENT_INTR(CALL_DSR, intr->vector, 0);
        
        intr->dsr( intr->vector, 1, (CYG_ADDRWORD)intr->data );
    }
    
#endif

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST

    while( dsr_list != NULL )
    {
        Cyg_Interrupt *intr;
        cyg_uint32 old_intr;
        cyg_count32 count;
        
        HAL_DISABLE_INTERRUPTS(old_intr);
        
        intr = dsr_list;
        dsr_list = intr->next_dsr;
        count = intr->dsr_count;
        intr->dsr_count = 0;
        
        HAL_RESTORE_INTERRUPTS(old_intr);

        intr->dsr( intr->vector, count, (CYG_ADDRWORD)intr->data );
        
    }
    
#endif
    
};

externC void
cyg_interrupt_call_pending_DSRs(void)
{
    Cyg_Interrupt::call_pending_DSRs_inner();
}

// This function will be provided by HALs that fully implement
// interrupt stack functionality
externC void 
hal_interrupt_stack_call_pending_DSRs(void)
  __attribute__ ((weak, alias("cyg_interrupt_call_pending_DSRs")));

void
Cyg_Interrupt::call_pending_DSRs(void)
{
    hal_interrupt_stack_call_pending_DSRs();
}


// -------------------------------------------------------------------------

void
Cyg_Interrupt::post_dsr(void)
{
//    CYG_REPORT_FUNCTION();

    CYG_INSTRUMENT_INTR(POST_DSR, vector, 0);

    cyg_uint32 old_intr;

    // We need to disable interrupts during this part to
    // guard against nested interrupts.
    
    HAL_DISABLE_INTERRUPTS(old_intr);

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_TABLE
    
    dsr_table[dsr_table_tail++] = this;
    if( dsr_table_tail >= CYGNUM_KERNEL_INTERRUPTS_DSRS_TABLE_SIZE )
        dsr_table_tail = 0;

#endif

#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS_LIST

    // Only add the interrupt to the dsr list if this is
    // the first DSR call.
    // At present DSRs are pushed onto the list and will be
    // called in reverse order. We do not define the order
    // in which DSRs are called, so this is acceptable.
    
    if( dsr_count++ == 0 )
    {
        next_dsr = dsr_list;
        dsr_list = this;
    }
    
#endif
    
    HAL_RESTORE_INTERRUPTS(old_intr);    
};

// -------------------------------------------------------------------------
// A C callable interface to Cyg_Interrupt::post_dsr() that can be used from
// the HAL.

externC void
cyg_interrupt_post_dsr( CYG_ADDRWORD intr_obj )
{
    Cyg_Interrupt* intr = (Cyg_Interrupt*) intr_obj;
    intr->post_dsr ();
}

// -------------------------------------------------------------------------

// FIXME: should have better name - Jifl
externC void
interrupt_end(
    cyg_uint32          isr_ret,
    Cyg_Interrupt       *intr,
    HAL_SavedRegisters  *regs
    )
{
//    CYG_REPORT_FUNCTION();

    // Sometimes we have a NULL intr object pointer.
    cyg_vector vector = (intr!=NULL)?intr->vector:0;
    
    CYG_INSTRUMENT_INTR(END, vector, isr_ret);
    
    
#ifndef CYGIMP_KERNEL_INTERRUPTS_CHAIN

    // Only do this if we are in a non-chained configuration.
    // If we are chained, then chain_isr below will do the DSR
    // posting.
    
    if( isr_ret & Cyg_Interrupt::CALL_DSR && intr != NULL ) intr->post_dsr();

#endif    

#ifdef CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT

    // If we have GDB support enabled, and there is the possibility
    // that this thread will be context switched as a result of this
    // interrupt, then save the pointer to the saved thread context in
    // the thread object so that GDB can get a meaningful context to
    // look at.
    
    Cyg_Scheduler::get_current_thread()->set_saved_context(regs);
    
#endif    
    
    // Now unlock the scheduler, which may also call DSRs
    // and cause a thread switch to happen.
    
    Cyg_Scheduler::unlock();

#ifdef CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT

    Cyg_Scheduler::get_current_thread()->set_saved_context(0);
    
#endif    
    
    CYG_INSTRUMENT_INTR(RESTORE, vector, 0);    
}

// -------------------------------------------------------------------------
// Interrupt chaining statics.

#ifdef CYGIMP_KERNEL_INTERRUPTS_CHAIN

Cyg_Interrupt *Cyg_Interrupt::chain_list[CYGNUM_HAL_ISR_COUNT];

#endif

// -------------------------------------------------------------------------
// Chaining ISR inserted in HAL vector

#ifdef CYGIMP_KERNEL_INTERRUPTS_CHAIN

cyg_uint32
Cyg_Interrupt::chain_isr(cyg_vector vector, CYG_ADDRWORD data)
{
    Cyg_Interrupt *p = *(Cyg_Interrupt **)data;

    CYG_INSTRUMENT_INTR(CHAIN_ISR, vector, 0);

    while( p != NULL )
    {
        if( p->vector == vector )
        {
            register cyg_uint32 isr_ret = p->isr(vector, p->data);

            if( isr_ret & Cyg_Interrupt::CALL_DSR ) p->post_dsr();

            if( isr_ret & Cyg_Interrupt::HANDLED ) break;
        }

        p = p->next;
    }

    return 0;
}

#endif

// -------------------------------------------------------------------------
// Attach an ISR to an interrupt vector.

void
Cyg_Interrupt::attach(void)
{
    CYG_REPORT_FUNCTION();

    CYG_ASSERT( vector >= CYGNUM_HAL_ISR_MIN, "Invalid vector");
    CYG_ASSERT( vector <= CYGNUM_HAL_ISR_MAX, "Invalid vector");

    CYG_INSTRUMENT_INTR(ATTACH, vector, 0);

    HAL_INTERRUPT_SET_LEVEL( vector, priority );
    
#ifdef CYGIMP_KERNEL_INTERRUPTS_CHAIN

    CYG_ASSERT( next == NULL , "Cyg_Interrupt already on a list");

    cyg_uint32 index;

    HAL_TRANSLATE_VECTOR( vector, index );

    if( chain_list[index] == NULL )
    {
        int in_use;
        // First Interrupt on this chain, just assign it and register
        // the chain_isr with the HAL.
        
        chain_list[index] = this;

        HAL_INTERRUPT_IN_USE( vector, in_use );
        CYG_ASSERT( 0 == in_use, "Interrupt vector not free.");
        HAL_INTERRUPT_ATTACH( vector, chain_isr, &chain_list[index], NULL );
    }
    else
    {
        // There are already interrupts chained, add this one into the
        // chain in priority order.
        
        Cyg_Interrupt **p = &chain_list[index];

        while( *p != NULL )
        {
            Cyg_Interrupt *n = *p;

            if( n->priority < priority ) break;
            
            p = &n->next;
        }
        next = *p;
        *p = this;
    }
    
#else
    
    {
        int in_use;


        HAL_INTERRUPT_IN_USE( vector, in_use );
        CYG_ASSERT( 0 == in_use, "Interrupt vector not free.");

        HAL_INTERRUPT_ATTACH( vector, isr, data, this );
    }

#endif    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Detach the ISR from the vector

void
Cyg_Interrupt::detach(void)
{
    CYG_REPORT_FUNCTION();

    CYG_ASSERT( vector >= CYGNUM_HAL_ISR_MIN, "Invalid vector");    
    CYG_ASSERT( vector <= CYGNUM_HAL_ISR_MAX, "Invalid vector");

    CYG_INSTRUMENT_INTR(DETACH, vector, 0);

#ifdef CYGIMP_KERNEL_INTERRUPTS_CHAIN

    // Remove the interrupt object from the vector chain.
    
    cyg_uint32 index;

    HAL_TRANSLATE_VECTOR( vector, index );

    Cyg_Interrupt **p = &chain_list[index];

    while( *p != NULL )
    {
        Cyg_Interrupt *n = *p;

        if( n == this )
        {
            *p = next;
            break;
        }
            
        p = &n->next;
    }

    // If this was the last one, detach the vector.
    
    if( chain_list[index] == NULL )
        HAL_INTERRUPT_DETACH( vector, chain_isr );
    
#else
    
    HAL_INTERRUPT_DETACH( vector, isr );

#endif

    CYG_REPORT_RETURN();
    
}

// -------------------------------------------------------------------------
// Get the current service routine

void
Cyg_Interrupt::get_vsr(cyg_vector vector, cyg_VSR **vsr)
{
    CYG_REPORT_FUNCTION();
    CYG_REPORT_FUNCARG2("vector = %d, mem to put VSR in is at %08x", vector,
                        vsr);

    CYG_ASSERT( vector >= CYGNUM_HAL_VSR_MIN, "Invalid vector");        
    CYG_ASSERT( vector <= CYGNUM_HAL_VSR_MAX, "Invalid vector");

    HAL_VSR_GET( vector, vsr );

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Install a vector service routine

void
Cyg_Interrupt::set_vsr(cyg_vector vector, cyg_VSR *vsr, cyg_VSR **old)
{
    CYG_REPORT_FUNCTION();

    CYG_REPORT_FUNCARG3( "vector = %d, new vsr is at %08x, mem to put "
                         "old VSR in is at %08x", vector, vsr, old);

    CYG_INSTRUMENT_INTR(SET_VSR, vector, vsr);

    CYG_ASSERT( vector >= CYGNUM_HAL_VSR_MIN, "Invalid vector");    
    CYG_ASSERT( vector <= CYGNUM_HAL_VSR_MAX, "Invalid vector");    

    CYG_INTERRUPT_STATE old_ints;
    
    HAL_DISABLE_INTERRUPTS(old_ints);

    HAL_VSR_SET( vector, vsr, old );
    
    HAL_RESTORE_INTERRUPTS(old_ints);

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Disable interrupts at the CPU


void
Cyg_Interrupt::disable_interrupts(void)
{
    CYG_REPORT_FUNCTION();

    CYG_INSTRUMENT_INTR(DISABLE, disable_counter+1, 0);

    CYG_INTERRUPT_STATE dummy;

    HAL_DISABLE_INTERRUPTS(dummy);

    CYG_ASSERT( disable_counter >= 0 , "Disable counter negative");
    
    disable_counter++;
    
    CYG_REPORT_RETURN();
}


// -------------------------------------------------------------------------
// Re-enable CPU interrupts

void
Cyg_Interrupt::enable_interrupts(void)
{
    CYG_REPORT_FUNCTION();
        
    CYG_INSTRUMENT_INTR(ENABLE, disable_counter, 0);

    CYG_ASSERT( disable_counter > 0 , "Disable counter not greater than zero");
    
    disable_counter--;

    if ( disable_counter == 0 )
    {
        HAL_ENABLE_INTERRUPTS();
    }

    CYG_REPORT_RETURN();
}
    
// -------------------------------------------------------------------------
// Mask a specific interrupt in a PIC

void
Cyg_Interrupt::mask_interrupt(cyg_vector vector)
{
    CYG_REPORT_FUNCTION();
    CYG_REPORT_FUNCARG1("vector=%d", vector);

    CYG_ASSERT( vector >= CYGNUM_HAL_ISR_MIN, "Invalid vector");    
    CYG_ASSERT( vector <= CYGNUM_HAL_ISR_MAX, "Invalid vector");

    CYG_INSTRUMENT_INTR(MASK, vector, 0);

    HAL_INTERRUPT_MASK( vector );

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Clear PIC mask

void
Cyg_Interrupt::unmask_interrupt(cyg_vector vector)
{
    CYG_REPORT_FUNCTION();

    CYG_ASSERT( vector >= CYGNUM_HAL_ISR_MIN, "Invalid vector");    
    CYG_ASSERT( vector <= CYGNUM_HAL_ISR_MAX, "Invalid vector");
    
    CYG_INSTRUMENT_INTR(UNMASK, vector, 0);

    HAL_INTERRUPT_UNMASK( vector );

    CYG_REPORT_RETURN();
}
    

// -------------------------------------------------------------------------
// Acknowledge interrupt at PIC

void
Cyg_Interrupt::acknowledge_interrupt(cyg_vector vector)
{
//    CYG_REPORT_FUNCTION();

    CYG_ASSERT( vector >= CYGNUM_HAL_ISR_MIN, "Invalid vector");    
    CYG_ASSERT( vector <= CYGNUM_HAL_ISR_MAX, "Invalid vector");

    CYG_INSTRUMENT_INTR(ACK, vector, 0);

    HAL_INTERRUPT_ACKNOWLEDGE( vector );
}

// -------------------------------------------------------------------------
// Change interrupt detection at PIC

void
Cyg_Interrupt::configure_interrupt(
    cyg_vector vector,              // vector to control
    cyg_bool level,                 // level or edge triggered
    cyg_bool up                     // hi/lo level, rising/falling edge
    )
{
    CYG_REPORT_FUNCTION();
    CYG_REPORT_FUNCARG3("vector = %d, level = %d, up = %d", vector, level,
                        up);

    CYG_ASSERT( vector >= CYGNUM_HAL_ISR_MIN, "Invalid vector");    
    CYG_ASSERT( vector <= CYGNUM_HAL_ISR_MAX, "Invalid vector");

    CYG_INSTRUMENT_INTR(CONFIGURE, vector, (level<<1)|up);

    HAL_INTERRUPT_CONFIGURE( vector, level, up );

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// EOF intr/intr.cxx
