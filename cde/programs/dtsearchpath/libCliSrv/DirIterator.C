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

