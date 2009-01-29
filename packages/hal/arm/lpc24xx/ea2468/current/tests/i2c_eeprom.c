//==========================================================================
//
//        ea2468_i2c_test.c
//
//        Simple I2C test of I2C eeprom on LPC2468 OEM board
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Uwe Kindler
// Contributors:
// Date:          2008-09-16
// Description:   Simple I2C read/write test of I2C eeprom
//####DESCRIPTIONEND####


//===========================================================================
//                                INCLUDES
//===========================================================================
#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>         // test macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_io.h>

// Package requirements
#if defined(CYGHWR_HAL_ARM_LPC24XX_I2C0_SUPP) && defined(CYGPKG_KERNEL)

#include <pkgconf/kernel.h>
#include <cyg/io/io.h>
#include <cyg/io/i2c.h>

// Package option requirements
#if defined(CYGFUN_KERNEL_API_C)

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/kernel/kapi.h>


//===========================================================================
//                               DATA TYPES
//===========================================================================
typedef struct st_thread_data
{
    cyg_thread   obj;
    long         stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];
    cyg_handle_t hdl;
} thread_data_t;


//===========================================================================
//                              LOCAL DATA
//===========================================================================
#define EEPROM_SIZE 0x8000
cyg_thread_entry_t i2c0_thread;
thread_data_t      i2c0_thread_data;
static cyg_uint8   write_buf[EEPROM_SIZE];
static cyg_uint8   read_buf[EEPROM_SIZE];

//===========================================================================
//                             I2C THREAD
//===========================================================================
void i2c0_thread(cyg_addrword_t data)
{
    cyg_uint16 i;
    cyg_uint32 result;

    diag_printf("I2C EEPROM read/write test at %d bit/s\n",
    		    CYGNUM_HAL_ARM_LPC24XX_I2C0_BUS_FREQ);
    diag_printf("Initialising write buffer\n");
    for (i = 0; i < EEPROM_SIZE; ++i)
    {
        write_buf[i] = (i & 0xFF);
    }

    diag_printf("Writing 32768 bytes to I2C EEPROM...\n");
    result = hal_lpc_eeprom_write(0, write_buf, EEPROM_SIZE, true);
    if (result != (EEPROM_SIZE))
    {
        CYG_TEST_FAIL_FINISH("Error writing to EEPROM");
    }

    diag_printf("Reading 32768 bytes from I2C EEPROM...\n");
    result = hal_lpc_eeprom_read(0, read_buf, EEPROM_SIZE, true);
    if (result != EEPROM_SIZE)
    {
        CYG_TEST_FAIL_FINISH("Error reading from EEPROM");
    }

    for (i = 0; i < EEPROM_SIZE; ++i)
    {
        if (write_buf[i] != read_buf[i])
        {
            diag_printf("Buffers don't match - write_buf[%d]=0x%02X, "
                        "read_buf[%d]=0x%02X\n",
                         i, write_buf[i], i, read_buf[i]);
            CYG_TEST_FAIL_FINISH("Buffers don't match");
        }
    }

    CYG_TEST_PASS_FINISH("I2C EEPROM read/write test OK");
}


void
cyg_start(void)
{
    CYG_TEST_INIT();

    //
    // create the threads that access the CAN device driver
    //
    cyg_thread_create(4, i2c0_thread,
                      (cyg_addrword_t) 0,
                      "i2c0_thread",
                      (void *) i2c0_thread_data.stack,
                      1024 * sizeof(long),
                      &i2c0_thread_data.hdl,
                      &i2c0_thread_data.obj);

    cyg_thread_resume(i2c0_thread_data.hdl);

    cyg_scheduler_start();
}

#else // CYGFUN_KERNEL_API_C
#define N_A_MSG "Needs kernel C API"
#endif

#else // CYGPKG_IO_CAN && CYGPKG_KERNEL
#define N_A_MSG "Needs LPC2xxx I2C support and Kernel"
#endif

#ifdef N_A_MSG
void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( N_A_MSG);
}
#endif // N_A_MSG

// EOF can_tx.c

//---------------------------------------------------------------------------
// eof i2c_test.c

