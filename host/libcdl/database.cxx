//{{{  Banner                                                   

//============================================================================
//
//      database.cxx
//
//      Temporary implementation of the CdlPackagesDatabase class
//      Implementations of the temporary CdlTargetsDatabase and
//      CdlTemplatesDatabase classes.
//
//============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// ----------------------------------------------------------------------------
// Copyright (C) 1999, 2000 Red Hat, Inc.
//
// This file is part of the eCos host tools.
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
//============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Contact(s):  bartv
// Date:        1999/01/21
// Version:     0.02
//
//####DESCRIPTIONEND####
//============================================================================

//}}}
//{{{  #include's                                               

// ----------------------------------------------------------------------------
#include "cdlconfig.h"

// Get the infrastructure types, assertions, tracing and similar
// facilities.
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>

// <cdl.hxx> defines everything implemented in this module.
// It implicitly supplies <string>, <vector> and <map> because
// the class definitions rely on these headers.
#include <cdl.hxx>

// strcmp() is useful when dealing with Tcl strings.
#include <cstring>

//}}}

//{{{  Statics                                                  

// ----------------------------------------------------------------------------
// Some test cases may want to read in a file other than
// "ecos.db", e.g. to facilitate testing the error conditions.
char*
CdlPackagesDatabaseBody::database_name = "ecos.db";

// ----------------------------------------------------------------------------
// The new_package etc. commands need to store the name of the
// current package so that subsequent commands can do the right thing.
// Using constant strings as the key avoids typo problems.
const char*     dbparser_current_package        = "::dbparser_current_package";
const char*     dbparser_current_target         = "::dbparser_current_target";
const char*     dbparser_component_repository   = "::component_repository";
const char*     dbparser_database_name          = "::database_name";
const char*     dbparser_pkgdir                 = "::pkgdir";
const char*     dbparser_current_version        = "::version";
const char*     dbparser_current_script         = "::script";
const char*     dbparser_database_key           = "dbparser_key";       // for assoc data
const char*     template_description_key        = "__cdl_extract_template_description"; // ditto
const char*     template_packages_key           = "_cdl_extract_template_packages";

CYGDBG_DEFINE_MEMLEAK_COUNTER(CdlPackagesDatabaseBody);

//}}}
//{{{  Utility Tcl scripts                                      

// ----------------------------------------------------------------------------
// Utility scripts.
//
// Given a directory and a filename relative to that directory,
// extract the contents of that file and store it in a variable
// "script".

static char* read_file_script = "                                       \n\
if {[file pathtype $::database_name] != \"relative\"} {                 \n\
    error \"Database name \\\"$::database_name\\\" should be relative\" \n\
}                                                                       \n\
set filename [file join $::component_repository $::database_name]       \n\
if {0 == [file exists $filename]} {                                     \n\
    error \"Component repository database $filename does not exist\"    \n\
}                                                                       \n\
if {0 == [file readable $filename]} {                                   \n\
    error \"Component repository database $filename is not readable\"   \n\
}                                                                       \n\
set fd     \"\"                                                         \n\
set script \"\"                                                         \n\
set status [catch {                                                     \n\
    set fd [open $filename r]                                           \n\
    set script [read $fd]                                               \n\
} message]                                                              \n\
if {$fd != \"\"} {                                                      \n\
    close $fd                                                           \n\
}                                                                       \n\
if { $status != 0 } {                                                   \n\
    error $message                                                      \n\
}                                                                       \n\
";

//}}}
//{{{  Tcl commands for the parser                              

//{{{  CdlDbParser class                                

// ----------------------------------------------------------------------------
// Commands that get invoked from inside the Tcl interpreter. These
// need access to the internals of the database objects, which can be
// achieved by making them static members of a CdlDbParser class.

class CdlDbParser {
  public:
    static int new_package(CdlInterpreter, int, char**);
    static int package_description(CdlInterpreter, int, char**);
    static int package_alias(CdlInterpreter, int, char**);
    static int package_directory(CdlInterpreter, int, char**);
    static int package_script(CdlInterpreter, int, char**);
    static int package_hardware(CdlInterpreter, int, char**);

    static int new_target(CdlInterpreter, int, char**);
    static int target_description(CdlInterpreter, int, char**);
    static int target_alias(CdlInterpreter, int, char**);
    static int target_packages(CdlInterpreter, int, char**);
    static int target_command_prefix(CdlInterpreter, int, char**);
    static int target_cflags(CdlInterpreter, int, char**);
    static int target_enable(CdlInterpreter, int, char**);
    static int target_disable(CdlInterpreter, int, char**);
    static int target_set_value(CdlInterpreter, int, char**);
};

//}}}
//{{{  CdlDbParser::package-related                     

// ----------------------------------------------------------------------------
// package <name> <body>

int
CdlDbParser::new_package(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::new_package", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);
    
    if (3 != argc) {
        interp->set_result("A package definition should include name and contents");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    std::string pkg_name        = argv[1];
    std::string msg             = std::string("Package ") + pkg_name + ": ";

    // Better make sure that this is not a duplicate definition.
    if (std::find(db->package_names.begin(), db->package_names.end(), pkg_name) != db->package_names.end()) {
        interp->set_result(msg + "a package can only be defined once");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // Add this package to the list.
    db->package_names.push_back(pkg_name);

    // Also create a new package structure. This requires a default structure,
    // which cannot be filled in until the body is executed.
    CdlPackagesDatabaseBody::package_data tmp_struct;
    db->packages[pkg_name]      = tmp_struct;

    CdlPackagesDatabaseBody::package_data& package = db->packages[pkg_name];
    // aliases and versions are vectors and will take care of themselves
    package.description         = "";
    package.directory           = "";
    package.script              = "";
    package.hardware            = false;

    // Sort out the commands, then invoke the script in argv[2]. There is
    // no need to worry about error recovery here, any errors will be
    // fatal anyway.
    CdlInterpreterCommandEntry commands[] = {
        CdlInterpreterCommandEntry("description", &CdlDbParser::package_description ),
        CdlInterpreterCommandEntry("alias",       &CdlDbParser::package_alias       ),
        CdlInterpreterCommandEntry("directory",   &CdlDbParser::package_directory   ),
        CdlInterpreterCommandEntry("script",      &CdlDbParser::package_script      ),
        CdlInterpreterCommandEntry("hardware",    &CdlDbParser::package_hardware    ),
        CdlInterpreterCommandEntry("",            0                                 )
    };
    int i;
    std::vector<CdlInterpreterCommandEntry> new_commands;
    for (i = 0; 0 != commands[i].command; i++) {
        new_commands.push_back(commands[i]);
    }
    std::vector<CdlInterpreterCommandEntry>* old_commands    = interp->push_commands(new_commands);
    interp->set_variable(dbparser_current_package, pkg_name);
    std::string str_result;
    if (TCL_OK != interp->eval(argv[2], str_result)) {
        interp->set_result(msg + str_result);
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    interp->pop_commands(old_commands);
    interp->unset_variable(dbparser_current_package);

    // Some of the fields are compulsory.
    if ("" == package.directory) {
        interp->set_result(msg + "missing directory specification");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if ("" == package.script) {
        interp->set_result(msg + "missing script specification");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if (0 == package.aliases.size()) {
        interp->set_result(msg + "at least one alias should be supplied");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: description <text>
int
CdlDbParser::package_description(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::package_description", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_package);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->packages.find(name) != db->packages.end());
    
    CdlPackagesDatabaseBody::package_data& package = db->packages[name];
    std::string msg = "Package " + name + ": ";

    if (2 != argc) {
        interp->set_result(msg + "the package description should be a single string");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if ("" != package.description) {
        interp->set_result(msg + "a package can have only one description");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }

    package.description = argv[1];
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: alias <list>
// For example: alias { "This is an alias" another_alias dummy_name }
int
CdlDbParser::package_alias(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::package_alias", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_package);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->packages.find(name) != db->packages.end());
    
    CdlPackagesDatabaseBody::package_data& package = db->packages[name];
    std::string msg = "Package " + name + ": ";
    
    // The alias command should be used only once
    if (0 < package.aliases.size()) {
        interp->set_result(msg + "there should be only one list of aliases");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // There should be one argument, a list of valid packages.
    if (2 != argc) {
        interp->set_result(msg + "alias should be followed by a list of known aliases");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    int         list_count      = 0;
    char**      list_entries    = 0;
    Tcl_Interp* tcl_interp      = interp->get_tcl_interpreter();
    if (TCL_OK != Tcl_SplitList(tcl_interp, argv[1], &list_count, &list_entries)) {
        interp->set_result(msg + Tcl_GetStringResult(tcl_interp));
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if (0 == list_count) {
        interp->set_result(msg + "at least one alias should be supplied");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    for (int i = 0; i < list_count; i++) {
        package.aliases.push_back(list_entries[i]);
    }
    Tcl_Free((char*)list_entries);
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: directory <path>
// The path is of course relative to the component repository.
int
CdlDbParser::package_directory(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::package_directory", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabaseBody*>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_package);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->packages.find(name) != db->packages.end());

    CdlPackagesDatabaseBody::package_data& package = db->packages[name];
    std::string msg = "Package " + name + ": ";

    // The directory command should be used only once
    if ("" != package.directory) {
        interp->set_result(msg + "a package can be located in only one directory");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // And there should be exactly one argument.
    if (2 != argc) {
        interp->set_result(msg + "only one directory can be specified");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }

    package.directory = argv[1];
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: hardware
// There are no arguments.
int
CdlDbParser::package_hardware(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::package_hardware", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabaseBody*>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_package);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->packages.find(name) != db->packages.end());

    CdlPackagesDatabaseBody::package_data& package = db->packages[name];
    std::string msg = "Package " + name + ": ";

    if (1 != argc) {
        interp->set_result(msg + "there should be no further data after hardware");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if (package.hardware) {
        interp->set_result(msg + "the hardware property should be specified only once");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    package.hardware    = true;
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: script <filename>
int
CdlDbParser::package_script(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::package_script", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabaseBody*>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_package);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->packages.find(name) != db->packages.end());

    CdlPackagesDatabaseBody::package_data& package = db->packages[name];
    std::string msg = "Package " + name + ": ";

    // The script command should be used only once
    if ("" != package.script) {
        interp->set_result(msg + "a package can have only one starting CDL script");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // And there should be exactly one argument.
    if (2 != argc) {
        interp->set_result(msg + "only one CDL script can be specified");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    package.script = argv[1];

    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

//}}}
//{{{  CdlDbParser::target-related                      

// ----------------------------------------------------------------------------
// target <name> <body>

int
CdlDbParser::new_target(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::new_target", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);
    
    if (3 != argc) {
        interp->set_result("A target definition should include name and contents");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    std::string target_name     = argv[1];
    std::string msg             = std::string("Target ") + target_name + ": ";

    // Better make sure that this is not a duplicate definition.
    if (std::find(db->target_names.begin(), db->target_names.end(), target_name) != db->target_names.end()) {
        interp->set_result(msg + "a target can only be defined once");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // Add this target to the list.
    db->target_names.push_back(target_name);

    // Also create a new target structure. This requires a default structure,
    // which cannot be filled in until the body is executed.
    CdlPackagesDatabaseBody::target_data tmp_struct;
    db->targets[target_name] = tmp_struct;

    CdlPackagesDatabaseBody::target_data& target = db->targets[target_name];
    // aliases, packages and compiler_flags are vectors and will take care of themselves
    target.description          = "";
    target.command_prefix       = "";

    // Sort out the commands, then invoke the script in argv[2]. There is
    // no need to worry about error recovery here, any errors will be
    // fatal anyway.
    CdlInterpreterCommandEntry commands[] = {
        CdlInterpreterCommandEntry("description",    &CdlDbParser::target_description    ),
        CdlInterpreterCommandEntry("alias",          &CdlDbParser::target_alias          ),
        CdlInterpreterCommandEntry("packages",       &CdlDbParser::target_packages       ),
        CdlInterpreterCommandEntry("command_prefix", &CdlDbParser::target_command_prefix ),
        CdlInterpreterCommandEntry("cflags",         &CdlDbParser::target_cflags         ),
        CdlInterpreterCommandEntry("enable",         &CdlDbParser::target_enable         ),
        CdlInterpreterCommandEntry("disable",        &CdlDbParser::target_disable        ),
        CdlInterpreterCommandEntry("set_value",      &CdlDbParser::target_set_value      ),
        CdlInterpreterCommandEntry("",               0                                   )
    };
    int i;
    std::vector<CdlInterpreterCommandEntry> new_commands;
    for (i = 0; 0 != commands[i].command; i++) {
        new_commands.push_back(commands[i]);
    }
    std::vector<CdlInterpreterCommandEntry>* old_commands    = interp->push_commands(new_commands);
    interp->set_variable(dbparser_current_target, target_name);
    std::string str_result;
    if (TCL_OK != interp->eval(argv[2], str_result)) {
        interp->set_result(msg + str_result);
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    interp->pop_commands(old_commands);
    interp->unset_variable(dbparser_current_target);

    // Some of the fields are compulsory.
    if (0 == target.aliases.size()) {
        interp->set_result(msg + "at least one alias should be supplied");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
#if 0
    // command_prefix is now handled in the configuration data.
    if ("" == target.command_prefix) {
        interp->set_result(msg + "missing command prefix specification");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
#endif    
    // There is no check for > 0 hardware packages. This is an unlikely
    // scenario but should be allowed for.
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: description <text>
int
CdlDbParser::target_description(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_description", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());
    
    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";

    if (2 != argc) {
        interp->set_result(msg + "the target description should be a single string");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if ("" != target.description) {
        interp->set_result(msg + "a target can have only one description");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }

    target.description = argv[1];
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: alias <list>
// For example: alias { "This is an alias" another_alias dummy_name }
int
CdlDbParser::target_alias(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_alias", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());
    
    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";
    
    // The alias command should be used only once
    if (0 < target.aliases.size()) {
        interp->set_result(msg + "there should be only one list of aliases");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // There should be one argument, a list of valid aliases
    if (2 != argc) {
        interp->set_result(msg + "alias should be followed by a list of known aliases");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    int         list_count      = 0;
    char**      list_entries    = 0;
    Tcl_Interp* tcl_interp      = interp->get_tcl_interpreter();
    if (TCL_OK != Tcl_SplitList(tcl_interp, argv[1], &list_count, &list_entries)) {
        interp->set_result(msg + Tcl_GetStringResult(tcl_interp));
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if (0 == list_count) {
        interp->set_result(msg + "at least one alias should be supplied");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    for (int i = 0; i < list_count; i++) {
        target.aliases.push_back(list_entries[i]);
    }
    Tcl_Free((char*)list_entries);
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: packages <list> ...
// For example: packages { CYGPKG_HAL_XXX CYGPKG_HAL_YYY }
int
CdlDbParser::target_packages(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_packages", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());
    
    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";
    
    // The packages command should be used only once
    if (0 < target.packages.size()) {
        interp->set_result(msg + "there should be only one list of packages");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // There should be one argument, a list of valid packages.
    if (2 != argc) {
        interp->set_result(msg + "packages should be followed by a list of known packages");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    int         list_count      = 0;
    char**      list_entries    = 0;
    Tcl_Interp* tcl_interp      = interp->get_tcl_interpreter();
    if (TCL_OK != Tcl_SplitList(tcl_interp, argv[1], &list_count, &list_entries)) {
        interp->set_result(msg + Tcl_GetStringResult(tcl_interp));
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if (0 == list_count) {
        interp->set_result(msg + "at least one package should be supplied");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    for (int i = 0; i < list_count; i++) {
        target.packages.push_back(list_entries[i]);
    }
    Tcl_Free((char*)list_entries);
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: command_prefix <string>
int
CdlDbParser::target_command_prefix(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_command_prefix", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabaseBody*>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());

    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";

    // The command_prefix command should be used only once
    if ("" != target.command_prefix) {
        interp->set_result(msg + "a target can have only one command_prefix string");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // And there should be exactly one argument.
    if (2 != argc) {
        interp->set_result(msg + "only one command_prefix can be specified");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }

    target.command_prefix = argv[1];
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: cflags <list of pairs> ...
// For example: cflags { ERRFLAGS "-Wall" DBGFLAGS "-g" }
int
CdlDbParser::target_cflags(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_cflags", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db      = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());
    
    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";
    
    // The cflags command should be used only once
    if (0 < target.cflags.size()) {
        interp->set_result(msg + "there should be only one set of compiler flags");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    // There should be one argument, a list of valid flags.
    if (2 != argc) {
        interp->set_result(msg + "cflags should be followed by a list of compiler flag/value pairs");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    int         list_count      = 0;
    char**      list_entries    = 0;
    Tcl_Interp* tcl_interp      = interp->get_tcl_interpreter();
    if (TCL_OK != Tcl_SplitList(tcl_interp, argv[1], &list_count, &list_entries)) {
        interp->set_result(msg + Tcl_GetStringResult(tcl_interp));
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    if (0 != (list_count % 2)) {
        interp->set_result(msg + "compiler flags and values must occur in pairs");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    int i;
    const std::vector<std::string>& valid_cflags = CdlPackagesDatabaseBody::get_valid_cflags();
    for (i = 0; i < list_count; i+= 2) {
        std::vector<std::string>::const_iterator j;
        for (j = valid_cflags.begin(); j != valid_cflags.end(); j++) {
            if (*j == list_entries[i]) {
                break;
            }
            if (j == valid_cflags.end()) {
                interp->set_result(msg + "invalid cflag name " + list_entries[i]);
                CYG_REPORT_RETVAL(TCL_ERROR);
                return TCL_ERROR;
            }
        }
    }
    // NOTE: do the quote marks have to be removed explicitly or is that done
    // by splitlist?
    for (i = 0; i < list_count; i+= 2) {
        target.cflags.push_back(std::make_pair(list_entries[i], list_entries[i+1]));
    }
    Tcl_Free((char*)list_entries);
    
    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: enable { opt1 opt2 ... }
// For example: enable { CYGPKG_HAL_ARM_CL7xxx_7211 }
int
CdlDbParser::target_enable(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_enable", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());

    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";

    // There should be one argument, a list of valid flags.
    if (2 != argc) {
        interp->set_result(msg + "enable should be followed by a list of CDL options");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    int         list_count      = 0;
    char**      list_entries    = 0;
    Tcl_Interp* tcl_interp      = interp->get_tcl_interpreter();
    if (TCL_OK != Tcl_SplitList(tcl_interp, argv[1], &list_count, &list_entries)) {
        interp->set_result(msg + Tcl_GetStringResult(tcl_interp));
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    for (int i = 0; i < list_count; i++) {
        target.enable.push_back(list_entries[i]);
    }
    Tcl_Free((char *) list_entries);

    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

                           
// Syntax: disable { opt1 opt2 ... }
// For example: disable { CYGPKG_HAL_ARM_CL7xxx_7111 }
int
CdlDbParser::target_disable(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_disable", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());

    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";

    // There should be one argument, a list of valid flags.
    if (2 != argc) {
        interp->set_result(msg + "disable should be followed by a list of CDL options");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    int         list_count      = 0;
    char**      list_entries    = 0;
    Tcl_Interp* tcl_interp      = interp->get_tcl_interpreter();
    if (TCL_OK != Tcl_SplitList(tcl_interp, argv[1], &list_count, &list_entries)) {
        interp->set_result(msg + Tcl_GetStringResult(tcl_interp));
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    for (int i = 0; i < list_count; i++) {
        target.disable.push_back(list_entries[i]);
    }
    Tcl_Free((char *) list_entries);

    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

// Syntax: set_value <option> <value>
// For example: set_value CYGHWR_MEMSIZE 0x100000
int
CdlDbParser::target_set_value(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("CdlDbParser::target_set_value", "result %d");
    CYG_REPORT_FUNCARG1XV(argc);
    CYG_PRECONDITION_CLASSC(interp);

    CdlPackagesDatabase db = static_cast<CdlPackagesDatabase>(interp->get_assoc_data(dbparser_database_key));
    CYG_INVARIANT_CLASSC(CdlPackagesDatabaseBody, db);

    std::string name = interp->get_variable(dbparser_current_target);
    CYG_ASSERTC("" != name);
    CYG_ASSERTC(db->targets.find(name) != db->targets.end());

    CdlPackagesDatabaseBody::target_data& target = db->targets[name];
    std::string msg = "Target " + name + ": ";

    // There should be one argument, a list of valid flags.
    if (3 != argc) {
        interp->set_result(msg + "set_value command should be followed by an option name and its value");
        CYG_REPORT_RETVAL(TCL_ERROR);
        return TCL_ERROR;
    }
    target.set_values.push_back(std::make_pair(std::string(argv[1]), std::string(argv[2])));

    CYG_REPORT_RETVAL(TCL_OK);
    return TCL_OK;
}

//}}}

//}}}

//{{{  CdlPackagesDatabase:: creation                           

// ----------------------------------------------------------------------------
// The exported interface is make(). The hard work is done inside the
// constructor.

CdlPackagesDatabase
CdlPackagesDatabaseBody::make(std::string repo)
    throw(CdlInputOutputException, std::bad_alloc)
{
    CYG_REPORT_FUNCNAMETYPE("CdlPackagesDatabase::make", "database %p");

    // Where is the component repository? The location may come from the
    // parent or from an environment variable ECOS_REPOSITORY
    if ("" == repo) {
        char *env = getenv("ECOS_REPOSITORY");
        if (0 == env) {
            throw CdlInputOutputException(std::string("No component repository specified and no ") +
                                          std::string("ECOS_REPOSITORY environment variable"));
        } else {
            repo = env;
        }
    }

    // Replace any backslashes in the repository with forward slashes.
    // The latter are used throughout the library
    // NOTE: this is not i18n-friendly.
    for (unsigned int i = 0; i < repo.size(); i++) {
        if ('\\' == repo[i]) {
            repo[i] = '/';
        }
    }
    CdlPackagesDatabase result = new CdlPackagesDatabaseBody(repo);
    CYG_REPORT_RETVAL(result);
    return result;
}

// ----------------------------------------------------------------------------

CdlPackagesDatabaseBody::CdlPackagesDatabaseBody(std::string repo)
    throw(CdlInputOutputException, std::bad_alloc)
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase:: constructor");
    CYG_PRECONDITIONC("" != repo);

    // There will be calls to check_this() while the database is evaluated,
    // so make sure that the database is valid first.
    component_repository                = repo;
    cdlpackagesdatabasebody_cookie      = CdlPackagesDatabaseBody_Magic;
    
    // We want to read in the entire packages file. Portability problems
    // can be largely eliminated by using a Tcl interpreter for this, but
    // under Windows there is a problem if the pathname is a cygwin one.
    // For now it is assumed that the supplied pathname is acceptable to
    // Tcl.
    //
    // A Tcl interpreter can be used to read in the file. It must not start
    // off as a safe interpreter because file I/O is needed. However it has
    // to be turned into a safe interpreter before the packages script is
    // actually executed.

    // No need for a try/catch here, there are no resources to free yet.
    CdlInterpreter interp = CdlInterpreterBody::make();

    try {
        interp->set_variable(dbparser_component_repository, repo);
        interp->set_variable(dbparser_database_name, database_name);
    }
    catch(std::bad_alloc) {
        delete interp;
        throw;
    }
    std::string str_result;
    if (TCL_OK != interp->eval(read_file_script, str_result)) {
        delete interp;
        throw CdlInputOutputException(str_result);
    }

    // We have the script. It comes from a source that is not completely
    // trusted so it can only be executed in a safe interpreter. However
    // after the script is read it will still be necessary to perform
    // glob commands afterwards to locate version subdirectories
    // and to check for the existence of the script files.
    try {
        CdlInterpreterCommandEntry commands[] =
        {
            CdlInterpreterCommandEntry("package",  &CdlDbParser::new_package  ),
            CdlInterpreterCommandEntry("target",   &CdlDbParser::new_target   ),
            CdlInterpreterCommandEntry("",         0                          )
        };
        unsigned int i;
        std::vector<CdlInterpreterCommandEntry> new_commands;
        for (i = 0; 0 != commands[i].command; i++) {
            new_commands.push_back(commands[i]);
        }
        interp->set_assoc_data(dbparser_database_key, static_cast<ClientData>(this));
        interp->push_commands(new_commands);

        if (TCL_OK != interp->eval("                    \n\
                set parser [interp create -safe]        \n\
                $parser    alias package  ::package     \n\
                $parser    alias target   ::target      \n\
                $parser    eval  $script                \n\
                ", str_result)) {
            throw CdlInputOutputException(str_result);
        }

        // There should be at least one package and target.
        if (0 == package_names.size()) {
            throw CdlInputOutputException("There are no packages in the database.");
        }
        if (0 == target_names.size()) {
            throw CdlInputOutputException("There are no targets in the database.");
        }
        
        // All of the package names should be valid CDL names.
        std::vector<std::string>::const_iterator name_i;
        std::vector<std::string>::const_iterator name_j;
        for (name_i = package_names.begin(); name_i != package_names.end(); name_i++) {
            if (!Cdl::is_valid_cdl_name(*name_i)) {
                throw CdlInputOutputException("Package " + *name_i + ", this is not a valid CDL name.");
            }
        }
        
        // The ecos.db data has been read in. For each package, find
        // the subdirectories and list them as versions. Each package
        // should have at least one version. Any errors will be
        // handled by the catch statement further down.
        for (std::map<std::string,package_data>::iterator pkg_i = packages.begin(); pkg_i != packages.end(); pkg_i++) {

            std::string pkgdir = repo + "/" + pkg_i->second.directory;
            std::vector<std::string> subdirs;
            unsigned int i;
            interp->locate_subdirs(pkgdir, subdirs);

            for (i = 0; i < subdirs.size(); i++) {
                if (("CVS" != subdirs[i]) &&
                    (interp->is_file(pkgdir + "/" + subdirs[i] + "/cdl/" + pkg_i->second.script) ||
                     interp->is_file(pkgdir + "/" + subdirs[i] + "/" + pkg_i->second.script))) {
                    pkg_i->second.versions.push_back(subdirs[i]);
                }
            }
            
            if (0 == pkg_i->second.versions.size()) {
                throw CdlInputOutputException("Package " + pkg_i->first + ": there are no version subdirectories");
            }
            std::sort(pkg_i->second.versions.begin(), pkg_i->second.versions.end(), Cdl::version_cmp());
        }

        // Now start looking for templates. These should reside in the
        // templates subdirectory of the component repository. Each template
        // should be in its own directory, and inside each directory should
        // be versioned template files with a .ect extension.
        std::string templates_dir = repo + "/" + "templates";
        std::vector<std::string> subdirs;
        interp->locate_subdirs(templates_dir, subdirs);

        for (i = 0; i < subdirs.size(); i++) {
            // Do not add the template to the known ones until we are sure there is
            // at least one valid template.
            std::vector<std::string> files;
            interp->locate_files(templates_dir + "/" + subdirs[i], files);
            unsigned int j;
            for (j = 0; j < files.size(); j++) {
                if ((4 < files[j].size()) && (".ect" == files[j].substr(files[j].size() - 4))) {
                    break;
                }
            }
            if (j != files.size()) {
                this->template_names.push_back(subdirs[i]);
                for ( ; j < files.size(); j++) {
                    if ((4 < files[j].size()) && (".ect" == files[j].substr(files[j].size() - 4))) {
                        this->templates[subdirs[i]].versions.push_back(files[j].substr(0, files[j].size() - 4));
                    }
                }
            }
        }

        // Consistency checks. All target-specific packages should have the
        // hardware attribute. Also, all the packages should exist.
        for (name_i = target_names.begin(); name_i != target_names.end(); name_i++) {
            for (name_j = targets[*name_i].packages.begin(); name_j != targets[*name_i].packages.end(); name_j++) {
                if (std::find(package_names.begin(), package_names.end(), *name_j) == package_names.end()) {
                    throw CdlInputOutputException("Target " + *name_i + " refers to an unknown package " + *name_j);
                }
                if (!packages[*name_j].hardware) {
                    throw CdlInputOutputException("Target " + *name_i + " refers to a non-hardware package " + *name_j);
                }
            }
        }
    }
    catch(...) {
        // Something has gone wrong. Clear out all of the data accumulated so far, as well
        // as the interpreter.
        delete interp;
        package_names.clear();
        target_names.clear();
        template_names.clear();
        packages.clear();
        targets.clear();
        templates.clear();
        throw;
    }

    delete interp;
    CYGDBG_MEMLEAK_CONSTRUCTOR();
    
    CYG_REPORT_RETURN();
}

//}}}
//{{{  CdlPackagesDatabase:: destructor                         

// ----------------------------------------------------------------------------
CdlPackagesDatabaseBody::~CdlPackagesDatabaseBody()
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase:: default destructor");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    cdlpackagesdatabasebody_cookie      = CdlPackagesDatabaseBody_Invalid;
    component_repository                = "";
    package_names.clear();
    target_names.clear();
    template_names.clear();
    packages.clear();
    targets.clear();
    templates.clear();

    CYGDBG_MEMLEAK_DESTRUCTOR();
    
    CYG_REPORT_RETURN();
}

//}}}
//{{{  CdlPackagesDatabase:: check_this()                       

// ----------------------------------------------------------------------------

bool
CdlPackagesDatabaseBody::check_this(cyg_assert_class_zeal zeal) const
{
    if (CdlPackagesDatabaseBody_Magic != cdlpackagesdatabasebody_cookie) {
        return false;
    }
    CYGDBG_MEMLEAK_CHECKTHIS();

    switch(zeal) {
      case cyg_system_test :
      case cyg_extreme :
      {
          std::vector<std::string>::const_iterator              names_i;
          std::map<std::string,package_data>::const_iterator    pkgs_i;
          
          // Every entry in the names vector should have an entry in the packages vector.
          for (names_i = package_names.begin(); names_i != package_names.end(); names_i++) {
              if (packages.find(*names_i) == packages.end()) {
                  return false;
              }
          }
          // The inverse should be true as well
          for (pkgs_i = packages.begin(); pkgs_i != packages.end(); pkgs_i++) {
              if (std::find(package_names.begin(), package_names.end(), pkgs_i->first) == package_names.end()) {
                  return false;
              }
          }
          
          // Repeat for targets.
          std::map<std::string,target_data>::const_iterator     targets_i;
          for (names_i = target_names.begin(); names_i != target_names.end(); names_i++) {
              if (targets.find(*names_i) == targets.end()) {
                  return false;
              }
          }
          for (targets_i = targets.begin(); targets_i != targets.end(); targets_i++) {
              if (std::find(target_names.begin(), target_names.end(), targets_i->first) == target_names.end()) {
                  return false;
              }
          }

          // And for templates
          std::map<std::string,template_data>::const_iterator    templates_i;
          for (names_i = template_names.begin(); names_i != template_names.end(); names_i++) {
              if (templates.find(*names_i) == templates.end()) {
                  return false;
              }
          }
          // The inverse should be true as well
          for (templates_i = templates.begin(); templates_i != templates.end(); templates_i++) {
              if (std::find(template_names.begin(), template_names.end(), templates_i->first) == template_names.end()) {
                  return false;
              }
          }
          
          // Possibly the package directories should be validated as
          // well, not to mention the various version subdirectories,
          // but doing file I/O inside an assertion is excessive.
      }
      case cyg_thorough :
      case cyg_quick:
          if ("" == component_repository) {
              return false;
          }
      case cyg_trivial:
      case cyg_none :
        break;
    }

    return true;
}

//}}}
//{{{  CdlPackagesDatabase:: misc                               

// ----------------------------------------------------------------------------

std::string
CdlPackagesDatabaseBody::get_component_repository() const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_component_repository");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    CYG_REPORT_RETURN();
    return component_repository;
}

//}}}
//{{{  CdlPackagesDatabase:: get package information            

// ----------------------------------------------------------------------------

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_packages(void) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_packages");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    CYG_REPORT_RETURN();
    return package_names;
}

bool
CdlPackagesDatabaseBody::is_known_package(std::string name) const
{
    CYG_REPORT_FUNCNAMETYPE("CdlPackagesDatabase::is_known_package", "result %d");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    bool result = false;
    if (std::find(package_names.begin(), package_names.end(), name) != package_names.end()) {
        result = true;
    }

    CYG_REPORT_RETVAL(result);
    return result;
}

const std::string&
CdlPackagesDatabaseBody::get_package_description(std::string pkg_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_package_description");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    std::map<std::string,package_data>::const_iterator pkgs_i = packages.find(pkg_name);
    if (pkgs_i != packages.end()) {
        CYG_REPORT_RETURN();
        return pkgs_i->second.description;
    }
    
    CYG_FAIL("Invalid package name passed to CdlPackagesDatabase::get_package_description()");
    static std::string dummy = "";
    return dummy;
}

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_package_aliases(std::string pkg_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_package_aliases");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    std::map<std::string,package_data>::const_iterator pkgs_i = packages.find(pkg_name);
    if (pkgs_i != packages.end()) {
        CYG_REPORT_RETURN();
        return pkgs_i->second.aliases;
    }
    
    CYG_FAIL("Invalid package name passed to CdlPackagesDatabase::get_package_aliases()");
    static std::vector<std::string> dummy;
    return dummy;
}

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_package_versions(std::string pkg_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_package_versions");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    std::map<std::string,package_data>::const_iterator pkgs_i = packages.find(pkg_name);
    if (pkgs_i != packages.end()) {
        CYG_REPORT_RETURN();
        return pkgs_i->second.versions;
    }
    
    CYG_FAIL("Invalid package name passed to CdlPackagesDatabase::get_package_versions()");
    static std::vector<std::string> dummy;
    return dummy;
}

const std::string&
CdlPackagesDatabaseBody::get_package_directory(std::string pkg_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_package_directory");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    std::map<std::string,package_data>::const_iterator pkgs_i = packages.find(pkg_name);
    if (pkgs_i != packages.end()) {
        CYG_REPORT_RETURN();
        return pkgs_i->second.directory;
    }
    
    CYG_FAIL("Invalid package name passed to CdlPackagesDatabase::get_package_directory()");
    static std::string dummy = "";
    return dummy;
}

const std::string&
CdlPackagesDatabaseBody::get_package_script(std::string pkg_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_package_script");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    std::map<std::string,package_data>::const_iterator pkgs_i = packages.find(pkg_name);
    if (pkgs_i != packages.end()) {
        CYG_REPORT_RETURN();
        return pkgs_i->second.script;
    }
    
    CYG_FAIL("Invalid package name passed to CdlPackagesDatabase::get_package_script()");
    static std::string dummy = "";
    return dummy;
}

bool
CdlPackagesDatabaseBody::is_hardware_package(std::string pkg_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::is_hardware_package");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    std::map<std::string,package_data>::const_iterator pkgs_i = packages.find(pkg_name);
    if (pkgs_i != packages.end()) {
        CYG_REPORT_RETURN();
        return pkgs_i->second.hardware;
    }
    
    CYG_FAIL("Invalid package name passed to CdlPackagesDatabase::is_hardware_package()");
    return false;
}

//}}}
//{{{  CdlPackagesDatabase:: get target information             

// ----------------------------------------------------------------------------

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_targets(void) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_targets");
    CYG_PRECONDITION_THISC();

    CYG_REPORT_RETURN();
    return target_names;
}

bool
CdlPackagesDatabaseBody::is_known_target(std::string name) const
{
    CYG_REPORT_FUNCNAMETYPE("CdlPackagesDatabase::is_known_target", "result %d");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    bool result = false;
    if (std::find(target_names.begin(), target_names.end(), name) != target_names.end()) {
        result = true;
    }

    CYG_REPORT_RETVAL(result);
    return result;
}

const std::string&
CdlPackagesDatabaseBody::get_target_description(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_description");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = targets.find(target_name);
    if (target_i != targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.description;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_description()");
    static std::string dummy = "";
    return dummy;
}

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_target_aliases(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_aliases");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = targets.find(target_name);
    if (target_i != targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.aliases;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_aliases()");
    static std::vector<std::string> dummy;
    return dummy;
}

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_target_packages(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_packages");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = targets.find(target_name);
    if (target_i != targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.packages;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_packages()");
    static std::vector<std::string> dummy;
    return dummy;
}

const std::string&
CdlPackagesDatabaseBody::get_target_command_prefix(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_command_prefix");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = targets.find(target_name);
    if (target_i != targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.command_prefix;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_command_prefix()");
    static std::string dummy = "";
    return dummy;
}

const std::vector<std::pair<std::string,std::string> >&
CdlPackagesDatabaseBody::get_target_compiler_flags(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_compiler_flags");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = targets.find(target_name);
    if (target_i != targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.cflags;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_compiler_flags()");
    static std::vector<std::pair<std::string,std::string> > dummy;
    return dummy;
}

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_target_enables(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_enables");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = this->targets.find(target_name);
    if (target_i != this->targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.enable;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_enables()");
    static std::vector<std::string> dummy;
    return dummy;
}

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_target_disables(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_disables");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = this->targets.find(target_name);
    if (target_i != this->targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.disable;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_disables()");
    static std::vector<std::string> dummy;
    return dummy;
}

const std::vector<std::pair<std::string, std::string> >&
CdlPackagesDatabaseBody::get_target_set_values(std::string target_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_target_set_values");
    CYG_PRECONDITION_THISC();

    std::map<std::string,target_data>::const_iterator target_i = this->targets.find(target_name);
    if (target_i != this->targets.end()) {
        CYG_REPORT_RETURN();
        return target_i->second.set_values;
    }
    
    CYG_FAIL("Invalid target name passed to CdlPackagesDatabase::get_target_values()");
    static std::vector<std::pair<std::string, std::string> > dummy;
    return dummy;
}

//}}}
//{{{  CdlPackagesDatabase:: get template information           

// ----------------------------------------------------------------------------
// Templates are different from packages and targets. The ecos.db file
// does not contain all the information in one convenient file, instead
// it is necessary to trawl through a templates sub-directory of the
// component repository. There are no aliases. Descriptions can be obtained
// only by executing the template file in a suitable interpreter.

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_templates(void) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_templates");
    CYG_PRECONDITION_THISC();

    CYG_REPORT_RETURN();
    return template_names;
}

bool
CdlPackagesDatabaseBody::is_known_template(std::string name) const
{
    CYG_REPORT_FUNCNAMETYPE("CdlPackagesDatabase::is_known_template", "result %d");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    bool result = false;
    if (std::find(template_names.begin(), template_names.end(), name) != template_names.end()) {
        result = true;
    }

    CYG_REPORT_RETVAL(result);
    return result;
}

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_template_versions(std::string template_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_template_versions");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    std::map<std::string,template_data>::const_iterator template_i = templates.find(template_name);
    if (template_i != templates.end()) {
        CYG_REPORT_RETURN();
        return template_i->second.versions;
    }
    
    CYG_FAIL("Invalid template name passed to CdlPackagesDatabase::get_template_versions()");
    static std::vector<std::string> dummy;
    return dummy;
}

std::string
CdlPackagesDatabaseBody::get_template_filename(std::string template_name, std::string version_name) const
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_template_filename");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    // Given the way templates are identified, the filename can be determined
    // easily by concatenating a few strings. The only complication is that
    // version_name may be an empty string, indicating that the most recent
    // version should be used.
    std::map<std::string,template_data>::const_iterator template_i = templates.find(template_name);
    if (template_i == templates.end()) {
        CYG_FAIL("Invalid template name passed to CdlPackagesDatabase::get_template_filename");
        CYG_REPORT_RETURN();
        return "";
    }
    if ("" == version_name) {
        CYG_ASSERTC(0 != template_i->second.versions.size());
        version_name = template_i->second.versions[0];
    } else {
        std::vector<std::string>::const_iterator vsn_i = std::find(template_i->second.versions.begin(),
                                                                   template_i->second.versions.end(), version_name);
        if (vsn_i == template_i->second.versions.end()) {
            CYG_FAIL("Invalid template version passed to CdlPackagesDatabase::get_template_filename");
            CYG_REPORT_RETURN();
            return "";
        }
    }

    std::string result = component_repository + "/templates/" + template_name + "/" + version_name + ".ect";
    CYG_REPORT_RETURN();
    return result;
}

// ----------------------------------------------------------------------------
// The descriptions now live in an eCos savefile, i.e. a Tcl script, so
// extracting them can be relatively expensive and needs to happen on
// a just-in-time basis.

const std::string
CdlPackagesDatabaseBody::get_template_description(std::string template_name, std::string version_name)
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_template_description");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    // Is this a known template?
    std::map<std::string, struct template_data>::iterator template_i = templates.find(template_name);
    if (template_i == templates.end()) {
        CYG_FAIL("Invalid template name passed to CdlPackagesDatabase::get_template_description");
        CYG_REPORT_RETURN();
        return "";
    }

    // Is it a known version of the template?
    if ("" == version_name) {
        CYG_ASSERTC(0 != template_i->second.versions.size());
        version_name = template_i->second.versions[0];
    } else {
        if (std::find(template_i->second.versions.begin(), template_i->second.versions.end(), version_name) ==
            template_i->second.versions.end()) {

            CYG_FAIL("Invalid template version passed to CdlPackagesDatabase::get_template_description");
            CYG_REPORT_RETURN();
            return "";
        }
    }

    // We have a valid template and version. Has the version file in
    // question been read in yet?
    std::map<std::string, struct template_version_data>::iterator version_i;
    version_i = template_i->second.version_details.find(version_name);
    if (version_i != template_i->second.version_details.end()) {
        CYG_REPORT_RETURN();
        return version_i->second.description;
    }
    
    std::string filename = this->get_template_filename(template_name, version_name);
    if ("" == filename) {
        CYG_REPORT_RETURN();
        return "";
    } 
    extract_template_details(filename, template_i->second.version_details[version_name].description,
                             template_i->second.version_details[version_name].packages);
    CYG_REPORT_RETURN();
    return template_i->second.version_details[version_name].description;
}

// ----------------------------------------------------------------------------
// Similarly extracting package information needs to happen on a
// just-in-time basis.
const std::vector<std::string>&
CdlPackagesDatabaseBody::get_template_packages(std::string template_name, std::string version_name)
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_template_packages");
    CYG_REPORT_FUNCARG1XV(this);
    CYG_PRECONDITION_THISC();

    static std::vector<std::string> dummy;
    
    // Is this a known template?
    std::map<std::string, struct template_data>::iterator template_i = templates.find(template_name);
    if (template_i == templates.end()) {
        CYG_FAIL("Invalid template name passed to CdlPackagesDatabase::get_template_packages");
        CYG_REPORT_RETURN();
        return dummy;
    }

    // Is it a known version of the template?
    if ("" == version_name) {
        CYG_ASSERTC(0 != template_i->second.versions.size());
        version_name = template_i->second.versions[0];
    } else {
        if (std::find(template_i->second.versions.begin(), template_i->second.versions.end(), version_name) ==
            template_i->second.versions.end()) {

            CYG_FAIL("Invalid template version passed to CdlPackagesDatabase::get_packages");
            CYG_REPORT_RETURN();
            return dummy;
        }
    }

    // We have a valid template and version. Has the version file in
    // question been read in yet?
    std::map<std::string, struct template_version_data>::iterator version_i;
    version_i = template_i->second.version_details.find(version_name);
    if (version_i != template_i->second.version_details.end()) {
        CYG_REPORT_RETURN();
        return version_i->second.packages;
    }
    
    std::string filename = this->get_template_filename(template_name, version_name);
    if ("" == filename) {
        CYG_REPORT_RETURN();
        return dummy;
    } 
    extract_template_details(filename, template_i->second.version_details[version_name].description,
                             template_i->second.version_details[version_name].packages);
    CYG_REPORT_RETURN();
    return template_i->second.version_details[version_name].packages;
}

// ----------------------------------------------------------------------------
// Extracting the description and package information involves running
// the script through a Tcl interpreter extended with the appropriate
// commands. Most of the savefile information is irrelevant and is handled
// by extract_ignore(). The commands of interest are cdl_configuration and
// its sub-commands description and package.

static int
extract_ignore(CdlInterpreter interp, int argc, char** argv)
{
    return TCL_OK;
}

static int
extract_cdl_configuration(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("extract_cdl_configuration", "result %d");
    CYG_REPORT_FUNCARG2XV(interp, argc);
    CYG_PRECONDITION_CLASSC(interp);

    int result = TCL_OK;
    
    // usage: cdl_configuration <name> <body>
    if (3 != argc) {
        interp->set_result("Invalid cdl_configuration command in template, expecting two arguments");
        result = TCL_ERROR;
    } else {
        // Ignore the first argument for now.
        std::string tmp;
        result = interp->eval(argv[2], tmp);
        
        // After processing the cdl_configuration command the description and
        // package information should be known. There is no point in processing
        // the rest of the file.
        if (TCL_OK == result) {
            interp->set_result("OK");
            result = TCL_ERROR;
        }
    }

    CYG_REPORT_RETVAL(result);
    return result;
}

static int
extract_cdl_description(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("extract_cdl_description", "result %d");
    CYG_REPORT_FUNCARG2XV(interp, argc);
    CYG_PRECONDITION_CLASSC(interp);

    int result = TCL_OK;
    
    // usage: package <name>
    if (2 != argc) {
        interp->set_result("Invalid description command in template, expecting just one argument");
        result = TCL_ERROR;
    } else {
        ClientData client_data = interp->get_assoc_data(template_description_key);
        CYG_ASSERTC(0 != client_data);
        std::string* result_ptr = static_cast<std::string*>(client_data);
        *result_ptr = argv[1];
    }

    CYG_REPORT_RETVAL(result);
    return result;
}

static int
extract_cdl_package(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAMETYPE("extract_cdl_package", "result %d");
    CYG_REPORT_FUNCARG2XV(interp, argc);
    CYG_PRECONDITION_CLASSC(interp);

    int result = TCL_OK;
    
    // usage: package <name> <version>
    if (2 > argc) {
        interp->set_result("Invalid package command in template, expecting two arguments");
        result = TCL_ERROR;
    } else {
        ClientData client_data = interp->get_assoc_data(template_packages_key);
        CYG_ASSERTC(0 != client_data);
        std::vector<std::string>* result_ptr = static_cast<std::vector<std::string>*>(client_data);
        result_ptr->push_back(argv[1]);
    }
    CYG_REPORT_RETVAL(result);
    return result;
}


void
CdlPackagesDatabaseBody::extract_template_details(std::string filename, std::string& description,
                                                      std::vector<std::string>& packages)
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::extract_template_description");

    CdlInterpreter interp = CdlInterpreterBody::make();
    interp->set_assoc_data(template_description_key, static_cast<ClientData>(&description));
    interp->set_assoc_data(template_packages_key,    static_cast<ClientData>(&packages));
    static CdlInterpreterCommandEntry extract_commands[] =
    {
        CdlInterpreterCommandEntry("cdl_savefile_version",  &extract_ignore                 ),
        CdlInterpreterCommandEntry("cdl_savefile_command",  &extract_ignore                 ),
        CdlInterpreterCommandEntry("cdl_configuration",     &extract_cdl_configuration      ),
        CdlInterpreterCommandEntry("hardware",              &extract_ignore                 ),
        CdlInterpreterCommandEntry("template",              &extract_ignore                 ),
        CdlInterpreterCommandEntry("description",           &extract_cdl_description        ),
        CdlInterpreterCommandEntry("package",               &extract_cdl_package            ),
        CdlInterpreterCommandEntry("unknown",               &extract_ignore                 ),
        CdlInterpreterCommandEntry("",                      0                               )
    };
    std::vector<CdlInterpreterCommandEntry> new_commands;
    for (int i = 0; 0 != extract_commands[i].command; i++) {
        new_commands.push_back(extract_commands[i]);
    }
    interp->push_commands(new_commands);

    std::string tmp;
    int result = interp->eval_file(filename, tmp);
    // Special escape mechanism, see extract_cdl_configuration() above
    if ((TCL_ERROR == result) && ("OK" == tmp)) {
        result = TCL_OK;
    }
#if 0    
    if (TCL_OK != result) {
        // No obvious way of recovering just yet
    }
#endif
    delete interp;
                           
    CYG_REPORT_RETURN();
}

//}}}
//{{{  CdlPackagesDatabase:: get_valid_cflags()                 

// ----------------------------------------------------------------------------

const std::vector<std::string>&
CdlPackagesDatabaseBody::get_valid_cflags()
{
    CYG_REPORT_FUNCNAME("CdlPackagesDatabase::get_valid_compiler_flags");

    static std::vector<std::string> result_vec;
    static const char* valid_flags[] = {
        "ARCHFLAGS",  "CARCHFLAGS",  "CXXARCHFLAGS",  "LDARCHFLAGS",
        "ERRFLAGS",   "CERRFLAGS",   "CXXERRFLAGS",   "LDERRFLAGS",
        "LANGFLAGS",  "CLANGFLAGS",  "CXXLANGFLAGS",  "LDLANGFLAGS",
        "DBGFLAGS",   "CDBGFLAGS",   "CXXDBGFLAGS",   "LDDBGFLAGS",
        "EXTRAFLAGS", "CEXTRAFLAGS", "CXXEXTRAFLAGS", "LDEXTRAFLAGS",
        0
    };

    if (0 == result_vec.size()) {
        for (int i = 0; 0 != valid_flags[i]; i++) {
            result_vec.push_back(valid_flags[i]);
        }
    }
    CYG_REPORT_RETURN();
    return result_vec;
}

//}}}
