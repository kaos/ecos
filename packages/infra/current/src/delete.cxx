//==========================================================================
//
//      common/delete.cxx
//
//      Default delete implementation
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1999-08-13
// Purpose:     Gives a default delete operation if the C library isn't present
// Description: This file supplies a definition of the default delete
//              operation (aka __builtin_delete and __builtin_vec_delete)
//              for use when the normal delete can't be used - normally when
//              the C library is not present
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/infra.h>

#include <cyg/infra/cyg_type.h>

// see the description comment in infra.cdl for
// CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS

#ifdef CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS
// then define these empty functions:

void operator delete(void *x) throw()
{
    CYG_EMPTY_STATEMENT;
}

void operator delete[](void *x) throw()
{
    CYG_EMPTY_STATEMENT;
}

#endif // CYGFUN_INFRA_EMPTY_DELETE_FUNCTIONS

// EOF delete.cxx
