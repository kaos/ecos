//==========================================================================
//
//      profile.c
//
//      Application profiling support
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003 eCosCentric Ltd.
// Copyright (C) 2002 Gary Thomas
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
// Author(s):    Gary Thomas
// Contributors: Bart Veer
// Date:         2002-11-14
// Purpose:      Application profiling support
// Description:  
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <pkgconf/system.h>
#include <pkgconf/profile_gprof.h>

#include <stdlib.h>
#include <string.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/profile/profile.h>
#include <cyg/profile/gmon_out.h>

#ifdef CYGPKG_PROFILE_TFTP
# include <network.h>
# include <tftp_support.h>
#endif

// ----------------------------------------------------------------------------
// A gmon.out file starts with a struct gmon_hdr containing a cookie
// "gmon", a format version number, and some spare bytes. The structure
// is initialized by the profile_on() entry point so that it does not
// get garbage collected by the collector and hence a gdb script can
// always access it.
static struct gmon_hdr  profile_gmon_hdr;

// The header is followed by data blocks. Each data block consists of a
// one-byte tag (HIST, ARC, or BB_COUNT), followed by data in a specific
// format.
static unsigned char    profile_tags[3];

// The profiling data always contains histogram data. Typically an
// extra hardware timer is made to interrupt at the desired rate
// and stores the interrupted pc.
static struct gmon_hist_hdr    profile_hist_hdr;

// The actual histogram counts. The file format only allows for 16-bit
// counts, which means overflow is a real possibility.
static cyg_uint16*      profile_hist_data;

// Each slot in the histogram data covers a range of pc addresses,
// allowing a trade off between memory requirements and precision.
static int              bucket_shift;

// Profiling is disabled on start-up and while a tftp transfer takes place.
static int              profile_enabled;

// This is used by the gdb script to reset the profile data.
static int              profile_reset_pending;

// The callgraph data. There is no header for this. Instead each non-zero
// entry is output separately, prefixed by an ARC tag. The data is accessed
// via a hash table/linked list combination. The tag is part of the
// structure to reduce the number of I/O operations needed for writing
// gmon.out.
struct profile_arc {
    cyg_uint32                  next;
    unsigned char               tags[4];
    struct gmon_cg_arc_record   record;
};

static struct profile_arc*  profile_arc_records;

// The next free slot in the arc_records table.
static int                  profile_arc_next    = 1;

#ifdef CYGPKG_PROFILE_CALLGRAPH
// The callgraph is accessed via a hash table. The hashing function is
// trivial, it just involves shifting an address an appropriate number
// of places.
static int*         profile_arc_hashtable;

// The sizes of these tables
static int          profile_arc_hash_count;
static int          profile_arc_records_count;

// Is the hashtable too small? Used for diagnostics.
static int          profile_arc_overflow;
#endif

// Reset current profiling data.
static void
profile_reset(void)
{
    memset(profile_hist_data, 0, profile_hist_hdr.hist_size * sizeof(cyg_uint16));

#ifdef CYGPKG_PROFILE_CALLGRAPH
    // Zeroing the callgraph can be achieved by zeroing the hash
    // table and resetting the next field used for indexing into
    // the arc data itself. Whenever an arc data slot is allocated
    // the count and addresses are reset.
    memset(profile_arc_hashtable, 0, profile_arc_hash_count * sizeof(int));
    profile_arc_next     = 1;
    profile_arc_overflow = 0;
#endif
}

// ----------------------------------------------------------------------------
// Accumulate profiling data.

// __profile_hit() will be called by HAL-specific code, typically in an ISR
// associated with a timer.

void
__profile_hit(CYG_ADDRWORD pc)
{
    int bucket;
    if (! profile_enabled ) {
        if (! profile_reset_pending) {
            return;
        }
        // reset_pending can be set by the gdb script to request resetting
        // the data. It avoids having to do lots of memory updates via the
        // gdb protocol, which is too slow.
        profile_reset_pending   = 0;
        profile_reset();
        profile_enabled         = 1;
    }
    
    if ((pc >= (CYG_ADDRWORD)profile_hist_hdr.low_pc) && (pc <= (CYG_ADDRWORD)profile_hist_hdr.high_pc)) {
        bucket = (pc - (CYG_ADDRWORD)profile_hist_hdr.low_pc) >> bucket_shift;
        if (profile_hist_data[bucket] < (unsigned short)0xFFFF) {
            profile_hist_data[bucket]++;
        }
    }
}

#ifdef CYGPKG_PROFILE_CALLGRAPH
// __profile_mcount() will be called by the HAL-specific mcount() routine.
// When code is compiled with -pg the compiler inserts calls to mcount()
// at the start of each function. Typically mcount() will not use standard
// calling conventions so it has to be provided by the HAL.
//
// The from_pc/to_pc data should end up in profile_arc_records. A hash table
// maps a PC into a list chained through the records array. The hash function
// is a simple shift, so a range of PC addresses (usually 256 bytes) map
// onto a single linked list of arc records.
//
// We can hash on either the caller_pc, the callee_pc, or some combination.
// The caller PC will typically be in the middle of some function. The
// number of arcs that hash into the same list will depend on the number of
// function calls within a 256-byte region of code, multiplied by the
// number of different functions called at each location. The latter will
// be 1 unless the code uses changing function pointers. The callee pc
// is near the start of a function, and the number of hash collisions will
// depend on the number of places that function is called from. Usually this
// will be small, but some utility functions may be called from many different
// places.
//
// Hashing on the caller PC should give more deterministic results.
//
// On some targets the compiler does additional work. For example on
// the 68K in theory there is no need for a hash table because the
// compiler provides a word with each callee for the head of the
// linked list. It is not easy to cope with that in generic code, so
// for now this code ignores such compiler assistance.
//
// It is assumed that __profile_mcount() will be called with interrupts
// disabled. 

void
__profile_mcount(CYG_ADDRWORD caller_pc, CYG_ADDRWORD callee_pc)
{
    int                 hash_index;
    struct profile_arc* current;

    // mcount() may be called at any time, even before profile_arc_records
    // is enabled. There is an assumption here that .bss has been zeroed
    // before the first call into C code, i.e. by the initial assembler
    // start-up.
    if (!profile_enabled) {
        if (! profile_reset_pending) {
            return;
        }
        profile_reset_pending   = 0;
        profile_reset();
        profile_enabled         = 1;
    }

    // Check the caller_pc because that is what is used to index the
    // hash table. Checking the callee_pc is optional and depends on
    // exactly how you interpret the start and end addresses passed to
    // profile_on().
    if ((caller_pc < (CYG_ADDRWORD)profile_hist_hdr.low_pc) ||
        (caller_pc > (CYG_ADDRWORD)profile_hist_hdr.high_pc)) {
        return;
    }

    hash_index = (int) ((caller_pc - (CYG_ADDRWORD)profile_hist_hdr.low_pc) >> CYGNUM_PROFILE_CALLGRAPH_HASH_SHIFT);
    if (0 == profile_arc_hashtable[hash_index]) {
        if (profile_arc_next == profile_arc_records_count) {
            profile_arc_overflow = 1;
        } else {
            profile_arc_hashtable[hash_index]   = profile_arc_next;
            current = &(profile_arc_records[profile_arc_next]);
            profile_arc_next++;
            current->next   = 0;
            current->record.from_pc = (void*) caller_pc;
            current->record.self_pc = (void*) callee_pc;
            current->record.count   = 1;
        }
    } else {
        current = &(profile_arc_records[profile_arc_hashtable[hash_index]]);
        while (1) {
            if ((current->record.from_pc == (void*) caller_pc) && (current->record.self_pc == (void*) callee_pc)) {
                current->record.count++;
                break;
            } else if (0 == current->next) {
                if (profile_arc_next == profile_arc_records_count) {
                    profile_arc_overflow = 1;
                } else {
                    current->next   = profile_arc_next;
                    current         = &(profile_arc_records[profile_arc_next]);
                    profile_arc_next++;
                    current->next   = 0;
                    current->record.from_pc = (void*) caller_pc;
                    current->record.self_pc = (void*) callee_pc;
                    current->record.count   = 1;
                }
                break;
            } else {
                current = &(profile_arc_records[current->next]);
            }
        }
    }
}
#endif

#ifdef CYGPKG_PROFILE_TFTP
// ----------------------------------------------------------------------------
// TFTP support
//
// To keep things simple this code only supports one open file at a time,
// and only gmon.out is supported.

static int              profile_tftp_next_index     = 0;
static unsigned char*   profile_tftp_current_block  = (unsigned char*) 0;
static int              profile_tftp_current_len    = 0;
static int              profile_tftp_is_open        = 0;

static int
profile_tftp_open(const char *filename, int flags)
{
    // Only allow one open file for now.
    if (profile_tftp_is_open) {
        return -1;
    }
    // Only read-only access is supported.
    if ((0 != (flags & ~O_RDONLY)) || (0 == (flags & O_RDONLY))) {
        return -1;
    }
    // Only gmon.out can be retrieved using this tftp daemon
    if (0 != strcmp(filename, "gmon.out")) {
        return -1;
    }
    // Everything is in order. Prepare for the first read. Profiling
    // is suspended while the tftp transfer is in progress to avoid
    // inconsistent results.
    profile_enabled             = 0;
    profile_tftp_is_open        = 1;
    profile_tftp_next_index     = 0;
    profile_tftp_current_len    = 0;

    // Report any callgraph overflows. This is best done when retrieving
    // the results, either in the gdb script or at tftp open time.
#ifdef CYGPKG_PROFILE_CALLGRAPH
    if (profile_arc_overflow) {
        diag_printf("Profiling: warning, the table of callgraph arcs has overflowed\n");
        diag_printf("This can be avoided by increasing CYGNUM_PROFILE_CALLGRAPH_ARC_PERCENTAGE\n");
    }
#endif
    
    return 1;
}

static int
profile_tftp_close(int fd)
{
    if (! profile_tftp_is_open) {
        return -1;
    }
    profile_tftp_is_open = 0;

    // The histogram counters are only 16 bits, so can easily overflow
    // during a long run. Resetting the counters here makes it possible
    // to examine profile data during different parts of the run with
    // a reduced risk of overflow.
    profile_reset();

    // Profiling was disabled in the open() call
    profile_enabled     = 1;
    return 0;
}

// gmon.out can only be read, not written.
static int 
profile_tftp_write(int fd, const void *buf, int len)
{
    return -1;
}

// The data that should go into gmon.out is spread all over memory.
// This utility is used to move from one block to the next.
static void
profile_tftp_read_next(void)
{
    switch (profile_tftp_next_index) {
      case 0 :      // The current block is the gmon hdr
        profile_tftp_current_block  = (unsigned char*) &profile_gmon_hdr;
        profile_tftp_current_len    = sizeof(struct gmon_hdr);
        break;
      case 1 :      // The histogram tag
        profile_tftp_current_block  = &(profile_tags[0]);
        profile_tftp_current_len    = 1;
        break;
      case 2 :      // The histogram header
        profile_tftp_current_block  = (unsigned char*) &profile_hist_hdr;
        profile_tftp_current_len    = sizeof(struct gmon_hist_hdr);
        break;
      case 3 :      // The histogram data
        profile_tftp_current_block  = (unsigned char*) profile_hist_data;
        profile_tftp_current_len    = profile_hist_hdr.hist_size * sizeof(cyg_uint16);
        break;
      default :     // One of the arc records. These start at array offset 1.
        {
            int arc_index    = profile_tftp_next_index - 3;
            if (arc_index >= profile_arc_next) {
                profile_tftp_current_block  = (unsigned char*) 0;
                profile_tftp_current_len    = 0;
            } else {
                // gmon.out should contain a 1 byte tag followed by each
                // arc record.
                profile_tftp_current_block  = (unsigned char*) &(profile_arc_records[arc_index].tags[3]);
                profile_tftp_current_len    = sizeof(struct gmon_cg_arc_record) + 1;
            }
            break;
        }
    }
    profile_tftp_next_index++;
}
    
// Read the next block of data. There is no seek operation so no need
// to worry about the current position. State from the previous reads
// is held in profile_tftp_current_block and profile_tftp_current_len
static int
profile_tftp_read(int fd, void *buf_arg, int len)
{
    unsigned char*  buf     = (unsigned char*) buf_arg;
    int             read    = 0;
    
    if ( ! profile_tftp_is_open ) {
        return -1;
    }

    while (len > 0) {
        if (0 == profile_tftp_current_len) {
            profile_tftp_read_next();
            if (0 == profile_tftp_current_len) {
                break;
            }
        }
        if (profile_tftp_current_len >= len) {
            // The request can be satisfied by the current block
            memcpy(&(buf[read]), profile_tftp_current_block, len);
            profile_tftp_current_block += len;
            profile_tftp_current_len   -= len;
            read += len;
            break;
        } else {
            memcpy(&(buf[read]), profile_tftp_current_block, profile_tftp_current_len);
            len  -= profile_tftp_current_len;
            read += profile_tftp_current_len;
            profile_tftp_current_len = 0;
        }
    }
    return read;
}

static struct tftpd_fileops profile_tftp_fileops = {
    &profile_tftp_open,
    &profile_tftp_close,
    &profile_tftp_write,
    &profile_tftp_read
};
#endif

// ----------------------------------------------------------------------------
// profile_on() has to be called by application code to start profiling.
// Application code will determine the start and end addresses, usually
// _stext and _etext, but it is possible to limit profiling to only
// some of the code. The bucket size controls how many PC addresses
// will be treated as a single hit: a smaller bucket increases precision
// but requires more memory. The resolution is used to initialize the
// profiling timer: more frequent interrupts means more accurate results
// but increases the risk of an overflow.

void 
profile_on(void *_start, void *_end, int _bucket_size, int resolution)
{
    int             bucket_size;
    cyg_uint32      version     = GMON_VERSION;
    CYG_ADDRWORD    text_size   = (CYG_ADDRWORD)_end - (CYG_ADDRWORD)_start;

    // Initialize statics. This also ensures that they won't be
    // garbage collected by the linker so a gdb script can safely
    // reference them.
    memcpy(profile_gmon_hdr.cookie, GMON_MAGIC, 4);
    memcpy(profile_gmon_hdr.version, &version, 4);
    profile_tags[0] = GMON_TAG_TIME_HIST;
    profile_tags[1] = GMON_TAG_CG_ARC;
    profile_tags[2] = GMON_TAG_BB_COUNT;
    strcpy(profile_hist_hdr.dimen, "seconds");
    profile_hist_hdr.dimen_abbrev   = 's';

    // The actual bucket size. For efficiency this should be a power of 2.
    bucket_size             = 1;
    bucket_shift            = 0;
    while (bucket_size < _bucket_size) {
        bucket_size     <<= 1;
        bucket_shift    += 1;
    }

    // The gprof documentation claims that this should be the size in
    // bytes. The implementation treats it as a count.
    profile_hist_hdr.hist_size  = (cyg_uint32) ((text_size + bucket_size - 1) / bucket_size);
    profile_hist_hdr.low_pc     = _start;
    profile_hist_hdr.high_pc    = (void*)((cyg_uint8*)_end - 1);
    // The prof_rate is the frequency in hz. The resolution argument is
    // an interval in microseconds.
    profile_hist_hdr.prof_rate  = 1000000 / resolution;
        
    // Now allocate a buffer for the histogram data.
    profile_hist_data = (cyg_uint16*) malloc(profile_hist_hdr.hist_size * sizeof(cyg_uint16));
    if ((cyg_uint16*)0 == profile_hist_data) {
        diag_printf("profile_on(): cannot allocate histogram buffer - ignored\n");
        return;
    }
    memset(profile_hist_data, 0, profile_hist_hdr.hist_size * sizeof(cyg_uint16));

#ifdef CYGPKG_PROFILE_CALLGRAPH
    // Two arrays are needed for keeping track of the callgraph. The
    // first is a hash table. The second holds the arc data. The
    // latter array contains an extra 50 slots to cope with degenerate
    // programs (including testcases).
    {
        int i;
        
        profile_arc_hash_count  = (int) ((text_size + (0x01 << CYGNUM_PROFILE_CALLGRAPH_HASH_SHIFT) - 1)
                                         >> CYGNUM_PROFILE_CALLGRAPH_HASH_SHIFT);
        profile_arc_records_count = (int)
            (CYGNUM_PROFILE_CALLGRAPH_ARC_PERCENTAGE * (text_size / 100)) /
            sizeof(struct profile_arc)
            + 50;

        profile_arc_hashtable = (int*) malloc(profile_arc_hash_count * sizeof(int));
        if ((int*)0 == profile_arc_hashtable) {
            diag_printf("profile_on(): cannot allocate call graph hash table\n  call graph profiling disabled\n");
        } else {
            memset(profile_arc_hashtable, 0, profile_arc_hash_count * sizeof(int));
            profile_arc_records = (struct profile_arc*) malloc(profile_arc_records_count * sizeof(struct profile_arc));
            if ((struct profile_arc*)0 == profile_arc_records) {
                diag_printf("profile_on(): cannot allocate call graph arc table\n  call graph profiling disabled\n");
                free(profile_arc_hashtable);
                profile_arc_hashtable = (int*) 0;
            } else {
                memset(profile_arc_records, 0, profile_arc_records_count * sizeof(struct profile_arc));
                for (i = 0; i < profile_arc_records_count; i++) {
                    profile_arc_records[i].tags[3] = GMON_TAG_CG_ARC;
                }
                profile_arc_next    = 1;    // slot 0 cannot be used because 0 marks an unused hash slot.
            }
        }
    }
#else
    profile_arc_records     = (struct profile_arc*) 0;
#endif
    
    diag_printf("Profile from %p..%p in %d buckets of size %d\n",
                profile_hist_hdr.low_pc, profile_hist_hdr.high_pc,
                profile_hist_hdr.hist_size, bucket_size);

    // Activate the profiling timer, which is usually provided by the
    // variant or target HAL. The requested resolution may not be
    // possible on the current hardware, so the HAL is allowed to
    // tweak it.
    resolution = hal_enable_profile_timer(resolution);
    profile_hist_hdr.prof_rate = 1000000 / resolution;

    profile_enabled = 1;

#ifdef CYGPKG_PROFILE_TFTP    
    // Create a TFTP server to provide the data
    (void) tftpd_start(CYGNUM_PROFILE_TFTP_PORT, &profile_tftp_fileops);
#endif    
}

// EOF profile.c
