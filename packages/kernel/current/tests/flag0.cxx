//==========================================================================
//
//        flag0.cxx
//
//        Flag test 0
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-05-11
// Description:   Limited to checking constructors/destructors
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/flag.hxx>

#include <cyg/infra/testcase.h>

#include "testaux.hxx"

static Cyg_Flag f0, f1;


static bool flash( void )
{
    Cyg_Flag f0;

    Cyg_Flag f1;

    return true;
}

void flag0_main( void )
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());
    
    CYG_TEST_PASS_FINISH("Flag 0 OK");

}

externC void
cyg_start( void )
{
    flag0_main();
}   
// EOF flag0.cxx
