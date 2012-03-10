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

ostream & operator<< 
	(
	ostream & os, 
	const ManSearchPath & sp
	)
{
    os << sp.GetEnvVar() << endl;
    sp.PrettyPrint(os);
    return os;
}
