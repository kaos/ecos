//==========================================================================
//
//      src/ecos/init.cxx
//
//==========================================================================
// ####BSDCOPYRIGHTBEGIN####                                    
// -------------------------------------------                  
// This file is part of eCos, the Embedded Configurable Operating System.
//
// Portions of this software may have been derived from FreeBSD 
// or other sources, and if so are covered by the appropriate copyright
// and license included herein.                                 
//
// Portions created by the Free Software Foundation are         
// Copyright (C) 2002 Free Software Foundation, Inc.            
// -------------------------------------------                  
// ####BSDCOPYRIGHTEND####                                      
//==========================================================================

//==========================================================================
//
//      ecos/init.cxx
//
//      Networking package initializer class
//
//==========================================================================
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


// Network initialization

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#define NET_INIT CYGBLD_ATTRIB_INIT_AFTER(CYG_INIT_LIBC)

// This is a dummy class just so we can execute the network package 
// initialization at it's proper priority

externC void cyg_net_init(void);

class net_init_class {
public:
    net_init_class(void) { 
        cyg_net_init();
    }
};

// And here's an instance of the class just to make the code run
static net_init_class _net_init NET_INIT;

externC void
cyg_do_net_init(void)
{
}
