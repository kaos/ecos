//===========================================================================
//
//      stdiosupp.cxx
//
//      Helper C library functions for standard I/O
//
//===========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header


// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <cyg/devs/common/table.h>  // Device table
#include "clibincl/stringsupp.hxx"  // _strncmp() and _strcmp()
#include "clibincl/stdiosupp.hxx"   // Header for this file


struct Cyg_Device_Table_t *
Cyg_libc_stdio_find_filename( const char *filename )
{
    int i;
    struct Cyg_Device_Table_t *dev=NULL;

    // Find the "file" in the device table
    if ( _strncmp( filename, "/dev/", 5) )
    {
        return NULL;
    } // if

    // match the bit after the "/dev/"
    for (i=0; (Cyg_Device_Table[i].name != NULL); i++ )
    {
        if ( !_strcmp( &filename[5], Cyg_Device_Table[i].name) )
        {
            dev=&Cyg_Device_Table[i];
            break;
        }
    } // for

    return dev; // dev will still be NULL if the filename wasn't found
} // Cyg_libc_stdio_find_filename()


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF stdiosupp.cxx
