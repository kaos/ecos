#ifndef CYGONCE_HAL_MIPS_STUB_H
#define CYGONCE_HAL_MIPS_STUB_H
//========================================================================
//
//      mips-stub.h
//
//      MIPS-specific definitions for generic stub
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
// Contributors:  Red Hat, nickg, dmoseley
// Date:          1998-06-08
// Purpose:       
// Description:   MIPS-specific definitions for generic stub
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================


#include <pkgconf/system.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUMREGS    90

#if defined(__mips64)
  #define REGSIZE(X) 8
  typedef unsigned long long target_register_t;
#elif defined(CYGPKG_HAL_MIPS_VR4300)
  // Even though we are only working with 32 bit registers, GDB expects 64 bits
  #define REGSIZE(X) 8
  typedef unsigned long target_register_t;
  // We need to sign-extend the registers so GDB doesn't get confused.
  #define CYGARC_SIGN_EXTEND_REGISTERS
#else
  #define REGSIZE(X) 4
  typedef unsigned long target_register_t;
#endif

enum regnames {
        REG_ZERO,   REG_AT,     REG_V0,     REG_V1,     REG_A0,     REG_A1,     REG_A2,     REG_A3,
        REG_T0,     REG_T1,     REG_T2,     REG_T3,     REG_T4,     REG_T5,     REG_T6,     REG_T7,
        REG_S0,     REG_S1,     REG_S2,     REG_S3,     REG_S4,     REG_S5,     REG_S6,     REG_S7,
        REG_T8,     REG_T9,     REG_K0,     REG_K1,     REG_GP,     REG_SP,     REG_S8,     REG_RA,
        REG_SR,     REG_LO,     REG_HI,     REG_BAD,    REG_CAUSE,  REG_PC,
        REG_CONFIG = 84,    REG_CACHE,  REG_DEBUG,  REG_DEPC,   REG_EPC
};
#define USE_LONG_NAMES_FOR_ENUM_REGNAMES

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
#if !defined(SET_PC_PROTOTYPE_EXISTS) && !defined(set_pc)
#define SET_PC_PROTOTYPE_EXISTS
extern void set_pc (target_register_t pc);
#endif

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */
#ifndef __single_step
void __single_step (void);
#endif

/* Clear the single-step state. */
void __clear_single_step (void);

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */
#ifndef __is_breakpoint_function
extern int __is_breakpoint_function (void);
#endif

/* Skip the current instruction. */
extern void __skipinst (void);

extern void __install_breakpoints (void);

extern void __clear_breakpoints (void);

extern void __install_breakpoint_list (void);

extern void __clear_breakpoint_list (void);

#ifdef __cplusplus
}      /* extern "C" */
#endif

#endif // ifndef CYGONCE_HAL_MIPS_STUB_H
