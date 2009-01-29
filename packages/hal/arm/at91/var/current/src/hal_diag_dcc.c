/*=============================================================================
//
//      hal_diag_dcc.c
//
//      HAL diagnostic output via the DCC interface.
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Andrew Lunn
// Contributors:jskov, gthomas
// Date:        2008-06-15
// Purpose:     HAL diagnostic output via DCC.
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types

#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/hal_diag.h>

#define DCC_TX_BUSY 2
#define DCC_RX_READY 1

//-----------------------------------------------------------------------------

static void
cyg_hal_plf_dcc_putc(void * __ch_data, char ch)
{
  unsigned int status;
  CYG_UNUSED_PARAM(void *, __ch_data);

  CYGARC_HAL_SAVE_GP();
  
  do {
    __asm__ volatile ( "mrc p14,0, %0, c0, c0\n" : "=r" (status));
  } while ( status & DCC_TX_BUSY );
  __asm__( "mcr p14,0, %0, c1, c0\n" : : "r" (ch));
  
  CYGARC_HAL_RESTORE_GP();
}

static cyg_bool
cyg_hal_plf_dcc_getc_nonblock(cyg_uint8* ch)
{
  cyg_uint32 status;
  cyg_uint32 c;
  
  __asm__( "mrc p14,0, %0, c0, c0\n" : "=r" (status));
  
  if (status & DCC_RX_READY) {
    __asm__( "mrc p14,0, %0, c1, c0\n" : "=r" (c));
    *ch = (char )c;
    return true;
  } else 
    return false;
}

static cyg_uint8
cyg_hal_plf_dcc_getc(void* __ch_data)
{
  cyg_uint8 ch;
  CYG_UNUSED_PARAM(void *, __ch_data);
  CYGARC_HAL_SAVE_GP();
  
  while(!cyg_hal_plf_dcc_getc_nonblock(&ch));
  
  CYGARC_HAL_RESTORE_GP();
  return ch;
}

static void
cyg_hal_plf_dcc_write(void* __ch_data, const cyg_uint8* __buf, 
                      cyg_uint32 __len)
{
  CYG_UNUSED_PARAM(void *, __ch_data);
  CYGARC_HAL_SAVE_GP();
  
  while(__len-- > 0)
    cyg_hal_plf_dcc_putc(NULL, *__buf++);
  
  CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_plf_dcc_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
  CYG_UNUSED_PARAM(void *, __ch_data);
  CYGARC_HAL_SAVE_GP();
  
  while(__len-- > 0)
    *__buf++ = cyg_hal_plf_dcc_getc(NULL);
  
  CYGARC_HAL_RESTORE_GP();
}

static cyg_bool
cyg_hal_plf_dcc_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
  int delay_count;
  cyg_bool res;
  CYG_UNUSED_PARAM(void *, __ch_data);

  CYGARC_HAL_SAVE_GP();

  delay_count = 100010; // delay in .1 ms steps

  for(;;) {
    res = cyg_hal_plf_dcc_getc_nonblock(ch);
    if (res || 0 == delay_count--)
      break;
    
    CYGACC_CALL_IF_DELAY_US(100);
  }
  
  CYGARC_HAL_RESTORE_GP();
  return res;
}

static int
cyg_hal_plf_dcc_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
  CYG_UNUSED_PARAM(void *, __ch_data);
  CYG_UNUSED_PARAM(__comm_control_cmd_t, __func);
  
  return 0;
}

static void
cyg_hal_plf_dcc_register(const int channel)
{
  hal_virtual_comm_table_t* comm;
  int cur;
  
  cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
  // Setup procs in the vector table
  
  CYGACC_CALL_IF_SET_CONSOLE_COMM(channel);
  comm = CYGACC_CALL_IF_CONSOLE_PROCS();
  CYGACC_COMM_IF_CH_DATA_SET(*comm, NULL);
  CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_dcc_write);
  CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_dcc_read);
  CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_dcc_putc);
  CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_dcc_getc);
  CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_dcc_control);
  CYGACC_COMM_IF_DBG_ISR_SET(*comm, NULL);
  CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_dcc_getc_timeout);

  // Restore to original console.
  CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}

void
cyg_hal_plf_dcc_init(const int channel)
{
  static int initialized = 0;
  
  if (initialized)
    return;
  
  initialized = 1;
  
  cyg_hal_plf_dcc_register(channel);
}

//-----------------------------------------------------------------------------
// End of hal_diag_dcc.c
