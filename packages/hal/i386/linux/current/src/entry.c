//==========================================================================
//
//      entry.c
//
//      Entry code for Linux synthetic target.
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
// Author(s):   proven
// Contributors:proven, jskov, jlarmour
// Date:        1999-01-06
// Purpose:     Entry point for Linux synthetic target.
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include CYGHWR_MEMORY_LAYOUT_H

externC void cyg_hal_isr_init( void );
externC void cyg_hal_invoke_constructors( void );
externC void cyg_start( void );
externC void cyg_hal_hardware_init( void );
externC void *cyg_hal_sys_brk(void *newbrk);

void _linux_entry( void )
{
    void *oldbrk=NULL;
    void *newbrk=(void *)(CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE);
    // Should get argc argv
    cyg_hal_hardware_init();
    cyg_hal_isr_init();
    // unconditionally set memory regino to the size specified in the
    // memory layout
    oldbrk = cyg_hal_sys_brk(newbrk);
    // should check 
    CYG_ASSERT( oldbrk>=newbrk, "brk() failed" );
    cyg_hal_invoke_constructors();
    cyg_start();
}

//-----------------------------------------------------------------------------
// End of entry.c
