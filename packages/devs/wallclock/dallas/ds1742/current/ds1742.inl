//==========================================================================
//
//      devs/wallclock/ds1742.inl
//
//      Wallclock implementation for Dallas 1742
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
// Author(s):     jskov
// Contributors:  jskov
// Date:          2000-05-25
// Purpose:       Wallclock driver for Dallas 1742
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef DS_BASE
# error "Need to know base of DS1742 RAM"
#endif

#include <cyg/infra/cyg_type.h>         // Common type definitions and support
#include <cyg/hal/hal_io.h>             // IO macros

// Offsets from DS_BASE
#define DS_SECONDS       0x7f9          // control bit!
#define DS_SECONDS_MASK  0x7f
#define DS_MINUTES       0x7fa
#define DS_HOUR          0x7fb
#define DS_DAY           0x7fc          // control bits
#define DS_DAY_MASK      0x07
#define DS_DATE          0x7fd
#define DS_MONTH         0x7fe
#define DS_YEAR          0x7ff
#define DS_CENTURY       0x7f8          // control bits!
#define DS_CENTURY_MASK  0x3f

// Control bits
#define DS_SECONDS_OSC   0x80           // set to stop oscillator (power save)
#define DS_CENTURY_READ  0x40           // set during read
#define DS_CENTURY_WRITE 0x80           // set during write
#define DS_DAY_BF        0x80           // battery flag
#define DS_DAY_FT        0x40           // frequency test


// Make sure test modes are disabled and that clock is running.
static void
init_ds_hwclock(void)
{
    cyg_uint8 _tmp;

    // Enable clock
    HAL_READ_UINT8(DS_BASE+DS_SECONDS, _tmp);
    _tmp &= ~DS_SECONDS_OSC;
    HAL_WRITE_UINT8(DS_BASE+DS_SECONDS, _tmp);

    // clear frequency test
    HAL_READ_UINT8(DS_BASE+DS_DAY, _tmp);
    _tmp &= ~DS_DAY_FT;
    HAL_WRITE_UINT8(DS_BASE+DS_DAY, _tmp);
}


static void
set_ds_hwclock(cyg_uint32 year, cyg_uint32 month, cyg_uint32 mday,
               cyg_uint32 hour, cyg_uint32 minute, cyg_uint32 second)
{
    cyg_uint8 _tmp;
    // Init write operation
    HAL_READ_UINT8(DS_BASE+DS_CENTURY, _tmp);
    _tmp &= ~(DS_CENTURY_WRITE|DS_CENTURY_READ);
    _tmp |= DS_CENTURY_WRITE;
    HAL_WRITE_UINT8(DS_BASE+DS_CENTURY, _tmp);

    // Entries with control bits
    HAL_READ_UINT8(DS_BASE+DS_CENTURY, _tmp);
    _tmp &= ~DS_CENTURY_MASK;
    _tmp |= TO_BCD(year/100) & DS_CENTURY_MASK;
    HAL_WRITE_UINT8(DS_BASE+DS_CENTURY, _tmp);

    HAL_READ_UINT8(DS_BASE+DS_SECONDS, _tmp);
    _tmp &= ~DS_SECONDS_MASK;
    _tmp |= TO_BCD(second) & DS_SECONDS_MASK;
    HAL_WRITE_UINT8(DS_BASE+DS_SECONDS, _tmp);

    HAL_READ_UINT8(DS_BASE+DS_DAY, _tmp);
    _tmp &= ~DS_DAY_FT;                 // clear frequency test
    HAL_WRITE_UINT8(DS_BASE+DS_DAY, _tmp);


    // Dedicated entries
    HAL_WRITE_UINT8(DS_BASE+DS_YEAR, TO_BCD(year % 100));
    HAL_WRITE_UINT8(DS_BASE+DS_MONTH, TO_BCD(month));
    HAL_WRITE_UINT8(DS_BASE+DS_DATE, TO_BCD(mday));
    HAL_WRITE_UINT8(DS_BASE+DS_HOUR, TO_BCD(hour));
    HAL_WRITE_UINT8(DS_BASE+DS_MINUTES, TO_BCD(minute));

    // Enable clock
    HAL_READ_UINT8(DS_BASE+DS_SECONDS, _tmp);
    _tmp &= ~DS_SECONDS_OSC;
    HAL_WRITE_UINT8(DS_BASE+DS_SECONDS, _tmp);

    // Finish write operation
    HAL_READ_UINT8(DS_BASE+DS_CENTURY, _tmp);
    _tmp &= ~(DS_CENTURY_WRITE|DS_CENTURY_READ);
    HAL_WRITE_UINT8(DS_BASE+DS_CENTURY, _tmp);
}

static void
get_ds_hwclock(cyg_uint32* year, cyg_uint32* month, cyg_uint32* mday,
               cyg_uint32* hour, cyg_uint32* minute, cyg_uint32* second)
{
    cyg_uint8 _tmp;

    // Init read operation
    HAL_READ_UINT8(DS_BASE+DS_CENTURY, _tmp);
    _tmp &= ~(DS_CENTURY_WRITE|DS_CENTURY_READ);
    _tmp |= DS_CENTURY_READ;
    HAL_WRITE_UINT8(DS_BASE+DS_CENTURY, _tmp);

    // Entries with control bits
    HAL_READ_UINT8(DS_BASE+DS_CENTURY, _tmp);
    _tmp &= DS_CENTURY_MASK;
    *year = 100*TO_DEC(_tmp);

    HAL_READ_UINT8(DS_BASE+DS_SECONDS, _tmp);
    _tmp &= DS_SECONDS_MASK;
    *second = TO_DEC(_tmp);

    // Dedicated entries
    HAL_READ_UINT8(DS_BASE+DS_YEAR, _tmp);
    *year += TO_DEC(_tmp);
    HAL_READ_UINT8(DS_BASE+DS_MONTH, _tmp);
    *month = TO_DEC(_tmp);
    HAL_READ_UINT8(DS_BASE+DS_DATE, _tmp);
    *mday = TO_DEC(_tmp);
    HAL_READ_UINT8(DS_BASE+DS_HOUR, _tmp);
    *hour = TO_DEC(_tmp);
    HAL_READ_UINT8(DS_BASE+DS_MINUTES, _tmp);
    *minute = TO_DEC(_tmp);

    // Finish read operation
    HAL_READ_UINT8(DS_BASE+DS_CENTURY, _tmp);
    _tmp &= ~(DS_CENTURY_WRITE|DS_CENTURY_READ);
    HAL_WRITE_UINT8(DS_BASE+DS_CENTURY, _tmp);
}

//-----------------------------------------------------------------------------
// End of devs/wallclock/ds1742.inl
