/*=================================================================
//
//        context.c
//
//        HAL Thread context handling test
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  nickg
// Date:          1998-10-07
//####DESCRIPTIONEND####
*/

#include <pkgconf/hal.h>

#include <pkgconf/infra.h>

#include <cyg/infra/testcase.h>

#include <cyg/infra/cyg_trac.h>
#include <cyg/hal/hal_arch.h>

#define CYG_TRACE_USER_BOOL 1

// -------------------------------------------------------------------------

#define THREADS         4
#define STACKSIZE       (2*1024)

char stack[THREADS][STACKSIZE];

CYG_ADDRWORD sp[THREADS];

cyg_count32 switches = 0;

// -------------------------------------------------------------------------

void entry0( CYG_ADDRWORD arg )
{
    CYG_TRACE1B("Thread %d started\n", arg );

    while( switches < 1000 )
    {
        HAL_THREAD_SWITCH_CONTEXT( &sp[arg], &sp[(arg+1) % THREADS] );

        CYG_TRACE1B("Thread %d resumed\n", arg );

        switches++;
    }

    CYG_TEST_PASS_FINISH("HAL Context test");
    
}

// -------------------------------------------------------------------------

void context_main(void)
{
    int i;
    
    CYG_TEST_INIT();

    // Init all thread contexts:
    
    for( i = 0 ; i < THREADS; i++ )
    {
        sp[i] = (CYG_ADDRWORD)stack[i]+STACKSIZE;
        
        HAL_THREAD_INIT_CONTEXT( sp[i], i, entry0, i*0x01010000 );
    }

    // Load the first thread.
    
    HAL_THREAD_LOAD_CONTEXT( &sp[0] );
}

// -------------------------------------------------------------------------

externC void
cyg_start( void )
{
    context_main();
}

// -------------------------------------------------------------------------
/* EOF context.c */
