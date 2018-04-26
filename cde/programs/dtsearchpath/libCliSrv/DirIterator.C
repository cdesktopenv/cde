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
/* $XConsortium: DirIterator.C /main/2 1995/07/17 14:09:48 drk $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "DirIterator.h"
#include <errno.h>

DirectoryIterator::DirectoryIterator
	(
	const CString & dir
	) : state(good_)
{
    theDir = opendir(dir.data());
    if (theDir == 0)
	state = bad_;
}

DirectoryIterator::~DirectoryIterator()
{
    closedir(theDir);
}

struct dirent * DirectoryIterator::operator()()
{
    struct dirent * direntry = readdir(theDir);

    if (direntry == 0)
	state = (errno ? bad_ : done_);

    return direntry;
}

