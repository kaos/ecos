#ifndef CYGONCE_LOADER_DLFCN_H
#define CYGONCE_LOADER_DLFCN_H

//==========================================================================
//
//      dlfcn.h
//
//      ELF dynamic loader API definitions
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
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-11-15
// Purpose:             Define ELF dynamic loader API
// Description:         The functions defined here collectively implement the
//                      external API of the ELF dynamic loader.
// Usage:               #include <cyg/loader/dlfcn.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>

#include <cyg/infra/cyg_type.h>         /* types etc.           */

// =========================================================================
// Mode values

#define RTLD_NOW        0x00    /* Relocate now (default)               */
#define RTLD_LAZY       0x01    /* Relocate opportunistically           */
#define RTLD_GLOBAL     0x00    /* make symbols available globally (default) */
#define RTLD_LOCAL      0x10    /* keep symbols secret                  */

// =========================================================================
// API calls

__externC void *dlopen (const char *file, int mode);

__externC void *dlopenmem(const void *addr, size_t size, int mode);

__externC int dlclose (void *handle);

__externC void *dlsym (void *handle, const char *name);

__externC const char *dlerror (void);

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_LOADER_DLFCN_H
// EOF dlfcn.h
