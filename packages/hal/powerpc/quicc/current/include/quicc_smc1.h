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
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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

extern void cyg_quicc_smc1_uart_putchar(char ch);
extern int  cyg_quicc_smc1_uart_rcvchar(void);
extern void cyg_quicc_init_smc1(void);

#endif /* CYGONCE_HAL_PPC_QUICC_SMC1_H */
