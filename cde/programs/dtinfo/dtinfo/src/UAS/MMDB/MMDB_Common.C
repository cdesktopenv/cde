// $XConsortium: MMDB_Common.cc /main/3 1996/06/11 16:43:28 cde-hal $
# if 0
# include "UAS_Base.hh"
# include "MMDB_Common.hh"
# include "UAS_Pointer.hh"
# include "UAS_List.hh"
# include "UAS_BookTab.hh"
# include "MMDB_BookTab.hh"

UAS_Pointer<UAS_List<UAS_BookTab> > 
MMDB_Common:: tabList (MMDB &theMMDB, info_base *ib, const oid_t &id) {
    doc_smart_ptr book (ib, id);
    short_list_handler *l = book.tab_list ();
    UAS_List<UAS_BookTab> *theList = new UAS_List<UAS_BookTab>();
    for (int i = 1; i <= (*l)->count(); i ++) {
	pstring_handler *p = (pstring_handler *)((*l)->get_component(i));
	theList->insert_item(new MMDB_BookTab(theMMDB, ib, (*p)->get()));
    }
    return theList;
}
#endif
