//=============================================================================
//
//      sh4_scif.h
//
//      Simple driver for the sh4 Serial Communication Interface with FIFO
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
// Contributors:Haruki Kashiwaya
// Date:        2000-08-09
// Description: Simple driver for the SH Serial Communication Interface
//              The driver can be used for the SCIF modules.
//              Clients of this file can configure the behavior with:
//              CYGNUM_SCIF_PORTS: number of SCI ports
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGNUM_HAL_SH_SH4_SCIF_PORTS

//--------------------------------------------------------------------------
// Exported functions

externC cyg_uint8 cyg_hal_plf_scif_getc(void* __ch_data);
externC void cyg_hal_plf_scif_putc(void* __ch_data, cyg_uint8 c);
void cyg_hal_plf_scif_init(int scif_index, int comm_index, 
                           int rcv_vect, cyg_uint8* base);

#ifdef CYGPRI_HAL_SH_SH4_SCIF_PRIVATE

//--------------------------------------------------------------------------
// SCIF register offsets
#define _REG_SCSMR  0x00
#define _REG_SCBRR  0x04
#define _REG_SCSCR  0x08
#define _REG_SCFTDR 0x0c
#define _REG_SCFSR  0x10
#define _REG_SCFRDR 0x14
#define _REG_SCFCR  0x18
#define _REG_SCFDR  0x1c
#define _REG_SCSPTR 0x20
#define _REG_SCLSR  0x24

//--------------------------------------------------------------------------

typedef struct {
    cyg_uint8* base;
    cyg_int32 msec_timeout;
    int isr_vector;
} channel_data_t;

//--------------------------------------------------------------------------

#if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)
// This one should only be used by old-stub compatibility code!
externC void cyg_hal_plf_scif_init_channel(const channel_data_t* chan);
#warning "You should not be using anything but vv diag"
#endif

#endif // CYGPRI_HAL_SH_SH4_SCIF_PRIVATE

#endif // CYGNUM_HAL_SH_SH4_SCIF_PORTS
//-----------------------------------------------------------------------------
// end of sh4_scif.h
