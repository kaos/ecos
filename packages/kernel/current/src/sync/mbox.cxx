//==========================================================================
//
//      mbox.cxx
//
//      Mbox mbox template class implementation
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
// Author(s):   hmt
// Contributors:        hmt
// Date:        1998-02-11
// Purpose:     Mbox implementation
// Description: This file contains the implementations of the mbox class
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation

#include <cyg/kernel/thread.inl>       // Cyg_Thread inlines

#include <cyg/kernel/mbox.hxx>         // our own header

#ifndef CYGIMP_MBOXT_INLINE            // force inlining
#define CYGIMP_MBOXT_INLINE            // of implementation
#endif

#ifdef CYGIMP_MBOX_USE_MBOXT_PLAIN
#include <cyg/kernel/mboxt.inl>        // mbox template implementation
#else
#include <cyg/kernel/mboxt2.inl>       // mbox template implementation
#endif

// -------------------------------------------------------------------------
// This module exists to cause exactly one instance of these functions to
// exist; this is just like a vanilla class, except we use the template
// class to acquire an implementation.  The template functions are inlined
// in each of these methods.


// -------------------------------------------------------------------------
// Constructor

Cyg_Mbox::Cyg_Mbox()
{
}

// -------------------------------------------------------------------------
// Destructor

Cyg_Mbox::~Cyg_Mbox()
{
}

// -------------------------------------------------------------------------
// debugging/assert function

#ifdef CYGDBG_USE_ASSERTS
cyg_bool 
Cyg_Mbox::check_this(cyg_assert_class_zeal zeal) const
{
    return m.check_this(zeal);
}
#endif

// -------------------------------------------------------------------------
// now the members themselves:
    
void *
Cyg_Mbox::get()
{
    void * p;
    if ( ! m.get( p ) )
        return NULL;
    return p;
}

#ifdef CYGFUN_KERNEL_THREADS_TIMER
void *
Cyg_Mbox::get( cyg_tick_count timeout )
{
    void * p;
    if ( ! m.get( p, timeout ) )
        return NULL;
    return p;
}
#endif

void *
Cyg_Mbox::tryget()
{
    void * p;
    if ( ! m.tryget( p ) )
        return NULL;
    return p;
}

#ifdef  CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
cyg_bool
Cyg_Mbox::put( void *item )
{
    return m.put( item );
}

#ifdef CYGFUN_KERNEL_THREADS_TIMER
cyg_bool
Cyg_Mbox::put( void *item, cyg_tick_count timeout )
{
    return m.put( item, timeout );
}
#endif
#endif // CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT

cyg_bool
Cyg_Mbox::tryput( void *item )
{
    return m.tryput( item );
}

void *
Cyg_Mbox::peek_item()                   // Get next item to be returned
{
    void *p;
    if ( ! m.peek_item( p ) )
        return NULL;
    return p;
}

// -------------------------------------------------------------------------
// EOF mbox.cxx
