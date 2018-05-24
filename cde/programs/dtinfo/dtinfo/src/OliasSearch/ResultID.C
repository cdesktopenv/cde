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
// $TOG: ResultID.C /main/7 1997/08/12 12:59:24 samborn $
#define C_ResultID
#define L_OliasSearch
#include <Prelude.h>

ResultID::ResultID (UAS_Pointer<UAS_SearchResults> res): fResults(res) {
}

ResultID::~ResultID () {
}

int
ResultID::ndocs () {
    return fResults->num_docs();
}

UAS_String
ResultID::sq () {
    UAS_String& temp_string =  *(UAS_String*)fResults->query();
    return temp_string;
}

const char *
ResultID::display_as () {
    return (char *)*(UAS_String *) fResults->query();
}

UAS_Pointer<UAS_SearchResults>
ResultID::results() {
    return fResults;
}
