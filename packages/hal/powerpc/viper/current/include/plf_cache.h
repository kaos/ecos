#ifndef CYGONCE_PLF_CACHE_H
#define CYGONCE_PLF_CACHE_H

//=============================================================================
//
//      plf_cache.h
//
//      Platform HAL cache details
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov
// Date:        2000-01-26
// Purpose:     Platform cache control API
// Description: The macros defined here provide the platform specific
//              cache control operations / behavior.
// Usage:       Is included via the architecture cache header:
//              #include <cyg/hal/hal_cache.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

//---------------------------------------------------------------------------
// Initial cache enabling
#ifdef CYGSEM_HAL_ROM_MONITOR
# define CYGPRI_INIT_CACHES
# define CYGPRI_ENABLE_CACHES
#endif

//-----------------------------------------------------------------------------
// FIXME: This definition forces the IO flash driver to use a
// known-good procedure for fiddling flash before calling flash device
// driver functions. The procedure breaks on other platform/driver
// combinations though so is depricated. Hence this definition.
//
// If you work on this target, please try to remove this definition
// and verify that the flash driver still works (both from RAM and
// flash). If it does, remove the definition and this comment for good
// [and the old macro definition if this happens to be the last client
// of that code].
#define HAL_FLASH_CACHES_OLD_MACROS

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_PLF_CACHE_H
// End of plf_cache.h
