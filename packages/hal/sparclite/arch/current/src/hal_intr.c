//==========================================================================
//
//      hal_intr.c
//
//      SPARClite Architecture specific interrupt dispatch tables
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
// Author(s):    hmt
// Contributors: hmt
// Date:         1999-02-20
// Purpose:      Interrupt handler tables for SPARClite.
//              
//####DESCRIPTIONEND####
//
//==========================================================================


#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_arch.h>

#include <cyg/infra/cyg_ass.h> // for CYG_FAIL() below

// ------------------------------------------------------------------------
// First level C default interrupt handler.

//static int count = 0;

cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    return 0; // 0x1def0000 + vector + (count += 0x0100);
}

// ------------------------------------------------------------------------
// First level C exception handler.

externC void __handle_exception (void);

externC HAL_SavedRegisters *_hal_registers;

void cyg_hal_exception_handler(CYG_ADDRWORD vector, CYG_ADDRWORD data,
                               CYG_ADDRWORD stackpointer )
{
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    // Set the pointer to the registers of the current exception
    // context. At entry the GDB stub will expand the
    // HAL_SavedRegisters structure into a (bigger) register array.
    _hal_registers = (HAL_SavedRegisters *)stackpointer;

    __handle_exception();

#elif defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && \
      defined(CYGPKG_HAL_EXCEPTIONS)
    // We should decode the vector and pass a more appropriate
    // value as the second argument. For now we simply pass a
    // pointer to the saved registers. We should also divert
    // breakpoint and other debug vectors into the debug stubs.

    cyg_hal_deliver_exception( vector, stackpointer );

#else
    CYG_FAIL("Exception!!!");
#endif    
    return;
}

// ISR tables
volatile
CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_VSR_COUNT] = {
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,

    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,

    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,

    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,
    (CYG_ADDRESS)hal_default_isr,  /* 16 of these */

    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,

    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,
    (CYG_ADDRESS)cyg_hal_exception_handler,

    (CYG_ADDRESS)cyg_hal_exception_handler, /* 11 of these */
};

volatile
CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_VSR_COUNT] = {
 0x11da1a00, 0x11da1a01, 0x11da1a02, 0x11da1a03,
 0x11da1a04, 0x11da1a05, 0x11da1a06, 0x11da1a07,
 0x11da1a08, 0x11da1a09, 0x11da1a0a, 0x11da1a0b,
 0x11da1a0c, 0x11da1a0d, 0x11da1a0e, 0x11da1a0f,
 0xeeda1a00, 0xeeda1a01, 0xeeda1a02, 0xeeda1a03, 0xeeda1a04,
 0xeeda1a05, 0xeeda1a06, 0xeeda1a07, 0xeeda1a08, 0xeeda1a09,
 0xeeda1a0A
};

volatile
CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_VSR_COUNT] = {
    0,    0,    0,    0,
    0,    0,    0,    0,
    0,    0,    0,    0,
    0,    0,    0,    0,

    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,
    0,
};

// EOF hal_intr.c
