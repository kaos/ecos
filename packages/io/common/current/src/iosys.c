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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:  gthomas
// Date:        1999-02-04
// Purpose:     Device I/O Support
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>

//extern void cyg_io_init(void) CYGBLD_ATTRIB_CONSTRUCTOR
//  CYG_INIT_PRIORITY(CYG_INIT_BEFORE(LIBC));

// Checks that two strings are "equivalent" device names
// 'n1' is a string from the user
// 'n2' is a name in a device table entry
// 'cyg_io_compare()' will return true IFF
//    n1 == n2, for all characters
//    n2 ends in '/' and matches n1 up to the terminating '/'
// 'ptr' will get a pointer to the residual string.
static bool
cyg_io_compare(const char *n1, const char *n2, const char **ptr)
{
    while (*n1 && *n2) {
        if (*n1++ != *n2++) {
            return false;
        }
    }
    if (*n1) {
        // See if the devtab name is is a substring
        if (*(n2-1) == '/') {
            *ptr = n1;
            return true;
        }
    }
    if (*n1 || *n2) {
        return false;
    }
    *ptr = n1;
    return true;
}

//
// This function is called during system initialization.  The purpose is
// to step through all devices linked into the system, calling their
// "init" entry points.  
//

void
cyg_io_init(void)
{
    static int _init = false;
    cyg_devtab_entry_t *t;
    if (_init) return;
    for (t = &__DEVTAB__[0]; t != &__DEVTAB_END__; t++) {
#ifdef CYGDBG_IO_INIT
        diag_printf("Init device '%s'\n", t->name);
#endif
        if (t->init(t)) {
            t->status = CYG_DEVTAB_STATUS_AVAIL;
        } else {
            // What to do if device init fails?
            t->status = 0;  // Device not [currently] available
        }
    }
    _init = true;
}

//
// Look up the devtab entry for a named device and return its handle.
// If the device is found and it has a "lookup" function, call that
// function to allow the device/driver to perform any necessary
// initializations.
//

Cyg_ErrNo
cyg_io_lookup(const char *name, cyg_io_handle_t *handle)
{
    cyg_devtab_entry_t *t, *st;
    Cyg_ErrNo res;
    const char *name_ptr;
    for (t = &__DEVTAB__[0]; t != &__DEVTAB_END__; t++) {
        if (cyg_io_compare(name, t->name, &name_ptr)) {
            // FUTURE: Check 'avail'/'online' here
            if (t->dep_name) {
                res = cyg_io_lookup(t->dep_name, (cyg_io_handle_t *)&st);
                if (res != ENOERR) {
                    return res;
                }
            } else {
                st = (cyg_devtab_entry_t *)0;
            }
            if (t->lookup) {
                // This indirection + the name pointer allows the lookup routine
                // to return a different 'devtab' handle.  This will provide for
                // 'pluggable' devices, file names, etc.
                res = (t->lookup)(&t, st, name_ptr);
                if (res != ENOERR) {
                    return res;
                }
            }
            *handle = (cyg_io_handle_t)t;
            return ENOERR;
        }
    }
    return -ENOENT;  // Not found
}

//
// 'write' data to a device.
//

Cyg_ErrNo 
cyg_io_write(cyg_io_handle_t handle, const void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    // Validate request
    if (!t->handlers->write) {
        return -EDEVNOSUPP;
    }
    // Special check.  If length is zero, this just verifies that the 
    // 'write' method exists for the given device.
    if (NULL != len && 0 == *len) {
        return ENOERR;
    }
    return t->handlers->write(handle, buf, len);
}

//
// 'read' data from a device.
//

Cyg_ErrNo 
cyg_io_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    // Validate request
    if (!t->handlers->read) {
        return -EDEVNOSUPP;
    }
    // Special check.  If length is zero, this just verifies that the 
    // 'read' method exists for the given device.
    if (NULL != len && 0 == *len) {
        return ENOERR;
    }
    return t->handlers->read(handle, buf, len);
}

//
// Get the configuration of a device.
//

Cyg_ErrNo 
cyg_io_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    // Validate request
    if (!t->handlers->get_config) {
        return -EDEVNOSUPP;
    }
    // Special check.  If length is zero, this just verifies that the 
    // 'get_config' method exists for the given device.
    if (NULL != len && 0 == *len) {
        return ENOERR;
    }
    return t->handlers->get_config(handle, key, buf, len);
}

//
// Change the configuration of a device.
//

Cyg_ErrNo 
cyg_io_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buf, cyg_uint32 *len)
{
    cyg_devtab_entry_t *t = (cyg_devtab_entry_t *)handle;
    // Validate request
    if (!t->handlers->set_config) {
        return -EDEVNOSUPP;
    }
    // Special check.  If length is zero, this just verifies that the 
    // 'set_config' method exists for the given device.
    if (NULL != len && 0 == *len) {
        return ENOERR;
    }
    return t->handlers->set_config(handle, key, buf, len);
}

/*---------------------------------------------------------------------------*/
/* End of io/iosys.c */
