//==========================================================================
//
//      lib/tftp_dummy_file.c
//
//      Dummy [in memory] file I/O functions
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
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-04-06
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// TFTP file support - minimal "dummy" version

#include <network.h>
#include <tftp_support.h>

static int dummy_open(const char *, int);
static int dummy_close(int);
static int dummy_write(int, const void *, int);
static int dummy_read(int, void *, int);

struct tftpd_fileops dummy_fileops = {
    dummy_open, dummy_close, dummy_write, dummy_read
};

struct _file_info;
struct _file {
    unsigned char      *pos, *eof;
    int                 flags;
    int                 mode;
    struct _file_info  *file;
};
#define FILE_OPEN 0x0001

#define NUM_FILES 8
static struct _file files[NUM_FILES];

struct _file_info {
    char          *name;
    unsigned char *data;
    int            length, size;
};

// TEMP
static char _uu_data[] = "This is a test
Four score and seven years ago,
our forefathers brought forth a new nation,
conceived in liberty and dedicated to the
proposition that all men are created equal.
Now we are engaged in a great civil war, testing
whether that nation, or any nation so conceived
and so dedicated, can long endure.
1111111111111111111111111111111111
2222222222222222222222222222222222
3333333333333333333333333333333333
4444444444444444444444444444444444
5555555555555555555555555555555555
6666666666666666666666666666666666
";

static char _f0_data[1024*1024];
static char _f1_data[1024*1024];
static char _f2_data[1024*1024];

static char _name0[256] = "", _name1[256] = "", _name2[256] = "";

static struct _file_info file_list[] = {
    { "uu", _uu_data, sizeof(_uu_data)-1, sizeof(_uu_data)-1},
    { _name0, _f0_data, 0, sizeof(_f0_data)}, // Empty file
    { _name1, _f1_data, 0, sizeof(_f1_data)}, // Empty file
    { _name2, _f2_data, 0, sizeof(_f2_data)}, // Empty file
    { 0, 0, 0}  // End of list
};

static inline struct _file *
dummy_fp(int fd)
{
    struct _file *fp;
    if ((fd < 0) || (fd >= NUM_FILES)) return (struct _file *)0;
    fp = &files[fd];
    if (!(fp->flags & FILE_OPEN)) return (struct _file *)0;
    return fp;
}

static int
dummy_open(const char *fn, int flags)
{
    int res = -1;
    int fd;
    struct _file *fp;
    struct _file_info *fi;

    fp = files;
    for (fd = 0;  fd < NUM_FILES;  fd++, fp++) {
        if (!(fp->flags & FILE_OPEN)) break;
    }
    if (fd == NUM_FILES) {
        return -1;  // No free files
    }
    if (flags & O_RDONLY) {
        // Search for an extant file
        fi = file_list;
        while (fi->name) {
            if (strcmp(fi->name, fn) == 0) {
                // Found it!
                fp->pos = fi->data;
                fp->eof = fi->data + fi->length;
                fp->flags = FILE_OPEN;
                fp->mode = flags;
                return fd;
            }
            fi++;
        }
    } else {
        // Search for a non-existant file
        fi = file_list;
        while (fi->name) {
            if (fi->name[0] == '\0' || strcmp(fi->name, fn) == 0) {
                if ( !fi->name[0] )
                    // Empty slot found
                    strcpy(fi->name, fn);
                fp->pos = fi->data;
                fp->eof = fi->data + fi->size;
                fp->file = fi;  // So we can update file info later
                fp->mode = flags;
                fp->flags = FILE_OPEN;
                return fd;
            }
            fi++;
        }
    }
    // No such file
    return res;
}

static int
dummy_close(int fd)
{
    struct _file *fp = dummy_fp(fd);
    if (!fp) return -1;
    if (fp->mode & O_WRONLY) {
        // Clean up - update file info for later
        fp->file->length = fp->pos - fp->file->data;
    }
    fp->flags = 0;  // No longer open
    return 0;
}

static int 
dummy_write(int fd, const void *buf, int len)
{
    struct _file *fp = dummy_fp(fd);
    int res;
    if (!fp) return -1;
    res = fp->eof - fp->pos;  // Space left in file
    if (res <= 0) return 0;  // End of file
    if (res > len) res = len;
    bcopy(buf, fp->pos, res);
    fp->pos += res;
    return res;
}

static int
dummy_read(int fd, void *buf, int len)
{
    struct _file *fp = dummy_fp(fd);
    int res;
    if (!fp) return -1;
    res = fp->eof - fp->pos;  // Number of bytes left in "file"
    if (res > len) res = len;
    if (res <= 0) return 0;  // End of file
    bcopy(fp->pos, buf, res);
    fp->pos += res;
    return res;
}


// EOF tftp_dummy_file.c
