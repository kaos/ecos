#ifndef CYGONCE_HAL_MMU_H
#define CYGONCE_HAL_MMU_H

//==========================================================================
//
//      hal_mmu.h
//
//      MMU definitions
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-05-10
// Purpose:      Define MMU for ARM
// Usage:        #include <cyg/hal/hal_mmu.h>

//              
//####DESCRIPTIONEND####
//
//==========================================================================

#define MMU_L1_TYPE         0x03  // Descriptor type
#define MMU_L1_TYPE_Fault   0x00  // Invalid
#define MMU_L1_TYPE_Page    0x11  // Individual page mapping
#define MMU_L1_TYPE_Section 0x12  // Mapping for 1M segment

#define MMU_L2_TYPE         0x03  // Descriptor type
#define MMU_L2_TYPE_Fault   0x00  // Invalid data
#define MMU_L2_TYPE_Large   0x01  // Large page (64K)
#define MMU_L2_TYPE_Small   0x02  // Small page (4K)

#define MMU_Bufferable      0x04  // Data can use write-buffer
#define MMU_Cacheable       0x08  // Data can use cache

#define MMU_AP_Limited     0x000  // Limited access
#define MMU_AP_Supervisor  0x400  // Supervisor RW, User none
#define MMU_AP_UserRead    0x800  // Supervisor RW, User read only
#define MMU_AP_Any         0xC00  // Supervisor RW, User RW

#define MMU_AP_ap0_Any     0x030
#define MMU_AP_ap1_Any     0x0C0
#define MMU_AP_ap2_Any     0x300
#define MMU_AP_ap3_Any     0xC00
#define MMU_AP_All (MMU_AP_ap0_Any|MMU_AP_ap1_Any|MMU_AP_ap2_Any|MMU_AP_ap3_Any)

#define MMU_DOMAIN(x)      ((x)<<5)

#define MMU_PAGE_SIZE      0x1000
#define MMU_SECTION_SIZE   0x100000

#define MMU_CP               p15      // Co-processor ID 
#define MMU_Control          c1       // Control register
#define MMU_Base             c2       // Page tables base
#define MMU_DomainAccess     c3       // Domain access control
#define MMU_FaultStatus      c5       // Fault status register
#define MMU_FaultAddress     c6       // Fault Address
#define MMU_InvalidateCache  c7       // Invalidate cache data
#define MMU_TLB              c8       // Translation Lookaside Buffer

// These seem to be 710 specific
#define MMU_FlushTLB         c5
#define MMU_FlushIDC         c7

#define MMU_Control_M  0x001    // Enable MMU
#define MMU_Control_A  0x002    // Enable address alignment faults
#define MMU_Control_C  0x004    // Enable cache
#define MMU_Control_W  0x008    // Enable write-buffer
#define MMU_Control_P  0x010    // Compatability: 32 bit code
#define MMU_Control_D  0x020    // Compatability: 32 bit data
#define MMU_Control_L  0x040    // Compatability:
#define MMU_Control_B  0x080    // Enable Big-Endian
#define MMU_Control_S  0x100    // Enable system protection
#define MMU_Control_R  0x200    // Enable ROM protection
#define MMU_Control_I  0x1000   // Enable Instruction cache
#define MMU_Control_X  0x2000   // Set interrupt vectors at 0xFFFF0000
#define MMU_Control_Init (MMU_Control_P|MMU_Control_D|MMU_Control_L)

//-----------------------------------------------------------------------------

#endif // CYGONCE_HAL_MMU_H
// End of hal_mmu.h
