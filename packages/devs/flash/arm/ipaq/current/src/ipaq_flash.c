//====================================================================
//
//      ipaq_flash.c
//
//      FLASH memory - Hardware support on Intel StrongARM SA1110
//
//====================================================================
//####COPYRIGHTBEGIN####
//                                                                          
//-------------------------------------------                              
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
//-------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):      gthomas
// Original data:  gthomas
// Contributors:   gthomas
// Date:           2001-03-01
//
//####DESCRIPTIONEND####
//
//====================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/ipaq.h>

void
ipaq_flash_enable(void *start, void *end)
{
    ipaq_EGPIO(SA1110_EIO_VPP, SA1110_EIO_VPP_ON);
}

void
ipaq_flash_disable(void *start, void *end)
{
    ipaq_EGPIO(SA1110_EIO_VPP, SA1110_EIO_VPP_OFF);
}
