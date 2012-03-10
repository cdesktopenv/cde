// $XConsortium: UAS_Templates.cc /main/3 1996/06/11 16:44:45 cde-hal $
# define EXPAND_TEMPLATES

# include <stream.h>
# include "Exceptions.hh"
# include "UAS_Exceptions.hh"
# include "UAS_Common.hh"
# include "UAS_EmbeddedObject.hh"
# include "UAS_Collection.hh"
# include "UAS_Path.hh"
# include "UAS_StyleSheet.hh"
# include "UAS_SearchEngine.hh"
# include "UAS_Factory.hh"
# include "MMDB.hh"

# ifdef DTSEARCH
# include "DtSR_BookcaseEntry.hh"
# include "DtSR_SearchResultsEntry.hh"
# include "DtSR_SearchResults.hh"
# include "DtSR_SearchEngine.hh"
# include "DtSR_Stems.hh"
# include "Util_Classes/DictLink.hh"
# include "Util_Classes/DictIter.hh"
# include "Util_Classes/Dict.hh"
# else
# include "Fulcrum_BookcaseEntry.hh"
# endif

typedef UAS_Pointer<UAS_String>                 _ptrString_;
typedef UAS_Pointer<UAS_Common>                 _ptrCommon_;
typedef UAS_Pointer<UAS_EmbeddedObject>         _ptrEmbeddedObject_;
typedef UAS_Pointer<UAS_Collection>             _ptrCollection_;
typedef UAS_Pointer<UAS_Path>                   _ptrPath_;
typedef UAS_Pointer<UAS_StyleSheet>             _ptrStyleSheet_;

typedef UAS_List<UAS_StyleSheet>                _listStyleSheet_;
typedef UAS_List<UAS_Common>                    _listCommon_;
typedef UAS_List<UAS_SearchResults>             _listSearchResults_;
typedef UAS_List<UAS_SearchScope>               _listSearchScope_;

typedef UAS_PtrList<FactoryEntry>               _ptrListFactoryEntry_;
typedef UAS_PtrList<MMDB>			_ptrListMMDB_;

typedef UAS_Sender<UAS_StatusMsg>               _sndStatusMsg_;
typedef UAS_Sender<UAS_PartialDataMsg>          _sndPartialDataMsg_;
typedef UAS_Sender<UAS_SearchMsg>               _sndSearchMsg_;

typedef UAS_ObjList<int>                        _objListInt_;
typedef UAS_PtrList<const char>                 _ptrListConstChar_;

#ifdef DTSEARCH
typedef UAS_Pointer<DtSR_BookcaseSearchEntry>	_ptrDtSR_BookcaseSearchEntry_;
typedef UAS_Pointer<DtSR_SearchResults>	_ptrDtSR_SearchResults_;
typedef UAS_Pointer<DtSR_SearchResultsEntry>	_ptrDtSR_SearchResultsEntry_;
typedef UAS_Pointer<DtSR_SearchEngine>	_ptrDtSR_SearchEngine_;
typedef Dict<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
				_dictStringptrDtSR_SearchResultsEntry_;
typedef DictIter<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
				_dictiterStringptrDtSR_SearchResultsEntry_;
typedef UAS_List<DtSR_BookcaseSearchEntry>	_listDtSR_BookcaseSearchEntry_;
typedef UAS_List<DtSR_Stems>	_listDtSR_Stems_;
#else
typedef UAS_PtrList<Fulcrum_BookcaseEntry>      _ptrListFulcrumBookcaseEntry_;
#endif

typedef UAS_List<UAS_String>			_listString_;
