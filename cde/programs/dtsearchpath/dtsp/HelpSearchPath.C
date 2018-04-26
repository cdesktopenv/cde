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
/* $XConsortium: HelpSearchPath.C /main/3 1995/11/03 12:30:59 rswiston $ */
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
 * HelpSearchPath
 *
 *	this constructor creates DTHELPSEARCHPATH in a three-step 
 *	process.
 *
 *	1. gathers environment variables and defaults to create a 
	   colon-separated list of paths
 *	2. normalizes the list into host:/path format
 *	3. builds the final version of the path
 *
 *	Hierarchy of search paths:
 *		DTSPUSERHELP
 *		User's home directory
 *		System Administrator's configuration directory
 *		DTSPSYSHELP
 *		Factory location
 *
 **********************************************************************/
HelpSearchPath::HelpSearchPath
	(
	CDEEnvironment * user,
	const char *     envvar,
	const char *     sep
	) : SearchPath(user, envvar, sep)
{
    if (user->DTHELPSP()) {
	if (user->DTUSERHELPSP()) {
	    search_path = *user->DTUSERHELPSP() + ",";
	    if (user->DTUSERAPPSP())
		search_path += *user->DTUSERAPPSP() + ",";
	    search_path += user->HOME() + ",";
	    if (!user->DTHELPSP()->contains(user->OS()->LocalHost(),",",":") &&
		!user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTHELPSP() + ",";
	    if (user->DTHELPSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP()->contains(user->SysAdmConfig(),",",","))
		search_path.replace(user->OS()->LocalHost() + ":,","");
	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";
	    search_path += user->FactoryInstall();
	}
        else {
	    if (user->DTUSERAPPSP())
		search_path = *user->DTUSERAPPSP() + ",";
	    search_path += user->HOME() + ",";
	    if (!user->DTHELPSP()->contains(user->OS()->LocalHost(),",",":") &&
		!user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTHELPSP() + ",";
	    if (user->DTHELPSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP()->contains(user->SysAdmConfig(),",",","))
		search_path.replace(user->OS()->LocalHost() + ":,","");
	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";
	    search_path += user->FactoryInstall();
	}
    }
    else if (user->DTUSERHELPSP()) {
	search_path = *user->DTUSERHELPSP() + ",";
	if (user->DTUSERAPPSP())
	    search_path += *user->DTUSERAPPSP() + ",";
	search_path += *user->DTAPPSP();
    }
    else {
	if (user->DTUSERAPPSP())
	    search_path = *user->DTUSERAPPSP() + ",";
	search_path += *user->DTAPPSP();
    }

    if (options->CheckingUser())
	search_path = *user->DTAPPSP();

    NormalizePath();
    TraversePath();

#if defined(hpux)
    AddToPath ("/etc/vhelp/%T/%L/%H");
    AddToPath ("/etc/vhelp/%T/%H");
    AddToPath ("/etc/vhelp/%T/%L/%H.hv");
    AddToPath ("/etc/vhelp/%T/%H.hv");
    AddToPath ("/etc/vhelp/%T/C/%H");
    AddToPath ("/etc/vhelp/%T/C/%H.hv");
    AddToPath ("/usr/vhelp/%T/%L/%H");
    AddToPath ("/usr/vhelp/%T/%H");
    AddToPath ("/usr/vhelp/%T/%L/%H.hv");
    AddToPath ("/usr/vhelp/%T/%H.hv");
    AddToPath ("/usr/vhelp/%T/C/%H");
    AddToPath ("/usr/vhelp/%T/C/%H.hv");
#endif
}


/***********************************************************************
 *  MakePath()
 *
 *     Given a search path element (host name:path name pair), construct
 *     the appropriate path for this particular desktop subsystem:
 *
 *     DTHELPSEARCHPATH
 *
 *     A path is constructed so that each host:/path pair is appended.
 ***********************************************************************/
void HelpSearchPath::MakePath
	(
	const CString & pair
	) 
{
    CTokenizedString element(pair,":");
    CString host_element = element.next();
    CString path_element = element.next();

    if (path_element.contains(user->HOME())) {
        if (host_element == user->OS()->LocalHost()) {
	    CString helpPath(path_element);
	    if (user->OS()->isDirectory(helpPath + "/help")
		 || options->dontOptimize()) {
                AddToPath (helpPath + "/help/" + user->UserHostDir() + "/%H");
                AddToPath (helpPath + "/help/" + user->UserHostDir() + "/%H.sdl");
                AddToPath (helpPath + "/help/" + user->UserHostDir() + "/%H.hv");
                AddToPath (helpPath + "/help/%H");
                AddToPath (helpPath + "/help/%H.sdl");
                AddToPath (helpPath + "/help/%H.hv");
	    }
	}
        else {
	    CString dir(ConstructPath(path_element + "/help", &host_element));
	    if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
                AddToPath (dir + "/" + user->UserHostDir() + "/%H");
                AddToPath (dir + "/" + user->UserHostDir() + "/%H.sdl");
                AddToPath (dir + "/" + user->UserHostDir() + "/%H.hv");
                AddToPath (dir + "/%H");
                AddToPath (dir + "/%H.sdl");
                AddToPath (dir + "/%H.hv");
	    }
	}
    }
    else {
	if (host_element == user->OS()->LocalHost()) {
	    CString helpPath(path_element);
	    if (validSearchPath(helpPath)) {
		helpPath += "/help";
	        if (user->OS()->isDirectory(helpPath)
		     || options->dontOptimize()) {
		    AddToPath (helpPath + "/%L/%H");
		    AddToPath (helpPath + "/%L/%H.sdl");
		    AddToPath (helpPath + "/%L/%H.hv");
		}
		helpPath += "/C";
	    }
	    if (user->OS()->isDirectory(helpPath) || options->dontOptimize()) {
		AddToPath (helpPath + "/%H");
		AddToPath (helpPath + "/%H.sdl");
		AddToPath (helpPath + "/%H.hv");
	    }
	}
	else {
	    CString dir(ConstructPath(path_element, &host_element));
	    if (validSearchPath(dir)) {
		dir += "/help";
		if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
                    AddToPath (dir + "/%L/%H");
	            AddToPath (dir + "/%L/%H.sdl");
	            AddToPath (dir + "/%L/%H.hv");
		}
		dir += "/C";
            }
	    if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
                AddToPath (dir + "/%H");
                AddToPath (dir + "/%H.sdl");
                AddToPath (dir + "/%H.hv");
            }
	}
    }
}

/*********************************************************************
 *  validSearchPath()
 *
 *    This member function verifies that the path in question is a
 *    standard CDE location, i.e. /etc/dt/appconfig or /usr/dt/appconfig
 *    so that the appropriate massaging can take place.
 *
 *********************************************************************/
int HelpSearchPath::validSearchPath
	(
	const CString & st
	) const
{
    if (st == user->SysAdmConfig())	// ...,/etc/dt/appconfig,...
	return 1;

    if (st == user->FactoryInstall())	// ...,/usr/dt/appconfig,...
	return 1;

    // ...,/nfs/machine/etc/dt/appconfig,...

    if (st.contains(user->SysAdmConfig(), "", Separator().data()))
	return 1;

    // ...,/nfs/machine/usr/dt/appconfig,...

    if (st.contains(user->FactoryInstall(),"",Separator().data()))
	return 1;

    // If this is an APP-specified path, it also needs to be massaged.
    // The elements in the APP paths were originally specified using 
    // host:/path, but were converted to /path for Help searchpaths
    // so use the comma as the leader and the trailer.

    if (user->DTAPPSP() && 
	user->DTAPPSP()->contains (st, ",", ","))
	return 1;

    if (user->DTUSERAPPSP() && 
	user->DTUSERAPPSP()->contains (st, ",", ","))
	return 1;

    return 0;
}
