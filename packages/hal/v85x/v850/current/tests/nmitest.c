//==========================================================================
//
//      nmitest.c
//
//      V850 NMI test program
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
// Copyright (C) 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: jlarmour
// Date:         2001-02-01
// Purpose:      Test NMI function
// Description:  This test allows the NMI functionality to be tested.
//               It is not built by default.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>

#include <cyg/kernel/kapi.h>

int nmicount;
char stack[4096];
static cyg_handle_t thr0h;
static cyg_thread thr0;

void
nmi_handler(cyg_addrword_t data, cyg_code_t exception_number,
            cyg_addrword_t info)
{
    nmicount++;
}

static void
entry0( cyg_addrword_t data )
{
    int oldnmicount=0;

    cyg_exception_set_handler( CYGNUM_HAL_VECTOR_NMI,
                               &nmi_handler,
                               0,
                               NULL,
                               NULL );
    for (;;) {
        if ( oldnmicount != nmicount ) {
            diag_printf("Caught NMI\n");
            oldnmicount = nmicount;
        }
    }
}

void
cyg_user_start(void)
{
    cyg_thread_create(4, entry0, (cyg_addrword_t)0, "thread 0",
                      (void *)&stack[0], sizeof(stack), &thr0h, &thr0 );
    cyg_thread_resume(thr0h);
}

// EOF nmitest.c
