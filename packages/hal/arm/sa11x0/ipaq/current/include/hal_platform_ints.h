#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL extended support for platform specific interrupts
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
// Contributors: gthomas
// Date:         2001-02-24
// Purpose:      Define Interrupt support
// Description:  The interrupt details for the SA1110/iPAQ are defined here.
// Usage:
//               #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include CYGBLD_HAL_VAR_INTS_H   // Generic interrupt support

// Define here extended support for this particular platform

// IRQ handler for extended interrupts
#define HAL_EXTENDED_IRQ_HANDLER(index)                                 \
{  /* Check for extended interrupt source.  If present, decode it into  \
      a valid eCos interrupt number and return.  Fall through if no     \
      extended interrupt is present. */                                 \
}

// Mask an extended interrupt
#define HAL_EXTENDED_INTERRUPT_MASK(vector)                             \
{  /* Mask an interrupt source.  If 'vector' corresponds to an extended \
      interrupt, handle it and return.  Otherwise, fall through for     \
      normal processing. */                                             \
}

// Unmask an extended interrupt
#define HAL_EXTENDED_INTERRUPT_UNMASK(vector)                             \
{  /* Unmask an interrupt source.  If 'vector' corresponds to an extended \
      interrupt, handle it and return.  Otherwise, fall through for       \
      normal processing. */                                               \
}

// Acknowledge an extended interrupt
#define HAL_EXTENDED_INTERRUPT_ACKNOWLEDGE(vector)                      \
{  /* Ack an interrupt source.  If 'vector' corresponds to an extended  \
      interrupt, handle it and return.  Otherwise, fall through for     \
      normal processing. */                                             \
}

// Configure an extended interrupt
#define HAL_EXTENDED_INTERRUPT_CONFIGURE(vector, level, up)                  \
{  /* Configure an interrupt source.  If 'vector' corresponds to an extended \
      interrupt, handle it and return.  Otherwise, fall through for          \
      normal processing. */                                                  \
}

// Configure an extended interrupt
#define HAL_EXTENDED_INTERRUPT_SET_LEVEL(vector, level)                      \
{  /* Configure an interrupt source.  If 'vector' corresponds to an extended \
      interrupt, handle it and return.  Otherwise, fall through for          \
      normal processing. */                                                  \
}

#endif // CYGONCE_HAL_PLATFORM_INTS_H
