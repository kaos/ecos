//=================================================================
//
//        ezxml.c
//
//        ezml test cases
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 Andrew Lunn
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     asl
// Contributors:  asl
// Date:          2005-01-31
// Description:   
//####DESCRIPTIONEND####

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>
#include <ezxml.h>

static char document[] = 
"<?xml version=\"1.0\"?>"
"<formula1>"
"  <team name=\"McLaren\">"
"    <driver>"
"      <name>Kimi Raikkonen</name>"
"      <points>45</points>"
"    </driver>"
"    <driver>"
"      <name>David Coultard</name>"
"      <points>24</points>"
"    </driver>"
"  </team>"
"</formula1>";

struct results_s
{
  char * driver;
  char * team;
  char * points;
};

static struct results_s results[]=
  {{ "Kimi Raikkonen", "McLaren", "45"},
   { "David Coultard", "McLaren", "24"}};

externC void
cyg_start( void )
{
  ezxml_t f1, team, driver;
  const char * teamname;
  int i;
  
  CYG_TEST_INIT();
  
  f1 = ezxml_parse_str(document, sizeof(document));

  for (team = ezxml_child(f1, "team"); team; team = team->next) {
    teamname = ezxml_attr(team, "name");
    for (i=0,driver = ezxml_child(team, "driver"); 
         driver; 
         driver = driver->next,i++) {
      if (strcmp(results[i].driver, ezxml_child(driver, "name")->txt) ||
          strcmp(results[i].team, teamname) ||
          strcmp(results[i].points, ezxml_child(driver, "points")->txt))
        CYG_TEST_FAIL("Parser failed first test");
    }
  }
  
  if (strcmp(ezxml_get(f1, "team", 0, "driver", 1, "name", -1)->txt,
              "David Coultard")) 
    CYG_TEST_FAIL_FINISH("Parser failed second test");
  else
    CYG_TEST_PASS_FINISH("Parser passed");
  
  ezxml_free(f1);
}
