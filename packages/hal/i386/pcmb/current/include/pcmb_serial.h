#ifndef CYGONCE_HAL_PCMB_SERIAL_H
#define CYGONCE_HAL_PCMB_SERIAL_H

//==========================================================================
//
//      pcmb_serial.h
//
//      i386/pc Motherboard serial device support
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
// Author(s):    nickg
// Contributors: 
// Date:         2001-03-07
// Purpose:      PC serial support
// Description:  
//               
//               
//               
//               
//               
//              
// Usage:
//               #include <cyg/hal/pcmb_serial.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_i386.h>
#include <pkgconf/hal_i386_pcmb.h>

#include <cyg/infra/cyg_type.h>

//---------------------------------------------------------------------------

//=============================================================================

typedef struct {
    cyg_uint16  base;
    cyg_int32   msec_timeout;
    cyg_int32   isr_vector;
} channel_data_t;

__externC channel_data_t pc_ser_channels[];

//=============================================================================

__externC void cyg_hal_plf_serial_init(void);

#ifdef CYGSEM_HAL_I386_PCMB_SCREEN_SUPPORT

__externC void cyg_hal_plf_screen_init(void);

#endif

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PCMB_SERIAL_H
// End of pcmb_serial.h
