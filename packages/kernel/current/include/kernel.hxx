#ifndef CYGONCE_KERNEL_KERNEL_HXX
#define CYGONCE_KERNEL_KERNEL_HXX

//==========================================================================
//
//      kernel.hxx
//
//      Kernel mega-include file
//
//==========================================================================
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:        nickg
// Date:        1997-09-09
// Purpose:     Include all kernel files
// Description: This file contains includes for all the kernel
//              headers. This simplifys things in the sources.
// Usage:       #include <cyg/kernel/kernel.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================


#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>

#include <cyg/infra/cyg_ass.h>            // assertion macros
#include <cyg/infra/cyg_trac.h>           // tracing macros

#include <cyg/kernel/errors.h>

#include <cyg/kernel/instrmnt.h>

#include <cyg/kernel/diag.h>

#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/intr.hxx>
#include <cyg/kernel/clock.hxx>

#include <cyg/kernel/sema.hxx>
#include <cyg/kernel/mutex.hxx>

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/clock.inl>

// -------------------------------------------------------------------------
#endif  // #ifndef CYGONCE_KERNEL_KERNEL_HXX
// EOF kernel.hxx
