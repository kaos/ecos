#ifndef CYGONCE_HAL_CLOCK_H
#define CYGONCE_HAL_CLOCK_H

//=============================================================================
//
//      hal_clock.h
//
//      HAL clock support
//
//=============================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, gthomas, hmt
// Contributors:        nickg, gthomas, hmt
// Date:        1999-01-28
// Purpose:     Define clock support
// Description: The macros defined here provide the HAL APIs for handling
//              the clock.
//              
// Usage:
//              #include <cyg/hal/hal_intr.h> // which includes this file
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

// #include <cyg/hal/hal_io.h>

#include <cyg/hal/hal_hwio.h> // HAL_SPARC_86940_READ/WRITE

//-----------------------------------------------------------------------------
// Clock control


// The vector used by the Real time clock is defined in hal_xpic.h

extern cyg_int32 cyg_hal_sparclite_clock_period;

//-----------------------------------------------------------------------------

#define HAL_SPARC_86940_REG_TIMER1_PRESCALER ( 0x14 * 4 )
#define HAL_SPARC_86940_REG_TIMER1_CONTROL   ( 0x15 * 4 )
#define HAL_SPARC_86940_REG_TIMER1_RELOAD    ( 0x16 * 4 )
#define HAL_SPARC_86940_REG_TIMER1_COUNT     ( 0x17 * 4 )



#define HAL_SPARC_86940_TIMER1_PRESCALER_WRITE( v ) \
            HAL_SPARC_86940_WRITE( HAL_SPARC_86940_REG_TIMER1_PRESCALER, v )

#define HAL_SPARC_86940_TIMER1_PRESCALER_READ( r ) \
            HAL_SPARC_86940_READ( HAL_SPARC_86940_REG_TIMER1_PRESCALER, r )

#define HAL_SPARC_86940_TIMER1_CONTROL_WRITE( v ) \
            HAL_SPARC_86940_WRITE( HAL_SPARC_86940_REG_TIMER1_CONTROL, v )

#define HAL_SPARC_86940_TIMER1_CONTROL_READ( r ) \
            HAL_SPARC_86940_READ( HAL_SPARC_86940_REG_TIMER1_CONTROL, r )

#define HAL_SPARC_86940_TIMER1_RELOAD_WRITE( v ) \
            HAL_SPARC_86940_WRITE( HAL_SPARC_86940_REG_TIMER1_RELOAD, v )

#define HAL_SPARC_86940_TIMER1_RELOAD_READ( r ) \
            HAL_SPARC_86940_READ( HAL_SPARC_86940_REG_TIMER1_RELOAD, r )

#define HAL_SPARC_86940_TIMER1_COUNT_READ( r ) \
            HAL_SPARC_86940_READ( HAL_SPARC_86940_REG_TIMER1_COUNT, r )

//-----------------------------------------------------------------------------

// Initialize the clock to 1MHz whatever the system clock speed.  This
// requires calculation...

externC void hal_clock_initialize( cyg_uint32 p );
#define HAL_CLOCK_INITIALIZE( _period_ ) hal_clock_initialize( _period_ )

// This is the easiest way to clear the interrupt.
#define HAL_CLOCK_RESET( _vector_, _period_ ) CYG_MACRO_START            \
  cyg_uint32 _scratch_;                                                  \
  HAL_SPARC_86940_TIMER1_COUNT_READ( _scratch_ );                        \
CYG_MACRO_END

#define HAL_CLOCK_READ( _pvalue_ ) CYG_MACRO_START                       \
  cyg_uint32 _read_;                                                     \
  HAL_SPARC_86940_TIMER1_COUNT_READ( _read_ );                           \
  *((cyg_uint32 *)(_pvalue_)) = cyg_hal_sparclite_clock_period - _read_; \
CYG_MACRO_END

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ )         HAL_CLOCK_READ( _pvalue_ )
#endif

//-----------------------------------------------------------------------------

#endif // ifndef CYGONCE_HAL_CLOCK_H
// End of hal_clock.h
