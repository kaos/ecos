//=============================================================================
//
//      plf_stub.c
//
//      Platform specific code for GDB stub support.
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
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-02-12
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_if.h>

#define L1 "eCos ROM   " __TIME__ "\n"
#define L2 __DATE__ "\n"

void
hal_plf_stub_init(void)
{
#if defined(CYGSEM_HAL_ROM_MONITOR) && defined(CYGSEM_HAL_VIRTUAL_VECTOR)
    // Put stub build date on the LCD.
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM()(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    CYGACC_CALL_IF_SET_CONSOLE_COMM()(2);

    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_WRITE(*comm)(CYGACC_COMM_IF_CH_DATA(*comm), L1, strlen(L1));
    CYGACC_COMM_IF_WRITE(*comm)(CYGACC_COMM_IF_CH_DATA(*comm), L2, strlen(L2));

    CYGACC_CALL_IF_SET_CONSOLE_COMM()(cur);
#endif
}

//-----------------------------------------------------------------------------
// End of plf_stub.c
