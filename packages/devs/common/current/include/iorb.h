#ifndef CYGONCE_DEVS_COMMON_IORB_H
#define CYGONCE_DEVS_COMMON_IORB_H

//==========================================================================
//
//	iorb.h
//
//	Header file for IORB data structure
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
// Purpose:	Define IORB data structure
// Description:	This file contains the C/C++ struct declaration of Cyg_IORB.
//		Cyg_IORB is the basic class for all device operations for
//		both the C and C++ API.
// Usage:	#include <cyg/devs/common/iorb.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>		// assertion macros
#include <cyg/infra/cyg_trac.h>		// tracing macros

// -------------------------------------------------------------------------
// Cyg_IORB

/*
 * This struct will be accessed by some ISR routines. Any field that is
 * modified by an ISR routine must be declared volatile. Currently this
 * includes next, ret_length, and resultstatus.
 * 
 * Note: For next this means the pointer next is volatile not the contents.
 */
typedef struct Cyg_IORB_t
{
#ifdef __cplusplus
    friend class Cyg_Device;

    private:
    Cyg_Device        * pdevice;

    // Accessor functions
    void set_pdevice(Cyg_Device * device) { pdevice = device; }

    public:

    // Accessor functions
    Cyg_Device *
    get_pdevice(void) { return(pdevice); }

#else
    // Make sure the C and C++ versions are the same size;
    CYG_ADDRESS         pdevice;
#endif

    struct Cyg_IORB_t * volatile next;		

    void              * buffer;         // ptr to buffer
    cyg_uint32		buffer_length;  // length of buffer
    volatile cyg_uint32 xferred_length; // length of buffer xferd

    cyg_uint8       	opcode;         // operation code + options
    volatile cyg_uint8 	status;         // state, result + error code

    CYG_ADDRWORD        callback_data;	// data field for callback routine
    void            	(*callback)( struct Cyg_IORB_t * piorb);
} Cyg_IORB;

// opflags codes
#define CYG_IORB_NOBLOCK  0x01		// opcode for special serial reads

// Resultstatus codes
#define CYG_IORB_OK	  0x00
#define CYG_IORB_CANCELED 0x02

#endif  // CYGONCE_DEVS_COMMON_IORB_H
// End of iorb.h

