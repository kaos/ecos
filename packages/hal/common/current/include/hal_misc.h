#ifndef CYGONCE_HAL_HAL_MISC_H
#define CYGONCE_HAL_HAL_MISC_H

//=============================================================================
//
//      hal_misc.h
//
//      HAL header for miscellaneous helper routines
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
// Author(s):   jskov
// Contributors:jskov
// Date:        2000-06-08
// Purpose:     Miscellaneous routines shared between HALs
// Usage:       #include <cyg/hal/hal_misc.h>
//                           
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>         // types & externC

externC cyg_bool cyg_hal_is_break(char *buf, int size);
externC void cyg_hal_user_break( CYG_ADDRWORD *regs );

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_MISC_H
// End of hal_misc.h
