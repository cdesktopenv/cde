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
/* $XConsortium: SearchPath.C /main/3 1995/11/03 12:32:30 rswiston $ */
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

/**********************************************************************
 *
 * SearchPath() - the constructor simply initializes the member data.
 *
 **********************************************************************/
SearchPath::SearchPath
	(
	CDEEnvironment *       user_,
	const char *           envvar,
	const char *           sep
	) : user(user_),
	    environment_var(envvar),
	    separator(sep),
	    parse_state(0),
	    final_search_path(""),
	    norm_search_path("")
{
}

/**********************************************************************
 *
 * ~SearchPath() - the destructor - nothing to do
 *
 **********************************************************************/
SearchPath::~SearchPath()
{
}


/**********************************************************************
 *
 * constructPath - this member function takes a host and a path and
 *		   either calls the Tooltalk filemapping library or
 *		   constructs it in the standard fashion.
 *
 **********************************************************************/
CString SearchPath::ConstructPath
	(
	const CString & path,
	const CString * host,
	unsigned char
	)
{
CString final_path;

    if (host) {
	if (options->doingTooltalk()) {
	    TTFile * file;
	    Try {
		file = new TTFile(*host,path);
		if (!file->ttFileOpFailed())
		    final_path = *file;
		if (options->removeAutoMountPoint()) {
		    if (final_path.contains(options->getAutoMountPoint(),
						"","/"))
			final_path.replace(options->getAutoMountPoint(),"");
		}
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

/*************************************************************************
 *  AssembleString()
 *
 *         Given a series of string elements one at a time--either a host
 *         or a path, in any order--tokenize them, and create
 *         a normalized path of the form:
 *
 *         "host:/path,host:/path"
 *
 *         Return this normalized path in $RETURN_STRING.
 *
 *************************************************************************/
void SearchPath::AssembleString
	(
	const CString & path_element
	) 
{
    if (parse_state == 1 && norm_search_path.length())
	if (norm_search_path[norm_search_path.length()-1] != ',')
	    norm_search_path += ",";

    if (path_element.length() == 0) {

        if (parse_state == 2) {
	    CString path(save_host + ":" + user->SysAdmConfig());
	    if (!norm_search_path.contains(path, ",", ","))
                norm_search_path += path;
            parse_state = 1;
	}
    }     
    else if (path_element[0] == '/') {

	// B -- a path

        if (parse_state == 1) {
	    CString path(user->OS()->LocalHost() + ":" + path_element);
	    if (!norm_search_path.contains(path, ",", ","))
                norm_search_path += path;
            parse_state = 1;
	}
        else if (parse_state == 2) {
	    CString path(save_host + ":" + path_element);
	    if (!norm_search_path.contains(path, ",", ","))
        	norm_search_path += path;
            parse_state = 1;
        }
    }

    else if (path_element[0] == '>') {

	// terminator--write out the saved host and default path.

	CString path(save_host + ":" + user->SysAdmConfig());
	if (!norm_search_path.contains(path, ",", ","))
	    norm_search_path += path;
	parse_state = 3;
    }

    else {

	//  A -- a host

        if (parse_state == 1) {
            save_host = path_element;
            parse_state = 2;
	}

        else if (parse_state == 2) {
	    CString path(save_host + ":" + user->SysAdmConfig());
	    if (!norm_search_path.contains(path, ",", ","))
                norm_search_path += path;
            save_host = path_element;
            parse_state = 2;
	}
    }
}

/*****************************************************************
 *  NormalizePath()
 * 
 *  This function takes a complete string and calls the helper
 *  function AssembleString() to build a normalized path.
 *
 *****************************************************************/
void SearchPath::NormalizePath()
{
    parse_state = 1;

    if (search_path.length()) {
	CTokenizedString subpath(search_path,":,");
	CString dirname = subpath.next();
	while (!subpath.Finished()) {
            AssembleString (dirname);
	    dirname = subpath.next();
	}
    }

    // flush the buffer

    if (parse_state == 2)
	AssembleString(">");
    else
	return;
}


/*********************************************************************
 *  AddToPath()
 *
 *     Take the second parameter (if it exists), and add the
 *     first parameter (always assumed) to its end separated by the
 *     specified separator
 *********************************************************************/

void SearchPath::AddToPath
	(
	const CString & path
	)
{
    if (!final_search_path.isNull()) {
	if (!final_search_path.contains (path, Separator().data(),
					       Separator().data())) {
	    final_search_path += Separator();
	    final_search_path += path;
	}
    }
    else
	final_search_path += path;
}

/*********************************************************************
 *  TraversePath()
 *
 *     Parse a given search path, using comma (,) and colon (:) as
 *     delimiters.  Pass the normalized path to the virtual MakePath.
 *     function.
 *
 *     N.B.  We assume that we have a normalized path at this point.
 *
 *********************************************************************/
void SearchPath::TraversePath()
{
    if (!norm_search_path.isNull()) {
	CTokenizedString subpath(norm_search_path,",");
	CString dirname = subpath.next();
	while (!dirname.isNull()) {
	    MakePath (dirname);
	    dirname = subpath.next();
	}
    }
}


/*********************************************************************
 *  AddPredefinedPath()
 *
 *    If it is possible that an environment variable can be set
 *    before dtsearchpath is run, check for its existence, and if it
 *    exists, append it to the searchpath.
 *
 *********************************************************************/
void SearchPath::AddPredefinedPath()
{
    CString envVar_in(environment_var);
    envVar_in += "SEARCHPATH";
    CString envVar_out = user->OS()->getEnvironmentVariable(envVar_in.data());
    if (!envVar_out.isNull()) {
	CString specified_path(envVar_out);
	if (!specified_path.contains(search_path))
	    search_path += Separator() + specified_path;
	else
	    search_path = specified_path;
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
int SearchPath::validSearchPath
	(
	const CString & st
	) const
{
    if (st == user->SysAdmConfig())
	return 1;

    if (st == user->FactoryInstall())
	return 1;

    // If this is an APP-specified path, it also needs to be massaged.

    // The elements in the APP paths may be host:/path so use the 
    // colon as the leader.

    if (user->DTAPPSP() && 
	user->DTAPPSP()->contains (st, ":", separator.data()))
	return 1;

    if (user->DTUSERAPPSP() && 
	user->DTUSERAPPSP()->contains (st, ":", separator.data()))
	return 1;

    // they may also be there by themselves: "host:,/path" so use the
    // comma as the leader.

    if (user->DTAPPSP() && 
	user->DTAPPSP()->contains (st, ",", separator.data()))
	return 1;

    if (user->DTUSERAPPSP() && 
	user->DTUSERAPPSP()->contains (st, ",", separator.data()))
	return 1;

    return 0;
}

/*********************************************************************
 *  useSystemPath()
 *
 *    This member function verifies whether system environment variable
 *    should be left unmodified, since some other configuraton mechanism
 *    is in effect.
 *
 */
int SearchPath::useSystemPath()
{
    return 0;
}

/*****************************************************************
 *  ExportPath()
 *
 *         Export the variable value to the rest of the session.
 *
 *****************************************************************/
void SearchPath::ExportPath()
{
    CString env(environment_var);
    env += "SEARCHPATH";

    if (!useSystemPath()) {
      user->OS()->shell()->putToEnv(env, final_search_path.data());
    }
}


/*****************************************************************
 *  Print()
 *
 *    In the absence of iostreams, use printf to output information
 *
 *****************************************************************/
void SearchPath::Print()
{
    printf("%sSEARCHPATH:\n", GetEnvVar());
    CString sp(GetSearchPath());
    if (!useSystemPath() && !sp.isNull()) {
	CTokenizedString path (sp,Separator().data());
	CString subpath = path.next();
	while (!subpath.isNull()) {
	    printf("\t%s\n",subpath.data());
	    subpath = path.next();
	}
        printf("\n");
    }
}

/****************************************************************
 *  PrettyPrint()
 *
 *    Print the list such that each entry occupys its own line
 *
 ****************************************************************/

#if defined(linux) || defined(CSRG_BASED)
void SearchPath::PrettyPrint
	(
	std::ostream & os
	) const
#else
void SearchPath::PrettyPrint
	(
	ostream & os
	) const
#endif
{
    CTokenizedString path (GetSearchPath(), Separator().data());
    CString subpath = path.next();
    while (!subpath.isNull()) {
#if defined(linux) || defined(CSRG_BASED)
	os << "        " << subpath << std::endl;
#else
	os << "        " << subpath << endl;
#endif
	subpath = path.next();
    }
#if defined(linux) || defined(CSRG_BASED)
    os << std::endl;
#else
    os << endl;
#endif
}


/****************************************************************
 *  operator<<()
 *
 *    Allow SearchPath types to be output using iostreams.
 *
 ****************************************************************/

#if defined(linux) || defined(CSRG_BASED)
std::ostream & operator<< 
	(
	std::ostream & os, 
	const SearchPath & sp
	)
#else
ostream & operator<< 
	(
	ostream & os, 
	const SearchPath & sp
	)
#endif
{
#if defined(linux) || defined(CSRG_BASED)
    os << sp.GetEnvVar() << "SEARCHPATH:" << std::endl;
#else
    os << sp.GetEnvVar() << "SEARCHPATH:" << endl;
#endif
    sp.PrettyPrint(os);
    return os;
}

