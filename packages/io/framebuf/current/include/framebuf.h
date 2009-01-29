#ifndef CYGONCE_IO_FRAMEBUF_H
#define CYGONCE_IO_FRAMEBUF_H

//=============================================================================
//
//      framebuf.h
//
//      Generic API for accessing framebuffers
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
// Author(s): 	bartv
// Date:	    2005-03-29
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/infra.h>
#include <pkgconf/io_framebuf.h>
#include <cyg/infra/cyg_type.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// The colour data type. For now this is a cyg_ucount32. On 16-bit targets
// where the display depth is less than 32bpp a cyg_ucount16 would suffice
// and be more efficient. A separate cyg_fb_colour type is used to allow this
// change to be made in future.
typedef cyg_ucount32    cyg_fb_colour;
typedef cyg_fb_colour   cyg_fb_color;
   
// ----------------------------------------------------------------------------
// Each framebuffer device exports one of these structures
typedef struct cyg_fb {
#ifdef CYGPKG_INFRA_DEBUG
    cyg_ucount32    fb_magic;
#endif    
    cyg_ucount16    fb_depth;   // 1, 2, 4, 8, 16, or 32 bits per pixel
    cyg_ucount16    fb_format;  // detailed format, e.g. 8-bit paletted, or 32-bit 0888
    cyg_ucount16    fb_width;   // of the framebuffer, not the viewport
    cyg_ucount16    fb_height;
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_VIEWPORT
    cyg_ucount16    fb_viewport_width;
    cyg_ucount16    fb_viewport_height;
#endif
    void*           fb_base;    // Base address of the framebuffer, or 0 if not directly accessible
    cyg_ucount16    fb_stride;  // number of bytes per line
    cyg_uint32      fb_flags0;
    cyg_uint32      fb_flags1;
    cyg_uint32      fb_flags2;
    cyg_uint32      fb_flags3;

    CYG_ADDRWORD    fb_driver0; // Driver private fields
    CYG_ADDRWORD    fb_driver1;
    CYG_ADDRWORD    fb_driver2;
    CYG_ADDRWORD    fb_driver3;
    
    int             (*fb_on_fn)(struct cyg_fb*);
    int             (*fb_off_fn)(struct cyg_fb*);
    int             (*fb_ioctl_fn)(      struct cyg_fb*, cyg_ucount16, void*,        size_t*);
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_DOUBLE_BUFFER    
    void            (*fb_synch_fn)(      struct cyg_fb*, cyg_ucount16);
#endif
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_PALETTE
    void            (*fb_read_palette_fn)(struct cyg_fb*, cyg_ucount32, cyg_ucount32, void*);
#endif
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_WRITEABLE_PALETTE
    void            (*fb_write_palette_fn)(struct cyg_fb*, cyg_ucount32, cyg_ucount32, const void*, cyg_ucount16);
#endif
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_TRUE_COLOUR
    cyg_fb_colour   (*fb_make_colour_fn)(struct cyg_fb*, cyg_ucount8, cyg_ucount8, cyg_ucount8);
    void            (*fb_break_colour_fn)(struct cyg_fb*, cyg_fb_colour, cyg_ucount8*, cyg_ucount8*, cyg_ucount8*);
#endif    
    
    void            (*fb_write_pixel_fn)(struct cyg_fb*, cyg_ucount16, cyg_ucount16, cyg_fb_colour);
    cyg_fb_colour   (*fb_read_pixel_fn)( struct cyg_fb*, cyg_ucount16, cyg_ucount16);
    void            (*fb_write_hline_fn)(struct cyg_fb*, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_fb_colour);
    void            (*fb_write_vline_fn)(struct cyg_fb*, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_fb_colour);
    void            (*fb_fill_block_fn)( struct cyg_fb*, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_fb_colour);
    void            (*fb_write_block_fn)(struct cyg_fb*, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_ucount16, const void*, cyg_ucount16, cyg_ucount16);
    void            (*fb_read_block_fn)( struct cyg_fb*, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_ucount16, void*, cyg_ucount16, cyg_ucount16);
    void            (*fb_move_block_fn)( struct cyg_fb*, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_ucount16, cyg_ucount16);
} cyg_fb;

// A magic number used to validate an fb pointer
#define CYG_FB_MAGIC    0xC201e147    

// The various supported colour formats. A 16-bit value.
#define CYG_FB_FORMAT_1BPP_MONO_0_BLACK         0x0000
#define CYG_FB_FORMAT_1BPP_MONO_0_WHITE         0x0001
#define CYG_FB_FORMAT_1BPP_PAL888               0x0002

#define CYG_FB_FORMAT_2BPP_GREYSCALE_0_BLACK    0x0100
#define CYG_FB_FORMAT_2BPP_GRAYSCALE_0_BLACK    0x0100
#define CYG_FB_FORMAT_2BPP_GREYSCALE_0_WHITE    0x0101
#define CYG_FB_FORMAT_2BPP_GRAYSCALE_0_WHITE    0x0101
#define CYG_FB_FORMAT_2BPP_PAL888               0x0102

#define CYG_FB_FORMAT_4BPP_GREYSCALE_0_BLACK    0x0200
#define CYG_FB_FORMAT_4BPP_GRAYSCALE_0_BLACK    0x0200
#define CYG_FB_FORMAT_4BPP_GREYSCALE_0_WHITE    0x0201
#define CYG_FB_FORMAT_4BPP_GRAYSCALE_0_WHITE    0x0202
#define CYG_FB_FORMAT_4BPP_PAL888               0x0203

#define CYG_FB_FORMAT_8BPP_PAL888               0x0300
#define CYG_FB_FORMAT_8BPP_TRUE_332             0x0301
    
#define CYG_FB_FORMAT_16BPP_TRUE_565            0x0400
#define CYG_FB_FORMAT_16BPP_TRUE_555            0x0401

#define CYG_FB_FORMAT_32BPP_TRUE_0888           0x0500
    
// Flags indicating exactly what the device driver can support.
// Each fb structure has four flags fields for a maximum of
// 128 flags, allowing for future expansion at a relatively
// low cost.
    
// The framebuffer memory is directly accessible and organized in a
// conventional fashion, i.e. increasing x means the addresses
// increase by 1 bit/nibble/byte/short/int as appropriate, and
// increasing y means adding n bytes/shorts/ints
#define CYG_FB_FLAGS0_LINEAR_FRAMEBUFFER        (0x01 <<  0)
// Only relevant for 1bpp, 2bpp and 4bpp displays. Little-endian
// means that pixel (0,0) occupies bit 0 of framebuffer byte 0.
// 0. Big-endian means that pixel(0,0) occupies bit 7.
#define CYG_FB_FLAGS0_LE                        (0x01 <<  1)

#define CYG_FB_FLAGS0_TRUE_COLOUR               (0x01 <<  8)    
#define CYG_FB_FLAGS0_TRUE_COLOR                (0x01 <<  8)    
#define CYG_FB_FLAGS0_PALETTE                   (0x01 <<  9)
#define CYG_FB_FLAGS0_WRITEABLE_PALETTE         (0x01 << 10)
    
#define CYG_FB_FLAGS0_DELAYED_PALETTE_UPDATE    (0x01 << 16)
#define CYG_FB_FLAGS0_VIEWPORT                  (0x01 << 17)
#define CYG_FB_FLAGS0_DOUBLE_BUFFER             (0x01 << 18)
#define CYG_FB_FLAGS0_PAGE_FLIPPING             (0x01 << 19)
#define CYG_FB_FLAGS0_BLANK                     (0x01 << 20)
#define CYG_FB_FLAGS0_BACKLIGHT                 (0x01 << 21)
#define CYG_FB_FLAGS0_MUST_BE_ON                (0x01 << 22)
    
// Some operations including updating the palette, double-buffer
// synch, page flipping and moving the viewport take an argument
// to specify when the operation should happen. Driver support
// for this is optional.    
#define CYG_FB_UPDATE_NOW                       0x0000
#define CYG_FB_UPDATE_VERTICAL_RETRACE          0x0001

// ----------------------------------------------------------------------------
// Macro to define a framebuffer. A couple of auxiliary macros are used
// for the conditional fields.
#ifdef CYGPKG_INFRA_DEBUG    
# define CYG_FB_FRAMEBUFFER_DEBUG   \
      .fb_magic   = CYG_FB_MAGIC,
#else
# define CYG_FB_FRAMEBUFFER_DEBUG
#endif    

#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_VIEWPORT    
# define CYG_FB_FRAMEBUFFER_VIEWPORT(_width_, _height_) \
    .fb_viewport_width  = _width_,                      \
    .fb_viewport_height = _height_,
#else
# define CYG_FB_FRAMEBUFFER_VIEWPORT(_width_, _height_)
#endif

#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_DOUBLE_BUFFER
# define CYG_FB_FRAMEBUFFER_SYNCH_FN(_fn_)              \
    .fb_synch_fn = _fn_,
#else
# define CYG_FB_FRAMEBUFFER_SYNCH_FN(_fn_)
#endif
    
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_PALETTE
# define CYG_FB_FRAMEBUFFER_READ_PALETTE_FN(_fn_)       \
    .fb_read_palette_fn     = _fn_,
#else
# define CYG_FB_FRAMEBUFFER_READ_PALETTE_FN(_fn_)
#endif

#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_WRITEABLE_PALETTE
# define CYG_FB_FRAMEBUFFER_WRITE_PALETTE_FN(_fn_)      \
    .fb_write_palette_fn     = _fn_,
#else
# define CYG_FB_FRAMEBUFFER_WRITE_PALETTE_FN(_fn_)
#endif

#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_TRUE_COLOUR
# define CYG_FB_FRAMEBUFFER_MAKE_COLOUR_FN(_fn_) \
    .fb_make_colour_fn      = _fn_,
# define CYG_FB_FRAMEBUFFER_BREAK_COLOUR_FN(_fn_) \
    .fb_break_colour_fn     = _fn_,
#else
# define CYG_FB_FRAMEBUFFER_MAKE_COLOUR_FN(_fn_)
# define CYG_FB_FRAMEBUFFER_BREAK_COLOUR_FN(_fn_)
#endif
    
#define CYG_FB_FRAMEBUFFER(_name_, _depth_, _format_,                                               \
                           _width_, _height_, _viewport_width_, _viewport_height_,                  \
                           _base_, _stride_,                                                        \
                           _flags0_, _flags1_, _flags2_, _flags3_,                                  \
                           _driver0_, _driver1_, _driver2_, _driver3_,                              \
                           _on_fn_, _off_fn_, _ioctl_fn_, _synch_fn_,                               \
                           _read_palette_fn_, _write_palette_fn_,                                   \
                           _make_colour_fn_, _break_colour_fn_,                                     \
                           _write_pixel_fn_, _read_pixel_fn_, _write_hline_fn_, _write_vline_fn_,   \
                           _fill_block_fn_, _write_block_fn_, _read_block_fn_, _move_block_fn_,     \
                           _spare0_, _spare1_, _spare2_, _spare3_ )                                 \
struct cyg_fb _name_ = {                                                \
    CYG_FB_FRAMEBUFFER_DEBUG                                            \
    .fb_depth           = _depth_,                                      \
    .fb_format          = _format_,                                     \
    .fb_width           = _width_,                                      \
    .fb_height          = _height_,                                     \
    CYG_FB_FRAMEBUFFER_VIEWPORT(_viewport_width_, _viewport_height_)    \
    .fb_stride          = _stride_,                                     \
    .fb_base            = _base_,                                       \
    .fb_flags0          = _flags0_,                                     \
    .fb_flags1          = _flags1_,                                     \
    .fb_flags2          = _flags2_,                                     \
    .fb_flags3          = _flags3_,                                     \
    .fb_driver0         = _driver0_,                                    \
    .fb_driver1         = _driver1_,                                    \
    .fb_driver2         = _driver2_,                                    \
    .fb_driver3         = _driver3_,                                    \
    .fb_on_fn           = _on_fn_,                                      \
    .fb_off_fn          = _off_fn_,                                     \
    .fb_ioctl_fn        = _ioctl_fn_,                                   \
    CYG_FB_FRAMEBUFFER_SYNCH_FN(_synch_fn_)                             \
    CYG_FB_FRAMEBUFFER_READ_PALETTE_FN(_read_palette_fn_)               \
    CYG_FB_FRAMEBUFFER_WRITE_PALETTE_FN(_write_palette_fn_)             \
    CYG_FB_FRAMEBUFFER_MAKE_COLOUR_FN(_make_colour_fn_)                 \
    CYG_FB_FRAMEBUFFER_BREAK_COLOUR_FN(_break_colour_fn_)               \
    .fb_write_pixel_fn  = _write_pixel_fn_,                             \
    .fb_read_pixel_fn   = _read_pixel_fn_,                              \
    .fb_write_hline_fn  = _write_hline_fn_,                             \
    .fb_write_vline_fn  = _write_vline_fn_,                             \
    .fb_fill_block_fn   = _fill_block_fn_,                              \
    .fb_write_block_fn  = _write_block_fn_,                             \
    .fb_read_block_fn   = _read_block_fn_,                              \
    .fb_move_block_fn   = _move_block_fn_                               \
}

 
// ----------------------------------------------------------------------------
// ioctl() operations: these are defined by a 16-bit key. The top byte
// gives a category, with values of 0x8000 onwards reserved for use by
// individual device drivers. The bottom byte defines the actual
// operation. This gives 128 generic ioctl categories.

# define CYG_FB_IOCTL_VIEWPORT_GET_POSITION     0x0100
# define CYG_FB_IOCTL_VIEWPORT_SET_POSITION     0x0101

typedef struct cyg_fb_ioctl_viewport {
    cyg_ucount16    fbvp_x;     // position of top-left corner of the viewport within
    cyg_ucount16    fbvp_y;     // the framebuffer
    cyg_ucount16    fbvp_when;  // set-only, now or vert retrace
} cyg_fb_ioctl_viewport;

# define CYG_FB_IOCTL_PAGE_FLIPPING_GET_PAGES   0x0200
# define CYG_FB_IOCTL_PAGE_FLIPPING_SET_PAGES   0x0201

typedef struct cyg_fb_ioctl_page_flip {
    cyg_uint32      fbpf_number_pages;
    cyg_uint32      fbpf_visible_page;
    cyg_uint32      fbpf_drawable_page;
    cyg_ucount16    fbpf_when;  // set-only, now or vert retrace
} cyg_fb_ioctl_page_flip;

# define CYG_FB_IOCTL_BLANK_GET                 0x0300
# define CYG_FB_IOCTL_BLANK_SET                 0x0301

typedef struct cyg_fb_ioctl_blank {
    cyg_bool        fbbl_on;
} cyg_fb_ioctl_blank;

# define CYG_FB_IOCTL_BACKLIGHT_GET             0x0400
# define CYG_FB_IOCTL_BACKLIGHT_SET             0x0401

typedef struct cyg_fb_ioctl_backlight {
    cyg_ucount32    fbbl_current;
    cyg_ucount32    fbbl_max;
} cyg_fb_ioctl_backlight;

// ----------------------------------------------------------------------------
// The functional API. This can be implemented either by inlines or by
// explicit functions, the latter allowing extra assertions.

#if defined(CYGPKG_INFRA_DEBUG) || defined(__CYG_FB_IN_FRAMEBUF_C)

extern int              cyg_fb_on(cyg_fb*);
extern int              cyg_fb_off(cyg_fb*);
extern void             cyg_fb_write_pixel( cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */,
                                            cyg_fb_colour);
extern cyg_fb_colour    cyg_fb_read_pixel(  cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */);
extern void             cyg_fb_write_hline( cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */,
                                            cyg_ucount16 /* len */,
                                            cyg_fb_colour);
extern void             cyg_fb_write_vline( cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */,
                                            cyg_ucount16 /* len */,
                                            cyg_fb_colour);
extern void             cyg_fb_fill_block(  cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */,
                                            cyg_ucount16 /* width */,   cyg_ucount16 /* height */,
                                            cyg_fb_colour);
extern void             cyg_fb_write_block( cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */,
                                            cyg_ucount16 /* width */,   cyg_ucount16 /* height */,
                                            const void*  /* source */,
                                            cyg_ucount16 /* offset */,  cyg_ucount16 /* source stride */);
extern void             cyg_fb_read_block(  cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */,
                                            cyg_ucount16 /* width */,   cyg_ucount16 /* height */,
                                            void*        /* dest */,
                                            cyg_ucount16 /* offset */,  cyg_ucount16 /* source stride */);
extern void             cyg_fb_move_block(  cyg_fb*,
                                            cyg_ucount16 /* x */,       cyg_ucount16 /* y */,
                                            cyg_ucount16 /* width */,   cyg_ucount16 /* height */,
                                            cyg_ucount16 /* new_x */,   cyg_ucount16 /* new_y */);
extern int              cyg_fb_ioctl(       cyg_fb*,
                                            cyg_uint16  /* key */,
                                            void*       /* data */,
                                            size_t*     /* len */);

extern void             cyg_fb_synch(       cyg_fb*,
                                            cyg_ucount16 /* when */);
    
extern void             cyg_fb_write_palette(   cyg_fb*,
                                                cyg_ucount32    /* first  */,
                                                cyg_ucount32    /* count  */,
                                                const void*     /* source */,
                                                cyg_ucount16    /* when   */);
extern void             cyg_fb_read_palette(    cyg_fb*,
                                                cyg_ucount32    /* first */,
                                                cyg_ucount32    /* count */,
                                                void*           /* dest  */);

extern cyg_fb_colour    cyg_fb_make_colour(     cyg_fb*,
                                               cyg_ucount8 /* r */, cyg_ucount8 /* g */, cyg_ucount8 /* b */);

extern void             cyg_fb_break_colour(    cyg_fb*,
                                                cyg_fb_colour,
                                                cyg_ucount8* /* r */, cyg_ucount8* /* g */, cyg_ucount8* /* b */);

#else

extern __inline__ int
cyg_fb_on(cyg_fb* _fb_)
{
    return (*(_fb_->fb_on_fn))(_fb_);
}

extern __inline__ int
cyg_fb_off(cyg_fb* _fb_)
{
    return (*(_fb_->fb_off_fn))(_fb_);
}

extern __inline__ void
cyg_fb_write_pixel(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_count32 _colour_)
{
    (*(_fb_->fb_write_pixel_fn))(_fb_, _x_, _y_, _colour_);
}

extern __inline__ cyg_fb_colour
cyg_fb_read_pixel(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_)
{
    return (*(_fb_->fb_read_pixel_fn))(_fb_, _x_, _y_);
}

extern __inline__ void
cyg_fb_write_hline(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_)
{
    (*(_fb_->fb_write_hline_fn))(_fb_, _x_, _y_, _len_, _colour_);
}

extern __inline__ void
cyg_fb_write_vline(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _len_, cyg_fb_colour _colour_)
{
    (*(_fb_->fb_write_vline_fn))(_fb_, _x_, _y_, _len_, _colour_);
}

extern __inline__ void
cyg_fb_fill_block(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_, cyg_fb_colour _colour_)
{
    (*(_fb_->fb_fill_block_fn))(_fb_, _x_, _y_, _width_, _height_, _colour_);
}

extern __inline__ void
cyg_fb_write_block(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                   const void* _source_, cyg_ucount16 _offset_, cyg_ucount16 _stride_)
{
    (*(_fb_->fb_write_block_fn))(_fb_, _x_, _y_, _width_, _height_, _source_, _offset_, _stride_);
}

extern __inline__ void
cyg_fb_read_block(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_,
                  void* _dest_, cyg_ucount16 _offset_, cyg_ucount16 _stride_)
{
    (*((_fb_)->fb_read_block_fn))(_fb_, _x_, _y_, _width_, _height_, _dest_, _offset_, _stride_);
}

extern __inline__ void
cyg_fb_move_block(cyg_fb* _fb_, cyg_ucount16 _x_, cyg_ucount16 _y_, cyg_ucount16 _width_, cyg_ucount16 _height_, cyg_ucount16 _newx_, cyg_ucount16 _newy_)
{
    (*(_fb_->fb_move_block_fn))(_fb_, _x_, _y_, _width_, _height_, _newx_, _newy_);
}

extern __inline__ int
cyg_fb_ioctl(cyg_fb* _fb_, cyg_uint16  _key_, void* _data_, size_t* _len_)
{
    int result = (*(_fb_->fb_ioctl_fn))(_fb_, _key_, _data_, _len_);
    return result;
}

extern __inline__ void
cyg_fb_synch(cyg_fb* _fb_, cyg_ucount16 _when_)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_DOUBLE_BUFFER    
    (*(_fb_->fb_synch_fn))(_fb_, _when_);
#else
    // Synch is a no-op
#endif    
}

extern __inline__ void
cyg_fb_read_palette(cyg_fb* _fb_, cyg_ucount32 _first_, cyg_ucount32 _count_, void* _dest_)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_PALETTE
    (*(_fb_->fb_read_palette_fn))(_fb_, _first_, _count_, _dest_);
#else
    CYG_UNUSED_PARAM(cyg_fb*, _fb_);
    CYG_UNUSED_PARAM(cyg_ucount32, _first_);
    CYG_UNUSED_PARAM(cyg_ucount32, _count_);
    CYG_UNUSED_PARAM(void*, _dest_);
#endif
}

extern __inline__ void
cyg_fb_write_palette(cyg_fb* _fb_, cyg_ucount32 _first_, cyg_ucount32 _count_, const void* _dest_, cyg_ucount16 _when_)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_WRITEABLE_PALETTE
    (*(_fb_->fb_write_palette_fn))(_fb_, _first_, _count_, _dest_, _when_);
#else
    CYG_UNUSED_PARAM(cyg_fb*, _fb_);
    CYG_UNUSED_PARAM(cyg_ucount32, _first_);
    CYG_UNUSED_PARAM(cyg_ucount32, _count_);
    CYG_UNUSED_PARAM(const void*, _dest_);
    CYG_UNUSED_PARAM(cyg_ucount16, _when_);
#endif
}

extern __inline__ cyg_fb_colour
cyg_fb_make_colour(cyg_fb* _fb_, cyg_ucount8 _r_, cyg_ucount8 _g_, cyg_ucount8 _b_)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_TRUE_COLOUR
    return (*(_fb_->fb_make_colour_fn))(_fb_, _r_, _g_, _b_);
#else
    CYG_UNUSED_PARAM(cyg_fb*, _fb_);
    CYG_UNUSED_PARAM(cyg_ucount8, _r_);
    CYG_UNUSED_PARAM(cyg_ucount8, _g_);
    CYG_UNUSED_PARAM(cyg_ucount8, _b_);
    return 0;
#endif    
}

extern __inline__ void
cyg_fb_break_colour(cyg_fb* _fb_, cyg_fb_colour _colour_, cyg_ucount8* _r_, cyg_ucount8* _g_, cyg_ucount8* _b_)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_TRUE_COLOUR
    (*(_fb_->fb_break_colour_fn))(_fb_, _colour_, _r_, _g_, _b_);
#else
    CYG_UNUSED_PARAM(cyg_fb*, _fb_);
    CYG_UNUSED_PARAM(cyg_fb_colour, _colour_);
    CYG_UNUSED_PARAM(cyg_ucount8*, _r_);
    CYG_UNUSED_PARAM(cyg_ucount8*, _g_);
    CYG_UNUSED_PARAM(cyg_ucount8*, _b_);
#endif    
}

#endif  // defined(CYGPKG_INFRA_DEBUG) || defined(__CYG_FB_IN_FRAMEBUF_C)

// ----------------------------------------------------------------------------
// The MACRO API
#define CYG_FB__AUX(_fb_, _field_)          CYG_FB_ ## _fb_ ## _field_

#define CYG_FB_STRUCT(_fb_)                 CYG_FB__AUX(_fb_, _STRUCT)
#define CYG_FB_DEPTH(_fb_)                  CYG_FB__AUX(_fb_, _DEPTH)
#define CYG_FB_FORMAT(_fb_)                 CYG_FB__AUX(_fb_, _FORMAT)
#define CYG_FB_WIDTH(_fb_)                  CYG_FB__AUX(_fb_, _WIDTH)
#define CYG_FB_HEIGHT(_fb_)                 CYG_FB__AUX(_fb_, _HEIGHT)
#define CYG_FB_VIEWPORT_WIDTH(_fb_)         CYG_FB__AUX(_fb_, _VIEWPORT_WIDTH)
#define CYG_FB_VIEWPORT_HEIGHT(_fb_)        CYG_FB__AUX(_fb_, _VIEWPORT_HEIGHT)
#define CYG_FB_STRIDE(_fb_)                 CYG_FB__AUX(_fb_, _STRIDE)
#define CYG_FB_BASE(_fb_)                   CYG_FB__AUX(_fb_, _BASE)
#define CYG_FB_FLAGS0(_fb_)                 CYG_FB__AUX(_fb_, _FLAGS0)
#define CYG_FB_FLAGS1(_fb_)                 CYG_FB__AUX(_fb_, _FLAGS1)
#define CYG_FB_FLAGS2(_fb_)                 CYG_FB__AUX(_fb_, _FLAGS2)
#define CYG_FB_FLAGS3(_fb_)                 CYG_FB__AUX(_fb_, _FLAGS3)

#define CYG_FB_PIXELx_VAR(  _fb_, _id_)                                     CYG_FB__AUX(_fb_, _PIXELx_VAR)(   _fb_, _id_)
#define CYG_FB_PIXELx_SET(  _fb_, _id_, _x_, _y_)                           CYG_FB__AUX(_fb_, _PIXELx_SET)(   _fb_, _id_, _x_, _y_)
#define CYG_FB_PIXELx_GET(  _fb_, _id_, _x_, _y_)                           CYG_FB__AUX(_fb_, _PIXELx_GET)(   _fb_, _id_, _x_, _y_)
#define CYG_FB_PIXELx_ADDX( _fb_, _id_, _incr_)                             CYG_FB__AUX(_fb_, _PIXELx_ADDX)(  _fb_, _id_, _incr_)
#define CYG_FB_PIXELx_ADDY( _fb_, _id_, _incr_)                             CYG_FB__AUX(_fb_, _PIXELx_ADDY)(  _fb_, _id_, _incr_)
#define CYG_FB_PIXELx_WRITE(_fb_, _id_, _colour_)                           CYG_FB__AUX(_fb_, _PIXELx_WRITE)( _fb_, _id_, _colour_)
#define CYG_FB_PIXELx_READ( _fb_, _id_)                                     CYG_FB__AUX(_fb_, _PIXELx_READ)(  _fb_, _id_)
#define CYG_FB_PIXELx_FLUSHABS(_fb_, _id_, _x0_, _y0_, _width_, _height_)   CYG_FB__AUX(_fb_, _PIXELx_FLUSHABS)( _fb_, _id_, _x0_, _y0_, _x1_, _y1_)
#define CYG_FB_PIXELx_FLUSHREL(_fb_, _id_, _x0_, _y0_, _dx_, _dy_)          CYG_FB__AUX(_fb_, _PIXELx_FLUSHREL)( _fb_, _id_, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXEL0_VAR(  _fb_)                                       CYG_FB_PIXELx_VAR(  _fb_, 0)
#define CYG_FB_PIXEL0_SET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_SET(  _fb_, 0, _x_, _y_)
#define CYG_FB_PIXEL0_GET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_GET(  _fb_, 0, _x_, _y_)
#define CYG_FB_PIXEL0_ADDX( _fb_, _incr_)                               CYG_FB_PIXELx_ADDX( _fb_, 0, _incr_)
#define CYG_FB_PIXEL0_ADDY( _fb_, _incr_)                               CYG_FB_PIXELx_ADDY( _fb_, 0, _incr_)
#define CYG_FB_PIXEL0_WRITE(_fb_, _colour_)                             CYG_FB_PIXELx_WRITE(_fb_, 0, _colour_)
#define CYG_FB_PIXEL0_READ( _fb_)                                       CYG_FB_PIXELx_READ( _fb_, 0)
#define CYG_FB_PIXEL0_FLUSHABS( _fb_, _x0_, _y0_, _width_, _height_)    CYG_FB_PIXELx_FLUSHABS( _fb_, 0, _x0_, _y0_, _width_, _height_)
#define CYG_FB_PIXEL0_FLUSHREL( _fb_, _x0_, _y0_, _dx_, _dy_)           CYG_FB_PIXELx_FLUSHREL( _fb_, 0, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXEL1_VAR(  _fb_)                                       CYG_FB_PIXELx_VAR(  _fb_, 1)
#define CYG_FB_PIXEL1_SET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_SET(  _fb_, 1, _x_, _y_)
#define CYG_FB_PIXEL1_GET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_GET(  _fb_, 1, _x_, _y_)
#define CYG_FB_PIXEL1_ADDX( _fb_, _incr_)                               CYG_FB_PIXELx_ADDX( _fb_, 1, _incr_)
#define CYG_FB_PIXEL1_ADDY( _fb_, _incr_)                               CYG_FB_PIXELx_ADDY( _fb_, 1, _incr_)
#define CYG_FB_PIXEL1_WRITE(_fb_, _colour_)                             CYG_FB_PIXELx_WRITE(_fb_, 1, _colour_)
#define CYG_FB_PIXEL1_READ( _fb_)                                       CYG_FB_PIXELx_READ( _fb_, 1)
#define CYG_FB_PIXEL1_FLUSHABS( _fb_, _x0_, _y0_, _width_, _height_)    CYG_FB_PIXELx_FLUSHABS( _fb_, 1, _x0_, _y0_, _width_, _height_)
#define CYG_FB_PIXEL1_FLUSHREL( _fb_, _x0_, _y0_, _dx_, _dy_)           CYG_FB_PIXELx_FLUSHREL( _fb_, 1, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXEL2_VAR(  _fb_)                                       CYG_FB_PIXELx_VAR(  _fb_, 2)
#define CYG_FB_PIXEL2_SET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_SET(  _fb_, 2, _x_, _y_)
#define CYG_FB_PIXEL2_GET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_GET(  _fb_, 2, _x_, _y_)
#define CYG_FB_PIXEL2_ADDX( _fb_, _incr_)                               CYG_FB_PIXELx_ADDX( _fb_, 2, _incr_)
#define CYG_FB_PIXEL2_ADDY( _fb_, _incr_)                               CYG_FB_PIXELx_ADDY( _fb_, 2, _incr_)
#define CYG_FB_PIXEL2_WRITE(_fb_, _colour_)                             CYG_FB_PIXELx_WRITE(_fb_, 2, _colour_)
#define CYG_FB_PIXEL2_READ( _fb_)                                       CYG_FB_PIXELx_READ( _fb_, 2)
#define CYG_FB_PIXEL2_FLUSHABS( _fb_, _x0_, _y0_, _width_, _height_)    CYG_FB_PIXELx_FLUSHABS( _fb_, 2, _x0_, _y0_, _width_, _height_)
#define CYG_FB_PIXEL2_FLUSHREL( _fb_, _x0_, _y0_, _dx_, _dy_)           CYG_FB_PIXELx_FLUSHREL( _fb_, 2, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_PIXEL3_VAR(  _fb_)                                       CYG_FB_PIXELx_VAR(  _fb_, 3)
#define CYG_FB_PIXEL3_SET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_SET(  _fb_, 3, _x_, _y_)
#define CYG_FB_PIXEL3_GET(  _fb_, _x_, _y_)                             CYG_FB_PIXELx_GET(  _fb_, 3, _x_, _y_)
#define CYG_FB_PIXEL3_ADDX( _fb_, _incr_)                               CYG_FB_PIXELx_ADDX( _fb_, 3, _incr_)
#define CYG_FB_PIXEL3_ADDY( _fb_, _incr_)                               CYG_FB_PIXELx_ADDY( _fb_, 3, _incr_)
#define CYG_FB_PIXEL3_WRITE(_fb_, _colour_)                             CYG_FB_PIXELx_WRITE(_fb_, 3, _colour_)
#define CYG_FB_PIXEL3_READ( _fb_)                                       CYG_FB_PIXELx_READ( _fb_, 3)
#define CYG_FB_PIXEL3_FLUSHABS( _fb_, _x0_, _y0_, _width_, _height_)    CYG_FB_PIXELx_FLUSHABS( _fb_, 3, _x0_, _y0_, _width_, _height_)
#define CYG_FB_PIXEL3_FLUSHREL( _fb_, _x0_, _y0_, _dx_, _dy_)           CYG_FB_PIXELx_FLUSHREL( _fb_, 3, _x0_, _y0_, _dx_, _dy_)

#define CYG_FB_ON(_fb_)                             ({ cyg_fb_on(& CYG_FB_STRUCT(_fb_)); })
#define CYG_FB_OFF(_fb_)                            ({ cyg_fb_off(& CYG_FB_STRUCT(_fb_)); })
#define CYG_FB_IOCTL(_fb_, _key_, _data_, _len_)    ({ cyg_fb_ioctl(& CYG_FB_STRUCT(_fb_), _key_, _data_, _len_); })
#define CYG_FB_SYNCH(_fb_, _when_)                                                      \
    CYG_MACRO_START                                                                     \
    cyg_fb_synch(& CYG_FB_STRUCT(_fb_), _when_);                                        \
    CYG_MACRO_END
#define CYG_FB_WRITE_PALETTE(_fb_, _first_, _count_, _data_, _when_)                    \
    CYG_MACRO_START                                                                     \
    cyg_fb_write_palette(& CYG_FB_STRUCT(_fb_), _first_, _count_, _data_, _when_);      \
    CYG_MACRO_END
#define CYG_FB_READ_PALETTE(_fb_, _first_, _count_, _data_)                             \
    CYG_MACRO_START                                                                     \
    cyg_fb_read_palette(& CYG_FB_STRUCT(_fb_), _first_, _count_, _data_);               \
    CYG_MACRO_END

#define CYG_FB_WRITE_PIXEL(_fb_, _x_, _y_, _colour_)                                        \
    CYG_FB__AUX(_fb_, _WRITE_PIXEL)(_x_, _y_, _colour_)
#define CYG_FB_READ_PIXEL(_fb_, _x_, _y_)                                                   \
    CYG_FB__AUX(_fb_, _READ_PIXEL)(_x_, _y_)
#define CYG_FB_WRITE_HLINE(_fb_, _x_, _y_, _len_, _colour_)                                 \
    CYG_FB__AUX(_fb_, _WRITE_HLINE)(_x_, _y_, _len_, _colour_)
#define CYG_FB_WRITE_VLINE(_fb_, _x_, _y_, _len_, _colour_)                                 \
    CYG_FB__AUX(_fb_, _WRITE_VLINE)(_x_, _y_, _len_, _colour_)
#define CYG_FB_FILL_BLOCK(_fb_, _x_, _y_, _width_, _height_, _colour_)                      \
    CYG_FB__AUX(_fb_, _FILL_BLOCK)(_x_, _y_, _width_, _height_, _colour_)
#define CYG_FB_WRITE_BLOCK(_fb_, _x_, _y_, _width_, _height_, _data_, _offset_, _stride_)   \
    CYG_FB__AUX(_fb_, _WRITE_BLOCK)(_x_, _y_, _width_, _height_, _data_, _offset_, _stride_)
#define CYG_FB_READ_BLOCK(_fb_, _x_, _y_, _width_, _height_, _data_, _offset_, _stride_)    \
    CYG_FB__AUX(_fb_, _READ_BLOCK)(_x_, _y_, _width_, _height_, _data_, _offset_, _stride_)
#define CYG_FB_MOVE_BLOCK(_fb_, _x_, _y_, _width_, _height_, _new_x_, _new_y_)              \
    CYG_FB__AUX(_fb_, _MOVE_BLOCK)(_x_, _y_, _width_, _height_, _new_x_, _new_y_)
#define CYG_FB_MAKE_COLOUR(_fb_, _r_, _g_, _b_)                                             \
    CYG_FB__AUX(_fb_, _MAKE_COLOUR)(_r_, _g_, _b_)
#define CYG_FB_MAKE_COLOR(_fb_, _r_, _g_, _b_)                                              \
    CYG_FB__AUX(_fb_, _MAKE_COLOUR)(_r_, _g_, _b_)
#define CYG_FB_BREAK_COLOUR(_fb_, _colour_, _r_, _g_, _b_)                                  \
    CYG_FB__AUX(_fb_, _BREAK_COLOUR)(_colour_, _r_, _g_, _b_)
#define CYG_FB_BREAK_COLOR(_fb_, _colour_, _r_, _g_, _b_)                                   \
    CYG_FB__AUX(_fb_, _BREAK_COLOUR)(_colour_, _r_, _g_, _b_)

// ----------------------------------------------------------------------------
// Default implementations of some of the per-device functions for linear
// framebuffers. Control operations like initialization are always
// device-specific and cannot be implemented by the generic package.

extern void cyg_fb_linear_write_pixel_1LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_1BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_2LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_2BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_4LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_4BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_8(   cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_16(  cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_pixel_32(  cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_fb_colour);

extern cyg_fb_colour   cyg_fb_linear_read_pixel_1LE(  cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_1BE(  cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_2LE(  cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_2BE(  cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_4LE(  cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_4BE(  cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_8(    cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_16(   cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);
extern cyg_fb_colour   cyg_fb_linear_read_pixel_32(   cyg_fb*,
                                                      cyg_ucount16 /* x */, cyg_ucount16 /* y */);

extern void cyg_fb_linear_write_hline_1LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_1BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_2LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_2BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_4LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_4BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_8(   cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_16(  cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_hline_32(  cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);

extern void cyg_fb_linear_write_vline_1LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_1BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_2LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_2BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_4LE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_4BE( cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_8(   cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_16(  cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);
extern void cyg_fb_linear_write_vline_32(  cyg_fb*,
                                           cyg_ucount16 /* x */,   cyg_ucount16 /* y */,
                                           cyg_ucount16 /* len */,
                                           cyg_fb_colour);

extern void cyg_fb_linear_fill_block_1LE( cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_1BE( cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_2LE( cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_2BE( cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_4LE( cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_4BE( cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_8(   cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_16(  cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);
extern void cyg_fb_linear_fill_block_32(  cyg_fb*,
                                          cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                          cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                          cyg_fb_colour);

extern void cyg_fb_linear_write_block_1LE( cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
extern void cyg_fb_linear_write_block_1BE( cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
extern void cyg_fb_linear_write_block_2LE( cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
                                           
extern void cyg_fb_linear_write_block_2BE( cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
extern void cyg_fb_linear_write_block_4LE( cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
extern void cyg_fb_linear_write_block_4BE( cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
extern void cyg_fb_linear_write_block_8(   cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
extern void cyg_fb_linear_write_block_16(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);
extern void cyg_fb_linear_write_block_32(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           const void*  /* source */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /* stride */);

extern void cyg_fb_linear_read_block_1LE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void*        /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_1BE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_2LE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_2BE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_4LE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_4BE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_8(    cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_16(   cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);
extern void cyg_fb_linear_read_block_32(   cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           void* /* dest */,
                                           cyg_ucount16 /* offset */,   cyg_ucount16 /*  stride */);

extern void cyg_fb_linear_move_block_1LE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_1BE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_2LE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_2BE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_4LE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_4BE(  cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_8(    cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_16(   cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);
extern void cyg_fb_linear_move_block_32(   cyg_fb*,
                                           cyg_ucount16 /* x */,        cyg_ucount16 /* y */,
                                           cyg_ucount16 /* width */,    cyg_ucount16 /* height */,
                                           cyg_ucount16 /* new_x */,    cyg_ucount16 /* new_y */);

// ----------------------------------------------------------------------------
// Dummy implementations of various device functions

extern int              cyg_fb_nop_on(cyg_fb*);
extern int              cyg_fb_nop_off(cyg_fb*);
extern int              cyg_fb_nop_ioctl(cyg_fb*, cyg_uint16, void*, size_t*);
extern void             cyg_fb_nop_synch(cyg_fb*, cyg_ucount16);
extern void             cyg_fb_nop_write_palette(cyg_fb*, cyg_ucount32, cyg_ucount32, const void*, cyg_ucount16);
extern void             cyg_fb_nop_read_palette( cyg_fb*, cyg_ucount32, cyg_ucount32, void*);
extern cyg_fb_colour    cyg_fb_nop_make_colour(cyg_fb*, cyg_ucount8, cyg_ucount8, cyg_ucount8);
extern cyg_fb_colour    cyg_fb_nop_make_color(cyg_fb*, cyg_ucount8, cyg_ucount8, cyg_ucount8);
extern void             cyg_fb_nop_break_colour(cyg_fb*, cyg_fb_colour, cyg_ucount8*, cyg_ucount8*, cyg_ucount8*);
extern void             cyg_fb_nop_break_color(cyg_fb*, cyg_fb_colour, cyg_ucount8*, cyg_ucount8*, cyg_ucount8*);

// ----------------------------------------------------------------------------
// Standard palettes. The generic framebuffer package provides EGA and VGA
// palettes.
extern const cyg_uint8  cyg_fb_palette_ega[16 * 3];
extern const cyg_uint8  cyg_fb_palette_vga[256 * 3];

#define CYG_FB_DEFAULT_PALETTE_BLACK        0x00
#define CYG_FB_DEFAULT_PALETTE_BLUE         0x01
#define CYG_FB_DEFAULT_PALETTE_GREEN        0x02
#define CYG_FB_DEFAULT_PALETTE_CYAN         0x03
#define CYG_FB_DEFAULT_PALETTE_RED          0x04
#define CYG_FB_DEFAULT_PALETTE_MAGENTA      0x05
#define CYG_FB_DEFAULT_PALETTE_BROWN        0x06
#define CYG_FB_DEFAULT_PALETTE_LIGHTGREY    0x07
#define CYG_FB_DEFAULT_PALETTE_LIGHTGRAY    0x07
#define CYG_FB_DEFAULT_PALETTE_DARKGREY     0x08
#define CYG_FB_DEFAULT_PALETTE_DARKGRAY     0x08
#define CYG_FB_DEFAULT_PALETTE_LIGHTBLUE    0x09
#define CYG_FB_DEFAULT_PALETTE_LIGHTGREEN   0x0A
#define CYG_FB_DEFAULT_PALETTE_LIGHTCYAN    0x0B
#define CYG_FB_DEFAULT_PALETTE_LIGHTRED     0x0C
#define CYG_FB_DEFAULT_PALETTE_LIGHTMAGENTA 0x0D
#define CYG_FB_DEFAULT_PALETTE_YELLOW       0x0E
#define CYG_FB_DEFAULT_PALETTE_WHITE        0x0F

// ----------------------------------------------------------------------------
// Support for some common true colour modes
#define CYG_FB_MAKE_COLOUR_8BPP_TRUE_332(_r_, _g_, _b_)                                             \
    ({  cyg_fb_colour _colour_;                                                                     \
        _colour_ = ((((_r_) & 0x00E0) >> 0) | (((_g_) & 0x00E0) >> 3) | (((_b_) & 0x00C0) >> 6));   \
        _colour_; })

#define CYG_FB_BREAK_COLOUR_8BPP_TRUE_332(_colour_, _r_, _g_, _b_ )                                 \
    CYG_MACRO_START                                                                                 \
    *(_r_) = ((_colour_) << 0) & 0x00E0;                                                            \
    *(_g_) = ((_colour_) << 3) & 0x00E0;                                                            \
    *(_g_) = ((_colour_) << 6) & 0x00C0;                                                            \
    CYG_MACRO_END

#define CYG_FB_MAKE_COLOUR_16BPP_TRUE_565(_r_, _g_, _b_)                                            \
    ({ ((((_r_) & 0x00F8) << 8) | (((_g_) & 0x00FC) << 3) | (((_b_) & 0x00F8) >> 3)); })

#define CYG_FB_BREAK_COLOUR_16BPP_TRUE_565(_colour_, _r_, _g_, _b_)                                 \
    CYG_MACRO_START                                                                                 \
    *(_r_) = ((_colour_) >> 8) & 0x00F8;                                                            \
    *(_g_) = ((_colour_) >> 3) & 0x00FC;                                                            \
    *(_b_) = ((_colour_) << 3) & 0x00F8;                                                            \
    CYG_MACRO_END

#define CYG_FB_MAKE_COLOUR_16BPP_TRUE_555(_r_, _g_, _b_)                                            \
    ({ ((((_r_) & 0x00F8) << 7) | (((_g_) & 0x00F8) << 2) | (((_b_) & 0x00F8) >> 3)); })

#define CYG_FB_BREAK_COLOUR_16BPP_TRUE_555(_colour_, _r_, _g_, _b_)                             \
    CYG_MACRO_START                                                                             \
    *(_r_) = ((_colour_) >> 7) & 0x00F8;                                                        \
    *(_g_) = ((_colour_) >> 2) & 0x00F8;                                                        \
    *(_b_) = ((_colour_) << 3) & 0x00F8;                                                        \
    CYG_MACRO_END

#define CYG_FB_MAKE_COLOUR_32BPP_TRUE_0888(_r_, _g_, _b_)                                       \
    ({ (((_r_) << 16) | ((_g_) << 8) | (_b_)); })

#define CYG_FB_BREAK_COLOUR_32BPP_TRUE_0888(_colour_, _r_, _g_, _b_)                            \
    CYG_MACRO_START                                                                             \
    *(_r_) = ((_colour_) >> 16) & 0x00FF;                                                       \
    *(_g_) = ((_colour_) >>  8) & 0x00FF;                                                       \
    *(_b_) = ((_colour_)      ) & 0x00FF;                                                       \
    CYG_MACRO_END

extern cyg_fb_colour    cyg_fb_dev_make_colour_8bpp_true_332(cyg_fb*, cyg_ucount8, cyg_ucount8, cyg_ucount8);
extern void             cyg_fb_dev_break_colour_8bpp_true_332(cyg_fb*, cyg_fb_colour, cyg_ucount8*, cyg_ucount8*, cyg_ucount8*);
extern cyg_fb_colour    cyg_fb_dev_make_colour_16bpp_true_565(cyg_fb*, cyg_ucount8, cyg_ucount8, cyg_ucount8);
extern void             cyg_fb_dev_break_colour_16bpp_true_565(cyg_fb*, cyg_fb_colour, cyg_ucount8*, cyg_ucount8*, cyg_ucount8*);
extern cyg_fb_colour    cyg_fb_dev_make_colour_16bpp_true_555(cyg_fb*, cyg_ucount8, cyg_ucount8, cyg_ucount8);
extern void             cyg_fb_dev_break_colour_16bpp_true_555(cyg_fb*, cyg_fb_colour, cyg_ucount8*, cyg_ucount8*, cyg_ucount8*);
extern cyg_fb_colour    cyg_fb_dev_make_colour_32bpp_true_0888(cyg_fb*, cyg_ucount8, cyg_ucount8, cyg_ucount8);
extern void             cyg_fb_dev_break_colour_32bpp_true_0888(cyg_fb*, cyg_fb_colour, cyg_ucount8*, cyg_ucount8*, cyg_ucount8*);

// ----------------------------------------------------------------------------
// A custom build step combines all the header files from the various
// framebuffer devices (usually just one) into a single header. This
// makes it easier for portable higher-level code to work with
// whatever devices happen to be available.
#include <cyg/io/framebufs/framebufs.h>

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif // ifndef CYGONCE_IO_FRAMEBUF_H
// End of framebuf.h
