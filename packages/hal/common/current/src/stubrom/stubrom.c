/*==========================================================================
//
//	stubrom.c
//
//	GDB Stub ROM main
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
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-06-02
// Purpose:     GDB Stub ROM main
// Description:	Main program for GDB stub ROM. It simply set the debug
//              traps and then breakpoints so we can talk to GDB.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <cyg/hal/hal_stub.h>

void cyg_start (void)
{
    set_debug_traps();

    for(;;) breakpoint();
    
    
}

// End of stubrom.c
