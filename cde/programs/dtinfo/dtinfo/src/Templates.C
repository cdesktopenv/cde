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
/* $XConsortium: Templates.C /main/24 1996/11/12 10:10:11 cde-hal $ */

#define C_xList
#define C_InputNotifier
#define L_Support

#define C_SearchScope
#define L_Search

#define C_QueryTerm
#define L_Query

#define C_Mark
#define C_MarkBase
#define L_Marks

#define C_GhostScriptEventMgr
#define C_InputMgrX
#define C_MessageMgr
#define L_Managers

#define C_PixmapGraphic
#define C_GraphicsMgr
#define L_Graphics

#define C_NodeViewInfo
#define L_Basic

class Ancestor;
class LinkAction;
class BookTab;
class MarkTml;
class MarkIcon;
class MarkCreated;
class MarkSelectionChanged;
#ifndef _IBMR2
class DisplayDocument;
#endif
class MarkBase;
class MarkCanvas;
class NodeWindowAgent;
class GraphicAgent;
class LibraryAgent;
class Mark_mmdb;
class HistoryAdd;
class HistoryDelete;
class SelectionChanged;
class PixmapGraphic;
class BookmarkEdit;
class InfobaseEntry;
class Header;
class PixmapPidObj;
class PsProcess;
#ifdef UseQSearch
class NTerminal;
#endif
class PartialElementFeatures;
class TableDefn;
class TGDefn;
class ColDefn;
class ColFormat;
class RowDefn;
class UpdateMenu;

#ifndef EXPAND_TEMPLATES
#define EXPAND_TEMPLATES
#endif
#ifndef TEMPL_DEBUG
#ifdef DEBUG
#undef DEBUG
#endif
#endif

#include "Exceptions.hh"
#include "UAS_Exceptions.hh"
#include "UAS_Common.hh"
#include "UAS_EmbeddedObject.hh"
#include "UAS_Collection.hh"
#include "UAS_Path.hh"
#include "UAS_OQLParser.hh"
#include "UAS_StyleSheet.hh"
#include "UAS_SearchEngine.hh"
#include "UAS_Factory.hh"
#include "UAS/MMDB/MMDB.hh"
#ifdef DTSEARCH
#include "UAS_BookcaseEntry.hh"
#include "UAS/DtSR/DtSR_BookcaseEntry.hh"
#include "UAS/DtSR/DtSR_Parser.hh"
#include "UAS/DtSR/DtSR_SearchResultsEntry.hh"
#include "UAS/DtSR/DtSR_SearchResults.hh"
#include "UAS/DtSR/DtSR_SearchEngine.hh"
#include "UAS/DtSR/DtSR_Stems.hh"
#include "UAS/DtSR/Util_Classes/DictLink.hh"
#include "UAS/DtSR/Util_Classes/Dict.hh"
#include "UAS/DtSR/Util_Classes/DictIter.hh"
#else
#include "UAS/Fulcrum/Fulcrum_BookcaseEntry.hh"
#endif

#include "OnlineRender/FontCache.hh"
#include <Dt/CanvasP.h>
#include <Dt/CanvasSegP.h>
#include "OnlineRender/TableDefn.hh"
#include "OnlineRender/SegClientData.hh"

#include "Marks/MarkInfo.hh"

/* #include "dti_cc/CC_Dlist.h" */
#include "dti_cc/CC_Stack.C"
/* #include "dti_cc/CC_String.h" */
#include "Dt/CanvasP.h"
#include "Dt/CanvasSegP.h"

#include "Prelude.h"


#ifdef  _IBMR2

#pragma define(xList<Ancestor *>)
#pragma define(xList<BookTab *>)
#pragma define(xList<MarkTml *>)
#pragma define(xList<MarkIcon *>)
#pragma define(xList<NodeWindowAgent *>)
#pragma define(xList<LibraryAgent *>)
#pragma define(xList<MarkBase *>)
#pragma define(xList<MarkCanvas *>)
#pragma define(xList<Mark_mmdb *>)
#pragma define(xList<const char *>)
#pragma define(xList<MarkBase::open_func_t>)
#pragma define(xList<MarkBase::mb_create_func_t>)

#pragma define(xList<Graphic *>)
#pragma define(xList<GraphicAgent *>)
#pragma define(xList<int>)
#pragma define(xList<long>)
#pragma define(xList<unsigned long>)
#pragma define(xList<BookmarkEdit *>)
#pragma define(xList<InfobaseEntry *>)
#pragma define(xList<Header *>)
#pragma define(xList<PixmapPidObj *>)
#pragma define(xList<PsProcess *>)

/* following are used across more than one browser subdir */
/* (Some local instantiations of Stack are made in OnlineRender subdir) */

#pragma define(Stack<char>)
#pragma define(Stack<char *>)

#pragma define(Stack<_dtCvSegment*>)
#pragma define(Stack<PartialElementFeatures*>)
#pragma define(Stack<TableDefn*>)
#pragma define(Stack<TGDefn*>)
#pragma define(Stack<connective_t>)

#pragma define(UAS_PtrList<UAS_String>)

#pragma define(UAS_Pointer<UAS_String>)
#pragma define(UAS_Pointer<UAS_Common>)
#pragma define(UAS_Pointer<UAS_EmbeddedObject>)


#pragma define(UAS_Pointer<UAS_Collection>)
#pragma define(UAS_Pointer<UAS_Path>)
#pragma define(UAS_Pointer<UAS_StyleSheet>)
#pragma define(UAS_Pointer<Graphic>)
#pragma define(UAS_Pointer<Mark>)
#pragma define(UAS_Pointer<MarkUnitInfo>)

#pragma define(UAS_List<UAS_StyleSheet>)
#pragma define(UAS_List<UAS_Common>)
#pragma define(UAS_List<UAS_SearchResults>)
#pragma define(UAS_List<UAS_SearchEngine>)

#pragma define(UAS_List<MarkUnitInfo>)

#pragma define(UAS_PtrList<FactoryEntry>)
#pragma define(UAS_PtrList<MMDB>)
#pragma define(UAS_PtrList<const char>)

#pragma define(UAS_ObjList<UAS_String>)

#ifdef DTSEARCH
#pragma define(UAS_Pointer<UAS_BookcaseEntry>)
#pragma define(UAS_Pointer<DtSR_BookcaseSearchEntry>)
#pragma define(UAS_Pointer<DtSR_SearchResults>)
#pragma define(UAS_Pointer<DtSR_SearchResultsEntry>)
#pragma define(UAS_Pointer<DtSR_SearchEngine>)
#pragma define(Dict<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >)
#pragma define(Dict<UAS_String, int>)
#pragma define(Dict<UAS_String, unsigned long>)
#pragma define(DictIter<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >)
#pragma define(UAS_List<DtSR_BookcaseSearchEntry>)
#pragma define(UAS_List<DtSR_Stems>)
#pragma define(UAS_List<UAS_List<UAS_String> >)
#else
#pragma define(UAS_PtrList<Fulcrum_BookcaseEntry>)
#endif

#pragma define(xList<UAS_Pointer<UAS_Common> >)
#pragma define(xList<UAS_Pointer<Mark> >)
#pragma define(xList<UAS_Pointer<Mark> >)
#pragma define(xList<UAS_Pointer<Graphic > >)
#pragma define(xList<UAS_SearchScope *>)

#ifndef DtinfoClient
/* GraphicRegistry used with PostScript graphics handling only */
#pragma define(xList<GraphicRegistry>)
#endif

#pragma define(UAS_Sender<UAS_SearchMsg>)
#pragma define(UAS_Sender<UAS_StatusMsg>)
#pragma define(UAS_Sender<UAS_PartialDataMsg>)
#pragma define(UAS_Sender<MarkCreated>)
#pragma define(UAS_Sender<EditMark>)
#pragma define(UAS_Sender<ViewMark>)
#pragma define(UAS_Sender<MarkChanged>)
#pragma define(UAS_Sender<MarkMoved>)
#pragma define(UAS_Sender<MarkDeleted>)
#pragma define(UAS_Sender<MarkSelectionChanged>)
#pragma define(UAS_Sender<LinkAction>)
#pragma define(UAS_Sender<UAS_DocumentRetrievedMsg>)
#pragma define(UAS_Sender<UAS_CollectionRetrievedMsg>)
#pragma define(UAS_Sender<HistoryAdd>)
#pragma define(UAS_Sender<HistoryDelete>)
#pragma define(UAS_Sender<SelectionChanged>)
#pragma define(UAS_Sender<DetachGraphic>)
#pragma define(UAS_Sender<ReAttachGraphic>)
#pragma define(UAS_Sender<ScopeCreated>)
#pragma define(UAS_Sender<ScopeDeleted>)
#pragma define(UAS_Sender<ScopeRenamed>)
#pragma define(UAS_Sender<InputReady>)
#pragma define(UAS_Sender<WantInputReady>)
#pragma define(UAS_Sender<WantInputReadyCancel>)
#pragma define(UAS_Sender<CancelOperation>)
#pragma define(UAS_Sender<UpdateMenu>)

#pragma define(UAS_Receiver<LinkAction>)
#pragma define(UAS_Receiver<UAS_CollectionRetrievedMsg>)
#pragma define(UAS_Receiver<UAS_DocumentRetrievedMsg>)
#pragma define(UAS_Receiver<HistoryAdd>)
#pragma define(UAS_Receiver<HistoryDelete>)
#pragma define(UAS_Receiver<SelectionChanged>)
#pragma define(UAS_Receiver<DetachGraphic>)
#pragma define(UAS_Receiver<ReAttachGraphic>)
#pragma define(UAS_Receiver<DisplayGraphic>)
#pragma define(UAS_Receiver<InputReady>)
#pragma define(UAS_Receiver<WantInputReady>)
#pragma define(UAS_Receiver<WantInputReadyCancel>)
#pragma define(UAS_Receiver<CancelOperation>)
#pragma define(UAS_Receiver<UAS_LibraryDestroyedMsg>)
#pragma define(UAS_Receiver<UpdateMenu>)
#pragma define(Stack<SegClientData*>)

#pragma define(UAS_Pointer<UAS_List<DtSR_BookcaseSearchEntry> >)

#else

#ifdef USL

#pragma instantiate CC_TValSlist<SegClientData*> 
#pragma instantiate CC_TValSlistIterator<SegClientData*> 
#pragma instantiate UAS_Receiver<UpdateMenu>
#pragma instantiate UAS_PtrList<UAS_String>
#pragma instantiate UAS_Pointer<UAS_String>

#pragma instantiate UAS_Pointer<UAS_Common>
#pragma instantiate UAS_Pointer<UAS_EmbeddedObject>
#pragma instantiate UAS_Pointer<UAS_Collection>
#pragma instantiate UAS_Pointer<UAS_Path>
#pragma instantiate UAS_Pointer<UAS_StyleSheet>
#pragma instantiate UAS_Pointer<Graphic>
#pragma instantiate UAS_Pointer<Mark>
#pragma instantiate UAS_Pointer<MarkUnitInfo>

#pragma instantiate UAS_List<UAS_StyleSheet>
#pragma instantiate UAS_List<UAS_Common>
#pragma instantiate UAS_List<UAS_SearchResults>
#pragma instantiate UAS_List<UAS_SearchEngine>

#pragma instantiate UAS_List<MarkUnitInfo>

#pragma instantiate UAS_PtrList<FactoryEntry>
#pragma instantiate UAS_PtrList<MMDB>
#pragma instantiate UAS_PtrList<const char>

#pragma instantiate UAS_Pointer<UAS_BookcaseEntry>
#pragma instantiate UAS_Pointer<DtSR_BookcaseSearchEntry>
#pragma instantiate UAS_Pointer<DtSR_SearchResults>
#pragma instantiate UAS_Pointer<DtSR_SearchResultsEntry>
#pragma instantiate UAS_Pointer<DtSR_SearchEngine>

#pragma instantiate Dict<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
#pragma instantiate Dict<UAS_String, int>
#pragma instantiate Dict<UAS_String, unsigned long>

#pragma instantiate DictIter<UAS_String, unsigned long>
#pragma instantiate DictIter<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
#pragma instantiate UAS_List<DtSR_BookcaseSearchEntry>
#pragma instantiate UAS_List<DtSR_Stems>
#pragma instantiate UAS_List<UAS_List<UAS_String> >

#pragma instantiate UAS_ObjList<UAS_String>

#pragma instantiate xList<Ancestor *>
#pragma instantiate xList<UAS_Pointer<UAS_Common> >
#pragma instantiate xList<BookTab *>
#pragma instantiate xList<MarkTml *>
#pragma instantiate xList<MarkIcon *>
#pragma instantiate xList<NodeWindowAgent *>
#pragma instantiate xList<LibraryAgent *>
#pragma instantiate xList<MarkBase *>
#pragma instantiate xList<MarkCanvas *>
#pragma instantiate xList<UAS_Pointer<Mark> >
#pragma instantiate xList<Mark_mmdb *>
#pragma instantiate xList<const char *>
#pragma instantiate xList<MarkBase::open_func_t>
#pragma instantiate xList<MarkBase::mb_create_func_t>

#pragma instantiate xList<UAS_Pointer<Graphic > >
#pragma instantiate xList<Graphic *>
#pragma instantiate xList<GraphicAgent *>
#pragma instantiate xList<int>
#pragma instantiate xList<long>
#pragma instantiate xList<unsigned long>
#pragma instantiate xList<BookmarkEdit *>
#pragma instantiate xList<UAS_SearchScope *>
#pragma instantiate xList<InfobaseEntry *>
#pragma instantiate xList<Header *>
#pragma instantiate xList<PixmapPidObj *>
#pragma instantiate xList<PsProcess *>

#pragma instantiate UAS_Sender<UAS_SearchMsg>
#pragma instantiate UAS_Sender<UAS_StatusMsg>
#pragma instantiate UAS_Sender<UAS_PartialDataMsg>
#pragma instantiate UAS_Sender<MarkCreated>
#pragma instantiate UAS_Sender<EditMark>
#pragma instantiate UAS_Sender<ViewMark>
#pragma instantiate UAS_Sender<MarkChanged>
#pragma instantiate UAS_Sender<MarkMoved>
#pragma instantiate UAS_Sender<MarkDeleted>
#pragma instantiate UAS_Sender<MarkSelectionChanged>
#pragma instantiate UAS_Sender<UpdateMenu>
#pragma instantiate UAS_PtrList<UAS_Sender<UpdateMenu> >
#pragma instantiate UAS_PtrList<UAS_Receiver<UpdateMenu> >
#pragma instantiate UAS_Sender<LinkAction>
#pragma instantiate UAS_Sender<UAS_DocumentRetrievedMsg>
#pragma instantiate UAS_Sender<UAS_CollectionRetrievedMsg>
#pragma instantiate UAS_Sender<HistoryAdd>
#pragma instantiate UAS_Sender<HistoryDelete>
#pragma instantiate UAS_Sender<SelectionChanged>
#pragma instantiate UAS_Sender<DetachGraphic>
#pragma instantiate UAS_Sender<ReAttachGraphic>
#pragma instantiate UAS_Sender<ScopeCreated>
#pragma instantiate UAS_Sender<ScopeDeleted>
#pragma instantiate UAS_Sender<ScopeRenamed>
#pragma instantiate UAS_Sender<InputReady>
#pragma instantiate UAS_Sender<WantInputReady>
#pragma instantiate UAS_Sender<WantInputReadyCancel>
#pragma instantiate UAS_Sender<CancelOperation>

#pragma instantiate UAS_Receiver<LinkAction>
#pragma instantiate UAS_Receiver<UAS_CollectionRetrievedMsg>
#pragma instantiate UAS_Receiver<UAS_DocumentRetrievedMsg>
#pragma instantiate UAS_Receiver<HistoryAdd>
#pragma instantiate UAS_Receiver<HistoryDelete>
#pragma instantiate UAS_Receiver<SelectionChanged>
#pragma instantiate UAS_Receiver<DetachGraphic>
#pragma instantiate UAS_Receiver<ReAttachGraphic>
#pragma instantiate UAS_Receiver<DisplayGraphic>
#pragma instantiate UAS_Receiver<InputReady>
#pragma instantiate UAS_Receiver<WantInputReady>
#pragma instantiate UAS_Receiver<WantInputReadyCancel>
#pragma instantiate UAS_Receiver<CancelOperation>
#pragma instantiate UAS_Receiver<UAS_LibraryDestroyedMsg>
 
#pragma instantiate Stack<char>
#pragma instantiate Stack<char *>

#pragma instantiate Stack<_dtCvSegment*>
#pragma instantiate Stack<PartialElementFeatures*>
#pragma instantiate Stack<TableDefn*>
#pragma instantiate Stack<TGDefn*>
#pragma instantiate Stack<SegClientData*>

#pragma instantiate CC_TPtrSlist<ColDefn>
#pragma instantiate CC_TPtrSlist<ColFormat>
#pragma instantiate CC_TPtrSlist<RowDefn>

#pragma instantiate hashTable<FontEntry,char>
#pragma instantiate Stack<connective_t>
 
#pragma instantiate CC_TPtrDlist<sr_DtCvSegment>
#pragma instantiate UAS_Pointer<UAS_List<DtSR_BookcaseSearchEntry> >

#else

#ifdef __osf__

#pragma define_template CC_TValSlist<SegClientData*> 
#pragma define_template CC_TValSlistIterator<SegClientData*> 
#pragma define_template UAS_Receiver<UpdateMenu>
#pragma define_template UAS_PtrList<UAS_String>
#pragma define_template UAS_Pointer<UAS_String>

#pragma define_template UAS_Pointer<UAS_Common>
#pragma define_template UAS_Pointer<UAS_EmbeddedObject>
#pragma define_template UAS_Pointer<UAS_Collection>
#pragma define_template UAS_Pointer<UAS_Path>
#pragma define_template UAS_Pointer<UAS_StyleSheet>
#pragma define_template UAS_Pointer<Graphic>
#pragma define_template UAS_Pointer<Mark>
#pragma define_template UAS_Pointer<MarkUnitInfo>

#pragma define_template UAS_List<UAS_StyleSheet>
#pragma define_template UAS_List<UAS_Common>
#pragma define_template UAS_List<UAS_SearchResults>
#pragma define_template UAS_List<UAS_SearchEngine>

#pragma define_template UAS_List<MarkUnitInfo>

#pragma define_template UAS_PtrList<FactoryEntry>
#pragma define_template UAS_PtrList<MMDB>
#pragma define_template UAS_PtrList<const char>

#pragma define_template UAS_Pointer<UAS_BookcaseEntry>
#pragma define_template UAS_Pointer<DtSR_BookcaseSearchEntry>
#pragma define_template UAS_Pointer<DtSR_SearchResults>
#pragma define_template UAS_Pointer<DtSR_SearchResultsEntry>
#pragma define_template UAS_Pointer<DtSR_SearchEngine>
#pragma define_template Dict<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
#pragma define_template Dict<UAS_String, int>
#pragma define_template Dict<UAS_String, unsigned long>
#pragma define_template DictIter<UAS_String, unsigned long>
#pragma define_template DictIter<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
#pragma define_template UAS_List<DtSR_BookcaseSearchEntry>
#pragma define_template UAS_List<DtSR_Stems>
#pragma define_template UAS_List<UAS_List<UAS_String> >

#pragma define_template UAS_ObjList<UAS_String>

#pragma define_template xList<Ancestor *>
#pragma define_template xList<UAS_Pointer<UAS_Common> >
#pragma define_template xList<BookTab *>
#pragma define_template xList<MarkTml *>
#pragma define_template xList<MarkIcon *>
#pragma define_template xList<NodeWindowAgent *>
#pragma define_template xList<LibraryAgent *>
#pragma define_template xList<MarkBase *>
#pragma define_template xList<MarkCanvas *>
#pragma define_template xList<UAS_Pointer<Mark> >
#pragma define_template xList<Mark_mmdb *>
#pragma define_template xList<const char *>
#pragma define_template xList<MarkBase::open_func_t>
#pragma define_template xList<MarkBase::mb_create_func_t>
#pragma define_template xList<UAS_Pointer<Mark> >

#pragma define_template xList<UAS_Pointer<Graphic > >
#pragma define_template xList<Graphic *>
#pragma define_template xList<GraphicAgent *>
#pragma define_template xList<int>
#pragma define_template xList<long>
#pragma define_template xList<unsigned long>
#pragma define_template xList<BookmarkEdit *>
#pragma define_template xList<UAS_SearchScope *>
#pragma define_template xList<InfobaseEntry *>
#pragma define_template xList<Header *>
#pragma define_template xList<PixmapPidObj *>
#pragma define_template xList<PsProcess *>

#pragma define_template UAS_Sender<UAS_SearchMsg>
#pragma define_template UAS_Sender<UAS_StatusMsg>
#pragma define_template UAS_Sender<UAS_PartialDataMsg>
#pragma define_template UAS_Sender<MarkCreated>
#pragma define_template UAS_Sender<EditMark>
#pragma define_template UAS_Sender<ViewMark>
#pragma define_template UAS_Sender<MarkChanged>
#pragma define_template UAS_Sender<MarkMoved>
#pragma define_template UAS_Sender<MarkDeleted>
#pragma define_template UAS_Sender<MarkSelectionChanged>
#pragma define_template UAS_Sender<UpdateMenu>
#pragma define_template UAS_PtrList<UAS_Sender<UpdateMenu> >
#pragma define_template UAS_PtrList<UAS_Receiver<UpdateMenu> >
#pragma define_template UAS_Sender<LinkAction>
#pragma define_template UAS_Sender<UAS_DocumentRetrievedMsg>
#pragma define_template UAS_Sender<UAS_CollectionRetrievedMsg>
#pragma define_template UAS_Sender<HistoryAdd>
#pragma define_template UAS_Sender<HistoryDelete>
#pragma define_template UAS_Sender<SelectionChanged>
#pragma define_template UAS_Sender<DetachGraphic>
#pragma define_template UAS_Sender<ReAttachGraphic>
#pragma define_template UAS_Sender<ScopeCreated>
#pragma define_template UAS_Sender<ScopeDeleted>
#pragma define_template UAS_Sender<ScopeRenamed>
#pragma define_template UAS_Sender<InputReady>
#pragma define_template UAS_Sender<WantInputReady>
#pragma define_template UAS_Sender<WantInputReadyCancel>
#pragma define_template UAS_Sender<CancelOperation>

#pragma define_template UAS_Receiver<LinkAction>
#pragma define_template UAS_Receiver<UAS_CollectionRetrievedMsg>
#pragma define_template UAS_Receiver<UAS_DocumentRetrievedMsg>
#pragma define_template UAS_Receiver<HistoryAdd>
#pragma define_template UAS_Receiver<HistoryDelete>
#pragma define_template UAS_Receiver<SelectionChanged>
#pragma define_template UAS_Receiver<DetachGraphic>
#pragma define_template UAS_Receiver<ReAttachGraphic>
#pragma define_template UAS_Receiver<DisplayGraphic>
#pragma define_template UAS_Receiver<InputReady>
#pragma define_template UAS_Receiver<WantInputReady>
#pragma define_template UAS_Receiver<WantInputReadyCancel>
#pragma define_template UAS_Receiver<CancelOperation>
#pragma define_template UAS_Receiver<UAS_LibraryDestroyedMsg>
 
#pragma define_template Stack<char>
#pragma define_template Stack<char *>

#pragma define_template Stack<_dtCvSegment*>
#pragma define_template Stack<PartialElementFeatures*>
#pragma define_template Stack<TableDefn*>
#pragma define_template Stack<TGDefn*>
#pragma define_template Stack<SegClientData*>

#pragma define_template CC_TPtrSlist<ColDefn>
#pragma define_template CC_TPtrSlist<ColFormat>
#pragma define_template CC_TPtrSlist<RowDefn>

#pragma define_template hashTable<FontEntry,char>
#pragma define_template Stack<connective_t>
 
#pragma define_template CC_TPtrDlist<sr_DtCvSegment>
#pragma define_template UAS_Pointer<UAS_List<DtSR_BookcaseSearchEntry> >

#else

typedef UAS_Pointer<UAS_String>                 _ptrString_;
typedef UAS_Pointer<UAS_Common>                 _ptrCommon_;
typedef UAS_Pointer<UAS_EmbeddedObject>         _ptrEmbeddedObject_;
typedef UAS_Pointer<UAS_Collection>             _ptrCollection_;
typedef UAS_Pointer<UAS_Path>                   _ptrPath_;
typedef UAS_Pointer<UAS_StyleSheet>             _ptrStyleSheet_;
typedef UAS_Pointer<Graphic> _ptrGraphic_;
typedef UAS_Pointer<Mark> _ptrMark_;
typedef UAS_Pointer<MarkUnitInfo> _ptrMarkUnitInfo_;

typedef UAS_List<UAS_StyleSheet>                _listStyleSheet_;
typedef UAS_List<UAS_Common>                    _listCommon_;
typedef UAS_List<UAS_SearchResults>             _listSearchResults_;
typedef UAS_List<UAS_SearchEngine>		_listSearchEngine_;

typedef UAS_List<MarkUnitInfo>			_listMarkUnitInfo_;

typedef UAS_PtrList<FactoryEntry>               _ptrListFactoryEntry_;
typedef UAS_PtrList<MMDB>			_ptrListMMDB_;
typedef UAS_PtrList<const char>			_ptrListConstChar_;
#ifdef DTSEARCH
typedef UAS_Pointer<UAS_BookcaseEntry>	_ptrUAS_BookcaseEntry_;
typedef UAS_Pointer<DtSR_BookcaseSearchEntry>	_ptrDtSR_BookcaseSearchEntry_;
typedef UAS_Pointer<DtSR_SearchResults>	_ptrDtSR_SearchResults_;
typedef UAS_Pointer<DtSR_SearchResultsEntry>	_ptrDtSR_SearchResultsEntry_;
typedef UAS_Pointer<DtSR_SearchEngine>	_ptrDtSR_SearchEngine_;
typedef Dict<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
				_dictStringptrDtSR_SearchResultsEntry_;
typedef Dict<UAS_String, int> _dictUAS_Stringint_;
typedef Dict<UAS_String, unsigned long> _dictStringunsignedlong_;
typedef DictIter<UAS_String, UAS_Pointer<DtSR_SearchResultsEntry> >
				_dictiterStringptrDtSR_SearchResultsEntry_;
typedef UAS_List<DtSR_BookcaseSearchEntry>	_listDtSR_BookcaseSearchEntry_;
typedef UAS_List<DtSR_Stems>	_listDtSR_Stems_;
typedef UAS_List<UAS_List<UAS_String> >	_listlistUAS_String_;
#else
typedef UAS_PtrList<Fulcrum_BookcaseEntry>	_ptrListFulBookcase_;
#endif

typedef UAS_ObjList<UAS_String>			_objListString_;

typedef xList<Ancestor *> _lstAncestor_;
typedef xList<UAS_Pointer<UAS_Common> > _lstPtrTab_;
typedef xList<BookTab *> _lstBookTab_;
typedef xList<MarkTml *> _lstMarkTml_;
typedef xList<MarkIcon *> _lstMarkIcon_;
typedef xList<NodeWindowAgent *> _lstNodeWindowAgent_;
typedef xList<LibraryAgent *> _lstLibraryAgent_;
typedef xList<MarkBase *> _lstMarkBase_;
typedef xList<MarkCanvas *> _lstMarkCanvas_;
typedef xList<UAS_Pointer<Mark> > _lstPtrMark_;
typedef xList<Mark_mmdb *> _lstMark_mmdb_;
typedef xList<const char *> _lstCC_;
typedef xList<MarkBase::open_func_t> _lstMBOpen_func_;
typedef xList<MarkBase::mb_create_func_t> _lstMBCreate_func_;
typedef xList<UAS_Pointer<Mark> > _lstPtrMark_;
#ifndef DtinfoClient
/* GraphicRegistry used with PostScript graphics handling only */
typedef xList<GraphicRegistry> _lstGraphReg_;
#endif
typedef xList<UAS_Pointer<Graphic > > _lstPtrGraph ;
typedef xList<Graphic *>  _lstGraphStar_;
typedef xList<GraphicAgent *>  _lstGraphAgentStar_;
typedef xList<int> _lstInt_;
typedef xList<long> _lstLong_;
typedef xList<unsigned long> _lstUnsignedLong_;
typedef xList<BookmarkEdit *> _lstMrkEdit_;
typedef xList<UAS_SearchScope *> _lstSearchScope_;
typedef xList<InfobaseEntry *> _lstIBEntry_;
typedef xList<Header *> _lstHeader_;
typedef xList<PixmapPidObj *> _lstPixmapPidObj_;
typedef xList<PsProcess *> _lstPsProcess_;

typedef UAS_Sender<UAS_SearchMsg>		_sndSearchMsg_;
typedef UAS_Sender<UAS_StatusMsg>               _sndStatusMsg_;
typedef UAS_Sender<UAS_PartialDataMsg>          _sndPartialDataMsg_;
typedef UAS_Sender<MarkCreated> _sndMarkCreated_;
typedef UAS_Sender<EditMark> _sndEditMark_;
typedef UAS_Sender<ViewMark> _sndViewMark_;
typedef UAS_Sender<MarkChanged> _sndMarkChanged_;
typedef UAS_Sender<MarkMoved> _sndMarkMoved_;
typedef UAS_Sender<MarkDeleted> _sndMarkDeleted_;
typedef UAS_Sender<MarkSelectionChanged> _sndMarkSelectionChanged_;
typedef UAS_Sender<LinkAction> _sndLinkAction_;
typedef UAS_Sender<UAS_DocumentRetrievedMsg> _sndDpyDocTOC_;
typedef UAS_Sender<UAS_CollectionRetrievedMsg> _sndDpyDocNode_;
typedef UAS_Sender<HistoryAdd> _sndHistAdd_;
typedef UAS_Sender<HistoryDelete> _sndHistDelete_;
typedef UAS_Sender<SelectionChanged> _sndSelCh_;
typedef UAS_Sender<DetachGraphic> _sndDetGr_ ;
typedef UAS_Sender<ReAttachGraphic> _sndReAtGr_;
typedef UAS_Sender<ScopeCreated> _sndScopeCreate_;
typedef UAS_Sender<ScopeDeleted> _sndScopeDel_;
typedef UAS_Sender<ScopeRenamed> _sndScopeRen_;
typedef UAS_Sender<InputReady> _sndInputRdy_;
typedef UAS_Sender<WantInputReady> _sndWInputRdy_;
typedef UAS_Sender<WantInputReadyCancel> _sndWInputRdyC_;
typedef UAS_Sender<CancelOperation> _sndCancelOp_;
typedef UAS_Sender<UpdateMenu> _sndUpdateMenu_;

typedef UAS_Receiver<LinkAction> _recLinkAction_;
typedef UAS_Receiver<UAS_CollectionRetrievedMsg> _recDpyDocTOC_;
typedef UAS_Receiver<UAS_DocumentRetrievedMsg> _drecDpyDocNode_;
typedef UAS_Receiver<HistoryAdd> _recHistAdd_;
typedef UAS_Receiver<HistoryDelete> _recHistDel_;
typedef UAS_Receiver<SelectionChanged> _recSelCh_;
typedef UAS_Receiver<DetachGraphic> _recDetGr_;
typedef UAS_Receiver<ReAttachGraphic> _recReAtGr_;
typedef UAS_Receiver<DisplayGraphic> _recDispGr_ ;
typedef UAS_Receiver<InputReady> _recInputRdy_;
typedef UAS_Receiver<WantInputReady> _recWInputRdy_;
typedef UAS_Receiver<WantInputReadyCancel> _recWInputRdyC_;
typedef UAS_Receiver<CancelOperation> _recCancelOp_;
typedef UAS_Receiver<UAS_LibraryDestroyedMsg> _recLibDestroyed_;
typedef UAS_Receiver<UpdateMenu> _recUpdateMenu_;

typedef UAS_PtrList<UAS_String> _ptrListUAS_String_;

/* following are used across more than one browser subdir */
/* (Some local instantiations of Stack are made in OnlineRender subdir) */

typedef Stack<char> _stkChar_;
typedef Stack<char *> _stkCharPtr_;

typedef Stack<_dtCvSegment*> _stkDtCvSegmentPtr_;
typedef Stack<PartialElementFeatures*> _stkPartialElementFeaturesPtr_;
typedef Stack<TableDefn*> _stkTableDefn_ ;
typedef Stack<TGDefn*> _stkTGDefn_ ;
typedef Stack<SegClientData*> _stkSegClientData_ ;

typedef CC_TPtrSlist<ColDefn> _ColDefnList_ ;
typedef CC_TPtrSlist<ColFormat> _ColFormatList_ ;
typedef CC_TPtrSlist<RowDefn> _RowDefnList_ ;

typedef hashTable<FontEntry,char> _hshtabFontEntryChar_;
/* Query subdir refs */
typedef Stack<connective_t> _stkConnectiveT_;
#ifdef UseQSearch
typedef Stack<NTerminal*> _stkNTPtr_;
#endif
/* Basic subdir refs */
typedef CC_TPtrDlist<sr_DtCvSegment> _ccTPtrDLstCvSegmnt_;

#endif
#endif
#endif

#if defined(sun) | defined(hpux)

typedef UAS_Pointer<UAS_String>                 _ptrString_;
typedef UAS_Pointer<UAS_Common>                 _ptrCommon_;
typedef UAS_Pointer<UAS_EmbeddedObject>         _ptrEmbeddedObject_;
typedef UAS_Pointer<UAS_Collection>             _ptrCollection_;
typedef UAS_Pointer<UAS_Path>                   _ptrPath_;
typedef UAS_Pointer<UAS_StyleSheet>             _ptrStyleSheet_;
typedef UAS_Pointer<Graphic> _ptrGraphic_;
typedef UAS_Pointer<Mark> _ptrMark_;

typedef UAS_List<UAS_StyleSheet>                _listStyleSheet_;
typedef UAS_List<UAS_Common>                    _listCommon_;
typedef UAS_List<UAS_SearchResults>             _listSearchResults_;
typedef UAS_List<UAS_SearchEngine>		_listSearchEngine_;

typedef UAS_PtrList<FactoryEntry>               _ptrListFactoryEntry_;
typedef UAS_PtrList<MMDB>			_ptrListMMDB_;
typedef UAS_PtrList<const char>			_ptrListConstChar_;
#ifdef DTSEARCH
typedef UAS_Pointer<UAS_BookcaseEntry>	_ptrUAS_BookcaseEntry_;
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
typedef UAS_List<UAS_List<UAS_String> >	_listlistUAS_String_;
#else
typedef UAS_PtrList<Fulcrum_BookcaseEntry>	_ptrListFulBookcase_;
#endif

typedef UAS_ObjList<UAS_String>			_objListString_;

typedef xList<Ancestor *> _lstAncestor_;
typedef xList<UAS_Pointer<UAS_Common> > _lstPtrTab_;
typedef xList<BookTab *> _lstBookTab_;
typedef xList<MarkTml *> _lstMarkTml_;
typedef xList<MarkIcon *> _lstMarkIcon_;
typedef xList<NodeWindowAgent *> _lstNodeWindowAgent_;
typedef xList<LibraryAgent *> _lstLibraryAgent_;
typedef xList<MarkBase *> _lstMarkBase_;
typedef xList<MarkCanvas *> _lstMarkCanvas_;
typedef xList<UAS_Pointer<Mark> > _lstPtrMark_;
typedef xList<Mark_mmdb *> _lstMark_mmdb_;
typedef xList<const char *> _lstCC_;
typedef xList<MarkBase::open_func_t> _lstMBOpen_func_;
typedef xList<MarkBase::mb_create_func_t> _lstMBCreate_func_;
typedef xList<UAS_Pointer<Mark> > _lstPtrMark_;
#ifndef DtinfoClient
/* GraphicRegistry used with PostScript graphics handling only */
typedef xList<GraphicRegistry> _lstGraphReg_;
#endif
typedef xList<UAS_Pointer<Graphic > > _lstPtrGraph ;
typedef xList<Graphic *>  _lstGraphStar_;
typedef xList<GraphicAgent *>  _lstGraphAgentStar_;
typedef xList<int> _lstInt_;
typedef xList<long> _lstLong_;
typedef xList<unsigned long> _lstUnsignedLong_;
typedef xList<BookmarkEdit *> _lstMrkEdit_;
typedef xList<UAS_SearchScope *> _lstSearchScope_;
typedef xList<InfobaseEntry *> _lstIBEntry_;
typedef xList<Header *> _lstHeader_;
typedef xList<PixmapPidObj *> _lstPixmapPidObj_;
typedef xList<PsProcess *> _lstPsProcess_;

typedef UAS_Sender<UAS_SearchMsg>		_sndSearchMsg_;
typedef UAS_Sender<UAS_StatusMsg>               _sndStatusMsg_;
typedef UAS_Sender<UAS_PartialDataMsg>          _sndPartialDataMsg_;
typedef UAS_Sender<MarkCreated> _sndMarkCreated_;
typedef UAS_Sender<EditMark> _sndEditMark_;
typedef UAS_Sender<ViewMark> _sndViewMark_;
typedef UAS_Sender<MarkChanged> _sndMarkChanged_;
typedef UAS_Sender<MarkMoved> _sndMarkMoved_;
typedef UAS_Sender<MarkDeleted> _sndMarkDeleted_;
typedef UAS_Sender<MarkSelectionChanged> _sndMarkSelectionChanged_;
typedef UAS_Sender<LinkAction> _sndLinkAction_;
typedef UAS_Sender<UAS_DocumentRetrievedMsg> _sndDpyDocTOC_;
typedef UAS_Sender<UAS_CollectionRetrievedMsg> _sndDpyDocNode_;
typedef UAS_Sender<HistoryAdd> _sndHistAdd_;
typedef UAS_Sender<HistoryDelete> _sndHistDelete_;
typedef UAS_Sender<SelectionChanged> _sndSelCh_;
typedef UAS_Sender<DetachGraphic> _sndDetGr_ ;
typedef UAS_Sender<ReAttachGraphic> _sndReAtGr_;
typedef UAS_Sender<ScopeCreated> _sndScopeCreate_;
typedef UAS_Sender<ScopeDeleted> _sndScopeDel_;
typedef UAS_Sender<ScopeRenamed> _sndScopeRen_;
typedef UAS_Sender<InputReady> _sndInputRdy_;
typedef UAS_Sender<WantInputReady> _sndWInputRdy_;
typedef UAS_Sender<WantInputReadyCancel> _sndWInputRdyC_;
typedef UAS_Sender<CancelOperation> _sndCancelOp_;

typedef UAS_Receiver<LinkAction> _recLinkAction_;
typedef UAS_Receiver<UAS_CollectionRetrievedMsg> _recDpyDocTOC_;
typedef UAS_Receiver<UAS_DocumentRetrievedMsg> _drecDpyDocNode_;
typedef UAS_Receiver<HistoryAdd> _recHistAdd_;
typedef UAS_Receiver<HistoryDelete> _recHistDel_;
typedef UAS_Receiver<SelectionChanged> _recSelCh_;
typedef UAS_Receiver<DetachGraphic> _recDetGr_;
typedef UAS_Receiver<ReAttachGraphic> _recReAtGr_;
typedef UAS_Receiver<DisplayGraphic> _recDispGr_ ;
typedef UAS_Receiver<InputReady> _recInputRdy_;
typedef UAS_Receiver<WantInputReady> _recWInputRdy_;
typedef UAS_Receiver<WantInputReadyCancel> _recWInputRdyC_;
typedef UAS_Receiver<CancelOperation> _recCancelOp_;
typedef UAS_Receiver<UAS_LibraryDestroyedMsg> _recLibDestroyed_;

/* following are used across more than one browser subdir */
/* (Some local instantiations of Stack are made in OnlineRender subdir) */

#if 0
typedef Stack<char> _stkChar_;
typedef Stack<char *> _stkCharPtr_;

typedef Stack<_dtCvSegment*> _stkDtCvSegmentPtr_;
typedef Stack<PartialElementFeatures*> _stkPartialElementFeaturesPtr_;
typedef Stack<TableDefn*> _stkTableDefn_ ;
typedef Stack<TGDefn*> _stkTGDefn_ ;
typedef Stack<SegClientData*> _stkSegClientData_ ;
#endif

#if 0
typedef CC_TPtrSlist<ColDefn> _ColDefnList_ ;
typedef CC_TPtrSlist<ColFormat> _ColFormatList_ ;
typedef CC_TPtrSlist<RowDefn> _RowDefnList_ ;
#endif

typedef hashTable<FontEntry,char> _hshtabFontEntryChar_;
/* Query subdir refs */
typedef Stack<connective_t> _stkConnectiveT_;
#ifdef UseQSearch
typedef Stack<NTerminal*> _stkNTPtr_;
#endif
/* Basic subdir refs */
typedef CC_TPtrDlist<sr_DtCvSegment> _ccTPtrDLstCvSegmnt_;

#endif

