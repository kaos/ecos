#ifndef CYGONCE_MEMALLOC_KAPIDATA_H
#define CYGONCE_MEMALLOC_KAPIDATA_H

/*==========================================================================
//
//      kapidata.h
//
//      Memory allocator portion of kernel C API
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
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-06-12
// Purpose:      Memory allocator data for kernel C API
// Description:  This is intentionally only to be included via
//               <cyg/kernel/kapi.h>
// Usage:        This file should not be used directly - instead it should
//               be used via <cyg/kernel/kapi.h>
//              
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/memalloc.h>

/*---------------------------------------------------------------------------*/

/* This corresponds to the extra fields provided by the mempoolt template
   not the actual size of the template in any given instance. */
typedef struct cyg_mempoolt {
    cyg_threadqueue queue;
} cyg_mempoolt;


struct cyg_mempool_var_memdq {
    struct cyg_mempool_var_memdq *prev, *next;
    cyg_int32 size;
};

struct cyg_mempool_var {
    struct cyg_mempool_var_memdq head;
    cyg_uint8  *obase;
    cyg_int32  osize;
    cyg_uint8  *bottom;
    cyg_uint8  *top;
    cyg_int32  alignment;
    cyg_int32  freemem;
    cyg_mempoolt mempoolt;
};

struct cyg_mempool_fix {
    cyg_uint32 *bitmap;
    cyg_int32 maptop;
    cyg_uint8  *mempool;
    cyg_int32 numblocks;
    cyg_int32 freeblocks;
    cyg_int32 blocksize;
    cyg_int32 firstfree;
    cyg_uint8  *top;
    cyg_mempoolt mempoolt;
};

#endif /* ifndef CYGONCE_MEMALLOC_KAPIDATA_H */
/* EOF kapidata.h */
