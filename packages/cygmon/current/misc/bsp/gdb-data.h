#ifndef __BSP_GDB_DATA_H__
#define __BSP_GDB_DATA_H__
//==========================================================================
//
//      gdb-data.h
//
//      Shared data specific to gdb stub.
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
// Purpose:      Shared data specific to gdb stub.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#ifndef __ASSEMBLER__
typedef int (*gdb_memproc_t)(void *__addr,    /* start addr of memory to read/write */
			     int  __asid,     /* address space id */
			     int  __size,     /* size of individual read/write ops */
			     int  __n,        /* number of read/write operations */
			     void *__buf);    /* result(read)/src(write) buffer */


typedef void (*gdb_regproc_t)(int  __regno,   /* Register number */
			      void *__regs,   /* pointer to saved regs */
			      void *__val);   /* pointer to register value */


typedef struct {
    /*
     * An application may override the standard BSP memory
     * read and/or write routines with these hooks.
     */
    gdb_memproc_t	__mem_read_hook;
    gdb_memproc_t	__mem_write_hook;

    /*
     * An application may override the standard BSP register
     * access routines with these hooks.
     */
    gdb_regproc_t	__reg_get_hook;
    gdb_regproc_t	__reg_set_hook;

    /*
     * An application may extend the gdb remote protocol by
     * installing hooks to handle unknown general query and
     * set packets ("q" pkts and 'Q' pkts) with these two hooks.
     */
    void                (*__pkt_query_hook)(unsigned char *__pkt);
    void                (*__pkt_set_hook)(unsigned char *__pkt);

    /*
     * An application may also extend the gdb remote protocol
     * by installing a hook to handle all unknown packets.
     */
    void                (*__pkt_hook)(unsigned char *__pkt);

    /*
     * The above hooks for receiving packets will probably need
     * a mechanism to respond. This vector is provided to allow
     * an application to append data to the outgoing packet which
     * will be sent after the above hooks are called.
     *
     * This vector uses a printf-like format string followed by
     * some number of arguments.
     */
    void                (*__pkt_append)(char *fmt, ...);

    /*
     * An application can read/write from/to gdb console
     * through these vectors.
     *
     * NB: console read is not supported and will block forever.
     */
    int                 (*__console_read)(char *__buf, int len);
    int                 (*__console_write)(char *__buf, int len);

} gdb_data_t;


extern gdb_data_t *__get_gdb_data(void);

#endif /* __ASSEMBLER__ */

#endif // __BSP_GDB_DATA_H__
