#ifndef __BSP_COMMON_GDB_H__
#define __BSP_COMMON_GDB_H__
//==========================================================================
//
//      gdb.h
//
//      Definitions for GDB stub.
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
// Purpose:      Definitions for GDB stub.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include "gdb-cpu.h"
#include <bsp/gdb-data.h>

#ifndef DEBUG_STUB
#define DEBUG_STUB 0
#endif

/*
 *  These need to match the same in devo/gdb/target.h
 */
#define  TARGET_SIGNAL_INT  2
#define  TARGET_SIGNAL_ILL  4
#define  TARGET_SIGNAL_TRAP 5
#define  TARGET_SIGNAL_ABRT 6
#define  TARGET_SIGNAL_FPE  8
#define  TARGET_SIGNAL_BUS  10
#define  TARGET_SIGNAL_SEGV 11


/*
 *  Socket to use for tcp/ip debug channel.
 */
#define GDB_TCP_SOCKET  1000


#ifndef __ASSEMBLER__

/* generic gdb protocol handler */
extern void _bsp_gdb_handler(int exc_nr, void *saved_regs);
extern gdb_data_t _bsp_gdb_data;


/* start forming an outgoing gdb packet */
/* if ack is true, prepend an ack character */
extern void _gdb_pkt_start(int ack);

/* Append data to packet using formatted string. */
extern void _gdb_pkt_append(char *fmt, ...);

/* Calculate checksum and append to end of packet. */
extern void _gdb_pkt_end(void);

/* Send the packet. Blocks waiting for ACK */
extern void _gdb_pkt_send(void);
#endif

#endif //__BSP_COMMON_GDB_H__
