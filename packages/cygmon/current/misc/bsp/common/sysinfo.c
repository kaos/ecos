//==========================================================================
//
//      sysinfo.c
//
//      Interface for getting system information.
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
// Purpose:      Interface for getting system information.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <stdlib.h>
#include <bsp/bsp.h>
#include "bsp_if.h"


/*
 * In order to construct the _bsp_memory_list, some board specific code
 * may have to size RAM regions. To do this easily and reliably, the code
 * needs to run from ROM before .bss and .data sections are initialized.
 * This leads to the problem of where to store the results of the memory
 * sizing tests. In this case, the _bsp_init_stack routine which sizes
 * memory and sets up the stack will place the board-specific information
 * on the stack and return with the stack pointer pointing to a pointer to
 * the information. That is, addr_of_info = *(void **)sp. The architecture
 * specific code will then copy that pointer to the _bsp_ram_info_ptr variable
 * after initializing the .data and .bss sections.
 */
void *_bsp_ram_info_ptr;

/*
 *  Name of CPU and board. Should be overridden by arch/board specific
 *  code.
 */
struct bsp_platform_info _bsp_platform_info = {
    "Unknown",  /* cpu name */
    "Unknown",  /* board name */
    ""          /* extra info */
};


/*
 *  Information about possible data cache. Should be overridden by
 *  by arch/board specific code.
 */
struct bsp_cachesize_info _bsp_dcache_info = {
    0, 0, 0
};


/*
 *  Information about possible instruction cache. Should be overridden by
 *  by arch/board specific code.
 */
struct bsp_cachesize_info _bsp_icache_info = {
    0, 0, 0
};


/*
 *  Information about possible secondary cache. Should be overridden by
 *  by arch/board specific code.
 */
struct bsp_cachesize_info _bsp_scache_info = {
    0, 0, 0
};



int
_bsp_sysinfo(enum bsp_info_id id, va_list ap)
{
    int  index, rval = 0;
    void *p;

    switch (id) {
      case BSP_INFO_PLATFORM:
	p = va_arg(ap, void *);
	*(struct bsp_platform_info *)p = _bsp_platform_info;
	break;

      case BSP_INFO_DCACHE:
	p = va_arg(ap, void *);
	*(struct bsp_cachesize_info *)p = _bsp_dcache_info;
	break;

      case BSP_INFO_ICACHE:
	p = va_arg(ap, void *);
	*(struct bsp_cachesize_info *)p = _bsp_icache_info;
	break;

      case BSP_INFO_SCACHE:
	p = va_arg(ap, void *);
	*(struct bsp_cachesize_info *)p = _bsp_scache_info;
	break;

      case BSP_INFO_MEMORY:
	index = va_arg(ap, int);
	p = va_arg(ap, void *);

	if (index >= 0 && index < _bsp_num_mem_regions)
	    *(struct bsp_mem_info *)p = _bsp_memory_list[index];
	else
	    rval = -1;
	break;

      case BSP_INFO_COMM:
	index = va_arg(ap, int);
	p = va_arg(ap, void *);

	if (index >= 0 && index < _bsp_num_comms)
	    *(struct bsp_comm_info *)p = _bsp_comm_list[index].info;
	else if (index == _bsp_num_comms && _bsp_net_channel != NULL)
	    *(struct bsp_comm_info *)p = _bsp_net_channel->info;
	else
	    rval = -1;
	break;

      default:
	rval =  -1;
    }

    return rval;
}



