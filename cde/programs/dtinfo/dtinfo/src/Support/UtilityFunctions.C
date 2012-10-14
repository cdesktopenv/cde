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
// $XConsortium: UtilityFunctions.cc /main/4 1996/06/11 16:35:41 cde-hal $
#include <sstream>
using namespace std;
# include "UtilityFunctions.hh"

UAS_List<UAS_Common>
bookcaseList () {
    UAS_List<UAS_Common> rval;
    UAS_List<UAS_String> rootLocs = UAS_Common::rootLocators ();
    int i;
    for (i = 0; i < rootLocs.length(); i ++) {
	UAS_Pointer<UAS_Common> curDoc (UAS_Common::create
						(*(UAS_String*)rootLocs[i]));
	//
	//  FOR NOW -- assume collection...
	//
	UAS_Pointer<UAS_Collection> curCol = (UAS_Collection *) ((UAS_Common *)
							curDoc);
	curDoc = curCol->root();

	if (curDoc->type() == UAS_LIBRARY) {
	    UAS_List<UAS_Common> kids = curDoc->children();
	    for (int j = 0; j < kids.length(); j ++) {
		if (kids[j]->type() == UAS_BOOKCASE) {
		    rval.insert_item(kids[j]);
		}
	    }
	} else if (curDoc->type() == UAS_BOOKCASE) {
	    rval.insert_item (curDoc);
	}
    }
    return rval;
}
