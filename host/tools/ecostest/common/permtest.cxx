// ####ECOSHOSTGPLCOPYRIGHTBEGIN####                                        
// -------------------------------------------                              
// This file is part of the eCos host tools.                                
// Copyright (C) 1998, 1999, 2000 Free Software Foundation, Inc.            
//
// This program is free software; you can redistribute it and/or modify     
// it under the terms of the GNU General Public License as published by     
// the Free Software Foundation; either version 2 or (at your option) any   
// later version.                                                           
//
// This program is distributed in the hope that it will be useful, but      
// WITHOUT ANY WARRANTY; without even the implied warranty of               
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        
// General Public License for more details.                                 
//
// You should have received a copy of the GNU General Public License        
// along with this program; if not, write to the                            
// Free Software Foundation, Inc., 51 Franklin Street,                      
// Fifth Floor, Boston, MA  02110-1301, USA.                                
// -------------------------------------------                              
// ####ECOSHOSTGPLCOPYRIGHTEND####                                          
//==========================================================================
//
//      permtest.cxx
//
//      Create a configuration based on a .ptest file                                                                
//
//==========================================================================
//==========================================================================
//#####DESCRIPTIONBEGIN####                                             
//
// Author(s):           bartv
// Contributors:        bartv
// Date:                1999-11-05
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cdl.hxx>
#include <cstdio>
#include <cstdlib>

// ----------------------------------------------------------------------------
// Statics.
//
// The global configuration is created as the result of a pkgconf command
// when executing the .ptest file
static CdlConfiguration configuration = 0;

// The database is set up before the script is evaluated.
static CdlPackagesDatabase database = 0;

// The component repository can come from a command line argument or from
// an environment variable.
static char*            component_repository = 0;

// The name of the .ptest file. This is also the name used for the configuration.
static std::string      ptest_file = "";

// The target comes from a command line argument, and is needed by
// tcl_pkgconf_command()
std::string             target = "";

// The startup comes from a command line argument.
static char*            startup = 0;

// ----------------------------------------------------------------------------
// Diagnostics support
static void error_fn(std::string msg)
{
    fprintf(stderr, "%s\n", msg.c_str());
    exit(EXIT_FAILURE);
}

static void warn_fn(std::string msg)
{
    fprintf(stderr, "%s\n", msg.c_str());
}

// ----------------------------------------------------------------------------
// Given a database, output the known targets. This happens in response to
// the --targets flag or when the specified target is known.
// It is up to the calling code to exit with a suitable error code.
void
output_targets()
{
    if (0 == database) {
        fprintf(stderr, "Internal error, attempt to list targets when there is no database.\n");
        exit(EXIT_FAILURE);
    }
    const std::vector<std::string>& known_targets = database->get_targets();
    std::vector<std::string>::const_iterator target_i;

    printf("Known targets:\n");
    for (target_i = known_targets.begin(); target_i != known_targets.end(); target_i++) {
        printf("  %s\n", target_i->c_str());
    }
}

// ----------------------------------------------------------------------------
// Given a component repository and a permutation argument, find the
// corresponding .ptest file and store the result in the global ptest_file
static void find_ptest(CdlInterpreter interp, char* ptest_arg)
{
    if (interp->is_file(ptest_arg)) {
        ptest_file = ptest_arg;
        return;
    }
    std::string tmp = std::string(ptest_arg) + ".ptest";
    if (interp->is_file(tmp)) {
        ptest_file = tmp;
        return;
    }
    tmp = std::string(component_repository) + "/../testing/pkgtest/permtests/" + ptest_arg;
    if (interp->is_file(tmp)) {
        ptest_file = tmp;
        return;
    }
    tmp += ".ptest";
    if (interp->is_file(tmp)) {
        ptest_file = tmp;
        return;
    }
}

// ----------------------------------------------------------------------------
// Given an alias for a target or package, turn it into the canonical name.
std::string
get_target_canonical_name(std::string alias)
{
    if (0 == database) {
        fprintf(stderr, "Internal error, attempt to get target canonical name when there is no database.\n");
        exit(EXIT_FAILURE);
    }

    std::string result = "";
    const std::vector<std::string>& known_targets = database->get_targets();
    std::vector<std::string>::const_iterator target_i;
    for (target_i = known_targets.begin(); ("" == result) && (target_i != known_targets.end()); target_i++) {
        if (alias == *target_i) {
            result = alias;
            break;
        }

        const std::vector<std::string>& aliases = database->get_target_aliases(*target_i);
        std::vector<std::string>::const_iterator alias_i;
        for (alias_i = aliases.begin(); alias_i != aliases.end(); alias_i++) {
            if (alias == *alias_i) {
                result = *target_i;
                break;
            }
        }
    }

    return result;
}

std::string
get_package_canonical_name(std::string alias)
{
    if (0 == database) {
        fprintf(stderr, "Internal error, attempt to get package canonical name when there is no database.\n");
        exit(EXIT_FAILURE);
    }

    std::string result = "";
    const std::vector<std::string>& known_packages = database->get_packages();
    std::vector<std::string>::const_iterator package_i;
    for (package_i = known_packages.begin(); ("" == result) && (package_i != known_packages.end()); package_i++) {
        if (alias == *package_i) {
            result = alias;
            break;
        }

        const std::vector<std::string>& aliases = database->get_package_aliases(*package_i);
        std::vector<std::string>::const_iterator alias_i;
        for (alias_i = aliases.begin(); alias_i != aliases.end(); alias_i++) {
            if (alias == *alias_i) {
                result = *package_i;
                break;
            }
        }
    }

    return result;
}

// ----------------------------------------------------------------------------
// The pkgconf command is responsible for creating the initial
// configuration. This should use the hardware as per the command-line
// target argument, either the default template or the uITRON package
// depending on the presence of an explicit -disable-uitron or
// --disable-uitron argument, and any additional packages added
// or removed.

static int
tcl_pkgconf_command(CdlInterpreter interp, int argc, char** argv)
{
    if (0 != configuration) {
        interp->set_result("Invalid `pkgconf' command, the configuration already exists.");
        return TCL_ERROR;
    }
    if ("" == target) {
        interp->set_result("Internal error, attempt to create a configuration with no known target.");
        return TCL_ERROR;
    }
    if (0 == database) {
        interp->set_result("Internal error, attempt to create a configuration before the database has been initialized.");
        return TCL_ERROR;
    }
    if ("" == ptest_file) {
        interp->set_result("Internal error, attempt to create a configuration when the .ptest file is unknown.");
        return TCL_ERROR;
    }

    // Create a new interpreter for the configuration. Re-using the one
    // being used for evaluating the .ptest file could cause problems.
    CdlInterpreter new_interp = CdlInterpreterBody::make();
    if (0 == new_interp) {
        interp->set_result("Unable to create a new Tcl interpreter, out of memory?");
        return TCL_ERROR;
    }
    
    configuration = CdlConfigurationBody::make(ptest_file, database, new_interp);
    if (0 == configuration) {
        interp->set_result("Unable to create a new configuration, out of memory?");
        return TCL_ERROR;
    }

    bool uitron = true;
    int  i;
    for (i = 1; i < argc; i++) {
        if ((0 == strcmp(argv[i], "-disable-uitron")) || (0 == strcmp(argv[i], "--disable-uitron"))) {
            uitron = false;
            break;
        }
    }

    try {
        configuration->set_hardware(target, &error_fn, &warn_fn);
        if (uitron) {
            configuration->set_template("uitron", "", &error_fn, &warn_fn);
        } else {
            configuration->set_template("default", "", &error_fn, &warn_fn);
        }

        for (i = 1; i < argc; i++) {
            char* disable = 0;
            char* enable  = 0;
            
            if (0 == strncmp(argv[i], "-disable-", 9)) {
                disable = &(argv[i][9]);
            } else if (0 == strncmp(argv[i], "--disable-", 10)) {
                disable = &(argv[i][10]);
            } else if (0 == strncmp(argv[i], "-enable-", 8)) {
                enable = &(argv[i][8]);
            } else if (0 == strncmp(argv[i], "--enable-", 9)) {
                enable = &(argv[i][9]);
            } else {
                interp->set_result(std::string("Invalid pkgconf argument `") + argv[i] + "'");
                return TCL_ERROR;
            }

            if ((0 != disable) && (0 != strcmp(disable, "uitron"))) {
                std::string package = get_package_canonical_name(disable);
                if ("" == package) {
                    interp->set_result(std::string("Error in pkgconf command, attempt to disable unknown package `") +
                                       disable + "'");
                    return TCL_ERROR;
                }
                const std::vector<CdlLoadable>& loadables = configuration->get_loadables();
                std::vector<CdlLoadable>::const_iterator package_i;
                for (package_i = loadables.begin(); package_i != loadables.end(); package_i++) {
                    if (package == (*package_i)->get_name()) {
                        break;
                    }
                }
                if (package_i == loadables.end()) {
                    printf("Warning, pkgconf command, cannot disable %s because it is not loaded in the current configuration",
                           disable);
                } else {
                    configuration->unload_package(package, false);
                }
            }

            if (0 != enable) {
                std::string package = get_package_canonical_name(enable);
                if ("" == package) {
                    interp->set_result(std::string("Error in pkgconf command, attempt to enable unknown package `") +
                                       enable + "'");
                    return TCL_ERROR;
                }
                const std::vector<CdlLoadable>& loadables = configuration->get_loadables();
                std::vector<CdlLoadable>::const_iterator package_i;
                for (package_i = loadables.begin(); package_i != loadables.end(); package_i++) {
                    if (package == (*package_i)->get_name()) {
                        break;
                    }
                }
                if (package_i != loadables.end()) {
                    interp->set_result(std::string("Error in pkgconf command, cannot enable `") + enable +
                                       "' because it is already loaded in the current configuration.");
                    return TCL_ERROR;
                }
                configuration->load_package(package, "", &error_fn, &warn_fn, false);
            }
        }
    } catch(std::bad_alloc) {
        interp->set_result(std::string("Unexpected error, out of memory?"));
        return TCL_ERROR;
    } catch(CdlInputOutputException e) {
        interp->set_result(std::string("File I/O exception: ") + e.get_message());
        return TCL_ERROR;
    } catch(CdlParseException e) {
        interp->set_result(std::string("Parse error: ") + e.get_message());
        return TCL_ERROR;
    } catch(...) {
        interp->set_result("Internal error, an unexpected C++ exception was raised");
        return TCL_ERROR;
    }

    return TCL_OK;
}

// ----------------------------------------------------------------------------
// Other Tcl commands, on the whole these are straightforward.
static int
tcl_header_command(CdlInterpreter interp, int argc, char** argv)
{
    // Modifying a configuration no longer involves editing
    // a header file. Instead a single overall configuration
    // is updated, and then a savefile is generated. All that
    // is necessary is to evaluate the second argument, which
    // should be a Tcl script containing enable/disable/value
    // commands.
    if (3 != argc) {
        interp->set_result("Invalid `header' command, expecting two arguments.");
        return TCL_ERROR;
    }
    if (0 == configuration) {
        interp->set_result("`header' command detected before the configuration was created.");
        return TCL_ERROR;
    }
    std::string str_result;
    int result = interp->eval(argv[2], str_result);
    return result;
}

static int
tcl_makefile_command(CdlInterpreter interp, int argc, char** argv)
{
    // The makefile command was used to modify the compiler flags.
    // This happens very differently in the CDL world, and the
    // best solution for now is to ignore this command completely.
    if (3 != argc) {
        interp->set_result("Invalid `makefile' command, expecting two arguments.");
        return TCL_ERROR;
    }
    if (0 == configuration) {
        interp->set_result("`header' command detected before the configuration was created.");
        return TCL_ERROR;
    }
    return TCL_OK;
}

static int
tcl_enable_command(CdlInterpreter interp, int argc, char** argv)
{
    // There should be one argument, the name of a configuration option.
    if (2 != argc) {
        interp->set_result("Invalid `enable' command, expecting one argument.");
        return TCL_ERROR;
    }
    if (0 == configuration) {
        interp->set_result("`enable' command detected before the configuration was created.");
        return TCL_ERROR;
    }

    CdlNode node = configuration->lookup(argv[1]);
    if (0 == node) {
        printf("Warning, attempt to enable unknown option %s\n", argv[1]);
        return TCL_OK;
    }
    CdlValuable valuable = dynamic_cast<CdlValuable>(node);
    if ((0 == valuable) || !valuable->is_modifiable()) {
        printf("Warning, attempt to disable non-modifiable option %s\n", argv[1]);
        return TCL_OK;
    }
    CdlValueFlavor flavor = valuable->get_flavor();
    if ((CdlValueFlavor_Bool != flavor) && (CdlValueFlavor_BoolData != flavor)) {
        printf("Warning, attempt to disable non-boolean option %s\n", argv[1]);
        return TCL_OK;
    }

    valuable->enable(CdlValueSource_User);
    return TCL_OK;
}

static int
tcl_disable_command(CdlInterpreter interp, int argc, char** argv)
{
    // There should be one argument, the name of a configuration option.
    if (2 != argc) {
        interp->set_result("Invalid `disable' command, expecting one argument.");
        return TCL_ERROR;
    }
    if (0 == configuration) {
        interp->set_result("`header' command detected before the configuration was created.");
        return TCL_ERROR;
    }

    CdlNode node = configuration->lookup(argv[1]);
    if (0 == node) {
        printf("Warning, attempt to disable unknown option %s\n", argv[1]);
        return TCL_OK;
    }
    CdlValuable valuable = dynamic_cast<CdlValuable>(node);
    if ((0 == valuable) || !valuable->is_modifiable()) {
        printf("Warning, attempt to disable non-modifiable option %s\n", argv[1]);
        return TCL_OK;
    }
    CdlValueFlavor flavor = valuable->get_flavor();
    if ((CdlValueFlavor_Bool != flavor) && (CdlValueFlavor_BoolData != flavor)) {
        printf("Warning, attempt to disable non-boolean option %s\n", argv[1]);
        return TCL_OK;
    }

    valuable->disable(CdlValueSource_User);
    return TCL_OK;
}

static int
tcl_value_command(CdlInterpreter interp, int argc, char** argv)
{
    // There should be two argument, the name of a configuration option and the new value
    if (3 != argc) {
        interp->set_result("Invalid `value' command, expecting two arguments.");
        return TCL_ERROR;
    }
    if (0 == configuration) {
        interp->set_result("`value' command detected before the configuration was created.");
        return TCL_ERROR;
    }

    CdlNode node = configuration->lookup(argv[1]);
    if (0 == node) {
        printf("Warning, attempt to change the value of an unknown option %s\n", argv[1]);
        return TCL_OK;
    }
    CdlValuable valuable = dynamic_cast<CdlValuable>(node);
    if ((0 == valuable) || !valuable->is_modifiable()) {
        printf("Warning, attempt to change the value of non-modifiable option %s\n", argv[1]);
        return TCL_OK;
    }
    CdlValueFlavor flavor = valuable->get_flavor();
    if ((CdlValueFlavor_BoolData != flavor) && (CdlValueFlavor_Data != flavor)) {
        printf("Warning, attempt to change the value of non-data option %s\n", argv[1]);
        return TCL_OK;
    }

    valuable->set_value(argv[2], CdlValueSource_User);

    // There's no point setting the value for a BoolData but keeping it disabled
    if (CdlValueFlavor_BoolData == flavor) {
        valuable->enable(CdlValueSource_User);
    }

    return TCL_OK;
}

// ----------------------------------------------------------------------------
// Specifying the startup. There should be an option CYGHWR_HAL_STARTUP
// or CYG_HAL_STARTUP which can be set to the appropriate value.
static void
process_startup()
{
    if (0 == configuration) {
        fprintf(stderr, "Error: attempting to set the startup before a configuration has been created.\n");
        exit(EXIT_FAILURE);
    }
    if (0 == startup) {
        fprintf(stderr, "Error: no startup has been specified.\n");
        exit(EXIT_FAILURE);
    }

    CdlNode node = configuration->lookup("CYGHWR_HAL_STARTUP");
    if (0 == node) {
        node = configuration->lookup("CYG_HAL_STARTUP");
    }
    if (0 == node) {
        printf("Warning, there is no option CYGHWR_HAL_STARTUP in the current configuration.\n");
        return;
    }
    CdlValuable valuable = dynamic_cast<CdlValuable>(node);
    if (0 == valuable) {
        fprintf(stderr, "Error: CYGHWR_HAL_STARTUP exists but is not an option in the current configuration.\n");
        exit(EXIT_FAILURE);
    }
    valuable->set_value(startup, CdlValueSource_User);
}

// ----------------------------------------------------------------------------
static void
usage(void)
{
    printf("Usage: permtest [options] -target <target> -startup <startup> <ptest>\n");
    printf("       permtest --help\n");
    printf("       permtest --targets\n");
    printf("\n");
    printf("  Valid options are:\n");
    printf("    -srcdir     <component repository>\n");
    
    // It is up to the calling code to exit with a suitable exit code.
}

// ----------------------------------------------------------------------------
int
main(int argc, char** argv)
{
    bool        list_targets = false;
    bool        show_help    = false;
    char*       target_arg   = 0;
    char*       ptest_arg    = 0;

    // Time to process all the arguments.
    int i;
    for (i = 1; i < argc; i++) {

        // If there are any -- arguments, ignore the first -
        char* arg = argv[i];
        if (('-' == arg[0]) && ('-' == arg[1])) {
            arg = &(arg[1]);
        }
        
        if ((0 == strcmp("-help", arg)) || (0 == strcmp("-H", arg))) {
            show_help = true;
            continue;
        }

        if (0 == strcmp("-targets",  arg)) {
            list_targets = true;
            continue;
        }

        if (0 == strncmp("-target", arg, 7)) {
            if (0 != target_arg) {
                fprintf(stderr, "Only one target should be specified.\n");
                usage();
                exit(EXIT_FAILURE);
            }
            if ('=' == arg[7]) {
                target_arg = &(arg[8]);
            } else {
                if (++i == argc) {
                    fprintf(stderr, "Missing argument after -target\n");
                    usage();
                    exit(EXIT_FAILURE);
                }
                target_arg = argv[i];
            }
            continue;
        }
        if (0 == strncmp("-srcdir", arg, 7)) {
            if (0 != component_repository) {
                fprintf(stderr, "The component repository should be specified only once.\n");
                usage();
                exit(EXIT_FAILURE);
            }
            if ('=' == arg[7]) {
                component_repository = &(arg[8]);
            } else {
                if (++i == argc) {
                    fprintf(stderr, "Missing argument after -srcdir\n");
                    usage();
                    exit(EXIT_FAILURE);
                }
                component_repository = argv[i];
            }
            continue;
        }
        if (0 == strncmp("-startup", argv[i], 8)) {
            if (0 != startup) {
                fprintf(stderr, "The startup mode should be specified only once.\n");
                usage();
                exit(EXIT_FAILURE);
            }
            if ('=' == arg[8]) {
                startup = &(arg[9]);
            } else {
                if (++i == argc) {
                    fprintf(stderr, "Missing argument after -startup\n");
                    usage();
                    exit(EXIT_FAILURE);
                }
                startup = argv[i];
            }
            continue;
        }

        // None of the standard arguments, this had better be a ptest
        if (0 == ptest_arg) {
            ptest_arg = argv[i];
            continue;
        } else {
            fprintf(stderr, "Invalid argument %s, only one permutation file should be specified.\n", argv[i]);
            usage();
            exit(EXIT_FAILURE);
        }
    }
    
    // That takes care of the argument processing, time to do
    // something interesting.
    if (show_help) {
        usage();
        return EXIT_SUCCESS;
    }
    
    // Locate the component repository. This can come from either a
    // command-line argument or from an environment variable. The
    // component repository is validated by creating a database.
    if ((0 == component_repository) || ('\0' == component_repository[0])) {
        component_repository = getenv("ECOS_REPOSITORY");
        if (0 == component_repository) {
            fprintf(stderr, "The eCos repository location is unknown.\n");
            fprintf(stderr, "Please use a --srcdir command line argument, or set\n");
            fprintf(stderr, "the ECOS_REPOSITORY environment variable.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Create the Tcl interpreter, to provide ready access to the file
    // system and for processing the permutation file itself.
    CdlInterpreter interp = CdlInterpreterBody::make();
    if (0 == interp) {
        fprintf(stderr, "Internal error, unable to create a Tcl interpreter.\n");
        exit(EXIT_FAILURE);
    }
    interp->push_context("permtest");
    interp->push_error_fn_ptr(&error_fn);
    interp->push_warning_fn_ptr(&warn_fn);

    // Now look for the ptest file.
    if ((0 == ptest_arg) || ('\0' == ptest_arg[0])) {
        fprintf(stderr, "Please specify a permutation to be tested, e.g. default.ptest\n");
        usage();
        exit(EXIT_FAILURE);
    }
    find_ptest(interp, ptest_arg);
    if ("" == ptest_file) {
        fprintf(stderr, "Unable to locate a permutation file corresponding to %s\n", ptest_arg);
        exit(EXIT_FAILURE);
    }
    
    try {
        database = CdlPackagesDatabaseBody::make(component_repository, &error_fn, &warn_fn);
    } catch(std::bad_alloc) {
        fprintf(stderr, "Failed to initialize the component repository database, out of memory?\n");
        exit(EXIT_FAILURE);
    } catch(CdlInputOutputException e) {
        fprintf(stderr, "Failed to initialize the component repository database.\n");
        fprintf(stderr, "    %s\n", e.get_message().c_str());
        exit(EXIT_FAILURE);
    } catch(...) {
        fprintf(stderr, "Internal error, unexpected C++ exception while initializing the component repository database.\n");
        exit(EXIT_FAILURE);
    }

    // Once we have a database we can process the target.
    if (list_targets) {
        output_targets();
        exit(EXIT_SUCCESS);
    }
    if ((0 == target_arg) || ('\0' == target_arg[0])) {
        fprintf(stderr, "Please specify a target using e.g. --target=xyz\n");
        usage();
        exit(EXIT_FAILURE);
    }
    target = get_target_canonical_name(target_arg);
    if ("" == target) {
        fprintf(stderr, "Invalid target %s\n", target_arg);
        output_targets();
        exit(EXIT_FAILURE);
    }

    if ((0 == startup) || ('\0' == startup[0])) {
        fprintf(stderr, "Please specify a startup for this target, e.g. rom or ram\n");
        usage();
        exit(EXIT_FAILURE);
    }
    
    interp->add_command("pkgconf",  &tcl_pkgconf_command);
    interp->add_command("header",   &tcl_header_command);
    interp->add_command("makefile", &tcl_makefile_command);
    interp->add_command("enable",   &tcl_enable_command);
    interp->add_command("disable",  &tcl_disable_command);
    interp->add_command("value",    &tcl_value_command);
    
    std::string str_result;
    int result =  interp->eval_file(ptest_file, str_result);
    if (TCL_OK != result) {
        fprintf(stderr, "An error occurred while processing the permutation file.\n");
        fprintf(stderr, "%s\n", str_result.c_str());
        exit(EXIT_FAILURE);
    }

    // Take care of the startup argument as well.
    process_startup();

    // Perform global conflict resolution, in case some of the
    // conflicts were caused by the order in which packages were
    // loaded.
    configuration->resolve_all_conflicts();
    
    // We now have a configuration. Is it valid?
    bool valid = true;
    const std::list<CdlConflict>& all_conflicts = configuration->get_all_conflicts();
    std::list<CdlConflict>::const_iterator conf_i;
    for (conf_i = all_conflicts.begin(); conf_i != all_conflicts.end(); conf_i++) {
        // HACK: ignore any conflicts inside interfaces.
        CdlNode node = (*conf_i)->get_node();
        CdlInterface interface = dynamic_cast<CdlInterface>(node);
        if (0 != interface) {
            continue;
        }
        CdlProperty prop = (*conf_i)->get_property();
        fprintf(stderr, "Conflict: %s %s, property %s\n    %s\n", node->get_class_name().c_str(), node->get_name().c_str(),
                prop->get_property_name().c_str(), (*conf_i)->get_explanation().c_str());
        valid = false;
    }
    if (!valid) {
        printf("This configuration is invalid and cannot be built.\n");
        exit(EXIT_FAILURE);
    }
        
    // It appears we have a valid configuration. Generate the savefile.
    try {
        configuration->save("ecos.ecc");
    } catch(std::bad_alloc) {
        fprintf(stderr, "Unexpected error while saving configuration, out of memory?\n");
        exit(EXIT_FAILURE);
    } catch(CdlInputOutputException e) {
        fprintf(stderr, "I/O error while saving configuration.\n");
        fprintf(stderr, "%s\n", e.get_message().c_str());
        exit(EXIT_FAILURE);
    }

    // Time to invoke ecosconfig. This needs a component repository environment variable
    interp->set_variable("env(ECOS_REPOSITORY)", component_repository);
    result = interp->eval("exec ecosconfig tree", str_result);
    if (TCL_OK != result) {
        fprintf(stderr, "Error when invoking ecosconfig\n");
        fprintf(stderr, "%s\n", str_result.c_str());
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
