//==========================================================================
//
//      ramfs3.c
//
//      Test fileio system, especially lseek calls.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           nickg, asl, Paluch Sebastian
// Contributors:        nickg
// Date:                2006-10-05
// Purpose:             Test fileio system
// Description:         This test uses the testfs to check out the initialization
//                      and basic operation of the fileio system
//                      
//                      
//                      
//                      
//                      
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <cyg/fileio/fileio.h>

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>            // HAL polled output
//==========================================================================

#define SHOW_RESULT( _fn, _res ) \
diag_printf("FAIL: " #_fn "() returned %ld %s\n", \
           (unsigned long)_res, _res<0?strerror(errno):"");

//==========================================================================

cyg_uint8 buf[256];
cyg_uint8 buf1[256];

//==========================================================================
// main

int main( int argc, char **argv )
{
    int err;
    FILE *stream;
    long pos;
    unsigned int i;
    char header[3];

    CYG_TEST_INIT();

    // --------------------------------------------------------------

    CYG_TEST_INFO("mount /");    
    err = mount( "", "/", "ramfs" );

    if( err < 0 ) SHOW_RESULT( mount, err );    
    
    CYG_TEST_INFO("creating /fseek");
    stream = fopen("/fseek","w+");
    if (!stream) {
      SHOW_RESULT( fopen, NULL);
      CYG_TEST_FAIL_FINISH("done");\
    }

    for (i = 0; i < sizeof(buf); i++) {
      buf[i] = i % 256;
    }
    
    CYG_TEST_INFO("writing test pattern");    
    err=fwrite(buf,sizeof(buf), 1, stream);
    if ( err < 0 ) SHOW_RESULT( fwrite, err );
    
    pos = ftell(stream);
    if (pos < 0) SHOW_RESULT( ftell, pos );
    if (pos != sizeof(buf))
      diag_printf("<FAIL>: ftell is not telling the truth.");
    
    CYG_TEST_INFO("fseek()ing to 85");
    err = fseek(stream, 85, SEEK_SET);
    if ( err < 0 ) SHOW_RESULT( fseek, err );

    pos = ftell(stream);
    if (pos < 0) SHOW_RESULT( ftell, pos );
    if (pos != 85) CYG_TEST_FAIL("ftell is not telling the truth");

    err = fread(header,3,1,stream);
    if ( err < 0 ) SHOW_RESULT( fwrite, err );
    if ((header[0] != 85) ||
        (header[1] != 86) ||
        (header[2] != 87))
      CYG_TEST_FAIL("Read returned false data");
    
    pos = ftell(stream);
    if (pos < 0) SHOW_RESULT( ftell, pos );
    if (pos != 88)  CYG_TEST_FAIL("ftell is not telling the truth");

    for (i = 88; i < 161; i++) {
      buf[i] = 0x42;
    }
    
    CYG_TEST_INFO("writing");
    err = fwrite(buf+88, 73, 1, stream);
    if ( err < 0 ) SHOW_RESULT( fwrite, err );

    pos = ftell(stream);
    if (pos < 0) SHOW_RESULT( ftell, pos );
    if (pos != 161)  CYG_TEST_FAIL("ftell is not telling the truth");

    CYG_TEST_INFO("closing file");
    err = fclose(stream);
    if (err != 0) SHOW_RESULT( fclose, err );

    CYG_TEST_INFO("open file /fseek");
    stream = fopen("/fseek", "r+");
    if (!stream) {
      SHOW_RESULT( fopen, NULL);
      CYG_TEST_FAIL_FINISH("done");\
    }

    CYG_TEST_INFO("Seeking to beginning of file");
    err = fseek(stream, 0, SEEK_SET);
    if ( err < 0 ) SHOW_RESULT( fseek, err );

    CYG_TEST_INFO("Reading buf1");
    err = fread(buf1,sizeof(buf1),1, stream);
    if (err != 1) SHOW_RESULT( fread, err );

    CYG_TEST_INFO("Comparing contents");
    if (memcmp(buf, buf1, sizeof(buf1))) 
      CYG_TEST_FAIL("File contents inconsistent");

    CYG_TEST_INFO("closing file");
    err = fclose(stream);
    if (err != 0) SHOW_RESULT( fclose, err );
    
    CYG_TEST_INFO("umount /");    
    err = umount( "/" );
    if( err < 0 ) SHOW_RESULT( umount, err );    
    
    CYG_TEST_PASS_FINISH("ramfs3");
}
