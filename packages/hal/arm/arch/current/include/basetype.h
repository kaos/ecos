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
// Author(s):   nickg, gthomas
// Contributors:  nickg, gthomas
// Date:        1998-09-11
// Purpose:     Define architecture base types.
// Usage:       Included by "cyg_type.h", do not use directly

//              
//####DESCRIPTIONEND####
//

//-----------------------------------------------------------------------------
// Characterize the architecture

#ifdef __ARMEB__
# define CYG_BYTEORDER           CYG_MSBFIRST    // Big endian
#else
# define CYG_BYTEORDER           CYG_LSBFIRST    // Little endian
#endif
#define CYG_DOUBLE_BYTEORDER    CYG_MSBFIRST    // Big? endian

//-----------------------------------------------------------------------------
// ARM does not usually use labels with underscores.

//#define CYG_LABEL_NAME(_name_) _name_

//-----------------------------------------------------------------------------
// Override the alignment definitions from cyg_type.h. ARM only alows 4 
// bytes alignment where as the default is 8 byte.

#define CYGARC_ALIGNMENT 4
#define CYGARC_P2ALIGNMENT 2

//-----------------------------------------------------------------------------
// Define the standard variable sizes

// The ARM architecture uses the default definitions of the base types,
// so we do not need to define any here.

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_BASETYPE_H
// End of basetype.h
