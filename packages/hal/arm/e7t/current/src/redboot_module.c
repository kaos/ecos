//==========================================================================
//
//        redboot_module.c
//
//        ARM E7T board RedBoot module wrapper
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
// Author(s):     gthomas
// Contributors:  gthomas, jskov
// Date:          2001-03-19
// Description:   AEB-2 FLASH module for RedBoot
//####DESCRIPTIONEND####

//
// This is the module 'wrapper' for RedBoot
//

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_stub.h>

// ARM AEB-2 module stuff

#ifdef CYGPKG_REDBOOT

#include <redboot.h>

#ifndef CHECKSUM
#define CHECKSUM 0x0
#endif

extern char __exception_handlers, __rom_data_end;

const char __title[] = "RedBoot";
const char __help[] = "RedBoot              " __DATE__;

struct ModuleHeader {
    cyg_uint32    magic;
    cyg_uint16    flags;
    cyg_uint8     major;
    cyg_uint8     minor;
    cyg_uint32    checksum;
    cyg_uint32    ro_base;
    cyg_uint32    ro_limit;
    cyg_uint32    rw_base;
    cyg_uint32    zi_base; 
    cyg_uint32    zi_limit;
    cyg_uint32    self;
    cyg_uint32    start;
    cyg_uint32    init;
    cyg_uint32    final;
    cyg_uint32    service;
    cyg_uint32    title;
    cyg_uint32    help;
    cyg_uint32    cmdtbl;
    cyg_uint32    swi_base;
    cyg_uint32    swi_handler;
};

const static struct ModuleHeader __hdr = {
    0x4D484944,                     // MHID
    2,                              // flags = auto start
    1,                              // major
    0,                              // minor
    CHECKSUM,                       // checksum
    (cyg_uint32) &__exception_handlers,         // start of module (read-only) image
    (cyg_uint32) &__rom_data_end,    // end of image
    0,                              // r/w base - unused
    0,                              // bss base - unused
    0,                              // bss limit - unused
    (cyg_uint32) &__hdr,            // self (for module identification)
    (cyg_uint32) &__exception_handlers,         // startup 
    0,                              // init - unused
    0,                              // final - unused
    0,                              // service - unused
    (cyg_uint32) &__title,          // title
    (cyg_uint32) &__help,           // help string
    0,                              // command table - unused
    0,                              // SWI table - unused
    0                               // SWI handler - unused
};

static void
__dummy(void *p)
{
}

void __dummy_init(void)
{
    __dummy((void*)&__hdr);
}

_RedBoot_init(__dummy_init, RedBoot_INIT_LAST);


#else
#error "Stand-alone RedBoot only"
#endif
