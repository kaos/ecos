//==========================================================================
//
//      plf_sections.h
//
//      V85x layout definitions - hardwired sections
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jlarmour
// Date:         2000-05-24
// Purpose:      Hardwired layout definitions
// Description:  Used only in linker scripts
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#if CYGINT_HAL_V850_VARIANT_SA1

#define SECTIONS_END . = ALIGN(4); _end = .; PROVIDE (end = .); \
  PROVIDE(_hal_vsr_table = 0x00FC0260); \
  PROVIDE(__allow_nmi = 0x00FC02F8); \
  PROVIDE(_hal_virtual_vector_table = 0x00FC0300);

#elif CYGINT_HAL_V850_VARIANT_SB1

#define SECTIONS_END . = ALIGN(4); _end = .; PROVIDE (end = .); \
  PROVIDE(_hal_vsr_table = 0x00FC0340); \
  PROVIDE(__allow_nmi = 0x00FC0410); \
  PROVIDE(_hal_virtual_vector_table = 0x00FC0440);

#else 
# error No v850 variant defined
#endif

// EOF plf_sections.h
