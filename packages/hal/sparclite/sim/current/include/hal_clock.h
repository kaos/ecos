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

#include <cyg/hal/hal_io.h>

//-----------------------------------------------------------------------------
// Clock control

// in erc32 simulator:
//  4 = UART A
//  5 = UART B
//  7 = UART error
// 12 = GPT (general purpose timer)
// 13 = RTC (realtime clock)
// 15 = watchdog

// in erc32 simulator:

// The vector used by the Real time clock is defined in hal_xpic.h

// We could place conditional code here to choose one clock or the other
// depending on the selected interrupt vector... but pro tem: (pun intended)

#define SPARC_MEC_RTC_CLOCK_SCALE (5)

/* These must be accessed word-wide to work! */

#define SPARC_MEC_RTC              (0x01f80080)

#define SPARC_MEC_RTC_COUNTER    (SPARC_MEC_RTC +    0)
#define SPARC_MEC_RTC_SCALER     (SPARC_MEC_RTC +    4)
                                                       
#define SPARC_MEC_GPT_COUNTER    (SPARC_MEC_RTC +    8)
#define SPARC_MEC_GPT_SCALER     (SPARC_MEC_RTC + 0x0c)

/* MEC timer control register bits */
#define SPARC_MEC_TCR_GACR 1      /* Continuous Running */
#define SPARC_MEC_TCR_GACL 2      /* Counter Load */      
#define SPARC_MEC_TCR_GASE 4      /* System Enable */     
#define SPARC_MEC_TCR_GASL 8      /* not used */          
#define SPARC_MEC_TCR_TCRCR 0x100 /* Continuous Running */
#define SPARC_MEC_TCR_TCRCL 0x200 /* Counter Load */
#define SPARC_MEC_TCR_TCRSE 0x400 /* System Enable */
#define SPARC_MEC_TCR_TCRSL 0x800 /* not used */

#define SPARC_MEC_RTC_CONTROL    (SPARC_MEC_RTC + 0x18)

externC cyg_int32 cyg_hal_sparclite_clock_period;

#define HAL_CLOCK_INITIALIZE( _period_ ) CYG_MACRO_START              \
  HAL_WRITE_UINT32( SPARC_MEC_RTC_SCALER, SPARC_MEC_RTC_CLOCK_SCALE );\
  cyg_hal_sparclite_clock_period = (_period_);                        \
  HAL_WRITE_UINT32( SPARC_MEC_RTC_COUNTER, (_period_) );              \
  HAL_WRITE_UINT32( SPARC_MEC_RTC_CONTROL,                            \
                           (SPARC_MEC_TCR_TCRCR |                     \
                            SPARC_MEC_TCR_TCRCL |                     \
                            SPARC_MEC_TCR_TCRSE) );                   \
CYG_MACRO_END

#define HAL_CLOCK_RESET( _vector_, _period_ ) /* nowt, it is freerunning */

#define HAL_CLOCK_READ( _pvalue_ ) CYG_MACRO_START                    \
   cyg_uint32 _read_;                                                 \
   HAL_READ_UINT32( SPARC_MEC_RTC_COUNTER, _read_ );                  \
   *((cyg_uint32 *)(_pvalue_)) =                                      \
                 (cyg_hal_sparclite_clock_period - _read_ );          \
CYG_MACRO_END


#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ )         HAL_CLOCK_READ( _pvalue_ )
#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CLOCK_H
// End of hal_clock.h
