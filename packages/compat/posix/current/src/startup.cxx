//==========================================================================
//
//      startup.cxx
//
//      POSIX startup code
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
// Author(s):           nickg, jlarmour
// Contributors:        nickg, jlarmour
// Date:                2000-03-27
// Purpose:             POSIX startup code
// Description:         This file contains code that must be run when the
//                      POSIX subsystem is started.
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/posix.h>

#include <cyg/infra/cyg_type.h>
#include "pprivate.h"                  // POSIX private header

// -------------------------------------------------------------------------

// define an object that will automatically initialize the POSIX subsystem

class cyg_posix_startup_dummy_constructor_class {
public:
    cyg_posix_startup_dummy_constructor_class() { 

#ifdef CYGPKG_POSIX_PTHREAD
        cyg_posix_pthread_start();
#endif
#ifdef CYGPKG_POSIX_SIGNALS
        cyg_posix_signal_start();
        cyg_posix_exception_start();
#endif
#ifdef CYGPKG_POSIX_TIMERS
        cyg_posix_clock_start();
#endif

    }
};

static cyg_posix_startup_dummy_constructor_class cyg_posix_startup_obj
                                  CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_COMPAT);

// -------------------------------------------------------------------------
// EOF startup.cxx
