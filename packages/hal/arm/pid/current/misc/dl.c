//==========================================================================
//
//        dl.c
//
//        ARM PID7 eval board FLASH program tool
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          1998-11-18
// Description:   Tool used for simple handshake downloads.
//####DESCRIPTIONEND####

#include <stdio.h>

#define SYNC_COUNT 63

int
main(int argc, char *argv[])
{
    int c, count, j;
    char cout, cin;
    FILE *in, *log;
    if ((in = fopen(argv[1], "r")) == (FILE *)NULL) {
        fprintf(stderr, "Can't open '%s'\n", argv[1]);
        exit(1);
    }
    if ((log = fopen("/tmp/dl.log", "w")) == (FILE *)NULL) {
        fprintf(stderr, "Can't open log file\n");
        exit(1);
    }
    fprintf(stderr, "Downloading '%s'\n", argv[1]);
    count = 0; j = 0;
    write(1, ">", 1);  // Magic start
    while ((c = fgetc(in)) != EOF) {
        cout = c;
        write(1, &cout, 1);
        if (++j == SYNC_COUNT) {
            read(0, &cin, 1);
            if (cin != cout) {
                fprintf(stderr, "Sync problem - in: %x, out: %x, byte: %x\n", cin, cout, count);
                fprintf(log, "Sync problem - in: %x, out: %x, byte: %x\n", cin, cout, count);
                fflush(log);
                break;
            }
            j = 0;
        }
        count++;
        if ((count % 256) == 255) fprintf(stderr, "%08X\n", count);
    }
    sleep(2);
    write(1, ":", 1);
    fclose(log);
    exit(0);
}
