#ifndef CYGONCE_MEMALLOC_KAPI_H
#define CYGONCE_MEMALLOC_KAPI_H

/*==========================================================================
//
//      kapi.h
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
// Purpose:      Memory allocator portion of kernel C API
// Description:  This is intentionally only to be included from
//               <cyg/kernel/kapi.h>
// Usage:        This file should not be used directly - instead it should
//               be used via <cyg/kernel/kapi.h>
//              
//
//####DESCRIPTIONEND####
//
//========================================================================*/

/* CONFIGURATION */

#include <pkgconf/memalloc.h>

/* TYPE DEFINITIONS */

struct cyg_mempool_var;
typedef struct cyg_mempool_var cyg_mempool_var;

struct cyg_mempool_fix;
typedef struct cyg_mempool_fix cyg_mempool_fix;

/*-----------------------------------------------------------------------*/
/* Memory pools                                                          */

/* There are two sorts of memory pools.  A variable size memory pool
   is for allocating blocks of any size.  A fixed size memory pool, has
   the block size specified when the pool is created, and only provides
   blocks of that size.  */

/* Create a variable size memory pool */
void cyg_mempool_var_create(
    void            *base,              /* base of memory to use for pool */
    cyg_int32       size,               /* size of memory in bytes        */
    cyg_handle_t    *handle,            /* returned handle of memory pool */
    cyg_mempool_var *var                /* space to put pool structure in */
    );

/* Delete variable size memory pool */
void cyg_mempool_var_delete(cyg_handle_t varpool);

#ifdef CYGSEM_MEMALLOC_ALLOCATOR_VARIABLE_THREADAWARE

/* Allocates a block of length size.  This waits if the memory is not
   currently available.  */
void *cyg_mempool_var_alloc(cyg_handle_t varpool, cyg_int32 size);

# ifdef CYGFUN_KERNEL_THREADS_TIMER

/* Allocates a block of length size.  This waits until abstime,
   if the memory is not already available.  NULL is returned if
   no memory is available. */
void *cyg_mempool_var_timed_alloc(
    cyg_handle_t     varpool,
    cyg_int32        size,
    cyg_tick_count_t abstime);

# endif
#endif

/* Allocates a block of length size.  NULL is returned if no memory is
   available. */
void *cyg_mempool_var_try_alloc(
    cyg_handle_t varpool,
    cyg_int32    size);

/* Frees memory back into variable size pool. */
void cyg_mempool_var_free(cyg_handle_t varpool, void *p);

/* Returns true if there are any threads waiting for memory in the
   given memory pool. */
cyg_bool_t cyg_mempool_var_waiting(cyg_handle_t varpool);

typedef struct {
    cyg_int32 totalmem;
    cyg_int32 freemem;
    void      *base;
    cyg_int32 size;
    cyg_int32 blocksize;
    cyg_int32 maxfree;                  // The largest free block
} cyg_mempool_info;

/* Puts information about a variable memory pool into the structure
   provided. */
void cyg_mempool_var_get_info(cyg_handle_t varpool, cyg_mempool_info *info);

/* Create a fixed size memory pool */
void cyg_mempool_fix_create(
    void            *base,              // base of memory to use for pool
    cyg_int32       size,               // size of memory in byte
    cyg_int32       blocksize,          // size of allocation in bytes
    cyg_handle_t    *handle,            // handle of memory pool
    cyg_mempool_fix *fix                // space to put pool structure in
    );

/* Delete fixed size memory pool */
void cyg_mempool_fix_delete(cyg_handle_t fixpool);

#ifdef CYGSEM_MEMALLOC_ALLOCATOR_FIXED_THREADAWARE
/* Allocates a block.  This waits if the memory is not
   currently available.  */
void *cyg_mempool_fix_alloc(cyg_handle_t fixpool);

# ifdef CYGFUN_KERNEL_THREADS_TIMER

/* Allocates a block.  This waits until abstime, if the memory
   is not already available.  NULL is returned if no memory is
   available. */
void *cyg_mempool_fix_timed_alloc(
    cyg_handle_t     fixpool,
    cyg_tick_count_t abstime);

# endif
#endif

/* Allocates a block.  NULL is returned if no memory is available. */
void *cyg_mempool_fix_try_alloc(cyg_handle_t fixpool);

/* Frees memory back into fixed size pool. */
void cyg_mempool_fix_free(cyg_handle_t fixpool, void *p);

/* Returns true if there are any threads waiting for memory in the
   given memory pool. */
cyg_bool_t cyg_mempool_fix_waiting(cyg_handle_t fixpool);

/* Puts information about a variable memory pool into the structure
   provided. */
void cyg_mempool_fix_get_info(cyg_handle_t fixpool, cyg_mempool_info *info);



#endif /* ifndef CYGONCE_MEMALLOC_KAPI_H */
/* EOF kapi.h */
