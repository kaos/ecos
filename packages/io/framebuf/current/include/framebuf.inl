#include <cyg/infra/diag.h>
#ifndef CYGONCE_IO_FRAMEBUF_INL
#define CYGONCE_IO_FRAMEBUF_INL

//=============================================================================
//
//      framebuf.inl
//
//      Inline functions for manipulating linear framebuffers
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
//####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2005-03-29
//####DESCRIPTIONEND####
//=============================================================================

// There are numerous ways of implementing the functions in this file.
// For example for depths >= 8 many operations can be performed using
// memset()/memcpy()/memmove(), or
// __builtin_memset()/__builtin_memcpy()/ memmove(), or by explicit
// direct accesses to framebuffer memory. The first of these can be
// discarded because the builtins are never worse than explicit
// function calls. The choice between the builtins and direct accesses
// is not so clear. On the one hand the library functions may well
// involve architecture-specific optimizations. On the other hand
// function calls are not free. This decision really needs to be taken
// on a per-architecture basis after careful measurements.
//
// A similar question arises regarding the handling of row numbers.
// One implementation involves multiplying by the width. Another
// involves keeping a table of pointers to the start of each row,
// avoiding a multiplication but adding a memory access. Again the
// trade off is not immediately obvious.
//
// For direct accesses the bus width is an issue. Typically the frame
// buffer will not be cached so the framebuffer memory accesses will
// account for much of the inner loop. For now 32-bit accesses are
// preferred. If the target has h/w support for 64-bit accesses that
// would improve performance, at the cost of extra code to handle
// alignments. If the video device is accessed via a 16-bit bus then
// it is assumed that the h/w will fix up 32->16 bit accesses faster
// than any s/w workaround.
//
// For now the code prefers builtins and multiplies.

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// Macro definitions for the pixel manipulation.
// Little-endian means pixel 0 occupies bit 0 within a byte
// Big-endian means pixel 0 occupies bit 7
#define CYG_FB_PIXELx_VARx_1LE(  _fb_, _id_)
#define CYG_FB_PIXELx_SETx_1LE(  _fb_, _id_, _fbaddr_, _stride_, _x_, _y_)
#define CYG_FB_PIXELx_GETx_1LE(  _fb_, _id_, _fbaddr_, _stride_, _x_ ,_y_)
#define CYG_FB_PIXELx_ADDx_1LE(  _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_ADDx_1LE(  _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_WRITEx_1LE(_fb_, _id_, _colour_)
#define CYG_FB_PIXELx_READx_1LE( _fb_, _id_)
#define CYG_FB_PIXELx_FLUSHABS_1LE(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)
#define CYG_FB_PIXELx_FLUSHREL_1LE(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXELx_VARx_1BE(  _fb_, _id_)
#define CYG_FB_PIXELx_SETx_1BE(  _fb_, _id_, _fbaddr_, _stride_, _x_, _y_)
#define CYG_FB_PIXELx_GETx_1BE(  _fb_, _id_, _fbaddr_, _stride_, _x_ ,_y_)
#define CYG_FB_PIXELx_ADDXx_1BE( _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_ADDYx_1BE( _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_WRITEx_1BE(_fb_, _id_, _colour_)
#define CYG_FB_PIXELx_READx_1BE( _fb_, _id_)
#define CYG_FB_PIXELx_FLUSHABS_1BE(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)
#define CYG_FB_PIXELx_FLUSHREL_1BE(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXELx_VARx_2LE(  _fb_, _id_, _which_)
#define CYG_FB_PIXELx_SETx_2LE(  _fb_, _id_, _fbaddr_, _stride_, _x_, _y_)
#define CYG_FB_PIXELx_GETx_2LE(  _fb_, _id_, _fbaddr_, _stride_, _x_ ,_y_)
#define CYG_FB_PIXELx_ADDx_2LE(  _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_ADDx_2LE(  _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_WRITEx_2LE(_fb_, _id_, _colour_)
#define CYG_FB_PIXELx_READx_2LE( _fb_, _id_)
#define CYG_FB_PIXELx_FLUSHABS_2LE(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)
#define CYG_FB_PIXELx_FLUSHREL_2LE(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXELx_VARx_2BE(  _fb_, _id_)
#define CYG_FB_PIXELx_SETx_2BE(  _fb_, _id_, _fbaddr_, _stride_, _x_, _y_)
#define CYG_FB_PIXELx_GETx_2BE(  _fb_, _id_, _fbaddr_, _stride_, _x_ ,_y_)
#define CYG_FB_PIXELx_ADDXx_2BE( _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_ADDYx_2BE( _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_WRITEx_2BE(_fb_, _id_, _colour_)
#define CYG_FB_PIXELx_READx_2BE( _fb_, _id_)
#define CYG_FB_PIXELx_FLUSHABS_2BE(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)
#define CYG_FB_PIXELx_FLUSHREL_2BE(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXELx_VARx_4LE(  _fb_, _id_, _which_)
#define CYG_FB_PIXELx_SETx_4LE(  _fb_, _id_, _fbaddr_, _stride_, _x_, _y_)
#define CYG_FB_PIXELx_GETx_4LE(  _fb_, _id_, _fbaddr_, _stride_, _x_ ,_y_)
#define CYG_FB_PIXELx_ADDx_4LE(  _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_ADDx_4LE(  _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_WRITEx_4LE(_fb_, _id_, _colour_)
#define CYG_FB_PIXELx_READx_4LE( _fb_, _id_)
#define CYG_FB_PIXELx_FLUSHABS_4LE(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)
#define CYG_FB_PIXELx_FLUSHREL_4LE(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXELx_VARx_4BE(  _fb_, _id_)
#define CYG_FB_PIXELx_SETx_4BE(  _fb_, _id_, _fbaddr_, _stride_, _x_, _y_)
#define CYG_FB_PIXELx_GETx_4BE(  _fb_, _id_, _fbaddr_, _stride_, _x_ ,_y_)
#define CYG_FB_PIXELx_ADDXx_4BE( _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_ADDYx_4BE( _fb_, _id_, _stride_, _incr_)
#define CYG_FB_PIXELx_WRITEx_4BE(_fb_, _id_, _colour_)
#define CYG_FB_PIXELx_READx_4BE( _fb_, _id_)
#define CYG_FB_PIXELx_FLUSHABS_4BE(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)
#define CYG_FB_PIXELx_FLUSHREL_4BE(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)


// ----------------------------------------------------------------------------
#define CYG_FB_PIXELx_VAR_8(_fb_, _which_)    cyg_uint8*  _cyg_fb_pixelpos8_ ## _fb_ ## _which_
#define CYG_FB_PIXELx_SET_8(_fb_, _which_, _fbaddr_, _stride_, _x_, _y_)            \
    CYG_MACRO_START                                                                 \
    _cyg_fb_pixelpos8_ ## _fb_ ## _which_  = (cyg_uint8*)_fbaddr_;                  \
    _cyg_fb_pixelpos8_ ## _fb_ ## _which_ += ((_stride_) * (_y_));                  \
    _cyg_fb_pixelpos8_ ## _fb_ ## _which_ += (_x_);                                 \
    CYG_MACRO_END

#define CYG_FB_PIXELx_GET_8(_fb_, _which_, _fbaddr_, _stride_, _x_ ,_y_)            \
    CYG_MACRO_START                                                                 \
    cyg_uint32 _offset_;                                                            \
    _offset_ = (_cyg_fb_pixelpos8_ ## _fb_ ## _which_) - (cyg_uint8*)(_fbaddr_);    \
    _y_ = _offset_ / (_stride_);                                                    \
    _x_ = _offset_ % (_stride_);                                                    \
    CYG_MACRO_END
    
#define CYG_FB_PIXELx_ADDX_8(_fb_, _which_, _stride_, _incr_)                       \
    CYG_MACRO_START                                                                 \
    _cyg_fb_pixelpos8_ ## _fb_ ## _which_ += (_incr_);                              \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_ADDY_8(_fb_, _which_, _stride_, _incr_)                       \
    CYG_MACRO_START                                                                 \
    _cyg_fb_pixelpos8_ ## _fb_ ## _which_ += ((_incr_) * (_stride_));               \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_WRITE_8(_fb_, _which_, _colour_)                              \
    CYG_MACRO_START                                                                 \
    * _cyg_fb_pixelpos8_ ## _fb_ ## _which_ = (_colour_);                           \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_READ_8(_fb_, _which_) ({ * _cyg_fb_pixelpos8_ ## _fb_ ## _which_; })

#define CYG_FB_PIXELx_FLUSHABS_8(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)             \
    CYG_MACRO_START                                                                 \
    CYG_MACRO_END
    
#define CYG_FB_PIXELx_FLUSHREL_8(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)             \
    CYG_MACRO_START                                                                 \
    CYG_MACRO_END
    
// ----------------------------------------------------------------------------
#define CYG_FB_PIXELx_VAR_16(_fb_, _which_)    cyg_uint16*  _cyg_fb_pixelpos16_ ## _fb_ ## _which_
#define CYG_FB_PIXELx_SET_16(_fb_, _which_, _fbaddr_, _stride8_, _x_, _y_)                                              \
    CYG_MACRO_START                                                                                                     \
    _cyg_fb_pixelpos16_ ## _fb_ ## _which_  = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + ((_stride8_) * (_y_)))) + (_x_);  \
    CYG_MACRO_END

#define CYG_FB_PIXELx_GET_16(_fb_, _which_, _fbaddr_, _stride8_, _x_ ,_y_)          \
    CYG_MACRO_START                                                                 \
    cyg_ucount16 _stride_ = (_stride8_) >> 1;                                       \
    cyg_uint32   _offset_;                                                          \
    _offset_ = (_cyg_fb_pixelpos16_ ## _fb_ ## _which_) - (cyg_uint16*)(_fbaddr_);  \
    _y_ = _offset_ / (_stride_);                                                    \
    _x_ = _offset_ % (_stride_);                                                    \
    CYG_MACRO_END
    
#define CYG_FB_PIXELx_ADDX_16(_fb_, _which_, _stride8_, _incr_)                     \
    CYG_MACRO_START                                                                 \
    _cyg_fb_pixelpos16_ ## _fb_ ## _which_ += (_incr_);                             \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_ADDY_16(_fb_, _which_, _stride8_, _incr_)                     \
    CYG_MACRO_START                                                                 \
    _cyg_fb_pixelpos16_ ## _fb_ ## _which_ += ((_incr_) * ((_stride8_) >> 1));      \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_WRITE_16(_fb_, _which_, _colour_)                             \
    CYG_MACRO_START                                                                 \
    * _cyg_fb_pixelpos16_ ## _fb_ ## _which_ = (_colour_);                          \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_READ_16(_fb_, _which_)                                        \
    ({ * _cyg_fb_pixelpos16_ ## _fb_ ## _which_; })

#define CYG_FB_PIXELx_FLUSHABS_16(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)            \
    CYG_MACRO_START                                                                 \
    CYG_MACRO_END
    
#define CYG_FB_PIXELx_FLUSHREL_16(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)            \
    CYG_MACRO_START                                                                 \
    CYG_MACRO_END
    
// ----------------------------------------------------------------------------
#define CYG_FB_PIXELx_VAR_32(_fb_, _which_)    cyg_uint32*  _cyg_fb_pixelpos32_ ## _fb_ ## _which_
#define CYG_FB_PIXELx_SET_32(_fb_, _which_, _fbaddr_, _stride8_, _x_, _y_)                                              \
    CYG_MACRO_START                                                                                                     \
    _cyg_fb_pixelpos32_ ## _fb_ ## _which_  = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + ((_stride8_) * (_y_)))) + (_x_);  \
    CYG_MACRO_END

#define CYG_FB_PIXELx_GET_32(_fb_, _which_, _fbaddr_, _stride8_, _x_ ,_y_)          \
    CYG_MACRO_START                                                                 \
    cyg_ucount16 _stride_ = (_stride8_) >> 2;                                       \
    cyg_uint32   _offset_;                                                          \
    _offset_ = (_cyg_fb_pixelpos32_ ## _fb_ ## _which_) - (cyg_uint32*)(_fbaddr_);  \
    _y_ = _offset_ / (_stride_);                                                    \
    _x_ = _offset_ % (_stride_);                                                    \
    CYG_MACRO_END
    
#define CYG_FB_PIXELx_ADDX_32(_fb_, _which_, _stride8_, _incr_)                     \
    CYG_MACRO_START                                                                 \
    _cyg_fb_pixelpos32_ ## _fb_ ## _which_ += (_incr_);                             \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_ADDY_32(_fb_, _which_, _stride8_, _incr_)                     \
    CYG_MACRO_START                                                                 \
    _cyg_fb_pixelpos32_ ## _fb_ ## _which_ += ((_incr_) * ((_stride8_) >> 2));      \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_WRITE_32(_fb_, _which_, _colour_)                             \
    CYG_MACRO_START                                                                 \
    * _cyg_fb_pixelpos32_ ## _fb_ ## _which_ = (_colour_);                          \
    CYG_MACRO_END
                                      
#define CYG_FB_PIXELx_READ_32(_fb_, _which_) ({ * _cyg_fb_pixelpos32_ ## _fb_ ## _which_; })

#define CYG_FB_PIXELx_FLUSHABS_32(_fb_, _which_, _x0_, _y0_, _x1_, _y1_)            \
    CYG_MACRO_START                                                                 \
    CYG_MACRO_END
    
#define CYG_FB_PIXELx_FLUSHREL_32(_fb_, _which_, _x0_, _y0_, _dx_, _dy_)            \
    CYG_MACRO_START                                                                 \
    CYG_MACRO_END
    
// ----------------------------------------------------------------------------
// Some of the functions could be speeded up by using 32-bit accesses
// instead of byte accesses, but that would introduce another endianness
// complication.    
    
// The always_inline attribute must be a applied to a declaration, not a
// definition, so combine the two via a single macro.
#define CYG_FB_LINEAR_INLINE_FN(_type_, _name_, _args_)                     \
static __inline__ _type_ _name_ _args_ __attribute__((__always_inline__));  \
static __inline__ _type_ _name_ _args_
    
CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_1LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 3);
    cyg_uint8   _mask_  = 0x0001 << (_x_ & 0x07);
    if (_colour_) {
        *_ptr8_ |= _mask_;
    } else {
        *_ptr8_ &= ~_mask_;
    }
}
                     
CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_1BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 3);
    cyg_uint8   _mask_  = 0x0080 >> (_x_ & 0x07);
    if (_colour_) {
        *_ptr8_ |= _mask_;
    } else {
        *_ptr8_ &= ~_mask_;
    }
}

CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_1LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 3);
    cyg_uint8   _mask_  = 0x0001 << (_x_ & 0x07);
    return (*_ptr8_ & _mask_) ? 1 : 0;
}

CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_1BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 3);
    cyg_uint8   _mask_  = 0x0080 >> (_x_ & 0x07);
    return (*_ptr8_ & _mask_) ? 1 : 0;
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_1LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 3);
    cyg_uint8   _mask_  = 0x0001 << (_x_ & 0x07);
    if (_colour_) {
        for ( ; _len_; _len_--) {
            *_ptr8_ |= _mask_;
            _ptr8_  += _stride_;
        }
    } else {
        _mask_  = ~_mask_;
        for ( ; _len_; _len_--) {
            *_ptr8_ &= _mask_;
            _ptr8_  += _stride_;
        }
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_1BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 3);
    cyg_uint8   _mask_  = 0x0080 >> (_x_ & 0x07);
    if (_colour_) {
        for ( ; _len_; _len_--) {
            *_ptr8_ |= _mask_;
            _ptr8_  += _stride_;
        }
    } else {
        _mask_  = ~_mask_;
        for ( ; _len_; _len_--) {
            *_ptr8_ &= _mask_;
            _ptr8_  += _stride_;
        }
    }
}

// ----------------------------------------------------------------------------

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_2LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    cyg_uint8   _shift_ = (_x_ & 0x03) << 1;

    *_ptr8_ = (*_ptr8_ & ~(0x03 << _shift_)) | (_colour_ << _shift_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_2BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    cyg_uint8   _shift_ = 6 - ((_x_ & 0x03) << 1);

    *_ptr8_ = (*_ptr8_ & ~(0x03 << _shift_)) | (_colour_ << _shift_);
}

CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_2LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    cyg_uint8   _shift_ = (_x_ & 0x03) << 1;

    return (*_ptr8_ >> _shift_) & 0x03;
}
                     
CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_2BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    cyg_uint8   _shift_ = 6 - ((_x_ & 0x03) << 1);

    return (*_ptr8_ >> _shift_) & 0x03;
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_2LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    cyg_uint8   _shift_ = (_x_ & 0x03) << 1;
    cyg_uint8   _mask_  = ~(0x03 << _shift_);
    _colour_    <<= _shift_;

    for ( ; _len_; _len_--) {
        *_ptr8_  = (*_ptr8_ & _mask_) | _colour_;
        _ptr8_  += _stride_;
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_2BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    cyg_uint8   _shift_ = 6 - ((_x_ & 0x03) << 1);
    cyg_uint8   _mask_  = ~(0x03 << _shift_);
    _colour_    <<= _shift_;

    for ( ; _len_; _len_--) {
        *_ptr8_  = (*_ptr8_ & _mask_) | _colour_;
        _ptr8_  += _stride_;
    }
}

// ----------------------------------------------------------------------------

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_4LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 1);
    if (_x_ & 0x01) {
        *_ptr8_ = (*_ptr8_ & 0x000F) | (_colour_ << 4);
    } else {
        *_ptr8_ = (*_ptr8_ & 0x00F0) | _colour_;
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_4BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 1);
    if (_x_ & 0x01) {
        *_ptr8_ = (*_ptr8_ & 0x00F0) | _colour_;
    } else {
        *_ptr8_ = (*_ptr8_ & 0x000F) | (_colour_ << 4);
    }
}

CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_4LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 1);
    if (_x_ & 0x01) {
        return (*_ptr8_ >> 4) & 0x000F;
    } else {
        return *_ptr8_ & 0x000F;
    }
}
                     
CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_4BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 1);
    if (_x_ & 0x01) {
        return *_ptr8_ & 0x000F;
    } else {
        return (*_ptr8_ >> 4) & 0x000F;
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_4LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    if (_x_ & 0x01) {
        _colour_ <<= 4;
        for ( ; _len_; _len_--) {
            *_ptr8_  = (*_ptr8_ & 0x000F) | _colour_;
            _ptr8_  += _stride_;
        }
    } else {
        for ( ; _len_; _len_--) {
            *_ptr8_ = (*_ptr8_ & 0x00F0) | _colour_;
        }
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_4BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + (_x_ >> 2);
    if (_x_ & 0x01) {
        for ( ; _len_; _len_--) {
            *_ptr8_ = (*_ptr8_ & 0x00F0) | _colour_;
        }
    } else {
        _colour_ <<= 4;
        for ( ; _len_; _len_--) {
            *_ptr8_  = (*_ptr8_ & 0x000F) | _colour_;
            _ptr8_  += _stride_;
        }
    }
}

// ----------------------------------------------------------------------------
CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_ = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + _x_;
    *_ptr8_ = _colour_;
}

CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint8*  _ptr8_ = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + _x_;
    return *_ptr8_;
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_hline_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_ = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + _x_;
    __builtin_memset(_ptr8_, _colour_, _len_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_ = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + _x_;
    for ( ; _len_; _len_--) {
        *_ptr8_  = _colour_;
        _ptr8_  += _stride_;
    }    
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_fill_block_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_, cyg_fb_colour _colour_))
{
    cyg_uint8*  _ptr8_ = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + _x_;
    for ( ; _height_; _height_--) {
        __builtin_memset(_ptr8_, _colour_, _width_);
        _ptr8_ += _stride_;
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_block_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         const void* _source_,
                         cyg_ucount16 _offset_, cyg_ucount16 _source_stride_))
{
    cyg_uint8*  _ptr8_ = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + _x_;
    cyg_uint8*  _src8_ = ((cyg_uint8*) _source_) + _offset_;
    
    for ( ; _height_; _height_--) {
        __builtin_memcpy(_ptr8_, _src8_, _width_);
        _ptr8_  += _stride_;
        _src8_  += _source_stride_;
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_read_block_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         void* _dest_,
                         cyg_ucount16 _offset_, cyg_ucount16 _dest_stride_))
{
    cyg_uint8*  _ptr8_  = ((cyg_uint8*)_fbaddr_) + (_stride_ * _y_) + _x_;
    cyg_uint8*  _dst8_  = ((cyg_uint8*)_dest_) + _offset_;
    
    for ( ; _height_; _height_--) {
        __builtin_memcpy(_dst8_, _ptr8_, _width_);
        _ptr8_  += _stride_;
        _dst8_  += _dest_stride_;
    }
}

// ----------------------------------------------------------------------------

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint16* _ptr16_ = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    *_ptr16_ = _colour_;
}

CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint16* _ptr16_ = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    return *_ptr16_;
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_hline_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint16* _ptr16_ = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    cyg_uint32* _ptr32_;
    cyg_bool    _right_;

    if (_x_ & 0x01) {
        *_ptr16_++ = _colour_;
        _len_      -= 1;
    }
    _ptr32_   = (cyg_uint32*) _ptr16_;
    _colour_ |= (_colour_ << 16);
    _right_   = (_len_ & 0x01);
    for ( _len_ >>= 1; _len_; _len_--) {
        *_ptr32_++ = _colour_;
    }
    if (_right_) {
        *((cyg_uint16*)_ptr32_) = _colour_;
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint16* _ptr16_ = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    for ( ; _len_; _len_--) {
        *_ptr16_     = _colour_;
        _ptr16_      = (cyg_uint16*)(((cyg_uint8*)_ptr16_) + _stride8_);
    }    
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_fill_block_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_, cyg_fb_colour _colour_))
{
    cyg_bool        _left_, _right_;
    cyg_uint16*     _ptr16_;
    cyg_uint32*     _ptr32_;
    cyg_ucount16    _stride32_;

    _ptr16_       = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    _left_ = (_x_ & 0x01);
    if (_left_) {
        _width_  -= 1;
    }
    _right_       = (_width_ & 0x01);

    if (_left_) {
        _ptr32_ = (cyg_uint32*) &(_ptr16_[1]);
        for (_y_ = _height_; _y_; _y_--) {
            *_ptr16_    = _colour_;
            _ptr16_     = (cyg_uint16*)(((cyg_uint8*)_ptr16_) + _stride8_);
        }
    } else {
        _ptr32_ = (cyg_uint32*) _ptr16_;
    }
    _width_     >>= 1;
    _stride32_    = _stride8_ - (_width_ << 2);
    _colour_     |= (_colour_ << 16);
    _ptr16_       = (cyg_uint16*) &(_ptr32_[_width_]);
    
    for ( _y_ = _height_; _y_; _y_--) {
        for ( _x_ = _width_; _x_; _x_--) {
            *_ptr32_++ = _colour_;
        }
        _ptr32_ = (cyg_uint32*)(((cyg_uint8*)_ptr32_) + _stride32_);
    }
    if (_right_) {
        for (_y_ = _height_; _y_; _y_--) {
            *_ptr16_    = _colour_;
            _ptr16_     = (cyg_uint16*)(((cyg_uint8*)_ptr16_) + _stride8_);
        }
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_block_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         const void* _source_,
                         cyg_ucount16 _offset_, cyg_ucount16 _source_stride8_))
{
    cyg_uint16* _ptr16_ = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    cyg_uint16* _src16_ = ((cyg_uint16*) _source_) + _offset_;

    _width_ <<= 1;
    for ( ; _height_; _height_--) {
        __builtin_memcpy(_ptr16_, _src16_, _width_);
        _ptr16_ = (cyg_uint16*)(((cyg_uint8*)_ptr16_) + _stride8_);
        _src16_ = (cyg_uint16*)(((cyg_uint8*)_src16_) + _source_stride8_);
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_read_block_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         void* _dest_,
                         cyg_ucount16 _offset_, cyg_ucount16 _dest_stride8_))
{
    cyg_uint16* _ptr16_ = ((cyg_uint16*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    cyg_uint16* _dst16_ = ((cyg_uint16*)_dest_) + _offset_;

    _width_ <<= 1;
    for ( ; _height_; _height_--) {
        __builtin_memcpy(_dst16_, _ptr16_, _width_);
        _ptr16_ = (cyg_uint16*)(((cyg_uint8*)_ptr16_) + _stride8_);
        _dst16_ = (cyg_uint16*)(((cyg_uint8*)_dst16_) + _dest_stride8_);
    }
}

// ----------------------------------------------------------------------------

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_pixel_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_fb_colour _colour_))
{
    cyg_uint32* _ptr32_ = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    *_ptr32_ = _colour_;
}

CYG_FB_LINEAR_INLINE_FN(cyg_fb_colour,
                        cyg_fb_linear_read_pixel_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_))
{
    cyg_uint32* _ptr32_ = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    return *_ptr32_;
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_hline_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint32* _ptr32_ = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    while (_len_--) {
        *_ptr32_++ = _colour_;
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_vline_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_))
{
    cyg_uint32* _ptr32_ = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    while (_len_--) {
        *_ptr32_    = _colour_;
        _ptr32_     = (cyg_uint32*)(((cyg_uint8*)_ptr32_) + _stride8_);
    }    
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_fill_block_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_, cyg_fb_colour _colour_))
{
    cyg_uint32*     _ptr32_ = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    cyg_ucount16    _len_;
    
    _stride8_   -= (_width_ << 2);
    for ( ; _height_; _height_--) {
        for (_len_ = _width_; _len_; _len_--) {
            *_ptr32_++ = _colour_;
        }
        _ptr32_ = (cyg_uint32*)(((cyg_uint8*)_ptr32_) + _stride8_);
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_write_block_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         const void* _source_,
                         cyg_ucount16 _offset_, cyg_ucount16 _source_stride8_))
{
    cyg_uint32* _ptr32_ = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    cyg_uint32* _src32_ = ((cyg_uint32*) _source_) + _offset_;

    _width_ <<= 2;
    for ( ; _height_; _height_--) {
        __builtin_memcpy(_ptr32_, _src32_, _width_);
        _ptr32_ = (cyg_uint32*)(((cyg_uint8*)_ptr32_) + _stride8_);
        _src32_ = (cyg_uint32*)(((cyg_uint8*)_src32_) + _source_stride8_);
    }
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_read_block_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride8_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         void* _dest_,
                         cyg_ucount16 _offset_, cyg_ucount16 _dest_stride8_))
{
    cyg_uint32* _ptr32_ = ((cyg_uint32*)(((cyg_uint8*)_fbaddr_) + (_stride8_ * _y_))) + _x_;
    cyg_uint32* _dst32_  = ((cyg_uint32*)_dest_) + _offset_;

    _width_ <<= 2;
    for ( ; _height_; _height_--) {
        __builtin_memcpy(_dst32_, _ptr32_, _width_);
        _ptr32_ = (cyg_uint32*)(((cyg_uint8*)_ptr32_) + _stride8_);
        _dst32_ = (cyg_uint32*)(((cyg_uint8*)_dst32_) + _dest_stride8_);
    }
}

// ----------------------------------------------------------------------------
// The actual move block functions are not inlined, but it is convenient to
// have dummy _inl functions anyway.

extern void cyg_fb_linear_move_block_1LE_impl(void*, cyg_ucount16 /* stride */,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_1BE_impl(void*, cyg_ucount16 /* stride */,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_2LE_impl(void*, cyg_ucount16 /* stride */,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_2BE_impl(void*, cyg_ucount16 /* stride */,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_4LE_impl(void*, cyg_ucount16 /* stride */,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_4BE_impl(void*, cyg_ucount16 /* stride */,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16,
                                              cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_8_impl(void*, cyg_ucount16 /* stride */,
                                            cyg_ucount16, cyg_ucount16,
                                            cyg_ucount16, cyg_ucount16,
                                            cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_16_impl(void*, cyg_ucount16 /* stride */,
                                             cyg_ucount16, cyg_ucount16,
                                             cyg_ucount16, cyg_ucount16,
                                             cyg_ucount16, cyg_ucount16);
extern void cyg_fb_linear_move_block_32_impl(void*, cyg_ucount16 /* stride */,
                                             cyg_ucount16, cyg_ucount16,
                                             cyg_ucount16, cyg_ucount16,
                                             cyg_ucount16, cyg_ucount16);

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_1LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_1LE_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_1BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_1BE_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_2LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_2LE_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_2BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_2BE_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_4LE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_4LE_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_4BE_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_4BE_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_8_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_8_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_16_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_16_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

CYG_FB_LINEAR_INLINE_FN(void,
                        cyg_fb_linear_move_block_32_inl,
                        (void* _fbaddr_, cyg_ucount16 _stride_,
                         cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                         cyg_ucount16 _new_x_, cyg_ucount16 _new_y_))
{
    cyg_fb_linear_move_block_32_impl(_fbaddr_, _stride_, _x_, _y_, _width_, _height_, _new_x_, _new_y_);
}

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif // ifndef CYGONCE_IO_FRAMEBUF_H
// End of framebuf.h
