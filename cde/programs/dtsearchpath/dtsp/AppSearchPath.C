/* $XConsortium: AppSearchPath.C /main/2 1995/07/17 14:08:52 drk $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "SearchPath.h"
#include "Environ.h"
#include "Options.h"
#include "TTFile.h"

extern Options * options;

/**********************************************************************
 *
 * AppSearchPath - this constructor creates DTAPPSEARCHPATH in a three-
 *		   step process.
 *
 *			1. gathers environment variables and defaults
 *			   to create a comma-separated list of paths
 *			2. normalizes the list into host:/path format
 *			3. builds the final version of the path
 *
 *                 Hierarchy of search paths:
 *			DTSPUSERAPPHOSTS
 *			User's home directory
 *			System Administrator's configuration directory
 *			DTSPSYSAPPHOSTS
 *			Factory location
 *
 **********************************************************************/
AppSearchPath::AppSearchPath
	(
	CDEEnvironment * user,
	const char *     envvar,
	const char *     sep
	) : SearchPath(user, envvar, sep)
{
    if (user->DTAPPSP()) {
	if (user->DTUSERAPPSP()) {
	    search_path = *user->DTUSERAPPSP() + "," +
			  user->HOME() + ",";
	    if (!user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTAPPSP() + "," +
			   user->FactoryInstall();
	}
	else {
	    search_path = user->HOME() + ",";
	    if (!user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTAPPSP() + "," +
			   user->FactoryInstall();
	}
    }

    else if (user->DTUSERAPPSP())
	search_path = *user->DTUSERAPPSP() + "," + user->DefaultSearchPath();

    else
	search_path = user->DefaultSearchPath();

    if (options->CheckingUser())
	search_path = user->DefaultSearchPath();

    // Now convert the initial list to host:/path format

    NormalizePath();

    // Convert the host:/path list to a colon-separated list of
    // valid paths

    TraversePath();

}

/************************************************************************
 *  MakePath()
 *
 *     Given a search path element (host name:path name pair), construct
 *     the appropriate path for this particular desktop subsystem:
 *
 *     DTAPPSEARCHPATH
 *
 *     A path is constructed so that each host:/path pair is appended.
 ************************************************************************/
void AppSearchPath::MakePath
	(
	const CString & pair
	) 
{
    CTokenizedString element(pair,":,");
    CString host_element = element.next();
    CString path_element = element.next();

    if (host_element == user->OS()->LocalHost()) {
	if (user->OS()->isDirectory(path_element) || options->dontOptimize())
            AddToPath (path_element);
    }
    else {
	CString dir(ConstructPath(path_element, &host_element));
	if (user->OS()->isDirectory(dir) || options->dontOptimize())
            AddToPath (dir);
    }
}


void AppSearchPath::FixUp()
{
    if (!final_search_path.isNull()) {
	CTokenizedString subpath(final_search_path,",");
	CString dirname = subpath.next();
	CString result_sp(dirname);
	while (1) {
	    result_sp += "/appmanager";
	    if (!dirname.contains(user->HOME()))
		result_sp += "/%L:" + dirname + "/appmanager/C";
	    dirname = subpath.next();
	    if (dirname.isNull())
		break;
	    result_sp += ":" + dirname;
	}
	final_search_path = result_sp;
	setSeparator(":");
    }
}
