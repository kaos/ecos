//==========================================================================
//
//      synthfb_init.cxx
//
//      Instantiate one or more framebuffer devices for the synthetic target.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009 Free Software Foundation, Inc.                        
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
// Date:          2007-02-0
//
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/devs_framebuf_synth.h>
#include <cyg/io/framebuf.h>

extern "C" void _cyg_synth_fb_instantiate(struct cyg_fb*);
                                          
class _synth_fb_init {
    
  public:
    _synth_fb_init(void)
    {
#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB0
        _cyg_synth_fb_instantiate(&cyg_synth_fb0);
#endif        
#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB1
        _cyg_synth_fb_instantiate(&cyg_synth_fb1);
#endif        
#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB2
        _cyg_synth_fb_instantiate(&cyg_synth_fb2);
#endif        
#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB3
        _cyg_synth_fb_instantiate(&cyg_synth_fb3);
#endif        
    }
};

static _synth_fb_init _synth_fb_init_object CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_DEV_CHAR);
