#ifndef CYGONCE_HAL_PPC_QUICC_SMC1_H
#define CYGONCE_HAL_PPC_QUICC_SMC1_H
//=============================================================================
//
//      quicc_smc1.h
//
//      PowerPC QUICC basic Serial IO using port SMC1
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Red Hat
// Contributors: hmt
// Date:         1999-06-08
// Purpose:      Provide basic Serial IO for MBX board
// Description:  Serial IO for MBX boards which connect their debug channel
//               to SMC1; or any QUICC user who wants to use SMC1.
// Usage:
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/quicc/ppc8xx.h>             // FIXME: bad, but need eppc_base


#if !defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)
// This one should only be used by old-stub compatibility code!
externC void cyg_hal_plf_serial_init_channel(void);
#endif

externC void cyg_hal_plf_serial_init(void);
externC void cyg_hal_plf_serial_putc(void* __ch_data, cyg_uint8 __ch);
externC cyg_uint8 cyg_hal_plf_serial_getc(void* __ch_data);

#endif /* CYGONCE_HAL_PPC_QUICC_SMC1_H */
