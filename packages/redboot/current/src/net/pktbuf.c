//==========================================================================
//
//      net/pktbuf.c
//
//      Stand-alone network packet support for RedBoot
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <net/net.h>

#define MAX_PKTBUF CYGNUM_REDBOOT_NETWORKING_MAX_PKTBUF

#define BUFF_STATS 1

#if BUFF_STATS
int max_alloc = 0;
int num_alloc = 0;
int num_free  = 0;
#endif

static pktbuf_t  pktbuf_list[MAX_PKTBUF];
static word      bufdata[MAX_PKTBUF][ETH_MAX_PKTLEN/2 + 1];
static pktbuf_t *free_list;


/*
 * Initialize the free list.
 */
void
__pktbuf_init(void)
{
    int  i;
    word *p;
    static int init = 0;

    if (init) return;
    init = 1;

    for (i = 0; i < MAX_PKTBUF; i++) {
	p = bufdata[i];
	if ((((unsigned long)p) & 2) != 0)
	    ++p;
	pktbuf_list[i].buf = p;
	pktbuf_list[i].bufsize = ETH_MAX_PKTLEN;
	pktbuf_list[i].next = free_list;
	free_list = &pktbuf_list[i];
    }
}

void
__pktbuf_dump(void)
{
    int i;
    for (i = 0; i < MAX_PKTBUF; i++) {
        printf("Buf[%d]/%p: buf: %p, len: %d/%d, next: %p\n", 
               i,
               (void*)&pktbuf_list[i],
               (void*)pktbuf_list[i].buf,
               pktbuf_list[i].bufsize,
               pktbuf_list[i].pkt_bytes,
               (void*)pktbuf_list[i].next);
    }
    printf("Free list = %p\n", (void*)free_list);
}

/*
 * simple pktbuf allocation
 */
pktbuf_t *
__pktbuf_alloc(int nbytes)
{
    pktbuf_t *p = free_list;

    if (p) {
	free_list = p->next;
	p->ip_hdr  = (ip_header_t *)p->buf;
	p->tcp_hdr = (tcp_header_t *)(p->ip_hdr + 1);
	p->pkt_bytes = 0;
#if BUFF_STATS
	++num_alloc;
	if ((num_alloc - num_free) > max_alloc)
	    max_alloc = num_alloc - num_free;
#endif
    }
    return p;
}


/*
 * free a pktbuf.
 */
void
__pktbuf_free(pktbuf_t *pkt)
{
#if BUFF_STATS
    --num_alloc;
#endif
#ifdef BSP_LOG
    {
	int i;
	word *p;

	for (i = 0; i < MAX_PKTBUF; i++) {
	    p = bufdata[i];
	    if ((((unsigned long)p) & 2) == 0)
		++p;
	    if (p == (word *)pkt)
		break;
	}
	if (i < MAX_PKTBUF) {
	    BSPLOG(bsp_log("__pktbuf_free: bad pkt[%x].\n", pkt));
	    BSPLOG(while(1));
	}
    }
#endif
    pkt->next = free_list;
    free_list = pkt;
}


