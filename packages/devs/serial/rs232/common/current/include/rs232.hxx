#ifndef CYGONCE_DEVS_SERIAL_RS232_COMMON_RS232_HXX
#define CYGONCE_DEVS_SERIAL_RS232_COMMON_RS232_HXX

//==========================================================================
//
//	rs232.hxx
//
//	Header file for the generic serial device 
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
// Date:	1998-04-20
// Purpose:	Define Cyg_Device_Serial data structure
// Description:	This file contains the C++ class declaration of 
//              Cyg_Device_Serial_RS232, which is the base class for all 
//              RS232 serial devices.
// Usage:	#include "cyg/devs/serial/rs232/common/rs232.hxx"
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>  	// assertion macros
#include <cyg/infra/cyg_trac.h>		// tracing macros

#include <cyg/devs/common/queuet.hxx>
#include <cyg/devs/common/serial.hxx>

#include <cyg/devs/serial/rs232/common/rs232.h>

// -------------------------------------------------------------------------
// Cyg_Device_Serial_RS232

class Cyg_Device_Serial_RS232
    : public Cyg_Device_Serial
{
    // nothing extra
};

#endif  // CYGONCE_DEVS_SERIAL_RS232_COMMON_RS232_HXX
// End of rs232.hxx

