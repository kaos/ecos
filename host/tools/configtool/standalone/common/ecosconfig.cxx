//####COPYRIGHTBEGIN####
//                                                                          
// ----------------------------------------------------------------------------
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.
//
// This program is part of the eCos host tools.
//
// This program is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
// more details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// ----------------------------------------------------------------------------
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//
//      ecosconfig.cxx
//
//      The implementation of ecosconfig command line processing
//
//==========================================================================
//==========================================================================
//#####DESCRIPTIONBEGIN####                                             
//
// Author(s):           jld
// Date:                1999-11-08
//
//####DESCRIPTIONEND####
//==========================================================================

#ifndef _MSC_VER
	#include <sys/param.h>
	#include <unistd.h> /* for realpath() */
#endif
#ifdef __CYGWIN__
	#include <sys/cygwin.h> /* for cygwin_conv_to_win32_path() */
#endif
#include "cdl_exec.hxx"
#include "ecosconfig.hxx"

#define TOOL_VERSION "1.3.2"
#define TOOL_COPYRIGHT "Copyright (c) 2000 Red Hat, Inc."
#define DEFAULT_SAVE_FILE "ecos.ecc"

int main (int argc, char * argv []) {

	// process command qualifiers
	std::string repository;     // --srcdir=
	std::string savefile;       // --config=
	std::string install_prefix; // --prefix=
	bool version = false;       // --version
	bool no_resolve = false;    // --no-resolve
	int command_index;
	for (command_index = 1; command_index < argc; command_index++) { // for each command line argument
		if (0 == strncmp (argv [command_index], "--srcdir=", 9)) {
			repository = & argv [command_index] [9];
		} else if (0 == strncmp (argv [command_index], "--config=", 9)) {
			savefile = & argv [command_index] [9];
		} else if (0 == strncmp (argv [command_index], "--prefix=", 9)) {
			install_prefix = & argv [command_index] [9];
		} else if (0 == strcmp (argv [command_index], "--version")) {
			version = true;
		} else if (0 == strcmp (argv [command_index], "--no-resolve")) {
			no_resolve = true;
		} else { // the argument is not a qualifier
			break; // end of qualifiers
		}
	}

	// usage message
	if (command_index == argc) { // if there is no command
		if (version) { // if the tool version was requested
			printf ("ecosconfig %s (%s %s)\n%s\n", TOOL_VERSION, __DATE__, __TIME__, TOOL_COPYRIGHT);
		} else { // the tool version was not requested
			usage_message (); // print the usage message
		}
		return EXIT_SUCCESS;
	}

	// set the default save file
	if (savefile.empty ()) { // if the save file was not specified on the command line
		savefile = DEFAULT_SAVE_FILE; // use the default save file
	}

	// find the repository
	if (repository.empty ()) { // if the repository was not specified on the command line
		const char * env_var = getenv ("ECOS_REPOSITORY");
		if (env_var) { // if the ECOS_REPOSITORY environment variable is defined
			repository = env_var;
		} else { // the ECOS_REPOSITORY environment variable is not defined
			// assume that the tool is located in the root of the repository
#ifdef _MSC_VER
			char toolpath [_MAX_PATH + 1];
			_fullpath (toolpath, argv [0], sizeof (toolpath)); // get the absolute path to the tool
#else
			char toolpath [MAXPATHLEN + 1];
			realpath (argv [0], toolpath); // get the absolute path to the tool
#endif
			repository = toolpath;
			for (unsigned int n = repository.size () - 1; n > 0; n--) { // for each char starting at the tail
				if (('\\' == repository [n]) || ('/' == repository [n])) { // if the char is a directory separator
					repository.resize (n); // remove the filename from the filepath
					break;
				}
			}
		}
	}

#ifdef __CYGWIN__
	// convert cygwin paths to win32 paths
	char buffer [MAXPATHLEN + 1];
	cygwin_conv_to_win32_path (repository.c_str (), buffer);
	repository = buffer;
	cygwin_conv_to_win32_path (savefile.c_str (), buffer);
	savefile = buffer;
	if (! install_prefix.empty ()) { // cygwin_conv_to_win32_path() does not copy an empty string
		cygwin_conv_to_win32_path (install_prefix.c_str (), buffer);
		install_prefix = buffer;
	}
#endif

	// process the command
	cdl_exec exec (trim_path (repository), savefile, trim_path (install_prefix), no_resolve);
	const std::string command = argv [command_index];
	bool status = true;

	if ("new" == command) {
		if (command_index + 2 == argc) {
			status = exec.cmd_new (argv [command_index + 1]);
		} else if (command_index + 3 == argc) {
			status = exec.cmd_new (argv [command_index + 1], argv [command_index + 2]);
		} else if (command_index + 4 == argc) {
			status = exec.cmd_new (argv [command_index + 1], argv [command_index + 2], argv [command_index + 3]);
		} else {
			status = false;
			usage_message ();
		}

	} else if ("tree" == command) {
		if (command_index + 1 == argc) {
			status = exec.cmd_tree ();
		} else {
			status = false;
			usage_message ();
		}

	} else if ("list" == command) {
		if (command_index + 1 == argc) {
			status = exec.cmd_list ();
		} else {
			status = false;
			usage_message ();
		}

	} else if ("check" == command) {
		if (command_index + 1 == argc) {
			status = exec.cmd_check ();
		} else {
			status = false;
			usage_message ();
		}

	} else if ("resolve" == command) {
		if (command_index + 1 == argc) {
			status = exec.cmd_resolve ();
		} else {
			status = false;
			usage_message ();
		}

	} else if ("add" == command) {
		if (command_index + 1 < argc) {
			std::vector<std::string> packages;
			for (int n = command_index + 1; n < argc; n++) {
				packages.push_back (argv [n]);
			}
			status = exec.cmd_add (packages);
		} else {
			status = false;
			usage_message ();
		}

	} else if ("remove" == command) {
		if (command_index + 1 < argc) {
			std::vector<std::string> packages;
			for (int n = command_index + 1; n < argc; n++) {
				packages.push_back (argv [n]);
			}
			status = exec.cmd_remove (packages);
		} else {
			status = false;
			usage_message ();
		}

	} else if ("version" == command) {
		if (command_index + 2 < argc) {
			std::vector<std::string> packages;
			for (int n = command_index + 2; n < argc; n++) {
				packages.push_back (argv [n]);
			}
			status = exec.cmd_version (argv [command_index + 1], packages);
		} else {
			status = false;
			usage_message ();
		}

	} else if ("target" == command) {
		if (command_index + 2 == argc) {
			status = exec.cmd_target (argv [command_index + 1]);
		} else {
			usage_message ();
			return -1;
		}

	} else if ("template" == command) {
		if (command_index + 2 == argc) {
			status = exec.cmd_template (argv [command_index + 1]);
		} else if (command_index + 3 == argc) {
			status = exec.cmd_template (argv [command_index + 1], argv [command_index + 2]);
		} else {
			status = false;
			usage_message ();
		}

	} else if ("export" == command) {
		if (command_index + 2 == argc) {
			status = exec.cmd_export (argv [command_index + 1]);
		} else {
			usage_message ();
			return -1;
		}

	} else if ("import" == command) {
		if (command_index + 2 == argc) {
			status = exec.cmd_import (argv [command_index + 1]);
		} else {
			usage_message ();
			return -1;
		}

	} else {
		status = false;
		usage_message ();
	}

	return status ? EXIT_SUCCESS : EXIT_FAILURE;
}

// remove the trailing directory separator from a file path if present
std::string trim_path (const std::string input) {
	std::string output = input;
	if (! output.empty ()) {
		const char last_char = output [output.size () - 1];
		if (('\\' == last_char) || ('/' == last_char)) { // if the last char is a directory separator
			output.resize (output.size () - 1); // remove the last char
		}
	}
	return output;
}

// print a usage message
void usage_message () {
	printf ("Usage: ecosconfig [ qualifier ... ] [ command ]\n");
	printf ("  commands are:\n");
	printf ("    list                                       : list repository contents\n");
	printf ("    new TARGET [ TEMPLATE [ VERSION ] ]        : create a configuration\n");
	printf ("    target TARGET                              : change the target hardware\n");
	printf ("    template TEMPLATE [ VERSION ]              : change the template\n");
	printf ("    add PACKAGE [ PACKAGE ... ]                : add package(s)\n");
	printf ("    remove PACKAGE [ PACKAGE ... ]             : remove package(s)\n");
	printf ("    version VERSION PACKAGE [ PACKAGE ... ]    : change version of package(s)\n");
	printf ("    export FILE                                : export minimal config info\n");
	printf ("    import FILE                                : import additional config info\n");
	printf ("    check                                      : check the configuration\n");
	printf ("    resolve                                    : resolve conflicts\n");
	printf ("    tree                                       : create a build tree\n");
	printf ("  qualifiers are:\n");
	printf ("    --config=FILE                              : the configuration file\n");
	printf ("    --prefix=DIRECTORY                         : the install prefix\n");
	printf ("    --srcdir=DIRECTORY                         : the source repository\n");
	printf ("    --no-resolve                               : disable conflict resolution\n");
	printf ("    --version                                  : show version and copyright\n");
}
