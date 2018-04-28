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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
//%%  (c) Copyright 1993, 1994 Novell, Inc.
//%%  $XConsortium: DtTt.h /main/5 1996/03/19 10:48:06 barstow $
#ifndef DTTT_H
#define DTTT_H

#include <Tt/tttk.h>
#include "tt_c++.h"
#include "ttChooser_ui.h"
#include "stringChooser_ui.h"

typedef enum {
	DTTT_PROCID,
	DTTT_MESSAGE,
	DTTT_PATTERN,
	DTTT_PTYPE,
	DTTT_OTYPE,
	DTTT_OBJECT,
	DTTT_SESSION,
	DTTT_DTSESSION,
	DTTT_DTFILE,
	DTTT_OP
} DtTtType;

typedef enum {
	_DtTtChooserNone,
	_DtTtChooserMessageOpen,
	_DtTtChooserMessageCreateStandard,
	_DtTtChooserMessageOpSet,
	_DtTtChooserMessageHandlerSet,
	_DtTtChooserMessageDestroy,
	_DtTtChooserPatternOpen,
	_DtTtChooserPatternOpAdd,
	_DtTtChooserPatternSenderAdd,
	_DtTtChooserPatternDestroy,
	_DtTtChooserDtSessionQuit,
	_DtTtChooserDtFileDeleted,
	_DtTtChooserDtFileModified,
	_DtTtChooserDtFileReverted,
	_DtTtChooserDtFileMoved,
	_DtTtChooserDtFileSaved,
	_DtTtChooserDtFileQuit,
	_DtTtChooserProcidSetDefault,
	_DtTtChooserProcidSuspend,
	_DtTtChooserProcidResume,
	_DtTtChooserDtProcidClose
} _DtTtChooserAction;

typedef enum {
	_DtStringChooseNone,
	_DtStringChoosePatternOp,
	_DtStringChoosePatternOtype,
	_DtStringChoosePatternObject,
	_DtStringChoosePatternSenderPtype,
	_DtStringChooseMessageOtype,
	_DtStringChooseMessageObject,
	_DtStringChooseMessageHandlerPtype,
	_DtStringChooseMessageSenderPtype,
	_DtStringChooseMessageStatusString,
	_DtStringChooseMessageArgValSet,
	_DtStringChoosePtype2Declare,
	_DtStringChooseMediaPtype2Declare,
	_DtStringChoosePtype2UnDeclare,
	_DtStringChoosePtype2SetDefault,
	_DtStringChoosePtype2Exists,
	_DtStringChooseSystem,
	_DtStringChoosePutenv,
	_DtStringChooseNetfile2File
} _DtStringChooserAction;

typedef enum {
	_DtSessionChooseNone,
	_DtSessionChoosePattern,
	_DtSessionChooseMessage,
	_DtSessionChooseJoin,
	_DtSessionChooseDtJoin,
	_DtSessionChooseQuit,
	_DtSessionChooseDefault
} _DtSessionChooserAction;

typedef enum {
	_DtFileChooseNone,
	_DtFileChoosePatternAdd,
	_DtFileChooseMessageSet,
	_DtFileChooseJoin,
	_DtFileChooseDtJoin,
	_DtFileChooseQuit,
	_DtFileChooseDefault,
	_DtFileChooseGetModified,
	_DtFileChooseSave,
	_DtFileChooseRevert,
	_DtFileChooseNetfile,
	_DtFileChooseChdir,
	_DtFileChooseTypesLoad
} _DtFileChooserAction;

typedef enum {	// invariant: IArgAdd == ArgAdd + 1
	_DtArgChooseNone,
	_DtArgChoosePatternArgAdd,
	_DtArgChoosePatternIArgAdd,
	_DtArgChoosePatternContextAdd,
	_DtArgChoosePatternIContextAdd,
	_DtArgChooseMessageArgAdd,
	_DtArgChooseMessageIArgAdd,
	_DtArgChooseMessageArgSet,
	_DtArgChooseMessageIArgSet,
	_DtArgChooseMessageContextSet,
	_DtArgChooseMessageIContextSet,
	_DtArgChooseContextJoin,
	_DtArgChooseIContextJoin,
	_DtArgChooseContextQuit,
	_DtArgChooseIContextQuit
} _DtArgChooserAction;

typedef void		(*DtTtMessageUpdateCallback)(
				Widget		propsWin,
				Tt_message	msg,
				Tt_state	oldState,
				Tt_status	lastOperation,
				Boolean		stillExists
			);

/*
 * Returns elem2Append on success, 0 on failure
 */
#define listAppend( list, listCount, type, elem2Append )		\
	(  listGrow( (void **)&list, listCount, sizeof( type ) )	\
	 ? (list[ listCount - 1 ] = elem2Append)			\
	 : 0 )

int			listGrow(
				void **		pList,
				unsigned int &	listCount,
				size_t		elemSize
			);

XmString		DtTtStatusString(
				Tt_status	type
			);
Tt_status		DtTtCreated(
				DtTtType	type,
				const void *	entity,
				const void *	clientData = 0
			);
Tt_status		DtTtCreated(
				DtTtType	type,
				Tt_pattern *	entity,
				const char *	name
			);
void *			DtTtNth(
				DtTtType	type,
				int		n
			);
void *			DtTtNthClientDatum(
				DtTtType	type,
				int		n
			);
void			DtTtNthClientDatumSet(
				DtTtType	type,
				int		n,
				const void *	clientData
			);
int			DtTtIndex(
				DtTtType	type,
				const void *	entity
			);
Tt_status		DtTtDestroyed(
				DtTtType	type,
				const void *	entity
			);
void			DtTtMessageWidgetUpdate(
				Widget		propsWin,
				Tt_message	msg,
				Tt_state	oldState,
				Tt_status	lastOperation,
				Boolean		stillExists
			);
Widget			DtTtMessageWidgetCreate(
				Widget			parent,
				Tt_message		msg,
				DtTtMessageUpdateCallback	notifyProc
			);
Widget			DtTtMessageWidget(
				Tt_message		msg
			);
Widget			DtTtPatternWidgetCreate(
				Widget			parent,
				Tt_pattern		msg,
				Tt_message_callback	notifyProc
			);
Widget			DtTtPatternWidget(
				Tt_pattern		pat
			);
Tt_status		DtTtSetLabel(
				Widget			labelWidget,
				const char *		string
			);
Tt_status		DtTtSetLabel(
				Widget			labelWidget,
				const char *		funcName,
				void *			returnVal
			);
Tt_status		DtTtSetLabel(
				Widget			labelWidget,
				const char *		funcName,
				Tt_status		returnVal
			);
int			DtTtSetLabel(
				Widget			labelWidget,
				const char *		funcName,
				int			returnVal
			);

void			_DtTtChooserSet(
				_DtTtChooserAction	choice,
				void *			item = 0
			);
XmString *		_DtTtChoices(
				DtTtType		type,
				int *			itemCount
			);
void			_DtStringChooserSet(
				_DtStringChooserAction	choice,
				void *			item
			);
void			_DtStringChooserSet(
				_DtStringChooserAction	choice,
				void *			item,
				const char *		val
			);
void			_DtSessionChooserSet(
				_DtSessionChooserAction	choice,
				void *			item
			);
void			_DtFileChooserSet(
				_DtFileChooserAction	choice,
				void *			item
			);
void			_DtArgChooserSet(
				_DtArgChooserAction	choice,
				void *			item,
				int			nth	= 0
			);
void			_DtArgChooserSet(
				_DtArgChooserAction	choice,
				void *			item,
				int			nth,
				Tt_mode			mode,
				Boolean			noValue	= True,
				char *			vtype	= 0,
				char *			val	= 0,
				int			ival	= 0
			);
void			_DtTtMsgCbChooserSet(
				Tt_message		msg
			);
void			_DtTtMsgCbChooserSet(
				Tt_pattern		pat
			);
Tt_message		_DtTtMediaLoadPatCb(
				Tt_message	msg,
				void           *clientdata,
				Tttk_op		op,
				Tt_status	diagnosis,
				unsigned char  *contents,
				int             len,
				char           *file,
				char           *docname
			);
void			_DtTtPatternUpdate(
				Tt_pattern		pat,
				_DtStringChooserAction	choice,
				char *			string
			);
void			_DtTtPatternUpdate(
				Tt_pattern		pat,
				_DtSessionChooserAction	choice,
				char *			session
			);
void			_DtTtPatternUpdate(
				Tt_pattern		pat,
				_DtFileChooserAction	choice,
				char *			file
			);
void			_DtTtPatternUpdate(
				Tt_pattern		pat,
				_DtTtChooserAction	choice,
				char *			op
			);
void			_DtTtPatternUpdate(
				Tt_pattern		pat,
				_DtArgChooserAction	choice,
				Tt_mode			mode,
				char *			vtype,
				char *			val,
				int			ival
			);
void			_DtTtPatternUpdate(
				Tt_pattern		pat,
				Tt_message_callback	cb
			);
void			_DtTtMessageUpdate(
				Tt_message		msg,
				_DtStringChooserAction	choice,
				char *			val
			);
void			_DtTtMessageUpdate(
				Tt_message		msg,
				_DtSessionChooserAction	choice,
				char *			session
			);
void			_DtTtMessageUpdate(
				Tt_message		msg,
				_DtFileChooserAction	choice,
				char *			file
			);
void			_DtTtMessageUpdate(
				Tt_message		msg,
				_DtTtChooserAction	choice,
				char *			op
			);
void			_DtTtMessageUpdate(
				Tt_message		msg,
				_DtArgChooserAction	choice,
				int			nth,
				Tt_mode			mode,
				char *			vtype,
				char *			val,
				int			ival
			);
void			_DtTtMessageUpdate(
				Tt_message		msg,
				Tt_message_callback	cb
			);
void			_DtOpen(
				Widget			label,
				const char *		cmd,
				const char *		tempnamTemplate
			);
void			_DtOpen(
				Widget			label,
				const char *		file
			);
void			_DtOpen(
				Widget			label,
				void *			buf,
				size_t			len,
				const char *		tempnamTemplate
			);
extern "C" {
Boolean			_DtCanHelp(
				const char *		topics
			);
Boolean			_DtHelped(
				Widget			helpDialog
			);
}
void			_DtMan(
				Widget			label,
				const char *		topic
			);
void			snoopIt(
				const char *		callBackType,
				void *			callBack,
				Tt_message		msg,
				Tt_pattern		pat      = 0,
				Boolean			printPat = False
			);


extern int		_DtTtPatsNameKey;

#endif
