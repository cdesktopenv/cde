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
// $TOG: UAS_Common.C /main/10 1998/04/17 11:41:00 mgreess $
# include <sstream>
using namespace std;
# include "UAS_Common.hh"
# include "UAS_Factory.hh"
# include "UAS_Exceptions.hh"
# include "UAS_SearchEngine.hh"
# include "UAS_Collection.hh"
# include "UAS_StyleSheet.hh"
# include "UAS_EmbeddedObject.hh"

# define CLASS UAS_Common

UAS_Sender<UAS_DocumentRetrievedMsg> &
UAS_Common::senderUAS_DocumentRetrievedMsg() 
{ 
    static UAS_Sender<UAS_DocumentRetrievedMsg> f_senderUAS_DocumentRetrievedMsg; 
    return (f_senderUAS_DocumentRetrievedMsg); 
};

STATIC_SENDER_CC(UAS_ErrorMsg);
STATIC_SENDER_CC(UAS_StatusMsg);
STATIC_SENDER_CC(UAS_LibraryDestroyedMsg);

UAS_Common::UAS_Common () {
    fRetrievalStatus = UAS_NOT_RETRIEVED;
}

UAS_Common::~UAS_Common () {
}

UAS_List<UAS_String>
UAS_Common::rootLocators () {
    return UAS_Factory::getRootLocators ();
}

UAS_Pointer<UAS_Common>
UAS_Common::create (const UAS_String &locator) {
    return UAS_Factory::create (locator);
}

UAS_Pointer<UAS_Common>
UAS_Common::create_relative (const UAS_String &locator) {
    return UAS_Factory::create_relative (locator, this);
}

UAS_Pointer<UAS_EmbeddedObject>
UAS_Common::create_embedded_object (const UAS_String &locator) {
    return UAS_Factory::create_embedded (locator, this);
}

UAS_RetrievalStatus
UAS_Common::retrieve (void *client_data) {
    return fRetrievalStatus;
}

UAS_RetrievalStatus
UAS_Common::retrieval_status () const {
    return fRetrievalStatus;
}

void
UAS_Common::cancel_retrieval () {
    fRetrievalStatus = UAS_NOT_RETRIEVED;
}

void
UAS_Common::flush () {
    fRetrievalStatus = UAS_NOT_RETRIEVED;
}

UAS_String
UAS_Common::error_information () const {
    return UAS_String ();
}

UAS_String
UAS_Common::title (UAS_TitleType) {
    return UAS_String ();
}

//
//  SWM -- HP compiler craps out of we leave the 2nd arg in. Since
//  we're not using in (FOR NOW), hack city.
//
UAS_String
UAS_Common::book_name (UAS_TitleType tt /*, UAS_Pointer<UAS_Collection>*/) const {
    UAS_Pointer<UAS_Common> bp = (UAS_Common *) this;
    while (bp != 0 && bp->type() != UAS_BOOK)
	bp = bp->parent();
    if (bp == 0)
	bp = (UAS_Common *) this;
    return bp->title(tt);
}

UAS_String
UAS_Common::locator () {
    return UAS_String ();
}

UAS_String
UAS_Common::content_type () {
    return UAS_String ("unknown/unknown", -1, UAS_NOT_OWNER);
}

#if 0
UAS_Pointer<UAS_CollectionElement>
UAS_Common::collection_element (UAS_Pointer<UAS_Collection> ic) {
    return (UAS_CollectionElement *) 0;
}

UAS_Pointer<UAS_PathElement>
UAS_Common::path_element (UAS_Pointer<UAS_Path> ip) {
    return (UAS_PathElement *) 0;
}
#endif

int
UAS_Common::searchable () const {
    return 1;
}

UAS_Pointer<UAS_SearchEngine>
UAS_Common::search_engine () {
    return (UAS_SearchEngine *) 0;
}

unsigned int
UAS_Common::data_length () {
    return 0;
}

UAS_String
UAS_Common::data () {
    return UAS_String ();
}

int
UAS_Common::input (UAS_String) {
    return 0;
}

int
UAS_Common::operator == (UAS_Common &c) {
    return locator() == c.locator();
}

int
UAS_Common::operator != (UAS_Common &c) {
    return !(*this == c);
}

UAS_String
UAS_Common::implementation_type () {
    return UAS_String ("unknown", -1, UAS_NOT_OWNER);
}

UAS_ObjectType
UAS_Common::type () {
    return UAS_UNKNOWN_OBJECT_TYPE;
}

UAS_List<UAS_Common>
UAS_Common::book_tab_list () {
    return UAS_List<UAS_Common> ();
}

UAS_List<UAS_StyleSheet>
UAS_Common::style_sheet_list () {
    return UAS_List<UAS_StyleSheet> ();
}

UAS_Pointer<UAS_Common>
UAS_Common::parent () {
    return (UAS_Common *) 0;
}

UAS_List<UAS_Common>
UAS_Common::children () {
    return UAS_List<UAS_Common> ();
}

UAS_Pointer<UAS_Common>
UAS_Common::next () {
    return (UAS_Common *) 0;
}

UAS_Pointer<UAS_Common>
UAS_Common::previous () {
    return (UAS_Common *) 0;
}

void
UAS_Common::initialize (UAS_List<UAS_String> &libs) {
    UAS_Factory::initialize (libs);
}

void
UAS_Common::finalize () {
    UAS_Factory::finalize ();
}

void
UAS_Common::destroy (UAS_Pointer<UAS_Common> lib) {
    if (lib->type() != UAS_LIBRARY)
	throw(CASTEXCEPT Exception());
    UAS_Factory::destroyRoot (lib);
}

UAS_Pointer<UAS_Common>
UAS_Common::get_library () {
    UAS_Pointer<UAS_Common> theRoot(this);
    while (theRoot != 0 && theRoot->type() != UAS_LIBRARY)
	theRoot = theRoot->parent();
    return theRoot;
}

int
UAS_Common::subtree_size()
{
    UAS_List<UAS_Common> kids = children();
    unsigned int num_kids = kids.length(); 

    int tree_size = 1;

    for (unsigned int i = 0; i < num_kids; i++) {
	if (! (kids[i] == (const int)NULL))
	    tree_size += kids[i]->subtree_size();
    }

#ifdef SUBTREE_SIZE_DEBUG
    cerr << "subtree size = " << tree_size << endl;
#endif

    return tree_size;
}
