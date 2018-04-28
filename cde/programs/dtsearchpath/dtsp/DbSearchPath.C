/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: DbSearchPath.C /main/2 1995/07/17 14:09:00 drk $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "SearchPath.h"
#include "Environ.h"
#include "TTFile.h"
#include "Options.h"

extern Options * options;

/**********************************************************************
 *
 * DatabaseSearchPath - this constructor creates DTDATAVASESEARCHPATH 
 *			in a three-step process.
 *
 *			1. gathers environment variables and defaults
 *			   to create a comma-separated list of paths
 *			2. normalizes the list into host:/path format
 *			3. builds the final version of the path
 *
 *                 Hierarchy of search paths:
 *			DTSPUSERDATABASEHOSTS
 *			User's home directory
 *			System Administrator's configuration directory
 *			DTSPSYSDATABASEHOSTS
 *			Factory location
 *
 **********************************************************************/
DatabaseSearchPath::DatabaseSearchPath
	(
	CDEEnvironment * user,
	const char *     envvar,
	const char *     sep
	) : SearchPath(user, envvar, sep)
{
    if (user->DTDBSP()) {
	if (user->DTUSERDBSP()) {

	    // Add the DTSPUSER and Home paths

            search_path = *user->DTUSERDBSP() + ",";
	    if (user->DTUSERAPPSP())
		search_path += *user->DTUSERAPPSP() + ",";
	    search_path += user->HOME() + ",";

	    // if localhost: is not specified in the database or app
	    // input paths, add the local configuration directory here.

	    if (!user->DTDBSP()->contains(user->OS()->LocalHost(),",",":") &&
		(!user->DTAPPSP() ||
		 !user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":")))
		search_path += user->SysAdmConfig() + ",";

	    // now, add the system database path

	    search_path += *user->DTDBSP() + ",";

	    // before adding the app path, see if localhost: is specified 
	    // in both paths and if it is, remove the first occurrence of
	    // localhost:

	    if (user->DTDBSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP() &&
		user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path.replace(user->OS()->LocalHost() + ":,","");

	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";

	    // finally, add the factory location

	    search_path += user->FactoryInstall();
	}
        else {
	    if (user->DTUSERAPPSP())
		search_path = *user->DTUSERAPPSP() + ",";
            search_path += user->HOME() + ",";
	    if (!user->DTDBSP()->contains(user->OS()->LocalHost(),",",":") &&
		(!user->DTAPPSP() ||
		 !user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":")))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTDBSP() + ",";
	    if (user->DTDBSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP() &&
		user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path.replace(user->OS()->LocalHost() + ":,","");
	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";
	    search_path += user->FactoryInstall();
	}
    }
    else if (user->DTUSERDBSP()) {
        search_path = *user->DTUSERDBSP() + ",";
	if (user->DTUSERAPPSP())
	    search_path += *user->DTUSERAPPSP() + ",";
	search_path += user->HOME() + ",";
	if (!user->DTAPPSP() ||
	    !user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
	    search_path += user->SysAdmConfig() + ",";
	if (user->DTAPPSP())
	    search_path += *user->DTAPPSP() + ",";
	search_path += user->FactoryInstall();
    }
    else {
	if (user->DTUSERAPPSP())
	    search_path = *user->DTUSERAPPSP() + ",";
        search_path += user->HOME() + ",";
	if (!user->DTAPPSP() ||
	    !user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
	    search_path += user->SysAdmConfig() + ",";
	if (user->DTAPPSP())
	    search_path += *user->DTAPPSP() + ",";
	search_path += user->FactoryInstall();
    }

    if (options->CheckingUser()) {
	if (user->DTUSERAPPSP())
            search_path = *user->DTUSERAPPSP();
	if (user->DTAPPSP())
            search_path += *user->DTAPPSP();
	else
	    search_path += user->DefaultSearchPath();
    }

    NormalizePath();
    TraversePath();
}


CString DatabaseSearchPath::ConstructPath
	(
	const CString & path,
	const CString * host,
	unsigned char   useTT
	)
{
    if (useTT) {
	CString final_path;
	if (host) {
	    if (options->doingTooltalk()) {
		TTFile * file;
		Try {
		    file = new TTFile(*host,path);
		    if (!file->ttFileOpFailed())
			final_path = *file;
		}
		Catch (TTFile::TT_Exception *, file) {
	            final_path = user->OS()->MountPoint() + *host + path;
		}
		delete file;
	    }
	    else 
		final_path = user->OS()->MountPoint() + *host + path;
	}
	else
	    final_path = path;
	return final_path;
    }

    if (host)
	return (*host + ":" + path);

    return path;
}

/************************************************************************
 *  MakeActionSearchPath()
 *
 *     Given a search path element (host name:path name pair), construct
 *     the appropriate path for this particular desktop subsystem:
 *
 *     DTDATABASESEARCHPATH
 *
 *     A path is constructed so that each host:/path pair is appended.
 ************************************************************************/
void DatabaseSearchPath::MakePath
	(
	const CString & pair
	) 
{
    CTokenizedString element(pair,":");
    CString host_e = element.next();
    CString path_e = element.next();

    if (path_e.contains(user->HOME())) {
        if (host_e == user->OS()->LocalHost()) {
	    if (user->OS()->isDirectory(path_e + "/types")
		 || options->dontOptimize())
                AddToPath (path_e + "/types");
	}
        else {
	    CString dir(ConstructPath(path_e + "/types", &host_e, 1));
	    if (user->OS()->isDirectory(dir) || options->dontOptimize())
		AddToPath (dir);
	}
    }
    else if (host_e == user->OS()->LocalHost()) {
	CString dir(path_e);
	if (validSearchPath(dir)) {
	    dir += "/types";
	    if (user->OS()->isDirectory(dir) || options->dontOptimize())
		AddToPath (dir + "/%L");
	    dir += "/C";
	}
	if (user->OS()->isDirectory(dir) || options->dontOptimize())
	    AddToPath (dir);
    }
    else {
	CString dirname(ConstructPath(path_e, &host_e,1));
	if (validSearchPath(path_e)) {
	    dirname += "/types";
	    if (user->OS()->isDirectory(dirname) || options->dontOptimize())
	        AddToPath (ConstructPath(path_e + "/types/%L", &host_e));
	    dirname += "/C";
	    if (user->OS()->isDirectory(dirname) || options->dontOptimize())
                AddToPath (ConstructPath(path_e + "/types/C", &host_e));
	}
	else {
	    if (user->OS()->isDirectory(dirname) || options->dontOptimize())
                AddToPath (ConstructPath(path_e, &host_e));
	}
    }
}

