//==========================================================================
//
//      io/iosys.c
//
//      I/O Subsystem + Device Table support
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
// Author(s):   gthomas
// Contributors:  gthomas
// Date:        1999-02-04
// Purpose:     Device Table
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>

// This is a dummy class just so we can execute the I/O package 
// initialization at it's proper priority

externC void cyg_io_init(void);

class cyg_io_init_class {
public:
    cyg_io_init_class(void) { 
        cyg_io_init();
    }
};

// And here's an instance of the class just to make the code run
static cyg_io_init_class _cyg_io_init CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_IO);
