#ifndef CYGONCE_IO_H
#define CYGONCE_IO_H
// ====================================================================
//
//      io.h
//
//      Device I/O 
//
// ====================================================================
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     Describe low level I/O interfaces.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// This file contains the user-level visible I/O interfaces

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#ifdef CYGPKG_ERROR
#include <cyg/error/codes.h>
#else
#error I/O subsystem requires 'error' package
#endif

// typedef int Cyg_ErrNo;

#ifdef __cplusplus
extern "C" {
#endif

typedef void *cyg_io_handle_t;

// Lookup a device and return it's handle
Cyg_ErrNo cyg_io_lookup(const char *name, 
                        cyg_io_handle_t *handle);
// Write data to a device
Cyg_ErrNo cyg_io_write(cyg_io_handle_t handle, 
                       const void *buf, 
                       cyg_uint32 *len);
// Read data from a device
Cyg_ErrNo cyg_io_read(cyg_io_handle_t handle, 
                      void *buf, 
                      cyg_uint32 *len);
// Get the configuration of a device
Cyg_ErrNo cyg_io_get_config(cyg_io_handle_t handle, 
                            cyg_uint32 key,
                            void *buf, 
                            cyg_uint32 *len);
// Change the configuration of a device
Cyg_ErrNo cyg_io_set_config(cyg_io_handle_t handle, 
                            cyg_uint32 key,
                            const void *buf, 
                            cyg_uint32 *len);

#ifdef __cplusplus
}
#endif

#endif  /* CYGONCE_IO_H */
/* EOF io.h */
