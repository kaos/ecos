//==========================================================================
//
//      crtbeginS.c
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
// Description:         This file contains code that must appear at the very start of
//                      a dynamically loaded library. It contains definitions for the
//                      start of the .init and .fini sections.
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
#define CTORS_SECTION_ASM_OP    "\t.section\t.ctors,\"aw\"\n"
#endif

#ifndef DTORS_SECTION_ASM_OP
#define DTORS_SECTION_ASM_OP    "\t.section\t.dtors,\"aw\"\n"
#endif

/*------------------------------------------------------------------------*/

typedef void (*pfunc) (void);

#if 1

asm(CTORS_SECTION_ASM_OP);
static pfunc __CTOR_LIST__[1] __attribute__((unused)) = { (pfunc) -1 };
asm(DTORS_SECTION_ASM_OP);
static pfunc __DTOR_LIST__[1] __attribute__((unused)) = { (pfunc) -1 };
asm(TEXT_SECTION_ASM_OP);

#else

extern pfunc __CTOR_LIST__[];
extern pfunc __DTOR_LIST__[];

#endif

/*------------------------------------------------------------------------*/    

asm (INIT_SECTION_ASM_OP);

void __attribute__ ((__unused__))
init_dummy (void)
{
    asm( TEXT_SECTION_ASM_OP );
}

/*------------------------------------------------------------------------*/

asm (FINI_SECTION_ASM_OP);

static void __attribute__ ((__unused__))
fini_dummy (void)
{
//  __do_global_dtors_aux ();
#ifdef FORCE_FINI_SECTION_ALIGN
  FORCE_FINI_SECTION_ALIGN;
#endif
  asm (TEXT_SECTION_ASM_OP);
}

/*------------------------------------------------------------------------*/

//==========================================================================
// End of crtbeginS.c
