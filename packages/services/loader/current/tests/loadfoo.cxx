//==========================================================================
//
//        loadfoo.cxx
//
//        Dynamic library test program
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
// Author(s):     nickg
// Contributors:  nickg
// Date:          2000-11-20
// Description:   Dynamic library test
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/kernel.h>
#include <pkgconf/hal.h>

#include <cyg/infra/testcase.h>

//#include <cyg/loader/loader.hxx>

#include <dlfcn.h>

//==========================================================================
// Include a version of the library ELF file converted to a C table

#include "tests/libfoo.so.c"

//==========================================================================

externC void cyg_dl_force(void);

typedef void vfn();

int fee_data = 0;

//int main( int argc, char **argv )
externC void cyg_start()
{
    CYG_TEST_INIT();

    CYG_TEST_INFO( "LoadFoo: started" );
#if 0    
    Cyg_LoaderStream_Mem memstream(libfoo, sizeof( libfoo ) );

    Cyg_LoadObject *obj;

//    cyg_dl_force();
    
    Cyg_Loader::loader->load( memstream, 0, &obj );

    vfn *foo = (vfn *)obj->symbol( "foo" );

#else

    void *fooh = dlopenmem( libfoo, sizeof(libfoo), RTLD_NOW );

    vfn *foo = (vfn *)dlsym( fooh, "foo" );
    
#endif

    
    if( foo )
    {
        CYG_TEST_INFO( "LoadFoo: foo() call" );    
        foo();
        CYG_TEST_INFO( "LoadFoo: foo() returned" );            
    }
    else
    {
        CYG_TEST_FAIL_FINISH( "LoadFoo: foo() NULL!!!" );    
    }

    CYG_TEST_PASS_FINISH("LoadFoo: OK");
    
}

//==========================================================================

externC void fee(int x)
{
    CYG_TEST_INFO( "LoadFoo: fee() called" );    
    fee_data = x;
    CYG_TEST_INFO( "LoadFoo: fee() returning" );    
}

//==========================================================================
// EOF loadfoo.cxx
