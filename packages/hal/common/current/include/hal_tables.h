#ifndef CYGONCE_HAL_TABLES_H
#define CYGONCE_HAL_TABLES_H

/*==========================================================================
//
//      hal_tables.h
//
//      Data table management
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
// Author(s):   nickg
// Date:        2000-09-04
// Purpose:     Provide HAL tables
// Description: This file defines a mechanism to include "tables" of objects
//              that are always included in the image no matter what, and are
//              constrained between labels.
//              
// Usage:       #include <cyg/hal/hal_tables.h>
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_arch.h>

/*------------------------------------------------------------------------*/

#define __string(_x) #_x
#define __xstring(_x) __string(_x)

#ifndef CYG_HAL_TABLE_BEGIN
#define CYG_HAL_TABLE_BEGIN( _label, _name )                                  \
__asm__(".section \"" __string(.ecos.table.##_name##.begin) "\",\"aw\"\n"     \
	".globl " __xstring(CYG_LABEL_DEFN(_label)) "\n"                      \
	".type    " __xstring(CYG_LABEL_DEFN(_label)) ",@object\n"            \
	".p2align " __xstring(CYGARC_P2ALIGNMENT) "\n"                        \
__xstring(CYG_LABEL_DEFN(_label)) ":\n"                                       \
	".previous\n"                                                         \
       )
#endif

#ifndef CYG_HAL_TABLE_END
#define CYG_HAL_TABLE_END( _label, _name )                                    \
__asm__(".section \"" __string(.ecos.table.##_name##.finish) "\",\"aw\"\n"    \
	".globl " __xstring(CYG_LABEL_DEFN(_label)) "\n"                      \
	".type    " __xstring(CYG_LABEL_DEFN(_label)) ",@object\n"            \
	".p2align " __xstring(CYGARC_P2ALIGNMENT) "\n"                        \
__xstring(CYG_LABEL_DEFN(_label)) ":\n"                                       \
	".previous\n"                                                         \
       )
#endif

// This macro must be applied to any types whose objects are to be placed in
// tables
#ifndef CYG_HAL_TABLE_TYPE
#define CYG_HAL_TABLE_TYPE CYGBLD_ATTRIB_ALIGN( CYGARC_ALIGNMENT )
#endif

#ifndef CYG_HAL_TABLE_EXTRA
#define CYG_HAL_TABLE_EXTRA( _name ) \
        CYGBLD_ATTRIB_SECTION(.ecos.table.##_name##.extra) \
        CYGBLD_ATTRIB_ALIGN( CYGARC_ALIGNMENT )
#endif

#ifndef CYG_HAL_TABLE_ENTRY
#define CYG_HAL_TABLE_ENTRY( _name ) \
        CYGBLD_ATTRIB_SECTION(.ecos.table.##_name##.data) \
        CYGBLD_ATTRIB_ALIGN( CYGARC_ALIGNMENT )
#endif

#ifndef CYG_HAL_TABLE_QUALIFIED_ENTRY
#define CYG_HAL_TABLE_QUALIFIED_ENTRY( _name, _qual ) \
        CYGBLD_ATTRIB_SECTION(.ecos.table.##_name##.data.##_qual) \
        CYGBLD_ATTRIB_ALIGN( CYGARC_ALIGNMENT )
#endif

/*------------------------------------------------------------------------*/
/* EOF hal_tables.h                                                       */
#endif // CYGONCE_HAL_TABLES_H
