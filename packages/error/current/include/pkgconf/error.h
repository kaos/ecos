#ifndef CYGONCE_PKGCONF_ERROR_H
#define CYGONCE_PKGCONF_ERROR_H
//===========================================================================
//
//      error.h
//
//      Configuration header for the Common error code package
//
//===========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-06-11
// Purpose:     To configure the Common error package
// Description: Does any necessary configuration of the Common error package
// Usage:       #include <pkgconf/error.h>  
//              #ifdef CYGPKG_ERROR
//              ...
//              
//
//####DESCRIPTIONEND####
//
//===========================================================================

// INCLUDES

// Include system configuration file to work out whether the Common error
// package is to be included at all (from CYGPKG_ERROR)
#include <pkgconf/system.h>

/*
    {{CFG_DATA

        cdl_package CYGPKG_ERROR {
            display      "Common error code support"
            description  "This package contains the common list of error and
                          status codes. It is held centrally to allow
                          packages to interchange error codes and status
                          codes in a common way, rather than each package
                          having its own conventions for error/status
                          reporting. The error codes are modelled on the
                          POSIX style naming e.g. EINVAL etc. This package
                          also provides the standard strerror() function to
                          convert error codes to textual representation."
            doc           ref/ecos-ref/p-cygpkg-error.html
            type         bool
        }

    }}CFG_DATA
*/


#endif // CYGONCE_PKGCONF_ERROR_H multiple inclusion protection

// EOF error.h
