#ifndef CYGONCE_HAL_PLF_INTR_H
#define CYGONCE_HAL_PLF_INTR_H
//==========================================================================
//
//      plf_intr.h
//
//      i386/PC Interrupt and clock support
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
// Author(s):    proven
// Contributors: proven, jskov, pjo
// Date:         1999-10-15
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//               This file contains info about interrupts and
//               peripherals that are common on all PCs; for example,
//               the clock always activates irq 0 and would therefore
//               be listed here; an ethernet card is configured for
//               the individual system and would be in plf_intr.h
//               instead.
//              
// Usage:
//               #include <cyg/hal/plf_intr.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_i386.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/pcmb_intr.h>

//----------------------------------------------------------------------------
// Reset.

#define HAL_PLATFORM_RESET()             hal_pc_reset()

#define HAL_PLATFORM_RESET_ENTRY	 &hal_pc_reset

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLF_INTR_H
// End of plf_intr.h
