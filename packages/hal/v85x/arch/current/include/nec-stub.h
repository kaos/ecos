#ifndef CYGONCE_HAL_NEC_STUB_H
#define CYGONCE_HAL_NEC_STUB_H
//========================================================================
//
//      nec-stub.h
//
//      NEC-specific definitions for generic stub
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Red Hat, nickg
// Contributors:  Red Hat, nickg
// Date:          1998-06-08
// Purpose:       
// Description:   NEC-specific definitions for generic stub
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================


#include <pkgconf/system.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUMREGS    66
#define REGSIZE(X) 4
typedef unsigned long target_register_t;

enum regnames {
    R0,  R1,  SP,  R3,  R4,  R5,  R6,  R7,
    R8,  R9 , R10, R11, R12, R13, R14, R15,
    R16, R17, R18, R19, R20, R21, R22, R23,
    R24, R25, R26, R27, R28, R29, R30, R31,
    EIPC, EIPSW, FEPC, FEPSW, ECR, PSW, SR6, SR7,
    SR8,  SR9 , SR10, SR11, SR12, SR13, SR14, SR15,
    SR16, SR17, SR18, SR19, SR20, SR21, SR22, SR23,
    SR24, SR25, SR26, SR27, SR28, SR29, SR30, SR31,
    PC, FP
};

typedef enum regnames regnames_t;

/* Given a trap value TRAP, return the corresponding signal. */
extern int __computeSignal (unsigned int trap_number);

/* Return the SPARC trap number corresponding to the last-taken trap. */
extern int __get_trap_number (void);

/* Return the currently-saved value corresponding to register REG. */
extern target_register_t get_register (regnames_t reg);

/* Store VALUE in the register corresponding to WHICH. */
extern void put_register (regnames_t which, target_register_t value);

/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */
extern void set_pc (target_register_t pc);

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */
void __single_step (void);

/* Clear the single-step state. */
void __clear_single_step (void);

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */
extern int __is_breakpoint_function (void);

/* Skip the current instruction. */
extern void __skipinst (void);

extern void __install_breakpoints (void);

extern void __clear_breakpoints (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // ifndef CYGONCE_HAL_NEC_STUB_H
