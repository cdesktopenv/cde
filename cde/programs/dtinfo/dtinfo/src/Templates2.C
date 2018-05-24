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
/* $XConsortium: Templates2.C /main/11 1996/10/21 17:43:57 rcs $ */

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

// moved ibm stuff here because compilation of Templates.C
// file on ibm was "over the top" - rCs

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

#pragma define(CC_TPtrSlist<ColDefn>)
#pragma define(CC_TPtrSlist<ColFormat>)
#pragma define(CC_TPtrSlist<RowDefn>)

#pragma define(hashTable<FontEntry,char>)
/* Query subdir refs */
#pragma define(Stack<connective_t>)
#ifdef UseQSearch
#pragma define(Stack<NTerminal*>)
#endif
/* Basic subdir refs */
#pragma define(CC_TPtrDlist<sr_DtCvSegment>)

#endif
