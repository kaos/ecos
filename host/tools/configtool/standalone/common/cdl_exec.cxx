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
//      cdl_exec.cxx
//
//      The implementation of each ecosconfig command
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

#ifdef _MSC_VER
	#include <direct.h> /* for getcwd() */
#else
	#include <unistd.h> /* for getcwd() */
#endif
#ifdef __CYGWIN__
	#include <sys/cygwin.h> /* for cygwin_conv_to_win32_path() */
#endif
#include "build.hxx"
#include "cdl_exec.hxx"

cdl_exec::cdl_exec (const std::string repository_tree, const std::string savefile_name, const std::string install_tree, bool no_resolve) :
	pkgdata (NULL),
	interp (NULL),
	config (NULL) {
	repository = repository_tree;
	savefile = savefile_name;
	install_prefix = install_tree;
	CdlTransactionBody::set_inference_callback_fn (&inference_callback);
	if (no_resolve) {
		CdlTransactionBody::disable_automatic_inference ();
	}
}

bool cdl_exec::cmd_new (const std::string cdl_hardware, const std::string cdl_template /* = "default" */, const std::string cdl_version /* = "" */) {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::make ("eCos", pkgdata, interp);
		config->set_hardware (resolve_hardware_alias (cdl_hardware), &diagnostic_handler, &diagnostic_handler);
		if (pkgdata->is_known_template (cdl_template) && ! cdl_version.empty ()) {
			const std::vector<std::string> & versions = pkgdata->get_template_versions (cdl_template);
			if (versions.end () == std::find (versions.begin (), versions.end (), cdl_version)) {
				throw CdlStringException ("Unknown version " + cdl_version);
			}
		}
		config->set_template (cdl_template, cdl_version, &diagnostic_handler, &diagnostic_handler);
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_target (const std::string cdl_target) {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		config->set_hardware (resolve_hardware_alias (cdl_target), &diagnostic_handler, &diagnostic_handler);
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_template (const std::string cdl_template, const std::string cdl_version /* = "" */) {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		if (pkgdata->is_known_template (cdl_template) && ! cdl_version.empty ()) {
			const std::vector<std::string> & versions = pkgdata->get_template_versions (cdl_template);
			if (versions.end () == std::find (versions.begin (), versions.end (), cdl_version)) {
				throw CdlStringException ("Unknown version " + cdl_version);
			}
		}
		config->set_template (cdl_template, cdl_version, &diagnostic_handler, &diagnostic_handler);
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_export (const std::string cdl_savefile) {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		config->save (cdl_savefile, /* minimal = */ true);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_import (const std::string cdl_savefile) {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		config->add (cdl_savefile, &diagnostic_handler, &diagnostic_handler);
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_add (const std::vector<std::string> cdl_packages) {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		for (unsigned int n = 0; n < cdl_packages.size (); n++) {
			config->load_package (resolve_package_alias (cdl_packages [n]), "", &diagnostic_handler, &diagnostic_handler);
		}
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_remove (const std::vector<std::string> cdl_packages) {
	unsigned int n;
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		for (n = 0; n < cdl_packages.size (); n++) {
			if (! config->lookup (resolve_package_alias (cdl_packages [n]))) {
				throw CdlStringException ("Unknown package " + cdl_packages [n]);
			}
		}
		for (n = 0; n < cdl_packages.size (); n++) {
			config->unload_package (resolve_package_alias (cdl_packages [n]));
		}
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_version (const std::string cdl_version, const std::vector<std::string> cdl_packages) {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		for (unsigned int n = 0; n < cdl_packages.size (); n++) {
			config->change_package_version (resolve_package_alias (cdl_packages [n]), cdl_version, &diagnostic_handler, &diagnostic_handler, true);
		}
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_tree () {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
#ifdef _MSC_VER
		char cwd [_MAX_PATH + 1];
#else
		char cwd [PATH_MAX + 1];
#endif
		getcwd (cwd, sizeof cwd);
#ifdef __CYGWIN__
		char cwd_win32 [MAXPATHLEN + 1];
		cygwin_conv_to_win32_path (cwd, cwd_win32);
		generate_build_tree (config, cwd_win32, install_prefix);
#else
		generate_build_tree (config, cwd, install_prefix);
#endif
		config->generate_config_headers (install_prefix.empty () ? "install/include/pkgconf" : install_prefix + "/include/pkgconf");
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_list () {
	bool status = false;
	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);

		// list the installed packages
		std::vector<std::string> packages = pkgdata->get_packages ();
		std::sort (packages.begin (), packages.end ());
		for (unsigned int package = 0; package < packages.size (); package++) {
			const std::vector<std::string> & aliases = pkgdata->get_package_aliases (packages [package]);
			printf ("Package %s (%s):\n aliases:", packages [package].c_str (), aliases [0].c_str ());
			for (unsigned int alias = 1; alias < aliases.size (); alias++) {
				printf (" %s", aliases [alias].c_str ());
			}
			const std::vector<std::string> & versions = pkgdata->get_package_versions (packages [package]);
			printf ("\n versions:");
			for (unsigned int version = 0; version < versions.size (); version++) {
				printf (" %s", versions [version].c_str ());
			}
			printf ("\n");
		}

		// list the available targets
		std::vector<std::string> targets = pkgdata->get_targets ();
		std::sort (targets.begin (), targets.end ());
		for (unsigned int target = 0; target < targets.size (); target++) {
			const std::vector<std::string> & aliases = pkgdata->get_target_aliases (targets [target]);
			printf ("Target %s (%s):\n aliases:", targets [target].c_str (), aliases [0].c_str ());
			for (unsigned int alias = 1; alias < aliases.size (); alias++) {
				printf (" %s", aliases [alias].c_str ());
			}
			printf ("\n");
		}

		// list the available templates
		std::vector<std::string> templates = pkgdata->get_templates ();
		std::sort (templates.begin (), templates.end ());
		for (unsigned int templ = 0; templ < templates.size (); templ++) {
			const std::vector<std::string> & versions = pkgdata->get_template_versions (templates [templ]);
			printf ("Template %s:\n versions:", templates [templ].c_str ());
			for (unsigned int version = 0; version < versions.size (); version++) {
				printf (" %s", versions [version].c_str ());
			}
			printf ("\n");
		}

		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_check () {
	bool status = false;
	unsigned int n;

	try {
		CdlTransactionBody::disable_automatic_inference ();
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		config->save (savefile); // tidy up any manual edits

		// report current target and template
		printf ("Target: %s\n", config->get_hardware ().c_str ());
		printf ("Template: %s\n", config->get_template ().c_str ());
		std::vector<std::string> template_packages = pkgdata->get_template_packages (config->get_template ());
		const std::vector<std::string> & hardware_packages = pkgdata->get_target_packages (config->get_hardware ());
		for (n = 0; n < hardware_packages.size (); n++) {
			template_packages.push_back (hardware_packages [n]);
		}

		// report loaded packages not in the templates
		const std::vector<CdlLoadable> & loadables = config->get_loadables ();
		std::vector<std::string> added_packages;
		std::vector<CdlLoadable>::const_iterator loadable_i;
		for (loadable_i = loadables.begin (); loadable_i != loadables.end (); loadable_i++) {
			const CdlNode & node = dynamic_cast<CdlNode> (* loadable_i);
			if (template_packages.end () == std::find (template_packages.begin (), template_packages.end (), node->get_name ())) {
				added_packages.push_back (node->get_name ());
			}
		}
		if (added_packages.size ()) {
			printf ("Added:\n");
		}
		for (n = 0; n < added_packages.size (); n++) {
			printf (" %s\n", added_packages [n].c_str ());
		}

		// report template packages not in the configuration
		std::vector<std::string> removed_packages;
		for (n = 0; n < template_packages.size (); n++) {
			if (! config->lookup (template_packages [n])) {
				removed_packages.push_back (template_packages [n]);
			}
		}
		if (removed_packages.size ()) {
			printf ("Removed:\n");
		}
		for (n = 0; n < removed_packages.size (); n++) {
			printf (" %s\n", removed_packages [n].c_str ());
		}

		// report packages of non-default version
		std::vector<CdlValuable> version_packages;
		for (loadable_i = loadables.begin (); loadable_i != loadables.end (); loadable_i++) {
			const CdlValuable & valuable = dynamic_cast<CdlValuable> (* loadable_i);
			if (pkgdata->get_package_versions (valuable->get_name ()) [0] != valuable->get_value ()) {
				version_packages.push_back (valuable);
			}
		}
		if (version_packages.size ()) {
			printf ("Version(s):\n");
		}
		for (n = 0; n < version_packages.size (); n++) {
			printf (" %s %s\n", version_packages [n]->get_name ().c_str (), version_packages [n]->get_value ().c_str ());
		}

		// report conflicts
		const std::list<CdlConflict> & conflicts = config->get_all_conflicts ();
		if (conflicts.size ()) {
			printf ("%u conflict(s):\n", conflicts.size ());
		} else {
			printf ("No conflicts\n");
		}
		std::list<CdlConflict>::const_iterator conf_i;
		for (conf_i = conflicts.begin (); conf_i != conflicts.end (); conf_i++) { // for each conflict
			report_conflict (* conf_i);
		}

		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

bool cdl_exec::cmd_resolve () {
	bool status = false;

	try {
		pkgdata = CdlPackagesDatabaseBody::make (repository, &diagnostic_handler, &diagnostic_handler);
		interp = CdlInterpreterBody::make ();
		config = CdlConfigurationBody::load (savefile, pkgdata, interp, &diagnostic_handler, &diagnostic_handler);
		config->resolve_all_conflicts ();
		config->save (savefile);
		status = true;
	} catch (CdlStringException exception) {
		exception_handler (exception);
	} catch (...) {
		exception_handler ();
	}

	delete_cdl_data ();
	return status;
}

CdlInferenceCallbackResult cdl_exec::inference_callback (CdlTransaction transaction) {
	const std::vector<CdlConflict> & resolved_conflicts = transaction->get_resolved_conflicts ();

	// report resolved conflicts
	if (resolved_conflicts.size ()) {
		printf ("%u conflict(s) resolved:\n", resolved_conflicts.size ());
	}
	for (unsigned int n = 0; n < resolved_conflicts.size (); n++) {
		report_conflict (resolved_conflicts [n]);
	}

	// accept all changes
	return CdlInferenceCallbackResult_Continue;
}

void cdl_exec::report_conflict (CdlConflict conflict) {
	printf (" %s:\n  %s\n", conflict->get_node ()->get_name ().c_str (), conflict->get_explanation ().c_str ());
}

void cdl_exec::diagnostic_handler (std::string message) {
	printf ("%s\n", message.c_str ());
}

void cdl_exec::exception_handler (CdlStringException exception) {
	printf ("%s\n", exception.get_message ().c_str ());
}

void cdl_exec::exception_handler () {
	printf ("Unknown error\n");
}

void cdl_exec::delete_cdl_data () {
	delete config;
	config = NULL;
	delete interp;
	interp = NULL;
	delete pkgdata;
	pkgdata = NULL;
}

std::string cdl_exec::resolve_package_alias (const std::string alias) {
	std::string package = alias;

	if (! pkgdata->is_known_package (alias)) { // if the alias is not a package name
		const std::vector<std::string> & packages = pkgdata->get_packages (); // get packages
		for (unsigned int n = 0; n < packages.size (); n++) { // for each package
			const std::vector<std::string> & aliases = pkgdata->get_package_aliases (packages [n]); // get package aliases
			if (aliases.end () != std::find (aliases.begin (), aliases.end (), alias)) { // if alias is found
				package = packages [n]; // note the package
				break;
			}
		}
	}
	return package;
}

std::string cdl_exec::resolve_hardware_alias (const std::string alias) {
	std::string target = alias;

	if (! pkgdata->is_known_target (alias)) { // if the alias is not a target name
		const std::vector<std::string> & targets = pkgdata->get_targets (); // get targets
		for (unsigned int n = 0; n < targets.size (); n++) { // for each target
			const std::vector<std::string> & aliases = pkgdata->get_target_aliases (targets [n]); // get target aliases
			if (aliases.end () != std::find (aliases.begin (), aliases.end (), alias)) { // if alias is found
				target = targets [n]; // note the target
				break;
			}
		}
	}
	return target;
}
