#ifndef CYGONCE_DEVS_GDB_1_HXX
#define CYGONCE_DEVS_GDB_1_HXX

//==========================================================================
//
//	gdb_1.hxx
//
//	Header file for the pseudo gdb device
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
// Date:	1998-04-22
// Purpose:	Define Cyg_Device_GDB_1 data structure
// Description:	This file contains the C++ class declaration of 
//		Cyg_Device_GDB_1. There can be only one as it 
//		inherits from a specific device.
// Usage:	#include <cyg/devs/gdb_1.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs.h>		// Device configureation file

#ifdef CYG_DEVICE_GDB_1
// Include Configureation options specific to the device
#include CYG_DEVICE_GDB_1
#include CYG_DEVICE_GDB_1_INCLUDE

#include <cyg/kernel/ktypes.h>
#include <cyg/kernel/mutex.hxx>

// -------------------------------------------------------------------------
// Cyg_Device_GDB_1

#define CYG_CLASS_DEVICE_GDB_1		Cyg_Device_GDB_1

class CYG_CLASS_DEVICE_GDB_1
    : public CYG_DEVICE_GDB_1_CLASS
{

#ifdef CYG_DEVICE_INTERNAL

private:
    /* Queue of write requests */
    Cyg_IORB * gdb_write_buffer_first;
    Cyg_IORB * gdb_write_buffer_last;

    static void iorb_setup(CYG_CLASS_DEVICE_GDB_1 *iorb);
    static void iorb_callback(Cyg_IORB *iorb);

    char gdb_write_buf[(CYGNUM_DEVICE_GDB_BUFFER_SIZE * 2 + 5)];	
    volatile cyg_uint32 gdb_write_length;
    Cyg_IORB gdb_write_iorb;

#endif

public:

    /* Constructor */
    CYG_CLASS_DEVICE_GDB_1();

    cyg_int32 io_write_assert(Cyg_IORB * iorb);
    cyg_int32 io_write_blocking(Cyg_IORB * iorb);
    cyg_int32 io_write_asynchronous(Cyg_IORB * iorb);

};

// Declare exported variables
#ifdef CYG_DEVICE_GDB_1_NAME
#ifndef CYG_DEVICE_GDB_1_DO_NOT_DECLARE
extern CYG_CLASS_DEVICE_GDB_1 CYG_DEVICE_GDB_1_NAME;
#endif
#endif

#endif  // CYG_DEVICE_GDB_1

#endif  // CYGONCE_DEVS_GDB_1_HXX
// End of gdb_1.hxx

