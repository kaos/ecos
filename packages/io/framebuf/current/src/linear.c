//==========================================================================
//
//      linear.c
//
//      Generic implementations of some of the driver functions for linear
//      framebuffers.
//
//==========================================================================
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
//==========================================================================
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2005-03-29
//
//###DESCRIPTIONEND####
//========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/io/framebuf.h>

// The real implementations for most of the functions are actually
// inlines held in framebuf.inl. That allows the implementation to be
// shared between the functions and the macros.
#include <cyg/io/framebuf.inl>

// ----------------------------------------------------------------------------
void
cyg_fb_linear_write_pixel_1LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_1LE_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_1LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_1LE_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_1LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_vline_1LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_1LE_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_1LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_block_1LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                              const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
}

void
cyg_fb_linear_read_block_1LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
}

void
cyg_fb_linear_move_block_1LE_impl(void* fbaddr, cyg_ucount16 stride,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
}

void
cyg_fb_linear_move_block_1LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_1LE_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}

// ----------------------------------------------------------------------------
void
cyg_fb_linear_write_pixel_1BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_1BE_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_1BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_1BE_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_1BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_vline_1BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_1BE_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_1BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_block_1BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                              const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
}

void
cyg_fb_linear_read_block_1BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
}

void
cyg_fb_linear_move_block_1BE_impl(void* fbaddr, cyg_ucount16 stride,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
}

void
cyg_fb_linear_move_block_1BE(cyg_fb* fb,
                          cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                          cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_1LE_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}

// ----------------------------------------------------------------------------

void
cyg_fb_linear_write_pixel_2LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_2LE_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_2LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_2LE_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_2LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_vline_2LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_2LE_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_2LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_block_2LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                              const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
}

void
cyg_fb_linear_read_block_2LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
}

void
cyg_fb_linear_move_block_2LE_impl(void* fbaddr, cyg_ucount16 stride,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
}

void
cyg_fb_linear_move_block_2LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_2LE_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}

// ----------------------------------------------------------------------------

void
cyg_fb_linear_write_pixel_2BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_2BE_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_2BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_2BE_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_2BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_vline_2BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_2BE_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_2BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_block_2BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                              const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
}

void
cyg_fb_linear_read_block_2BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
}

void
cyg_fb_linear_move_block_2BE_impl(void* fbaddr, cyg_ucount16 stride,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
}

void
cyg_fb_linear_move_block_2BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_2BE_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}
    
// ----------------------------------------------------------------------------
void
cyg_fb_linear_write_pixel_4LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_4LE_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_4LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_4LE_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_4LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_vline_4LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_4LE_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_4LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_block_4LE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                              const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
}

void
cyg_fb_linear_read_block_4LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
}

void
cyg_fb_linear_move_block_4LE_impl(void* fbaddr, cyg_ucount16 stride,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
}

void
cyg_fb_linear_move_block_4LE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_4LE_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}

// ----------------------------------------------------------------------------
void
cyg_fb_linear_write_pixel_4BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_4BE_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_4BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_4BE_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_4BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_4BE_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_write_vline_4BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_fill_block_4BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
}

void
cyg_fb_linear_write_block_4BE(cyg_fb* fb,
                              cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                              const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
}

void
cyg_fb_linear_read_block_4BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
}

void
cyg_fb_linear_move_block_4BE_impl(void* fbaddr, cyg_ucount16 stride,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
}

void
cyg_fb_linear_move_block_4BE(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_4BE_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}

// ----------------------------------------------------------------------------

void
cyg_fb_linear_write_pixel_8(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_8_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_8(cyg_fb* fb,
                           cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_8_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_8(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_hline_8_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_write_vline_8(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_8_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_8(cyg_fb* fb,
                           cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
    cyg_fb_linear_fill_block_8_inl(fb->fb_base, fb->fb_stride, x, y, width, height, colour);
}

void
cyg_fb_linear_write_block_8(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                            const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
    cyg_fb_linear_write_block_8_inl(fb->fb_base, fb->fb_stride, x, y, width, height, source, offset, source_stride);
}

void
cyg_fb_linear_read_block_8(cyg_fb* fb,
                           cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                           void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
    cyg_fb_linear_read_block_8_inl(fb->fb_base, fb->fb_stride, x, y, width, height, dest, offset, dest_stride);
}

void
cyg_fb_linear_move_block_8_impl(void* fbaddr, cyg_ucount16 stride,
                                cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_uint8*  source;
    cyg_uint8*  dest;
    int         i;
    
    source  = ((cyg_uint8*)fbaddr) + (    y * stride) +     x;
    dest    = ((cyg_uint8*)fbaddr) + (new_y * stride) + new_x;
    
    if ((new_y < y)             ||  // Possible overlapping blocks but no conflict
        (new_y > (y + height))  ||  // No overlap
        ((new_x + width) < x)   ||  // No overlap
        (new_x > (x + width))) {    // No overlap
        
        for ( ; height; height--) {
            __builtin_memcpy(dest, source, width);
            source  += stride;
            dest    += stride;
        }
        return;
    }

    // There is an overlap. Can we do a safe bottom-to-top lot of memcpy()'s ?
    if (new_y > y) {
        source += (height * stride);
        dest   += (height * stride);
        
        for ( ; height; height--) {
            source  -= stride;
            dest    -= stride;
            __builtin_memcpy(dest, source, width);
        }
        return;
    }

    // We must have (y == new_y) and an overlap, i.e. a short
    // horizontal move. We could use memmove() but the default
    // implementation is no better than what we can do here.
    if (new_x < x) {
        stride -= width;
        for ( ; height; height--) {
            for ( i = width; i ; i--) {
                *dest++ = *source++;
            }
            source  += stride;
            dest    += stride;
        }
    } else if (new_x > x) {
        source  += width;
        dest    += width;
        stride  += width;
        for ( ; height; height--) {
            for ( i = width; i; i--) {
                *--dest = *--source;
            }
            source  += stride;
            dest    += stride;
        }
    }
}

void
cyg_fb_linear_move_block_8(cyg_fb* fb,
                           cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                           cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_8_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}

// ----------------------------------------------------------------------------
void
cyg_fb_linear_write_pixel_16(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_16_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_16(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_16_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_16(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_hline_16_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_write_vline_16(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_16_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_16(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
    cyg_fb_linear_fill_block_16_inl(fb->fb_base, fb->fb_stride, x, y, width, height, colour);
}

void
cyg_fb_linear_write_block_16(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
    cyg_fb_linear_write_block_16_inl(fb->fb_base, fb->fb_stride, x, y, width, height, source, offset, source_stride);
}

void
cyg_fb_linear_read_block_16(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                            void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
    cyg_fb_linear_read_block_16_inl(fb->fb_base, fb->fb_stride, x, y, width, height, dest, offset, dest_stride);
}

void
cyg_fb_linear_move_block_16_impl(void* fbaddr, cyg_ucount16 stride8,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_uint16* source;
    cyg_uint16* dest;
    int         i;
    
    source  = ((cyg_uint16*)(((cyg_uint8*)fbaddr) + (    y * stride8))) + x;
    dest    = ((cyg_uint16*)(((cyg_uint8*)fbaddr) + (new_y * stride8))) + new_x;
    
    if ((new_y < y)             ||  // Possible overlapping blocks but no conflict
        (new_y > (y + height))  ||  // No overlap
        ((new_x + width) < x)   ||  // No overlap
        (new_x > (x + width))) {    // No overlap
        
        width <<= 1;
        for ( ; height; height--) {
            __builtin_memcpy(dest, source, width);
            source  = (cyg_uint16*)(((cyg_uint8*)source) + stride8);
            dest    = (cyg_uint16*)(((cyg_uint8*)source) + stride8);
        }
        return;
    }

    // There is an overlap. Can we do a safe bottom-to-top lot of memcpy()'s ?
    if (new_y > y) {
        source = (cyg_uint16*)(((cyg_uint8*)source) + (height * stride8));
        dest   = (cyg_uint16*)(((cyg_uint8*)dest)   + (height * stride8));
        width <<= 1;
        for ( ; height; height--) {
            source   = (cyg_uint16*)(((cyg_uint8*)source) - stride8);
            dest     = (cyg_uint16*)(((cyg_uint8*)dest) - stride8);
            __builtin_memcpy(dest, source, width);
        }
        return;
    }

    // We must have (y == new_y) and an overlap, i.e. a short
    // horizontal move. We could use memmove() but the default
    // implementation is no better than what we can do here.
    if (new_x < x) {
        stride8 -= (width << 1);
        for ( ; height; height--) {
            for ( i = width; i ; i--) {
                *dest++ = *source++;
            }
            source  = (cyg_uint16*)(((cyg_uint8*)source) + stride8);
            dest    = (cyg_uint16*)(((cyg_uint8*)dest) + stride8);
        }
    } else if (new_x > x) {
        source  += width;
        dest    += width;
        stride8 += (width << 1);
        for ( ; height; height--) {
            for ( i = width; i ; i--) {
                *--dest = *--source;
            }
            source  = (cyg_uint16*)(((cyg_uint8*)source) + stride8);
            dest    = (cyg_uint16*)(((cyg_uint8*)dest) + stride8);
        }
    }
}

void
cyg_fb_linear_move_block_16(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                            cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_16_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}

// ----------------------------------------------------------------------------
void
cyg_fb_linear_write_pixel_32(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    cyg_fb_linear_write_pixel_32_inl(fb->fb_base, fb->fb_stride, x, y, colour);
}

cyg_fb_colour
cyg_fb_linear_read_pixel_32(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y)
{
    return cyg_fb_linear_read_pixel_32_inl(fb->fb_base, fb->fb_stride, x, y);
}

void
cyg_fb_linear_write_hline_32(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_hline_32_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_write_vline_32(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    cyg_fb_linear_write_vline_32_inl(fb->fb_base, fb->fb_stride, x, y, len, colour);
}

void
cyg_fb_linear_fill_block_32(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
    cyg_fb_linear_fill_block_32_inl(fb->fb_base, fb->fb_stride, x, y, width, height, colour);
}

void
cyg_fb_linear_write_block_32(cyg_fb* fb,
                             cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                             const void* source, cyg_ucount16 offset, cyg_ucount16 source_stride)
{
    cyg_fb_linear_write_block_32_inl(fb->fb_base, fb->fb_stride, x, y, width, height, source, offset, source_stride);
}

void
cyg_fb_linear_read_block_32(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                            void* dest, cyg_ucount16 offset, cyg_ucount16 dest_stride)
{
    cyg_fb_linear_read_block_32_inl(fb->fb_base, fb->fb_stride, x, y, width, height, dest, offset, dest_stride);
}

void
cyg_fb_linear_move_block_32_impl(void* fbaddr, cyg_ucount16 stride8,
                                  cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                                  cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_uint32* source;
    cyg_uint32* dest;
    int         i;
    
    source  = ((cyg_uint32*)(((cyg_uint8*)fbaddr) + (    y * stride8))) + x;
    dest    = ((cyg_uint32*)(((cyg_uint8*)fbaddr) + (new_y * stride8))) + new_x;
    
    if ((new_y < y)             ||  // Possible overlapping blocks but no conflict
        (new_y > (y + height))  ||  // No overlap
        ((new_x + width) < x)   ||  // No overlap
        (new_x > (x + width))) {    // No overlap
        
        width <<= 2;
        for ( ; height; height--) {
            __builtin_memcpy(dest, source, width);
            source  = (cyg_uint32*)(((cyg_uint8*)source) + stride8);
            dest    = (cyg_uint32*)(((cyg_uint8*)source) + stride8);
        }
        return;
    }

    // There is an overlap. Can we do a safe bottom-to-top lot of memcpy()'s ?
    if (new_y > y) {
        source  = (cyg_uint32*)(((cyg_uint8*)source) + (height * stride8));
        dest    = (cyg_uint32*)(((cyg_uint8*)dest)   + (height * stride8));
        width <<= 2;
        for ( ; height; height--) {
            source   = (cyg_uint32*)(((cyg_uint8*)source) - stride8);
            dest     = (cyg_uint32*)(((cyg_uint8*)dest) - stride8);
            __builtin_memcpy(dest, source, width);
        }
        return;
    }

    // We must have (y == new_y) and an overlap, i.e. a short
    // horizontal move. We could use memmove() but the default
    // implementation is no better than what we can do here.
    if (new_x < x) {
        stride8 -= (width << 2);
        for ( ; height; height--) {
            for ( i = width; i ; i--) {
                *dest++ = *source++;
            }
            source  = (cyg_uint32*)(((cyg_uint8*)source) + stride8);
            dest    = (cyg_uint32*)(((cyg_uint8*)dest) + stride8);
        }
    } else if (new_x > x) {
        source  += width;
        dest    += width;
        stride8 += (width << 2);
        for ( ; height; height--) {
            for ( i = width; i ; i--) {
                *--dest = *--source;
            }
            source  = (cyg_uint32*)(((cyg_uint8*)source) + stride8);
            dest    = (cyg_uint32*)(((cyg_uint8*)dest) + stride8);
        }
    }
}

void
cyg_fb_linear_move_block_32(cyg_fb* fb,
                            cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                            cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    cyg_fb_linear_move_block_32_inl(fb->fb_base, fb->fb_stride, x, y, width, height, new_x, new_y);
}
