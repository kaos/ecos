//==========================================================================
//
//      sync/mqueue.cxx
//
//      Mqueue message queue non-inline implementation
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2001-10-12
// Purpose:      Non-inlined implementation of mqueue message queue.
// Description:  This file contains the non-inlined instantiations of the
//               mqueue message queue implementation functions.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#ifdef CYGIMP_KERNEL_SYNCH_MQUEUE_NOT_INLINE

#define CYGPRI_KERNEL_SYNCH_MQUEUE_INLINE
#include <cyg/kernel/mqueue.hxx>
#include <cyg/kernel/mqueue.inl>

#endif

// -------------------------------------------------------------------------
// EOF sync/mqueue.cxx
