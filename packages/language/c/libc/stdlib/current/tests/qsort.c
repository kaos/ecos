//=================================================================
//
//        qsort.c
//
//        Testcase for C library qsort()
//
//=================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-30
// Description:   Contains testcode for C library qsort() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <pkgconf/system.h>
#include <stdlib.h>
#include <cyg/infra/testcase.h>


// FUNCTIONS


static int
Compar( const void *int1, const void *int2 )
{
    if ( *(int*)int1 < *(int*)int2 )
        return -1;
    else if ( *(int*)int1 == *(int*)int2 )
        return 0;
    else
        return 1;
} // Compar()

#ifndef CYGPKG_LIBC_STARTUP
void cyg_user_start(void)
#else
int
main( int argc, char *argv[] )
#endif
{
    unsigned int ctr;
    int fail=0;
    int i_array_sorted[] = {1, 5, 8, 35, 84, 258, 1022, 5300, 7372, 9029};
    int i_array_unsorted[] = {258, 8, 7372, 1, 5, 9029, 1022, 35, 5300, 84};

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "qsort() function");

    qsort(i_array_unsorted,
          sizeof(i_array_unsorted) / sizeof(i_array_unsorted[0]),
          sizeof(int), &Compar);

    for (ctr = 0;
         ctr < (sizeof(i_array_unsorted) / sizeof(i_array_unsorted[0]));
         ++ctr)
    {
        if (i_array_sorted[ctr] != i_array_unsorted[ctr])
            ++fail;
    } // for

    CYG_TEST_PASS_FAIL( fail == 0, "qsort() sorts correctly" );

//    CYG_TEST_NA("Testing is not applicable to this configuration");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "qsort() function");

} // main()

// EOF qsort.c
