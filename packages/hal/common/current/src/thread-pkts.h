/* thread-packets.h -
   Optional packet processing for thread aware debugging.
   Externs as called by central packet switch routine
   */

/* 
 * Copyright (c) 1998,1999 Cygnus Solutions
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

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
