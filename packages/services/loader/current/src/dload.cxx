//==========================================================================
//
//      dload.cxx
//
//      Dynamic loader API
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
// Date:                2000-11-03
// Purpose:             Loader class implementation
// Description:         This file contains the dynamic ELF loader API.
//                      This presents the standard dlxxx() calls by invoking
//                      the loader classes as appropriate.
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/isoinfra.h>

#include <cyg/kernel/ktypes.h>          // base kernel types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <string.h>

#include <cyg/loader/loader.hxx>        // Loader header

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>                      // API definitions

// =========================================================================
// Cyg_LoaderStream_File class

// =========================================================================
// API calls

cyg_uint32 global_symbol_object;

// =========================================================================
// Load and open object

__externC void *dlopen (const char *file, int mode)
{
    CYG_REPORT_FUNCTION();

    void *ret = NULL;

    if( file == NULL )
    {
        // Special case to allow access to all symbols.

        ret = (void *)&global_symbol_object;
    }
#ifdef CYGPKG_IO_FILEIO    
    else
    {
        int fd = open( file, O_RDONLY );

        if( fd < 0 )
            return NULL;
        
        Cyg_LoaderStream_File filestream( fd );

        Cyg_LoadObject *obj;

        cyg_code error = Cyg_Loader::loader->load( filestream, mode, &obj );

        if( error == 0)
        {
            ret = (void *)obj;
        }

        close( fd );
    }
#endif
    
    CYG_REPORT_RETVAL(ret);    
    return ret;
}

// =========================================================================

__externC void *dlopenmem(const void *addr, size_t size, int mode)
{
    CYG_REPORT_FUNCTION();    

    void *ret = NULL;

    Cyg_LoaderStream_Mem memstream( addr, size );

    Cyg_LoadObject *obj;

    cyg_code error = Cyg_Loader::loader->load( memstream, mode, &obj );

    if( error == 0)
        ret = (void *)obj;
    
    CYG_REPORT_RETVAL(ret);    
    return ret;
}

// =========================================================================

__externC int dlclose (void *handle)
{
    CYG_REPORT_FUNCTION();

    int ret = 0;

    if( handle == (void *)global_symbol_object )
    {
        // Nothing to do here...
    }
    else
    {
        Cyg_LoadObject *obj = (Cyg_LoadObject *)handle;

        Cyg_Loader::loader->close( obj );
    }
    
    CYG_REPORT_RETVAL(ret);    
    return ret;
}

// =========================================================================

__externC void *dlsym (void *handle, const char *name)
{
    CYG_REPORT_FUNCTION();

    void *ret = NULL;

    Cyg_LoadObject *obj = (Cyg_LoadObject *)handle;

    ret = obj->symbol( name );
    
    CYG_REPORT_RETVAL(ret);    
    return ret;
}

// =========================================================================

__externC const char *dlerror (void)
{
    CYG_REPORT_FUNCTION();

    const char *ret = Cyg_Loader::loader->error_string();

    CYG_REPORT_RETVAL(ret);
    return ret;
}


// =========================================================================
// EOF dload.cxx
