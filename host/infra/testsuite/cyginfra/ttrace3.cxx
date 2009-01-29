//==========================================================================
//
//      ttrace3.cxx
//
//      Trace test case                                                                
//
//==========================================================================
// ####ECOSHOSTGPLCOPYRIGHTBEGIN####                                        
// -------------------------------------------                              
// This file is part of the eCos host tools.                                
// Copyright (C) 1999, 2000 Free Software Foundation, Inc.                  
//
// This program is free software; you can redistribute it and/or modify     
// it under the terms of the GNU General Public License as published by     
// the Free Software Foundation; either version 2 or (at your option) any   
// later version.                                                           
//
// This program is distributed in the hope that it will be useful, but      
// WITHOUT ANY WARRANTY; without even the implied warranty of               
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        
// General Public License for more details.                                 
//
// You should have received a copy of the GNU General Public License        
// along with this program; if not, write to the                            
// Free Software Foundation, Inc., 51 Franklin Street,                      
// Fifth Floor, Boston, MA  02110-1301, USA.                                
// -------------------------------------------                              
// ####ECOSHOSTGPLCOPYRIGHTEND####                                          
//==========================================================================
//#####DESCRIPTIONBEGIN####                                             
//
// Author(s):           bartv
// Contributors:        bartv
// Date:                1999-01-06
// Purpose:
// Description:         Do an awful lot of tracing, to make sure that the
//                      circular buffer is circular.
//
//####DESCRIPTIONEND####
//==========================================================================


#define CYGDBG_USE_TRACING
#define CYGDBG_INFRA_DEBUG_FUNCTION_REPORTS
#include <cyg/infra/testcase.h>
#include <cyg/infra/cyg_trac.h>
#include <cstdlib>

void
dummy(void)
{
    CYG_REPORT_FUNCNAME("dummy");
    CYG_REPORT_RETURN();
}

int
main(int argc, char** argv)
{
    for (int i = 0; i < 1000000; i++) {
        CYG_TRACE0(true, "no argument here");
        dummy();
    }
    
    CYG_TEST_PASS_FINISH("Lots of tracing is possible");
}


