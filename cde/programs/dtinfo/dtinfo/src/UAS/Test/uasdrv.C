// $XConsortium: uasdrv.cc /main/4 1996/06/11 16:45:11 cde-hal $
# include <stream.h>
# include <stdio.h>


# include "Exceptions.hh"
# include "UAS.hh"

#if 0
void
printTitles (UAS_Pointer<UAS_Common> doc, int level) {
    int i;
    for (i = 0; i < level; i ++)
	fprintf (stderr, "    ");
    fprintf (stderr, "%s\n", (char *) doc->title ());
    UAS_Pointer<UAS_Common> curDoc;
    if (doc->type() == UAS_BOOK) {
	curDoc = doc->next();
	while (curDoc != 0) {
	    fprintf (stderr, "********** %s \n", (char *) curDoc->title ());
	    curDoc = curDoc->next();
	}
    } else {
	UAS_List<UAS_Common> theList = doc->children ();
	for (i = 0; i < theList.length(); i ++)
	    printTitles (theList[i], level + 1);
    }
}

#endif

void
printLocs (UAS_Pointer<UAS_Common> doc, int level) {
    for (int i = 0; i < level; i ++)
	cerr << "    ";
    UAS_String theLoc = doc->locator();
    if (level >= 2) {
	char buf[1024];
	UAS_String loc = doc->id();
	(void) sprintf (buf, "mmdb:LOCATOR=%s", (char *) loc);
	UAS_Pointer<UAS_Common> theDoc = UAS_Common::create(buf);
	cerr << (char *) theLoc << ", " << (char *) theDoc->title() << endl;
    } else {
	cerr << (char *) theLoc << endl;
    }
    UAS_List<UAS_Common> kids = doc->children();
    for (i = 0; i < kids.length(); i ++) {
	printLocs (kids[i], level + 1);
    }
}

int main (int, char *[]) {
    INIT_EXCEPTIONS();
    UAS_Common::initialize ();
    UAS_List<UAS_String> locList = UAS_Common::rootLocators ();
    for (int i = 0; i < locList.length(); i ++) {
	UAS_String &cur = locList[i];
	UAS_Pointer<UAS_Common> curDoc = UAS_Common::create(cur);
	UAS_Pointer<UAS_Collection> curCol = (UAS_Collection *)
	    ((UAS_Common *) curDoc);
	printLocs (curCol->root(), 0);
	curDoc = curCol->root();
	curCol = 0;
	UAS_Common::destroy (curDoc);
    }
    UAS_Common::finalize ();
    return 0;
}
