#ifndef CYGONCE_HAL_MM_H
#define CYGONCE_HAL_MM_H

//=============================================================================
//
//      hal_mm.h
//
//      ARM SA11x0 MM common definitions 
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
// Author(s):    gthomas
// Contributors: hmt
//               Travis C. Furrer <furrer@mit.edu>
// Date:         2000-12-13
// Purpose:      ARM SA11x0 MM common definitions
// Description:  The macros defined here provide common definitions for
//		 memory management initialization.
// Usage:
//               #include <cyg/hal/hal_mm.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================


// -------------------------------------------------------------------------
// MMU initialization:
// 
// These structures are laid down in memory to define the translation
// table.
// 

/*
 * SA-1100 Translation Table Base Bit Masks
 */
#define ARM_TRANSLATION_TABLE_MASK               0xFFFFC000

/*
 * SA-1100 Domain Access Control Bit Masks
 */
#define ARM_ACCESS_TYPE_NO_ACCESS(domain_num)    (0x0 << (domain_num)*2)
#define ARM_ACCESS_TYPE_CLIENT(domain_num)       (0x1 << (domain_num)*2)
#define ARM_ACCESS_TYPE_MANAGER(domain_num)      (0x3 << (domain_num)*2)

struct ARM_MMU_FIRST_LEVEL_FAULT {
    int id : 2;
    int sbz : 30;
};
#define ARM_MMU_FIRST_LEVEL_FAULT_ID 0x0

struct ARM_MMU_FIRST_LEVEL_PAGE_TABLE {
    int id : 2;
    int imp : 2;
    int domain : 4;
    int sbz : 1;
    int base_address : 23;
};
#define ARM_MMU_FIRST_LEVEL_PAGE_TABLE_ID 0x1

struct ARM_MMU_FIRST_LEVEL_SECTION {
    int id : 2;
    int b : 1;
    int c : 1;
    int imp : 1;
    int domain : 4;
    int sbz0 : 1;
    int ap : 2;
    int sbz1 : 8;
    int base_address : 12;
};
#define ARM_MMU_FIRST_LEVEL_SECTION_ID 0x2

struct ARM_MMU_FIRST_LEVEL_RESERVED {
    int id : 2;
    int sbz : 30;
};
#define ARM_MMU_FIRST_LEVEL_RESERVED_ID 0x3

#define ARM_MMU_FIRST_LEVEL_DESCRIPTOR_ADDRESS(ttb_base, table_index) \
   (unsigned long *)((unsigned long)(ttb_base) + ((table_index) << 2))

#define ARM_FIRST_LEVEL_PAGE_TABLE_SIZE 0x4000

#define ARM_MMU_SECTION(ttb_base, actual_base, virtual_base,              \
                        cacheable, bufferable, perm)                      \
    CYG_MACRO_START                                                       \
        register union ARM_MMU_FIRST_LEVEL_DESCRIPTOR desc;               \
                                                                          \
        desc.word = 0;                                                    \
        desc.section.id = ARM_MMU_FIRST_LEVEL_SECTION_ID;                 \
        desc.section.domain = 0;                                          \
        desc.section.c = (cacheable);                                     \
        desc.section.b = (bufferable);                                    \
        desc.section.ap = (perm);                                         \
        desc.section.base_address = (actual_base);                        \
        *ARM_MMU_FIRST_LEVEL_DESCRIPTOR_ADDRESS(ttb_base, (virtual_base)) \
                            = desc.word;                                  \
    CYG_MACRO_END

#define X_ARM_MMU_SECTION(abase,vbase,size,cache,buff,access)      \
    { int i; int j = abase; int k = vbase;                         \
      for (i = size; i > 0 ; i--,j++,k++)                          \
      {                                                            \
        ARM_MMU_SECTION(ttb_base, j, k, cache, buff, access);      \
      }                                                            \
    }

union ARM_MMU_FIRST_LEVEL_DESCRIPTOR {
    unsigned long word;
    struct ARM_MMU_FIRST_LEVEL_FAULT fault;
    struct ARM_MMU_FIRST_LEVEL_PAGE_TABLE page_table;
    struct ARM_MMU_FIRST_LEVEL_SECTION section;
    struct ARM_MMU_FIRST_LEVEL_RESERVED reserved;
};

#define ARM_UNCACHEABLE                         0
#define ARM_CACHEABLE                           1
#define ARM_UNBUFFERABLE                        0
#define ARM_BUFFERABLE                          1

#define ARM_ACCESS_PERM_NONE_NONE               0
#define ARM_ACCESS_PERM_RO_NONE                 0
#define ARM_ACCESS_PERM_RO_RO                   0
#define ARM_ACCESS_PERM_RW_NONE                 1
#define ARM_ACCESS_PERM_RW_RO                   2
#define ARM_ACCESS_PERM_RW_RW                   3

/*
 * Initialization for the Domain Access Control Register
 */
#define ARM_ACCESS_DACR_DEFAULT      (          \
        ARM_ACCESS_TYPE_MANAGER(0)    |         \
        ARM_ACCESS_TYPE_NO_ACCESS(1)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(2)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(3)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(4)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(5)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(6)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(7)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(8)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(9)  |         \
        ARM_ACCESS_TYPE_NO_ACCESS(10) |         \
        ARM_ACCESS_TYPE_NO_ACCESS(11) |         \
        ARM_ACCESS_TYPE_NO_ACCESS(12) |         \
        ARM_ACCESS_TYPE_NO_ACCESS(13) |         \
        ARM_ACCESS_TYPE_NO_ACCESS(14) |         \
        ARM_ACCESS_TYPE_NO_ACCESS(15)  )

// ------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_MM_H
// End of hal_mm.h





