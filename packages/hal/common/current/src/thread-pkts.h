/* thread-packets.h -
   Optional packet processing for thread aware debugging.
   Externs as called by central packet switch routine
   */

/* 
 * Copyright (c) 1998 Cygnus Support
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

#define STUB_PKT_GETTHREADLIST(a,b,c)  { stub_pkt_getthreadlist(a,b,c);}
#define STUB_PKT_GETTHREADINFO(a,b,c) { stub_pkt_getthreadinfo(a,b,c);}
#define STUB_PKT_CHANGETHREAD(a,b,c) {  stub_pkt_changethread(a,b,c);}
#define STUB_PKT_THREAD_ALIVE(a,b,c)  { stub_pkt_thread_alive(a,b,c);}
#define STUB_PKT_CURRTHREAD(a,b,c)    { stub_pkt_currthread(a,b,c);}
#else
#define STUB_PKT_GETTHREADLIST(a,b,c)  {}
#define STUB_PKT_GETTHREADINFO(a,b,c) {}
#define STUB_PKT_CHANGETHREAD(a,b,c)  {}
#define STUB_PKT_THREAD_ALIVE(a,b,c)   {}
#define STUB_PKT_CURRTHREAD(a,b,c)    {}
#endif
