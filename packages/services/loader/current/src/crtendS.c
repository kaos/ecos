//==========================================================================
//
//      crtendS.c
//
//      C runtime support
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
// Date:                2000-11-25
// Purpose:             C runtime support
// Description:         This file contains code that must appear at the very end of
//                      a dynamically loaded library. It contains definitions for the
//                      end of the .init and .fini sections.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>

#include <cyg/loader/loader.hxx>

/*------------------------------------------------------------------------*/

#ifndef INIT_SECTION_ASM_OP
#define INIT_SECTION_ASM_OP  ".section\t.init,\"ax\"\n"
#endif

#ifndef FINI_SECTION_ASM_OP
#define FINI_SECTION_ASM_OP  ".section\t.fini,\"ax\"\n"
#endif

#ifndef TEXT_SECTION_ASM_OP
#define TEXT_SECTION_ASM_OP  ".text\n"
#endif

#ifndef CTORS_SECTION_ASM_OP
#define CTORS_SECTION_ASM_OP    "\t.section\t.ctors,\"aw\""
#endif

#ifndef DTORS_SECTION_ASM_OP
#define DTORS_SECTION_ASM_OP    "\t.section\t.dtors,\"aw\""
#endif

/*------------------------------------------------------------------------*/

typedef void (*pfunc) (void);

#if 1
asm(CTORS_SECTION_ASM_OP);
static pfunc __CTOR_END__[1] __attribute__((unused)) = { (pfunc) 0 };
asm(DTORS_SECTION_ASM_OP);
static pfunc __DTOR_END__[] __attribute__((unused)) = { (pfunc) 0 };
asm( TEXT_SECTION_ASM_OP );

#else

extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];
extern pfunc __DTOR_END__[];

#endif

/*------------------------------------------------------------------------*/

static void
cyg_hal_invoke_constructors(void)
{
#if 1
    pfunc *p;

    for (p = &__CTOR_END__[-1]; *p != (pfunc) -1 ; p--)
        (*p) ();
#elif 0
    pfunc *p;

    for (p = &__CTOR_LIST__[(int)__CTOR_LIST__[0]]; p != &__CTOR_LIST__[0] ; p--)
        (*p) ();    
#endif    
} // cyg_hal_invoke_constructors()

/*------------------------------------------------------------------------*/

static void __attribute__ ((__unused__))
init_dummy (void)
{
    asm (INIT_SECTION_ASM_OP);
    cyg_hal_invoke_constructors();
}

asm( TEXT_SECTION_ASM_OP );

/*------------------------------------------------------------------------*/

static void __attribute__ ((__unused__))
fini_dummy (void)
{
  asm (FINI_SECTION_ASM_OP);
//  __do_global_dtors_aux ();
#ifdef FORCE_FINI_SECTION_ALIGN
  FORCE_FINI_SECTION_ALIGN;
#endif
}

asm (TEXT_SECTION_ASM_OP);

/*------------------------------------------------------------------------*/

//==========================================================================
// End of crtendS.c
