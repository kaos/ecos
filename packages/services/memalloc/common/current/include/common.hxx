#ifndef CYGONCE_MEMALLOC_COMMON_HXX
#define CYGONCE_MEMALLOC_COMMON_HXX

/*==========================================================================
//
//      common.hxx
//
//      Shared definitions used by memory allocators
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
// Purpose:      Shared definitions used by memory allocators
// Description:  
// Usage:        #include <cyg/memalloc/common.hxx>
//              
//
//####DESCRIPTIONEND####
//
//========================================================================*/

/* CONFIGURATION */

#include <pkgconf/memalloc.h>

/* TYPE DEFINITIONS */

// struct Cyg_Mempool_Status is returned by the get_status() method of
// standard eCos memory allocators. After return from get_status(), any
// field of type T may be set to ((T)-1) to indicate that the information
// is not available or not applicable to this allocator.


class Cyg_Mempool_Status {
public:
    const cyg_uint8 *arenabase; // base address of entire pool
    cyg_int32   arenasize;      // total size of entire pool
    cyg_int32   freeblocks;     // number of chunks free for use
    cyg_int32   totalallocated; // total allocated space in bytes
    cyg_int32   totalfree;      // total space in bytes not in use
    cyg_int32   blocksize;      // block size if fixed block
    cyg_int32   maxfree;        // size of largest unused block
    cyg_int8    waiting;        // are there any threads waiting for memory?
    const cyg_uint8 *origbase;  // address of original region used when pool
                                // created
    cyg_int32   origsize;       // size of original region used when pool
                                // created

    // maxoverhead is the *maximum* per-allocation overhead imposed by
    // the allocator implementation. Note: this is rarely the typical
    // overhead which often depends on the size of the allocation requested.
    // It includes overhead due to alignment constraints. For example, if
    // maxfree and maxoverhead are available for this allocator, then an
    // allocation request of (maxfree-maxoverhead) bytes must always succeed
    // Unless maxoverhead is set to -1 of course, in which case the allocator
    // does not support reporting this information.

    cyg_int8    maxoverhead;    

    void
    init() {
        arenabase       = (const cyg_uint8 *)-1;
        arenasize       = -1;
        freeblocks      = -1;
        totalallocated  = -1;
        totalfree       = -1;
        blocksize       = -1;
        maxfree         = -1;
        waiting         = -1;
        origbase        = (const cyg_uint8 *)-1;
        origsize        = -1;
        maxoverhead     = -1;
    }

    // constructor
    Cyg_Mempool_Status() { init(); }
};

// Flags to pass to get_status() methods to tell it which stat(s) is/are
// being requested

#define CYG_MEMPOOL_STAT_ARENABASE       (1<<0)
#define CYG_MEMPOOL_STAT_ARENASIZE       (1<<1)
#define CYG_MEMPOOL_STAT_FREEBLOCKS      (1<<2)
#define CYG_MEMPOOL_STAT_TOTALALLOCATED  (1<<3)
#define CYG_MEMPOOL_STAT_TOTALFREE       (1<<4)
#define CYG_MEMPOOL_STAT_BLOCKSIZE       (1<<5)
#define CYG_MEMPOOL_STAT_MAXFREE         (1<<6)
#define CYG_MEMPOOL_STAT_WAITING         (1<<7)
#define CYG_MEMPOOL_STAT_ORIGBASE        (1<<9)
#define CYG_MEMPOOL_STAT_ORIGSIZE        (1<<10)
#define CYG_MEMPOOL_STAT_MAXOVERHEAD     (1<<11)

// And an opaque type for any arguments with these flags
typedef cyg_uint16 cyg_mempool_status_flag_t;


#endif /* ifndef CYGONCE_MEMALLOC_COMMON_HXX */
/* EOF common.hxx */
