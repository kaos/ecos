//=============================================================================
//
//      sh3_sci.h
//
//      Simple driver for the SH Serial Communication Interface (SCI)
//
//=============================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-05-17
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGNUM_HAL_SH_SH3_SCI_PORTS

//--------------------------------------------------------------------------
// Exported functions

externC cyg_uint8 cyg_hal_plf_sci_getc(void* __ch_data);
externC void cyg_hal_plf_sci_putc(void* __ch_data, cyg_uint8 c);
externC void cyg_hal_plf_sci_init(int sci_index, int comm_index, 
                                  int rcv_vect, cyg_uint8* base);


#ifdef CYGPRI_HAL_SH_SH3_SCI_PRIVATE

//--------------------------------------------------------------------------
// SCI register offsets
#if (CYGARC_SH_MOD_SCI >= 2)
# define _REG_SCSPTR             -0x4 // serial port register
#endif
#define _REG_SCSMR                0x0 // serial mode register
#define _REG_SCBRR                0x2 // bit rate register
#define _REG_SCSCR                0x4 // serial control register
#define _REG_SCTDR                0x6 // transmit data register
#define _REG_SCSSR                0x8 // serial status register
#define _REG_SCRDR                0xa // receive data register

//--------------------------------------------------------------------------

typedef struct {
    cyg_uint8* base;
    cyg_int32 msec_timeout;
    int isr_vector;
} channel_data_t;

//--------------------------------------------------------------------------

#if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)
// This one should only be used by old-stub compatibility code!
externC void cyg_hal_plf_sci_init_channel(const channel_data_t* chan);
#warning "You should not be using anything but vv diag"
#endif

#endif // CYGPRI_HAL_SH_SH3_SCI_PRIVATE

#endif // CYGNUM_HAL_SH_SH3_SCI_PORTS
//-----------------------------------------------------------------------------
// end of sh3_sci.h
