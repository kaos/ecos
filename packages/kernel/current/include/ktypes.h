#ifndef CYGONCE_KERNEL_KTYPES_H
#define CYGONCE_KERNEL_KTYPES_H

//==========================================================================
//
//      ktypes.h
//
//      Standard types used in the kernel and its interfaces
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   nickg from an original by hmt
// Contributors:  nickg
// Date:        1997-09-08
// Purpose:     Define kernel specific types
// Description: Kernel specific types
// Usage:       #include <cyg/kernel/ktypes.h>
//              ...
//              cyg_priority priority; // etc
//              
//####DESCRIPTIONEND####
//
//==========================================================================

// -------------------------------------------------------------------------
// Check that a configuration file is present.

#ifndef CYGONCE_PKGCONF_KERNEL_H
#error "No Configuration file included"
#endif

// -------------------------------------------------------------------------

#include <cyg/infra/cyg_type.h>
#include <stddef.h>              // for size_t

// -------------------------------------------------------------------------
// Integer types:

typedef cyg_int32       cyg_code;       // General return/error/status code

typedef cyg_count32     cyg_priority;   // priority value

typedef cyg_uint32      cyg_vector;     // vector number/descriptor

typedef cyg_uint64      cyg_tick_count; // clock tick count value

// -------------------------------------------------------------------------
// Predefinitions of various kernel classes

#ifdef __cplusplus

class Cyg_Scheduler;
class Cyg_Scheduler_Implementation;

class Cyg_HardwareThread;
class Cyg_SchedThread;
class Cyg_SchedThread_Implementation;
class Cyg_Thread;

class Cyg_ThreadQueue;
class Cyg_ThreadQueue_Implementation;

#endif


// -------------------------------------------------------------------------
// Class and structure conversion macros.
// CYG_CLASSFROMFIELD translates a pointer to a field of a struct or
// class into a pointer to the class.
// CYG_OFFSETOFBASE yields the offset of a base class of a derived
// class.
// CYG_CLASSFROMBASE translates a pointer to a base class into a pointer
// to a selected derived class. The base class object _must_ be part of
// the specified derived class. This is essentially a poor mans version
// of the RTTI dynamic_cast operator.
// Caveat: These macros do not work for virtual base classes.

#define CYG_CLASSFROMFIELD(_type_,_member_,_ptr_)\
    ((_type_ *)((char *)(_ptr_)-((char *)&(((_type_ *)0)->_member_))))

#ifdef __cplusplus

#define CYG_OFFSETOFBASE(_type_,_base_)\
    ((char *)((_base_ *)((_type_ *)4)) - (char *)4)

# define CYG_CLASSFROMBASE(_class_,_base_,_ptr_)\
    ((_class_ *)((char *)(_ptr_) - CYG_OFFSETOFBASE(_class_,_base_)))

#endif

// -------------------------------------------------------------------------
#endif // CYGONCE_KERNEL_KTYPES_H
// EOF ktypes.h

