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
/* $XConsortium: TTFile.C /main/2 1995/07/17 14:10:04 drk $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "TTFile.h"
#include <Tt/tt_c.h>
#include <stdio.h>

TTFile::TTFile
	(
	const CString & host, 
	const CString & path
	) : CString(), status(TT_OK)
{
    char * temp = tt_host_file_netfile(host.data(), path.data());

    if ((status = tt_ptr_error(temp)) != TT_OK)
	Throw (TT_Exception(temp));

    contents = tt_netfile_file(temp);
    tt_free(temp);

    if ((status = tt_ptr_error(contents)) != TT_OK)
	Throw (TT_Exception(contents));
}

TTFile::TTFile
	(
	const TTFile & file
	)
{
    contents = new char [file.length() + 1];
    strcpy(contents,file.data());
    status = file.status;
}

TTFile::~TTFile()
{
    if (contents)
	tt_free(contents);
}

#ifndef HAS_EXCEPTIONS
void TTFile::TT_Exception
	(
	char *     str
	)
{
#ifdef IOSTREAMSWORKS
    cerr << tt_status_message(tt_pointer_error(str)) << endl;
#else
    fprintf(stderr, "%s\n", tt_status_message(tt_pointer_error(str)));
#endif
}
#endif

TTFile & TTFile::operator=
	(
	const TTFile & file
	)
{
    if (file != *this) {
	delete [] contents;
	contents = new char [file.length() + 1];
	strcpy(contents,file.data());
	status = file.status;
    }
    return *this;
}

#if defined(linux) || defined(CSRG_BASED) || defined(sun)
std::ostream & operator<<
	(
	std::ostream & os,
	TTFile &  file
	)
#else
ostream & operator<<
	(
	ostream & os,
	TTFile &  file
	)
#endif
{
    if (file.ttFileOpFailed())
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
	return os << "Error in filename mapping; status = " 
		  << file.getStatus() << std::endl;
#else
	return os << "Error in filename mapping; status = " 
		  << file.getStatus() << endl;
#endif
    else
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
	return os << file.data() << std::endl;
#else
	return os << file.data() << endl;
#endif
}
