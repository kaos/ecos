/*=================================================================
//
//        cache.c
//
//        SYNTH ftok test
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     asl
// Contributors:  asl
// Date:          05/11/2005
//####DESCRIPTIONEND####
*/

#ifndef HOST
#include <cyg/hal/hal_io.h>
#include <cyg/infra/diag.h>

#define printf diag_printf
#define ftok cyg_hal_sys_ftok

#else

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#endif

int main(void)
{
  printf("ftok(\"/etc/passwd\",0x12)) = 0x%8x\n", 
         ftok("/etc/passwd",0x12));

  printf("ftok(\"/etc/passwd\",0x72)) = 0x%8x\n", 
         ftok("/etc/passwd",0x72));

  printf("ftok(\"/boot/vmlinuz\",0x72)) = 0x%8x\n", 
         ftok("/boot/vmlinuz",0x72));

  printf("ftok(\"/boot/vmlinuz\",0x12)) = 0x%8x\n", 
         ftok("/boot/vmlinuz",0x12));

  return 0;
}

#ifndef HOST

externC void
cyg_start( void )
{
    main();
}
#endif
