/*========================================================================
//
//      heapgen.cpp
//
//      Helper file for heapgen.tcl
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-06-13
// Purpose:       Helper file for heapgen.tcl
// Description:   Exports macros derived from the configuration so that
//                they are visible to heapgen.tcl. This file is
//                preprocessed by a make rule in the CDL to generate
//                "heapgeninc.tcl"
//                Note, this isn't a real C file. It is only to be
//                preprocessed, not compiled
// Usage:         
//
//####DESCRIPTIONEND####
//
//======================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/memalloc.h>

#define STRINGIFY1(_x_) #_x_
#define STRINGIFY(_x_) STRINGIFY1(_x_)

set memlayout_h   STRINGIFY(CYGHWR_MEMORY_LAYOUT_H)
set memlayout_ldi STRINGIFY(CYGHWR_MEMORY_LAYOUT_LDI)
set malloc_impl_h   STRINGIFY(CYGBLD_MEMALLOC_MALLOC_IMPLEMENTATION_HEADER)
#define __MALLOC_IMPL_WANTED
#include CYGBLD_MEMALLOC_MALLOC_IMPLEMENTATION_HEADER
set malloc_impl_class   STRINGIFY(CYGCLS_MEMALLOC_MALLOC_IMPL)

/* EOF heapgen.cpp */
