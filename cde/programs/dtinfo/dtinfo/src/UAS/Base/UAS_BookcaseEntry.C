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
// $TOG: UAS_BookcaseEntry.C /main/5 1998/04/17 11:40:44 mgreess $
#include "UAS_BookcaseEntry.hh"
#include "UAS_Exceptions.hh"
#include "UAS_Collection.hh"

#include <string.h>

static const char *
UASbasename(const char *path)
{
    if (!path)
        return NULL;

    const char *p;

    for (p = path + strlen(path) - 1; *p != '/' && p != path; p--);

    if (*p == '/')
        p++;

    return p;
}


// /////////////////////////////////////////////////////////////////
// UAS_BookcaseEntry - ctor, dtor
// /////////////////////////////////////////////////////////////////

UAS_BookcaseEntry::UAS_BookcaseEntry(UAS_Pointer<UAS_Common> &bookcase,
                                     int searchable) 
: f_searchable(searchable),
  f_name(0),
  f_base_num(-1)
{
    int len;

    f_bookcase_name = bookcase->id();
    const char *bc_path = f_bookcase_name;
    if (! bc_path) {
	f_name = f_path = NULL;
    }
    else {

	const char *bc_name = UASbasename(bc_path);

	len = strlen(bc_name);
	f_name = new char[len + 1];
	*((char *) memcpy(f_name, bc_name, len) + len) = '\0';

	len = strlen(bc_path);
	f_path = new char[len + 1];
	*((char *) memcpy(f_path, bc_path, len) + len) = '\0';
    }

   f_infolib_id  = bookcase->lid();
   f_bookcase_id = bookcase->bid();
}

UAS_BookcaseEntry::~UAS_BookcaseEntry()
{
    if (f_name) delete f_name;
    if (f_path) delete f_path;
}


void
UAS_BookcaseEntry::set_book_list (UAS_ObjList<int> &list)
{ 
    f_book_list = list;		// replace it 
}
