# {{{  Banner                           

#===============================================================================
#
#    pkgconf.tcl
#
#    A temporary build system for target-side software.
#
#===============================================================================
#####COPYRIGHTBEGIN####
#
# -------------------------------------------
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
#===============================================================================
######DESCRIPTIONBEGIN####
#
# Author(s):	bartv
# Contributors:	bartv
# Date:		1998-04-10
# Purpose:      To generate a build tree (aka a configuration directory)
#               and fill in basic configurability information.
# Description:
# Usage:
#
#####DESCRIPTIONEND####
#===============================================================================
#

# }}}

# {{{  Version check                    

# ----------------------------------------------------------------------------
# pkgconf.tcl requires at least version 8.0 of Tcl, since it makes use of
# namespaces. It is possible that some users still have older versions.

if { [info tclversion] < 8.0 } {
    puts "You are running Tcl version [info tclversion], patchlevel [info patchlevel]"
    puts "The pkgconf.tcl script requires version 8.0 or later."
    puts "Tcl is open source software and can be readily obtained via the internet."
    puts "Good starting places for more information include www.tclconsortium.org"
    puts "and www.scriptics.com"
    exit 0
}

# }}}
# {{{  Namespace definition             

# ----------------------------------------------------------------------------
# Namespaces. All code and variables in this script are kept in the namespace
# "pkgconf". This is not really necessary for stand-alone operation, but if it
# ever becomes desirable to embed this script in a larger application then
# using a namespace is a lot easier.
#
# As a fringe benefit, all global variables can be declared inside this
# namespace and initialised.
#

namespace eval pkgconf {

    # Is this program running on a real operating system or under Windows ?
    variable windows_host [expr {$tcl_platform(platform) == "windows"}]

    # Is this script running in command-line or GUI mode ? This is determined
    # in large part by the command used to invoke the script.
    variable gui_mode 0

    # Keep track of whether or not GUI mode is possible at all. If
    # TK is not loaded into the current interpreter then only
    # command-line operation is possible.
    variable gui_possible 0
    
    # Where is the component repository ? The following input sources
    # are available:
    # 1) the command line argument --srcdir. This requires
    #    the arguments to be parsed early on.
    # 2) the environment variable PKGCONF_COMPONENT_REPOSITORY.
    # 3) $argv0 should correspond to the location of the pkgconf.tcl
    #    script.
    #
    variable component_repository ""
    if { [info exists ::env(PKGCONF_COMPONENT_REPOSITORY)] } {
	set component_repository $::env(PKGCONF_COMPONENT_REPOSITORY)
    } else {
	# Care has to be taken not to end up with a spurious /. at the end,
	# since that would confuse later checks of component_repository != build_tree
	set component_repository [pwd]
	if { [file dirname $argv0] != "." } {
	    set component_repository [file join $component_repository [file dirname $argv0]]
	}
    }

    # Where is the build tree ? This can be specified using the command
    # line argument --builddir, the environment variable PKGCONF_BUILD_TREE,
    # or it defaults to the current directory (in accordance with the
    # normal behaviour of configure scripts).
    variable build_tree ""
    if { [info exists ::env(PKGCONF_BUILD_TREE)] } {
	set build_tree $::env(PKGCONF_BUILD_TREE)
    } else {
	set build_tree [pwd]
    }

    # Details of the command line arguments, if any.
    variable list_targets_arg   0;      # --targets
    variable list_packages_arg  0;      # --packages
    variable list_pkgdata_arg   0;      # --pkgdata
    variable list_flags_arg     0;      # --flags
    variable no_windows_arg     0;      # --nw
    variable target_arg         "";     # --target=abc
    variable platform_arg       "";     # --platform=sim
    variable startup_arg        "";     # --platform=rom
    variable prefix_arg         "";     # --prefix=/tmp/install
    variable disable_args       "";     # --disable-uitron
                                        # This is a list of all disabled packages.
    variable enable_args        "";     # --enable-uitron, as above.
    array set version_args {};          # --kernel_version=v0.3
                                        # This is an array indexed by package name.
    array set cflags_args {};           # DBGFLAGS="-Wall -Werror"
                                        # This is an array indexed by flag name.
    variable force_arg 0;               # Forcibly overwrite configuration files.
    variable defaults_arg 0;            # Do not use the values from the save file.
    variable use_links_arg 0;           # Make use of symbolic links.
    variable use_copies_arg 0;          # The inverse.
    variable debug_arg 0;               # --debug, control infrastructure options
    variable nodebug_arg 0;             # --nodebug, ditto
    
    # Details of all known packages. These come out of the packages file.
    # The variable known_packages is a simple list, and the rest of the
    # data is held in an array package_data().
    variable known_packages ""
    array set package_data {};

    # Details of all known targets, as supplied by the targets file.
    # The variable known_targets is a simple list, and the rest of the
    # data is held in an array target_data.
    variable known_targets ""
    array set target_data {};

    # Details of all the compiler flags that can be used within the
    # build system. 
    variable known_compiler_flags {
	ARCHFLAGS  CARCHFLAGS  CXXARCHFLAGS  LDARCHFLAGS
	ERRFLAGS   CERRFLAGS   CXXERRFLAGS   LDERRFLAGS
	LANGFLAGS  CLANGFLAGS  CXXLANGFLAGS  LDLANGFLAGS
	DBGFLAGS   CDBGFLAGS   CXXDBGFLAGS   LDDBGFLAGS
	EXTRAFLAGS CEXTRAFLAGS CXXEXTRAFLAGS LDEXTRAFLAGS
    }

    # Details of the current configuration values. It is convenient
    # to store these in an array as well. The array gets filled in by
    # pkgconf::setup_defaults
    array set config_data {};

    # Contents of the current save file, if any.
    array set saved_data {};

    # These variables are used by the code that creates the build tree
    variable inc_dirs;                  # For each package with exported header
                                        # files, the path to those header files
                                        # relative to either the component repository
                                        # or the build tree.
    variable misc_dirs;                 # Ditto for package misc directories.
    variable tests_dirs;                # Ditto for package tests directories.
    variable src_dirs;                  # Ditto for src directories.

    variable install_inc_dirs;          # Names of all the directories that should
                                        # exist below install/include, including pkgconf.
    variable install_inc_files;         # Ditto for all the files, but excluding
                                        # ones in pkgconf.
    variable pkgconf_files;             # Names of all the configuration headers
    variable install_test_dirs;         # What directories should be created in install/tests
    
    # What routines should be invoked for outputting fatal errors and
    # for warning messages ?
    variable fatal_error_handler pkgconf::cli_fatal_error
    variable warning_handler     pkgconf::cli_warning
    variable report_handler      pkgconf::cli_report
}

# }}}
# {{{  Infrastructure                   

# ----------------------------------------------------------------------------
# Minimal infrastructure support.
#
# There must be some way of reporting fatal errors, of outputting warnings,
# and of generating report messages. The implementation of these things
# obviously depends on whether or not TK is present. In addition, if this
# script is being run inside a larger application then that larger
# application must be able to install its own versions of the routines.
#
# Once it is possible to report fatal errors, an assertion facility becomes
# feasible.
#

#
# These routines output fatal errors, warnings or miscellaneous messages.
# Their implementations depend on the mode in which this script is operating.
#
proc pkgconf::fatal_error { msg } {
    $pkgconf::fatal_error_handler "$msg"
}

proc pkgconf::warning { msg } {
    $pkgconf::warning_handler "$msg"
}

proc pkgconf::report { msg } {
    $pkgconf::report_handler "$msg"
}

#
# Command line versions.
# NOTE: some formatting so that there are linebreaks at ~72 columns would be
# a good idea.
#
proc pkgconf::cli_fatal_error_handler { msg } {
    puts "pkgconf fatal error: $msg"
    exit 1
}

proc pkgconf::cli_warning_handler { msg } {
    puts "pkgconf warning: $msg"
}

proc pkgconf::cli_report_handler { msg } {
    puts "$msg"
}

#
# Determine the default destination for warnings and for fatal errors.
# In GUI mode message boxes should be used, otherwise the text should just
# go to stdout (stderr is more dubious).
#
# This routine gets invoked before any command line arguments have
# been processed, to set up a sensible default. It gets invoked again
# afterwards, since there may have been a change between GUI and CLI
# mode.
#
# After the first call to this function it is possible to use assertions.
#
proc pkgconf::initialise_error_handling { } {

    if { $pkgconf::gui_mode } {
	set pkgconf::fatal_error_handler pkgconf::gui_fatal_error_handler
	set pkgconf::warning_handler     pkgconf::gui_warning_handler
	set pkgconf::report_handler      pkgconf::gui_report_handler
    } else {
	set pkgconf::fatal_error_handler pkgconf::cli_fatal_error_handler
	set pkgconf::warning_handler     pkgconf::cli_warning_handler
	set pkgconf::report_handler      pkgconf::cli_report_handler
    }
}

#
# These routines can be used by containing programs to provide their
# own error handling.
#
proc pkgconf::set_fatal_error_handler { fn } {
    ASSERT { $fn != "" }
    set pkgconf::fatal_error_handler $fn
}

proc pkgconf::set_warning_handler { fn } {
    ASSERT { $fn != "" }
    set pkgconf::warning_handler $fn
}

proc pkgconf::set_report_handler { fn } {
    ASSERT { $fn != "" }
    set pkgconf::report_handler $fn
}

#
# A very simple assertion facility. It takes a single argument, an expression
# that should be evaluated in the calling function's scope, and on failure it
# should generate a fatal error.
#
proc pkgconf::ASSERT { condition } {

    set result [uplevel 1 [list expr $condition]]
    
    if {  $result == 0 } {
	fatal_error "Assertion failure.\nPredicate \"$condition\"\nThe failure occurred in \"[info level -1]\""
    }
}

# }}}
# {{{  Utilities                        

# ----------------------------------------------------------------------------
# cdl_compare_version. This is a partial implementation of the full
# cdl_compare_version facility defined in the product specification. Its
# purpose is to order the various versions of a given package with
# the most recent version first. As a special case, "current" is
# always considered the most recent.
#
# There are similarities between cdl_compare_version and with Tcl's
# package vcompare, but cdl_compare_version is more general.
#

proc pkgconf::cdl_compare_version { arg1 arg2 } {

    if { $arg1 == $arg2 } {
	return 0
    }
    if { $arg1 == "current"} {
	return -1
    }
    if { $arg2 == "current" } {
	return 1
    }

    set index1 0
    set index2 0
    set ch1    ""
    set ch2    ""
    set num1   ""
    set num2   ""
    
    while { 1} {

	set ch1 [string index $arg1 $index1]
	set ch2 [string index $arg2 $index2]
	set num1 ""
	set num2 ""

	if { ($ch1 == "") && ($ch2 == "") } {
	    
	    # Both strings have terminated at the same time. There may have
	    # been some spurious leading zeroes in numbers.
	    return 0
	    
	} elseif { $ch1 == "" } {

	    # The first string has ended first. If ch2 is a separator then
	    # arg2 is a derived version, e.g. v0.3.p1 and hence newer. Otherwise ch2
	    # is an experimental version v0.3beta and hence older.
	    if { [string match \[-._\] $ch2] } {
		return 1
	    } else {
		return -1
	    }
	} elseif { $ch2 == "" } {

	    # Equivalent to the above.
	    if { [string match \[-._\] $ch1] } {
		return -1
	    } else {
		return 1
	    }
	}

	# There is still data to be processed.
	# Check for both strings containing numbers at the current index.
	if { ( [string match \[0-9\] $ch1] ) && ( [string match \[0-9\] $ch2] ) } {

	    # Extract the entire numbers from the version string.
	    while { [string match \[0-9\] $ch1] } {
		set  num1 "$num1$ch1"
		incr index1
		set  ch1 [string index $arg1 $index1]
	    }
	    while { [string match \[0-9\] $ch2] } {
		set  num2 "$num2$ch2"
		incr index2
		set ch2 [string index $arg2 $index2]
	    }

	    if { $num1 < $num2 } {
		return 1
	    } elseif { $num1 > $num2 } {
		return -1
	    }
	    continue
	}

	# This is not numerical data. If the two characters are the same then
	# move on.
	if { $ch1 == $ch2 } {
	    incr index1
	    incr index2
	    continue
	}
	
	# Next check if both strings are at a separator. All separators can be
	# used interchangeably.
	if { ( [string match \[-._\] $ch1] ) && ( [string match \[-._\] $ch2] ) } {
	    incr index1
	    incr index2
	    continue
	}

	# There are differences in the characters and they are not interchangeable.
	# Just return a standard string comparison.
	return [string compare $ch1 $ch2]
    }
}

if { 0 } {

    puts "compare current  v0.1     : [pkgconf::cdl_compare_version current v0.1]"
    puts "compare v0.1     current  : [pkgconf::cdl_compare_version v0.1 current]"
    puts "compare current  current  : [pkgconf::cdl_compare_version current current]"
    puts "compare v0.1     v0.1     : [pkgconf::cdl_compare_version v0.1 v0.1]"
    puts "compare v0_1     v0.1     : [pkgconf::cdl_compare_version v0_1 v0.1]"
    puts "compare v0.1     v0_1     : [pkgconf::cdl_compare_version v0.1 v0_1]"
    puts "compare v0_1     v0-1     : [pkgconf::cdl_compare_version v0_1 v0-1]"
    puts "compare v0-1     v0_1     : [pkgconf::cdl_compare_version v0-1 v0_1]"
    puts "compare v0.2     v0.1     : [pkgconf::cdl_compare_version v0.2 v0.1]"
    puts "compare v0.1     v0.2     : [pkgconf::cdl_compare_version v0.1 v0.2]"
    puts "compare v0.10    v0.2     : [pkgconf::cdl_compare_version v0.10 v0.2]"
    puts "compare v0.2     v0.10    : [pkgconf::cdl_compare_version v0.2 v0.10]"
    puts "compare 980410   980412   : [pkgconf::cdl_compare_version 980410 980412]"
    puts "compare 980412   980410   : [pkgconf::cdl_compare_version 980412 980410]"
    puts "compare ss980410 ss980412 : [pkgconf::cdl_compare_version ss980410 ss980412]"
    puts "compare ss980412 ss980410 : [pkgconf::cdl_compare_version ss980412 ss980410]"
    puts "compare v0.1.2   v0.1.3   : [pkgconf::cdl_compare_version v0.1.2 v0.1.3]"
    puts "compare v0.1.3   v0.1.2   : [pkgconf::cdl_compare_version v0.1.3 v0.1.2]"
    puts "compare v1.0     v0.1.3   : [pkgconf::cdl_compare_version v1.0 v0.1.3]"
    puts "compare v0.1.3   v1.0     : [pkgconf::cdl_compare_version v0.1.3 v1.0]"
    puts "compare v1.0beta v1.0     : [pkgconf::cdl_compare_version v1.0beta v1.0]"
    puts "compare v1.0     v1.0beta : [pkgconf::cdl_compare_version v1.0 v1.0beta]"
    puts "compare v1.0.p1  v1.0     : [pkgconf::cdl_compare_version v1.0.p1 v1.0]"
    puts "compare v1.0     v1.0.p1  : [pkgconf::cdl_compare_version v1.0 v1.0.p1]"
}

# }}}
# {{{  Argument parsing                 

# ----------------------------------------------------------------------------
# Parsing the arguments. The following arguments should be accepted for
# backwards compatibility reasons.
#
#     --target=<arch_name>
#     --platform=sim
#     --startup=rom     or   --startup=ram
#     --prefix=<directory>
#     --disable-uitron
#     --disable-libc
#
# There was also a --disable-kernel_c_api, but that is no longer supported.
# For now disabling only applies to entire packages, not to components
# within a package. The argument --release=debug has also been disabled
# because it affects too many things (the makevars file, various #define's,
# etc.).
#
# Also it was possible to use --disable-uitron=yes, but that was
# undocumented and is not worth supporting.
#
# Configure allowed --srcdir=xxx, although it did not need it. It is useful
# to support this, and to add --builddir. --installdir as a prefix alias
# then makes sense.
#
# --help should be supported if present as a single argument.
#
# It is desirable to support additional arguments of the form:
#
#     --enable-uitron           (to undo a disable in the saved data)
#     --kernel-version=current
#     --uitron-version=v0.3
#
# Similarly it is desirable to allow users to control compiler flags, e.g.
#     DBGFLAGS="-Wall -Werror"
#
# --debug can be used to set appropriate debug options in the
# infrastructure. This should probably be --enable-debug, but that
# could cause confusion. There is also a --nodebug option.
#
# A new argument is --nw, to disable the GUI environment. Other new
# arguments are:
#   --targets and --packages to display the appropriate details
#   --force to force files to be copied across from the repository
#   --defaults to cause the program to ignore the values in the save file.
#   --linkhdrs and --copyhdrs to control how header files should be handled.
#
# It is also a good idea to be a bit more flexible about the format
# of the command line arguments, for example -target=xxx should work,
# as should -target xxx
#
# The argv0 argument should be the name of this script. It can be used
# to get at the component repository location. If this script has been
# run incorrectly then currently it will fail: in future it may be
# desirable to check an environment variable instead.
#
# The argv argument is a string containing the rest of the arguments.
# If any of the arguments contain spaces then this argument will be
# surrounded by braces. If any of the arguments contain braces then
# things will break.
#

proc pkgconf::parse_arguments { argv0 argv } {

    # First, check for --help or any of the variants. If this script
    # is running in a larger program then it is assumed that the
    # containing program will not pass --help as an argument.
    if { ( $argv == "--help" ) || ( $argv == "-help" ) ||
         ( $argv == "--H"    ) || ( $argv == "-H" ) } {

	 argument_help
	 exit 0
    }

    if { $argv != "" } {

	# There are arguments. If any of the arguments contained
	# spaces then these arguments will have been surrounded
	# by braces, which is a nuisance. So start by turning the
	# arguments into a numerically indexed array.

	set argc 0
	array set args { }
	foreach arg $argv {
	    set args([incr argc]) $arg
	}

	# Now examine each argument with regular expressions. It is
	# useful to have some variables filled in by the regexp
	# matching.
	set dummy  ""
	set match1 ""
	set match2 ""
	for { set i 1 } { $i <= $argc } { incr i } {

	    # Check for -nw. The regexp allows an optional -, followed by
	    # the text -nw, and nothing else.
	    if { [regexp -- {^-?-nw$} $args($i)] == 1 } {
	        set pkgconf::no_windows_arg 1
		continue
	    }

	    # Check for --packages and the other simple ones.
	    if { [regexp -- {^-?-packages$} $args($i)] == 1 } {
		set pkgconf::list_packages_arg 1
		continue
	    }
	    if { [regexp -- {^-?-pkgdata$} $args($i)] == 1 } {
		set pkgconf::list_pkgdata_arg 1
		continue
	    }
	    if { [regexp -- {^-?-targets$} $args($i)] == 1 } {
		set pkgconf::list_targets_arg 1
		continue
	    }
	    if { [regexp -- {^-?-flags$} $args($i)] == 1 } {
		set pkgconf::list_flags_arg 1
		continue
	    }
	    if { [regexp -- {^-?-force$} $args($i)] == 1 } {
		set pkgconf::force_arg 1
		continue
	    }
	    if { [regexp -- {^-?-defaults$} $args($i)] == 1 } {
		set pkgconf::defaults_arg 1
		continue
	    }
	    if { [regexp -- {^-?-linkhdrs$} $args($i)] == 1 } {
		set pkgconf::use_links_arg 1
		continue
	    }
	    if { [regexp -- {^-?-copyhdrs$} $args($i)] == 1 } {
		set pkgconf::use_copies_arg 1
		continue
	    }
	    if { [regexp -- {^-?-debug$} $args($i)] == 1 } {
		set pkgconf::debug_arg 1
		continue
	    }
	    if { [regexp -- {^-?-nodebug$} $args($i)] == 1 } {
		set pkgconf::nodebug_arg 1
		continue
	    }

	    # Now for the directories. srcdir and builddir will have
	    # been calculated already, but may be overridden here.
	    if { [regexp -- {^-?-src_?dir=?(.*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    set pkgconf::component_repository $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --srcdir"
		    } else {
			set pkgconf::component_repository $args([incr i])
		    }
		}
		continue
	    }
	    if { [regexp -- {^-?-build_?dir=?(.*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    set pkgconf::build_tree $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --builddir"
		    } else {
			set pkgconf::build_tree $args([incr i])
		    }
		}
		continue
	    }
	    
	    # And prefix, before moving on to the more interesting ones.
	    # Prefix is a pathname so spaces are legitimate, as are dots,
	    # directory separators, and drive indicators.
	    #
	    # NOTE: strictly speaking the pattern should be platform specific.
	    if { [regexp -- {^-?-prefix=?(.*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    set pkgconf::prefix_arg $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --prefix"
		    } else {
			set pkgconf::prefix_arg $args([incr i])
		    }
		}
		continue
	    }

	    # --installdir is a useful alias for prefix.
	    if { [regexp -- {^-?-install_?dir=?(.*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    set pkgconf::prefix_arg $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --installdir"
		    } else {
			set pkgconf::prefix_arg $args([incr i])
		    }
		}
		continue
	    }
	    

	    # Now check for a --target arg. The -target must match exactly,
	    # but there can be one leading hyphen that gets ignored. The
	    # target can be part of the same argument (with an = in between)
	    # or it can be all of the next argument.
	    #
	    # This code will match spuriously with --targetxxx, i.e. the
	    # = is optional. This is harmless.
	    #
	    # The target is not validated at this point, that happens after
	    # reading in the targets file. It is possible to specify
	    # several --target arguments, and only the last one takes
	    # effect.
	    if { [regexp -- {^-?-target=?([_a-zA-Z0-9]*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    set pkgconf::target_arg $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --target"
		    } else {
			set pkgconf::target_arg $args([incr i])
		    }
		}
		continue
	    }

	    # Ditto for --platform. 
	    if { [regexp -- {^-?-platform=?([_a-zA-Z0-9]*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    set pkgconf::platform_arg $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --platform"
		    } else {
			set pkgconf::platform_arg $args([incr i])
		    }
		}
		continue
	    }

	    # And for --startup.
	    if { [regexp -- {^-?-startup=?([_a-zA-Z0-9]*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    set pkgconf::startup_arg $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --startup"
		    } else {
			set pkgconf::startup_arg $args([incr i])
		    }
		}
		continue
	    }

	    # Now for --disable-<package>. The disabled packages are stored
	    # in a simple list.
	    if { [regexp -- {^-?-disable-?=?([_a-zA-Z0-9]*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    lappend pkgconf::disable_args $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --disable"
		    } else {
			lappend pkgconf::disable_args $args([incr i])
		    }
		}
		continue
	    }
	    
	    # The same for --enable-<package>.
	    if { [regexp -- {^-?-enable-?=?([_a-zA-Z0-9]*)$} $args($i) dummy match1] == 1 } {
		if { $match1 != "" } {
		    lappend pkgconf::enable_args $match1
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --enable"
		    } else {
			lappend pkgconf::enable_args $args([incr i])
		    }
		}
		continue
	    }

	    # And for --<package>-version=xxx
	    # These versions are stored in an array indexed by the
	    # package string.
	    if {[regexp -- {^-?-([_a-zA-Z0-9]+)-?version=?([_\.a-zA-Z0-9]*)$} $args($i) dummy match1 match2] == 1} {
		if { $match2 != "" } {
		    set pkgconf::version_args($match1) $match2
		} else {
		    if { $i == $argc } {
			fatal_error "pkgconf: missing argument after --$match1-version"
		    } else {
			set pkgconf::version_args($match1) $args([incr i])
		    }
		}
		continue
	    }

	    # For each of the known compiler flags, check for a definition of
	    # this flag. The flags are stored in an array cflags_args().
	    # Note that an empty string is legal, so that the default flags
	    # from the targets file can be disabled.
	    set its_a_flag 0
	    foreach flag $pkgconf::known_compiler_flags {
		if { [regexp -- "^[set flag]=(.*)\$" $args($i) dummy match1] == 1 } {
		    set pkgconf::cflags_args($flag) $match1
		    set its_a_flag 1
		    break
		}
	    }
	    if { $its_a_flag != 0 } {
		continue
	    }
	    
	    # An unrecognised argument.
	    fatal_error "pkgconf: invalid argument $args($i)"
	}
    } 

    # Under Windows it is desirable to do some checking on any directories that
    # have been provided. Some cygwin pathnames that a user might supply are
    # not acceptable to Tcl.
    if { $pkgconf::windows_host } {
	set pkgconf::component_repository [get_pathname_for_tcl $pkgconf::component_repository]
	set pkgconf::build_tree           [get_pathname_for_tcl $pkgconf::build_tree]
	if { $pkgconf::prefix_arg != "" } {
	    set pkgconf::prefix_arg [get_pathname_for_tcl $pkgconf::prefix_arg]
	}
    }
    
    # Enabling this facilitates testing of the parsing code.
    if { 0 } {
	puts "component repository is    $pkgconf::component_repository"
	puts "build tree is              $pkgconf::build_tree"
	puts "prefix_arg is              $pkgconf::prefix_arg"
	puts "list_targets_arg is        $pkgconf::list_targets_arg"
	puts "list_packages_arg is       $pkgconf::list_packages_arg"
	puts "list_pkgdata_arg is        $pkgconf::list_pkgdata_arg"
	puts "list_flags_arg is          $pkgconf::list_flags_arg"
	puts "no_windows_arg is          $pkgconf::no_windows_arg"
	puts "target_arg is              $pkgconf::target_arg"
	puts "platform_arg is            $pkgconf::platform_arg"
	puts "startup_arg is             $pkgconf::startup_arg"
	puts "disable_args is            $pkgconf::disable_args"
	puts "enable_args is             $pkgconf::enable_args"
	puts "force_arg is               $pkgconf::force_arg"
	puts "defaults_arg is            $pkgconf::defaults_arg"
	puts "use_links_arg is           $pkgconf::use_links_arg"
	puts "use_copies_arg is          $pkgconf::use_copies_arg"
	puts "debug_arg is               $pkgconf::debug_arg"
	puts "nodebug_arg is             $pkgconf::nodebug_arg"
	foreach i [array names pkgconf::version_args] {
	    puts "Package $i needs version $pkgconf::version_args($i)"
	}
	foreach flag [array names pkgconf::cflags_args] {
	    puts "Compiler flag $flag has value $pkgconf::cflags_args($flag)"
	}
	exit 0
    }
    
}

#
# Display help information if the user has typed --help, -H, --H, or -help.
# The help text uses two hyphens for consistency with configure.
# Arguably this should change.
#
# Not all legal command line arguments are listed. Some of the command
# line arguments like --flags are intended for internal use only.
#
# NOTE: if this command is being invoked from a shell script that sets
# the defaults for a particular architecture then the information will not
# be completely accurate. There is no easy solution for this problem.
#
# NOTE: it would be a really good idea to give an example command line.
# Unfortunately this command line must include a target architecture,
# and there is no sensible default for that.
#
proc pkgconf::argument_help { } {

    puts "Usage: pkgconf \[options\]"
    puts "Available options are:"
    puts "  --help               : display this text."
    puts "  --packages           : list all packages."
    puts "  --targets            : list all possible targets."
    puts "  --target=<arch>      : target the specified architecture."
    puts "  --platform=sim       : target the simulator rather than hardware."
    puts "  --startup=rom        : generate code suitable for ROMming."
    puts "  --disable-<pkg>      : disable a given package."
    puts "  --enable-<pkg>       : enable a given package."
    puts "  --<pkg>-version=v0.3 : use a specific version of a package."
    puts "  --defaults           : use default settings instead of the save file."
    puts "  --srcdir=<dir>       : specify location of the component repository."
    puts "  --builddir=<dir>     : specify location of the build directory."
    puts "  --prefix=<dir>       : specify location of the install directory."
}

# }}}
# {{{  Packages file                    

# ----------------------------------------------------------------------------
# The component repository should contain a file "packages" containing details
# of all supported packages. This file is actually an executable Tcl script
# containing instances of the "package" command, which takes three arguments.
# It is necessary to update the variables pkgconf::known_packages and
# pkgconf::package_data.
#

proc pkgconf::read_packages { } {

    ASSERT { $pkgconf::component_repository != "" }
    ASSERT { [array names pkgconf::package_data] == "" }
    ASSERT { $pkgconf::known_packages == "" }

    # A safe interpreter is used to process the packages file.
    # This is somewhat overcautious, but it is also harmless.
    # The following two commands are made accessible to the slave
    # interpreter and are responsible for updating the actual data.
    proc set_package_data { name value } {
	set ::pkgconf::package_data($name) $value
    }
    proc add_known_package { name } {
	lappend ::pkgconf::known_packages $name
    }

    # Create the parser, add the aliased commands, and then define
    # the routines that do the real work.
    set parser [interp create -safe]
    $parser alias set_package_data  pkgconf::set_package_data
    $parser alias add_known_package pkgconf::add_known_package
    
    $parser eval {
	
	set current_package ""
	
	proc package { name body } {
	    
	    add_known_package                   $name
	    set_package_data "$name,alias"      $name
	    set_package_data "$name,dir"        ""
	    set_package_data "$name,versions"   ""
	    set_package_data "$name,default"    0
	    set_package_data "$name,incdir"     ""
	    set_package_data "$name,hardware"   0

	    # Evaluate the body such that the commands know what the current package is.
	    set ::current_package $name
	    eval $body
	    set ::current_package ""
	}

	proc alias { str } {
	    set_package_data "$::current_package,alias" $str
	}

	# The remaining commands are basically clones of alias.
	proc directory { dir } {
	    set_package_data "$::current_package,dir" $dir
	}

	proc default_value { val } {
	    set_package_data "$::current_package,default" $val
	}

	proc include_dir { dir } {
	    set_package_data "$::current_package,incdir" $dir
	}

	proc hardware { } {
	    set_package_data "$::current_package,hardware" 1
	}
    }

    # The parser is ready to evaluate the script. To avoid having to give the
    # safe interpreter file I/O capabilities, the file is actually read in
    # here and then evaluated.
    set filename [file join $pkgconf::component_repository "packages"]
    set status [ catch {
	set fd [open $filename r]
	set script [read $fd]
	close $fd
	$parser eval $script
    } message]

    if { $status != 0 } {
	pkgconf::fatal_error "Unexpected error while processing $filename\n$message"
    }

    # The interpreter and the aliased commands are no longer required.
    rename set_package_data {}
    rename add_known_package {}
    interp delete $parser
	
    # At this stage the packages file has been read in. It is a good idea to
    # check that all of these packages are present and correct, and incidentally
    # figure out which versions are present.
    foreach pkg $pkgconf::known_packages {
	
	set pkgdir [file join $pkgconf::component_repository $pkgconf::package_data($pkg,dir)]
	if { ![file exists $pkgdir] || ![file isdir $pkgdir] } {
	    fatal_error \
"The file [file join $pkgconf::component_repository "packages"] lists a package\
$pkg which should be present in $pkgdir. This package is missing."
	}

	# Each subdirectory should correspond to a release. A utility routine
	# is available for this.
	set pkgconf::package_data($pkg,versions) [locate_subdirs $pkgdir]
	if { $pkgconf::package_data($pkg,versions) == "" } {
	    fatal_error "The package $pkg does not contain any version directories."		    
	}

	# Sort all the versions using a version-aware comparison version
	set pkgconf::package_data($pkg,versions) [
	    lsort -command pkgconf::cdl_compare_version $pkgconf::package_data($pkg,versions)
	]
    }

    # Handle the --packages argument.
    if { $pkgconf::list_packages_arg != 0 } {

	# The user wants to know about all the arguments. Just to be fancy
	# this code does some alignment.
	set name_maxlen 0
	
	foreach pkg $pkgconf::known_packages {
	    if { [string length $pkg] > $name_maxlen} {
		set name_maxlen [string length $pkg]
	    }
	}
	
	puts "Known packages are:"
	foreach pkg $pkgconf::known_packages {
	    puts "[format "  %-*s : " $name_maxlen $pkg]"
	    puts "    Available versions: $pkgconf::package_data($pkg,versions)"
	    puts "    Aliases           : $pkgconf::package_data($pkg,alias)"
	    if { $pkgconf::package_data($pkg,hardware) } {
		puts "    This package is only available on specific hardware."
	    }
	    if { $pkgconf::package_data($pkg,default) } {
		puts "    This package is enabled by default."
	    } else  {
		puts "    This package is disabled by default."
	    }
	}
    }

    # Ditto for the --pkgdata argument. In this case the data is intended for use
    # by other programs, and is not necessarily intelligible to users.
    if { $pkgconf::list_pkgdata_arg != 0 } {
	foreach pkg $pkgconf::known_packages {
	    set dir [file join $pkgconf::package_data($pkg,dir) [lindex $pkgconf::package_data($pkg,versions) 0]]
	    set dir [file nativename $dir]
	    if { $pkgconf::windows_host != 0 } {
		regsub -all {\\} $dir {\\\\} dir
	    }
	    set alias [get_build_alias $pkg]
	    puts "\"$pkg\" \"$alias\" \"$dir\""
	}
    }
}

#
# Given a package name as supplied by the user, return the internal package name.
# This involves searching through the list of aliases.
#
proc pkgconf::find_package { name } {

    foreach pkg $pkgconf::known_packages {

	if { [string toupper $pkg] == [string toupper $name] } {
	    return $pkg
	}

	foreach alias $pkgconf::package_data($pkg,alias) {
	    if { [string toupper $alias] == [string toupper $name] } {
		return $pkg
	    }
	}
    }

    return ""
}

#
# Given a package name, return the first alias string. This is useful for
# output code.
#
proc pkgconf::get_package_alias { name } {

    pkgconf::ASSERT { [lsearch -exact $pkgconf::known_packages $name] != -1 }
    set alias [lindex $pkgconf::package_data($name,alias) 0]
    return $alias
}

#
# Does a given package actually exist ? This copes with aliases
# as well.
proc pkgconf::is_package { name } {

    ASSERT { $name != "" }

    set result 0
    set name [find_package $name]
    if { $name != "" } {
	set result 1
    }
    return $result
}

#
# Is a particular package a hardware-specific package ? This code
# deals with aliases as well
proc pkgconf::is_hardware_package { name } {

    ASSERT { $name != "" }

    set name [find_package $name]
    ASSERT { $name != "" }

    set result 0
    if { $pkgconf::package_data($name,hardware) != 0 } {
	set result 1
    }
    return $result
}

#
# Is a particular version string valid for the given package ?
proc pkgconf::is_valid_version { pkg version } {

    ASSERT { $pkg     != "" }
    ASSERT { $version != "" }

    set pkg [find_package $pkg]
    ASSERT { $pkg != "" }

    set result 0
    set index [lsearch -exact $pkgconf::package_data($pkg,versions) $version]
    if { $index != -1 } {
	set result 1
    }
    return $result
}

# }}}
# {{{  Targets file                     

# ----------------------------------------------------------------------------
# The handling of the targets file is similar to that for the packages file,
# but a targets file is a bit more complicated.
#

proc pkgconf::read_targets { } {
    
    ASSERT { $pkgconf::component_repository != "" }
    ASSERT { [array names pkgconf::target_data] == "" }
    ASSERT { $pkgconf::known_targets == "" }
    ASSERT { $pkgconf::known_compiler_flags != "" }
    
    proc add_known_target { name } {
	lappend ::pkgconf::known_targets $name
	set ::pkgconf::target_data($name,alias)           $name
	set ::pkgconf::target_data($name,prefix)          ""
	set ::pkgconf::target_data($name,known_platforms) ""
	set ::pkgconf::target_data($name,packages)        ""
    }
    proc add_known_platform { arch name } {
	lappend ::pkgconf::target_data($arch,known_platforms) $name
	set ::pkgconf::target_data($arch,$name,alias)         ""
	set ::pkgconf::target_data($arch,$name,startup)       ""
	set ::pkgconf::target_data($arch,$name,packages)      ""
    }
    proc set_target_data { name value } {
	set ::pkgconf::target_data($name) $value
    }

    set parser [interp create -safe]
    interp share {} stdout $parser
    $parser alias set_target_data               pkgconf::set_target_data
    $parser alias add_known_target              pkgconf::add_known_target
    $parser alias add_known_platform            pkgconf::add_known_platform
    $parser eval [list set known_compiler_flags $pkgconf::known_compiler_flags]

    $parser eval {

	set current_target   ""
	set current_platform ""

	proc target { name body } {

	    add_known_target $name
	    
	    # Evaluate the body such that the commands know what the current target is.
	    set ::current_target $name
	    eval $body
	    set ::current_target ""
	}
	
	proc alias { str } {

	    # If processing a platform body, update the platform data. Otherwise update the
	    # target data. There should be only one alias command per body.
	    if { $::current_platform != "" } {
		set_target_data "$::current_target,$::current_platform,alias" $str
	    } else {
		set_target_data "$::current_target,alias" $str
	    }
	}
	
	proc command_prefix { str } {
	    # Allow the command prefix to be overridden on a per-platform basis
	    if { $::current_platform != "" } {
		set_target_data "$::current_target,$::current_platform,prefix" $str
	    } else {
		set_target_data "$::current_target,prefix" $str
	    }
	}

	proc packages { list } {
	    if { $::current_platform != "" } {
		set_target_data "$::current_target,$::current_platform,packages" $list
	    } else {
		set_target_data "$::current_target,packages" $list
	    }
	}
	
	proc hal { dir } {
	    if { $::current_platform != "" } {
		set_target_data "$::current_target,$::current_platform,haldir" $dir
	    } else {
		set_target_data "$::current_target,haldir" $dir
	    }
	}
	
	proc base_platform { str } {
	    if { $::current_platform != "" } {
		set_target_data "$::current_target,$::current_platform,base_platform" $str
	    } else {
		set_target_data "$::current_target,base_platform" $str
	    }
	}
	
	proc platform { name body } {

	    add_known_platform $::current_target $name
	    
	    set ::current_platform $name
	    eval $body
	    set ::current_platform ""
	}
	
	proc startup { types } {
	    set_target_data "$::current_target,$::current_platform,startup" $types
	}

	# The cflags command is much more complicated than the others. The argument
	# to cflags should be a list of paired data, e.g. ERRFLAGS -Wall.
	# The first entry in each pair should be one of the known compiler
	# flags, the second one should be its value.
	proc cflags { str } {

	    set arg_count [llength $str]
	    if { ($arg_count % 2) != 0 } {
		error "Invalid argument list to cflags command in $current_target"
	    }
	    for { set i 0 } { $i < $arg_count } { incr i 2 } {
		set flag [lindex $str $i]
		set value [lindex $str [expr $i + 1]]
		if { [lsearch -exact $::known_compiler_flags $flag] == -1 } {
		    error "Invalid compiler flag $flag for target $::current_target"
		}
		if { $::current_platform != "" } {
		    set_target_data "$::current_target,$::current_platform,cflags,$flag" $value
		} else {
		    set_target_data "$::current_target,cflags,$flag" $value
		}
	    }
	}
	
    }
    
    # The parser is ready to evaluate the script. To avoid having to give the
    # safe interpreter file I/O capabilities, the file is actually read in
    # here and then evaluated.
    set filename [file join $pkgconf::component_repository "targets"]
    set status [ catch {
	set fd [open $filename r]
	set script [read $fd]
	close $fd
	$parser eval $script
    } message]

    if { $status != 0 } {
	puts "Unexpected error while processing $filename\n$message"
	exit 1
    }

    # The interpreter and the aliased commands are no longer required.
    rename add_known_target   {}
    rename add_known_platform {}
    rename set_target_data    {}
    interp delete $parser

    # The targets file has been read in, so it is time for some consistency checks.
    foreach target $pkgconf::known_targets {

	# If this target involves some special packages, check that these
	# packages were actually defined in the packages file. Also check
	# that these packages are listed as hardware packages so that
	# they will not be enabled by default.
	ASSERT { [info exists pkgconf::target_data($target,packages)] }
	foreach pkg $pkgconf::target_data($target,packages) {

	    if { [is_package $pkg] == 0 } {
		fatal_error \
"The targets file $filename lists a package $pkg\
for the $target architecture. This package is not listed\
in the packages file."

            } elseif { [is_hardware_package $pkg] == 0 } {

	        fatal_error \
"The targets file $filename lists a package $pkg\
for the $target architecture. According to the packages\
file this package is not hardware-specific."
	    }
	}
	
	ASSERT { [info exists pkgconf::target_data($target,known_platforms)] }
	foreach platform $pkgconf::target_data($target,known_platforms) {

	    ASSERT { [info exists pkgconf::target_data($target,$platform,packages)] }
	    foreach pkg $pkgconf::target_data($target,packages) {

		if { [is_package $pkg] == 0 } {
		    
		    fatal_error \
"The target file $filename lists a package $pkg\
for the $target architecture and the $platform platform.\
This package is not listed in the packages file."

                } elseif { [is_hardware_package $pkg] == 0 } {

			fatal_error \
"The target file $filename lists a package $pkg\
for the $target architecture and the $platform platform.\
According to the packages file this package is not hardware-specific."
		}
	    }
	}

	# Also check that all required data for all targets and platforms has
	# been supplied. A command prefix is required, as is a startup list.

	if { [llength $pkgconf::target_data($target,known_platforms)] == 0 } {
	    warning "The targets file $filename lists the $target architecture but no platforms."
	    set index [lsearch -exact $pkgconf::known_targets $target]
	    ASSERT { $index != -1 }
	    set pkgconf::known_targets [lreplace $pkgconf::known_targets $index $index]
	    continue
	}

	if { $pkgconf::target_data($target,prefix) == "" } {
	    fatal_error "The targets file $filename does not list a command prefix for the $target architecture."
	}

	foreach platform $pkgconf::target_data($target,known_platforms) {
	    if { [llength $pkgconf::target_data($target,$platform,startup)] == 0 } {
		fatal_error \
"The targets file $filename does not list any startup options for target $target, platform $platform."
	    }
	}
    }

    # At this stage there may be no targets left over.
    if { [llength $pkgconf::known_targets] == 0 } {
	fatal_error "The targets file $filename does not list any usable targets."
    }
    
    # All the directories exist. Did the user ask for a list of all targets ?
    if { $pkgconf::list_targets_arg } {

	# The user wants to know about all the targets. Just to be fancy
	# this code does some alignment.
	set name_maxlen 0
	
	foreach target $pkgconf::known_targets {
	    if { [string length $target] > $name_maxlen} {
		set name_maxlen [string length $target]
	    }
	}
	
	puts "Known targets:"
	foreach target $pkgconf::known_targets {
	    
	    puts "[format "  %-*s : aliases $pkgconf::target_data($target,alias)" $name_maxlen $target]"
	    
	    set packages $pkgconf::target_data($target,packages)
	    if { [llength $packages] > 0 } {
		puts "    Required packages"
		foreach package $packages {
		    puts "        [pkgconf::get_package_alias $package] ($package)"
		}
	    }
	    
	    foreach platform $pkgconf::target_data($target,known_platforms) {
		puts "    Platform $platform, supported startups $pkgconf::target_data($target,$platform,startup)"
		set packages $pkgconf::target_data($target,$platform,packages)
		if { [llength $packages] > 0 } {
		    puts "      Required packages"
		    foreach package $packages {
			puts "          [pkgconf::get_package_alias $package] ($package)"
		    }
		}
	    }
	}
    }
}

#
# Given a target name as supplied by the user, return the internal target name.
# This involves searching through the list of aliases.
#
proc pkgconf::find_target { name } {

    foreach target $pkgconf::known_targets {

	if { [string toupper $target] == [string toupper $name] } {
	    return $target
	}

	foreach alias $pkgconf::target_data($target,alias) {
	    if { [string toupper $alias] == [string toupper $name] } {
		return $target
	    }
	}
    }

    return ""
}

#
# Ditto but for a given platform name. The target name must be known already.
#

proc pkgconf::find_platform { target name } {

    pkgconf::ASSERT { [lsearch -exact $pkgconf::known_targets $target] != -1 }

    foreach platform $pkgconf::target_data($target,known_platforms) {

	if { [string toupper $platform] == [string toupper $name] } {
	    return $platform
	}

	foreach alias $pkgconf::target_data($target,$platform,alias) {

	    if { [string toupper $alias] == [string toupper $name] } {
		return $platform
	    }
	}
    }
    return ""
}

#
# Given a target name, return the first alias. This is useful for output
# code.
#
proc pkgconf::get_target_alias { name } {

    pkgconf::ASSERT { [lsearch -exact $pkgconf::known_targets $name] != -1 }

    set alias [lindex $pkgconf::target_data($name,alias) 0]
    return $alias
}

#
# Ditto for platforms.
#
proc pkgconf::get_platform_alias { target name } {

    pkgconf::ASSERT { [lsearch -exact $pkgconf::known_targets $target] != -1 }
    pkgconf::ASSERT { [lsearch -exact $pkgconf::target_data($target,known_platforms) $name] != -1 }

    set alias [lindex $pkgconf::target_data($target,$name,alias) 0]
    return $alias
}

#
# For a given target and platform, return an appropriate set of compiler flags.
#
proc pkgconf::get_platform_cflags { target platform flag } {

    ASSERT { [lsearch -exact $pkgconf::known_targets $target] != -1 }
    ASSERT { [lsearch -exact $pkgconf::target_data($target,known_platforms) $platform] != -1 }
    ASSERT { [lsearch -exact $pkgconf::known_compiler_flags $flag] != -1 }

    # A particular flag may be defined specifically for a platform.
    # Or it may come from the target information. Or there may not be
    # a default value.
    if { [info exists pkgconf::target_data($target,$platform,cflags,$flag)] != 0 } {
	return $pkgconf::target_data($target,$platform,cflags,$flag)
    } elseif { [info exists pkgconf::target_data($target,cflags,$flag)] != 0 } {
	return $pkgconf::target_data($target,cflags,$flag)
    } else {
	return ""
    }
}

#
# Is a given target or platform actually valid ? This code copes
# with aliases as well.
proc pkgconf::is_target { name } {

    ASSERT { $name != "" }

    set name [find_target $name]
    set result 0
    if { $name != "" } {
	set result 1
    }
    return $result
}

proc pkgconf::is_platform { target name } {

    ASSERT { $target != "" }
    ASSERT { $name   != "" }

    set target [find_target $target]
    ASSERT { $target != "" }
    
    set name [find_platform $target $name]
    set result 0
    if { $name != "" } {
	set result 1
    }
    return $result
}

proc pkgconf::is_startup { target platform name } {

    ASSERT { $target != "" }
    ASSERT { $name   != "" }

    set target [find_target $target]
    ASSERT { $target != "" }

    set platform [find_platform $target $platform]
    ASSERT { $platform != "" }

    set index [lsearch -exact $pkgconf::target_data($target,$platform,startup) $name]
    set result 0
    if { $index != -1 } {
	set result 1
    }
    return $result
}

#
# Get hold of the default platform and startup for a given target.

proc pkgconf::get_default_platform { target } {

    ASSERT { $target != "" }
    
    set target [find_target $target]
    ASSERT { $target != "" }

    set platform [lindex $pkgconf::target_data($target,known_platforms) 0]
    return $platform
}

proc pkgconf::get_base_platform { target platform } {

    ASSERT { $target   != "" }
    ASSERT { $platform != "" }

    set target [find_target $target]
    ASSERT { $target != "" }
    set platform [find_platform $target $platform]
    ASSERT { $platform != "" }
    
    if [info exists pkgconf::target_data($target,$platform,base_platform) ] {
	set base_platform [lindex $pkgconf::target_data($target,$platform,base_platform) 0]
    } else {
	set base_platform $platform
    }
    return $base_platform
}

proc pkgconf::get_default_startup { target platform } {

    ASSERT { $target   != "" }
    ASSERT { $platform != "" }

    set target [find_target $target]
    ASSERT { $target != "" }
    set platform [find_platform $target $platform]
    ASSERT { $platform != "" }
    
    set startup [lindex $pkgconf::target_data($target,$platform,startup) 0]
    return $startup
}

# }}}
# {{{  Configuration Data               

# {{{  setup_defaults                   

# ----------------------------------------------------------------------------
# The setup_defaults routine is responsible for deciding on default values.
# If there is only one supported architecture then this can be selected,
# and then it is also possible to select a platform and a startup mode.
# All known packages can be enabled by default, and the latest version
# of each package can be selected.
#
# All configuration data is held in the array config_data(). It is
# important to make sure that all fields are initialised.
#

proc pkgconf::setup_defaults { } {

    ASSERT { [llength [array names pkgconf::config_data] ] == 0 }
    ASSERT { $pkgconf::known_packages != "" }
    ASSERT { $pkgconf::known_targets  != "" }
    ASSERT { $pkgconf::build_tree != "" }
    
    set pkgconf::config_data(target)    ""
    set pkgconf::config_data(platform)  ""
    set pkgconf::config_data(base_platform)  ""
    set pkgconf::config_data(startup)   ""
    set pkgconf::config_data(prefix)    ""
    set pkgconf::config_data(use_links) 0

    # Packages fall into a number of different categories.
    #
    # 1) some packages are hardware-specific. They should be enabled
    #    by default if that hardware has been selected, but the user
    #    must still be able to disable them in case they cause problems.
    #
    # 2) some packages are enabled by default, but can be disabled by
    #    the user. There are actually some packages that should never
    #    be disabled, especially infra, but that would complicate things.
    #
    # 3) some packages are disabled by default, but can be enabled by
    #    the user.
    #
    # The easiest way to track this information is to store the target
    # and platform data, which gives us the hardware packages, plus
    # separate lists of which packages are explicitly enabled or disabled.
    # All other information can be derived from that.
    set pkgconf::config_data(enabled_packages)  ""
    set pkgconf::config_data(disabled_packages) ""

    # For each package fill in a default version number.
    foreach pkg $pkgconf::known_packages {
	
	ASSERT { $pkgconf::package_data($pkg,versions) != "" }
	ASSERT { [info exists pkgconf::config_data($pkg,version)] == 0 }
	set pkgconf::config_data($pkg,version) [lindex $pkgconf::package_data($pkg,versions) 0]
    }

    # If there is only one target architecture, default to it. Also
    # default to the first platform for that target and the first
    # startup for that platform.
    if { [llength $pkgconf::known_targets] == 1 } {
 	set pkgconf::config_data(target) [lindex $pkgconf::known_targets 0]

	ASSERT { $pkgconf::target_data($pkgconf::config_data(target),known_platforms) != "" }
	set pkgconf::config_data(platform) [get_default_platform $pkgconf::config_data(target)]
	set pkgconf::config_data(base_platform) [get_base_platform $pkgconf::config_data(target) $pkgconf::config_data(platform)]

	ASSERT { $pkgconf::target_data($pkgconf::config_data(target),$pkgconf::config_data(platform),startup) != "" }
	set pkgconf::config_data(startup) [get_default_startup $pkgconf::config_data(target) $pkgconf::config_data(platform)]
    }

    if { [info exists ::env(PKGCONF_USE_LINKS)] } {
	set pkgconf::config_data(use_links) 1
    }

    if { 0 } {
	foreach i [array names pkgconf::config_data] {
	    puts "pkgconf::config_data($i) : $pkgconf::config_data($i)"
	}
	exit 0
    }
}

# }}}
# {{{  read_save_file                   

# ----------------------------------------------------------------------------
# The build tree may have a save file. The aim here is that
# if the user wants to change some aspects of the configuration, e.g. to
# switch between the eval board and the simulator, it should not be
# necessary to pass all the command line arguments again. Instead the
# system should work out past settings and re-use those where possible.
#
# This routine should be invoked after the defaults are set up but
# before the command line arguments take effect.
#
# There is a command line argument --defaults which stops the saved
# values from taking effect. The save file is still read in because
# it is useful to know what is in the build tree at the moment.
#

proc pkgconf::read_save_file { } {

    ASSERT { $pkgconf::build_tree != "" }
    
    # Get rid of any old settings that might be hanging around.
    foreach i [array names pkgconf::saved_data] {
	unset pkgconf::saved_data($i)
    }

    set filename [file join $pkgconf::build_tree "pkgconf.sav"]
    if { [file exists $filename] && [file isfile $filename] } {
	
	# Read in the saved data. A safe interpreter is used, just in case.
	proc set_saved_data { name value } {
	    set pkgconf::saved_data($name) $value
	}
	set parser [interp create -safe]
	$parser alias set_saved_data pkgconf::set_saved_data

	set status [ catch {
	    set fd [open $filename r]
	    set script [read $fd]
	    close $fd
	    $parser eval $script
	} message]
	if { $status != 0 } {
	    pkgconf::fatal_error "Unexpected error while reading back save file $filename\n$message"
	}

	rename set_saved_data {}
	interp delete $parser
    }

    # It is a good idea to check that the data read back is consistent.
    # It is also a good idea to check some aliases, in case something
    # has been renamed between releases.
    if { [info exists pkgconf::saved_data(target)] } {

	set target_name [find_target $pkgconf::saved_data(target)]
	if { $target_name == "" } {
	    warning "The save file $filename contained an invalid target $pkgconf::saved_data(target). This has been ignored."
	} else {
	    set pkgconf::saved_data(target)    $target_name
	    set pkgconf::config_data(target)   $target_name
	    set pkgconf::config_data(platform) [get_default_platform $pkgconf::config_data(target)]
	    set pkgconf::config_data(base_platform) [get_base_platform $pkgconf::config_data(target) $pkgconf::config_data(platform)]
	    set pkgconf::config_data(startup) \
		    [get_default_startup $pkgconf::config_data(target) $pkgconf::config_data(platform)]
	}
    }

    # There is no point in validating platform or startup unless the target is known.
    if { $pkgconf::config_data(target) != "" } {

	ASSERT { $pkgconf::target_data($pkgconf::config_data(target),known_platforms) != "" }
	
	if { [info exists pkgconf::saved_data(platform)] } {

	    set platform_name [find_platform $pkgconf::config_data(target) $pkgconf::saved_data(platform)]
	    if { $platform_name == "" } {
		warning \
"The save file $filename specified an unknown platform $pkgconf::saved_data(platform).\
The platform $pkgconf::config_data(platform) has been substituted."
	    } else {
		set pkgconf::saved_data(platform)  $platform_name
		set pkgconf::config_data(platform) $platform_name
		set pkgconf::config_data(base_platform) [get_base_platform $pkgconf::config_data(target) $pkgconf::saved_data(platform)]
		set pkgconf::config_data(startup) [get_default_startup $pkgconf::config_data(target) $platform_name]
	    }
	    
	}

	# Currently if there is a target then there is always a platform,
	# possibly the default one.
	ASSERT { $pkgconf::target_data($pkgconf::config_data(target),$pkgconf::config_data(platform),startup) != "" }

	if { [info exists pkgconf::saved_data(startup)] } {

	    if { [is_startup $pkgconf::config_data(target) $pkgconf::config_data(platform) $pkgconf::saved_data(startup)] \
		    == 0 } {
		warning \
"The save file $filename specified an unknown startup $pkgconf::saved_data(startup).\
$pkgconf::config_data(startup) startup has been substituted."
	    } else {
		set pkgconf::config_data(startup) $pkgconf::saved_data(startup)
	    }
	}
    }

    # There should be an entry listing all packages for which there is
    # a version entry. This can be used to update the version numbers
    # which should be used in this configuration.
    if { [info exists pkgconf::saved_data(versioned_packages) ] != 0 } {
	foreach pkg $pkgconf::saved_data(versioned_packages) {
	    
	    # Check that there is a saved version string for this package.
	    # There should be, but it is not worth worrying about.
	    if { [info exists pkgconf::saved_data($pkg,version)] == 0 } {
		continue
	    }
	    set version $pkgconf::saved_data($pkg,version)

	    # Does this package still exist ?
	    if { [is_package $pkg] == 0 } {
		warning \
"The save file $filename specified version $version for package $pkg.\
This package does not exist in the current repository, so the version string\
has been ignored."
                continue
	    }
	    set pkg [find_package $pkg]
	    ASSERT { $pkg != "" }

	    # Is the version valid ?
	    if { [is_valid_version $pkg $version] == 0 } {
		warning \
"The save file $filename specified version $version for package $pkg.\
This version is not present in the current repository so the version\
string has been ignored."
	    } else {
		set pkgconf::config_data($pkg,version) $version
	    }
	}
    }

    # Now check that all the packages listed in the save file are known.
    # If any are missing then they are just discarded. Known packages
    # are used to update the appropriate current configuration data.
    if { [info exists pkgconf::saved_data(enabled_packages)] == 0 } {
	set pkgconf::saved_data(enabled_packages) ""
    }
    if { [info exists pkgconf::saved_data(disabled_packages)] == 0 } {
	set pkgconf::saved_data(disabled_packages) ""
    }

    foreach pkg $pkgconf::saved_data(enabled_packages) {

	if { [is_package $pkg] == 0 } {
	    warning \
"The save file $filename mentioned a package $pkg.\
This package does not exist in the current repository and
has been ignored."
            continue
	}

	set pkg   [find_package $pkg]
	set index [lsearch -exact $pkgconf::config_data(enabled_packages) $pkg]
	if { $index == -1 } {
	    lappend pkgconf::config_data(enabled_packages) $pkg
	}
    }

    foreach pkg $pkgconf::saved_data(disabled_packages) {

	if { [is_package $pkg] == 0 } {
	    warning \
"The save file $filename mentioned a package $pkg.\
This package does not exist in the current repository and
has been ignored."
            continue
	}

	set pkg [find_package $pkg]
	set index [lsearch -exact $pkgconf::config_data(disabled_packages) $pkg]
	if { $index == -1 } {
	    lappend pkgconf::config_data(disabled_packages) $pkg
	}

	# Better make sure that a package is not simultaneously enabled and disabled.
	set index [lsearch -exact $pkgconf::config_data(enabled_packages) $pkg]
	if { $index != -1 } {
	    set pkgconf::config_data(enabled_packages) [lreplace $pkgconf::config_data(enabled_packages) $index $index]
	}
    }

    # Now check for any compiler flags
    foreach flag $pkgconf::known_compiler_flags {
	if { [info exists pkgconf::saved_data(cflags,$flag)] } {
	    set pkgconf::config_data(cflags,$flag) $pkgconf::saved_data(cflags,$flag)
	}
    }
    
    # Update the install directory if appropriate.
    # There is not a lot of checking that can be done here.
    if { [info exists pkgconf::saved_data(prefix) ] } { 
	set pkgconf::config_data(prefix) $pkgconf::saved_data(prefix)
    }

    # And check for the use_links option
    if { [info exists pkgconf::saved_data(use_links)] } {
	set pkgconf::config_data(use_links) $pkgconf::saved_data(use_links)
    }
    
    # Now, possibly undo all of this if the user specified --defaults.
    if { $pkgconf::defaults_arg != 0 } {
	foreach entry [array names pkgconf::config_data] {
	    unset pkgconf::config_data($entry)
	}
	setup_defaults
    }
    
    if { 0 } {
	foreach i [array names pkgconf::config_data] {
	    puts "pkgconf::config_data($i) : $pkgconf::config_data($i)"
	}
	exit 0
    }
}

# }}}
# {{{  write_save_file                  

# ----------------------------------------------------------------------------
# Outputting the save file. This is a relatively simple operation assuming
# no file I/O errors happen. The saved_data array is updated with the new
# contents of the save file. It is not yet clear whether this will be useful,
# but it should not cause any problems either.
#

proc pkgconf::write_save_file { } {

    # Check for minimal consistency. If any variables are missing then
    # this will be handled (poorly) by the catch statement.
    ASSERT { [info exists pkgconf::config_data(target)] }
    ASSERT { $pkgconf::build_tree != "" }

    set savename [file join $pkgconf::build_tree "pkgconf.sav"]
    set open_file ""
    
    set status [catch {

	set open_file [open $savename w]
	output_banner $open_file "pkgconf.sav"

        if { $pkgconf::config_data(target) != "" } {
            puts $open_file "set_saved_data target   $pkgconf::config_data(target)"
	    set pkgconf::saved_data(target) $pkgconf::config_data(target)
	}
	if { $pkgconf::config_data(platform) != "" } {
	    puts $open_file "set_saved_data platform $pkgconf::config_data(platform)"
	    set pkgconf::saved_data(platform) $pkgconf::config_data(platform)
	}
	if { $pkgconf::config_data(base_platform) != "" } {
	    puts $open_file "set_saved_data base_platform $pkgconf::config_data(base_platform)"
	    set pkgconf::saved_data(base_platform) $pkgconf::config_data(base_platform)
	}
	if { $pkgconf::config_data(startup) != "" } {
	    puts $open_file "set_saved_data startup  $pkgconf::config_data(startup)"
	    set pkgconf::saved_data(startup) $pkgconf::config_data(startup)
	}
	if { $pkgconf::config_data(enabled_packages) != "" } {
	    # The use of the list command here avoids problems with multiple
	    # arguments when the file is read back.
	    puts $open_file "set_saved_data enabled_packages [list $pkgconf::config_data(enabled_packages)]"
	    set pkgconf::saved_data(enabled_packages) $pkgconf::config_data(enabled_packages)
	}
	if { $pkgconf::config_data(disabled_packages) != "" } {
	    puts $open_file "set_saved_data disabled_packages [list $pkgconf::config_data(disabled_packages)]"
	    set pkgconf::saved_data(disabled_packages) $pkgconf::config_data(disabled_packages)
	}
        foreach pkg $pkgconf::known_packages {
	    ASSERT { [info exists pkgconf::config_data($pkg,version)] }
	    puts $open_file "set_saved_data \"$pkg,version\" $pkgconf::config_data($pkg,version)"
	    set pkgconf::saved_data($pkg,version) $pkgconf::config_data($pkg,version)
	}
	# This allows the versions to be read back in more easily, even if
	# there is a change to the number of packages.
	puts $open_file "set_saved_data versioned_packages [list $pkgconf::known_packages]"
	
	foreach flag $pkgconf::known_compiler_flags {
	    if { [info exists pkgconf::config_data(cflags,$flag)] != 0 } {
		puts $open_file "set_saved_data \"cflags,$flag\" \"$pkgconf::config_data(cflags,$flag)\""
	    }
	}
	if { $pkgconf::config_data(use_links) != 0 } {
	    puts $open_file "set_saved_data use_links 1"
	}

	puts $open_file "set_saved_data prefix   \"$pkgconf::config_data(prefix)\""
	set pkgconf::saved_data(prefix) $pkgconf::config_data(prefix)
	
	puts  $open_file "\n# End of $savename"
	close $open_file
    } message ]

    if { $status != 0 } {

	if { $open_file != "" } {
	    # Whatever error occurred, it happened after the file was opened.
	    # This is bad news, the file may be corrupt. To avoid complications,
	    # try to delete it.
	    catch {
		close $open_file
		file delete $savename
	    }
	    # Also get rid of the saved_data array.
	    foreach i [array names saved_data] {
		unset saved_data($i)
	    }
	}
	fatal_error "An unexpected error occurred while generating the save file $savename: $message"
    }
}

# }}}
# {{{  process_arguments()              

# ----------------------------------------------------------------------------
# This procedure takes care of updating the configuration information using
# the command line arguments, for example if there was an argument specifying
# a different version for a particular package then it is this procedure
# which takes care of it. This procedure also takes care of validating the
# arguments.
#
# This procedure should be invoked after setup_defaults has filled in the
# default values for all configuration data, and after read_save_file
# has updated them with saved data.
#
# There is a separate validation routine which gets invoked first.
# This takes care of serious errors in the command line arguments.
#

proc pkgconf::process_arguments { } {

    variable known_compiler_flags
    variable known_packages
    variable package_data
    variable known_targets
    variable target_data
    
    variable config_data
    variable target_arg
    variable platform_arg
    variable startup_arg
    variable prefix_arg
    variable disable_args
    variable enable_args
    variable version_args
    variable use_links_arg
    variable use_copies_arg
    variable cflags_args

    # Constructing the file name strings here facilitates generating
    # error messages.
    set packages_file [file join $pkgconf::component_repository "packages"]
    set targets_file  [file join $pkgconf::component_repository "targets"]

    # The target, platform, and startup values are used throughout so it
    # is convenient to have locals.
    set target   $pkgconf::config_data(target)
    set platform $pkgconf::config_data(platform)
    set startup  $pkgconf::config_data(startup)
    
    # Start by validating the --target argument This target must
    # be present in the known_targets list. If it is then the
    # target architecture can be updated.
    
    if { $pkgconf::target_arg != "" } {

	set target [find_target $pkgconf::target_arg]
	if { $target == "" } {
	    fatal_error "Invalid argument --target=$target_arg, this target is not listed in $targets_file"
	}
	set pkgconf::config_data(target) $target

	# It is possible that the platform and startup data from the save file
	# are no longer valid because of a change in the target. If so then the
	# relevant fields have to be cleared.
	if { $platform != "" } {
	    set platform [find_platform $target $platform]
	    set pkgconf::config_data(platform) $platform
	    set pkgconf::config_data(base_platform) [get_base_platform $target $platform]
	    if { ($platform != "") && ($startup != "") } {
		if { [lsearch -exact $pkgconf::target_data($target,$platform,startup) $startup] == -1 } {
		    set startup ""
		    set pkgconf::config_data(startup) ""
		}
	    }
	}
	
	# Next, if the platform and startup are not currently known then
	# it is possible to set up defaults for this target.
	if { $platform == "" } {
	    set platform [lindex $pkgconf::target_data($target,known_platforms) 0]
	    set pkgconf::config_data(platform) $platform
	} 
	if { $startup == "" } {
	    set startup [lindex $pkgconf::target_data($target,$platform,startup) 0]
	    set pkgconf::config_data(startup) $startup
	}
    }

    # If the target is known then it is possible to verify platform and startup
    # arguments. Without a target these arguments are meaningless.
    
    if { ($target != "") && ($pkgconf::platform_arg != "") } {

	set platform [find_platform $target $pkgconf::platform_arg]
	if { $platform == "" } {
	    fatal_error "Invalid argument --platform=$pkgconf::platform_arg, this platform is not listed in $targets_file"
	} 
	set pkgconf::config_data(platform) $platform
    }

    set pkgconf::config_data(base_platform) [get_base_platform $target $platform]

    if { ($target != "") && ($platform != "") && ($pkgconf::startup_arg != "" ) } {
	
	if { [lsearch -exact $pkgconf::target_data($target,$platform,startup) $pkgconf::startup_arg] == -1 } {
	    fatal_error "Invalid argument --startup=$startup_arg, this startup is not listed in $targets_file"
	}
	set startup $pkgconf::startup_arg
	set config_data(startup) $startup
    }

    # It is not really possible to validate the prefix argument.
    if { $pkgconf::prefix_arg != "" } {
	set pkgconf::config_data(prefix) $pkgconf::prefix_arg
    }

    # Cope with the use of symbolic links vs. copies
    if { $pkgconf::use_links_arg } {
	set pkgconf::config_data(use_links) 1
    }
    if { $pkgconf::use_copies_arg } {
	set pkgconf::config_data(use_links) 0
    }
    
    # For each package that is being disabled, make sure it is on the
    # list of disabled packages and not on the list of enabled packages.
    # The former operation is actually unnecessary if the package is
    # disabled by default, but harmless.
    foreach pkg $pkgconf::disable_args {
	set match [find_package $pkg]
	if { $match == "" } {
	    fatal_error "Invalid argument --disable-$pkg, package $pkg is not listed in $packages_file."
	}

	set index [lsearch -exact $pkgconf::config_data(enabled_packages) $match]
	if { $index != -1 } {
	    set pkgconf::config_data(enabled_packages) [lreplace $pkgconf::config_data(enabled_packages) $index $index]
	}
	set index [lsearch -exact $pkgconf::config_data(disabled_packages) $match]
	if { $index == -1 } {
	    lappend pkgconf::config_data(disabled_packages) $match
	}
    }

    # For each package that should be enabled, add it to the list of enabled
    # packages and remove it from the list of disabled packages. Again the
    # former is actually unnecessary if the package is enabled by default.
    foreach pkg $pkgconf::enable_args {
	set match [find_package $pkg]
	if { $match == "" } {
	    fatal_error "Invalid argument --enable-$pkg, package $pkg is not listed in $packages_file."
	}

	set index [lsearch -exact $pkgconf::config_data(disabled_packages) $match]
	if { $index != -1 } {
	    set pkgconf::config_data(disabled_packages) [lreplace $pkgconf::config_data(disabled_packages) $index $index]
	}
	set index [lsearch -exact $pkgconf::config_data(enabled_packages) $match]
	if { $index == -1 } {
	    lappend config_data(enabled_packages) $match
	}
    }
    
    # For each package where a version has been specified, validate this
    # version and use it. Also make sure that the package is enabled.
    foreach pkg [array names pkgconf::version_args] {

	set match [find_package $pkg]
	if { $match == "" } {
	    fatal_error "Invalid argument --$pkg-version, package $pkg is not listed in $packages_file."
	}
	if { [lsearch -exact $pkgconf::package_data($match,versions) $pkgconf::version_args($pkg)]  == -1 } {
	    fatal_error "Invalid argument --$pkg-version=$pkgconf::version_args($pkg), that version is not available."
	}
	set pkgconf::config_data($match,version) $pkgconf::version_args($pkg)
	
	set index [lsearch -exact $pkgconf::config_data(disabled_packages) $match]
	if { $index != -1 } {
	    set pkgconf::config_data(disabled_packages) [lreplace $pkgconf::config_data(disabled_packages) $index $index]
	}
	if { ($pkgconf::package_data($match,hardware) == 1) || ($pkgconf::package_data($match,default) == 0) } {
	    set index [lsearch -exact $pkgconf::config_data(enabled_packages) $match]
	    if { $index == -1 } {
		lappend config_data(enabled_packages) $match
	    }
	}
    }

    # For each compiler flag that has been specified explicitly on the command line,
    # store it in the configuration data
    foreach flag [array names pkgconf::cflags_args] {
	set pkgconf::config_data(cflags,$flag) $pkgconf::cflags_args($flag)
    }
}

# }}}
# {{{  reset_data()                     

# ----------------------------------------------------------------------------
# This routine gets rid of all data. It can be used if the script is
# switching from one configuration to a different one.
#
proc pkgconf::reset_data { } {

    foreach i [array names pkgconf::config_data] {
	unset pkgconf::config_data($i)
    }
    foreach i [array names pkgconf::saved_data] {
	unset pkgconf::saved_data($i)
    }
}

# }}}

# }}}
# {{{  GUI                              

# {{{  GUI variables                    

namespace eval pkgconf {

    namespace export gui

    # Which editor should be used for fine-grained configuration options ?
    # The VISUAL environment variable is the correct way to obtain this
    # information on Unix operating systems. Windows machines do not
    # seem to have an obvious equivalent.
    variable editor ""
    if { [info exists ::env(VISUAL)] } {
	set editor $::env(VISUAL)
    }

    # The names of the pages in the notebook. These names are
    # invented by the notebook widget.
    variable target_frame   ""
    variable packages_frame ""
    variable options_frame  ""
    variable flags_frame    ""
    variable build_frame    ""

    # It is convenient to keep track of whether or not certain information
    # has changed. This simplifies the task of deciding whether or not
    # a page in the notebook needs redrawing.
    #
    # Note: currently if the user makes a change and then undoes that
    # change the variable remains set. It is possible to get around this,
    # but that would require a bit more work.
    variable packages_page_needs_refresh 0
    variable options_page_needs_refresh  0
    variable build_page_needs_refresh    0

    # Is it currently possible to do a build, or is it necessary to
    # update the build tree first.
    variable build_tree_needs_update     0

    # Each notebook page is given its own array of global data.
    array set target_page_data {};
    array set packages_page_data {};
    array set options_page_data {};
    array set flags_page_data {};
    array set build_page_data {};
    
    # The state of the build output window. There are two flags to consider:
    # whether the output window is visible or not; and whether the output
    # window is docked or not.
    variable output_docked  1
    variable output_visible 0
    
    # For gui mode, what is the name of the toplevel window ?
    # If this script is embedded in a larger application then it can
    # be made to run inside a frame in that window.
    namespace export set_toplevel_window
    variable toplevel ".pkgconf"
}

# }}}
# {{{  Effective Tcl support            

# ----------------------------------------------------------------------------
# For now the intention is to allow this script to be run from a completely
# standard wish or tclsh interpreter, version 8.0 or later. This means it is
# not possible to use extensions such as TIX or ITCL. However in the GUI
# it is still desirable to have a notebook widget, so here is an
# implementation in vanilla TK. The balloon widget from the same source
# is used as well.
#
# The code derives from the book Effective Tcl/Tk programming by Mark
# Harrison and Michael McLennan. The actual source code was obtained from
# the Addison-Wesley web site and contains the following copyright.
#
#       This distribution contains examples from the "Effective Tcl/Tk
#       Programming" book.  You can study these examples as you read
#       through the book, and you can steal the code for your own
#       applications!
#
#
# The code is enclosed in a separate namespace. Also I have modified
# the code in a number of places to make it more general-purpose.
#
namespace eval pkgconf::efftcl {

# ----------------------------------------------------------------------
#  EXAMPLE: tabnotebook that can dial up pages
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

# BLV fix: this script may execute in tclsh rather than in wish,
# so the option command may not be available.    
if { [info command "option"] == "option"} {

    # BLV: I do not like this background colour for the tabs.
    if { 0 } {
	option add *Tabnotebook.tabs.background #666666 widgetDefault
    }
    option add *Tabnotebook.margin 6 widgetDefault
    option add *Tabnotebook.tabColor #a6a6a6 widgetDefault
    option add *Tabnotebook.activeTabColor #d9d9d9 widgetDefault
    option add *Tabnotebook.tabFont \
	    -*-helvetica-bold-r-normal--*-120-* widgetDefault
}

# BLV fix: replace global variables with per-namespace ones.
array set tnInfo {}
array set bhInfo {}

proc tabnotebook_create {win} {
    variable tnInfo

    frame $win -class Tabnotebook
    canvas $win.tabs -highlightthickness 0
    pack $win.tabs -fill x

    notebook_create $win.notebook
    pack $win.notebook -expand yes -fill both

    set tnInfo($win-tabs)    ""
    set tnInfo($win-current) ""
    set tnInfo($win-pending) ""
    return $win
}

#
# BLV fix: add three arguments, one specifying a function to be
# invoked when the tab becomes visible, another for when the user
# leaves the tab, and a text string for balloon help. This function can
# be used to redraw a page if this is necessary due to changes in other
# pages, and an empty string can be used if desired. An empty string
# can also be used for the balloon help.
#

proc tabnotebook_page {win name enterfn leavefn help} {
    variable tnInfo

    set page [notebook_page $win.notebook $name]
    lappend tnInfo($win-tabs)  $name
    set tnInfo($name-enterfn)  $enterfn
    set tnInfo($name-leavefn)  $leavefn
    set tnInfo($name-help)     $help
    
    if {$tnInfo($win-pending) == ""} {
        set id [after idle [list pkgconf::efftcl::tabnotebook_refresh $win]]
        set tnInfo($win-pending) $id
    }
    return $page
}

proc tabnotebook_refresh {win} {
    variable tnInfo

    $win.tabs delete all

    set margin [option get $win margin Margin]
    set color [option get $win tabColor Color]
    set font [option get $win tabFont Font]
    set x 2
    set maxh 0

    foreach name $tnInfo($win-tabs) {
        set id [$win.tabs create text \
            [expr $x+$margin+2] [expr -0.5*$margin] \
            -anchor sw -text $name -font $font \
            -tags [list $name]]

        set bbox [$win.tabs bbox $id]
        set wd [expr [lindex $bbox 2]-[lindex $bbox 0]]
        set ht [expr [lindex $bbox 3]-[lindex $bbox 1]]
        if {$ht > $maxh} {
            set maxh $ht
        }

        $win.tabs create polygon 0 0  $x 0 \
            [expr $x+$margin] [expr -$ht-$margin] \
            [expr $x+$margin+$wd] [expr -$ht-$margin] \
            [expr $x+$wd+2*$margin] 0 \
            2000 0  2000 10  0 10 \
            -outline black -fill $color \
            -tags [list $name tab tab-$name]

        $win.tabs raise $id

        $win.tabs bind $name <ButtonPress-1> \
            [list pkgconf::efftcl::tabnotebook_display $win $name]

	# BLV - add support for balloon help
	if { $tnInfo($name-help) != "" } {
	    balloonhelp_canvas_for $win.tabs $name $tnInfo($name-help)
	}
        set x [expr $x+$wd+2*$margin]
    }
    set height [expr $maxh+2*$margin]
    $win.tabs move all 0 $height

    $win.tabs configure -width $x -height [expr $height+4]

    if {$tnInfo($win-current) != ""} {
        tabnotebook_display $win $tnInfo($win-current)
    } else {
        tabnotebook_display $win [lindex $tnInfo($win-tabs) 0]
    }
    set tnInfo($win-pending) ""
}

proc tabnotebook_display {win name} {
    variable tnInfo
    variable nbInfo
    
    # BLV: invoke appropriate callbacks in the current and new
    # tabs.
    set old_tab $tnInfo($win-current)
    if { ($old_tab != "") && ($tnInfo($old_tab-leavefn) != "") } {
	 $tnInfo($old_tab-leavefn) $nbInfo($win.notebook-page-$old_tab))
    }
    if { $tnInfo($name-enterfn) != "" } {
	$tnInfo($name-enterfn) $nbInfo($win.notebook-page-$name)
    }
    
    notebook_display $win.notebook $name

    set normal [option get $win tabColor Color]
    $win.tabs itemconfigure tab -fill $normal

    set active [option get $win activeTabColor Color]
    $win.tabs itemconfigure tab-$name -fill $active
    $win.tabs raise $name

    set tnInfo($win-current) $name
}

# ----------------------------------------------------------------------
#  EXAMPLE: simple notebook that can dial up pages
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

# BLV - again check for TK being present.
if { [info command option] == "option" } {
    option add *Notebook.borderWidth 2 widgetDefault
    option add *Notebook.relief sunken widgetDefault
}

# BLV: replace nbInfo global with a per-namespace variable
array set nbInfo {}

proc notebook_create {win} {
    variable nbInfo

    frame $win -class Notebook
    pack propagate $win 0

    set nbInfo($win-count) 0
    set nbInfo($win-pages) ""
    set nbInfo($win-current) ""
    return $win
}

proc notebook_page {win name} {
    variable nbInfo

    set page "$win.page[incr nbInfo($win-count)]"
    lappend nbInfo($win-pages) $page
    set nbInfo($win-page-$name) $page

    frame $page

    if {$nbInfo($win-count) == 1} {
        after idle [list pkgconf::efftcl::notebook_display $win $name]
    }
    
    return $page
}

proc notebook_display {win name} {
    variable nbInfo

    set page ""
    if {[info exists nbInfo($win-page-$name)]} {
        set page $nbInfo($win-page-$name)
    } elseif {[winfo exists $win.page$name]} {
        set page $win.page$name
    }
    if {$page == ""} {
        error "bad notebook page \"$name\""
    }

    notebook_fix_size $win

    if {$nbInfo($win-current) != ""} {
        pack forget $nbInfo($win-current)
    }
    pack $page -expand yes -fill both
    set nbInfo($win-current) $page
}

proc notebook_fix_size {win} {
    variable nbInfo

    update idletasks

    set maxw 0
    set maxh 0
    foreach page $nbInfo($win-pages) {
        set w [winfo reqwidth $page]
        if {$w > $maxw} {
            set maxw $w
        }
        set h [winfo reqheight $page]
        if {$h > $maxh} {
            set maxh $h
        }
    }
    set bd [$win cget -borderwidth]
    set maxw [expr $maxw+2*$bd]
    set maxh [expr $maxh+2*$bd]
    $win configure -width $maxw -height $maxh
}

# ----------------------------------------------------------------------
#  EXAMPLE: use "wm" commands to manage a balloon help window
# ----------------------------------------------------------------------
#  Effective Tcl/Tk Programming
#    Mark Harrison, DSC Communications Corp.
#    Michael McLennan, Bell Labs Innovations for Lucent Technologies
#    Addison-Wesley Professional Computing Series
# ======================================================================
#  Copyright (c) 1996-1997  Lucent Technologies Inc. and Mark Harrison
# ======================================================================

# BLV fix: again this script may execute in tclsh rather than in wish.
if { [info command "option"] == "option" } {
    option add *Balloonhelp*background white widgetDefault
    option add *Balloonhelp*foreground black widgetDefault
    option add *Balloonhelp.info.wrapLength 3i widgetDefault
    option add *Balloonhelp.info.justify left widgetDefault
    option add *Balloonhelp.info.font \
	    -*-lucida-medium-r-normal-sans-*-120-* widgetDefault

    toplevel .balloonhelp -class Balloonhelp \
	    -background black -borderwidth 1 -relief flat

    # BLV fix: I want to create the arrow image inline rather than
    # read it in from a file.
    image create bitmap arrow -data "\
#define arrow_width 8
#define arrow_height 8
static unsigned char arrow_bits[] = {
   0x1f, 0x0f, 0x0f, 0x1f, 0x39, 0x70, 0x20, 0x00};
"
    label .balloonhelp.arrow -anchor nw -image arrow

#    label .balloonhelp.arrow -anchor nw \
#	    -bitmap @[file join $env(EFFTCL_LIBRARY) images arrow.xbm]
    pack .balloonhelp.arrow -side left -fill y

    label .balloonhelp.info
    pack .balloonhelp.info -side left -fill y

    wm overrideredirect .balloonhelp 1
    wm withdraw .balloonhelp
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_for <win> <mesg>
#
#  Adds balloon help to the widget named <win>.  Whenever the mouse
#  pointer enters this widget and rests within it for a short delay,
#  a balloon help window will automatically appear showing the
#  help <mesg>.
# ----------------------------------------------------------------------

proc balloonhelp_for {win mesg} {
    variable bhInfo
    set bhInfo($win) $mesg

    bind $win <Enter> {pkgconf::efftcl::balloonhelp_pending %W %W}
    bind $win <Leave> {pkgconf::efftcl::balloonhelp_cancel}
}

# BLV: a variant that can be used for canvas items.

proc balloonhelp_canvas_for { canvas id mesg } {
    variable bhInfo
    set bhInfo($canvas-$id) $mesg

    $canvas bind $id <Enter> [list pkgconf::efftcl::balloonhelp_pending $canvas $canvas-$id]
    $canvas bind $id <Leave> {pkgconf::efftcl::balloonhelp_cancel}
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_control <state>
#
#  Turns balloon help on/off for the entire application.
# ----------------------------------------------------------------------
set bhInfo(active) 1

proc balloonhelp_control {state} {
    variable bhInfo

    if {$state} {
        set bhInfo(active) 1
    } else {
        balloonhelp_cancel
        set bhInfo(active) 0
    }
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_pending <win>
#
#  Used internally to mark the point in time when a widget is first
#  touched.  Sets up an "after" event so that balloon help will appear
#  if the mouse remains within the current window.
# ----------------------------------------------------------------------

# BLV - the first argument identifies the window, the second the help
# string.
proc balloonhelp_pending {win id} {
    variable bhInfo

    balloonhelp_cancel
    set bhInfo(pending) [after 1500 [list pkgconf::efftcl::balloonhelp_show $win $id]]
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_cancel
#
#  Used internally to mark the point in time when the mouse pointer
#  leaves a widget.  Cancels any pending balloon help requested earlier
#  and hides the balloon help window, in case it is visible.
# ----------------------------------------------------------------------
proc balloonhelp_cancel {} {
    variable bhInfo

    if {[info exists bhInfo(pending)]} {
        after cancel $bhInfo(pending)
        unset bhInfo(pending)
    }
    wm withdraw .balloonhelp
}

# ----------------------------------------------------------------------
#  USAGE:  balloonhelp_show <win>
#
#  Used internally to display the balloon help window for the
#  specified <win>.
# ----------------------------------------------------------------------

# BLV - the first argument identifies the window, the second the
# help message.

proc balloonhelp_show {win id} {
    variable bhInfo

    if {$bhInfo(active)} {
        .balloonhelp.info configure -text $bhInfo($id)

	# BLV: switched from using window-relative positions
	# to mouse relative position.
	if { 0 } {
	    set x [expr [winfo rootx $win]+10]
	    set y [expr [winfo rooty $win]+[winfo height $win]]
	} else {
	    set coords [winfo pointerxy $win]
	    set x [expr [lindex $coords 0] + 15]
	    set y [expr [lindex $coords 1] + 15]
	}
	if { ($x >= 0) && ($y >= 0) } {
	    wm geometry .balloonhelp +$x+$y
	    wm deiconify .balloonhelp
	    raise .balloonhelp
	}
    }
    unset bhInfo(pending)
}

}

# }}}
# {{{  standalone initialisation        

# ----------------------------------------------------------------------------
# GUI Start-up. It is necessary to work out a few things about the operating
# environment.
#
# 1) is this script being run via tclsh or via wish ? If the former then only
#    command line operation is possible. If the latter then a GUI environment
#    can be provided as well.
#
# 2) if TK is present, has it been extended with Tix ? If so use the standard
#    TK color scheme etc., not the Tix variant.
#
# 3) if TK is present, get rid of the top-level window.
#
# If this command is not being run stand-alone then it is the responsibility
# of the calling code to take appropriate action, e.g. to enable gui mode
# and to set up the appropriate options.
#

proc pkgconf::gui_standalone_init { } {

    variable gui_possible
    
    if { [info command tk] == "tk" } {

	# TK is loaded. For now allow GUI mode, but the actual decision
	# depends on command line arguments.
	set gui_possible 1

	# Get rid of the top-level window. Otherwise this might pop up if the
	# script performs a blocking operation too early.
	wm withdraw .

	# If TIX is also present, prevent the TIX color scheme etc. from taking
	# effect.
	if { [info command tix] == "tix" } {
	    tix resetoptions TK TK
	}
	
	# It is probably not a good idea to accept "send" messages. However
	# setting the appname also affects option handling, which is more
	# useful. Note that the send command is not available in all versions
	# of Tcl.
	if { [info command send] == "send" } {
	    rename send {}
	}
	tk appname pkgconf
    
    } else {

	# TK is not loaded, so gui mode is not possible.
	set gui_possible 0
    }
}

# }}}
# {{{  set toplevel window              

# ----------------------------------------------------------------------------
# Allow a containing program, if any, to specify the window in which
# this script should do its funky stuff. It is not possible to change
# this window while pkgconf is active, so if for any reason it is
# necessary to change the toplevel window then the old one must be
# destroyed first, the new name must be installed, and then there
# must be a new call to pkgconf::gui to redraw the window.
#

proc pkgconf::set_toplevel_window { name } {
    variable toplevel
    set toplevel $name
}

# }}}
# {{{  main GUI draw routine            

# ----------------------------------------------------------------------------
# The main GUI code.
#
# The routine pkgconf::gui is responsible for creating the various windows.
# These are as follows:
#
# 1) a menu bar containing menus for File, Window and Help. Currently the only
#    entry in File is Exit, and the entries in Help are all disabled. The
#    Window menu allows the output window to be hidden or shown.
#
# 2) a notebook frame containing four pages:
#
#    a) target hardware. This lists all known target architectures,
#       and allows the user to select the appropriate one using a
#       set of radio buttons.
#
#    b) packages. This lists all known packages and the versions for
#       each package. Unfortunately neither radio buttons nor checkbuttons
#       provide entirely the right paradigm here, but checkbuttons are
#       good enough.
#
#    c) an edit page. This allows the user to invoke a text editor for
#       any of the fine-grained configuration headers.
#
#    d) a compiler flags page. This allows the user to manipulate the
#       compiler flags etc.
#
#    e) a build page. This allows the user to create the build tree and to
#       invoke make, make tests, make clean, etc. There is also an entry
#       field to allow the install directory to be changed.
#
# 3) a label field along the bottom, which can be used by the various
#    frames to display warning messages.
#
# 4) a separate frame to hold the outputs of builds. This can be either
#    docked below the main frame, or it can be a separate toplevel window.
#    The build frame contains a number of buttons to allow the text
#    screen to be cleared, removed from the display, docked/undocked,
#    or saved to a file.
#
# 5) the options database is consulted to decide whether the output
#    window should be docked or not by default.
#
# 6) if there is a file ~/.pkgconfrc this file gets executed. It is
#    assumed to contain valid Tcl code.
#
# If this script is running inside a large application then there may be
# a frame $toplevel_window already. If so this frame should be used.
# Otherwise a separate toplevel window should be created, complete with
# menu bar.
#

proc pkgconf::gui { } {

    variable toplevel
    if { [winfo exists $toplevel] == 0 } {
	create_toplevel_window
    }
    
    # Now create a notebook, and add four pages to it. The names of these
    # pages have to be stored in separate variables.
    variable target_frame
    variable packages_frame
    variable options_frame
    variable flags_frame
    variable build_frame
    
    efftcl::tabnotebook_create $toplevel.notebook
    set target_frame   [efftcl::tabnotebook_page $toplevel.notebook "Target"   \
	    pkgconf::nbpage_targets_enter pkgconf::nbpage_targets_leave        \
	    "Select target hardware"]
    set packages_frame [efftcl::tabnotebook_page $toplevel.notebook "Packages" \
	    pkgconf::nbpage_packages_enter pkgconf::nbpage_packages_leave      \
	    "Select desired packages"]
    set options_frame  [efftcl::tabnotebook_page $toplevel.notebook "Options"  \
	    pkgconf::nbpage_options_enter pkgconf::nbpage_options_leave        \
	    "Edit configuration options"]
    set flags_frame    [efftcl::tabnotebook_page $toplevel.notebook "Flags"    \
	    pkgconf::nbpage_flags_enter pkgconf::nbpage_flags_leave            \
	    "Edit compiler flags"]
    set build_frame    [efftcl::tabnotebook_page $toplevel.notebook "Build"    \
	    pkgconf::nbpage_build_enter pkgconf::nbpage_build_leave            \
	    "Build this configuration"]

    # By default the notebook is the widget that expands. If a build output
    # window is present instead then it is the output window that expands.
    pack $toplevel.notebook -side top -fill both -expand 1

    # Make sure that the message window exists, since some of the
    # notebook update routines refer to it.
    label $toplevel.message -text "" -anchor w -relief raised -borderwidth 2
    pack  $toplevel.message -side top -fill x

    # Now create the notebook pages.
    nbpage_targets_initialise  $target_frame
    nbpage_packages_initialise $packages_frame
    nbpage_options_initialise  $options_frame
    nbpage_flags_initialise    $flags_frame
    nbpage_build_initialise    $build_frame
    
    # Create the output window, but leave it hidden for now.
    frame  $toplevel.output
    frame  $toplevel.output.buttons
    button $toplevel.output.buttons.hide   -text "Hide"    -command pkgconf::output_hide_window
    button $toplevel.output.buttons.undock -text "Undock"  -command pkgconf::output_undock_window
    button $toplevel.output.buttons.clear  -text "Clear"   -command pkgconf::output_clear_window
    button $toplevel.output.buttons.save   -text "Save..." -command pkgconf::output_save
    pack   $toplevel.output.buttons.hide         \
	    $toplevel.output.buttons.undock      \
	    $toplevel.output.buttons.clear       \
	    $toplevel.output.buttons.save -side left -expand 1
    pack   $toplevel.output.buttons -side top -fill x
    text   $toplevel.output.text -wrap word -state disabled -yscrollcommand [list $toplevel.output.scroll set]
    pack   $toplevel.output.text -side left -fill both -expand 1
    scrollbar $toplevel.output.scroll -orient vertical -command [list $toplevel.output.text yview]
    pack   $toplevel.output.scroll -side right -fill y

    # Create a toplevel window matching the output frame, but leave it
    # hidden for now. Give it the same bindings as the toplevel window.
    toplevel .pkgconf_output -menu .menubar -class Pkgconf
    wm withdraw .pkgconf_output
    wm title    .pkgconf_output "pkgconf build output"
    wm protocol .pkgconf_output WM_DELETE_WINDOW { pkgconf::handle_exit }
    
    bind .pkgconf_output <Alt-q> { pkgconf::handle_exit }
    bind .pkgconf_output <Alt-f><KeyPress-x> { pkgconf::handle_exit }
    
    frame  .pkgconf_output.buttons
    button .pkgconf_output.buttons.hide   -text "Hide"    -command pkgconf::output_hide_window
    button .pkgconf_output.buttons.undock -text "Dock"    -command pkgconf::output_undock_window
    button .pkgconf_output.buttons.clear  -text "Clear"   -command pkgconf::output_clear_window
    button .pkgconf_output.buttons.save   -text "Save..." -command pkgconf::output_save
    pack   .pkgconf_output.buttons.hide         \
	    .pkgconf_output.buttons.undock      \
	    .pkgconf_output.buttons.clear       \
	    .pkgconf_output.buttons.save -side left -expand 1
    pack   .pkgconf_output.buttons -side top -fill x
    text   .pkgconf_output.text -wrap word -state disabled -yscrollcommand { .pkgconf_output.scroll set }
    pack   .pkgconf_output.text -side left -fill both -expand 1
    scrollbar .pkgconf_output.scroll -orient vertical -command { .pkgconf_output.text yview }
    pack   .pkgconf_output.scroll -side right -fill y

    efftcl::balloonhelp_for $toplevel.output.buttons.hide \
	    "Remove the build output window, until more output appears."
    efftcl::balloonhelp_for .pkgconf_output.buttons.hide \
	    "Remove the build output window, until more output appears."
    efftcl::balloonhelp_for $toplevel.output.buttons.undock \
	    "Make the build output appear in a separate window."
    efftcl::balloonhelp_for .pkgconf_output.buttons.undock \
	    "Attach the build output to the main pkgconf window."
    efftcl::balloonhelp_for $toplevel.output.buttons.clear \
	    "Remove all build output text."
    efftcl::balloonhelp_for .pkgconf_output.buttons.clear \
	    "Remove all build output text."
    efftcl::balloonhelp_for $toplevel.output.buttons.save \
	    "Save build output to a file."
    efftcl::balloonhelp_for .pkgconf_output.buttons.save \
	    "Save build output to a file."

    # Consult the options database.
    variable output_docked
    option add "*docked" 1 startupFile
    set output_docked [option get $toplevel docked Pkgconf]

    # Look for a file ~/.pkgconfrc and execute it.
    if { [file isfile [file join "~" ".pkgconfrc"]] } {
	if { [namespace eval :: { catch { source [file join "~" ".pkgconfrc"] } message } ] != 0 } {
	    warning "The following error occurred while executing ~/.pkgconfrc: $message"
	}
    }
}

# ----------------------------------------------------------------------------
# This routine creates the toplevel window, if pkgconf is running
# independently rather than inside a larger application.

proc pkgconf::create_toplevel_window { } {

    variable toplevel
    
    # Start by creating the menubar. TK8's support for native menus is
    # used.
    menu .menubar -type menubar
    menu .menubar.config -tearoff 0 -type normal
    menu .menubar.edit   -tearoff 0 -type normal
    menu .menubar.view   -tearoff 0 -type normal
    menu .menubar.help   -tearoff 0 -type normal
    
    .menubar.config add command -label "Exit" -command pkgconf::handle_exit -underline 1 \
	    -accelerator "Alt+Q"
    
    .menubar.edit add command -label "Undo" -state disabled
    .menubar.edit add command -label "Redo" -state disabled
    
    .menubar.view add checkbutton -label "Show output" \
	    -variable pkgconf::output_visible -onvalue 1 -offvalue 0 \
	    -command pkgconf::output_menu
    .menubar.view add checkbutton -label "Balloon help" \
	    -variable pkgconf::efftcl::bhInfo(active) -onvalue 1 -offvalue 0
    
    .menubar.help add command -label "Help on pkgconf"    -state disabled
    .menubar.help add command -label "Help on the system" -state disabled
    .menubar.help add command -label "Search"             -state disabled
    .menubar.help add separator
    .menubar.help add command -label "About pkgconf" -state disabled
    
    .menubar add cascade -menu .menubar.config -label "Configuration" -underline 0
    .menubar add cascade -menu .menubar.edit   -label "Edit"          -underline 0
    .menubar add cascade -menu .menubar.view   -label "View"          -underline 0
    .menubar add cascade -menu .menubar.help   -label "Help"          -underline 0

    # Create the toplevel window, giving it a menubar and a name,
    # and establish the accelerator binding.
    toplevel $toplevel -menu .menubar -class Pkgconf
    wm title $toplevel pkgconf
    bind $toplevel <Alt-q> { pkgconf::handle_exit }
    wm protocol $toplevel WM_DELETE_WINDOW { pkgconf::handle_exit }

    # Although there is a Configuration menu instead of a file menu, it is
    # desirable to support alt-f-x as a reasonably standard way of exiting
    # the program.
    bind $toplevel <Alt-f><KeyPress-x> { pkgconf::handle_exit }
}

# ----------------------------------------------------------------------------
# This routine gets invoked for exit handling: when the user invokes the
# exit entry on the file menu; or when the alt-q accelerator is used; or
# when a window delete request is received. For now the code just exits,
# but it might be an idea to offer to save configuration changes if any.
#
proc pkgconf::handle_exit { } {
    exit 0
}

# }}}
# {{{  Notebook pages                   

# {{{  Targets page                     

# ----------------------------------------------------------------------------
# The targets page. This allows the user to select the target architecture,
# platform, and startup. The range of available target architectures cannot
# change while the program is running, so this window is actually fixed.
#

proc pkgconf::nbpage_targets_initialise { win } {

    variable config_data
    variable known_targets
    variable target_data
    variable target_page_data

    # I need a variable that can be associated with the radiobuttons and
    # identifies the selected target uniquely. It is not really
    # possible to do this with the three variables in the config data
    # directly.
    set target_page_data(selection) [list $config_data(target) $config_data(platform) $config_data(startup)]
    
    # Conceivably the number of targets could be larger than can fit
    # conveniently on to one screen, so it is desirable use a scrollable
    # form. Start by creating a scrollbar, a canvas, and a frame within
    # that canvas, and set up a standard resize callback. The canvas
    # starts off small, but expands as necessary. The actual size will
    # therefore be determined by whichever page does not have a
    # vertical scrollbar, typically the build page. 
    scrollbar $win.scrollbar -command "$win.viewport yview" -orient vertical
    pack $win.scrollbar -side right -fill y

    canvas $win.viewport -height 10m -yscrollcommand "$win.scrollbar set"
    pack $win.viewport -fill both -expand 1 -padx 20 -pady 20

    frame $win.viewport.form
    $win.viewport create window 0 0 -anchor nw -window $win.viewport.form

    bind $win.viewport.form <Configure> "pkgconf::nbpage_form_resized $win"

    # Now fill in the form with details of all the targets.
    # These are arranged in a simple grid.
    set form $win.viewport.form
    set row 0

    foreach target $known_targets {

	label $form.arch-$row -text "Target [get_target_alias $target]" -anchor w
	grid  $form.arch-$row -row $row -column 0 -sticky w
	incr row

	foreach platform $target_data($target,known_platforms) {
	    foreach startup $target_data($target,$platform,startup) {
		
		label $form.platform-$row -text "    Platform [get_platform_alias $target $platform]," -anchor w
		grid  $form.platform-$row -row $row -column 0 -sticky w
		label $form.startup-$row -text "$startup startup" -anchor w
		grid  $form.startup-$row -row $row -column 1 -sticky w -ipadx 5
		radiobutton $form.button-$row -variable pkgconf::target_page_data(selection) \
			-value [list $target $platform $startup] \
			-command pkgconf::clear_status_line
		grid $form.button-$row -row $row -column 2 -sticky w
		incr row
	    }
	}
    }
}

#
# The enter routine gets invoked when the user switches to the targets tab,
# and also during start-up since the targets tab is the first one. It is
# responsible for keeping track of the initial target selection, so that
# when the user switches to a different tab it is possible to know whether
# or not the target has actually changed.
#
# As a useful side effect, the routine updates the status line.
#
proc pkgconf::nbpage_targets_enter { win } {

    variable config_data
    variable target_page_data

    set target_page_data(initial_selection) $target_page_data(selection)
    
    if { ($config_data(target) == "") || ($config_data(platform) == "") || ($config_data(startup) == "") } {
	set_status_line "Please select a target system."
    } else {
	clear_status_line
    }
}

proc pkgconf::nbpage_targets_leave { win } {

    variable target_page_data

    if { $target_page_data(selection) != $target_page_data(initial_selection) } {
	variable config_data
	variable build_tree_needs_update

	set config_data(target)     [lindex $target_page_data(selection) 0]
	set config_data(platform)   [lindex $target_page_data(selection) 1]
	set config_data(base_platform) [get_base_platform $config_data(target) $config_data(platform)]
	set config_data(startup)    [lindex $target_page_data(selection) 2]
	set build_tree_needs_update 1

	# A change of target has implications for the compiler flags, which
	# must be handled immediately. The user may not go into the flags
	# tab so the changes cannot be done in the flags tab enter/leave
	# routines.
	update_compiler_flags
    }
}

#
# This routine take care of resize operations. Whenever the size
# of the form changes the canvas' scroll region needs to be adjusted.
#
# Note that this code is also used for the packages page.
#
proc pkgconf::nbpage_form_resized { win } {

    set bbox      [$win.viewport bbox all]
    set old_bbox  [$win.viewport cget -scrollregion]
    set width     [winfo width $win.viewport.form]
    set old_width [$win.viewport cget -width]
    if { ("$bbox" != "$old_bbox") || ($width != $old_width) } {
	$win.viewport configure -width $width -scrollregion $bbox
    }
}

# }}}
# {{{  Packages page                    

# ----------------------------------------------------------------------------
# The packages page. This is mostly similar to the targets page.
# It is necessary to provide a mechanism for disabling packages.
# It is also necessary to worry about changes to the target architecture.
# The initialise() routine takes care of most of these things.

proc pkgconf::nbpage_packages_initialise { win } {

    variable config_data
    variable known_packages
    variable package_data
    variable target_data
    variable packages_page_data

    # Start by storing details of the target that is currently
    # being displayed.
    set packages_page_data(current_target)   $config_data(target)
    set packages_page_data(current_platform) $config_data(platform)
    
    # Typically the number of packages will be larger than can fit
    # conveniently on to one screen, so it is desirable use a scrollable
    # form. Start by creating a scrollbar, a canvas, and a frame within
    # that canvas, and set up a standard resize callback. The canvas
    # starts of small, but expands as necessary. The actual size will
    # therefore be determined by whichever page does not have a
    # vertical scrollbar, typically the build page.
    scrollbar $win.scrollbar -command "$win.viewport yview" -orient vertical
    pack $win.scrollbar -side right -fill y

    canvas $win.viewport -height 10m -yscrollcommand "$win.scrollbar set"
    pack   $win.viewport -fill both -expand 1 -padx 20 -pady 20

    frame $win.viewport.form
    $win.viewport create window 0 0 -anchor nw -window $win.viewport.form

    bind $win.viewport.form <Configure> "pkgconf::nbpage_form_resized $win"

    # Now fill in the form with details of all the packages.
    # These are arranged in a simple grid.
    set form $win.viewport.form
    set row 0

    # Now handle the packages themselves. Each package can be in one
    # of the following states:
    #
    # 1) a hardware package is enabled by default if it is specified
    #    for the current target or platform, disabled otherwise.
    #    However the user should be able to explicitly enable or
    #    disable it. This is handled by config_data(enabled_packages)
    #    and config_data(disabled_packages).
    #
    # 2) a non-hardware package may be enabled or disabled by default,
    #    but the user can change this. Again this is handled by
    #    enabled_packages and disabled_packages.
    #
    # It is convenient to cache the list of target and platform specific
    # packages.
    set target_packages   ""
    set platform_packages ""
    if { $config_data(target) != "" } {
	set target_packages $target_data($config_data(target),packages)
	if { $config_data(platform) != "" } {
	    set platform_packages $target_data($config_data(target),$config_data(platform),packages)
	}
    }
    foreach pkg $known_packages {

	# What is the current version of each package ?
	set current_version ""

	if { $package_data($pkg,hardware) != 0 } {
	    if { ([lsearch -exact $target_packages $pkg] != -1) || ([lsearch -exact $platform_packages $pkg] != -1)} {
		set current_version $config_data($pkg,version)
	    }
	} elseif { $package_data($pkg,default) != 0 } {
	    set current_version $config_data($pkg,version)
	}
	
	if { [lsearch -exact $config_data(enabled_packages) $pkg] != -1 } {
	    set current_version $config_data($pkg,version)
	}
	if { [lsearch -exact $config_data(disabled_packages) $pkg] != -1 } {
	    set current_version ""
	}

	# Store the current version, so that it is easier to figure out what
	# has changed.
	set packages_page_data($pkg,version) $current_version

	label $form.label-$row -text "[get_package_alias $pkg]:"
	grid  $form.label-$row -row $row -column 0 -sticky w
	foreach version $package_data($pkg,versions) {
	    label $form.version-$row -text "Version $version"
	    grid  $form.version-$row -row $row -column 1 -sticky w -ipady 5

	    radiobutton $form.button-$row -variable pkgconf::packages_page_data($pkg,version) \
		    -value $version -command [list pkgconf::nbpage_packages_change $pkg]
	    grid $form.button-$row -row $row -column 2 -sticky w
	    incr row
	}
	label $form.version-$row -text "Disable"
	grid  $form.version-$row -row $row -column 1 -sticky w
	radiobutton $form.button-$row -variable pkgconf::packages_page_data($pkg,version) \
		-value "" -command [list pkgconf::nbpage_packages_change $pkg]
	grid $form.button-$row -row $row -column 2 -sticky w
	incr row
    }
}

#
# This routine gets invoked whenever there is a change to one of the
# packages. It sets a flag indicating that the build tree is out of data.
# It will also update the appropriate config_data.
#
proc pkgconf::nbpage_packages_change { pkg } {

    variable build_tree_needs_update
    set build_tree_needs_update 1

    variable packages_page_data
    variable config_data
    
    set version $packages_page_data($pkg,version)
    if { $version == "" } {
	
	# This package is being explicitly disabled.
	set index [lsearch -exact $config_data(enabled_packages) $pkg]
	if { $index != -1 } {
	    set config_data(enabled_packages) [lreplace $config_data(enabled_packages) $index $index]
	}

	set index [lsearch -exact $config_data(disabled_packages) $pkg]
	if { $index == -1 } {
	    lappend config_data(disabled_packages) $pkg
	}
    } else {
	
	# A package version is being changed, possibly causing it to
	# be enabled.
	set config_data($pkg,version) $version

	set index [lsearch -exact $config_data(disabled_packages) $pkg]
	if { $index != -1 } {
	    set config_data(disabled_packages) [lreplace $config_data(disabled_packages) $index $index]
	}

	set index [lsearch -exact $config_data(enabled_packages) $pkg]
	if { $index == -1 } {
	    lappend config_data(enabled_packages) $pkg
	}
    }
}

#
# The packages page needs to be redrawn every time a different target is
# selected, so that the user can select which version of the HAL packages
# should be used. The redraw can be achieved by deleting the existing
# contents of the window and re-invoking the initialise function.

proc pkgconf::nbpage_packages_enter { win } {

    variable packages_page_data
    variable config_data
    
    if { ($packages_page_data(current_target)   != $config_data(target)) ||
         ($packages_page_data(current_platform) != $config_data(platform)) } {
	     
	     foreach subwin [winfo children $win] {
		 destroy $subwin
	     }
	     nbpage_packages_initialise $win
    }

    # If the target architecture is not yet known, remind the user.
    if { ($config_data(target) == "") || ($config_data(platform) == "") || ($config_data(startup) == "") } {
	set_status_line "Please select a target system."
    } else {
	clear_status_line
    }
}

#
# Currently this code does nothing, instead it is left to the
# nbpage_packages_change routine above to update the appropriate
# global information. This is unfortunate in that if the user
# makes a mistake and undoes it pkgconf still thinks that the
# build tree is out of date.

proc pkgconf::nbpage_packages_leave { win } {
}

# }}}
# {{{  Options page                     

# ----------------------------------------------------------------------------
# The options page. This consists of two frame arranged vertically.
# The top frame allows the user to select the editor to use, which is
# initialised from the VISUAL environment variable. The bottom frame
# is another scrollable canvas allowing the user to select a header
# file to edit.
#
# A number of special cases have to be allowed for. If there is no
# text editor then all the buttons to edit a text file have to
# be disabled. If there is no pkgconf directory in the build tree
# yet then there will be no files to edit. Even if there is an
# an pkgconf directory it may not contain all the files it is
# supposed to, especially if the build tree is not up to date.
#

proc pkgconf::nbpage_options_initialise { win } {

    # Start with the top frame. This contains a label, an entry field,
    # and a button that invokes a file selector.
    # NOTE: these borderwidths should not be hardwired.
    variable editor
    frame  $win.top -relief raised -borderwidth 2
    label  $win.top.label  -text "Editor:"
    entry  $win.top.entry  -relief sunken -borderwidth 2 -textvariable pkgconf::editor
    button $win.top.button -text "Browse..." -command pkgconf::nbpage_options_browse
    pack   $win.top.button -side right
    pack   $win.top.entry  -side right -fill x -expand 1
    pack   $win.top.label  -side left
    pack   $win.top        -side top -fill x

    efftcl::balloonhelp_for $win.top.entry  "The text editor that should be used."
    efftcl::balloonhelp_for $win.top.button "Select the text editor that should be used."

    # The entry widget does not support a command callback when
    # a string has been typed in, so it is necessary to use the
    # trace facility instead to enable or disable the buttons.
    # Note that this routine may get invoked from the update routine,
    # so it is necessary to clean up the old trace.
    trace vdelete  pkgconf::editor w pkgconf::nbpage_options_editor_changed
    trace variable pkgconf::editor w pkgconf::nbpage_options_editor_changed

    # At this point there are now button widgets in this page. The
    # nbpage_options_editor_changed routine used this list.
    variable options_page_data
    set options_page_data(buttons) ""
    
    # Now check for editable files in the pkgconf directory.
    # Three of the files, makefile, pkgconf.mak and system.h are generated
    # and should not be edited by users. There may also be various .stamp
    # files used by the build system.
    variable build_tree
    set files ""
    foreach filename [glob -nocomplain [file join $build_tree "pkgconf" "*"]] {
	set filename [file tail $filename]
	if { [string match {*.stamp} $filename] } continue
	if { ($filename == "makefile") || ($filename == "pkgconf.mak") || ($filename == "system.h") } continue
	lappend files $filename
    }

    if { $files == "" } {

	# OK, just display a message. There is likely to be an appropriate
	# warning in the message frame at the bottom of the window already.
	label $win.message -text "There are no configuration files to edit."
	pack  $win.message -fill both -expand 1

	# A valid configuration always has a makevars file to edit, so the
	# implication is that the build tree is out of date. It probably
	# makes sense to set the relevant flag here.
	variable build_tree_needs_update
	set build_tree_needs_update 1
	return
    }

    # The editable files are displayed in a scrollable frame, just
    # like the targets and packages pages.
    scrollbar $win.scrollbar -command "$win.viewport yview" -orient vertical
    pack $win.scrollbar -side right -fill y

    canvas $win.viewport -height 10m -yscrollcommand "$win.scrollbar set"
    pack $win.viewport -fill both -expand 1 -padx 20 -pady 20

    frame $win.viewport.form
    $win.viewport create window 0 0 -anchor nw -window $win.viewport.form

    bind $win.viewport.form <Configure> "pkgconf::nbpage_form_resized $win"

    # Now fill in the form with all the buttons.
    set form $win.viewport.form
    set row 0

    foreach file $files {

	button $form.button-$row -text "<pkgconf/$file>" -anchor w \
		-command [list pkgconf::nbpage_options_edit $file]
	grid $form.button-$row -row $row -column 0 -sticky we -padx 10 -pady 5
	label $form.label-$row -anchor w -text "Edit [file join $build_tree pkgconf $file]"
	grid $form.label-$row -row $row -column 1 -sticky w

	efftcl::balloonhelp_for $form.button-$row "Edit this configuration file."
	lappend options_page_data(buttons) $form.button-$row
	if { $editor == "" } {
	    $form.button-$row configure -state disabled
	}
	incr row
    }
}

# The options page needs to be redrawn every time the build tree is
# regenerated, since this may result in a different set of editable
# configuration header files.

proc pkgconf::nbpage_options_enter { win } {

    variable options_page_needs_refresh

    if { $options_page_needs_refresh != 0 } {
	set options_page_needs_refresh 0
	foreach subwin [winfo children $win] { destroy $subwin }
	nbpage_options_initialise $win
    }

    # It is also desirable to display some warnings.
    variable config_data
    variable build_tree_needs_update
    variable editor
    variable options_page_data

    set options_page_data(no_editor) 0

    if { ($config_data(target) == "") || ($config_data(platform) == "") || ($config_data(startup) == "") } {
	set_status_line "The target hardware has not yet been selected."
    } elseif { $build_tree_needs_update } {
	set_status_line "The build tree needs updating."
    } elseif { $editor == "" } {
	set options_page_data(no_editor) 1
	set_status_line "No text editor has been defined yet."
    } else {
	clear_status_line
    }
}

#
# There are no obvious actions to be taken if the user switches to
# a different tab. Even if the user has invoked the editor on a
# configuration file there is no guarantee that anything has actually
# changed.
proc pkgconf::nbpage_options_leave { win } {
}

#
# This procedure allows the user to select a text editor. It gets invoked
# from the browse button.
#
# NOTE: on windows it might be desirable to specify a .exe extension as
# the type, but even that is optional under NT.
#
proc pkgconf::nbpage_options_browse { } {

    variable editor
    variable build_tree
    variable toplevel
    
    set filename [tk_getOpenFile -initialdir $build_tree -parent $toplevel -title "Select text editor"]
    if { $filename == "" } {
	return
    }
    set editor $filename
}

#
# This procedure is invoked whenever the editor variable is changed.
# Its purpose is to enable or disable the buttons for the various
# configuration headers.
#
# The code does not actually check whether the current editor value corresponds
# to a valid executable. It is just about possible to check for an existing
# file somewhere in the path, but that is expensive and there is still no
# guarantee that the file is a valid executable.
#

proc pkgconf::nbpage_options_editor_changed { name1 name2 mode } {

    variable editor
    variable options_page_data
    variable toplevel
    
    if { $editor == "" } {

	foreach button $options_page_data(buttons) {
	    $button configure -state disabled
	}
    } else {
	if { $options_page_data(no_editor) } {
	    $toplevel.message configure -text ""
	    set options_page_data(no_editor) 0
	}
	foreach button $options_page_data(buttons) {
	    $button configure -state normal
	}
    }
}

# This routine gets invoked to edit a header file. It simply
# executes the current editor in the background. No attempt is
# made to wait for command completion, to prevent a single file
# from being edited multiple times, or anything like that.

proc pkgconf::nbpage_options_edit { filename } {
    
    variable editor
    variable build_tree

    run_command pkgconf::nbpage_options_ignore $editor [file join $build_tree pkgconf $filename]
}

# This callback gets invoked from the run_command code whenever the
# editor sends output. In the case of emacsclient such output
# is the unfortunate string "Waiting for Emacs...Done". In the case
# of xterm -e vi there is no output.
#
# On the whole it is better to ignore the output. This may mean
# that warning or error messages get discared.

proc pkgconf::nbpage_options_ignore { msg } {

}

# }}}
# {{{  Flags page                       

# ----------------------------------------------------------------------------
# The flags page. This consists of a big grid, arranged vertically into
# five separate categories of flags. Within each category there is a
# label identifying the category and four separate entry fields for the
# various variables.
#
# The data for a given flag can come from two sources. The default value
# comes from the targets file, and in most cases this will be an empty
# string. Alternatively the user can overwrite it, e.g. as a command
# line argument or in the GUI, in which case the changed information
# gets saved in the pkgconf save file.
#

proc pkgconf::nbpage_flags_initialise { win } {

    variable flags_page_data
    variable known_compiler_flags
    variable config_data

    # Start by creating array entries for all the known compiler flags.
    if { ($config_data(target) != "") && ($config_data(platform) != "") } {
	foreach flag $known_compiler_flags {
	    set flags_page_data($flag) [get_platform_cflags $config_data(target) $config_data(platform) $flag]
	}
    } else {
	foreach flag $known_compiler_flags {
	    set flags_page_data($flag) ""
	}
    }

    # If the current configuration information contains any compiler flags,
    # update the appropriate variable.
    foreach flag $known_compiler_flags {
	if { [info exists config_data(cflags,$flag)] } {
	    set flags_page_data($flag) $config_data(cflags,$flag)
	}
    }

    # Keep track of which target and platform the current settings correspond to.
    set flags_page_data(target)   $config_data(target)
    set flags_page_data(platform) $config_data(platform)

    # Time to actually create the interface.
    # Again this tab uses a frame inside a scrolled canvas.
    scrollbar $win.scrollbar -command "$win.viewport yview" -orient vertical
    pack $win.scrollbar -side right -fill y

    canvas $win.viewport -height 10m -yscrollcommand "$win.scrollbar set"
    pack   $win.viewport -fill both -expand 1 -padx 20 -pady 20

    frame $win.viewport.form
    $win.viewport create window 0 0 -anchor nw -window $win.viewport.form

    bind $win.viewport.form <Configure> "pkgconf::nbpage_form_resized $win"

    # Now fill in the form with details of all the flags.
    # These are arranged in a simple grid.
    set form $win.viewport.form
    set row 0

    grid columnconfigure $form 1 -weight 1
    label $form.archflags -text "Architecture-related compiler flags"
    grid  $form.archflags -row $row -column 0 -columnspan 2 -sticky w 
    incr row
    foreach flag { ARCHFLAGS CARCHFLAGS CXXARCHFLAGS LDARCHFLAGS } {
	label $form.label-$flag -text $flag
	grid $form.label-$flag -row $row -column 0 -sticky w
	entry $form.entry-$flag -textvariable pkgconf::flags_page_data($flag) -width 60
	grid $form.entry-$flag -row $row -column 1 -sticky we
	incr row
    }

    label $form.errflags -text "Warnings and errors"
    grid  $form.errflags -row $row -column 0 -columnspan 2 -sticky w 
    incr row
    foreach flag { ERRFLAGS CERRFLAGS CXXERRFLAGS LDERRFLAGS } {
	label $form.label-$flag -text $flag
	grid $form.label-$flag -row $row -column 0 -sticky w
	entry $form.entry-$flag -textvariable pkgconf::flags_page_data($flag)
	grid $form.entry-$flag -row $row -column 1 -sticky we
	incr row
    }

    label $form.dbgflags -text "Debugging and optimization levels"
    grid  $form.dbgflags -row $row -column 0 -columnspan 2 -sticky w 
    incr row
    foreach flag { DBGFLAGS CDBGFLAGS CXXDBGFLAGS LDDBGFLAGS } {
	label $form.label-$flag -text $flag
	grid $form.label-$flag -row $row -column 0 -sticky w
	entry $form.entry-$flag -textvariable pkgconf::flags_page_data($flag)
	grid $form.entry-$flag -row $row -column 1 -sticky we
	incr row
    }

    label $form.langflags -text "Language-related flags"
    grid  $form.langflags -row $row -column 0 -columnspan 2 -sticky w 
    incr row
    foreach flag { LANGFLAGS CLANGFLAGS CXXLANGFLAGS LDLANGFLAGS } {
	label $form.label-$flag -text $flag
	grid $form.label-$flag -row $row -column 0 -sticky w
	entry $form.entry-$flag -textvariable pkgconf::flags_page_data($flag)
	grid $form.entry-$flag -row $row -column 1 -sticky we
	incr row
    }

    label $form.extraflags -text "Miscellaneous"
    grid  $form.extraflags -row $row -column 0 -columnspan 2 -sticky w 
    incr row
    foreach flag { EXTRAFLAGS CEXTRAFLAGS CXXEXTRAFLAGS LDEXTRAFLAGS } {
	label $form.label-$flag -text $flag
	grid $form.label-$flag -row $row -column 0 -sticky w
	entry $form.entry-$flag -textvariable pkgconf::flags_page_data($flag)
	grid $form.entry-$flag -row $row -column 1 -sticky we
	incr row
    }
}

#
# There is nothing to be done on entry to this tab. If the target architecture
# has changed then there will have been a separate call to update_compiler_flags
# below. This cannot be delayed until the enter call because there is no
# guarantee that the user will switch to the flags tab after changing target
# architecture.
#
proc pkgconf::nbpage_flags_enter { win } {
}

#
# On leaving the page it is necessary to check for any flags that are
# different from the default settings, and update the config_data
# settings appropriately.
#
proc pkgconf::nbpage_flags_leave { win } {

    variable config_data
    variable flags_page_data
    variable known_compiler_flags
    
    if { ($config_data(target) == "") && ($config_data(platform) == "") } {
	
	# If no target has been defined then it is not possible to compare
	# with the default setting. All that can be done is set or clear
	# the current config setting.
	foreach flag $known_compiler_flags {
	    if { $flags_page_data($flag) == "" } {
		if { [info exists config_data(cflags,$flag) ] } {
		    unset config_data(cflags,$flag)
		}
	    } else {
		set config_data(cflags,$flag) $flags_page_data($flag)
	    }
	}
    } else {

	# For any flag which currently has default settings,
	# clear the appropriate config_data() entry. Otherwise store
	# the flag data.
	foreach flag $known_compiler_flags {

	    if { $flags_page_data($flag) == [get_platform_cflags $config_data(target) $config_data(platform) $flag] } {
		if { [info exists config_data(cflags,$flag) ] } {
		    unset config_data(cflags,$flag)
		}
	    } else {
		set config_data(cflags,$flag) $flags_page_data($flag)
	    }
	}
    }
}

#
# When the target architecture changes it is necessary to update the compiler
# flags appropriately. Basically any flag that was defined by the old target
# has to be cleared, and then any flag defined for the new target has to be
# installed. There is a risk that some user changes will be lost, but there
# is no easy solution to this.
#
proc pkgconf::update_compiler_flags { } {

    variable config_data
    variable flags_page_data
    variable known_compiler_flags

    foreach flag $known_compiler_flags {

	if { ($flags_page_data(target) == "") || ($flags_page_data(platform) == "") } {
	    set old_default_flag ""
	} else {
	    set old_default_flag [get_platform_cflags $flags_page_data(target) $flags_page_data(platform) $flag]
	}
	set new_default_flag [get_platform_cflags $config_data(target) $config_data(platform) $flag]

	# If there is no change to the default flags leave things well alone.
	# That way user changes will not get overwritten.
	if { $old_default_flag == $new_default_flag } {
	    continue
	}

	# Otherwise always install the new settings, which may mean eliminating
	# user changes.
	set flags_page_data($flag) $new_default_flag
	if { [info exists config_data(cflags,$flag)] } {
	    unset config_data(cflags,$flag)
	}
    }

    set flags_page_data(target)   $config_data(target)
    set flags_page_data(platform) $config_data(platform)
}

# }}}
# {{{  Build page                       

# ----------------------------------------------------------------------------
# The build page. This consists of the following buttons, gridded,
# with explanatory text.
#
# 1) Update build tree
# 2) Make 
# 3) Make tests
# 4) Make misc
# 5) Make clean
#
# The make buttons should be disabled if the build tree is out of date.
# This is controlled by the variable build_tree_needs_update, which
# may have got set during the options page creation. An additional
# check that needs to be made here is whether or not the makefile
# exists. If the current config data does not specify the target
# then even the "Update tree" button must be disabled.
#
# Whenever a make is in progress all the buttons should be disabled.
#

proc pkgconf::nbpage_build_initialise { win } {

    # Update the variable build_tree_needs_update if there is no
    # makefile in the build tree.
    variable build_tree_needs_update
    variable component_repository
    variable build_tree
    variable config_data
    
    if { [file isfile [file join $build_tree makefile]] == 0 } {
	set build_tree_needs_update 1
    }
    if { ($config_data(target) == "") || ($config_data(platform) == "") || ($config_data(startup) == "") } {
	set build_tree_needs_update 1
    }

    # Create three labels identifying the various trees.
    frame $win.top
    label $win.top.source_label  -anchor w -text "Component repository:"
    label $win.top.source_value  -anchor w -text "$component_repository"
    label $win.top.build_label   -anchor w -text "Current build tree:"
    label $win.top.build_value   -anchor w -text "$build_tree"
    label $win.top.install_label -anchor w -text "Current install tree:"
    label $win.top.install_value -anchor w -text "$config_data(prefix)"
    grid  $win.top.source_label  -row 0 -column 0 -sticky w
    grid  $win.top.source_value  -row 0 -column 1 -sticky we
    grid  $win.top.build_label   -row 1 -column 0 -sticky w
    grid  $win.top.build_value   -row 1 -column 1 -sticky we
    grid  $win.top.install_label -row 2 -column 0 -sticky w
    grid  $win.top.install_value -row 2 -column 1 -sticky we
    grid  columnconfigure $win.top 1 -weight 1
    pack  $win.top -side top -fill x -padx 5 -pady 5
    
    # Create a grid with the four buttons and with explanatory
    # messages.
    frame $win.grid
    pack $win.grid -fill both -expand 1
    set win $win.grid
    
    button $win.tree  -text "Update tree" -command pkgconf::nbpage_build_update_tree
    button $win.make  -text "Make"        -command pkgconf::nbpage_build_make
    button $win.tests -text "Make tests"  -command pkgconf::nbpage_build_tests
    button $win.misc  -text "Make misc"  -command pkgconf::nbpage_build_misc
    button $win.clean -text "Make clean"  -command pkgconf::nbpage_build_clean

    if { ($config_data(target) == "" ) || ($config_data(platform) == "") || ($config_data(startup) == "") } {
	$win.tree configure -state disabled
    }
    if { $build_tree_needs_update } {
	$win.make  configure -state disabled
	$win.tests configure -state disabled
	$win.misc configure -state disabled
	$win.clean configure -state disabled
    }
    grid   $win.tree  -row 0 -column 0 -sticky we -padx 5 -pady 5
    grid   $win.make  -row 1 -column 0 -sticky we -padx 5 -pady 5
    grid   $win.tests -row 2 -column 0 -sticky we -padx 5 -pady 5
    grid   $win.misc  -row 3 -column 0 -sticky we -padx 5 -pady 5
    grid   $win.clean -row 4 -column 0 -sticky we -padx 5 -pady 5

    message $win.treemsg -anchor w -justify left -width 300 -text \
	    "Create or update the build and install trees."
    message $win.makemsg -anchor w -justify left -width 300 -text \
	    "Build the current configuration, leaving libraries and header files in the install tree."
    message $win.testsmsg -anchor w -justify left -width 300 -text \
	    "Build the test executables for the current configuration. These executables will be\
	    placed in the install tree."
    message $win.miscmsg -anchor w -justify left -width 300 -text \
	    "Build the misc executables for the current configuration. These executables will be\
	    placed in the misc directory."
    message $win.cleanmsg -anchor w -justify left -width 300 -text \
	    "Perform a clean-up operation in the build and install trees."
    grid $win.treemsg  -row 0 -column 1 -sticky we
    grid $win.makemsg  -row 1 -column 1 -sticky we
    grid $win.testsmsg -row 2 -column 1 -sticky we
    grid $win.miscmsg  -row 3 -column 1 -sticky we
    grid $win.cleanmsg -row 4 -column 1 -sticky we
    grid columnconfigure $win 1 -weight 1
}

# The build page needs refreshing every time that the user changes the
# target or one of the packages. The important question is whether or
# not the build tree needs to be updated before any builds can take place.
# Of course the build tree can only be generated if the target architecture
# is known.

proc pkgconf::nbpage_build_enter { win } {

    variable build_tree_needs_update
    variable build_frame
    variable toplevel
    
    if { $build_tree_needs_update } {
	$build_frame.grid.make  configure -state disabled
	$build_frame.grid.tests configure -state disabled
	$build_frame.grid.misc configure -state disabled
	$build_frame.grid.clean configure -state disabled

	set_status_line "The build tree must be updated before this configuration can be built."
    } else {
	clear_status_line
    }

    variable config_data
    if { ($config_data(target) == "") || ($config_data(platform) == "") || ($config_data(startup) == "") } {
	$build_frame.grid.tree configure -state disabled
	set_status_line "The target hardware has not yet been selected."
    } else {
	$build_frame.grid.tree configure -state normal
    }
}

#
# There are no actual settings that can be changed in this tab,
# so no updates are required.
#
proc pkgconf::nbpage_build_leave { win } {
}

#
# This callback function is invoked during the various makes.
# If it is passed an empty string then the command has finished and
# all the buttons can be reenabled. Otherwise it is necessary to send
# some text to the output window.

proc pkgconf::nbpage_build_handle_output { mesg } {

    if { $mesg == "" } {
	output_add_text "Build has finished.\n"
	nbpage_build_control_buttons 1
    } else {
	output_add_text "$mesg\n"
    }
}


# This utility is used to control all the buttons in the build frame.
proc pkgconf::nbpage_build_control_buttons { on } {

    variable build_frame
    set buttons [grid slaves $build_frame.grid -column 0]
    foreach button $buttons {
	if { $on } {
	    $button configure -state normal
	} else {
	    $button configure -state disabled
	}
    }
}

#
# These functions get invoked for the buttons in the build page.
#
proc pkgconf::nbpage_build_update_tree { } {

    variable toplevel
    variable options_page_needs_refresh
    variable build_tree_needs_update
    
    nbpage_build_control_buttons 0
    produce_build_tree
    nbpage_build_control_buttons 1
    $toplevel.message configure -text ""
    set options_page_needs_refresh 1
    set build_tree_needs_update    0
}

proc pkgconf::nbpage_build_make { } {

    variable build_tree
    variable config_data
    
    nbpage_build_control_buttons 0
    output_add_text "Building current configuration.\nBuild tree is $build_tree.\nInstall tree is $config_data(prefix)\n"
    run_command pkgconf::nbpage_build_handle_output make -C $build_tree
}

proc pkgconf::nbpage_build_tests { } {

    variable build_tree
    variable config_data
    nbpage_build_control_buttons 0
    output_add_text \
	    "Building test executables.\nBuild tree is $build_tree\nInstall tree is $config_data(prefix)\n"
    run_command pkgconf::nbpage_build_handle_output make -C $build_tree tests
}

proc pkgconf::nbpage_build_misc { } {

    variable build_tree
    variable config_data
    nbpage_build_control_buttons 0
    output_add_text \
	    "Building misc executables.\nBuild tree is $build_tree\nInstall tree is $config_data(prefix)\n"
    run_command pkgconf::nbpage_build_handle_output make -C $build_tree misc
}

proc pkgconf::nbpage_build_clean { } {

    variable build_tree
    nbpage_build_control_buttons 0
    output_add_text "Cleaning build tree $build_tree.\n"
    run_command pkgconf::nbpage_build_handle_output make -C $build_tree clean
}

# }}}

# }}}
# {{{  Output window                    

# ----------------------------------------------------------------------------
# These routines take care of the output window. There are two variables
# describing the current state of the window: output_docked and
# output_visible. There are four commands which can be invoked from
# buttons, and a further command which can be invoked from the windows
# menu. There is also a routine which can be used to add text to
# the current output window, making it visible if necessary.
#
# The packing of the main .pkgconf window has to be redone every time
# the output window is displayed or hidden, if the output window is
# currently docked. To avoid duplicating code there are auxiliary
# routine output_make_visible and output_make_invisible.
#

# This routine is invoked from the View menu. Its purpose is to
# hide or display the output window, depending on the value of
# output_visible. Note that output_visible indicates the desired
# state of affairs, not the actual state of affairs.
#
proc pkgconf::output_menu { } {

    variable output_visible

    if { $output_visible == 0 } {
	output_make_invisible
    } else {
	output_make_visible
    }
}

proc pkgconf::output_make_visible { } {

    variable output_docked
    variable toplevel
    
    if { $output_docked == 0 } {
	# The output is in a separate window. Deiconifying it should make
	# it visible.
	wm deiconify .pkgconf_output
    } else {
	# It is necessary to repack the main window. This time it is the
	# output frame that should expand.
	pack forget $toplevel.notebook $toplevel.message
	pack $toplevel.notebook -side top -fill x
	pack $toplevel.message  -side top -fill x
	pack $toplevel.output   -side top -fill both -expand 1
    }
}

proc pkgconf::output_make_invisible { } {

    variable output_docked
    variable toplevel
    
    if { $output_docked == 0 } {
	# The output is in a separate window, just withdraw it.
	wm withdraw .pkgconf_output
    } else {
	# It is necessary to repack the .pkgconf window without
	# the output window. This time it is the notebook which
	# should expand to fit all available space.
	pack forget $toplevel.notebook $toplevel.message $toplevel.output
	pack $toplevel.notebook -side top -fill both -expand 1
	pack $toplevel.message -side top -fill x
    }
}

#
# This routine is invoked when the output window is already visible
# and the user has pressed the button in that window to make it
# invisible.
#
proc pkgconf::output_hide_window { } {

    variable output_visible
    variable toplevel
    ASSERT { $output_visible != 0 }

    output_make_invisible
    set output_visible 0
}

#
# Switch between a docked and an undocked window. The window must
# currently be visible. Any text must be copied across from
# one widget to the other.
#
proc pkgconf::output_undock_window { } {

    variable output_visible
    variable output_docked
    variable toplevel

    ASSERT { $output_visible != 0 }

    output_make_invisible
    
    $toplevel.output.text configure -state normal
    .pkgconf_output.text configure -state normal

    if { $output_docked == 0 } {
	$toplevel.output.text insert end [.pkgconf_output.text get 1.0 "end - 1 chars"]
	.pkgconf_output.text delete 1.0 end
	set output_docked 1
    } else {
	.pkgconf_output.text insert end [$toplevel.output.text get 1.0 "end - 1 chars"]
	$toplevel.output.text delete 1.0 end
	set output_docked 0
    }

    $toplevel.output.text configure -state disabled
    .pkgconf_output.text configure -state disabled

    output_make_visible
}

#
# Clear the text window, which should currently be visible.
# 
proc pkgconf::output_clear_window { } {

    variable output_visible
    variable output_docked
    variable toplevel

    ASSERT { $output_visible != 0 }
    if { $output_docked == 0 } {
	.pkgconf_output.text configure -state normal
	.pkgconf_output.text delete 1.0 end
	.pkgconf_output.text configure -state disabled
    } else {
	$toplevel.output.text configure -state normal
	$toplevel.output.text delete 1.0 end
	$toplevel.output.text configure -state disabled
    }
}

#
# Save the current contents of the text window to a file.
#
proc pkgconf::output_save { } {

    variable build_tree
    variable output_docked
    variable toplevel
    
    set filename [tk_getSaveFile -initialdir $build_tree -initialfile pkgconf.log \
	    -parent [expr { $output_docked ? "$toplevel" : ".pkgconf_output" } ] ]
    
    if { $filename == "" } {
	return
    }
    
    if { [catch {
	set file [open $filename w]
	if { $output_docked == 0 } {
	    puts $file [.pkgconf_output.text get 1.0 "end - 1 chars"]
	} else {
	    puts $file [$toplevel.output.text get 1.0 "end - 1 chars"]
	}
	close $file
    } message] != 0 } {
	warning "Failed to save build output to $filename: $message"
    }
}

#
# Output some text. This gets invoked mainly during builds, but also
# by the report() function when generating a build tree. It is
# necessary to make sure that the text widget is visible.
#
proc pkgconf::output_add_text { msg } {

    variable output_visible
    variable output_docked
    variable toplevel
    
    if { $output_visible == 0 } {
	output_make_visible
	set output_visible 1
    }

    if { $output_docked == 0 } {
	.pkgconf_output.text configure -state normal
	.pkgconf_output.text insert end $msg
	.pkgconf_output.text configure -state disabled
	.pkgconf_output.text see end
    } else {
	$toplevel.output.text configure -state normal
	$toplevel.output.text insert end $msg
	$toplevel.output.text configure -state disabled
	$toplevel.output.text see end
    }
}

# }}}
# {{{  Status line                      

# ----------------------------------------------------------------------------
# Some utility routines to update the status line.

proc pkgconf::set_status_line { text } {
    variable toplevel
    $toplevel.message configure -text $text
}

proc pkgconf::clear_status_line { } {
    variable toplevel
    $toplevel.message configure -text ""
}

# }}}
# {{{  Diagnostics                      

# ----------------------------------------------------------------------------
# GUI versions of the same routines. For now tk_messageBox will have
# to suffice for fatal error 
#
proc pkgconf::gui_fatal_error_handler { msg } {

    variable toplevel

    if { [winfo exists $toplevel] } {
	tk_messageBox -parent $toplevel -icon error -title "pkgconf: fatal error" -type ok -message $msg
    } else {
	tk_messageBox -icon error -title "pkgconf: fatal error" -type ok -message $msg
    }
    exit 1
}

proc pkgconf::gui_warning_handler { msg } {

    variable toplevel

    if { [winfo exists $toplevel] } {
	tk_messageBox -parent $toplevel -icon error -title "pkgconf: fatal error" -type ok -message $msg
    } else {
	tk_messageBox -icon error -title "pkgconf: fatal error" -type ok -message $msg
    }
}

proc pkgconf::gui_report_handler { msg } {

    output_add_text "$msg\n"
    update
}

# }}}
# {{{  Executing external commands      

# ----------------------------------------------------------------------------
# This code is responsible for executing external commands. The broken nature
# of pipes under Windows makes this somewhat more difficult than it should be.

proc pkgconf::run_command { callback command args } {

    # Start up the specified command. Actually the main command is run such that
    # both stdout and stderr are piped into cat, thus making sure that the
    # stderr output is not discarded.
    set result [catch {	set pipe [open  "|[concat $command $args] 2>@ stdout" "r"] } message]
    if { $result != 0 } {
	warning "Failed to execute $command: $message"
	return
    }

    # Now set the pipe to non-blocking, to prevent the whole GUI from
    # locking up by accident.
    fconfigure $pipe -blocking 0

    # Set up an event handler to read back any output.
    fileevent $pipe readable [list pkgconf::handle_command_output $pipe $callback]
}

#
# This routine gets invoked whenever there may be data waiting on the
# pipe.
#
proc pkgconf::handle_command_output { pipe callback } {

    set data  ""
    set count [gets $pipe data]
    if { $count > 0 } {
	$callback $data
    } elseif { [eof $pipe] } {
	catch { close $pipe }
	$callback ""
    }
}

# }}}

# }}}
# {{{  Produce build tree               

# {{{  Description                      

# ----------------------------------------------------------------------------
# Producing the build tree. This requires the following information:
#
# 1) the location of the component repository. This should be known.
#
# 2) the location of the build tree. This should also be known.
#
# 3) full details of the target architecture. These may not have been supplied
#    yet, which would result in a fatal error.
#
# 4) details of which packages are explicitly enabled or disabled.
#
# 5) details of which version of each package should be used.
#
# 6) prefix information, i.e. where the system will be installed.
#    The default value "" indicates an install directory relative to the
#    build tree.
#
# This routine may be called for an empty build tree, or it
# may be used to update an existing build tree.
#
# The steps that have to be taken are as follows:
#
# 1) create a pkgconf directory, which is where all the editable
#    header files will end up. A few other files will end up there
#    as well.
#
# 2) examine all packages, including the hal packages. For each package:
#
#    a) if there is a src directory in the component repository, reproduce
#       this in the build tree. If this src directory has any
#       sub-directories, these have to be created as well.
#
#       The src directory should contain a file PKGconf.mak. This has to be
#       copied across as makefile. There may also be PKGconf.mak files in
#       subdirectories which should also get copied across. Typically the
#       latter will be for internal development purposes.
#
#    b) if there is an include directory in the component repository,
#       make a matching directory in the build tree. Ditto for any
#       subdirectories other than pkgconf.
#
#       The purpose of this include directory is to allow users to make
#       editable copies of the header files in the build tree. A makefile
#       is generated in this directory which will take care of updating the
#       header files that actually get used for building, i.e. those in the
#       include directory. The header files are not actually copied across
#       at this point, that will happen during the first make.
#
#       The pkgconf directory is handled differently. Every file in
#       pkgconf is copied across to the build tree's pkgconf directory,
#       unless there is already a matching file in the build tree.
#       This avoids overwriting edits made by the user in the build
#       tree. The disadvantage is that if the version in the component
#       repository is edited it will have to be copied by hand, or the
#       -force command line argument will have to be used.
#
#    c) if there is a tests directory in the component repository, make
#       a matching directory in the build tree and copy
#       across PKGconf.mak. If this tests directory has any subdirectories
#       then these must be created as well.
#
#    d) similarly for any 'misc' directories
#
# 3) create the install directory tree, complete with lib, include, and tests
#    directories, and the include/pkgconf sub-directory. Additional
#    sub-directories within include need not be generated at this point,
#    that happens as such sub-directories are encountered within the
#    component repository.
#
# 4) copy across the makevars file.
#
# 5) generate the files specific to this configuration.
#
# Note that subdirectories corresponding to old packages are not
# deleted, they simply get ignored. Arguably this should change,
# but then you run the risk of deleting user changes in a package
# that has been disabled only temporarily.
#

# }}}
# {{{  Check requirements               

# ----------------------------------------------------------------------------
# It is not possible to generate a build tree unless certain requirements
# are satisfied. Most of these will have been checked elsewhere.
#
proc pkgconf::check_requirements { } {

    ASSERT { $pkgconf::build_tree != "" }
    ASSERT { $pkgconf::component_repository != "" }

    if { $pkgconf::component_repository == $pkgconf::build_tree } {
	report "Builds of eCos cannot happen in the same directory as the sources."
	report "Currently your build tree is $pkgconf::build_tree."
	report "This is the same directory as your component repository."
	fatal_error "Unable to proceed with builds in this directory."
    }

    if { $pkgconf::config_data(target) == "" } {
	fatal_error "No target architecture has been specified."
    }
    if { $pkgconf::config_data(platform) == "" } {
	fatal_error "No target platform has been specified."
    }
    if { $pkgconf::config_data(startup) == "" } {
	fatal_error "No target startup mechanism has been specified."
    }
}

# }}}
# {{{  Directory and file utilities     

# ----------------------------------------------------------------------------
# Start with a number of utility routines to access all files in
# a directory, stripping out well-known files such as makefile.am.
# The routines take an optional pattern argument if only certain
# files are of interest.
#
# Note that symbolic links are returned as well as files.
#
proc pkgconf::locate_files { dir { pattern "*"} } {

    ASSERT { $dir != "" }

    # Start by getting a list of all the files.
    set filelist [glob -nocomplain -- [file join $dir $pattern]]

    # Eliminate the pathnames from all of these files
    set filenames ""
    foreach file $filelist {
	if { [string range $file end end] != "~" } {
	    lappend filenames [file tail $file]
        }
    }

    # Eliminate any obviously spurious entries.
    foreach file { CVS Makefile.am Makefile.in makefile acinclude.m4 aclocal.m4 configure.in configure } {
	set index [lsearch -exact $filenames $file]
	if { $index != -1 } {
	    set filenames [lreplace $filenames $index $index]
	}
    }

    # Eliminate any subdirectories.
    set subdirs ""
    foreach name $filenames {
	if { [file isdir [file join $dir $name]] } {
	    lappend subdirs $name
	}
    }
    foreach subdir $subdirs {
	set index [lsearch -exact $filenames $subdir]
	set filenames [lreplace $filenames $index $index]
    }

    return $filenames
}

#
# This utility returns all sub-directories, as opposed to all files.
# A variant glob pattern is used here. This version is not recursive.
proc pkgconf::locate_subdirs { dir { pattern "*" }} {

    ASSERT { $dir != "" }

    set dirlist [glob -nocomplain -- [file join $dir $pattern "."]]

    # Eliminate the pathnames and the spurious /. at the end of each entry
    set dirnames ""
    foreach dir $dirlist {
	lappend dirnames [file tail [file dirname $dir]]
    }

    # Get rid of the CVS directory, if any
    set index [lsearch -exact $dirnames "CVS"]
    if { $index != -1 } {
	set dirnames [lreplace $dirnames $index $index]
    }

    # That should be it.
    return $dirnames
}

#
# A variant which is recursive. This one does not support a pattern.
#
proc pkgconf::locate_all_subdirs { dir } {

    ASSERT { $dir != "" }

    set result ""
    foreach subdir [locate_subdirs $dir] {
	lappend result $subdir
	foreach x [locate_all_subdirs [file join $dir $subdir]] {
	    lappend result [file join $subdir $x]
	}
    }
    return $result
}

#
# This routine returns a list of all the files in a given directory and in
# all subdirectories, preserving the subdirectory name.
#
proc pkgconf::locate_all_files { dir { pattern "*" } } {

    ASSERT { $dir != "" }

    set files   [locate_files $dir $pattern]
    set subdirs [locate_subdirs $dir]

    foreach subdir $subdirs {
	set subfiles [locate_all_files [file join $dir $subdir] $pattern]
	foreach file $subfiles {
	    lappend files [file join $subdir $file]
	}
    }

    return $files
}

#
# Sometimes a directory may be empty, or contain just a CVS subdirectory,
# in which case there is no point in copying it across.
#
proc pkgconf::is_empty_directory { dir } {

    ASSERT { $dir != "" }

    set contents [glob -nocomplain -- [file join $dir "*"]]
    if { [llength $contents] == 0 } {
	return 1
    }
    if { ([llength $contents] == 1) && [string match {*CVS} $contents] } {
	return 1
    }
    return 0
}

# ----------------------------------------------------------------------------
# Given a pathname such as kernel/current/include, this routine returns
# a string which points at the build tree's pkgconf directory
# complete with the appropriate number of ..'s in the pathname
#
proc pkgconf::get_dotdots_pkgconf { dir } {

    set result ".."

    while { 1} {
	set dir [file dirname $dir]
	if { ($dir == ".") || ($dir == ":") } {
	    break
	}
	set result [file join $result ".."]
    }

    set result [file join $result "pkgconf"]
    return $result
}

# ----------------------------------------------------------------------------
# Internally this script works using native path names. When outputting the
# makefile information it is necessary to output the paths in a format that
# is understood by GnuMake.
#
proc pkgconf::get_pathname_for_make { dir } {

    # First of all, handle directory separators. Under Unix this is a no-op.
    # Under Windows the current directory separator may be \ instead of /.
    # It is not entirely clear under what circumstances Tcl's "file nativename"
    # facility uses / vs. \, but converting all existing backslashes to
    # forward slashes should be harmless.
    if { $pkgconf::windows_host != 0 } {
	regsub -all -- {\\} $dir "/" dir
    }

    # Next cope with spaces in pathnames. GnuMake does not always cope
    # sensibly with these, even if backslash quoting is used - there
    # are known problems with VPATH and with the wildcard function. Under
    # Unix there is no simple solution, but it is unlikely that the problem
    # will arise in the first place. Under Windows it is quite likely that
    # the problem will arise, but it is possible to work around it
    # by using a short file name.
    
    set index [string first " " $dir]
    if { $index != -1 } {
	if { $pkgconf::windows_host != 0 } {

	    # This call will only work if the specified object already exists.
	    # This should not be a problem, as the pathnames are likely to
	    # be relative to the component repository, the build tree, or the
	    # install tree. The component repository must already exist. The
	    # build tree must already exist, since this call will only be
	    # invoked when generating makefiles inside the build tree. The
	    # install tree will have been created before pkgconf/pkgconf.mak
	    # is written out, and that is the only place that needs to refer
	    # to it.

	    set dir [file attribute $dir -shortname]
	} else {
	    fatal_error "Cannot cope with pathname \"$pathname\".\nPathnames with spaces are not supported on this platform."
	}
    }

    # Other characters that have special meaning include: []*?()$^|&#<>"'`;
    # At least, that is the list used within vmake. $ has to be quoted as $$,
    # the others should be quoted with a single backslash.
    regsub -all -- {\$} $dir {$$} dir
    foreach char { {\[} {\]} {\*} {\?} {\(} {\)} {\^} {\|} {\&} {\#} {\<} {\>} {\"} {\'} {\`} {\;} } {
	regsub -all -- $char $dir "\\$char" dir
    }

    # Now cope with Windows drive names.
    #
    # First, volume-relative filenames. If the directory string begins with
    # c:xxx, as opposed to c:/xxx, it is necessary to figure out what the
    # current directory is on drive c: and use this as prefix for the rest
    # of the string.
    #
    # Once volume-relative filenames have been handled the directory may be
    # along the lines of c:/Cygnus/eCos/whatever. This is problematical for
    # GnuMake since the colon character has special meaning in various places,
    # e.g. to identify static pattern rules. cygwin32 uses //c/ as an alias
    # for c:/ wherever appropriate.
    if { $pkgconf::windows_host != 0 } {

	set dummy  ""
	set match1 ""
	set match2 ""
	set match3 ""
	
	if { [regexp -- {^([a-zA-Z]:)(/)?(.*)} $dir dummy match1 match2 match3] == 1 } {
	    if { $match2 == "" } {
		set result [catch {
		    set current_dir [pwd]
		    cd $match1
		    set dir [pwd]
		    regsub -all -- {\\} $dir "/" dir
		    set dir [file join $dir $match3]
		    cd $current_dir
		} message]
  	        if { $result != 0 } {
		    fatal_error "Failed to determine current directory on drive $match1"
		}
	    }
	}

	if { [regexp -- {^([a-zA-Z]):} $dir dummy match1] == 1 } {
	    set dir "//$match1[string range $dir 2 end]"
	}
    }
    
    return $dir
}

# ----------------------------------------------------------------------------
# Take a cygwin32 filename such as //d/tmp/pkgobj and turn it into something
# acceptable to Tcl, i.e. d:/tmp/pkgobj. There are a few other complications...

proc pkgconf::get_pathname_for_tcl { name } {
    
    if { $pkgconf::windows_host } {
	set dummy  ""
	set match1 "" 
	set match2 ""

	# If the pathname is relative to ~, expand it now.
	if { [regexp -- {^~(.*)$} $name dummy match1] } {
	    set name [file nativename $name]
	}
	
	# For consistency, get rid of any backslashes in the pathname.
	# Especially since the file nativename command above may have
	# introduced some.
	regsub -all -- {\\} $name "/" name

	# And now replace //d/tmp/pkgobj with d:/tmp/pkgobj
	if { [regexp --  {^//([a-zA-Z])(/.*)$} $name dummy match1 match2] == 1 } {
	    set name "$match1:$match2"
	}

	# If the pathname is relative to the cygwin root, get hold of the
	# drive name.
	if { ([regexp -- {^/(.*)$} $name dummy match1] == 1) && ([regexp -- {^//.*$} $name dummy] == 0) } {
	    set saved_dir [pwd]
	    cd /
	    set name [file join [pwd] $match1]
	    cd $saved_dir
	}
    }
    return $name
}

# ----------------------------------------------------------------------------
# Output a standard banner to a generated file.
#

proc pkgconf::output_banner { file name } {

    puts $file \
"#
# File $name
#
# This file is generated automatically by the pkgconf program.
# It should not be edited. Any changes made to this file may
# be overwritten by pkgconf.
#"
}

# ----------------------------------------------------------------------------
# Make sure that a newly created or copied file is writable. This operation
# is platform-specific. Under Unix at most the current user is given
# permission, since there does not seem to be any easy way to get hold
# of the real umask.

proc pkgconf::make_writable { name } {

    ASSERT { $name != "" }
    ASSERT { [file isfile $name] }
    
    if { [file writable $name] == 0 } {
	if { $pkgconf::windows_host != 0 } {
	    file attributes $name -readonly 0
	} else {
	    set mask [file attributes $name -permissions]
	    set mask [expr $mask | 0200]
	    file attributes $name -permissions $mask
	}
    }
}

# }}}
# {{{  Other utilities                  

# ----------------------------------------------------------------------------
# This code works out what packages should go into the current configuration.
# The rules are as follows:
#
# 1) all hardware-specific packages should be disabled, unless they are
#    mentioned explicitly in the targets file for the current target or
#    platform, or unless they are enabled explicitly by the user.
#
# 2) all other packages which are enabled by default should be included,
#    unless they are disabled explicitly by the user.
#
# 3) all non-hardware packages which are disabled by default should be
#    left out, unless they are enabled explicitly by the user.

proc pkgconf::get_current_packages { } {

    ASSERT { $pkgconf::config_data(target)   != "" }
    ASSERT { $pkgconf::config_data(platform) != "" }
    ASSERT { [info exists pkgconf::target_data($pkgconf::config_data(target),packages)] }
    ASSERT { [info exists pkgconf::target_data($pkgconf::config_data(target),$pkgconf::config_data(platform),packages)] }
    ASSERT { [info exists pkgconf::config_data(enabled_packages)] }
    ASSERT { [info exists pkgconf::config_data(disabled_packages)] }
    
    set result ""
    set target_packages   $pkgconf::target_data($pkgconf::config_data(target),packages)
    set platform_packages $pkgconf::target_data($pkgconf::config_data(target),$pkgconf::config_data(platform),packages)
    set enabled_packages  $pkgconf::config_data(enabled_packages)
    set disabled_packages $pkgconf::config_data(disabled_packages)
    
    foreach pkg $pkgconf::known_packages {

	ASSERT { [info exists pkgconf::package_data($pkg,hardware)] }
	
	if { $pkgconf::package_data($pkg,hardware) } {

	    if { [lsearch -exact $disabled_packages $pkg] != -1 } {
		continue
	    } elseif { [lsearch -exact $target_packages $pkg] != -1 } {
		lappend result $pkg
	    } elseif { [lsearch -exact $platform_packages $pkg] != -1 } {
		lappend result $pkg
	    } elseif { [lsearch -exact $enabled_packages $pkg] != -1 } {
		lappend result $pkg
	    }

	} elseif { $pkgconf::package_data($pkg,default) != 0 } {
	    if { [lsearch -exact $disabled_packages $pkg] == -1 } {
		lappend result $pkg
	    }
	} else {
	    if { [lsearch -exact $enabled_packages $pkg] != -1 } {
		lappend result $pkg
	    }
	}
    }

    return $result
}

# ----------------------------------------------------------------------------
# A utility to determine whether or not a particular setting has changed relative to
# what is currently in the build tree. Information about the latter comes from
# the save file, but may not exist the first time that the build tree is generated.
# The config_data array itself may be sparsely populated, especially when it comes
# to compiler flags.

proc pkgconf::value_has_changed { name } {

    ASSERT {$name != ""}

    if { [info exists pkgconf::config_data($name)] == 0 } {

	if { [info exists pkgconf::saved_data($name)] == 0 } {
	    return 0
	} else {
	    return 1
	}
	
    } else {

	if { [info exists pkgconf::saved_data($name)] == 0 } {
	    return 1
	}
	if { $pkgconf::saved_data($name) != $pkgconf::config_data($name) } {
	    return 1
	}
	return 0
    }
}

# ----------------------------------------------------------------------------
# Has there been a change in the packages in the system, either which packages
# are present or any of their versions ? If there has been a change in the
# packages then the amount of work that has to be done inside pkgconf is
# increased significantly.

proc pkgconf::packages_have_changed { } {
    
    if { [value_has_changed "target"] || [value_has_changed "platform"] } {
	return 1
    }
    if { [value_has_changed "enabled_packages"] || [value_has_changed "disabled_packages"] } {
	return 1
    }
    foreach pkg $pkgconf::known_packages {
	if { [value_has_changed "$pkg,version"] } {
	    return 1
	}
    }

    return 0
}


# ----------------------------------------------------------------------------
# Given the name of a package, e.g. CYGPKG_KERNEL, return something which is
# more acceptable in the build tree. Creating directories and files with
# names containing lots of upper case characters is unacceptable.

proc pkgconf::get_build_alias { name } {

    ASSERT { $name != ""}

    set index [string first "_" $name] 
    if { $index != -1 } {
	set new_name [string range $name [incr index] end]
	if { $new_name != "" } {
	    set name $new_name
	}
    }
    set name [string tolower $name]
    return $name
}

# }}}
# {{{  Process a package                

# ----------------------------------------------------------------------------
# Process an entire package. The header files, the src directory, and the
# tests directory are all handled separately.

proc pkgconf::process_package { name dir } {

    report "Processing package $dir"
    
    set incdir [file join $dir "include"]
    if { [file isdir [file join $pkgconf::component_repository $incdir]] } {
	lappend pkgconf::inc_dirs $incdir
	process_package_includes $name $incdir
    }

    set srcdir [file join $dir "src"]
    if { [file isdir [file join $pkgconf::component_repository $srcdir]] } {
	if { [file isfile [file join $pkgconf::component_repository $srcdir PKGconf.mak]] } {
	    lappend pkgconf::src_dirs $srcdir
	    process_package_src $srcdir
	}
    }

    set testsdir [file join $dir "tests"]
    if { [file isdir [file join $pkgconf::component_repository $testsdir]] } {
	if { [file isfile [file join $pkgconf::component_repository $testsdir PKGconf.mak]] } {
	    lappend pkgconf::tests_dirs $testsdir
	    process_package_tests $name $testsdir
	} 
    }

    set miscdir [file join $dir "misc"]
    if { [file isdir [file join $pkgconf::component_repository $miscdir]] } {
	if { [file isfile [file join $pkgconf::component_repository $miscdir PKGconf.mak]] } {
	    lappend pkgconf::misc_dirs $miscdir
	    process_package_misc $miscdir
	} 
    }
}

# ----------------------------------------------------------------------------
# Processing the header files is somewhat complicated. The include directories
# and any subdirectories except pkgconf have to be created in the build tree.
# Details of these subdirectories get stored in the variable header_subdirs.
# A pro-forma makefile has to be generated in the build tree, using details
# of all the header files. These header files must also be appended to the
# variable header_files. Any files in pkgconf have to be copied across
# to the pkgconf directory, but must not overwrite the files that are
# already there.
#
# As an additional complication, each package may define its own header
# file directory. For example a package xyz may specify that all of its
# exported header files should go into the xyz subdirectory of the install
# include tree, not at the top level. There are special cases for the HAL.
#
# Note that if there are any spurious directories already present, these
# do not get deleted. However the generated makefile will not reference
# them so it should be harmless.
#

proc pkgconf::process_package_includes { name dir } {

    # What subdirectory in the install tree should be used ?
    ASSERT { [info exists pkgconf::package_data($name,incdir)] }
    set pkg_incdir $pkgconf::package_data($name,incdir)

    # Get information about the package's include directory.
    set pkg_inc_dirs      [locate_all_subdirs [file join $pkgconf::component_repository $dir]]
    set pkg_pkgconf_files [locate_files       [file join $pkgconf::component_repository $dir "pkgconf"]]
    set pkg_inc_files     [locate_all_files   [file join $pkgconf::component_repository $dir]]

    # Remove all pkgconf headers from the all_headers list. pkgconf headers
    # are handled by a separate makefile in the pkgconf directory.
    foreach hdr $pkg_pkgconf_files {
	set index [lsearch -exact $pkg_inc_files [file join pkgconf $hdr]]
	ASSERT { $index != -1 }
	set pkg_inc_files [lreplace $pkg_inc_files $index $index]
    }

    # Remove all unwanted subdirectories. These include pkgconf (which is handled
    # specially) and any empty subdirectories.
    set tmp ""
    foreach subdir $pkg_inc_dirs {
	if { ($subdir == "pkgconf") } {
	    continue
	}
	if { [is_empty_directory [file join $pkgconf::component_repository $dir $subdir] ] } {
	    continue
	}
	lappend tmp $subdir
    }
    set pkg_inc_dirs $tmp
    
    # Create the include directory in the build tree, plus any subdirectories.
    # This directory tree must map the structure of the source tree, so there is
    # no need to worry about thispkg_incdir.
    file mkdir [file join $pkgconf::build_tree $dir]
    foreach subdir $pkg_inc_dirs {
	file mkdir [file join $pkgconf::build_tree $dir $subdir]
    }

    # Copy across the pkgconf files that are not already present.
    # Also keep track of this information globally.
    foreach hdr $pkg_pkgconf_files {
	if { [lsearch -exact $pkgconf::pkgconf_files $hdr] != -1 } {
	    return -code error "Duplicate configuration header file $hdr"
	}
	lappend pkgconf::pkgconf_files $hdr
	
	set src  [file join $pkgconf::component_repository $dir "pkgconf" $hdr]
	set dest [file join $pkgconf::build_tree "pkgconf" $hdr]
	if { (! [file isfile $dest]) || ($pkgconf::force_arg != 0) } {
	    file copy -force -- $src $dest
	    make_writable $dest
	}
    }

    # Keep track of all the subdirectories that should be generated
    # in the install tree. Note that packages may share these
    # subdirectories, e.g. the sys subdirectory will be used by
    # both the C library and by a POSIX compatibily layer.
    #
    # It is necessary to list pkg_incdir separately, and all
    # of its parent directories. Otherwise it is not possible to
    # clean up the install tree correctly when packages get removed.
    if { $pkg_incdir != "" } {
	set current_path ""
	foreach field [file split $pkg_incdir] {
	    set current_path [file join $current_path $field]
	    if { [lsearch -exact $pkgconf::install_inc_dirs $current_path] == -1 } {
		lappend pkgconf::install_inc_dirs $current_path
	    }
	}
    }
    foreach subdir $pkg_inc_dirs {
	set dest [file join $pkg_incdir $subdir]
	if { [lsearch -exact $pkgconf::install_inc_dirs $dest] == -1 } {
	    lappend pkgconf::install_inc_dirs $dest
	}
    }
    
    # Keep track of all header files, checking for duplicates.
    foreach hdr $pkg_inc_files {
	set dest [file join $pkg_incdir $hdr]
	if { [lsearch -exact $pkgconf::install_inc_files $dest] != -1 } {
	    return -code error "Duplicate header file $dest" 
	}
	lappend pkgconf::install_inc_files $dest
    }

    # Generate the makefile in the target directory. First set up
    # some variables to contain the right sort of data for a makefile.
    if { $pkg_incdir == "" } {
	set pkg_incdir "."
    } else {
	set pkg_incdir [get_pathname_for_make $pkg_incdir]
    }
    set tmp ""
    foreach file $pkg_inc_files {
	lappend tmp [get_pathname_for_make $file]
    }
    set pkg_inc_files $tmp

    set filename [file join $dir "makefile"]
    set makefile [open [file join $pkgconf::build_tree $filename] "w"]
    output_banner $makefile $filename
    puts $makefile \
"
include [get_pathname_for_make [file join [get_dotdots_pkgconf $dir] "pkgconf.mak"]]

include [get_pathname_for_make [file join [get_dotdots_pkgconf $dir] "system.mak"]]

HEADERS := $pkg_inc_files
TARGETS := \$(foreach hdr,\$(HEADERS),\$(PREFIX)/include/$pkg_incdir/\$(hdr))
.PHONY  : build clean
VPATH   := . [file join \$(COMPONENT_REPOSITORY) $dir]

build: \$(TARGETS)

"
    foreach hdr $pkg_inc_files {
	puts $makefile "\$(PREFIX)/include/$pkg_incdir/$hdr : $hdr"
	if { $pkgconf::config_data(use_links) == 0 } {
	    puts $makefile "\t@\$(CP) \$< \$@"
	} else {
	    puts $makefile "\t@\$(RM) \$@; ln -s \$< \$@"
	}
	puts $makefile ""
    }
    
    puts $makefile "clean:"
    foreach hdr $pkg_inc_files {
	puts $makefile "\t@\$(RM) \$(PREFIX)/include/$pkg_incdir/$hdr"
    }
    puts $makefile "\n# End of $filename"
    close $makefile
}

# ----------------------------------------------------------------------------
# Processing the src directory is relatively straightforward. It is
# necessary to create the src directory in the build tree, plus any
# subdirectories. It is also necessary to copy across the PKGconf.mak
# file from the component repository to the build tree, assuming the
# destination file does not already exist, and do the same in any
# subdirectories.
#
proc pkgconf::process_package_src { dir } {

    set srcdir  [file join $pkgconf::component_repository $dir]
    set destdir [file join $pkgconf::build_tree $dir]
    
    file mkdir $destdir
    set subdirs [locate_all_subdirs $srcdir]
    foreach subdir $subdirs {
	if { [is_empty_directory [file join $srcdir $subdir] ] } {
	    continue
	}
	file mkdir [file join $destdir $subdir]
    }

    set makefiles [locate_all_files $srcdir "PKGconf.mak"]
    foreach makefile $makefiles {
	set destname [file join $destdir [file dirname $makefile] "makefile"]

	if { ([file isfile $destname] == 0) || $pkgconf::force_arg } {
	    file copy -force -- [file join $srcdir $makefile] $destname
	    make_writable $destname
	}
    }
}

# ----------------------------------------------------------------------------
# Processing the tests directories is just the same as the source
# directories, for now. There is one additional complication in that
# the test executables have to get installed, so it is necessary to
# keep track of which directories are needed in the install tree.

proc pkgconf::process_package_tests { name dir } {
    
    set srcdir  [file join $pkgconf::component_repository $dir]
    set destdir [file join $pkgconf::build_tree $dir]

    lappend pkgconf::install_test_dirs [get_build_alias $name]
    
    file mkdir $destdir
    set subdirs [locate_all_subdirs $srcdir]

    foreach subdir $subdirs {
	lappend pkgconf::install_test_dirs [file join [get_build_alias $name] $subdir]
	file mkdir [file join $destdir $subdir]
    }

    set makefiles [locate_all_files $srcdir "PKGconf.mak"]
    foreach makefile $makefiles {
	set destname [file join $destdir [file dirname $makefile] "makefile"]
	if { ([file isfile $destname] == 0) || $pkgconf::force_arg } {
	    file copy -force -- [file join $srcdir $makefile] $destname
	    make_writable $destname
	}
    }
}

# ----------------------------------------------------------------------------
# Processing the misc directory is just like the src directory.
#
proc pkgconf::process_package_misc { dir } {

    set miscdir  [file join $pkgconf::component_repository $dir]
    set destdir [file join $pkgconf::build_tree $dir]
    
    file mkdir $destdir
    set subdirs [locate_all_subdirs $miscdir]
    foreach subdir $subdirs {
	if { [is_empty_directory [file join $miscdir $subdir] ] } {
	    continue
	}
	file mkdir [file join $destdir $subdir]
    }

    set makefiles [locate_all_files $miscdir "PKGconf.mak"]
    foreach makefile $makefiles {
	set destname [file join $destdir [file dirname $makefile] "makefile"]

	if { ([file isfile $destname] == 0) || $pkgconf::force_arg } {
	    file copy -force -- [file join $miscdir $makefile] $destname
	    make_writable $destname
	}
    }
}

# }}}
# {{{  Package debug options            

# ----------------------------------------------------------------------
# Enable appropriate debug options in various packages. Currently the
# only option that is affected is CYGPKG_INFRA_DEBUG in pkgconf/infra.h

proc pkgconf::enable_package_debug_options { } {

    report "Enabling debug-related options"

    set fd [open [file join $pkgconf::build_tree "pkgconf" "infra.h"] "r"]
    set data [read $fd]
    close $fd

    if { [regexp -- {undef[ ]+CYGPKG_INFRA_DEBUG} $data] } {
	regsub -- {undef[ ]+CYGPKG_INFRA_DEBUG} $data {define CYGPKG_INFRA_DEBUG} data
	set fd [open [file join $pkgconf::build_tree "pkgconf" "infra.h"] "w"]
	puts $fd $data
	close $fd
    }
}

# Or the inverse.
proc pkgconf::disable_package_debug_options { } {

    report "Disabling debug-related options"

    set fd [open [file join $pkgconf::build_tree "pkgconf" "infra.h"] "r"]
    set data [read $fd]
    close $fd

    if { [regexp -- {define[ ]+CYGPKG_INFRA_DEBUG} $data] } {
	regsub -- {define[ ]+CYGPKG_INFRA_DEBUG} $data {undef CYGPKG_INFRA_DEBUG} data
	set fd [open [file join $pkgconf::build_tree "pkgconf" "infra.h"] "w"]
	puts $fd $data
	close $fd
    }
}

# }}}
# {{{  Install tree                     

# ----------------------------------------------------------------------------
# These routines take care of the install tree. The routine
# produce_install_tree gets invoked after details of all the packages
# are known, and hence after details of all the subdirectories
# are known.
#
# Currently this routine will empty out the lib and tests directory
# completely. It will also get rid of any files and subdirectories in
# include which are no longer of interest.
#
# Clearing out libtarget.a and any similar libraries avoids problems
# if the target architecture has changed. Such a change should cause
# all source files to be rebuilt via a dependency on the makefile
# auxiliary files. However it would result in an attempt to create
# a library archive containing a mixture of architectures, which could
# cause all sorts of problems. To make sure that the library gets
# regenerated during the next build all the stamp files have to be
# deleted.
#
# It is not a good idea to manipulate header files unnecessarily,
# since changing these  would result in application code being
# recompiled as well as system code. However if the old and new
# configurations involve a header file with the same name but coming
# from different packages, (e.g. a standard HAL header if you switch
# targets or platforms, or a package header if you change the version
# of that package) then there are complications.
#
# For now the code is draconian and just deletes everything. This
# needs to be optimised.
#

proc pkgconf::produce_install_tree { } {

    ASSERT { $pkgconf::build_tree != "" }

    if { $pkgconf::config_data(prefix) != "" } {
	set prefix $pkgconf::config_data(prefix)
    } else {
	set prefix [file join $pkgconf::build_tree "install"]
    }
    ASSERT { $prefix != "" }

    # There should be at least an include/pkgconf subdirectory
    ASSERT { $pkgconf::install_inc_dirs != "" }
    
    # In Tcl the mkdir call is a no-op if the directory already exists.
    report "Generating install directory $prefix"
    file mkdir $prefix
    file mkdir [file join $prefix "include"]
    file mkdir [file join $prefix "lib"]
    file mkdir [file join $prefix "tests"]

    foreach subdir $pkgconf::install_inc_dirs {
	file mkdir [file join $prefix "include" $subdir]
    }
    foreach subdir $pkgconf::install_test_dirs {
	file mkdir [file join $prefix "tests" $subdir]
    }

    if { 0 } {
	
	# BLV - this code does not work with the makefiles that are now
	# generated for the various include directories. If the old
	# configuration involved a header file xyz.h from package A and
	# the new configuration involves a header file xyz.h from a
	# different package (or a different version of the same package)
	# then the old header file would hang around. A temporary
	# work-around is to delete all header files. A proper fix is
	# rather more complicated.
	
	# Examine all the files currently in the include directory. If
	# any of them are not recognised then they should be deleted.
	# Typically this happens if a package is removed.
	#
	# There may be problems if users put the install tree under
	# source code control, in that files just disappear. However even if
	# the source code control system restores an old file this should be
	# harmless. The directory routines ignore CVS subdirectories and all
	# hidden files.
	#
	# Files in pkgconf have to be treated specially. These are listed
	# in pkgconf_files, not header_files.
    
	set existing_headers [locate_all_files [file join $prefix "include"]]
	set pkgconf_headers  [locate_all_files [file join $prefix "include" pkgconf]]

	foreach file $pkgconf_headers {
	    if { [lsearch -exact $pkgconf::pkgconf_files $file] == -1 } {
		set filename [file join $prefix "include" $file]
		report "Deleting unnecessary file $filename"
		file delete -- $filename
	    }

	    # Make sure that this file does not get processed again in the
	    # next loop.
	    set file  [file join "pkgconf" $file]
	    set index [lsearch -exact $existing_headers $file]
	    ASSERT { $index != -1 }
	    set existing_headers [lreplace $existing_headers $index $index]
	}
    
	foreach file $existing_headers {
	    if { [lsearch -exact $pkgconf::install_inc_files $file] == -1 } {
		set filename  [file join $prefix "include" $file]
		report "Deleting unnecessary file $filename"
		file delete -- $filename
	    }
	}

	# If the user has switched from symbolic links to file copies then
	# it is also necessary to delete the old symbolic links. The reverse
	# also applies.
	set existing_headers [locate_all_files [file join $prefix "include"]]
	set files_removed 0
	foreach file $existing_headers {
	
	    set filename [file join $prefix "include" $file]
	    set type [file type $filename]
	    
	    if { ($pkgconf::config_data(use_links) == 0 ) && ($type == "link") } {
		file delete -- $filename
		set files_removed 1
	    } elseif { ($pkgconf::config_data(use_links) == 1 ) && ($type == "file") } {
		file delete -- $filename
		set files_removed 1
	    }
	}
	if { $files_removed != 0 } {
	    if { $pkgconf::config_data(use_links) == 0 } {
		report "Symbolic links in [file join $prefix include] deleted."
	    } else {
		report "Header file copies in [file join $prefix include] deleted."
	    }
	}
    
	# Now get rid of any unwanted subdirectories.
	set existing_subdirs [locate_all_subdirs [file join $prefix "include"]]
	foreach subdir $existing_subdirs {
	    if { [lsearch -exact $pkgconf::install_inc_dirs $subdir] == -1 } {
		set dirname [file join $prefix "include" $subdir]
		report "Deleting unnecessary directory $dirname"
		file delete -force -- $dirname
	    }
	}
	
    } else {

	# BLV - delete all header files for now. This is inefficient but safe.
	
	set existing_headers [locate_all_files [file join $prefix "include"]]
	foreach file $existing_headers {
	    set filename [file join $prefix "include" $file]
	    file delete -- $filename
	}
	
	# Now get rid of any unwanted subdirectories.
	set existing_subdirs [locate_all_subdirs [file join $prefix "include"]]
	foreach subdir $existing_subdirs {
	    if { [lsearch -exact $pkgconf::install_inc_dirs $subdir] == -1 } {
		set dirname [file join $prefix "include" $subdir]
		report "Deleting unnecessary directory $dirname"
		file delete -force -- $dirname
	    }
	}
    }

    # Get rid of all the files in lib.
    set lib_files [locate_files [file join $prefix "lib"]]
    if { $lib_files != "" } {
	report "Deleting old library files $lib_files"
	foreach file $lib_files {
	    file delete -- [file join $prefix "lib" $file]
	}
    }

    # And get rid of any stamp files spread around the object tree
    foreach src_dir $pkgconf::src_dirs {
	set dirname [file join $pkgconf::build_tree $src_dir]
	set stamp_files [locate_files $dirname "*.stamp"]
	foreach file $stamp_files {
	    file delete -- [file join $dirname $file]
	}
    }
    
    # Ditto for tests
    set test_files [locate_all_files [file join $prefix "tests"]]
    if { $test_files != "" } {
	report "Deleting old test executables."
	foreach file $test_files {
	    file delete -- [file join $prefix "tests" $file]
	}
    }
    foreach test_dir $pkgconf::tests_dirs {
	set dirname [file join $pkgconf::build_tree $test_dir]
	set stamp_files [locate_files $dirname "*.stamp"]
	foreach file $stamp_files {
	    file delete -- [file join $dirname $file]
	}
    }

    # If there has been a change in the packages in the system
    # then the various makefile.deps files currently in the
    # build tree may contain references to header files that
    # are no longer present. Therefore it is necessary to
    # get rid of the makefile.deps files. Any changes to the
    # packages will result in a new pkgconf/pkgconf.mak file
    # and all object files depend on this, so everything that
    # needs to be rebuilt will still be rebuilt and new
    # dependency files will be generated.
    #
    # If there has not been a change to the packages then the
    # makefile.deps files must not be deleted, or there would
    # be problems when just a header file is changed.
    if { [packages_have_changed] } {

	foreach src_dir $pkgconf::src_dirs {
	    set dirname [file join $pkgconf::build_tree $src_dir]
	    set dep_files [locate_files $dirname "makefile.deps"]
	    foreach file $dep_files {
		file delete -- [file join $dirname $file]
	    }
	}
    
	foreach test_dir $pkgconf::tests_dirs {
	    set dirname [file join $pkgconf::build_tree $test_dir]
	    set dep_files [locate_files $dirname "makefile.deps"]
	    foreach file $dep_files {
		file delete -- [file join $dirname $file]
	    }
	}
    }
}

# }}}
# {{{  Produce misc. files              

# ----------------------------------------------------------------------------
# Three files have to be generated, and one additional file can be copied.
#
# 1) pkgconf/makevars has to be copied from the component repository.
# 2) pkgconf/makevars relies on pkgconf/pkgconf.mak, which is generated
# 3) pkgconf/system.h has to be generated.
# 5) pkgconf/makefile has to be generated.
#

proc pkgconf::produce_misc_files { } {

    report "Generating miscellaneous files."

    # Start by copying across pkgconf/makevars unless these files
    # already exist in the build tree. Note that these files are
    # user-editable so they cannot be overwritten automatically.
    set destname   [file join $pkgconf::build_tree "pkgconf" "makevars"]
    set sourcename [file join $pkgconf::component_repository "pkgconf" "makevars"]
    if { (![file isfile $destname]) || ($pkgconf::force_arg != 0) } {
	file copy -force -- $sourcename $destname
	make_writable $destname
    }

    # Next generate the file pkgconf.mak. This should only happen if any
    # of the data in the file is changed, because there are file-level
    # dependencies in the various makefiles. Additions to the packages
    # file will normally be caught by a missing version number. Other
    # changes to the packages or targets file may not be caught.

    set config_changed 0
    if { [packages_have_changed] } {
	set config_changed 1
    }
    if { [value_has_changed "prefix"] } {
	set config_changed 1
    }
    foreach flag $pkgconf::known_compiler_flags {
	if { [value_has_changed "cflags,$flag"] } {
	    set config_changed 1
	    break
	}
    }
    if { ![file isfile [file join $pkgconf::build_tree "pkgconf" "pkgconf.mak"]] } {
	set config_changed 1
    }
    if { $pkgconf::force_arg != 0 } {
	set config_changed 1
    }
    
    if { $config_changed != 0 } {

	set filename [file join "pkgconf" "pkgconf.mak"]
	set file [open [file join $pkgconf::build_tree $filename] "w"]
	output_banner $file $filename

	# All PKGconf.mak files should include the master pkgconf.mak
	# file ASAP. It is desirable to have a goal at the top of that
	# file. This is likely to be the first goal detected by make,
	# and hence the default.
	puts $file "\ndefault: build\n"
	
	set platform $pkgconf::config_data(platform)
	if [info exists pkgconf::target_data($pkgconf::config_data(target),$platform,prefix)] {
	    set command_prefix $pkgconf::target_data($pkgconf::config_data(target),$platform,prefix)
	} else {
	    set command_prefix $pkgconf::target_data($pkgconf::config_data(target),prefix)
	}

	puts $file ""
	puts $file "COMPONENT_REPOSITORY\t\t:= [get_pathname_for_make $pkgconf::component_repository]"
	puts $file "BUILD_TREE\t\t\t:= [get_pathname_for_make $pkgconf::build_tree]"

	# Provide details of target etc. so that packages can specify
	# per-target files.
	puts $file "TARGET\t\t\t\t:= $pkgconf::config_data(target)"
	puts $file "PLATFORM\t\t\t:= $pkgconf::config_data(platform)"
	puts $file "BASE_PLATFORM\t\t\t:= $pkgconf::config_data(base_platform)"
	puts $file "STARTUP\t\t\t\t:= $pkgconf::config_data(startup)"
	
	# The prefix (i.e. install directory) should default to somewhere
	# relative to the build tree.
	set prefix $pkgconf::config_data(prefix)
	if { $prefix == "" } {
	    set prefix [file join $pkgconf::build_tree "install"]
	}
	puts $file "PREFIX\t\t\t\t:= [get_pathname_for_make $prefix]"
	if { $pkgconf::windows_host != 0 } {
	    puts $file "EXEEXT\t\t\t\t:= .exe"
	}
	puts $file ""
	puts $file "PKGCONF_CC\t\t\t:= $command_prefix-gcc"
	puts $file "PKGCONF_CXX\t\t\t:= $command_prefix-gcc"
	puts $file "PKGCONF_AR\t\t\t:= $command_prefix-ar"
	puts $file "PKGCONF_LD\t\t\t:= $command_prefix-ld"
	puts $file "PKGCONF_OBJCOPY\t\t\t:= $command_prefix-objcopy"
	puts $file ""

	foreach flag $pkgconf::known_compiler_flags {
	    set value ""
	    if { [info exists pkgconf::config_data(cflags,$flag)] != 0 } {
		set value $pkgconf::config_data(cflags,$flag)
	    } else {
		set value [get_platform_cflags $pkgconf::config_data(target) $pkgconf::config_data(platform) $flag]
	    }
	    if { $value != "" } {
		puts $file "PKGCONF_[set flag]\t\t:= $value"
	    }
	}
	puts $file ""

	foreach pkg [get_current_packages] {
	    set varname "[get_build_alias $pkg]_DIR"
	    set dirname [file join $pkgconf::package_data($pkg,dir) $pkgconf::config_data($pkg,version)]
	    puts $file "$varname\t\t:= $dirname"
	}
	
	# There are several files containing information about make variables
	# rules. The current file, pkgconf.mak, contains information that
	# is known to the pkgconf script and should not be edited by users.
	# There may be a per-package file in the build tree's pkgconf
	# directory, containing per-package settings. And there is the
	# makevars file which users can edit to override anything else.
	#
	# The makefiles for the various packages should only
	# need to include one of these files, the rest can be handled
	# automatically. Since pkgconf.mak is generated it knows the
	# location of the build tree, making it a lot easier to find
	# the others. Therefore pkgconf.mak is included first, then
	# the per-package file, and finally the makevars file.

	puts $file "
PACKAGE_RULES_FILE := \$(wildcard \$(BUILD_TREE)/pkgconf/\$(PACKAGE).mak)
ifneq ($(PACKAGE_RULES_FILE),)
include $(PACKAGE_RULES_FILE)
endif"

	puts $file "\ninclude \$(BUILD_TREE)/pkgconf/makevars"

	puts $file "\n# End of $filename"
	close $file
    }
    
    # And the file system.h
    #
    # Again, because of file-level dependencies this file should only
    # be written out if anything has actually changed.
    set config_changed 0
    if { [packages_have_changed] || [value_has_changed "startup"] } {
	set config_changed 1
    }
    if { ![file isfile [file join $pkgconf::build_tree "pkgconf" "system.h"]] } {
	set config_changed 1
    }
    if { $pkgconf::force_arg != 0 } {
	set config_changed 1
    }

    if { $config_changed != 0 } {

	set filename [file join "pkgconf" "system.h"]
	set file [open [file join $pkgconf::build_tree $filename] "w"]
	puts $file \
"#ifndef CYGONCE_PKGCONF_SYSTEM_H
#define CYGONCE_PKGCONF_SYSTEM_H
/*
 * File $filename
 *
 * This file is generated automatically by the pkgconf program.
 * It should not be edited. Any changes to this file may be
 * overwritten by pkgconf.
 */
"
        puts $file "#define CYG_HAL_[string toupper $pkgconf::config_data(target)]"
        puts $file \
	    "#define CYG_HAL_[string toupper $pkgconf::config_data(target)]_[string toupper $pkgconf::config_data(platform)]"
        puts $file "#define CYG_HAL_STARTUP_[string toupper $pkgconf::config_data(startup)]"
        puts $file ""
        set current_packages [get_current_packages]
        foreach pkg $pkgconf::known_packages {
	    if { [lsearch -exact $current_packages $pkg] == -1 } {
		puts $file "#undef  $pkg"
	    } else {
		puts $file "#define $pkg"
	    }
	}

	# Additions for the MLT
	puts $file "\n"
	puts $file "#define CYGHWR_MEMORY_LAYOUT_LDI <pkgconf/mlt_[set pkgconf::config_data(target)]_[set pkgconf::config_data(base_platform)]_[set pkgconf::config_data(startup)].ldi>"
	puts $file "#define CYGHWR_MEMORY_LAYOUT_H   <pkgconf/mlt_[set pkgconf::config_data(target)]_[set pkgconf::config_data(base_platform)]_[set pkgconf::config_data(startup)].h>"

	# Also output details of the HAL header files that should be included
	puts $file "\n"
	puts $file "#define CYGBLD_HAL_TARGET_H   <pkgconf/hal_[set pkgconf::config_data(target)].h>"
	puts $file "#define CYGBLD_HAL_PLATFORM_H <pkgconf/hal_[set pkgconf::config_data(target)]_[set pkgconf::config_data(base_platform)].h>"
	
	puts $file "\
\n#endif  /* CYGONCE_PKGCONF_SYSTEM_H */ 		
/* EOF $filename */"
	close $file
    }

    # 'system.mak' has the same basic information, but in a form
    # that can be used by makefile fragments.

    if { $config_changed != 0 } {

	set filename [file join "pkgconf" "system.mak"]
	set file [open [file join $pkgconf::build_tree $filename] "w"]
	puts $file \
"#
# File $filename
#
# This file is generated automatically by the pkgconf program.
# It should not be edited. Any changes to this file may be
# overwritten by pkgconf.
#
"
        puts $file "CYG_HAL_[string toupper $pkgconf::config_data(target)]=1"
        puts $file \
	    "CYG_HAL_[string toupper $pkgconf::config_data(target)]_[string toupper $pkgconf::config_data(platform)]=1"
        puts $file "CYG_HAL_STARTUP_[string toupper $pkgconf::config_data(startup)]=1"
        puts $file ""
        set current_packages [get_current_packages]
        foreach pkg $pkgconf::known_packages {
	    if { [lsearch -exact $current_packages $pkg] != -1 } {
		puts $file "$pkg=1"
	    }
	}
	puts $file "\n# /* EOF $filename */"
	close $file
    }

    # Always output a makefile in the pkgconf directory. This is harmless
    # because there should be no file-level dependencies on this file (just
    # like the toplevel makefile).
    set filename [file join "pkgconf" "makefile"]
    set makefile [open [file join $pkgconf::build_tree $filename] "w"]
    output_banner $makefile $filename

    puts $makefile \
"include pkgconf.mak

.PHONY  : build clean
HEADERS := $pkgconf::pkgconf_files
TARGETS := \$(foreach hdr,\$(HEADERS),\$(PREFIX)/include/pkgconf/\$(hdr))

build: \$(TARGETS)
"

    foreach hdr $pkgconf::pkgconf_files {
	puts $makefile "\$(PREFIX)/include/pkgconf/$hdr : $hdr"
	if { $pkgconf::config_data(use_links) == 0 } {
	    puts $makefile "\t@\$(CP) \$< \$@"
	} else {
	    puts $makefile "\t@\$(RM) \$@; ln -s \$< \$@"
	}
	puts $makefile ""
    }

    puts $makefile "clean:"
    foreach hdr $pkgconf::pkgconf_files {
	puts $makefile "\t@\$(RM) \$(PREFIX)/include/pkgconf/$hdr"
    }
    puts $makefile "\n# End of $filename"
    close $makefile
    
}

# }}}
# {{{  Produce the master makefile      

# ----------------------------------------------------------------------------
# This routine is responsible for generating the master makefile that
# exists at the top of a build tree. This makefile supports three main
# targets: default, tests and clean. There is a subsidiary target
# "headers" which takes care of the header files.
#

proc pkgconf::produce_makefile { } {

    report "Generating toplevel makefile."
    
    ASSERT { $pkgconf::build_tree != "" }
    set filename [file join $pkgconf::build_tree "makefile"]
    set makefile [open $filename "w"]
    output_banner $makefile "makefile"

    puts $makefile \
"
include pkgconf/pkgconf.mak
.PHONY: default build clean tests headers"

    puts $makefile "\nbuild: $(PREFIX)/lib/extras.o"
    puts $makefile "\t@echo Build finished."

    puts $makefile "\n$(PREFIX)/lib/extras.o: $(PREFIX)/lib/libextras.a"
    puts $makefile "\t$(CC) $(ARCHFLAGS) $(LDARCHFLAGS) -nostdlib -Wl,-r -Wl,--whole-archive $(PREFIX)/lib/libextras.a -o $(PREFIX)/lib/extras.o"
    
    puts $makefile "\n$(PREFIX)/lib/libextras.a: headers"
    foreach src_dir $pkgconf::src_dirs {
	puts $makefile "\t\$(MAKE) -C $src_dir"
    }
    
    puts $makefile "\ntests: build"
    foreach tests_dir $pkgconf::tests_dirs {
	puts $makefile "\t\$(MAKE) -C $tests_dir"
    }
    puts $makefile "\t@echo Tests build finished."
    
    puts $makefile "\nmisc: build"
    foreach misc_dir $pkgconf::misc_dirs {
	puts $makefile "\t\$(MAKE) -C $misc_dir"
    }
    puts $makefile "\t@echo Misc build finished."
    
    puts $makefile \
"\nlibobjs := \$(wildcard \$(PREFIX)/lib/*)
clean: 
ifneq (\$(libobjs),)
\t\$(RM) \$(libobjs)
endif
\tmake -C pkgconf clean"
    foreach src_dir $pkgconf::src_dirs {
	puts $makefile "\t\$(MAKE) -C $src_dir clean"
    }
    foreach misc_dir $pkgconf::misc_dirs {
	puts $makefile "\t\$(MAKE) -C $misc_dir clean"
    }
    foreach tests_dir $pkgconf::tests_dirs {
	puts $makefile "\t\$(MAKE) -C $tests_dir clean"
    }
    foreach inc_dir $pkgconf::inc_dirs {
	puts $makefile "\t@\$(MAKE) -silent -C $inc_dir clean"
    }
    puts $makefile "\t@echo Clean finished."
    
    puts $makefile "\nheaders:"
    puts $makefile "\t@echo Checking header files."
    puts $makefile "\t@\$(MAKE) -silent -C pkgconf"
    foreach inc_dir $pkgconf::inc_dirs {
	puts $makefile "\t@\$(MAKE) -silent -C $inc_dir"
    }
    
    puts $makefile ""
    puts $makefile "# End of makefile"
    close $makefile
}

# }}}

# {{{  Produce_build_tree               

proc pkgconf::produce_build_tree { } {

    # First, check that all the requirements have been satisfied. Normally
    # this will have been taken care of earlier.
    check_requirements

    # What is the install tree ?
    set prefix $pkgconf::config_data(prefix)
    if { $prefix == "" } {
	set prefix [file join $pkgconf::build_tree "install"]
    }
    set target   $pkgconf::config_data(target)
    set platform $pkgconf::config_data(platform)
    set startup  $pkgconf::config_data(startup)
    
    # Report the configuration that is being built.
    report "Generating build tree."
    report "Component repository is $pkgconf::component_repository"
    report "Build tree is $pkgconf::build_tree"
    report "Install directory is $prefix"
    report "Architecture $target, platform $platform, startup $startup."
    if { $pkgconf::config_data(use_links) != 0 } {
	report "Using symbolic links for the header files."
    }
    
    # Reset the list of all the header files that should be in the install
    # tree, and related variables.
    set pkgconf::inc_dirs               ""
    set pkgconf::src_dirs               ""
    set pkgconf::misc_dirs              ""
    set pkgconf::tests_dirs             ""
    set pkgconf::install_inc_dirs       "pkgconf"
    set pkgconf::install_inc_files      ""
    set pkgconf::pkgconf_files          "makevars pkgconf.mak system.h"
    set pkgconf::install_test_dirs      ""
    
    set status [catch {

	# Create the toplevel build tree directory, if necessary.
	if { ! [file exists $pkgconf::build_tree] } {
	    report "Creating build tree $pkgconf::build_tree"
	    file mkdir $pkgconf::build_tree
	}
	# Create the pkgconf subdirectory. A failure here indicates that
	# the build tree is not writable, and it is useful to detect this
	# before too much processing takes place.
	set dirname [file join $pkgconf::build_tree pkgconf]
	if { ! [file exists $dirname] } {
	    report "Creating directory $dirname"
	    file mkdir $dirname
	}

	# Process each package that is part of the current configuration
	foreach pkg [get_current_packages] {
	    set dirname $pkgconf::package_data($pkg,dir)
	    set dirname [file join $dirname $pkgconf::config_data($pkg,version)]
	    process_package $pkg $dirname
	}

	# If this is a debug build (according to the --debug command line
	# option), update the appropriate packages.
	if {$pkgconf::debug_arg != 0} {
	    enable_package_debug_options
	} elseif {$pkgconf::nodebug_arg != 0} {
	    disable_package_debug_options
	}
	
	# The install tree must exist before the misc files are generated.
	# See get_pathname_for_make for details.
	produce_install_tree
	produce_misc_files
	produce_makefile
	
	report "Generating save file."
	write_save_file
	report "The Build tree is ready."
	
    } message]

    if { $status != 0 } {
	fatal_error "An unexpected error occurred while generating the build tree: $message"
    }
}

# }}}

# }}}
# {{{  main()                           

# ----------------------------------------------------------------------------
# Initialise data. This routine is responsible for reading in the
# packages file and the targets file, setting up the default data,
# reading in the save file if any, and processing the command line
# arguments if any.
#

proc pkgconf::initialise_data { } {
    
    # First, read the packages and targets files. These routines will
    # also deal with the --packages and --targets arguments, and
    # they will check for any invalid packages or targets that may
    # have been passed. The order of these two calls should not be
    # changed since read_targets needs to know about the available
    # packages for some of its validations.
    pkgconf::read_packages
    pkgconf::read_targets

    # If either or both of the --packages or --targets arguments
    # were used, do not go any further. The user was just asking
    # for information. Ditto for --pkgdata, which can be used by
    # other programs to get information about the various
    # packages in a format more appropriate for them.
    #
    # NOTE: the parsing code does not actually check that no
    # further arguments were passed, but it is unlikely that the
    # user would both ask for a list of packages and expect
    # something to happen.
    if { ( $pkgconf::list_packages_arg != 0 ) ||
         ( $pkgconf::list_targets_arg != 0 )  ||
         ( $pkgconf::list_pkgdata_arg != 0 ) } {
	exit 0
    }

    # Set up default values for all options which are managed directly
    # by this program.
    pkgconf::setup_defaults
    
    # Check for a save file in the existing build tree.
    # If so read it in, updating the default values.
    pkgconf::read_save_file

    # Now process the command line arguments, which will again result
    # in updates to the default values.
    pkgconf::process_arguments

    # If the -flags argument was used, list the compiler flags currently
    # in use. This could not be done earlier since the compiler flags
    # may depend on the save file and on other command line arguments.
    if { $pkgconf::list_flags_arg != 0 } {
	foreach flag $pkgconf::known_compiler_flags {
	    set value ""
	    if { [info exists pkgconf::config_data(cflags,$flag)] != 0 } {
		set value $pkgconf::config_data(cflags,$flag)
	    } else {
		set value [get_platform_cflags $pkgconf::config_data(target) $pkgconf::config_data(platform) $flag]
	    }
	    if { $value != "" } {
		puts "$flag\t\"$value\""
	    }
	}
	exit 0
    }
    
    if { 0 } {
	puts "target:   $pkgconf::config_data(target)"
	puts "platform: $pkgconf::config_data(platform)"
	puts "base_platform: $pkgconf::config_data(base_platform)"
	puts "startup:  $pkgconf::config_data(startup)"
	puts "prefix:   $pkgconf::config_data(prefix)"
	puts "packages: $pkgconf::config_data(packages)"
	foreach pkg $pkgconf::config_data(packages) {
	    puts "$pkg version $pkgconf::config_data($pkg,version)"
	}
	exit 0
    }
}

# ----------------------------------------------------------------------------
# Main(). This code only runs if the script is being run stand-alone rather
# than as part of a larger application. The controlling predicate is the
# existence of the variable pkgconf_not_standalone which can be set by
# the containing program if any.
#

if { ! [info exists pkgconf_not_standalone] } {

    # Do some necessary GUI initialisation such as hiding the main window
    # during startup.
    pkgconf::gui_standalone_init

    # Decide where warnings and fatal errors should go.
    pkgconf::initialise_error_handling
    
    # Parse the arguments and set the global variables appropriately.
    pkgconf::parse_arguments $argv0 $argv

    # Initialise all the data, e.g. by reading in the packages and
    # targets files.
    pkgconf::initialise_data
    
    # At this stage the code can do one of two things: run the GUI,
    # or build/update the build tree.
    # The GUI can be run if the necessary commands are available and
    # if the user did not pass -nw.
    if { $pkgconf::gui_possible && ! $pkgconf::no_windows_arg } {
    } else {
	set pkgconf::gui_mode 0
    }

    # Reset error handling to the correct mode
    pkgconf::initialise_error_handling

    # Either enter GUI mode, or produce the build tree
    if { $pkgconf::gui_mode } {
	pkgconf::gui
    } else {
	pkgconf::produce_build_tree
	exit 0
    }
}

# }}}


