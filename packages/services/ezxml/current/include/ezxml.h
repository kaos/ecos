//==========================================================================
//
//      ezxml.h
//
//      Simple XML parser
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric Ltd
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Aaron Voisine
// Contributors: Matt Jerdonek 
// Date:         2005-01-31
// Purpose:      
// Description:  
//              
// This code is part of eCos (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

/* ezxml.h
 *
 * Copyright 2004 Aaron Voisine <aaron@voisine.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _EZXML_H
#define _EZXML_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EZXML_BUFSIZE 1024

// returns the next tag of the same name in the same section and depth or NULL
// if not found
#define ezxml_next(xml) xml->next

// returns the tag character content or empty string if none
#define ezxml_txt(xml) xml->txt

typedef struct ezxml *ezxml_t;
struct ezxml {
    char *name;      // tag name
    char **attr;     // tag attributes { name, value, name, value, ... NULL }
    char *txt;       // tag character content, empty string if none
    size_t off;      // tag offset in parent tag character content
    ezxml_t next;    // next tag with same name in this section at this depth
    ezxml_t sibling; // next tag with different name in same section and depth
    ezxml_t ordered; // next tag, same section and depth, in original order
    ezxml_t child;   // head of sub tag list, NULL if none
    ezxml_t parent;  // parent tag, NULL if current tag is root tag
    short flags;     // additional information, only used internally for now
};
    
// Given a string of xml data and its length, parses it and creates an ezxml
// structure. For efficiency, modifies the data by adding null terminators
// and decoding ampersand sequences. If you don't want this, copy the data and
// pass in the copy. Returns NULL on failure.
ezxml_t ezxml_parse_str(char *s, size_t len);

// A wrapper for ezxml_parse_str() that accepts a file descriptor. First
// attempts to mem map the file. Failing that, reads the file into memory.
// Returns NULL on failure.
ezxml_t ezxml_parse_fd(int fd);

// a wrapper for ezxml_parse_fd() that accepts a file name
ezxml_t ezxml_parse_file(const char *file);
    
// Wrapper for ezxml_parse_str() that accepts a file stream. Reads the entire
// stream into memory and then parses it. For xml files, use ezxml_parse_file()
// or ezxml_parse_fd()
ezxml_t ezxml_parse_fp(FILE *fp);
    
// returns the first child tag (one level deeper) with the given name or NULL if
// not found
ezxml_t ezxml_child(ezxml_t xml, const char *name);

// Returns the Nth tag with the same name in the same section at the same depth
// or NULL if not found. An index of 0 returns the tag given.
ezxml_t ezxml_idx(ezxml_t xml, int idx);

// returns the value of the requested tag attribute, or NULL if not found
const char *ezxml_attr(ezxml_t xml, const char *attr);

// Traverses the ezxml sturcture to retrive a specific subtag. Takes a variable
// length list of tag names and indexes. Final index must be -1. Example: 
// title = ezxml_get(library, "shelf", 0, "book", 2, "title", -1);
// This retrieves the title of the 3rd book on the 1st shelf of library.
// Returns NULL if not found.
ezxml_t ezxml_get(ezxml_t xml, ...);

// Converts an ezxml structure back to xml. Returns a string of xml data that
// must be freed.
char *ezxml_toxml(ezxml_t xml);

// returns a NULL terminated array of processing instructions for the given
// target
const char **ezxml_pi(ezxml_t xml, const char *target);

// frees the memory allocated for an ezxml structure
void ezxml_free(ezxml_t xml);
    
// returns parser error message or empty string if none
const char *ezxml_error(ezxml_t xml);

#ifdef __cplusplus
}
#endif

#endif // _EZXML_H
