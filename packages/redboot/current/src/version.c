//==========================================================================
//
//      version.c
//
//      RedBoot version "string"
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-12-13
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/redboot.h>
#include <cyg/hal/hal_arch.h>

//
// These strings are used for informational purposes only.  If the end
// user wishes to replace them, simply redefine them in a platform specific
// file, e.g. in a local file used to provide additional local RedBoot
// customization.  Examples of such are platforms which define a special
// 'exec' command used to start a Linux kernel.
//
// CAUTION! Since this file is compiled using very special rules, it is not
// possible to replace it with a local version contained in the "build" tree.
// Replacing the information exported herein is accomplished via the techniques
// outlined above.
//

// Please do not edit the line below by hand: it is changed by the RedBoot
// release process. In a CVS branch, the line is commented out.
// In a release, it is defined to be the RedBoot release number.
//#define CYGDAT_REDBOOT_CUSTOM_VERSION current

#if defined(CYGDAT_REDBOOT_CUSTOM_VERSION)
#define _REDBOOT_VERSION CYGDAT_REDBOOT_CUSTOM_VERSION
#define _CERTIFICATE Non-certified
#elif defined(CYGPKG_REDBOOT_current)
#define _REDBOOT_VERSION UNKNOWN
#define _CERTIFICATE Non-certified
#else
#define _REDBOOT_VERSION CYGPKG_REDBOOT
#define _CERTIFICATE Red Hat certified
#endif
#define __s(x) #x
#define _s(x) __s(x)

char RedBoot_version[] CYGBLD_ATTRIB_WEAK =
   "\nRedBoot(tm) bootstrap and debug environment [" _s(CYG_HAL_STARTUP) "]"
   "\n" _s(_CERTIFICATE) " release, version " _s(_REDBOOT_VERSION) 
   " - built " __TIME__ ", " __DATE__ "\n\n";

// Override default GDB stubs 'info'
// Note: this can still be a "weak" symbol since it will occur in the .o
// file explicitly mentioned in the link command.  User programs will 
// still be allowed to override it.
char GDB_stubs_version[] CYGBLD_ATTRIB_WEAK = 
    "eCos GDB stubs [via RedBoot] - built " __DATE__ " / " __TIME__;
