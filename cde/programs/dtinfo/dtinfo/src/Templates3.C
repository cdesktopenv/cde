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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: Templates3.C /main/6 1996/08/21 15:58:12 drk $ */

#define C_xList
#define C_InputNotifier
#define L_Support

#define C_SearchScope
#define L_Search

#define C_QueryTerm
#define C_QueryGroup
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

#define EXPAND_TEMPLATES
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
#include "OnlineRender/FontCache.hh"
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

/* #include "dti_cc/CC_Dlist.h" */
#include "dti_cc/CC_Stack.C"
/* #include "dti_cc/CC_String.h" */
#include "Dt/CanvasP.h"
#include "Dt/CanvasSegP.h"

#include "Prelude.h"


#ifdef __osf__

// new templates

#pragma define_template UAS_List<UAS_String>
#pragma define_template UAS_List<UAS_SearchResultsEntry>

#pragma define_template UAS_ObjList<int>

#pragma define_template UAS_Pointer<UAS_TextRun>
#pragma define_template UAS_Pointer<UAS_List<UAS_TextRun> >
#pragma define_template UAS_Pointer<UAS_List<UAS_SearchResultsEntry> >
#pragma define_template UAS_Pointer<UAS_List<UAS_TextRun> >

//  UAS_PtrList
#pragma define_template UAS_PtrList<UAS_BookcaseEntry>

//  UAS_PtrList<UAS_Receiver...
#pragma define_template UAS_PtrList<UAS_Receiver<CancelOperation> >
#pragma define_template UAS_PtrList<UAS_Receiver<SelectionChanged> >
#pragma define_template UAS_PtrList<UAS_Receiver<DisplayGraphic> >
#pragma define_template UAS_PtrList<UAS_Receiver<MarkSelectionChanged> >

//  UAS_PtrList<UAS_Sender...
#pragma define_template UAS_PtrList<UAS_Sender<MarkChanged> >
#pragma define_template UAS_PtrList<UAS_Sender<UAS_LibraryDestroyedMsg> >
#pragma define_template UAS_PtrList<UAS_Sender<ViewMark> >
#pragma define_template UAS_PtrList<UAS_Sender<MarkDeleted> >
#pragma define_template UAS_PtrList<UAS_Sender<DetachGraphic> >
#pragma define_template UAS_PtrList<UAS_Sender<ReAttachGraphic> >
#pragma define_template UAS_PtrList<UAS_Sender<SelectionChanged> >
#pragma define_template UAS_PtrList<UAS_Sender<MarkCreated> >
#pragma define_template UAS_PtrList<UAS_Sender<MarkSelectionChanged> >
#pragma define_template UAS_PtrList<UAS_Sender<UAS_DocumentRetrievedMsg> >
#pragma define_template UAS_PtrList<UAS_Sender<ScopeCreated> >
#pragma define_template UAS_PtrList<UAS_Sender<ScopeDeleted> >

//  UAS_Receiver
#pragma define_template UAS_Receiver<MarkDeleted>
#pragma define_template UAS_Receiver<ScopeDeleted>
#pragma define_template UAS_Receiver<EditMark>
#pragma define_template UAS_Receiver<MarkMoved>
#pragma define_template UAS_Receiver<ViewMark>
#pragma define_template UAS_Receiver<MarkChanged>
#pragma define_template UAS_Receiver<MarkCreated>
#pragma define_template UAS_Receiver<MarkSelectionChanged>
#pragma define_template UAS_Receiver<UAS_ErrorMsg>
#pragma define_template UAS_Receiver<UAS_StatusMsg>
#pragma define_template UAS_Receiver<ScopeCreated>
#pragma define_template UAS_Receiver<ScopeRenamed>


#pragma define_template CC_TPtrSlist<sr_DtCvSegment>
#pragma define_template Stack<UAS_String>

//  UAS_Sender
#pragma define_template UAS_Sender<UAS_CancelRetrievalMsg>
#pragma define_template UAS_Sender<UAS_ErrorMsg>
#pragma define_template UAS_Sender<DisplayGraphic>
#pragma define_template UAS_Sender<UAS_LibraryDestroyedMsg>

//  UAS_PtrList<UAS_Receiver...
#pragma define_template UAS_PtrList<UAS_Receiver<UAS_ErrorMsg> >
#pragma define_template UAS_PtrList<UAS_Receiver<WantInputReady> >
#pragma define_template UAS_PtrList<UAS_Receiver<WantInputReadyCancel> >
#pragma define_template UAS_PtrList<UAS_Receiver<UAS_CancelRetrievalMsg> >
#pragma define_template UAS_PtrList<UAS_Receiver<MarkCreated> >
#pragma define_template UAS_PtrList<UAS_Receiver<MarkMoved> >
#pragma define_template UAS_PtrList<UAS_Receiver<HistoryAdd> >
#pragma define_template UAS_PtrList<UAS_Receiver<HistoryDelete> >
#pragma define_template UAS_PtrList<UAS_Receiver<InputReady> >

//  UAS_PtrList<UAS_Sender...
#pragma define_template UAS_PtrList<UAS_Sender<WantInputReady> >
#pragma define_template UAS_PtrList<UAS_Sender<WantInputReadyCancel> >
#pragma define_template UAS_PtrList<UAS_Sender<HistoryDelete> >
#pragma define_template UAS_PtrList<UAS_Sender<MarkMoved> >
#pragma define_template UAS_PtrList<UAS_Sender<UAS_ErrorMsg> >
#pragma define_template UAS_PtrList<UAS_Sender<HistoryAdd> >

#endif
