/*=============================================================================
//
//	devs/diag.c
//
//	Redirect diag output to the appropriate device
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
// Author(s): 	proven
// Contributors:	proven
// Date:	1998-05-11
// Purpose:     Kernel diagnostic output
// Description:	Implementations of kernel diagnostic routines.
//
//####DESCRIPTIONEND####
//
//==========================================================================*/

#include <pkgconf/devices.h>	// Include all the possible devices

/*---------------------------------------------------------------------------*/

#ifdef CYGDBG_INFRA_DIAG_USE_DEVICE
#if defined(CYG_DEVICE_DIAG_CLASS) && defined(CYG_DEVICE_DIAG_NAME)

extern CYG_DEVICE_DIAG_CLASS CYG_DEVICE_DIAG_NAME;

/*---------------------------------------------------------------------------*/
/* Write single char to output                                               */

cyg_ucount8 sync_mode = 0;

externC void diag_device_write_char(char c)
{
    Cyg_IORB iorb;

    iorb.buffer = &c;
    iorb.buffer_length = 1;
    if (sync_mode) {
	CYG_DEVICE_DIAG_NAME.io_write_assert(&iorb);
    } else {
        CYG_DEVICE_DIAG_NAME.io_write_blocking(&iorb);
    }
}

externC void diag_device_start_sync()
{
    sync_mode = 1;
}

externC void diag_device_end_sync()
{
    sync_mode = 0;
}

#else
#error "Must define CYG_DEVICE_DIAG_CLASS and CYG_DEVICE_DIAG_NAME"
#endif // ! (defined(CYG_DEVICE_DIAG_CLASS) && defined(CYG_DEVICE_DIAG_NAME))
#endif // CYGDBG_INFRA_DIAG_USE_DEVICE

/*---------------------------------------------------------------------------*/
/* End of devs/diag.c */
