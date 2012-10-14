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
// $XConsortium: dict_driver.cc /main/3 1996/06/11 16:42:53 cde-hal $
#include <string.h>

#include <iostream>
using namespace std;

#include "Exceptions.hh"
#include "UAS_Exceptions.hh"
#include "UAS_String.hh"

#include "Dict.hh"
#include "DictIter.hh"

main()
{
    INIT_EXCEPTIONS();

    Dict<UAS_String, int> map(NULL, 0);

    for (;;) {
	char string[128];
	*string = '\0';
	cin.getline(string, 128);
	while (! strlen(string)) {
	    cin.getline(string, 128);
	}
	if (strcasecmp(string, "end") == 0)
	    break;
	UAS_Pointer<UAS_String> uas_string
		= new UAS_String(string, strlen(string), UAS_OWNER);
	map[*uas_string]++;
	*string = '\0';
    }

    printf("map size = %d\n", map.size());

    DictIter<UAS_String, int> mapiter;
    for (mapiter = map.first(); mapiter() ;mapiter++) {
	printf("%s, %d\n", (const char*)mapiter.key(), mapiter.value());
    }
}
