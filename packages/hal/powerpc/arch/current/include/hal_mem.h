#ifndef CYGONCE_HAL_MEM_H
#define CYGONCE_HAL_MEM_H

//=============================================================================
//
//      hal_mem.h
//
//      HAL memory control API
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:nickg, jskov, hmt
// Date:        2000-02-11
// Purpose:     Memory control API
// Description: The macros defined here provide the HAL APIs for handling
//              simple memory management operations.
// Usage:
//              #include <cyg/hal/hal_mem.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

//=============================================================================
// Memory mapping
typedef struct {
    CYG_ADDRESS  virtual_addr;
    CYG_ADDRESS  physical_addr;
    cyg_int32    size;
    cyg_uint8    flags;
} cyg_memdesc_t;

// each variant HAL must provide these functions for mapping/clearing
// (simple BAT/TLB) memory mappings.
externC int
cyg_hal_map_memory (int id,CYG_ADDRESS virt, CYG_ADDRESS phys, 
                    cyg_int32 size, cyg_uint8 flags);
externC void
cyg_hal_clear_MMU (void);

// each platform HAL must provide one of these to describe how memory
// should be mapped/cached, ideally weak aliased so that apps can override:
externC cyg_memdesc_t cyg_hal_mem_map[];

#define CYGARC_MEMDESC_CI       1       // cache inhibit
#define CYGARC_MEMDESC_GUARDED  2       // guarded

// these macros should ease that task, and ease any future extension of the
// structure (physical == virtual addresses):
#define CYGARC_MEMDESC_CACHE( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), 0 }

#define CYGARC_MEMDESC_NOCACHE( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), CYGARC_MEMDESC_CI }

#define CYGARC_MEMDESC_CACHEGUARD( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), CYGARC_MEMDESC_GUARDED }

#define CYGARC_MEMDESC_NOCACHEGUARD( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), CYGARC_MEMDESC_GUARDED|CYGARC_MEMDESC_CI }

#define CYGARC_MEMDESC_TABLE_END      {0, 0, 0, 0}
#define CYGARC_MEMDESC_TABLE          cyg_memdesc_t cyg_hal_mem_map[]
#define CYGARC_MEMDESC_EMPTY_TABLE    { CYGARC_MEMDESC_TABLE_END }

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_MEM_H
// End of hal_mem.h
