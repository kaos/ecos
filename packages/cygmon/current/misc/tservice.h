#ifndef __TSERVICE_H__
#define __TSERVICE_H__
//==========================================================================
//
//      tservice.h
//
//      These are the core functions are expected to be provided by the
//      target dependent services.
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
// Author(s):    
// Contributors: gthomas
// Date:         1999-10-20
// Purpose:      
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

/* These are the core functions are expected to be provided by the
   target dependent services. They are used by both cygmon and by
   libstub
*/


extern int read_memory (mem_addr_t *src, int size, int amt, char *dst);

extern int write_memory (mem_addr_t *dst, int size, int amt, char *src);



extern void set_breakpoint (struct bp *bp);
extern void clear_breakpoint (struct bp *bp);


#ifndef HAVE_BSP
extern void set_pc (target_register_t pc);
#endif

extern target_register_t next_step_pc (void);

extern void enable_interrupts (void);

#endif // __TSERVICE_H__


extern void initialize_mon(void);
