//{{{  Banner                                   

//============================================================================
//
//     parse.cxx
//
//     Miscellaneous parsing routines
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
// Date:        1999/02/23
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

// <cdlcore.hxx> defines everything implemented in this module.
// It implicitly supplies <string>, <vector> and <map> because
// the class definitions rely on these headers.
#include <cdlcore.hxx>

//}}}

//{{{  Description                              

// ----------------------------------------------------------------------------
// All CDL data is read via a Tcl interpreter, so the parsing is done by
// procedures that appear as Tcl commands. This has obvious advantages in
// terms of expressive power, but does result in a little bit of confusion
// when producing diagnostics.
//
// Errors should not bypass the Tcl interpreter, to ensure that that
// stays in a consistent state. In particular it is not possible to let
// arbitrary C++ exceptions to go straight through the Tcl interpreter,
// this is likely to result in a corrupted interpreter.
// 
// Also, it is not a good idea to abort parsing as soon as anything
// goes wrong. Instead there should be an error handling callback associated
// with the interpreter, which can be used to report errors. This
// callback may choose to raise an exception.
//
// Consider the case of parsing a property (which accounts for most of
// the code in this module). A property does not exist in isolation,
// only within the context of a suitable CDL entity such as an option.
// If parsing succeeds and a property object is created then it must
// be added to the current CDL entity.
//
// Therefore a typical parse routine looks like this:
//
//  1) get the current CDL node from the interpreter
//  2) do basic parsing of the data. Any errors should be reported
//     via the callback.
//  3) create a suitable CdlProperty object
//  4) add the property to the current entity
//
// std::bad_alloc and CdlStringException exceptions can be thrown, they
// will be intercepted by the CdlInterpreter class.

//}}}
//{{{  Statics                                  

// ----------------------------------------------------------------------------
// The string "property " is needed in various places. Provide it as a static
// to cut down the number of times the string constructor has to run.
static std::string property_string = "property ";

//}}}
//{{{  Generic parsing-related utilities        

//{{{  argv manipulation                        

// ----------------------------------------------------------------------------
// Some of the properties have aliases in the CDL data, so argv[0] has to be
// used to work out what is actually being parsed. However the Tcl interpreter
// may prefix the command name with :: to indicate the global namespace.
const char*
CdlParse::get_tcl_cmd_name(const char* name)
{
    if ((name[0] == ':') && (name[1] == ':')) {
        return &(name[2]);
    } else {
        return name;
    }
}

// Given a list of arguments, concatenate them together into a C++ string.
// This makes expression parsing easier. The final argument should be an
// index into the array, typically the result of a call to skip_argv_options().
std::string
CdlParse::concatenate_argv(int argc, char** argv, int index)
{
    CYG_REPORT_FUNCNAME("CdlParse::concatenate_argv");

    std::string result = "";
    for ( int i = index ; i < argc; i++) {
        if (i > index) {
            result += ' ';
        }
        result += std::string(argv[i]);
    }
    
    CYG_REPORT_RETURN();
    return result;
}

// ----------------------------------------------------------------------------
// Option parsing.
//
// Some properties accept modifiers in their argument list. For example,
// by default a "compile" property is just a list of source files that
// should be compiled and added to the current library. It is possible
// to specify an alternative library via a modifier:
//
//    ...
//    compile -library=libextras.a dummy.cxx
//    ...
//
// The rules applied for such options are intended to follow common Tcl
// practice:
//
// 1) any initial arguments beginning with a - are assumed to be
//    options.
//
// 2) the first argument which does not begin with a - stops option
//    processing.
//
// 3) option processing can also be terminated with a -- argument.
//    This may occasionally be required, e.g. to have -ve constants
//    in an expression.
//
// 4) the parsing code does not distinguish between single and double
//    hyphens. If there happens to be a second - in an option then this
//    is just ignored.
//
// 5) it is not necessary to supply the whole option name, as long as
//    what is provided is unambiguous. For example -lib=libextras.a is
//    acceptable (for now anyway, conceivably it would break in future).
//    Option processing is case sensitive.
//
// 6) name/value pairs can take the form -name=value or the form
//    -name value, whichever is appropriate.
//
// The parse_options() function takes the current interpreter (so that
// it can generate diagnostics), a prefix such as `property
// "requires"' or `package CYGPKG_HAL', details of the options to be
// parsed, an argc/argv list, an index, and a reference to a vector.
// The parsed options get placed in the vector, and the index argument
// gets updated to point at the first non-option argument. It will
// report problems via report_warning().
//
// The options description consists of a pointer to an array of
// C strings (allowing static initialization). Passing a zero
// argument indicates that the property takes no options. Otherwise
// the array should be zero terminated.
//
// Each entry in the array takes the form "name:flags". The optional
// flags consist of zero or more characters indicating how the option
// should be interpreted. Valid flags are:
//
// f    - this option takes no data, it is just a boolean flag. The
//        default behaviour assumes name/value pairs.
//
// m    - this option can occur multiple times. The default behaviour
//        is that each option can only occur once.

// Utility function to get hold of an option name without the colon
// or terminating flags.

static std::string
get_option_string(char* name)
{
    std::string result = "";
    while ((*name != ':') && (*name != '\0')) {
        result += *name++;
    }
    return result;
}

int
CdlParse::parse_options(CdlInterpreter interp, std::string diag_prefix, char** options,
                                 int argc, char** argv, int index,
                                 std::vector<std::pair<std::string,std::string> >& result)
{
    CYG_REPORT_FUNCNAMETYPE("CdlParse::parse_options", "final index %d");
    CYG_REPORT_FUNCARG4XV(interp, options, argc, argv);
    CYG_PRECONDITION_CLASSC(interp);
    CYG_PRECONDITIONC(argc > 0);        // The property name must be present.
    // It is possible for some of the arguments to have been processed already.
    CYG_PRECONDITIONC((index > 0) && (index <= argc));

    while((index < argc) && ('-' == argv[index][0])) {

        std::string name  = "";
        std::string value = "";

        // The sequence -- should always terminate option processing.
        if (0 == strcmp(argv[index], "--")) {
            index++;
            break;
        }

        char* arg_ptr       = argv[index];
        // Skip the initial -, and the second one as well if it is present.
        if ('-' == *++arg_ptr) {
            arg_ptr++;
        }

        // Construct the option name. This is the current argument up
        // to but not including the '=' character or EOD.
        while (('=' != *arg_ptr) && ('\0' != *arg_ptr)) {
            name += *arg_ptr++;
        }

        if ("" == name) {
            // One of "-", "-=xxx", or "--=x"
            CdlParse::report_warning(interp, diag_prefix + ", invalid option string " + argv[index]);
        }

        // Do not try to extract the value unless we are sure there
        // should be one. Instead try to match the option name. The
        // current value of name should be a unique substring of
        // one of the known option strings.
        //
        // Note that the supplied options descriptor can be NULL,
        // since most properties do not yet take any options.
        // In that case opt_index will remain at -1, and we should
        // get an "invalid option" diagnostic.
        unsigned int i;
        int opt_index = -1;
        if (0 != options) {
            for (i = 0; 0 != options[i]; i++) {
                if (0 == strncmp(name.c_str(), options[i], name.size())) {
                    if (-1 != opt_index) {
                        CdlParse::report_warning(interp, diag_prefix + ", ambiguous option name " + name +
                                                 ", it can match " + get_option_string(options[opt_index]) +
                                                 " or " + get_option_string(options[i]));
                        index++;
                        break;
                    } else {
                        opt_index = i;
                    }
                }
            }
        }
        if (-1 == opt_index) {
            CdlParse::report_warning(interp, diag_prefix + ", invalid option " + name);
            index++;
            break;
        }

        // The option has been identified successfully. Extract the flags.
        bool    flag_flag       = false;
        bool    multiple_flag   = false;
        char*   tmp = options[opt_index];
        while (('\0' != *tmp) && (':' != *tmp)) {
            tmp++;
        }
        if (':' == *tmp) {
            do {
                tmp++;
                if ('f' == *tmp) {
                    flag_flag = true;
                } else if ('m' == *tmp) {
                    multiple_flag = true;
                } else if ('\0' != *tmp) {
                    CYG_FAIL("Invalid property option");
                }
            } while ('\0' != *tmp);
        }

        // We now know the full option name. Use it for future diagnostics.
        name = get_option_string(options[opt_index]);

        // Take care of the value.
        if (flag_flag) {
            // There should not be a value. If the current argument is of the
            // form x=y then this is an error.
            if ('=' == *arg_ptr) {
                CdlParse::report_warning(interp, diag_prefix + ", option " + name + " does not take any data");
            }
            // Leave index pointing at the next argument to be processed.
            index++;
        } else {
            if ('=' == *arg_ptr) {
                value = std::string(++arg_ptr);
            } else if (++index == argc) {
                CdlParse::report_warning(interp, diag_prefix + ", missing data for option " + name);
            } else {
                value = argv[index];
            }
            index++;
        }
        // At this stage index points at the next argument to be processed, and should not
        // be updated again.
        
        // Unless the option can occur multiple times, make sure that it is not already
        // present in the options vector.
        if (!multiple_flag) {
            for (i = 0; i < result.size(); i++) {
                if (name == result[i].first) {
                    CdlParse::report_warning(interp, diag_prefix + ", option " + name +
                                             " can only be used once.");
                    break;
                }
            }
        }

        // The name/value pair is valid, so add it to the result vector.
        result.push_back(std::make_pair(name, value));
    }
    
    CYG_REPORT_RETVAL(index);
    return index;
}

//}}}
//{{{  Diagnostic prefix                        

// Construct a suitable prefix for any warning or error message. This
// should include the filename and the entity name.
//
// Obviously a line number would be rather useful as well, but this is not
// very easy because of the way Tcl interpreters work.

std::string
CdlParse::get_diagnostic_prefix(CdlInterpreter interp)
{
    std::string filename        = interp->get_filename();
    CdlNode     current_node    = interp->get_node();

    std::string result = ("" != filename) ? filename : "<unknown data source>";
    if (0 != current_node) {
        result += ", " + current_node->get_class_name() + " " + current_node->get_name();
    }
    result += "\n    ";
    return result;
}

//}}}
//{{{  Error count tracking                     

// Keep track of the number of errors that have occurred while doing some
// parsing. This functionality is not provided directly by the CdlInterpreter
// class, instead it is implemented using assoc data.

static const char       error_count_key[]       = "CdlErrorCount";

static void
error_count_delproc(ClientData data, Tcl_Interp* interp)
{
    CYG_REPORT_FUNCNAME("CdlParse::error_count_delproc");
    int* newed_ptr = static_cast<int*>(data);
    delete newed_ptr;
    CYG_REPORT_RETURN();
}

void
CdlParse::clear_error_count(CdlInterpreter interp)
{
    CYG_REPORT_FUNCNAME("CdlParse::clear_error_count");
    CYG_REPORT_FUNCARG1("interp %p", interp);
    CYG_PRECONDITION_CLASSC(interp);

    int*        newed_ptr = static_cast<int*>(interp->get_assoc_data(error_count_key));
    if (0 != newed_ptr) {
        *newed_ptr = 0;
    }

    CYG_REPORT_RETURN();
}

void
CdlParse::incr_error_count(CdlInterpreter interp, int how_much)
{
    CYG_REPORT_FUNCNAME("CdlParse::incr_error_counter");
    CYG_REPORT_FUNCARG2("interp %p, how_much %d", interp, how_much);
    CYG_PRECONDITION_CLASSC(interp);
    CYG_PRECONDITION(how_much > 0, "previous errors cannot be undone");
    
    int* newed_ptr = static_cast<int*>(interp->get_assoc_data(error_count_key));
    if (0 == newed_ptr) {
        newed_ptr = new int(how_much);
        interp->set_assoc_data(error_count_key, static_cast<void*>(newed_ptr), &error_count_delproc);
    } else {
        CYG_ASSERT((*newed_ptr + how_much) > *newed_ptr, "number of parsing errors should not overflow");
        *newed_ptr += how_much;
    }

    CYG_REPORT_RETURN();
}

int
CdlParse::get_error_count(CdlInterpreter interp)
{
    CYG_REPORT_FUNCNAMETYPE("CdlParse::get_error_count", "count %d");
    CYG_REPORT_FUNCARG1("interp %p", interp);
    CYG_PRECONDITION_CLASSC(interp);

    int result = 0;
    int* newed_ptr = static_cast<int*>(interp->get_assoc_data(error_count_key));
    if (0 != newed_ptr) {
        result = *newed_ptr;
    }

    CYG_REPORT_RETVAL(result);
    return result;
}

//}}}
//{{{  Error and warning reporting              

// Report an error or warning. This involves adding a suitable prefix
// and invoking the reporting callback currently associated with the
// interpreter. For errors it is also necessary to increment the error
// counter so that later count can detect the number of errors that
// have occurred.
//
// The error callback is allowed to raise a CdlParseException. This should
// not be caught here. Instead this exception is caught in every parse
// routine, before it can go back through the Tcl interpreter.
//
// FIXME: cope with prefixing multiline error messages.

void
CdlParse::report_error(CdlInterpreter interp, std::string message)
{
    CYG_REPORT_FUNCNAME("CdlParse::report_error");
    CYG_REPORT_FUNCARG1("interp %p", interp);
    CYG_PRECONDITION_CLASSC(interp);

    incr_error_count(interp);
    
    message = get_diagnostic_prefix(interp) + message;

    CdlDiagnosticFnPtr fn = interp->get_error_fn_ptr();
    CYG_ASSERT(0 != fn, "during parsing an interpreter should have an associated error reporting function");
    (*fn)(message);

    CYG_REPORT_RETURN();
}

void
CdlParse::report_warning(CdlInterpreter interp, std::string message)
{
    CYG_REPORT_FUNCNAME("CdlParse::report_warning");
    CYG_REPORT_FUNCARG1("interp %p", interp);
    CYG_PRECONDITION_CLASSC(interp);
    
    message = get_diagnostic_prefix(interp) + message;
    CdlDiagnosticFnPtr fn = interp->get_warning_fn_ptr();
    if (0 != fn) {
        (*fn)(message);
    }

    CYG_REPORT_RETURN();
}

//}}}
//{{{  The "unknown" command                    

// ----------------------------------------------------------------------------
// This routine should be installed in interpreters that get used for
// parsing CDL scripts. It gets invoked when the CDL script contains
// an unrecognised command, e.g. because of a typo, and makes sure that
// the usual diagnostics process is observed.
//
// This routine should be uninstalled after the parsing is complete,
// to avoid e.g. a ParseException when it is not expected.
int
CdlParse::unknown_command(CdlInterpreter interp, int argc, char** argv)
{
    CYG_REPORT_FUNCNAME("CdlParse::unknown_command");
    CYG_REPORT_FUNCARG3XV(interp, argc, argv);
    CYG_PRECONDITIONC(2 <= argc);
    CYG_PRECONDITION_CLASSC(interp);
    
    report_error(interp, std::string("Unknown command `") + argv[1] + "'.");
    CYG_UNUSED_PARAM(int, argc);
    
    return TCL_OK;
}

//}}}

//}}}
//{{{  Property-related parser utilities        

// ----------------------------------------------------------------------------
// Utilities related to parsing properties, rather than more general parsing.

// Provide a prefix that matches the current property.
std::string
CdlParse::get_property_prefix(char* argv0)
{
    CYG_REPORT_FUNCNAME("CdlParse::get_property_prefix");

    std::string result = std::string("Property " ) + CdlParse::get_tcl_cmd_name(argv0) + ", ";

    CYG_REPORT_RETURN();
    return result;
}

std::string
CdlParse::get_property_prefix(CdlProperty prop)
{
    CYG_REPORT_FUNCNAME("CdlParse::get_property_prefix");

    std::string result = std::string("Property ");
    const std::vector<std::string>& argv = prop->get_argv();
    result = result + argv[0] + ", ";
    
    CYG_REPORT_RETURN();
    return result;
}

// A variant of report_parse_error() which also adds the property prefix.
void
CdlParse::report_property_parse_error(CdlInterpreter interp, char* argv0, std::string msg)
{
    CYG_REPORT_FUNCNAME("CdlPase::report_property_parse_error");

    report_error(interp, get_property_prefix(argv0) + msg);
    
    CYG_REPORT_RETURN();
}

void
CdlParse::report_property_parse_error(CdlInterpreter interp, CdlProperty prop, std::string msg)
{
    CYG_REPORT_FUNCNAME("CdlParse::report_property_parse_error");

    report_error(interp, get_property_prefix(prop) + msg);

    CYG_REPORT_RETURN();
}

//}}}
//{{{  Generic property parsers                 

// ----------------------------------------------------------------------------
// Generic parsers
//
// These routines provide some more generic property parsing routines. argv[0]
// generally provides sufficient information to allow for sensible error messages.
// The command-specific parsers have to provide a property name. In addition it is
// possible to provide a function to handle per-command options, and another
// function that performs a final sanity check before the property gets added
// to the current entity.

//{{{  parse_minimal_property()         

// ----------------------------------------------------------------------------
// A minimal property takes no arguments.

int
CdlParse::parse_minimal_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                 char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_Minimal))
{
    CYG_REPORT_FUNCNAME("parse_minimal_property");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_Minimal new_property = 0;
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);
        
        if (data_index < argc) {
            CdlParse::report_property_parse_error(interp, argv[0],  std::string("Unexpected data ") + argv[data_index]);
        } else {
        
            // The command is valid, turn it into a property.
            // The property has been parsed successfully. Add it to the current node
            CdlNode current_node = interp->get_node();
            CYG_ASSERTC(0 != current_node);
            new_property = CdlProperty_MinimalBody::make(current_node, name, argc, argv, options);
            if (0 != final_parser) {
                (*final_parser)(interp, new_property);
            }
        }
    } catch(...) {
        
        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    
    return TCL_OK;
}

//}}}
//{{{  parse_string_property()          

// ----------------------------------------------------------------------------

int
CdlParse::parse_string_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_String))
{
    CYG_REPORT_FUNCNAME("parse_string_property");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_String new_property = 0;
    
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);

        if (data_index == argc) {
            CdlParse::report_property_parse_error(interp, argv[0], "missing argument.");
        } else if ((data_index + 1) < argc) {
            CdlParse::report_property_parse_error(interp, argv[0], std::string("Too many arguments, expecting just one."));
        } else {
        
            CdlNode current_node = interp->get_node();
            CYG_ASSERTC(0 != current_node);
            new_property = CdlProperty_StringBody::make(current_node, name, argv[data_index], argc, argv, options);
            if (0 != final_parser) {
                (*final_parser)(interp, new_property);
            }
        }
    } catch(...) {
        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    
    return TCL_OK;
}

//}}}
//{{{  parse_tclcode_property()         

// ----------------------------------------------------------------------------

int
CdlParse::parse_tclcode_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                 char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_TclCode))
{
    CYG_REPORT_FUNCNAME("parse_tclcode_property");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_TclCode new_property = 0;
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index      = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);
        
        if (data_index == argc) {
            CdlParse::report_property_parse_error(interp, argv[0], "missing Tcl code.");
        } else if ((data_index + 1) < argc) {
            CdlParse::report_property_parse_error(interp, argv[0], std::string("Invalid number of arguments.\n") +
                                         "    Expecting one argument, a Tcl code fragment.");
        } else if (!Tcl_CommandComplete(argv[data_index])) {
            CdlParse::report_property_parse_error(interp, argv[0], "incomplete Tcl code fragment.");
        } else {
        
            CdlNode current_node = interp->get_node();
            CYG_ASSERTC(0 != current_node);
            new_property = CdlProperty_TclCodeBody::make(current_node, name, argv[data_index], argc, argv, options);
            if (0 != final_parser) {
                (*final_parser)(interp, new_property);
            }
        }
     } catch(...) {
        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    
    return TCL_OK;
}

//}}}
//{{{  parse_stringvector_property()    

// ----------------------------------------------------------------------------

int
CdlParse::parse_stringvector_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                      char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_StringVector))
{
    CYG_REPORT_FUNCNAME("parse_tclcode_property");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_StringVector new_property = 0;
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index      = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);
        
        if (data_index == argc) {
            CdlParse::report_property_parse_error(interp, argv[0], "missing arguments.");
        } else {

            // Creating the property requires a vector of strings.
            std::vector<std::string>  strings;
            for ( ; data_index < argc; data_index++) {
                strings.push_back(argv[data_index]);
            }
            CdlNode current_node = interp->get_node();
            CYG_ASSERTC(0 != current_node);
            new_property = CdlProperty_StringVectorBody::make(current_node, name, strings, argc, argv, options);
            if (0 != final_parser) {
                (*final_parser)(interp, new_property);
            }
        }
    } catch(...) {

        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    
    return TCL_OK;
}

//}}}
//{{{  parse_reference_property()       

// ----------------------------------------------------------------------------

int
CdlParse::parse_reference_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                   char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_Reference),
                                   CdlUpdateHandler update_handler)
{
    CYG_REPORT_FUNCNAME("parse_reference_property");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_Reference new_property = 0;
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);
        
        if (data_index == argc) {
            CdlParse::report_property_parse_error(interp, argv[0], "missing argument.");
        } else if ((data_index + 1) < argc) {
            CdlParse::report_property_parse_error(interp, argv[0], "too many arguments, expecting just one.");
        } else {
            std::string refname = argv[data_index];
            if (!Cdl::is_valid_cdl_name(refname)) {
                CdlParse::report_property_parse_error(interp, argv[0], "`" + refname + "' is not a valid CDL name");
            } else {
                CdlNode current_node = interp->get_node();
                CYG_ASSERTC(0 != current_node);
                new_property = CdlProperty_ReferenceBody::make(current_node, name, refname,
                                                               update_handler, argc, argv, options);
                if (0 != final_parser) {
                    (*final_parser)(interp, new_property);
                }
            }
        }
    } catch(...) {
        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    
    return TCL_OK;
}

//}}}
//{{{  parse_expression_property()      

// ----------------------------------------------------------------------------

int
CdlParse::parse_expression_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                    char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_Expression),
                                    CdlUpdateHandler update_handler)
{
    CYG_REPORT_FUNCNAME("parse_expression_property");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_Expression new_property = 0;
    CdlExpression expr = 0;
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);
        
        std::string all_args = CdlParse::concatenate_argv(argc, argv, data_index);
        if ("" == all_args) {
            CdlParse::report_property_parse_error(interp, argv[0], "missing expression data.");
        } else {
        
            // The CdlExpression class has its own parsing routine. This
            // will raise an exception if there are any problems. It is
            // desirable to catch the exception and report the error via
            // the normal reporting mechanisms, which may allow parsing to
            // continue.
            try {
                expr = CdlExpressionBody::parse(all_args);
            } catch(CdlParseException e) {
                CdlParse::report_property_parse_error(interp, argv[0], e.get_message());
            }
            if (0 != expr) {
                CdlNode current_node = interp->get_node();
                CYG_ASSERTC(0 != current_node);
                new_property = CdlProperty_ExpressionBody::make(current_node, name, expr, update_handler, argc, argv, options);
                if (0 != final_parser) {
                    (*final_parser)(interp, new_property);
                }
            }
        }
    } catch(...) {
        if (0 != expr) {
            delete expr;
        }
        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    
    if (0 != expr) {
        delete expr;
    }
    return TCL_OK;
}

//}}}
//{{{  parse_list_expression_property() 

// ----------------------------------------------------------------------------

int
CdlParse::parse_listexpression_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                        char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_ListExpression),
                                        CdlUpdateHandler update_handler)
{
    CYG_REPORT_FUNCNAME("parse_list_expression_property");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_ListExpression new_property = 0;
    CdlListExpression expr = 0;
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);

        std::string all_args = CdlParse::concatenate_argv(argc, argv, data_index);
        if ("" == all_args) {
            CdlParse::report_property_parse_error(interp, argv[0], "missing list expression data.");
        } else {
        
            try {
                expr = CdlListExpressionBody::parse(all_args);
            } catch(CdlParseException e) {
                CdlParse::report_property_parse_error(interp, argv[0], e.get_message());
            }
            if (0 != expr) {
                CdlNode current_node = interp->get_node();
                CYG_ASSERTC(0 != current_node);
                new_property = CdlProperty_ListExpressionBody::make(current_node, name, expr, update_handler,
                                                                    argc, argv, options);
                if (0 != final_parser) {
                    (*final_parser)(interp, new_property);
                }
            }
        }
    } catch(...) {
        if (0 != expr) {
            delete expr;
        }
        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    if (0 != expr) {
        delete expr;
    }
    return TCL_OK;
}

//}}}
//{{{  parse_goalexpression_property()  

// ----------------------------------------------------------------------------

int
CdlParse::parse_goalexpression_property(CdlInterpreter interp, int argc, char** argv, std::string name,
                                        char** options_desc, void (*final_parser)(CdlInterpreter, CdlProperty_GoalExpression),
                                        CdlUpdateHandler update_handler)
{
    CYG_REPORT_FUNCNAMETYPE("parse_goal_expression_property", "result %d");
    CYG_PRECONDITION_CLASSC(interp);
    
    CdlProperty_GoalExpression new_property = 0;
    CdlGoalExpression expr = 0;
    try {
        std::vector<std::pair<std::string,std::string> > options;
        int data_index = CdlParse::parse_options(interp, property_string + argv[0], options_desc, argc, argv, 1, options);

        std::string all_args = CdlParse::concatenate_argv(argc, argv, data_index);
        if ("" == all_args) {
            CdlParse::report_property_parse_error(interp, argv[0], "missing goal expression data.");
        } else {

            try {
                expr = CdlGoalExpressionBody::parse(all_args);
            } catch(CdlParseException e) {
                CdlParse::report_property_parse_error(interp, argv[0], e.get_message());
            }
            if (0 != expr) {
                CdlNode current_node = interp->get_node();
                CYG_ASSERTC(0 != current_node);
                new_property = CdlProperty_GoalExpressionBody::make(current_node, name, expr, update_handler,
                                                                    argc, argv, options);
                if (0 != final_parser) {
                    (*final_parser)(interp, new_property);
                }
            }
        }
    } catch(...) {

        if (0 != expr) {
            delete expr;
        }
        if (0 != new_property) {
            delete new_property;
        }
        throw;
    }
    if (0 != expr) {
        delete expr;
    }

    return TCL_OK;
}

//}}}

//}}}
