//========================================================================
//
//      thread-pkts.h
//
//      Optional packet processing for thread aware debugging.
//
//========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Date:          1998-08-25
// Purpose:       
// Description:   Optional packet processing for thread aware debugging.
//                Externs as called by central packet switch routine
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================


/* The DEBUG_THREADS flag is usually set in board.h */

#if DEBUG_THREADS  /* activate this in board.h */

/* export the thread id's sent by gdb: */
extern int _gdb_cont_thread, _gdb_general_thread ;

extern char * stub_pack_Tpkt_threadid(char * pkt) ;

extern long stub_get_currthread(void) ;
extern int  stub_lock_scheduler(int lock, int kind, long id) ;

extern void stub_pkt_changethread(char * inbuf,
                                 char * outbuf,
                                 int bufmax) ;


extern void stub_pkt_getthreadlist(char * inbuf,
                                  char * outbuf,
                                  int bufmax) ;


extern void stub_pkt_getthreadinfo(
                  char * inbuf,
                  char * outbuf,
                  int bufmax) ;

extern void stub_pkt_thread_alive(
                                  char * inbuf,
                                  char * outbuf,
                                  int bufmax) ;

extern void stub_pkt_currthread(
                                char * inbuf,
                                char * outbuf,
                                int bufmax) ;

#define STUB_PKT_GETTHREADLIST(A,B,C) { stub_pkt_getthreadlist(A,B,C); }
#define STUB_PKT_GETTHREADINFO(A,B,C) { stub_pkt_getthreadinfo(A,B,C); }
#define STUB_PKT_CHANGETHREAD(A,B,C)  { stub_pkt_changethread(A,B,C);  }
#define STUB_PKT_THREAD_ALIVE(A,B,C)  { stub_pkt_thread_alive(A,B,C);  }
#define STUB_PKT_CURRTHREAD(A,B,C)    { stub_pkt_currthread(A,B,C);    }
#define PACK_THREAD_ID(PTR)           { PTR = stub_pack_Tpkt_threadid (PTR); }
#else
#define STUB_PKT_GETTHREADLIST(A,B,C) {}
#define STUB_PKT_GETTHREADINFO(A,B,C) {}
#define STUB_PKT_CHANGETHREAD(A,B,C)  {}
#define STUB_PKT_THREAD_ALIVE(A,B,C)  {}
#define STUB_PKT_CURRTHREAD(A,B,C)    {}
#define PACK_THREAD_ID(PTR)           {}
#endif
