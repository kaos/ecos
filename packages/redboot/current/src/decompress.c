//==========================================================================
//
//      decompress.c
//
//      RedBoot decompress support
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov
// Contributors: jskov
// Date:         2001-03-08
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>

#ifdef CYGPKG_COMPRESS_ZLIB
#include <cyg/compress/zlib.h>
static z_stream stream;

static int
gzip_init(_pipe_t* p)
{
    int err;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.next_in = NULL;
    stream.avail_in = 0;
    stream.next_out = NULL;
    stream.avail_out = 0;
    err = inflateInit(&stream);

    return err;
}

static int
gzip_inflate(_pipe_t* p)
{
    int err;

    stream.next_in = p->in_buf;
    stream.avail_in = p->in_avail;
    stream.next_out = p->out_buf;
    stream.avail_out = -1;              // no limits
    err = inflate(&stream, Z_SYNC_FLUSH);
    p->out_size += (stream.next_out - p->out_buf);
    p->out_buf = stream.next_out;
    p->msg = stream.msg;

    return err;
}

static int
gzip_close(_pipe_t* p, int err)
{
    switch (err) {
    case Z_STREAM_END:
        err = 0;
        break;
    case Z_OK:
        // Decompression didn't complete
        p->msg = "premature end of input";
        // fall-through
    default:
        err = -1;
        break;
    }

    inflateEnd(&stream);

    return err;
}

_decompress_fun_init* _dc_init = gzip_init;
_decompress_fun_inflate* _dc_inflate = gzip_inflate;
_decompress_fun_close* _dc_close = gzip_close;
#endif // CYGPKG_COMPRESS_ZLIB
