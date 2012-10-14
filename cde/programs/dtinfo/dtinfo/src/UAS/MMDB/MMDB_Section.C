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
// $TOG: MMDB_Section.C /main/11 1998/04/17 11:43:00 mgreess $
# include "UAS_Exceptions.hh"
# include "MMDB_Section.hh"
# include "MMDB_BookCase.hh"
#if 0
# include "MMDB_SectionCollection.hh"
# include "MMDB_Path.hh"
#endif
# include "MMDB_StyleSheet.hh"
# include "MMDB_Factory.hh"
# include "UAS_Collection.hh"
# include "UAS_SearchEngine.hh"

char* g_mmdb_section_label;

MMDB_Section::MMDB_Section (MMDB &mmdb, const UAS_String &locator):
    MMDB_Common (mmdb, mmdb.infobase (locator)),
    fNode (infobase (), locator_smart_ptr (infobase(), locator).node_id()),
    f_precise_id(locator)
{
    fRetrievalStatus = UAS_RETRIEVED;
   
    f_label = locator_smart_ptr(infobase(), locator).label();

    // hack!! Temporary communication channel with CanvasRenderer
    g_mmdb_section_label = (char*)f_label;
}

MMDB_Section::MMDB_Section (MMDB &mmdb, info_base *ib, const oid_t &node_id,
			    UAS_String tt):
    MMDB_Common (mmdb, ib),
    fNode (ib, node_id),
    f_precise_id (fNode.locator())
{
    fRetrievalStatus = UAS_RETRIEVED;
    if (tt == "") {
	fTabTitle = title (UAS_SHORT_TITLE);
    } else {
	fTabTitle = tt;
    }

    f_label = title();
}

MMDB_Section::~MMDB_Section () {
}

UAS_String
MMDB_Section::title (UAS_TitleType tt) {
    if (isbook()) {
	if (tt == UAS_LONG_TITLE) {
	    return UAS_String (doc_smart_ptr(infobase(),fNode.its_oid()).long_title());
	} else {
	    return UAS_String (doc_smart_ptr(infobase(),fNode.its_oid()).short_title());
	}
    } else {
	if (tt == UAS_LONG_TITLE) {
	    return UAS_String (fNode.long_title());
	} else {
	    return UAS_String (fNode.short_title());
	}
    }
}

UAS_String
MMDB_Section::locator () {
    MMDB_URL rval;
    rval.fType = "section";
    rval.fLocator = fNode.locator();
    rval.fInfoBase = infobase()->get_base_name();
    rval.fInfoLib = mmdb().infoLibPath();
    return rval.locator();
}

UAS_String
MMDB_Section::id () {
    return fNode.locator();
}

UAS_String
MMDB_Section::lid () {
    info_lib &il = *(mmdb().infolib());
    return UAS_String(il.get_info_lib_uid());
}
 
UAS_String
MMDB_Section::bid () {
    return UAS_String(infobase()->get_base_uid());
}

UAS_String
MMDB_Section::locale()
{
    return UAS_String(infobase()->get_info_base_locale());
}

UAS_String
MMDB_Section::content_type () {
    return UAS_String ("text/sgml", -1, UAS_NOT_OWNER);
}

UAS_String
MMDB_Section::data () {
    //
    //  Don't copy the data here (UAS_NOT_OWNER accomplishes this.)
    //  The danger is if someone blows away this object, but continues
    //  to use the returned UAS_String.
    //
    unsigned int ds = fNode.data_size();
    return UAS_String (fNode.data(), ds, UAS_NOT_OWNER);
}

unsigned int
MMDB_Section::data_length () {
    return fNode.data_size ();
}


UAS_List<UAS_Common>
MMDB_Section::book_tab_list () {
    UAS_List<UAS_Common> theList;
    if (isbook()) {
	doc_smart_ptr book (infobase(), fNode.its_oid());
	short_list_handler *l = book.tab_list ();
	for (int i = 1; i <= (*l)->count(); i ++) {
	    pstring_handler *p = (pstring_handler *) ((*l)->get_component(i));
	    UAS_String title, oidStr;
	    UAS_String desc ((*p)->get());
	    desc.split ('\t', title, oidStr);
	    oid_t TabOID = oid_t ((char *) oidStr, true, false);
	    theList.insert_item (new MMDB_Section (mmdb(), infobase(),
				     TabOID, title));
	    delete p ;
	}
	delete l ;
    }
    return theList;
}

UAS_String
MMDB_Section::tab_title () {
    return fTabTitle;
}

UAS_List<UAS_StyleSheet>
MMDB_Section::style_sheet_list () {
    UAS_List<UAS_StyleSheet> theList;
    theList.insert_item (UAS_Pointer<UAS_StyleSheet>
			 (new MMDB_StyleSheet(mmdb(),infobase(),
					      fNode.stylesheet_id(), 
					      SS_ONLINE)));
    theList.insert_item (new MMDB_StyleSheet(mmdb(),infobase(),
		    fNode.stylesheet_id(), SS_HARDCOPY));
    return theList;
}

UAS_RetrievalStatus
MMDB_Section::retrieve(void *client_data) {
    UAS_DocumentRetrievedMsg theMsg ((UAS_Common *) this);
    UAS_Common::send_message (theMsg, client_data);
    return UAS_Common::retrieve ();
}

UAS_Pointer<UAS_Common>
MMDB_Section::next () {
    UAS_Pointer<UAS_Common> rval;
    dlp_smart_ptr theDLP (infobase(), fNode.its_oid());
    oid_t id = theDLP.next_dlp_oid();
    if (id.icode () != 0) {
	dlp_smart_ptr nextDLP (theDLP, id);
	rval = UAS_Pointer<UAS_Common> (
		new MMDB_Section (mmdb(), infobase(), nextDLP.node_id(), ""));
    }
    return rval;
}

UAS_Pointer<UAS_Common>
MMDB_Section::previous () {
    dlp_smart_ptr theDLP (infobase(), fNode.its_oid());
    oid_t id = theDLP.prev_dlp_oid();
    if (id.icode () == 0) {
	return (UAS_Common *) 0;
    } else {
	dlp_smart_ptr prevDLP (theDLP, id);
	return new MMDB_Section (mmdb(), infobase(), prevDLP.node_id(), "");
    }
}

UAS_Pointer<UAS_Common>
MMDB_Section::parent() {
    if (isbook()) {
	return new MMDB_BookCase (mmdb(), infobase());
    } else {
	toc_smart_ptr theTOC (infobase(), fNode.its_oid());
	return new MMDB_Section (mmdb(), infobase(), theTOC.parent(), "");
    }
}

UAS_List<UAS_Common>
MMDB_Section::children () {
    UAS_List<UAS_Common> theList;
    toc_smart_ptr theTOC (infobase(), fNode.its_oid());
    oid_list_handler *l = theTOC.children ();
    int cnt = (*l)->count();
    for (int i = 1; i <= cnt; i ++) {
	oid_t curID = (*l)->operator()(i);
	theList.insert_item (new MMDB_Section (mmdb(), infobase(), curID, ""));
    }
    delete l ;
    return theList;
}

int
MMDB_Section::isbook () {
    toc_smart_ptr theTOC (infobase(), fNode.its_oid());
    return theTOC.parent().icode() == 0;
}

UAS_ObjectType
MMDB_Section::type () {
    if (isbook())
	return UAS_BOOK;
    return UAS_SECTION;
}

UAS_Pointer<UAS_SearchEngine>
MMDB_Section::search_engine () {
    UAS_Pointer<UAS_Common> theRoot(parent());
    while (theRoot != 0 && theRoot->type() != UAS_LIBRARY)
	theRoot = theRoot->parent();
    if (theRoot == 0)
	throw (CASTEXCEPT Exception());
    return theRoot->search_engine();
}
