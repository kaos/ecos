//=================================================================
//
//        ctype.c
//
//        General testcase for C library ctype functions
//
//=================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     ctarpy, jlarmour
// Contributors:  jlarmour
// Date:          1999-03-23
// Description:   Contains general testcode for C library ctype functions
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <stdlib.h>
#include <ctype.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>


// FUNCTIONS

void
cyg_package_start( void )
{
    cyg_iso_c_start();
} // cyg_package_start()


int
main( int argc, char *argv[] )
{
    int c; // character we use as a parameter

    CYG_TEST_INIT();

    CYG_TEST_INFO( "Starting tests from testcase " __FILE__ " for C library "
                   "<ctype.h> functions" );

    // Check isalnum
    c = 't';
    CYG_TEST_PASS_FAIL( isalnum(c), "isalnum('t')");
    c = '2';
    CYG_TEST_PASS_FAIL( isalnum(c), "isalnum('2')");
    c = 2;
    CYG_TEST_PASS_FAIL( !isalnum(c), "!isalnum(2)");
    c = 127;
    CYG_TEST_PASS_FAIL( !isalnum(c), "!isalnum(127)");

    // Check isalpha
    c = 'A';
    CYG_TEST_PASS_FAIL( isalpha(c), "isalpha('A')");
    c = 'a';
    CYG_TEST_PASS_FAIL( isalpha(c), "isalpha('a')");
    c = '2';
    CYG_TEST_PASS_FAIL( !isalpha(c), "!isalpha('2')");
    c = '\n';
    CYG_TEST_PASS_FAIL( !isalpha(c), "!isalpha('newline')");

    // Check iscntrl
    c = 'a';
    CYG_TEST_PASS_FAIL( !iscntrl(c), "!iscntrl('a')");
    c = 7;
    CYG_TEST_PASS_FAIL( iscntrl(c), "iscntrl('7')");
    c = '\n';
    CYG_TEST_PASS_FAIL( iscntrl(c), "iscntrl(newline)");
    c = 0x7F;
    CYG_TEST_PASS_FAIL( iscntrl(c), "iscntrl(0x7F)");

    // Check isdigit
    c = '2';
    CYG_TEST_PASS_FAIL( isdigit(c), "isdigit('2')");
    c = '0';
    CYG_TEST_PASS_FAIL( isdigit(c), "isdigit('0')");
    c = 't';
    CYG_TEST_PASS_FAIL( !isdigit(c), "!isdigit('t')");

    // Check isgraph
    c = ')';
    CYG_TEST_PASS_FAIL( isgraph(c), "isgraph(')')");
    c = '~';
    CYG_TEST_PASS_FAIL( isgraph(c), "isgraph('~')");
    c = '9';
    CYG_TEST_PASS_FAIL( isgraph(c), "isgraph('9')");
    c = 9;
    CYG_TEST_PASS_FAIL( !isgraph(c), "!isgraph(9)");
    c = ' ';
    CYG_TEST_PASS_FAIL( !isgraph(c), "!isgraph(' ')");
    c = '\t';
    CYG_TEST_PASS_FAIL( !isgraph(c), "!isgraph(tab)");
    c = '\n';
    CYG_TEST_PASS_FAIL( !isgraph(c), "!isgraph(newline)");
    c = 0x7F;
    CYG_TEST_PASS_FAIL( !isgraph(c), "!isgraph(DEL)");
    c = 200;
    CYG_TEST_PASS_FAIL( !isgraph(c), "!isgraph(200)");
    c = '\0';
    CYG_TEST_PASS_FAIL( !isgraph(c), "!isgraph(NUL)");

    // Check islower
    c = 'J';
    CYG_TEST_PASS_FAIL( !islower(c), "islower('J')");
    c = 'j';
    CYG_TEST_PASS_FAIL( islower(c), "islower('j')");
    c = '5';
    CYG_TEST_PASS_FAIL( !islower(c), "!islower(5)");

    // Check isprint
    c = ' ';
    CYG_TEST_PASS_FAIL( isprint(c), "isprint(' ')");
    c = 'x';
    CYG_TEST_PASS_FAIL( isprint(c), "isprint('x')");
    c = '\b';
    CYG_TEST_PASS_FAIL( !isprint(c), "!isprint(backspace)");

    // Check ispunct
    c = '.';
    CYG_TEST_PASS_FAIL( ispunct(c), "ispunct('.')");
    c = '#';
    CYG_TEST_PASS_FAIL( ispunct(c), "ispunct('#')");
    c = '@';
    CYG_TEST_PASS_FAIL( ispunct(c), "ispunct('@')");
    c = 'f';
    CYG_TEST_PASS_FAIL( !ispunct(c), "!ispunct('f')");
    c = '7';
    CYG_TEST_PASS_FAIL( !ispunct(c), "!ispunct('7')");
    c = '\n';
    CYG_TEST_PASS_FAIL( !ispunct(c), "!ispunct('newline')");

    // Check isspace
    c = ' ';
    CYG_TEST_PASS_FAIL( isspace(c), "isspace(' ')");
    c = '\t';
    CYG_TEST_PASS_FAIL( isspace(c), "isspace(tab)");
    c = '\r';
    CYG_TEST_PASS_FAIL( isspace(c), "isspace(return)");
    c = '\v';
    CYG_TEST_PASS_FAIL( isspace(c), "isspace(vertical tab)");
    c = '\n';
    CYG_TEST_PASS_FAIL( isspace(c), "isspace(newline)");
    c = 'd';
    CYG_TEST_PASS_FAIL( !isspace(c), "!isspace('d')");
    c = ',';
    CYG_TEST_PASS_FAIL( !isspace(c), "!isspace(',')");

    // Check isupper
    c = 'A';
    CYG_TEST_PASS_FAIL( isupper(c), "isupper('A')");
    c = 'a';
    CYG_TEST_PASS_FAIL( !isupper(c), "!isupper('a')");
    c = '2';
    CYG_TEST_PASS_FAIL( !isupper(c), "!isupper('2')");
    c = '\b';
    CYG_TEST_PASS_FAIL( !isupper(c), "!isupper(backspace)");

    // Check isxdigit
    c = 'f';
    CYG_TEST_PASS_FAIL( isxdigit(c), "isxdigit('f')");
    c = 'D';
    CYG_TEST_PASS_FAIL( isxdigit(c), "isxdigit('D')");
    c = '1';
    CYG_TEST_PASS_FAIL( isxdigit(c), "isxdigit('1')");
    c = '0';
    CYG_TEST_PASS_FAIL( isxdigit(c), "isxdigit('0')");
    c = 'g';
    CYG_TEST_PASS_FAIL( !isxdigit(c), "!isxdigit('g')");
    c = 'x';
    CYG_TEST_PASS_FAIL( !isxdigit(c), "!isxdigit('x')");

    // Check tolower
    c = 'F';
    CYG_TEST_PASS_FAIL( tolower(c) == 'f', "tolower('F')");
    c = 'g';
    CYG_TEST_PASS_FAIL( tolower(c) == 'g', "tolower('g')");
    c = '3';
    CYG_TEST_PASS_FAIL( tolower(c) == '3', "tolower('3')");

    // Check toupper
    c = 'f';
    CYG_TEST_PASS_FAIL( toupper(c) == 'F', "toupper('f')");
    c = 'G';
    CYG_TEST_PASS_FAIL( toupper(c) == 'G', "toupper('G')");
    c = ',';
    CYG_TEST_PASS_FAIL( toupper(c) == ',', "toupper(',')");

    CYG_TEST_FINISH( "Finished tests from testcase " __FILE__ " for C library "
                     "<ctype.h> functions" );
} // main()

// EOF ctype.c
