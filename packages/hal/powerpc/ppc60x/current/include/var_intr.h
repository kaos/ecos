#ifndef CYGONCE_VAR_INTR_H
#define CYGONCE_VAR_INTR_H
//=============================================================================
//
//      var_intr.h
//
//      Variant HAL interrupt and clock support
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
// Contributors:nickg, jskov, jlarmour, hmt
// Date:        2000-04-02
// Purpose:     Variant interrupt support
// Description: The macros defined here provide the HAL APIs for handling
//              interrupts and the clock on the PPC60x variant CPUs.
// Usage:       Is included via the architecture interrupt header:
//              #include <cyg/hal/hal_intr.h>
//              ...
//
//####DESCRIPTIONEND####
//
//=============================================================================

// No builtin interrupt controller in the PPC60x CPUs.

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_VAR_INTR_H
// End of var_intr.h
