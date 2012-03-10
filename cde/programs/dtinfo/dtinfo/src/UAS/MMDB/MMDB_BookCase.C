// $XConsortium: MMDB_BookCase.C /main/7 1996/09/04 01:35:39 cde-hal $
# include "MMDB_BookCase.hh"
# include "MMDB_Library.hh"
# include "MMDB_Section.hh"
# include "MMDB_Factory.hh"

MMDB_BookCase::MMDB_BookCase (MMDB &theMMDB, info_base *ibase):
	       MMDB_Common (theMMDB, ibase) {
}

MMDB_BookCase::MMDB_BookCase (MMDB &theMMDB, const UAS_String &name):
		    MMDB_Common (theMMDB,
		    theMMDB.infolib()->get_info_base(name)) {
}

UAS_Pointer<UAS_Common>
MMDB_BookCase::parent () {
    return new MMDB_Library (mmdb());
}

UAS_List<UAS_Common>
MMDB_BookCase::children () {
    UAS_List<UAS_Common> theList;
    int idx;
    int total = infobase()->num_of_docs();
    for (idx = 1; idx <= total; idx ++) {
        doc_smart_ptr d(infobase(), idx);
        theList.insert_item(new MMDB_Section(mmdb(),infobase(),d.locator_id(),""));
    }
    return theList;
}

UAS_String
MMDB_BookCase::title (UAS_TitleType) {
    return UAS_String (infobase()->get_base_desc());
}

UAS_String
MMDB_BookCase::locator () {
    MMDB_URL rval;
    rval.fType = "collection";
    rval.fInfoLib = mmdb().infoLibPath();
    rval.fInfoBase = infobase()->get_base_name();
    return rval.locator();
}

UAS_String
MMDB_BookCase::id () {
    return infobase()->get_base_name();
}

UAS_String
MMDB_BookCase::lid () {
    info_lib &il = *(mmdb().infolib());
    return UAS_String(il.get_info_lib_uid());
}

UAS_String
MMDB_BookCase::bid () {
    return UAS_String(infobase()->get_base_uid());
}

UAS_String
MMDB_BookCase::locale()
{
    return UAS_String(infobase()->get_info_base_locale());
}

UAS_ObjectType
MMDB_BookCase::type () {
    return UAS_BOOKCASE;
}

UAS_Pointer<UAS_SearchEngine>
MMDB_BookCase::search_engine() {
    //  Assumption here is that parent() yields a library
    return parent()->search_engine();
}
