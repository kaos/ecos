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
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:  nickg
// Date:        1997-09-08
// Purpose:     Define architecture base types.
// Usage:       Included by "cyg_type.h", do not use directly

//              
//####DESCRIPTIONEND####
//

//-----------------------------------------------------------------------------
// Characterize the architecture

#define CYG_BYTEORDER           CYG_MSBFIRST    // Big endian

//-----------------------------------------------------------------------------
// Powerpc does not usually use labels with underscores. Some labels generated
// by the linker do, so add an underscore where required.

#define CYG_LABEL_NAME(_name_) _##_name_

//-----------------------------------------------------------------------------
// Define the standard variable sizes

// The PowerPC architecture uses the default definitions of the base types,
// so we do not need to define any here.

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_BASETYPE_H
// End of basetype.h
