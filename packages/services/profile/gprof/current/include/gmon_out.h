// Slightly adapted from OpenBSD "/src/gnu/usr.bin/binutils/gprof/gmon_out.h"
// for eCos environment

/*
 * This file specifies the format of gmon.out files.  It should have
 * as few external dependencies as possible as it is going to be
 * included in many different programs.  That is, minimize the
 * number of #include's.
 *
 * A gmon.out file consists of a header (defined by gmon_hdr) followed
 * by a sequence of records.  Each record starts with a one-byte tag
 * identifying the type of records, followed by records specific data.
 */
#ifndef gmon_out_h
#define gmon_out_h

#include <cyg/infra/cyg_type.h>

#define	GMON_MAGIC	"gmon"	/* magic cookie */
#define GMON_VERSION	1	/* version number */

/*
 * Raw header as it appears on file (without padding):
 */
struct gmon_hdr
{
    char cookie[4];
    char version[4];    // a cyg_uint32, target-side endianness
    char spare[3 * 4];
};

/* types of records in this file: */
typedef enum
{
    GMON_TAG_TIME_HIST = 0, GMON_TAG_CG_ARC = 1, GMON_TAG_BB_COUNT = 2
}
GMON_Record_Tag;

/* The histogram tag is followed by this header, and then an array of       */
/* cyg_uint16's for the actual counts.                                      */

struct gmon_hist_hdr
{
    /* host-side gprof adapts to sizeof(void*) and endianness.              */
    /* It is assumed that the compiler does not insert padding around the   */
    /* cyg_uint32's or the char arrays.                                     */
    void*       low_pc;             /* base pc address of sample buffer     */
    void*       high_pc;            /* max pc address of sampled buffer     */
    cyg_uint32  hist_size;          /* size of sample buffer                */
    cyg_uint32  prof_rate;          /* profiling clock rate                 */
    char        dimen[15];			/* phys. dim., usually "seconds"        */
    char        dimen_abbrev;		/* usually 's' for "seconds"            */
};

/* An arc tag is followed by a single arc record. self_pc corresponds to    */
/* the location of an mcount() call, at the start of a function. from_pc    */
/* corresponds to the return address, i.e. where the function was called    */
/* from. count is the number of calls.                                      */

struct gmon_cg_arc_record
{
    void*       from_pc;            /* address within caller's body         */
    void*       self_pc;        	/* address within callee's body         */
    cyg_uint32  count;              /* number of arc traversals             */
};

/* In theory gprof can also process basic block counts, as per the          */
/* compiler's -fprofile-arcs flag. The compiler-generated basic block       */
/* structure should contain a table of addresses and a table of counts,     */
/* and the compiled code updates those counts. Current versions of the      */
/* compiler (~3.2.1) do not output the table of addresses, and without      */
/* that table gprof cannot process the counts. Possibly gprof should read   */
/* in the .bb and .bbg files generated for gcov processing, but that does   */
/* not happen at the moment.                                                */
/*                                                                          */
/* So for now gmon.out does not contain basic block counts and gprof        */
/* operations that depend on it, e.g. --annotated-source, won't work.       */

#endif /* gmon_out_h */
