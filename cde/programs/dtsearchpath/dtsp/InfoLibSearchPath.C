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
/* $TOG: InfoLibSearchPath.C /main/5 1998/08/17 10:33:55 mgreess $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <stdlib.h>
#include "SearchPath.h"
#include "Environ.h"
#include "TTFile.h"
#include "Options.h"

extern Options * options;

/**********************************************************************
 *
 * InfoLibSearchPath
 *
 *	this constructor creates DTINFOLIBSEARCHPATH in a three-step 
 *	process.
 *
 *	1. gathers environment variables and defaults to create a 
 *	   colon-separated list of paths
 *	2. normalizes the list into host:/path format
 *	3. builds the final version of the path
 *
 *	Hierarchy of search paths:
 *		DTSPUSERINFOLIB
 *		DTSPUSERAPPHOSTS
 *		System Administrator's configuration directory
 *		DTSPSYSINFOLIB
 *		DTSPSYSAPPHOSTS
 *		Factory location
 *
 **********************************************************************/
InfoLibSearchPath::InfoLibSearchPath
	(
	CDEEnvironment * user,
	const char *     envvar,
	const char *     sep
	) : SearchPath(user, envvar, sep)
{
    CString oldSysAdmConfig = user->sysAdmConfig;
    CString oldFactoryInstall = user->factoryInstall;

    // Need to re-initialize the defaults.
    user->sysAdmConfig   = "/etc/dt";
    user->factoryInstall = "/usr/dt";

    if (user->DTINFOLIBSP()) {
	if (user->DTUSERINFOLIBSP()) {
	    search_path = *user->DTUSERINFOLIBSP() + ",";
	    if (user->DTUSERAPPSP())
		search_path += *user->DTUSERAPPSP() + ",";
	    if (!user->DTINFOLIBSP()->contains(user->OS()->LocalHost(),",",":") &&
		(!user->DTAPPSP() ||
		 !user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":")))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTINFOLIBSP() + ",";
	    if (user->DTINFOLIBSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP() &&
		user->DTAPPSP()->contains(user->SysAdmConfig(),",",","))
		search_path.replace(user->OS()->LocalHost() + ":,","");
	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";
	    search_path += user->FactoryInstall();
	}
        else {
	    if (user->DTUSERAPPSP())
		search_path = *user->DTUSERAPPSP() + ",";
	    if (!user->DTINFOLIBSP()->contains(user->OS()->LocalHost(),",",":") &&
		(!user->DTAPPSP() ||
		 !user->DTAPPSP()->contains(user->OS()->LocalHost(),",",":")))
		search_path += user->SysAdmConfig() + ",";
	    search_path += *user->DTINFOLIBSP() + ",";
	    if (user->DTINFOLIBSP()->contains(user->OS()->LocalHost(),",",":") &&
		user->DTAPPSP() &&
		user->DTAPPSP()->contains(user->SysAdmConfig(),",",","))
		search_path.replace(user->OS()->LocalHost() + ":,","");
	    if (user->DTAPPSP())
		search_path += *user->DTAPPSP() + ",";
	    search_path += user->FactoryInstall();
	}
    }
    else if (user->DTUSERINFOLIBSP()) {
	search_path = *user->DTUSERINFOLIBSP() + ",";

	if (user->DTUSERAPPSP())
	    search_path += *user->DTUSERAPPSP() + ",";

	search_path += user->SysAdmConfig() + ",";
	if (user->DTAPPSP())
	    search_path += *user->DTAPPSP() + ",";
	search_path += user->FactoryInstall();
    }
    else {
	if (user->DTUSERAPPSP())
	    search_path = *user->DTUSERAPPSP() + ",";

	search_path += user->SysAdmConfig() + ",";
	if (user->DTAPPSP())
	    search_path += *user->DTAPPSP() + ",";
	search_path += user->FactoryInstall();
    }

    if (options->CheckingUser())
    {
	if (user->DTAPPSP())
	    search_path = *user->DTAPPSP();
    }

    NormalizePath();
    TraversePath();

    user->sysAdmConfig = oldSysAdmConfig;
    user->factoryInstall = oldFactoryInstall;
}


/***********************************************************************
 *  MakePath()
 *
 *     Given a search path element (host name:path name pair), construct
 *     the appropriate path for this particular desktop subsystem:
 *
 *     DTINFOLIBSEARCHPATH
 *
 *     A path is constructed so that each host:/path pair is appended.
 ***********************************************************************/
void InfoLibSearchPath::MakePath
	(
	const CString & pair
	) 
{
    CTokenizedString element(pair,":");
    CString host_element = element.next();
    CString path_element = element.next();

    if (path_element.contains(user->HOME())) {
        if (host_element == user->OS()->LocalHost()) {
	    CString infolibPath(path_element);
	    if (user->OS()->isDirectory(infolibPath + "/infolib")
		 || options->dontOptimize()) {
                AddToPath (infolibPath + "/infolib/" + user->UserHostDir() + 
			   "%I.dti");
                AddToPath (infolibPath + "/infolib/%L/%I.dti");
	    }
	}
        else {
	    CString dir(ConstructPath(path_element + "/infolib", &host_element));
	    if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
                AddToPath (dir + "/" + user->UserHostDir() + "/%I.dti");
                AddToPath (dir + "/%I.dti");
	    }
	}
    }
    else {
	if (host_element == user->OS()->LocalHost()) {
	    CString infolibPath(path_element);
	    if (validSearchPath(infolibPath)) {
		infolibPath += "/infolib";
	        if (user->OS()->isDirectory(infolibPath)
		     || options->dontOptimize()) {
		    AddToPath (infolibPath + "/%L/%I.dti");
		}
		infolibPath += "/C";
	    }
	    if (user->OS()->isDirectory(infolibPath) || 
				options->dontOptimize()) {
		AddToPath (infolibPath + "/%I.dti");
	    }
	}
	else {
	    CString dir(ConstructPath(path_element, &host_element));
	    if (validSearchPath(dir)) {
		dir += "/infolib";
		if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
	            AddToPath (dir + "/%L/%I.dti");
		}
		dir += "/C";
            }
	    if (user->OS()->isDirectory(dir) || options->dontOptimize()) {
                AddToPath (dir + "/%I.dti");
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
int InfoLibSearchPath::validSearchPath
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
    // host:/path, but were converted to /path for InfoLib searchpaths
    // so use the comma as the leader and the trailer.

    if (user->DTAPPSP() && 
	user->DTAPPSP()->contains (st, ",", ","))
	return 1;

    if (user->DTUSERAPPSP() && 
	user->DTUSERAPPSP()->contains (st, ",", ","))
	return 1;

    return 0;
}
