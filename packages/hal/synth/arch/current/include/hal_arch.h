#ifndef CYGONCE_HAL_HAL_ARCH_H
#define CYGONCE_HAL_HAL_ARCH_H

//=============================================================================
//
//      hal_arch.h
//
//      Architecture specific abstractions for synthetic target.
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:proven, pjo, nickg,bartv
// Date:        1998-10-05
// Purpose:     Define architecture abstractions
// Usage:       #include <cyg/hal/hal_arch.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

// ----------------------------------------------------------------------------
// Architectural definitions such as processor save state information.
// For the synthetic target most of this information has to be
// provided by the variant HALs.

#include <cyg/hal/var_arch.h>

//-----------------------------------------------------------------------------
// Exception handling function.
// This function is provided by either the kernel or the common HAL to
// deliver an exception to the current thread.
// NOTE: the declaration should move to a common package. 
externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//-----------------------------------------------------------------------------
// Execution reorder barrier.
// When optimizing the compiler can reorder code. In multithreaded systems
// where the order of actions is vital, this can sometimes cause problems.
// This macro may be inserted into places where reordering should not happen.

#define HAL_REORDER_BARRIER() asm volatile ( "" : : : "memory" )

//-----------------------------------------------------------------------------
// Idle thread code.
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. In the synthetic target it involves blocking
// until there is a signal.

externC void hal_idle_thread_action(cyg_uint32 loop_count);

#define HAL_IDLE_THREAD_ACTION(_count_) hal_idle_thread_action(_count_)

//--------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_ARCH_H
// End of hal_arch.h
