#ifndef CYGONCE_KERNEL_ERRORS_H
#define CYGONCE_KERNEL_ERRORS_H

//==========================================================================
//
//	errors.h
//
//	Error values from kernel
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
// Date:	1997-10-07
// Purpose:	Define error codes
// Description:	Error codes returned by various bits of the kernel.
// Usage:       #include <cyg/kernel/errors.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================

// General successful result:

#define CYGERR_OK                               0

// -------------------------------------------------------------------------
// Define base of codes:

#define CYGERR_KERNEL_BASE                      0xEE000000

// -------------------------------------------------------------------------
// Thread related errors

#define CYGERR_KERNEL_THREAD_BASE               (CYGERR_KERNEL_BASE+0x00010000)

#define CYGERR_KERNEL_THREAD_PRIORITY_INVALID   (CYGERR_KERNEL_THREAD_BASE+1)
#define CYGERR_KERNEL_THREAD_PRIORITY_DUPLICATE (CYGERR_KERNEL_THREAD_BASE+2)

// -------------------------------------------------------------------------
// Interrupt related errors

#define CYGERR_KERNEL_INTR_BASE                 (CYGERR_KERNEL_BASE+0x00020000)

#define CYGERR_KERNEL_INTR_VECTOR_INVALID       (CYGERR_KERNEL_INTR_BASE+1)

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_ERRORS_H
// EOF errors.h
