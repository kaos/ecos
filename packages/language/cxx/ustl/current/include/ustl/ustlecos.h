#ifndef CYGONCE_USTLECOS_H
#define CYGONCE_USTLECOS_H
// ==========================================================================
//
//      ustlecos.h
//
//      eCos specific implementations and additions to uSTL library
//
// ===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2009 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later
// version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// As a special exception, if other files instantiate templates or use
// macros or inline functions from this file, or you compile this file
// and link it with other works to produce a work based on this file,
// this file does not by itself cause the resulting work to be covered by
// the GNU General Public License. However the source code for this file
// must still be made available in accordance with section (3) of the GNU
// General Public License v2.
//
// This exception does not invalidate any other reasons why a work based
// on this file might be covered by the GNU General Public License.
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####
// ===========================================================================
// ===========================================================================
// #####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: 
// Date:         2009-07-28
// Purpose:      eCos specific additions to uSTL library
// Description:
// Usage:        Do not include directly  #include <ustl.h>
//
// ####DESCRIPTIONEND####
//
// ========================================================================*/


//
// If exception handling is disabled then we provide a low level and 
// lightwight "exeption handler"
//
#ifndef __EXCEPTIONS
namespace ustl
{

typedef void (*app_exception_handler_t)(const exception& ex);


// allow an application to register its own ustl exception handler
void set_app_exception_handler(app_exception_handler_t func);

// an application exception handler may use this function to print the
// exception to diagnostic output
void diag_print_exception(const exception& ex);
} // namespace ustl
#endif

// ---------------------------------------------------------------------------
#endif // CYGONCE_USTLECOS_H
