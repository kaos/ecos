#ifndef CYGONCE_VAR_CACHE_H
#define CYGONCE_VAR_CACHE_H

//=============================================================================
//
//      var_cache.h
//
//      HAL cache control API
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
// Author(s):    nickg
// Contributors: nickg,jlarmour
// Date:         2001-03-21
// Purpose:      Cache control API
// Description:  The macros defined here provide the HAL APIs for handling
//               cache control operations.
// Usage:
//               #include <cyg/hal/var_cache.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/plf_cache.h>

// No caches defined in this variant by default
#ifndef HAL_ICACHE_ENABLE
# define HAL_ICACHE_ENABLE()
# define HAL_ICACHE_DISABLE()
# define HAL_ICACHE_SYNC()
# define HAL_ICACHE_INVALIDATE_ALL()
#endif

#ifndef HAL_DCACHE_ENABLE
# define HAL_DCACHE_ENABLE()
# define HAL_DCACHE_DISABLE()
# define HAL_DCACHE_SYNC()
# define HAL_DCACHE_INVALIDATE_ALL()
#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_VAR_CACHE_H
// End of var_cache.h
