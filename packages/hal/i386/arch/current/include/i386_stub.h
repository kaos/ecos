#ifndef CYGONCE_HAL_I386_STUB_H
#define CYGONCE_HAL_I386_STUB_H
//==========================================================================
//
//      i386_stub.h
//
//      i386/PC GDB stub support
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
// Author(s):    pjo
// Contributors: pjo, nickg
// Date:         1999-10-15
// Purpose:      i386/PC GDB stub support
// Description:  Definitions for the GDB stubs. Most of this comes from
//               the original libstub code.
//              
// Usage:
//               #include <cyg/hal/plf_intr.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================


#define NUMREGS 				(16)
#define REGSIZE(x)				(((x) < CS)? 4 : 2)


enum regnames
{	EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
	PC /* also known as eip */,
	PS /* also known as eflags */,
	CS, SS, DS, ES, FS, GS
};

typedef enum regnames regnames_t ;
typedef unsigned long target_register_t ;

#define PS_C			0x00000001
#define PS_Z			0x00000040
#define PS_V			0x00000080
#define PS_T			0x00000100

#define SP				(ESP)
#define EIP				(PC)


/* Find out what our last trap was. */
extern int __get_trap_number(void) ;

/* Given a trap number, compute the signal code for it. */
extern int __computeSignal(unsigned int trap_number) ;

/* Enable single stepping after the next user resume instruction. */
extern void __single_step(void) ;
extern void __clear_single_step(void) ;

extern void __install_breakpoints(void) ;
extern void __clear_breakpoints(void) ;


//---------------------------------------------------------------------------
#endif /* CYGONCE_HAL_I386_STUB_H */
// End of i386_stub.h
