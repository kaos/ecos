#ifndef CYGONCE_IO_COMMON_IO_DIAG_H
#define CYGONCE_IO_COMMON_IO_DIAG_H
//=============================================================================
//
//	io_diag.h
//
//	Redirect diag output to the configured console device
//
//============================================================================
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
//============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	jskov
// Contributors:jskov
// Date:	1999-04-09
// Purpose:     Kernel diagnostic output
// Description:	Implementations of kernel diagnostic routines.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

externC void diag_device_init(void);
externC void diag_device_write_char(char c);
externC void diag_device_start_sync(void);
externC void diag_device_end_sync(void);

#endif // CYGONCE_IO_COMMON_IO_DIAG_H
//-----------------------------------------------------------------------------
// EOF io_diag.h
