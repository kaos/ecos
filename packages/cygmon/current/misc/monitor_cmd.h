//==========================================================================
//
//      monitor_cmd.h
//
//      Monitor command definitions for the CygMON ROM monitor
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
// Purpose:      Monitor command definitions for the CygMON ROM monitor
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#ifndef MONITOR_CMD_H
#define MONITOR_CMD_H

#ifndef ASM
typedef enum {
  INVOCATION, USAGE, SHORT_HELP, LONG_HELP
} cmdmode_t;

struct cmdentry
{
  char *alias;
  char *cmd;
  int (*function) (cmdmode_t);
};

extern struct cmdentry cmdtab [];

extern int monitor_loop (void);

extern char inbuf[];

extern char **argvect;

typedef int (*srec_input_func_t)(void);

extern int load_srec(srec_input_func_t inp_func);

#ifdef USE_HELP
extern void usage (char *string);
extern void short_help (char *string);
extern void long_help (char *string);
extern void example (char *string);
#else
#define usage(x) no_help_usage ()
#define short_help(x) no_help()
#define long_help(x) no_help()
#define example(x)
extern void no_help (void);
extern void no_help_usage (void);
#endif
extern int help_cmd (cmdmode_t mode);
extern int mem_cmd (cmdmode_t mode);

extern int dump_cmd (cmdmode_t mode);
extern int ethaddr_cmd (cmdmode_t mode);
extern int ipaddr_cmd (cmdmode_t mode);
extern int tcpport_cmd (cmdmode_t mode);
extern int load_cmd (cmdmode_t mode);
extern int reg_cmd (cmdmode_t mode);
extern int go_cmd (cmdmode_t mode);
extern int othernames_cmd (cmdmode_t mode);
extern int step_cmd (cmdmode_t mode);
extern int transfer_cmd (cmdmode_t mode);
extern int timer_cmd (cmdmode_t mode);
extern int disassemble_cmd (cmdmode_t mode);
extern int breakpoint_cmd (cmdmode_t mode);
extern int clear_breakpoint_cmd (cmdmode_t mode);
extern int memusage_cmd (cmdmode_t mode);
extern int set_serial_port_cmd (cmdmode_t mode);
extern int set_serial_speed_cmd (cmdmode_t mode);
extern int version_cmd (cmdmode_t mode);
extern int cache_cmd (cmdmode_t mode);
extern int set_term_cmd (cmdmode_t mode);
extern int reset_cmd (cmdmode_t mode);
extern int copy_cmd (cmdmode_t mode);
extern int fill_cmd (cmdmode_t mode);
extern int set_program_args_cmd (cmdmode_t mode);
extern int swapmem_cmd (cmdmode_t mode);
extern int checksumcmd (cmdmode_t mode);
extern int int_cmd (cmdmode_t mode);
#endif

#endif
