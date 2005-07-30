//==========================================================================
//
//      jffs2_3.c
//
//      Test garbage collect on a filesystem
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 Andrew Lunn
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           asl
// Contributors:        asl
// Date:                2005-01-16
// Purpose:             Test garbage collect on a filesystem
// Description:         This test creates and deletes files in order
//                      to test the garbage collection code.
//                      
//####DESCRIPTIONEND####
//
//==========================================================================
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <cyg/fileio/fileio.h>
#include <cyg/io/flash.h>
#include <cyg/crc/crc.h>

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>            // HAL polled output

//==========================================================================

#define ITERATIONS 1000000
#define NELEM(_x_) (sizeof(_x_)/sizeof(*(_x_)))

#define SHOW_RESULT( _fn, _res ) \
diag_printf("FAIL: " #_fn "() returned %d %s\n", _res, \
            (unsigned long) _res<0?strerror(errno):"");

//==========================================================================
// file creation, deletion and testing functions

static void create_file(int i)
{
  cyg_int32 buffer[1020];
  char name[16];
  cyg_uint32 j;
  int fd, err;
  
  sprintf(name,"test%07d",i);

  fd = creat(name, S_IRWXU);
  if (fd == -1) SHOW_RESULT( creat, fd );
  
  for (j=1; j < NELEM(buffer); j++) {
    buffer[j] = rand();
  }
  
  buffer[0] = 0;
  buffer[0] = cyg_posix_crc32((unsigned char *)buffer, sizeof(buffer));
  
  err = write(fd, buffer, sizeof(buffer));
  if (err == -1) SHOW_RESULT( write, err );
  
  err = close(fd);
  if (err == -1) SHOW_RESULT( close, err );
}

static void delete_file(int i)
{
  char name[16];
  int err;
  
  sprintf(name,"test%07d",i);

  err = unlink(name);
  if (err == -1) SHOW_RESULT( unlink, err );
}

static void check_file(int i)
{
  char name[16];
  int err, fd;
  cyg_int32 buffer[1020];
  cyg_uint32 crc;
  
  sprintf(name,"test%07d",i);

  fd = open(name, O_RDONLY);
  if (fd == -1) SHOW_RESULT( open, fd );
  
  err = read(fd, buffer, sizeof(buffer));
  if (err == -1) SHOW_RESULT( read, fd );
  
  crc = buffer[0];
  buffer[0] = 0;
  
  if (crc != cyg_posix_crc32((unsigned char *)buffer, sizeof(buffer))) {
    CYG_TEST_FAIL("File corrupt");
  }

  err = close(fd);
  if (err == -1) SHOW_RESULT( read, fd );
}


//==========================================================================
// main

int main( int argc, char **argv )
{
    int err, iteration;
    struct mallinfo minfo
;
    CYG_TEST_INIT();

    // --------------------------------------------------------------

    CYG_TEST_INFO("mount /");    
    err = mount( CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1, "/", "jffs2" );
    if( err < 0 ) SHOW_RESULT( mount, err );    
    
    chdir ("/");
    
    iteration=0;
    create_file(iteration);
    while (iteration < ITERATIONS) {
      if (!(iteration % 1000)) {
        minfo = mallinfo();
        diag_printf("<INFO> Iteration %07d fordblks = %7d\n", 
                    iteration, minfo.fordblks);
      }
      iteration++;
      create_file(iteration);
      check_file(iteration-1);
      delete_file(iteration-1);
      check_file(iteration);
    }
    
    CYG_TEST_INFO("umount /");
    err = umount( "/" );
    if( err < 0 ) SHOW_RESULT( umount, err );    
    
    CYG_TEST_PASS_FINISH("jffs2_3");
}
