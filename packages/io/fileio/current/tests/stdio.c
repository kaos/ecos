//==========================================================================
//
//      stdio.c
//
//      Test stdio integration of fileio system
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
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-05-25
// Purpose:             Test stdio integration of fileio system
// Description:         This test uses the testfs to check that the options
//                      in the STDIO package that enable use of the fileio
//                      API work correctly. 
//                      
//                      
//                      
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <stdio.h>
#include <string.h>

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>            // HAL polled output

//==========================================================================
// Include the test filesystem.
// If we could make tests out of multiple files, then we could just link
// against the object file for this rather than including it.

#include "testfs.c"

//==========================================================================

#define SHOW_RESULT( _fn, _res ) \
diag_printf("<INFO>: " #_fn "() returned %d %s\n", _res, _res<0?strerror(errno):"");


#ifdef CYGPKG_LIBC_STDIO

//==========================================================================
// main

int main( int argc, char **argv )
{
    int err;
    FILE *f;
    fpos_t fpos;
    
    int flibble = 4567;
    char *wibble = "abcdefghijk";

    int flibble1;
    char wibble1[20];
    
    CYG_TEST_INIT();

    
    f = fopen("/foo", "w" );
    if( f == NULL ) SHOW_RESULT( fopen, -1 );

    err = fprintf(f, "flibble %d wibble %s\n", flibble, wibble );
    if( err < 0 ) SHOW_RESULT( fprintf, err );
    
    err = fprintf(f, "another flibble %d another wibble %s\n", flibble, wibble );
    if( err < 0 ) SHOW_RESULT( fprintf, err );
    
    err = fclose( f );
    if( err == EOF ) SHOW_RESULT( fclose, -1 );


    
    f = fopen("/foo", "r" );
    if( f == NULL ) SHOW_RESULT( fopen, -1 );

    err = fscanf(f, "flibble %d wibble %s\n", &flibble1, wibble1 );
    if( err < 0 ) SHOW_RESULT( fscanf, err );

    diag_printf("<INFO>: flibble1 %d wibble1 %s\n",flibble1,wibble1);
    
    CYG_TEST_CHECK( flibble1 == flibble , "Bad flibble result from fscanf");
    CYG_TEST_CHECK( strcmp(wibble,wibble1) == 0, "Bad wibble result from fscanf");


    err = fgetpos( f, &fpos );
    if( err < 0 ) SHOW_RESULT( fgetpos, err );    
    
    err = fseek( f, 0, SEEK_SET );
    if( err < 0 ) SHOW_RESULT( fseek, err );

    err = fscanf(f, "flibble %d wibble %s\n", &flibble1, wibble1 );
    if( err < 0 ) SHOW_RESULT( fscanf, err );

    diag_printf("<INFO>: flibble1 %d wibble1 %s\n",flibble1,wibble1);
    
    CYG_TEST_CHECK( flibble1 == flibble , "Bad flibble result from fscanf");
    CYG_TEST_CHECK( strcmp(wibble,wibble1) == 0, "Bad wibble result from fscanf");


    err = fseek( f, 0, SEEK_END );
    if( err < 0 ) SHOW_RESULT( fseek, err );

    err = fsetpos( f, &fpos );
    if( err < 0 ) SHOW_RESULT( fsetpos, err );    
    
    err = fscanf(f, "another flibble %d another wibble %s\n", &flibble1, wibble1 );
    if( err < 0 ) SHOW_RESULT( fscanf, err );

    diag_printf("<INFO>: flibble1 %d wibble1 %s\n",flibble1,wibble1);
    
    CYG_TEST_CHECK( flibble1 == flibble , "Bad flibble result from fscanf");
    CYG_TEST_CHECK( strcmp(wibble,wibble1) == 0, "Bad wibble result from fscanf");


    
    err = fclose( f );


    
    CYG_TEST_PASS_FINISH("stdio");
    
    return 0;
}

#else

// -------------------------------------------------------------------------

int main( int argc, char **argv )
{

    CYG_TEST_INIT();

    CYG_TEST_NA("stdio");
}

#endif    

// -------------------------------------------------------------------------
// EOF stdio.c
