#ifndef CYGONCE_HAL_BASETYPE_H
#define CYGONCE_HAL_BASETYPE_H

//=============================================================================
//
//      basetype.h
//
//      Standard types for this architecture.
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
// Author(s):   nickg
// Contributors:  nickg
// Date:        1998-02-05
// Purpose:     Define architecture base types.
// Usage:       Included by <cyg/infra/cyg_types.h>, do not use directly
//              
//####DESCRIPTIONEND####
//

#include <pkgconf/hal.h>

//-----------------------------------------------------------------------------
// Characterize the architecture

#if defined(CYGPKG_HAL_MIPS_MSBFIRST)
# define CYG_BYTEORDER           CYG_MSBFIRST    // Little endian
#elif defined(CYGPKG_HAL_MIPS_LSBFIRST)
# define CYG_BYTEORDER           CYG_LSBFIRST    // Big endian
#else
# error MIPS endianess not defined by configuration
#endif

//-----------------------------------------------------------------------------
// MIPS does not usually use labels with undersores. Some labels generated
// by the linker do, so add an underscore where required.

#define CYG_LABEL_NAME(_name_) _##_name_

//-----------------------------------------------------------------------------
// Define the standard variable sizes

// The MIPS architecture uses the default definitions of the base types,
// so we do not need to define any here.

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_BASETYPE_H
// End of basetype.h
