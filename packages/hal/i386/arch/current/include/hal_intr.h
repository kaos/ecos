#ifndef CYGONCE_HAL_HAL_INTR_H
#define CYGONCE_HAL_HAL_INTR_H

//==========================================================================
//
//      hal_intr.h
//
//      HAL Interrupt and clock support
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
// Author(s):    proven
// Contributors: proven, jskov, pjo, nickg
// Date:         1999-02-20
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//              
// Usage:
//               #include <cyg/hal/hal_intr.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_i386.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_intr.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

#ifndef CYGNUM_HAL_ISR_MAX
#define CYGNUM_HAL_VECTOR_NMI                0
#define CYGNUM_HAL_ISR_MIN                   0   
#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_VECTOR_NMI
#define CYGNUM_HAL_ISR_COUNT                 ( CYGNUM_HAL_ISR_MAX + 1 )
#endif /* CYGNUM_HAL_ISR_MAX, pjo 15 oct 1999 */

#ifndef CYGNUM_HAL_VSR_COUNT
#define CYGNUM_HAL_VSR_MIN                   0
#define CYGNUM_HAL_VSR_MAX                   0
#define CYGNUM_HAL_VSR_COUNT                 ( CYGNUM_HAL_VSR_MAX + 1 )
#endif /* CYGNUM_HAL_VSR_COUNT, pjo 15 oct 1999 */

#ifndef CYGNUM_HAL_EXCEPTION_COUNT
#define CYGNUM_HAL_EXCEPTION_MIN             0
#define CYGNUM_HAL_EXCEPTION_MAX             0
#define CYGNUM_HAL_EXCEPTION_COUNT           ( CYGNUM_HAL_EXCEPTION_MAX + 1 )
#endif /* CYGNUM_HAL_EXCEPTION_COUNT, pjo 15 oct 1999 */

//--------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS  cyg_hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD cyg_hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS  cyg_hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// VSR table
externC volatile CYG_ADDRESS  cyg_hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

//--------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//---------------------------------------------------------------------------
// Interrupt and VSR attachment macros

externC cyg_uint32 cyg_hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

#define HAL_INTERRUPT_IN_USE( _vector_, _state_)        \
    CYG_MACRO_START                                     \
    cyg_uint32 _index_;                                 \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);         \
                                                        \
    if (cyg_hal_interrupt_handlers[_index_]             \
        ==(CYG_ADDRESS)&cyg_hal_default_isr)            \
        (_state_) = 0;                                  \
    else                                                \
        (_state_) = 1;                                  \
    CYG_MACRO_END

// pjo, 18 oct 1999: Temporary.  Soon the i386/pc hal will be able to
// use this definition of interrupt attach.
#ifndef HAL_INTERRUPT_ATTACH
#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )       \
    CYG_MACRO_START                                                     \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR((_vector_), _index_);                          \
                                                                        \
    if (cyg_hal_interrupt_handlers[_index_]                             \
        ==(CYG_ADDRESS)&cyg_hal_default_isr)                            \
    {                                                                   \
        cyg_hal_interrupt_handlers[_index_] = (CYG_ADDRESS)(_isr_);     \
        cyg_hal_interrupt_data[_index_] = (CYG_ADDRWORD)(_data_);       \
        cyg_hal_interrupt_objects[_index_] = (CYG_ADDRESS)(_object_);   \
    }                                                                   \
    CYG_MACRO_END
#endif /* HAL_INTERRUPT_ATTACH */

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ ) \
    CYG_MACRO_START                             \
    cyg_uint32 _index_;                         \
    HAL_TRANSLATE_VECTOR((_vector_), _index_);  \
                                                \
    if (cyg_hal_interrupt_handlers[_index_]     \
        == (CYG_ADDRESS)(_isr_))                \
    {                                           \
        cyg_hal_interrupt_handlers[_index_] =   \
            (CYG_ADDRESS)&cyg_hal_default_isr;  \
        cyg_hal_interrupt_data[_index_] = 0;    \
        cyg_hal_interrupt_objects[_index_] = 0; \
    }                                           \
    CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                         \
    *((CYG_ADDRESS *)(_pvsr_)) = cyg_hal_vsr_table[(_vector_)];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )                       \
    CYG_MACRO_START                                                     \
    if( (_poldvsr_) != NULL )                                           \
        *(CYG_ADDRESS *)(_poldvsr_) = cyg_hal_vsr_table[(_vector_)];    \
    cyg_hal_vsr_table[(_vector_)] = (CYG_ADDRESS)(_vsr_);               \
    CYG_MACRO_END

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
// End of hal_intr.h
