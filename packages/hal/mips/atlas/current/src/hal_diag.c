/*=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:        nickg, dmoseley
// Date:        1998-03-02
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_diag.h>

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/hal_io.h>

//-----------------------------------------------------------------------------
// Select which diag channels to use

//#define CYG_KERNEL_DIAG_LCD
#define CYG_KERNEL_DIAG_SERIAL

/*---------------------------------------------------------------------------*/

void hal_diag_led(int x)
{
  HAL_WRITE_UINT32(HAL_DISPLAY_LEDBAR, x);
#if !defined(CYG_KERNEL_DIAG_LCD)
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIWORD, x);
#endif
}

externC void diag_write_string (const char*);

#ifdef CYG_KERNEL_DIAG_SERIAL
extern void cyg_hal_plf_comms_init(void);
extern void cyg_hal_plf_serial_putc(void*, cyg_uint8);
extern cyg_uint8 cyg_hal_plf_serial_getc(void*);
#endif

void hal_diag_init(void)
{
#if defined(CYGSEM_HAL_ROM_MONITOR) && !defined(CYG_KERNEL_DIAG_SERIAL)
  // It's handy to have the LCD initialized at reset when using it
  // for debugging output.
  // The serial port likely doesn't work yet.  Let's wait.
  diag_write_string ("eCos ROM   " __TIME__ "\n");
  diag_write_string (__DATE__ "\n");
#endif

#if defined(CYG_KERNEL_DIAG_SERIAL)
  cyg_hal_plf_comms_init();
#endif
}

#if defined(CYG_KERNEL_DIAG_LCD)
static void hal_diag_clear_lcd(void)
{
  volatile int i = 0x20000;
  while (--i) ;

  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS0, ' ');
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS1, ' ');
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS2, ' ');
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS3, ' ');
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS4, ' ');
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS5, ' ');
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS6, ' ');
  HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS7, ' ');
}
#endif /* defined(CYG_KERNEL_DIAG_LCD) */

void hal_diag_write_char(char c)
{
#if defined(CYG_KERNEL_DIAG_LCD)
  static volatile CYG_WORD* reg = HAL_DISPLAY_ASCIIPOS0;
#endif

  unsigned long __state;

  HAL_DISABLE_INTERRUPTS(__state);

  if(c == '\n')
    {
#if defined(CYG_KERNEL_DIAG_LCD)
      reg = HAL_DISPLAY_ASCIIPOS0;
      hal_diag_clear_lcd();
#endif
#if defined (CYG_KERNEL_DIAG_SERIAL)
      cyg_hal_plf_serial_putc(NULL, '\r');
      cyg_hal_plf_serial_putc(NULL, '\n');
#endif
    }
  else if (c == '\r')
    {
      // Ignore '\r'
    }
  else
    {
#if defined(CYG_KERNEL_DIAG_LCD)
      if (reg == HAL_DISPLAY_ASCIIPOS0)
        hal_diag_clear_lcd();

      HAL_WRITE_UINT32(reg, c);

      // Advance to next LED position.
      if (reg == HAL_DISPLAY_ASCIIPOS0)
        reg = HAL_DISPLAY_ASCIIPOS1;
      else if (reg == HAL_DISPLAY_ASCIIPOS1)
        reg = HAL_DISPLAY_ASCIIPOS2;
      else if (reg == HAL_DISPLAY_ASCIIPOS2)
        reg = HAL_DISPLAY_ASCIIPOS3;
      else if (reg == HAL_DISPLAY_ASCIIPOS3)
        reg = HAL_DISPLAY_ASCIIPOS4;
      else if (reg == HAL_DISPLAY_ASCIIPOS4)
        reg = HAL_DISPLAY_ASCIIPOS5;
      else if (reg == HAL_DISPLAY_ASCIIPOS5)
        reg = HAL_DISPLAY_ASCIIPOS6;
      else if (reg == HAL_DISPLAY_ASCIIPOS6)
        reg = HAL_DISPLAY_ASCIIPOS7;
      else // reg == HAL_DISPLAY_ASCIIPOS7 or UNKNOWN
        reg = HAL_DISPLAY_ASCIIPOS0;
#endif
#if defined(CYG_KERNEL_DIAG_SERIAL)
      cyg_hal_plf_serial_putc(NULL, c);
#endif
    }

  HAL_RESTORE_INTERRUPTS(__state);
}

void hal_diag_read_char(char* c)
{
  *c = cyg_hal_plf_serial_getc(NULL);
}

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */
