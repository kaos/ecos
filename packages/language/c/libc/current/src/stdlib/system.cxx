//========================================================================
//
//      system.cxx
//
//      Implementation of ISO C system() function from section 7.10.4.5
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-03-04
// Purpose:       This file implements the ISO C system() function
// Description:   This file provides a trivial implementation of the
//                system() function from ISO C section 7.10.4.5. It
//                simply exists to show that a command processor is
//                not available
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// INCLUDES

#include <stdlib.h>                // Header for this file

// FUNCTIONS

extern "C" int
__system( const char * )
{
    return 0;
} // system()

extern "C" int system( const char * ) CYGBLD_ATTRIB_WEAK_ALIAS(__system);

// EOF system.cxx
