//==========================================================================
//
//      entable.c
//
//      Convert binary file to C table
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
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-11-25
// Purpose:             Convert binary file to C table
// Description:         This is a simple host-side program that converts a binary
//                      file on it's input to a C format table of unsigned chars
//                      on its output. The single argument gives the name that the
//                      table should be given.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <stdio.h>

int main( int argc, char **argv )
{
    int i;

    if( argc != 2 )
    {
        fprintf( stderr, "usage: %s <table name>\n",argv[0]);
        exit(1);
    }
    
    printf( "unsigned char %s[] = {\n", argv[1]);

    for(i = 0; ; i++)
    {
        int c = getchar();

        if( c == EOF ) break;

        if( (i % 16) == 0 )
            printf("\n\t");

        printf( "0x%02x, ",c);
    }

    printf("\n};\n");

    exit(0);
}

//==========================================================================
// End of entable.c
