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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: ManSearchPath.C /main/3 1995/11/03 12:31:46 rswiston $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "SearchPath.h"
#include "TTFile.h"
#include "Options.h"

extern Options * options;

ManSearchPath::ManSearchPath
	(
	CDEEnvironment * user,
	const char *     envvar,
	const char *     sep
	) : SearchPath(user, envvar, sep)
{
#if defined(__FreeBSD__)
    /* Installer on FreeBSD sets up man configuration so that
     * setting MANPATH is not necessary
     */
    if (!user->OS()->MANPATH().isNull()) {
#endif
    if (user->DTMANPATH())
	search_path = user->FactoryManPath() + "," + *user->DTMANPATH();
    else
	search_path = user->FactoryManPath();

    // add OS manpath now so duplicate path elements can be removed by Normalize
    // Normalize should recognize both ':' and ',' as separators.

    if (!user->OS()->MANPATH().isNull())
        search_path += ":" + user->OS()->MANPATH();

    NormalizePath();
    TraversePath();
#if defined(__FreeBSD__)
    }
#endif
}


/************************************************************************
 *  MakePath()
 *
 *     Given a search path element (host name:path name pair), construct
 *     the appropriate path for this particular desktop subsystem:
 *
 *     DTMANPATH
 *
 *     A path is constructed so that each host:/path pair is appended.
 ************************************************************************/
void ManSearchPath::MakePath
	(
	const CString & pair
	) 
{
    CTokenizedString element(pair,":");
    CString host_element = element.next();
    CString path_element = element.next();

    if (host_element == user->OS()->LocalHost()) {
	if (user->OS()->isDirectory(path_element) || options->dontOptimize())
	    AddToPath (path_element);
    }
    else {
	CString dirname(ConstructPath(path_element, &host_element));
	if (user->OS()->isDirectory(dirname) || options->dontOptimize())
            AddToPath (dirname);
    }
}


/*****************************************************************
 *  ExportPath()
 *
 *         Export the variable value to the rest of the session.
 *
 *****************************************************************/
void ManSearchPath::ExportPath()
{
    CString env(GetEnvVar());
    user->OS()->shell()->putToEnv(env,
		       final_search_path.data());
}


void ManSearchPath::Print()
{
    printf("%s:\n", GetEnvVar());
    CString sp(GetSearchPath());
    if (!sp.isNull()) {
	CTokenizedString path (sp,Separator().data());
	CString subpath = path.next();
	while (!subpath.isNull()) {
	    printf("\t%s\n",subpath.data());
	    subpath = path.next();
	}
        printf("\n");
    }
}

#if defined(linux)
std::ostream & operator<< 
	(
	std::ostream & os, 
	const ManSearchPath & sp
	)
#else
ostream & operator<< 
	(
	ostream & os, 
	const ManSearchPath & sp
	)
#endif
{
#if defined(linux)
    os << sp.GetEnvVar() << std::endl;
#else
    os << sp.GetEnvVar() << endl;
#endif
    sp.PrettyPrint(os);
    return os;
}
