#ifndef CYGONCE_HAL_BASETYPE_H
#define CYGONCE_HAL_BASETYPE_H

//=============================================================================
//
//      basetype.h
//
//      Standard types for x86 synthetic target.
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
// Author(s):   proven
// Contributors:proven
// Date:        1998-10-02
// Purpose:     Define architecture base types.
// Usage:       Included by "cyg_type.h", do not use directly
//
//####DESCRIPTIONEND####
//
//=============================================================================

//-----------------------------------------------------------------------------
// Characterize the architecture

#define CYG_BYTEORDER           CYG_LSBFIRST    // Little endian

//-----------------------------------------------------------------------------
// I386 does not usually use labels with underscores.

//#define CYG_LABEL_NAME(_name_) _name_

//-----------------------------------------------------------------------------
// Override the alignment definitions from cyg_type.h. x86 requires
// 32 *byte* alignment because gcc sometimes tries to be clever with
// aligning things on cache lines.

#define CYGARC_ALIGNMENT 32
#define CYGARC_P2ALIGNMENT 5

//-----------------------------------------------------------------------------
// Define the standard variable sizes

// The ix86 architecture uses the default definitions of the base types,
// so we do not need to define any here.

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_BASETYPE_H
// End of basetype.h
