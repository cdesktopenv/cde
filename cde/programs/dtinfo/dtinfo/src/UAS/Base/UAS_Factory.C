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
// $XConsortium: UAS_Factory.C /main/9 1996/10/04 12:33:17 cde-hal $
# include <sstream>
using namespace std;
# include <string.h>
# include "UAS_Pointer.hh"
# include "UAS_String.hh"
# include "UAS_Factory.hh"
# include "UAS_Common.hh"
# include "UAS_Exceptions.hh"
# include "UAS_Collection.hh"
# include "UAS_EmbeddedObject.hh"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

static UAS_PtrList<FactoryEntry> *gFactoryList;

void
UAS_Factory::initialize (UAS_List<UAS_String>& libs) {
    if (gFactoryList == 0)
	return;
    UAS_PtrList<FactoryEntry>&theList = *gFactoryList;
    for (int i = 0; i < theList.numItems(); i ++)
	theList[i]->factory()->initializeFactory(libs);
}

void
UAS_Factory::finalize () {
    if (gFactoryList == 0)
	return;
    UAS_PtrList<FactoryEntry>&theList = *gFactoryList;
    for (int i = 0; i < theList.numItems(); i ++)
	theList[i]->factory()->finalizeFactory();
}

UAS_Factory *
UAS_Factory::lookup (const UAS_String& accessType) {
    if (gFactoryList == 0)
	return (UAS_Factory *) 0;
    UAS_PtrList<FactoryEntry>&theList = *gFactoryList;
    for (int i = 0; i < theList.numItems(); i ++) {
	if (!strcmp (theList[i]->access(), accessType))
	    return theList[i]->factory();
    }
    return (UAS_Factory *) 0;
}

int
UAS_Factory::register_factory (const char *acc, UAS_Factory *newFactory)
{
    if (acc == NULL || newFactory == NULL)
	return UAS_REGISTER_FAIL;

    if (lookup(UAS_String(acc))) // one already exists for the type
	return UAS_REGISTER_RETURN;

    if (gFactoryList == 0)
	gFactoryList = new UAS_PtrList<FactoryEntry>();
    FactoryEntry *newEnt = new FactoryEntry (acc, newFactory);
    gFactoryList->append (newEnt);
#ifdef REGISTER_DEBUG
    fprintf(stderr, "(DEBUG) factory for \"%s\" has been registered.\n", acc);
#endif
    return 0;
}

void
UAS_Factory::unregister_factory (const char *acc)
{
    if (acc == NULL || gFactoryList == NULL)
	return;

    UAS_PtrList<FactoryEntry>&theList = *gFactoryList;
    for (int i = 0; i < theList.numItems(); i++) {
	if (! strcmp (theList[i]->access(), acc)) {
	    gFactoryList->remove(theList[i]);
#ifdef REGISTER_DEBUG
	    fprintf(stderr,
		"(DEBUG) factory for \"%s\" has been un-registered.\n", acc);
#endif
	    break;
	}
    }
}

UAS_Pointer<UAS_Common>
UAS_Factory::create (const UAS_String &locator) {
    UAS_String access, rest;
    locator.split (':', access, rest);
    if (access == "") {
	throw (UAS_Exception((char*)UAS_String(
		CATGETS(Set_UAS_Base, 1,
			"No access part provided in locator (URL)."))));
    }
    UAS_Factory *theFactory = UAS_Factory::lookup(access);
    if (theFactory == 0) {
	char buf[BUFSIZ];
	(void) sprintf (buf, CATGETS(Set_UAS_Base, 2,
				     "Don't know how to create %s: objects"),
			(char*)access);

	throw (UAS_Exception(buf));
    }
    return theFactory->create_object (locator);
}

UAS_Pointer<UAS_Common>
UAS_Factory::create_relative (const UAS_String &locator,
			      UAS_Pointer<UAS_Common> relative_to) {
    UAS_Factory *theFactory = UAS_Factory::get_factory (locator);
    if (theFactory != 0) {
	return theFactory->create_object (locator);
    } else {
	theFactory = UAS_Factory::get_factory (relative_to->locator());
	return theFactory->create_relative_object(locator, relative_to);
    }
}

UAS_Pointer<UAS_EmbeddedObject>
UAS_Factory::create_embedded (const UAS_String &locator,
			      UAS_Pointer<UAS_Common> relative_to) {
    UAS_Factory *theFactory = get_factory (locator);
    if (theFactory != 0) {
	return theFactory->create_embedded_object (locator);
    } else {
	theFactory = get_factory (relative_to->locator());
	return theFactory->create_relative_embedded_object(locator,relative_to);
    }
}

UAS_Factory *
UAS_Factory::get_factory (const UAS_String& locator) {
    UAS_String access, rest;
    locator.split (':', access, rest);
    return UAS_Factory::lookup (access);
}

UAS_List<UAS_String>
UAS_Factory::getRootLocators () {
    UAS_List<UAS_String> rval;
    if (gFactoryList == 0)
	return rval;
    UAS_PtrList<FactoryEntry>&theList = *gFactoryList;
    for (int i = 0; i < theList.numItems(); i ++) {
	UAS_List<UAS_String> cur = theList[i]->factory()->rootLocators();
	for (int j = 0; j < cur.length(); j ++) {
	    rval.insert_item (cur[j]);
	}
    }
    return rval;
}

void
UAS_Factory::destroyRoot (UAS_Pointer<UAS_Common> theRoot) {
    UAS_Factory *theFactory = get_factory (theRoot->locator());
    if (theFactory)
	theFactory->destroy_root_object (theRoot);
}
