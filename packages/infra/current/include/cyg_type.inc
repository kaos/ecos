#ifndef CYGONCE_INFRA_CYG_TYPE_INC
#define CYGONCE_INFRA_CYG_TYPE_INC

//==========================================================================
//
//      cyg_type.inc
//
//      Standard types, and some useful coding macros.
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-07-25
// Purpose:      Define symbols for use in linker scripts and "generic" asm
// Description:  This file should only use #defines - it should be safe
//               to include from both linker scripts and assembler files
// Usage:        #include <cyg/infra/cyg_type.inc>
//
//              
//####DESCRIPTIONEND####

#include <cyg/hal/basetype.h>

// -------------------------------------------------------------------------
// Label name macros. Some toolsets generate labels with initial
// underscores and others don't. CYG_LABEL_NAME should be used on
// labels in C/C++ code that are defined in assembly code or linker
// scripts. CYG_LABEL_DEFN is for use in assembly code and linker
// scripts where we need to manufacture labels that can be used from
// C/C++.
// These are default implementations that should work for most targets.
// They may be overridden in basetype.h if necessary.

#ifndef CYG_LABEL_DEFN
# define CYG_LABEL_DEFN(_label) _label
#endif

// -------------------------------------------------------------------------
// Provide a default architecture alignment.
// This may be overridden in basetype.h if necessary.

#ifndef CYGARC_ALIGNMENT
# define CYGARC_ALIGNMENT 8
#endif
// And corresponding power of two alignment
#ifndef CYGARC_P2ALIGNMENT
# define CYGARC_P2ALIGNMENT 3
#endif

#endif /* CYGONCE_INFRA_CYG_TYPE_INC */

// EOF cyg_type.inc
