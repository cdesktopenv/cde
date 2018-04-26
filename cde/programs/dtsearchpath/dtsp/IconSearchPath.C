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
/* $XConsortium: IconSearchPath.C /main/3 1995/11/03 12:31:13 rswiston $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "SearchPath.h"
#include <stdlib.h>
#include "TTFile.h"
#include "Options.h"

extern Options * options;

/**********************************************************************
 *
 * IconSearchPath
 *
 *	This constructor creates DTICONSEARCHPATH in a three-step 
 *	process.
 *
 *	1. gathers environment variables and defaults to create a 
 *	   colon-separated list of paths
 *	2. normalizes the list into host:/path format
 *	3. builds the final version of the path
 *
 *      Hierarchy of search paths:
 *		DTSPUSERICON
 *		User's home directory
 *		System Administrator's configuration directory
 *		DTSPSYSICON
 *		Factory location
 *
 **********************************************************************/
IconSearchPath::IconSearchPath
	(
	CDEEnvironment * user,
	const char *     envvar,
	const char *     ist,
	const char *     znd,
	const char *     sep
	) : SearchPath(user,envvar,sep),
	    first(ist),
	    second(znd)
{
    if (user->DTICONSP()) {
	if (user->DTUSERICONSP()) {
            search_path = *user->DTUSERICONSP() + ",";
	    if (user->DTUSERAPPSP())
		search_path += *user->DTUSERAPPSP() + ",";
	    search_path += user->HOME() + ",";
	    if (!user->DTICONSP()->contains(user->OS()->LocalHost(),",",":") &&
		!user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTICONSP() + ",";
	    if (user->DTICONSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP()->contains(user->SysAdmConfig(),",",","))
		search_path.replace(user->OS()->LocalHost() + ":,","");
	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";
	    search_path += user->SysAdmConfig() + "," +
			   user->FactoryInstall();
	}
        else {
	    if (user->DTUSERAPPSP())
		search_path = *user->DTUSERAPPSP() + ",";
            search_path += user->HOME() + ",";
	    if (!user->DTICONSP()->contains(user->OS()->LocalHost(),",",":") &&
		!user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":"))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTICONSP() + ",";
	    if (user->DTICONSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP()->contains(user->SysAdmConfig(),",",","))
		search_path.replace(user->OS()->LocalHost() + ":,","");
	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";
	    search_path += user->SysAdmConfig() + "," +
			   user->FactoryInstall();
	}
    }
    else if (user->DTUSERICONSP()) {
	search_path = *user->DTUSERICONSP() + ",";
	if (user->DTUSERAPPSP())
	    search_path += *user->DTUSERAPPSP() + ",";
	search_path += *user->DTAPPSP();
    }
    else {
	if (user->DTUSERAPPSP())
	    search_path = *user->DTUSERAPPSP() + ",";
	search_path += *user->DTAPPSP();
    }

    // NormalizePath should remove duplicates
    AddPredefinedPath();

    if (options->CheckingUser())
	search_path = *user->DTAPPSP();

    NormalizePath();
    TraversePath();
}


/************************************************************************
 *  MakeIconSearchPath()
 *
 *     Given a search path element (host name:path name pair), construct
 *     the appropriate path for this particular desktop subsystem:
 *
 *     DTICONSEARCHPATH
 *
 *     A path is constructed so that each host:/path pair is appended.
 ************************************************************************/
void IconSearchPath::MakePath
	(
	const CString & pair
	) 
{
    CTokenizedString element(pair,":");
    CString host_element = element.next();
    CString path_element = element.next();

    if (path_element.contains(user->HOME())) {
        if (host_element == user->OS()->LocalHost()) {
            if (user->OS()->isDirectory(path_element + "/icons")
		 || options->dontOptimize()) {
                AddToPath (path_element + "/icons/%B%M" + first);
                AddToPath (path_element + "/icons/%B%M" + second);
                AddToPath (path_element + "/icons/%B");
	    }
	}
        else {
	    CString dir(ConstructPath(path_element + "/icons", &host_element));
            if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
                AddToPath (dir + "/%B%M" + first);
                AddToPath (dir + "/%B%M" + second);
                AddToPath (dir + "/%B");
	    }
        }
    }
    else {
        if (host_element == user->OS()->LocalHost()) {
	    CString dirname(path_element);
	    if (validSearchPath(dirname)) {
		dirname += "/icons";
	        if (user->OS()->isDirectory (dirname)
		     || options->dontOptimize()) {
		    AddToPath (dirname + "/%L/%B%M" + first);
		    AddToPath (dirname + "/%L/%B%M" + second);
		    AddToPath (dirname + "/%L/%B");
		}
		dirname += "/C";
	    }
	    if (user->OS()->isDirectory (dirname) || options->dontOptimize()) {
		AddToPath (dirname + "/%B%M" + first);
		AddToPath (dirname + "/%B%M" + second);
		AddToPath (dirname + "/%B");
	    }		
	}
        else {
	    CString dir = ConstructPath(path_element, &host_element);
	    if (validSearchPath(dir)) {
		dir += "/icons";
		if (user->OS()->isDirectory (dir) || options->dontOptimize()) {
		    AddToPath (dir + "/%L/%B%M" + first);
		    AddToPath (dir + "/%L/%B%M" + second);
		    AddToPath (dir + "/%L/%B");
		}
		dir += "/C";
	    }
	    if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
		AddToPath (dir + "/%B%M" + first);
		AddToPath (dir + "/%B%M" + second);
		AddToPath (dir + "/%B");
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
int IconSearchPath::validSearchPath
	(
	const CString & st
	) const
{
    if (st == user->SysAdmConfig())	// ...,/etc/dt/appconfig,...
	return 1;

    if (st == user->FactoryInstall())	// ...,/usr/dt/appconfig,...
	return 1;

    // ...,/nfs/machine/etc/dt/appconfig,...

    if (st.contains(user->SysAdmConfig(),"",Separator().data()))
	return 1;

    // ...,/nfs/machine/usr/dt/appconfig,...

    if (st.contains(user->FactoryInstall(),"",Separator().data()))
	return 1;

    // If this is an APP-specified path, it also needs to be massaged.
    // The elements in the APP paths were originally specified using 
    // host:/path, but were converted to /path for Icon searchpaths
    // so use the comma as the leader and the trailer.

    if (user->DTAPPSP() && 
	user->DTAPPSP()->contains (st, ",", ","))
	return 1;

    if (user->DTUSERAPPSP() && 
	user->DTUSERAPPSP()->contains (st, ",", ","))
	return 1;

    return 0;
}
