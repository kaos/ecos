//========================================================================
//
//      malloc.cxx
//
//      Implementation of ISO C memory allocation routines
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-21
// Purpose:       Provides ISO C calloc(), malloc(), realloc() and free()
//                functions
// Description:   Implementation of ANSI standard allocation routines as per
//                ISO C section 7.10.3
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Do we want these functions?
#ifdef CYGPKG_LIBC_MALLOC

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support
#include <string.h>                // For memset() and memcpy()
#include <stdlib.h>                // header for this file
#include <pkgconf/kernel.h>        // kernel configuration
#include <cyg/kernel/memvar.hxx>   // Kernel variable size block allocator

#include "clibincl/stdlibsupp.hxx" // Additional support for stdlib stuff
                                   // internal to the C library

// EXPORTED SYMBOLS

externC void *
calloc( size_t, size_t ) CYGBLD_ATTRIB_WEAK_ALIAS(_calloc);

externC void
free( void * ) CYGBLD_ATTRIB_WEAK_ALIAS(_free);

externC void *
malloc( size_t ) CYGBLD_ATTRIB_WEAK_ALIAS(_malloc);

externC void *
realloc( void *, size_t ) CYGBLD_ATTRIB_WEAK_ALIAS(_realloc);


// STATIC VARIABLES

// the data space for the memory pool - can be externally overriden
// FIXME: but should be in different file for this to work
cyg_uint8 cyg_libc_malloc_memorypool[ CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE ] 
             CYGBLD_ATTRIB_WEAK;

// the memory pool object itself
static Cyg_Mempool_Variable pool CYG_INIT_PRIORITY( LIBC ) =
Cyg_Mempool_Variable( cyg_libc_malloc_memorypool,
                      CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE );


// FUNCTIONS

void *
_malloc( size_t size )
{
    void *data_ptr;

    CYG_REPORT_FUNCNAMETYPE( "_malloc", "returning pointer %08x" );
    
    CYG_REPORT_FUNCARG1DV( size );

    // first check if size wanted is 0
    if (!size) {
        CYG_REPORT_RETVAL( NULL );
        return NULL;
    } // if

    // ask the pool for the data
    data_ptr = pool.try_alloc( size );

    // if it isn't NULL is the pointer valid?
    if (data_ptr != NULL) {
        CYG_CHECK_DATA_PTR( data_ptr,
                            "allocator returned invalid pointer!" );

        // And just check its alignment
        CYG_ASSERT( !((CYG_ADDRWORD)data_ptr & (sizeof(CYG_ADDRWORD) - 1)),
                    "Allocator has returned badly aligned data!");
    } // if

    CYG_REPORT_RETVAL( data_ptr );

    return data_ptr;
} // _malloc()


void
_free( void *ptr )
{
    CYG_REPORT_FUNCNAME( "_free");
    
    CYG_REPORT_FUNCARG1XV( ptr );

    // if null pointer, do nothing as per spec
    if (ptr==NULL)
        return;

    CYG_CHECK_DATA_PTR( ptr, "Pointer to free isn't even valid!" );

    // get pool to free it. Use size==0 for pool to determine the size
    // Also ignore return value - there's nothing we can do with it
    pool.free( (cyg_uint8 *) ptr, 0 );

    CYG_REPORT_RETURN();

} // _free()


void *
_calloc( size_t nmemb, size_t size )
{
    void *data_ptr;
    cyg_ucount32 realsize;

    CYG_REPORT_FUNCNAMETYPE( "_calloc", "returning pointer %08x" );
    
    CYG_REPORT_FUNCARG2DV( nmemb, size );

    realsize = nmemb * size;

    data_ptr = _malloc( realsize );

    // Fill with 0's if non-NULL
    if (data_ptr != NULL)
        memset( data_ptr, 0, realsize );

    CYG_REPORT_RETVAL( data_ptr );
    return data_ptr;
} // _calloc()


externC void *
_realloc( void *ptr, size_t size )
{
    void *ret_ptr;
    cyg_int32 oldsize;

    CYG_REPORT_FUNCNAMETYPE( "_realloc", "returning pointer %08x" );

    CYG_REPORT_FUNCARG2( "ptr=%08x, size=%d", ptr, size );

    // if pointer is NULL, we must malloc it
    if (ptr == NULL) {
        ret_ptr = _malloc( size );
        CYG_REPORT_RETVAL( ret_ptr );
        return ret_ptr;
    } // if

    CYG_CHECK_DATA_PTR( ptr, "_realloc() passed a bogus pointer!" );

    // if size is 0, we must free it
    if (size == 0) {
        _free(ptr);
        CYG_REPORT_RETVAL( NULL );
        return NULL;
    } // if
        

    oldsize = pool.get_allocation_size( (cyg_uint8 *)ptr );

    CYG_ASSERT( oldsize != -1,
                "_realloc() couldn't find allocation size!" );

    ret_ptr = _malloc( size );
    
    // as long as malloc succeeded
    if (ret_ptr != NULL) {
        // copy old contents
        memcpy( ret_ptr, ptr, size < (size_t) oldsize ? size
                                                      : (size_t) oldsize );
    
        _free(ptr);
    } // if

    CYG_REPORT_RETVAL( ret_ptr );
    return ret_ptr;
} // _realloc()

#endif // ifdef CYGPKG_LIBC_MALLOC

// EOF malloc.cxx
