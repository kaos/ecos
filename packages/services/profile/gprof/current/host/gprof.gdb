# =====================================================================
#
#     gprof.gdb
#
# =====================================================================
######ECOSHOSTGPLCOPYRIGHTBEGIN####
# ----------------------------------------------------------------------------
# Copyright (C) 2003 eCosCentric Ltd.
#
# This file is part of the eCos host tools.
#
# This program is free software; you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation; either version 2 of the License, or (at your option) 
# any later version.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
# more details.
# 
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 
# 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# ----------------------------------------------------------------------------
######ECOSHOSTGPLCOPYRIGHTEND####
# =====================================================================
######DESCRIPTIONBEGIN####
#
# Author(s):	bartv
# Date:		2003-09-01
# Purpose:	extract gprof data from the target
#
#####DESCRIPTIONEND####
# =====================================================================

define gprof_fetch
  if 0 == profile_hist_data
    echo Target-side profiling has not been initialized.\n
  else
    if 0 != profile_reset_pending
      echo No profiling data has accumulated since the last reset.\n
    else
      dump   binary memory gmon.out &profile_gmon_hdr ((char*)&profile_gmon_hdr + sizeof(struct gmon_hdr))
      append binary memory gmon.out &profile_tags[0] &profile_tags[1]
      append binary memory gmon.out &profile_hist_hdr ((char*)&profile_hist_hdr + sizeof(struct gmon_hist_hdr))
      append binary memory gmon.out profile_hist_data (profile_hist_data + profile_hist_hdr.hist_size)
      if 0 != profile_arc_records
        if 0 != profile_arc_overflow
          echo Warning: the table of callgraph arcs has overflowed\n
          echo   This can be avoided by increasing CYGNUM_PROFILE_CALLGRAPH_ARC_PERCENTAGE\n
        end
        # This loop is slow, but there is not much that can be done about it.
        # The gmon.out file should contain 13 bytes per arc record. Keeping
        # all the data in a packed array would involve non-aligned data,
        # which would add significantly to the run-time overheads.
        set $profile_i = 1
        set $profile_count = profile_arc_next
        while $profile_i != $profile_count
          append binary memory gmon.out &(profile_arc_records[$profile_i].tags[3]) &(profile_arc_records[$profile_i + 1])
          set $profile_i = $profile_i + 1
        end
      end
    end
  end
end

define gprof_reset
  if 0 == profile_hist_data
    echo Target-side profiling has not been initialized.\n
  else
    # An initial attempt to implement this by modifying profile_hist_data
    # and profile_arc_hashtable was not satisfactory. gdb does not have
    # commands to clear whole arrays, and doing it a slot at a time proved
    # too slow. An alternative approach is to call a target-side reset
    # function but that only works if the target is in a sensible state.
    # The final implementation is to just set a flag and let the target
    # sort things out.
    set profile_reset_pending = 1
    set profile_enabled = 0
  end
end

define gprof_dump
  if 0 == profile_hist_data
    echo Target-side profiling has not been initialized.\n
  else
    gprof_fetch
    gprof_reset
  end
end

document gprof_fetch
Extract gprof profiling data from the target and write it to a file
gmon.out in the current directory. This macro should only be used
with eCos configurations that contain the profiling package, and
only after the target has called the profile_on() initialization
routine.
end

document gprof_reset
Reset all gprof profiling data in the target. This macro should only
be used with eCos configurations that contain the profiling package,
and only after the target has called the profile_on() initialization
routine.
end

document gprof_dump
Extract gprof profiling data from the target and write it to a file
gmon.out in the current directory. The profiling data is then reset.
This macro should only be used with eCos configurations that contain
the profiling package, and only after the target has called the
profile_on() initialization routine.
end
