// $XConsortium: UtilityFunctions.cc /main/4 1996/06/11 16:35:41 cde-hal $
#include <stream.h>
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
