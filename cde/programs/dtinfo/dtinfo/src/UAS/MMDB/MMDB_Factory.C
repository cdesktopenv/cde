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

 */
// $TOG: MMDB_Factory.C /main/15 1998/04/17 11:42:43 mgreess $
# include <sstream>
using namespace std;
# include "UAS_Exceptions.hh"
# include "MMDB_Factory.hh"
# include "UAS_Exceptions.hh"
# include "MMDB_EmbeddedObject.hh"
# include "MMDB_Library.hh"
# include "MMDB_BookCase.hh"
# include "MMDB_Section.hh"
# include "UAS_Collection.hh"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

OLIAS_DB *MMDB_Factory::fOLIASDB = 0;

static MMDB_Factory  the_mmdb_factory; // instantiation

MMDB_Factory::MMDB_Factory ()
{
    UAS_Factory::register_factory("mmdb", this);
}

MMDB_Factory::~MMDB_Factory ()
{
    finalizeFactory ();
    UAS_Factory::unregister_factory("mmdb");
}

OLIAS_DB &
MMDB_Factory::olias_db () {
    if (fOLIASDB == 0) {
	fOLIASDB = new OLIAS_DB ();
    }
    return *fOLIASDB;
}

MMDB *
MMDB_Factory::createMMDB (const UAS_String &infoLibPath) {
    MMDB *rval = 0;
    //
    //  The MMDB leaks memory if we try to create an infolib
    //  with a bogus path. This routine is called often with
    //  infoLibPath == "", so we'll trap that case now
    //
    if (infoLibPath.length()) {
	if (!olias_db().validInfoLibPath(infoLibPath)) {
	    UAS_ErrorMsg badPath;
	    char buf[BUFSIZ];
	    (void) sprintf (buf, CATGETS(Set_UAS_MMDB, 2,
					"Invalid Infolib path: %s"),
					(char *) infoLibPath);
	    badPath.fErrorMsg = buf;
	    UAS_Common::send_message (badPath);
	    return rval;
	}
	mtry {
	    rval = new MMDB (infoLibPath);
	    fMMDBList.append (rval);
	} mcatch_any() {
#if 0
	    char buf[BUFSIZ];
	    (void) sprintf (buf, CATGETS(Set_UAS_MMDB, 3,
					"Ignoring invalid Infolib path: %s"),
					(char *) infoLibPath);
	    UAS_ErrorMsg theError;
	    theError.fErrorMsg = buf;
	    UAS_Common::send_message (theError);
#endif
	    rval = 0;
	} end_try;
    }
    return rval;
}

// check to make sure duplicate infolibs are not added.
// for a duplicate infolib, just return a pointer to
// the already-existing infolib.
// (Duplicate infolib means infolibs with the same lid (uid))
MMDB *
MMDB_Factory::getMMDB (const UAS_String &infoLibPath) {
    int i;
    UAS_String uid(fOLIASDB->getInfoLibUid((char*)infoLibPath));
    for (i = 0; i < fMMDBList.numItems(); i ++) {
	if (uid == fMMDBList[i]->infoLibUid())
	    return fMMDBList[i];
    }
    for (i  = 0; i < fDeadMMDBList.numItems(); i ++) {
	if (uid == fDeadMMDBList[i]->infoLibUid())
	    return fDeadMMDBList[i];
    }
    return createMMDB (infoLibPath);
}

void
MMDB_Factory::resolveLocator (const UAS_String &loc,
			      MMDB *&returnMMDB,
			      info_base *&returnInfoBase) {
    returnMMDB = 0;
    returnInfoBase = 0;
    for (int i = 0; i < fMMDBList.numItems(); i ++) {
	if (returnInfoBase = fMMDBList[i]->infolib()->
		getInfobaseByComponent(loc, info_lib::LOC)) {
	    returnMMDB = fMMDBList[i];
	    return;
	}
    }
}

MMDB *
MMDB_Factory::getMMDBFromObject (UAS_Pointer<UAS_Common> theDoc) {
    if (theDoc->implementation_type() != "mmdb")
	return 0;
    switch (theDoc->type()) {
	case UAS_LIBRARY: {
	    MMDB_Library *l = (MMDB_Library *) ((UAS_Common *) theDoc);
	    return &(l->mmdb());
	}
	case UAS_BOOKCASE: {
	    MMDB_BookCase *c = (MMDB_BookCase *) ((UAS_Common *) theDoc);
	    return &(c->mmdb());
	}
	case UAS_BOOK:
	case UAS_SECTION: {
	    MMDB_Section *s =  (MMDB_Section *) ((UAS_Common *) theDoc);
	    return &(s->mmdb());
	}
	default:
	    return 0;
    }
}

UAS_Pointer<UAS_Common>
MMDB_Factory::create_object (const UAS_String &locator) {
    MMDB_URL curURL;
    curURL.locator (locator);
    MMDB *theMMDB = getMMDB (curURL.fInfoLib);
    info_base *theInfoBase = 0;
    if (theMMDB == 0) {
	if (curURL.fLocator != "") {
	    resolveLocator (curURL.fLocator, theMMDB, theInfoBase);
	    if (theMMDB == 0) {
		throw (CASTEXCEPT Exception());
	    } else {
		UAS_Pointer<UAS_Common> rval =
		    new MMDB_Section (*theMMDB, curURL.fLocator);
		if (curURL.fType == "collection")
		    return new UAS_Collection (rval);
		return rval;
	    }
	}
    }

    if (theMMDB && curURL.fLocator != "") {
	UAS_Pointer<UAS_Common> rval =
		new MMDB_Section (*theMMDB, curURL.fLocator);
	if (curURL.fType == "collection")
	    return new UAS_Collection (rval);
	return rval;
    } else if (theMMDB && curURL.fInfoBase != "") {
	return new UAS_Collection(new MMDB_BookCase(*theMMDB,curURL.fInfoBase));
    } else if (theMMDB && curURL.fInfoLib != "") {
	return new UAS_Collection (new MMDB_Library(*theMMDB));
    } else {
	throw (CASTEXCEPT Exception());
    }
}

UAS_Pointer<UAS_Common>
MMDB_Factory::create_relative_object (const UAS_String &locator,
			       UAS_Pointer<UAS_Common> theDoc) {

    //
    //  First, try to get the object by using the current
    //  MMDB information library.
    //
    MMDB *theMMDB = getMMDBFromObject (theDoc);
    if (theMMDB) {
	info_base *ib = theMMDB->infolib()->
	    getInfobaseByComponent(locator, info_lib::LOC);
	if (ib) {
	    // need to pass precise id to the constructor
	    return new MMDB_Section (*theMMDB, locator);
	}
    }

    //
    //  Couldn't find it in theDoc's infolib. Try all the infolibs
    //  we know about.
    //
    for (int i = 0; i < fMMDBList.numItems(); i ++) {
	info_base *ib = fMMDBList[i]->infolib()->
		getInfobaseByComponent(locator, info_lib::LOC);
	if (ib) {
	    // need to pass precise id to the constructor
	    return new MMDB_Section(*(fMMDBList[i]), locator);
	}
    }

    return UAS_Pointer<UAS_Common>(NULL);
}

UAS_Pointer<UAS_EmbeddedObject>
MMDB_Factory::create_embedded_object (const UAS_String &locator) {
    for (int i = 0; i < fMMDBList.numItems(); i ++) {
	info_base *ib = fMMDBList[i]->infolib()->
		getInfobaseByComponent(locator, info_lib::GRA);
	if (ib) {
	    return new MMDB_EmbeddedObject (*fMMDBList[i], ib, locator);
	}
    }
    throw (CASTEXCEPT Exception());
}

UAS_Pointer<UAS_EmbeddedObject>
MMDB_Factory::create_relative_embedded_object (const UAS_String &locator,
		               UAS_Pointer<UAS_Common> theDoc) {
    //
    //  First try resolving to theDoc's infolib
    //
    MMDB *theMMDB = getMMDBFromObject (theDoc);
    if (theMMDB) {
	info_base *ib = theMMDB->infolib()->
		getInfobaseByComponent(locator, info_lib::GRA);
	if (ib) {
	    return new MMDB_EmbeddedObject (*theMMDB, ib, locator);
	}
    }

    //
    //  Cross-infolib link. Iterate through the list of infolibs.
    //
    for (int i = 0; i < fMMDBList.numItems(); i ++) {
	info_base *ib = fMMDBList[i]->infolib()->
		getInfobaseByComponent(locator, info_lib::GRA);
	if (ib) {
	    return new MMDB_EmbeddedObject (*(fMMDBList[i]), ib, locator);
	}
    }
    throw (CASTEXCEPT Exception());
}

void
MMDB_Factory::initializeFactory (UAS_List<UAS_String>& libs) {
    //
    //  Take DTINFO_INFOLIB_PATH and split out the colon-separated
    //  paths. Create an MMDB object for each path and, if creation
    //  was successful, add it to our list of infolibs. Send messages
    //  for each invalid 
    //
    //char *libenv = getenv ("DTINFO_INFOLIB_PATH");
    //if (!libenv) {
	//return;
    //}
    //UAS_String libstr (libenv);
    //UAS_List<UAS_String> fields = libstr.splitFields (':');
    //for (int i = 0; i < fields.length(); i ++) {
	//(void) getMMDB (*(UAS_String*)fields[i]);
    //}
    for (int i = 0; i < libs.length(); i ++) {
	//  If this call doesn't create a new MMDB, it sends
	//  an error message to the application.
	(void) getMMDB (*(UAS_String*)libs[i]);
    }
}

void
MMDB_Factory::finalizeFactory () {
    while (fMMDBList.numItems() > 0) {
	MMDB *dead = fMMDBList[0];
	fMMDBList.remove(dead);
	delete dead;
    }
    while (fDeadMMDBList.numItems() > 0) {
	MMDB *deaddead = fDeadMMDBList[0];
	fDeadMMDBList.remove(deaddead);
	delete deaddead;
    }
    delete fOLIASDB;
    fOLIASDB = 0;
}

UAS_List<UAS_String>
MMDB_Factory::rootLocators () {
    //
    // CHANGE THIS TO BE UAS_ObjList...
    //
    UAS_List<UAS_String> rval;
    for (int i = 0; i < fMMDBList.numItems(); i ++) {
	MMDB_URL cur;
	cur.fType = "collection";
	cur.fInfoLib = fMMDBList[i]->infoLibPath();
	rval.insert_item (new UAS_String (cur.locator()));
    }
    return rval;
}

void
MMDB_Factory::destroy_root_object (UAS_Pointer<UAS_Common> theRoot) {
    if (theRoot->implementation_type() != "mmdb" ||
	theRoot->type() != UAS_LIBRARY)
	throw(CASTEXCEPT Exception());
    MMDB_Library *theLib = (MMDB_Library *) (UAS_Common *) theRoot;
    MMDB *deadMMDB = &(theLib->mmdb());
    int i;
    for (i = 0; i < fMMDBList.numItems(); i ++)
	if (fMMDBList[i] == deadMMDB)
	    break;
    if (i >= fMMDBList.numItems())
	throw(CASTEXCEPT Exception());

    // remove deadMMDB before sending msg because the user
    // can possibly inquire mmdb list on the thread.
    fMMDBList.remove (deadMMDB);

    //  Give the user a chance to blow away all objects associated
    //  with this library.
    //
    UAS_LibraryDestroyedMsg theMsg (theRoot);
    UAS_Common::send_message (theMsg);

    //  Do the final damage.
    delete deadMMDB;
}

UAS_String
MMDB_Factory::genInfolibName () {
    static int counter = 0;
    char buf[BUFSIZ];
    (void) sprintf (buf, CATGETS(Set_UAS_MMDB, 4, "DtMmdb Library %d"), ++counter);
    return UAS_String (buf);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  MMDB_URL
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MMDB_URL::MMDB_URL () {
}

UAS_String
MMDB_URL::locator () {
    UAS_String rval = "mmdb:";
    int hasStuff = 0;
    if (fInfoLib != "") {
	rval = rval + "INFOLIB=" + fInfoLib;
	hasStuff ++;
    }
    if (fInfoBase != "") {
	if (hasStuff)
	    rval = rval + "&";
	rval = rval + "BOOKCASE=" + fInfoBase;
	hasStuff ++;
    }
    if (fLocator != "") {
	if (hasStuff)
	    rval = rval + "&";
	rval = rval + "LOCATOR=" + fLocator;
	hasStuff ++;
    }
    if (fType == "") {
	fType = "collection"; // default value
    }
    if (hasStuff)
	rval = rval + "&";
    rval = rval + "TYPE=" + fType;
    hasStuff ++;

    return rval;
}

void
MMDB_URL::installPair (const UAS_String &key, const UAS_String &value) {
    if (key == "INFOLIB") {
	fInfoLib = value;
    } else if (key == "BOOKCASE") {
	fInfoBase = value;
    } else if (key == "TYPE") {
	fType = value;
    } else if (key == "LOCATOR") {
	fLocator = value;
    }
}

void
MMDB_URL::locator (const UAS_String &in) {
    UAS_String access, rest;
    in.split (':', access, rest);
    UAS_List<UAS_String> pairs = rest.splitFields ('&');
    for (int i = 0; i < pairs.length(); i ++) {
	UAS_String key, value;
	pairs[i]->split ('=', key, value);
	installPair (key, value);
    }
}

