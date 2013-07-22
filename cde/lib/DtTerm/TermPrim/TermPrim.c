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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$TOG: TermPrim.c /main/15 1999/10/14 16:35:04 mgreess $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1995, 1996 Digital Equipment Corporation.	        *
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

extern char _DtTermPrimPullInTermWhatString[];
static char *termWhatString = _DtTermPrimPullInTermWhatString;
extern char * _DtTermPrimGetMessage( char *filename, int set, int n, char *s );

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include <Xm/MessageB.h>
#include <Xm/XmP.h>
#include <Xm/DisplayP.h>
#include <Xm/XmPrivate.h>
#include <Xm/VirtKeys.h>
#include <Xm/MwmUtil.h>
#include <Xm/DrawP.h>
#include "TermHeader.h"
#include "TermPrimOSDepI.h"		/* for SETENV_LINES_AND_COLS	*/
#include "TermPrimI.h"
#include "TermPrimP.h"
#include "TermPrimBufferP.h"
#include "TermPrimAction.h"
#include "TermPrimGetPty.h"
#include "TermPrimParser.h"
#include "TermPrimParseTable.h"
#include "TermPrimPendingText.h"
#include "TermPrimPendingTextP.h"
#include "TermPrimRenderFont.h"
#include "TermPrimRenderFontSet.h"
#include "TermPrimSelectP.h"
#include "TermPrimSetUtmp.h"
#include "TermPrimUtil.h"     
#include "TermPrimDebug.h"
#include "TermPrimWMProtocols.h"
#include "TermPrimSetPty.h"
#include <Xm/RepType.h>
#include "TermPrimRepType.h"
#include "TermPrimMessageCatI.h"
#include <X11/CoreP.h>
#include <Xm/ManagerP.h>
#include <signal.h>
#include <ctype.h>
#include <nl_types.h>
#include <wchar.h>
#if defined(linux) || defined(hpV4)
# include <sys/types.h> /* For FD_* macros. */
# include <sys/time.h> /* For select() prototype. */
#else
# include <sys/select.h>
#endif

extern void TermReadKey(Widget w, XEvent *event, String *params,
	Cardinal *num_params);
static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass wc);
static void initializeKeyboard(DtTermPrimData tpd);
static void initializeModes(DtTermPrimData tpd);
static void initializeStraps(DtTermPrimData tpd);
static void Initialize(Widget rw, Widget nw, Arg *args, Cardinal *num_args);
static void Resize(Widget w);
static void Redisplay(Widget w, XEvent *event, Region region);
static Boolean SetValues(Widget cw, Widget rw, Widget nw, ArgList args,
	Cardinal *num_args);
static void Realize(Widget w, XtValueMask *p_valueMask,
	XSetWindowAttributes *attributes);
static void Destroy(Widget w);
static void readPty(XtPointer client_data, int *source,
	XtInputId *id);
static void handleKeyEvents(Widget w, XtPointer closure, XEvent *event,
	Boolean *cont);
static void handleButtonEvents(Widget w, XtPointer closure, XEvent *event,
	Boolean *cont);
static void handleNonMaskableEvents(Widget w, XtPointer eventData,
	XEvent *event, Boolean *cont);
static void handlePropertyChangeEvents(Widget w, XtPointer eventData,
	XEvent *event, Boolean *cont);
static void handleProcessStructureNotifyEvent(Widget w, XtPointer eventData,
	XEvent *event, Boolean *cont);
static Boolean moreInput(int pty);
static void CapsLockUpdate(Widget w, Boolean capsLock);
static void InitializeVerticalScrollBar(Widget w, Boolean initCallbacks);
static void VerticalScrollBarCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void setThickness(Widget widget, int offset, XrmValue *value);

/*
 * on the spot callbacks
 */
static int PreeditStart(
    XIC xic,
    XPointer client_data,
    XPointer call_data);

static void PreeditDone(
    XIC xic,
    XPointer client_data,
    XPointer call_data);

static void PreeditDraw(
    XIC xic,
    XPointer client_data,
    XIMPreeditDrawCallbackStruct *call_data);

static void PreeditCaret(
    XIC xic,
    XPointer client_data,
    XIMPreeditCaretCallbackStruct *call_data);


/* action list for class: Term... */

static XtActionsRec actionsList[] = {
    {"self-insert",		_DtTermPrimActionKeyInput},
    {"insert",		        _DtTermPrimActionKeyInput},
    {"key-release",		_DtTermPrimActionKeyRelease},
    {"grab-focus",		_DtTermPrimSelectGrabFocus},
    {"select-adjust",		_DtTermPrimSelectExtend},
    {"extend-end",		_DtTermPrimSelectExtendEnd},
    {"extend-start",            _DtTermPrimSelectExtendStart},
    {"copy-to",         	_DtTermPrimSelectInsert},
    {"copy-clipboard",          _DtTermPrimSelectCopyClipboardEventIF},
    {"paste-clipboard",         _DtTermPrimSelectPasteClipboardEventIF},
    {"process-bdrag",           _DtTermPrimSelectProcessBDrag},
    {"leave",			_DtTermPrimActionLeave},
    {"enter",			_DtTermPrimActionEnter},
    {"focus-in",		_DtTermPrimActionFocusIn},
    {"focus-out",		_DtTermPrimActionFocusOut},
    {"keymap",			_DtTermPrimActionKeymap},
    {"redraw-display",		_DtTermPrimActionRedrawDisplay},
    {"return",			_DtTermPrimActionReturn},
    {"stop",			_DtTermPrimActionStop},
    {"string",			_DtTermPrimActionString},
    {"tab",			_DtTermPrimActionTab},
    {"select-page",             _DtTermPrimSelectPage},
    {"select-all",              _DtTermPrimSelectAll},
    {"process-cancel",          _DtTermPrimSelectProcessCancel}
};

#define	defaultColumns	80
#define	defaultRows	24

/* the resource list for Term... */
static XtResource resources[] =
{
    {
	DtNcharCursorStyle, DtCCharCursorStyle, DtRDtTermCharCursorStyle,
	sizeof(unsigned char),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.charCursorStyle),
	XmRImmediate, (XtPointer) DtTERM_CHAR_CURSOR_BOX
    },
    {
	DtNconsoleMode, DtCConsoleMode, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.consoleMode),
	XmRImmediate, (XtPointer) False
    },
    {
	DtNblinkRate, DtCBlinkRate, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.blinkRate),
	XmRImmediate, (XtPointer) 250
    },
    {
	DtNbaseWidth, DtCBaseWidth, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.baseWidth),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNbaseHeight, DtCBaseHeight, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.baseHeight),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNwidthInc, DtCWidthInc, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.widthInc),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNheightInc, DtCHeightInc, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.heightInc),
	XmRImmediate, (XtPointer) 0
    },
    {
	DtNtermDevice, DtCTermDevice, XmRInt, sizeof(int),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.pty),
	XmRImmediate, (XtPointer) -1
    },
    {
	DtNtermDeviceAllocate, DtCTermDeviceAllocate, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.ptyAllocate),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNtermSlaveName, DtCTermSlaveName, XmRString, sizeof(char *),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.ptySlaveName),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNsaveLines, DtCSaveLines, XmRString, sizeof(char *),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.saveLines),
	XmRImmediate, (XtPointer) "4s"
    },
    {
	DtNrows, DtCRows, XmRShort, sizeof(short),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.rows),
	XmRImmediate, (XtPointer) defaultRows
    },
    {
	DtNcolumns, DtCColumns, XmRShort, sizeof(short),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.columns),
	XmRImmediate, (XtPointer) defaultColumns
    },
#ifdef	HPVUE
    {
	DtNbackgroundIsSelect, DtCBackgroundIsSelect, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.backgroundIsSelect),
	XtRImmediate, (XtPointer) True
    },
#else	/* HPVUE */
    {
	DtNbackgroundIsSelect, DtCBackgroundIsSelect, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermPrimitiveRec, term.backgroundIsSelect),
	XtRImmediate, (XtPointer) False
    },
#endif	/* HPVUE */
    {
	XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermPrimitiveRec, primitive.traversal_on),
	XtRImmediate, (XtPointer) True
    },
    {
	DtNshadowType, DtCShadowType, XmRShadowType, sizeof (unsigned char),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.shadowType),
	XmRImmediate, (XtPointer) DtSHADOW_IN
    },
    {
	XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermPrimitiveRec, primitive.shadow_thickness),
	XmRCallProc, (XtPointer) setThickness
    },
    {
	DtNmarginWidth, DtCMarginWidth, XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.marginWidth),
	XmRImmediate, (XtPointer) 2
    },
    {
	DtNmarginHeight, DtCMarginHeight, XmRVerticalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.marginHeight),
	XmRImmediate, (XtPointer) 2
    },
    {
	DtNuserBoldFont, DtCUserBoldFont, XmRFontList,
	sizeof(XmFontList),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.boldFontList),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNuserFont, DtCUserFont, XmRFontList,
	sizeof(XmFontList),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.fontList),
	XmRImmediate, (XtPointer) NULL
    },
    {
	XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
	sizeof (Dimension),
	XtOffsetOf( struct _DtTermPrimitiveRec, primitive.highlight_thickness),
	XmRCallProc, (XtPointer) setThickness
    },
    {
	DtNvisualBell, DtCVisualBell, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.visualBell),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNmarginBell, DtCMarginBell, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.marginBell),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNnMarginBell, DtCNMarginBell, XmRInt, sizeof(int),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.nMarginBell),
	XtRImmediate, (XtPointer) 8
    },
    {
	DtNjumpScroll, DtCJumpScroll, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.jumpScroll),
	XtRImmediate, (XtPointer) True
    },
    {
	DtNsubprocessLoginShell, DtCSubprocessLoginShell, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.subprocessLoginShell),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNsubprocessTerminationCallback, DtCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.subprocessTerminationCallback),
	XmRPointer, (XtPointer) NULL
    },
    {
	DtNstatusChangeCallback, DtCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.statusChangeCallback),
	XmRPointer, (XtPointer) NULL
    },
    {
	DtNoutputLogCallback, DtCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.outputLogCallback),
	XmRPointer, (XtPointer) NULL
    },
    {
	DtNinputVerifyCallback, DtCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.inputVerifyCallback),
	XmRPointer, (XtPointer) NULL
    },
    {
	DtNverticalScrollBar, DtCVerticalScrollBar, XmRWidget, sizeof(Widget),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.verticalScrollBar),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNsubprocessPid, DtCSubprocessPid, XmRInt, sizeof(int),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.subprocessPid),
	XtRImmediate, (XtPointer) -1
    },
    {
	DtNsubprocessExec, DtCSubprocessExec, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.subprocessExec),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNsubprocessTerminationCatch, DtCSubprocessTerminationCatch,
	XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.subprocessTerminationCatch),
	XmRImmediate, (XtPointer) True
    },
    {
	DtNsubprocessCmd, DtCSubprocessCmd, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.subprocessCmd),
	XtRImmediate, (XtPointer) NULL
    },
    {
	DtNsubprocessArgv, DtCSubprocessArgv, XtRStringArray, sizeof(String *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.subprocessArgv),
	XmRImmediate, (XtPointer) NULL
    },
    {
	DtNemulationId, DtCEmulationId, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.emulationId),
	XtRImmediate, (XtPointer) "DtTermPrimitiveWidget"
    },
    {
	DtNtermId, DtCTermId, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.termId),
	XtRImmediate, (XtPointer) "dumb"
    },
    {
	DtNtermName, DtCTermName, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.termName),
	XtRImmediate, (XtPointer) "dumb"
    },
    {
	DtNttyModes, DtCTtyModes, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.ttyModes),
	XtRImmediate, (XtPointer) NULL
    },
    {
	DtNcsWidth, DtCCsWidth, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.csWidth),
	XtRImmediate, (XtPointer) NULL
    },
    {
	DtNkshMode, DtCKshMode, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.kshMode),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNpointerBlank, DtCPointerBlank, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.pointerBlank),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNpointerBlankDelay, DtCPointerBlankDelay, XmRInt, sizeof(int),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.pointerBlankDelay),
	XtRImmediate, (XtPointer) 2
    },
    {
	DtNpointerShape, DtCCursor, XtRCursor, sizeof(Cursor),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.pointerShape),
	XtRString,  (XtPointer) "xterm"
    },
    {
	DtNpointerColor, DtCForeground, XtRPixel, sizeof(Pixel), 
	XtOffsetOf( struct _DtTermPrimitiveRec, term.pointerColor),
	XtRString,  (XtPointer) "XtDefaultForeground"
    },
    {
	DtNpointerColorBackground, DtCBackground, XtRPixel, sizeof(Pixel),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.pointerColorBackground),
	XtRString,  (XtPointer) "XtDefaultBackground"
    },
    {
	DtNmapOnOutput, DtCMapOnOutput , XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.mapOnOutput ),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNmapOnOutputDelay, DtCMapOnOutputDelay, XmRInt, sizeof(int),       
	XtOffsetOf( struct _DtTermPrimitiveRec, term.mapOnOutputDelay ),
	XtRImmediate, (XtPointer) 0
    },
    {
	DtNlogging, DtCLogging , XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.logging ),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNlogFile, DtCLogFile , XmRString, sizeof(char *),   
	XtOffsetOf( struct _DtTermPrimitiveRec, term.logFile ),
	XtRImmediate, (XtPointer) NULL
    },
    {
	DtNlogInhibit, DtCLogInhibit , XmRBoolean, sizeof(Boolean), 
	XtOffsetOf( struct _DtTermPrimitiveRec, term.logInhibit ),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNreverseVideo, DtCReverseVideo, XmRBoolean, sizeof(Boolean),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.reverseVideo ),
	XtRImmediate, (XtPointer) False
    },
    {
        DtNallowSendEvents , DtCAllowSendEvents , XmRBoolean, sizeof(Boolean),
        XtOffsetOf(struct _DtTermPrimitiveRec, term.allowSendEvents),
        XtRImmediate, (XtPointer) False
    }
};

/*
 * Goofy debug'ed code by HP.
 * IBM's JP kbd accepts Shift+KP_Multiply as a valid key sequence.
 * I don't know why HP added non-exposed(internal) Xt's action ???
 * Anyway, remove KP_Multiply entry from defaultTranslations[]
 */
static char defaultTranslations[] = "\
	Ctrl<Key>Cancel:	stop(long)\n\
	~Ctrl<Key>Cancel:	stop()\n\
	<Key>Tab:		tab()\n\
	<Key>KP_Tab:		tab()\n\
	<KeyRelease>:		key-release()\n\
	<KeyPress>:		insert()\n\
        ~Shift~Ctrl<Btn1Down>:  grab-focus()\n\
        Shift~Ctrl<Btn1Down>:   extend-start()\n\
        ~Ctrl<Btn1Motion>:      select-adjust()\n\
        ~Ctrl<Btn1Up>:          extend-end()\n\
        ~Shift<Btn2Down>:       process-bdrag()\n\
        ~Shift<Btn2Up>:         copy-to()\n\
	<EnterWindow>:		enter()\n\
	<LeaveWindow>:		leave()\n\
	<FocusIn>:		focus-in()\n\
	<FocusOut>:		focus-out()\n\
	";


/* global class record for instances of class: TermPrim
 */

externaldef(termclassrec) DtTermPrimitiveClassRec dtTermPrimitiveClassRec =
{
    /* core class record */
    {
	/* superclass		*/	(WidgetClass) &xmPrimitiveClassRec,
	/* class_name		*/	"DtTermPrim",
	/* widget_size		*/	sizeof(DtTermPrimitiveRec),
	/* class_initialize	*/	ClassInitialize,
	/* class_part_init	*/	ClassPartInitialize,
	/* class_inited		*/	FALSE,
	/* initialize		*/	Initialize,
	/* initialize_hook	*/	(XtArgsProc) NULL,
	/* realize		*/	Realize,
	/* actions		*/	actionsList,
	/* num_actions		*/	XtNumber(actionsList),
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class		*/	NULLQUARK,
	/* compress_motion	*/	TRUE,
	/* compress_exposure	*/	FALSE,
	/* compress_enterlv	*/	TRUE,
	/* visible_interest	*/	TRUE,
	/* destroy		*/	Destroy,
	/* resize		*/	Resize,
	/* expose		*/	Redisplay,
	/* set_values		*/	SetValues,
	/* set_values_hook	*/	(XtArgsFunc) NULL,
	/* set_values_almost	*/	XtInheritSetValuesAlmost,
	/* get_values_hook	*/	(XtArgsProc) NULL,
	/* accept_focus		*/	(XtAcceptFocusProc) NULL,
	/* version		*/	XtVersion,
	/* callback_private	*/	(XtPointer) NULL,
	/* tm_table		*/	defaultTranslations,
	/* query_geometry	*/	(XtGeometryHandler) NULL,
	/* display_accelerator	*/	(XtStringProc) NULL,
	/* extension		*/	(XtPointer) NULL,
    },

    /* primitive class record */
    {
	/* Primitive border_highlight	*/  XmInheritWidgetProc,
	/* Primitive border_unhighlight	*/  XmInheritWidgetProc,
	/* translations			*/  "" /*NULL*/ /*XtInheritTranslations*/,
	/* arm_and_activate		*/  NULL,
	/* get resources		*/  (XmSyntheticResource *) NULL,
	/* num get_resources		*/  0,
	/* extension			*/  (XtPointer) NULL,
    },

    /* term class record */
    {
	/* parser_start_state		*/  &_DtTermPrimStateStart,
	/* use_history_buffer		*/  False,
	/* allow_scroll_below_buffer	*/  False,
	/* wrap_right_after_insert	*/  False,
        /* buffer_create_proc           */  _DtTermPrimBufferCreateBuffer,
	/* buffer_free_proc		*/  _DtTermPrimBufferFreeBuffer,
	/* term_insert_proc		*/  _DtTermPrimInsertText,
        /* sizeOfBuffer                 */  (short) sizeof(TermBufferRec),
        /* sizeOfLine                   */  (short) sizeof(TermLineRec),
        /* sizeOfEnh                    */  (short) sizeof(TermEnhPart),
    }
};

externaldef(termwidgetclass) WidgetClass dtTermPrimitiveWidgetClass =
	(WidgetClass)&dtTermPrimitiveClassRec;

/* Class Initialize...
 */
static void
ClassInitialize(void)
{
    /* register our resource converters... */
    (void) _DtTermPrimInitRepTypes();

    return;
}

static void
ClassPartInitialize(WidgetClass w_class)
{
    (void) DtTermInitialize();
}

static void
initializeKeyboard
(
    DtTermPrimData tpd
)
{
    /* 
    ** Make sure the keyboard is unlocked...
    */
    tpd->keyboardLocked.escape      = False;
    tpd->keyboardLocked.xferPending = False;
    tpd->keyboardLocked.error       = False;
    tpd->keyboardLocked.record      = False;

    /* other keyboard modes...
    */
    tpd->autoLineFeed               = False;
    tpd->halfDuplex                 = False;
}

/* 
** initialize the term-generic straps...
*/
static void
initializeStraps
(
    DtTermPrimData tpd
)
{
    tpd->transmitFunctions = False;
    tpd->autoWrapRight     = True;
}

/* 
** initialize the term-generic modes...
*/
static void
initializeModes
(
    DtTermPrimData tpd
)
{
    tpd->scrollLockMode    = SCROLL_LOCKoff;
}

/*
** initialize on the spot data...
*/
static void
initializeOTS
(
    DtTermPrimData tpd
)
{
    tpd->onthespot = (OnTheSpotData)XtMalloc(sizeof(OnTheSpotDataRec));
    tpd->onthespot->start = 0;
    tpd->onthespot->end = 0;
    tpd->onthespot->pre_len = 0;
    tpd->onthespot->cursor = 0;
    tpd->onthespot->under_preedit = False;
}


void
_DtTermPrimGetFontSet
(
    Widget w,
    XmFontList fontList,
    XFontSet *fontSet,
    XFontStruct **font
)
{
    XmFontContext fontContext;
    XmFontListEntry fontListEntry;
    XmFontType fontType;
    XtPointer pointer;

    *fontSet = (XFontSet) 0;
    *font = (XFontStruct *) 0;

    if (!XmFontListInitFontContext(&fontContext, fontList)) {
	(void) fprintf(stderr, "XmFontListInitFontContext() failed\n");
	(void) exit(1);
    }

    /* suppress the CodeCenter warning:
     * "Assignment in conditional 'while' expression."...
     */
    /*SUPPRESS 624*/
    while ((fontListEntry = XmFontListNextEntry(fontContext))) {
	pointer = XmFontListEntryGetFont(fontListEntry, &fontType);
	if (fontType == XmFONT_IS_FONTSET) {
	    int i;
	    int num_fonts;
	    XFontStruct **fonts;
	    char **fontNames;

	    Debug('f', fprintf(stderr, ">>fontType == XmFONT_IS_FONTSET\n"));
	    *fontSet = (XFontSet) pointer;
	    num_fonts = XFontsOfFontSet(*fontSet, &fonts, &fontNames);
	    for (i = 0; i < num_fonts; i++) {
		Debug('f', fprintf(stderr, ">>  font %d: %s\n", i + 1,
			fontNames[i]));
	    }
	} else {
	    unsigned long ret;
	    Debug('f', fprintf(stderr, ">>fontType != XmFONT_IS_FONTSET\n"));
	    *font = (XFontStruct *) pointer;
	    if (XGetFontProperty(*font, XA_FONT, &ret)) {
		Debug('f', fprintf(stderr, ">>font: %s\n",
			XGetAtomName(XtDisplay(w), ret)));
	    }
	}
    }

#ifdef	NOTDEF
    if (!*fontSet) {
	unsigned long ret;
	char *fontName;
	char **missingCharsetList;
	int missingCharsetCount;

	/* build a fontSet from the font... */
	if (XGetFontProperty(*font, XA_FONT, &ret)) {
	    fontName = XGetAtomName(XtDisplay(w), ret);
	}
	*fontSet = XCreateFontSet(XtDisplay(w),
		fontName,
		&missingCharsetList,
		&missingCharsetCount,
		(char **) 0);
	if (missingCharsetCount > 0) {
	    int i;

	    for (i = 0; i < missingCharsetCount; i++)
		(void) fprintf(stderr, "missing charset in fontset \"%s\"\n",
			missingCharsetList[i]);
	}
    }
#endif	/* NOTDEF */

    /* DKS: at some point, we may want to do something with the other
     * fonts in the fontList as well...
     */
    /* free up malloc'ed memory... */
    (void) XmFontListFreeFontContext(fontContext);
}

static TermFont
CreateRenderFont
(
    Widget		  w,
    XmFontList		  fontList,
    XFontSet		 *retFontSet,
    XFontStruct		**retFont
)
{
    TermFont		  termFont;
    XFontSet		  fontSet = (XFontSet) 0;
    XFontStruct		 *font = (XFontStruct *) 0;

    /* get our fontset from the fontlist...  */
    (void) _DtTermPrimGetFontSet(w, fontList, &fontSet, &font);

    /* generate a TermFont from either the fontset or the font... */
    if (fontSet) {
	termFont = _DtTermPrimRenderFontSetCreate(w, fontSet);
    } else {
	termFont = _DtTermPrimRenderFontCreate(w, font);
    }

    /* return the font and fontSet if requested... */
    if (retFontSet)
	*retFontSet = fontSet;
    if (retFont)
	*retFont = font;

    /* return the generated font... */
    return(termFont);
}

static void
AdjustWindowUnits
(
    Widget		  w
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    /* let's adjust the units by the base font size... */
    if (tw->term.fontSet) {
	XFontSetExtents *fontSetExtents;
	int i;
	int num_fonts;
	XFontStruct **fonts;
	char **fontNames;

	num_fonts = XFontsOfFontSet(tw->term.fontSet, &fonts, &fontNames);
	/* look for the single byte font that includes the normal
	 * ascii characters...
	 */
	for (i = 0; i < num_fonts; i++) {
	    if ((fonts[i]->min_byte1 == 0) && (fonts[i]->max_byte1 == 0) &&
		    (fonts[i]->min_char_or_byte2 <= 'A') &&
		    (fonts[i]->max_char_or_byte2 >= 'Z')) {
		break;
	    }
	}
	if (i >= num_fonts) {
	    /* look for the multi-bypte font that includes the normal ascii
	     * characters...
	     */
	    for (i = 0; i < num_fonts; i++) {
		if (fonts[i]->min_byte1 == 0) {
		    break;
		}
	    }
	}
	if (i >= num_fonts) {
	    /* as a last resort, just use the first font... */
	    i = 0;
	}

	fontSetExtents = XExtentsOfFontSet(tw->term.fontSet);

	/* build termFont for this fontset... */
	tw->term.widthInc = fonts[i]->max_bounds.width;
	tw->term.heightInc = fontSetExtents->max_logical_extent.height;
        /* why are there two "ascents"?  TMH */
	tw->term.ascent = -fontSetExtents->max_logical_extent.y;
	tw->term.tpd->ascent = tw->term.ascent;

	tw->term.tpd->cellWidth = tw->term.widthInc;
	tw->term.tpd->cellHeight = tw->term.heightInc;
    } else {
	tw->term.widthInc = tw->term.font->max_bounds.width;
	tw->term.heightInc = tw->term.font->ascent + tw->term.font->descent;
        /* why are there two "ascents"? TMH   */
	tw->term.ascent = tw->term.font->ascent;
	tw->term.tpd->ascent = tw->term.font->ascent;

	tw->term.tpd->cellWidth = tw->term.widthInc;
	tw->term.tpd->cellHeight = tw->term.heightInc;
    }
}

static KeyCode *
GetCapsLockKeyCodes
(
    XModifierKeymap		 *modifierMapping,
    short			 *numCapsLockKeyCodes
)
{
    KeyCode			 *capsLockKeyCodes;
    int				  i1;

    /* initialize number of caps lock key codes... */
    *numCapsLockKeyCodes = 0;

    /* malloc storage for the keycodes (it will be less than or equal to
     * the max)...
     */
    capsLockKeyCodes = (KeyCode *)
	    XtMalloc(modifierMapping->max_keypermod * sizeof(KeyCode));

    /* copy them over...  */
    for (i1 = 0; i1 < modifierMapping->max_keypermod; i1++) {
	capsLockKeyCodes[*numCapsLockKeyCodes] =
		modifierMapping->modifiermap[LockMapIndex *
		modifierMapping->max_keypermod + i1];
	/* only count non-zero keycodes... */
	if (capsLockKeyCodes[*numCapsLockKeyCodes])
	    (*numCapsLockKeyCodes)++;
    }

    return(capsLockKeyCodes);
}

static unsigned int
GetMetaMask
(
    XModifierKeymap		 *modifierMapping,
    KeySym			 *keyboardMapping,
    int				  keysymsPerKeycode,
    int				  minKeycodes,
    int				  maxKeycodes
)
{
    unsigned int		  i1;
    unsigned int		  metaMask;
    Boolean			  match;
    int				  mapIndex;
    KeyCode			  thisKeyCode;
    KeySym			  thisKeySym;

    /* figure out what modifier corresponds to the meta key.
     * If there is none, return 0...
     */
    for (match = False, metaMask = Mod1Mask, mapIndex = Mod1MapIndex;
	    (mapIndex <= Mod5MapIndex) && !match;
	    metaMask <<= 1, mapIndex++) {
	for (i1 = 0; i1 < modifierMapping->max_keypermod; i1++) {
	    thisKeyCode = modifierMapping->modifiermap[mapIndex *
		    modifierMapping->max_keypermod + i1];
	    if (thisKeyCode < minKeycodes)
		break;
	    thisKeySym = keyboardMapping[(thisKeyCode - minKeycodes) *
		    keysymsPerKeycode];
	    if ((thisKeySym == XK_Meta_L) || (thisKeySym == XK_Meta_R)) {
		/* we found either meta_l or meta_r... */
		match = True;
		break;
	    }
	}

	if (match) {
	    /* we found either meta_l or meta_r... */
	    break;
	}
    }

    if (match) {
	return(metaMask);
    }

    return(0);
}

/*ARGSUSED*/
static void
Initialize(Widget ref_w, Widget w, Arg *args, Cardinal *num_args)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd;
    XmFontContext fontContext;
    XmFontListEntry fontListEntry;
    XmFontType fontType;
    XModifierKeymap *modifierMapping;
    KeySym *keyboardMapping;
    int minKeycodes;
    int maxKeycodes;
    int keysymsPerKeycode;
    XmRepTypeId shadowTypeID;
    int i;

    Debug('T', timeStamp("TermPrim Initialize() starting"));


    /* initialize... */
    /* we need to insure that everthing is initialized.  Let's do a
     * checklist...
     */
    /* lower euid */
    (void) _DtTermPrimToggleSuidRoot(False);

    /* Widget verticalScrollBar		set via setvalues		*/
    /* short verticalScrollBarValue */
    tw->term.verticalScrollBarValue = 0;

    /* short verticalScrollBarMaximum */
    tw->term.verticalScrollBarMaximum = 0;

    /* short verticalScrollBarSliderSize */
    tw->term.verticalScrollBarSliderSize = 0;

    /* short verticalScrollBarPageIncrement */
    tw->term.verticalScrollBarPageIncrement = 0;

    /* unsigned char charCursorStyle	set via setvalues		*/
    /* int blinkRate			set via setvalues		*/
    /* Boolean stickyPrevCursor		set via setvalues		*/
    /* Boolean stickyNextCursor		set via setvalues		*/
    /* Boolean backgroundIsSelect	set via setvalues		*/
    /* Boolean visualBell		set via setvalues		*/
    /* Boolean marginBell		set via setvalues		*/
    /* int nMarginBell			set via setvalues		*/
    /* Boolean jumpScroll		set via setvalues		*/
    /* Boolean hasFocus */
    tw->term.hasFocus = False;

    /* char *termId			set via setvalues		*/
    /* char *termName			set via setvalues		*/
    /* char *ttyModes			set via setvalues		*/
    /* unsigned char shadowType		set via setvalues		*/
    /* XmFontList fontList		set via setvalues		*/
    /* XFontStruct *font */
    tw->term.font = (XFontStruct *) 0;
    tw->term.boldFont = (XFontStruct *) 0;

    /* XFontSet fontSet */
    tw->term.fontSet = (XFontSet) 0;
    tw->term.boldFontSet = (XFontSet) 0;

    /* Boolean haveFontSet */
    tw->term.haveFontSet = False;

    /* char *saveLines			set via setvalues		*/
    /* short rows			set via setvalues		*/
    /* short columns			set via setvalues		*/
    /* Dimension marginHeight		set via setvalues		*/
    /* Dimension marginWidth		set via setvalues		*/
    /* int baseWidth			set via setvalues		*/
    /* int baseHeight			set via setvalues		*/
    /* int widthInc			set via setvalues		*/
    /* int heightInc			set via setvalues		*/
    /* int ascent */
    tw->term.ascent = 0;

    /* int pty				set via setvalues		*/
    /* int ptyAllocate			set via setvalues		*/
    /* char *ptySlaveName		set via setvalues		*/
    /* int subprocessPid		set via setvalues		*/
    /* char *subprocessCWD */
    tw->term.subprocessCWD = (char *) 0;

    /* char *subprocessCmd		set via setvalues		*/
    /* char **subprocessArgv		set via setvalues		*/
    /* Boolean subprocessLoginShell	set via setvalues		*/
    /* Boolean subprocessTerminationCatch
					set via setvalues		*/
    /* Boolean subprocessExec		set via setvalues		*/
    /* _termSubprocId subprocessId */
    tw->term.subprocessId = (_termSubprocId) 0;

    /* XtCallbackList statusChangeCallback
					set via setvalues		*/
    /* XtCallbackList subprocessTerminationCallback
					set via setvalues		*/

    /* Boolean allowOsfKeysyms */
    tw->term.allowOsfKeysyms = False;

    if (tw->term.backgroundIsSelect) {
	/* set the background pixel based on the select color... */
	(void) XmGetColors(XtScreen(w), w->core.colormap,
		tw->core.background_pixel, NULL, NULL, NULL,
		&tw->core.background_pixel);
    }

    /* malloc terminalData dataspace... */
    tw->term.tpd = (DtTermPrimData) malloc(sizeof(DtTermPrimDataRec));
    tpd = tw->term.tpd;

    /* clear malloc'ed memory... */
    (void) memset(tpd, '\0', sizeof(DtTermPrimDataRec));

    /* initialize any of it...
     */
    tpd->cursorState = CURSORoff;
    tpd->cursorVisible = True;
    tpd->IMHasFocus = False;
    tpd->IMCursorRow = -1;
    tpd->IMCursorColumn = -1;

    /* 
    ** Initialize the keyboard, straps, and modes...
    */
    initializeKeyboard(tpd);
    initializeModes(tpd);
    initializeStraps(tpd);
    initializeOTS(tpd);

    /* initialize scroll variables...
     */

    tpd->useHistoryBuffer = ((DtTermPrimitiveClassRec *) (tw->core.widget_class))->
	    term_primitive_class.use_history_buffer;
    tpd->allowScrollBelowBuffer = ((DtTermPrimitiveClassRec *) (tw->core.widget_class))->
	    term_primitive_class.allow_scroll_below_buffer;
    tpd->wrapRightAfterInsert = ((DtTermPrimitiveClassRec *) (tw->core.widget_class))->
	    term_primitive_class.wrap_right_after_insert;

    /* initialize pending text... */
    tpd->pendingRead = _DtTermPrimPendingTextCreate();
    tpd->pendingWrite = _DtTermPrimPendingTextCreate();

    /*
    ** Initialize the utmp stuff...
    */
    _DtTermPrimUtmpInit(w);
    
    /* 
    ** Initialize the selection inforamtion
    */
    tpd->selectInfo = _DtTermPrimSelectCreate(w);

    /* force unit type to pixels... */
    tw->primitive.unit_type = XmPIXELS;

    /* we have a fontlist.  Get a fontset from it if we can, else get a
     * fontstruct and make a fontset if we can...
     */
    /* get the text default fontlist if we don't have one... */
    if (!tw->term.fontList) {
	tw->term.fontList = XmeGetDefaultRenderTable(w, XmTEXT_FONTLIST);
    }

    tpd->termFont = CreateRenderFont(w, tw->term.fontList,
	    &tw->term.fontSet, &tw->term.font);

    if (tw->term.boldFontList) {
	tpd->boldTermFont = CreateRenderFont(w, tw->term.boldFontList,
		&tw->term.boldFontSet, &tw->term.boldFont);
    } else {
	/* let's try and build a bold fontlist off of the base fontlist... */
	if (tw->term.fontSet) {
	    int num_fonts;
	    XFontStruct **fonts;
	    char **fontNames;
	    char boldFontNames[BUFSIZ];
	    char *c1;
	    char *c2;
	    int i1;
	    int i2;
	    char **missingCharsetList;
	    int missingCharsetCount;

	    Debug('f', fprintf(stderr, ">>generating bold fontset\n"));
	    num_fonts = XFontsOfFontSet(tw->term.fontSet, &fonts, &fontNames);
	    for (i1 = 0, c2 = boldFontNames; i1 < num_fonts; i1++) {
		/* if this is not the first name we need a comma to
		 * separate the names...
		 */
		if (i1 > 0) {
		    *c2++ = ',';
		    *c2++ = ' ';
		}

		/* copy over the first 3 fields... */
		for (c1 = fontNames[i1], i2 = 0; (i2 < 3) && *c1; i2++) {
		    while (*c1 && (*c1 != '-')) {
			*c2++ = *c1++;
		    }
		    if (!*c1) {
			break;
		    }
		    /* copy over the '-'... */
		    *c2++ = *c1++;
		}
		/* make boldFont bold by swapping the bold in for the
		 * weight...
		 */
		(void) strcpy(c2, "bold");
		c2 += strlen("bold");

		/* skip over the weight in the source... */
		while (*c1 && (*c1 != '-')) {
		    c1++;
		}

		/* copy over the rest of the fontname... */
		while (*c1) {
		    *c2++ = *c1++;
		}
	    }

	    /* null term... */
	    *c2 = '\0';

	    /* now create the fontset... */
	    tw->term.boldFontSet = XCreateFontSet(XtDisplay(w),
		    boldFontNames,
		    &missingCharsetList,
		    &missingCharsetCount,
		    (char **) 0);
	    if (missingCharsetCount > 0) {
		int i;

		for (i = 0; i < missingCharsetCount; i++)
		    Debug('f', fprintf(stderr,
			    ">>missing charsets in boldfont \"%s\"\n",
			    missingCharsetList[i]));
		(void) XFreeStringList(missingCharsetList);
		if (tw->term.boldFontSet) {
		    (void) XFreeFontSet(XtDisplay(w), tw->term.boldFontSet);
		    tw->term.boldFontSet = (XFontSet) 0;
		}
	    }

	    /* create a bold render font... */
	    if (tw->term.boldFontSet) {
		tpd->boldTermFont =
			_DtTermPrimRenderFontSetCreate(w, tw->term.boldFontSet);
	    }
	} else if (tw->term.font) {
	    unsigned long ret;
	    char *fontName;
	    char boldFontName[BUFSIZ];
	    char *c1;
	    char *c2;
	    int i2;

	    /* get the fontname associated with the font... */
	    if (XGetFontProperty(tw->term.font, XA_FONT, &ret)) {
		fontName = XGetAtomName(XtDisplay(w), ret);
		/* copy over the first 3 fields... */
		for (c1 = fontName, c2 = boldFontName, i2 = 0;
			(i2 < 3) && *c1; i2++) {
		    while (*c1 && (*c1 != '-')) {
			*c2++ = *c1++;
		    }
		    if (!*c1) {
			break;
		    }
		    /* copy over the '-'... */
		    *c2++ = *c1++;
		}
		/* make boldFont bold by swapping the bold in for the
		 * weight...
		 */
		(void) strcpy(c2, "bold");
		c2 += strlen("bold");

		/* skip over the weight in the source... */
		while (*c1 && (*c1 != '-')) {
		    c1++;
		}

		/* copy over the rest of the fontname... */
		while (*c1) {
		    *c2++ = *c1++;
		}

		/* null term the string... */
		*c2 = '\0';

		tw->term.boldFont = XLoadQueryFont(XtDisplay(w), boldFontName);
		/* create a bold render font... */
		if (tw->term.boldFont) {
		    tpd->boldTermFont =
			    _DtTermPrimRenderFontCreate(w, tw->term.boldFont);
		}
                if (fontName) XFree(fontName) ;
	    }
	}
    }

    /* save away our original fonts as defaults... */
    tpd->defaultTermFont = tpd->termFont;
    tpd->defaultBoldTermFont = tpd->boldTermFont;

    /* look through our XFontSet or XFontStruct and adjust our
     * width and height increments...
     */
    (void) AdjustWindowUnits(w);

    /* initialize the base width/height... */
    tw->term.baseWidth = 2 * (tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginWidth);
    tw->term.baseHeight = 2 * (tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginHeight);

    /* adjust width and height by rows/columns and the font size... */
    tw->core.width =
	    tw->term.columns * tw->term.widthInc + tw->term.baseWidth;
    tw->core.height =
	    tw->term.rows * tw->term.heightInc + tw->term.baseHeight;
    Debug('w', fprintf(stderr, ">>core.width=%u\n", tw->core.width));
    Debug('w', fprintf(stderr, ">>core.height=%u\n", tw->core.height));

    /* set the termData fields... */
    (void) _DtTermPrimParserInitContext(w);
    tpd->parserNotInStartState = False;
    tpd->offsetX = tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginWidth;
    tpd->offsetY = tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginHeight;
    tpd->cellWidth = tw->term.widthInc;
    tpd->cellHeight = tw->term.heightInc;
    tpd->ascent = tw->term.ascent;
    tpd->windowMapped = False ;
    time( &tpd->creationTime) ;         
    if (tw->term.pointerBlank) tpd->pointerFirst = True ;

    /* multi-byte specific fields...
     */
    tpd->mbCurMax = MB_CUR_MAX;
    /* use a debug flag to force ourselves into multi-byte
     * mode for single byte locales...
     */
    DebugF('m', 1, tpd->mbCurMax = MB_LEN_MAX);
    tpd->mbPartialCharLen = 0;	/* no pending partial multi-byte char */

    /* check results of type converters... */
    shadowTypeID = XmRepTypeGetId(XmRShadowType);
    if (shadowTypeID != XmREP_TYPE_INVALID) {
	if (!XmRepTypeValidValue(shadowTypeID, tw->term.shadowType, w))
	    tw->term.shadowType = DtSHADOW_IN;
    }

    /* get the keyboard modifier mapping... */
    modifierMapping = XGetModifierMapping(XtDisplay(w));
    (void) XDisplayKeycodes(XtDisplay(w), &minKeycodes, &maxKeycodes);
    keyboardMapping = XGetKeyboardMapping(XtDisplay(w), minKeycodes,
	    maxKeycodes - minKeycodes + 1, &keysymsPerKeycode);

    /* get the caps lock keycodes... */
    tpd->capsLockKeyCodes = GetCapsLockKeyCodes(modifierMapping,
	    &tpd->numCapsLockKeyCodes);

    /* get the modifier bit that corresponds to meta.
     * If there is none, use mod1...
     */
    tpd->metaMask = GetMetaMask(modifierMapping, keyboardMapping,
	    keysymsPerKeycode, minKeycodes, maxKeycodes);
    if (!tpd->metaMask) {
	/* default to mod1... */
	tpd->metaMask = Mod1Mask;
    }
	    
    (void) XFree(keyboardMapping);
    (void) XFreeModifiermap(modifierMapping);

    (void) XtAddEventHandler(w,
	    (EventMask) KeyPressMask | KeyReleaseMask,
	    False, handleKeyEvents, (Opaque) NULL);
    (void) XtAddEventHandler(w,
	    (EventMask) ButtonPressMask | ButtonReleaseMask,
	    False, handleButtonEvents, (Opaque) NULL);
    (void) XtAddEventHandler(w,
	    (EventMask) 0,
	    True, handleNonMaskableEvents, (Opaque) NULL);
    { 
        Widget sw ;

        for (sw = w; !XtIsShell(sw); sw = XtParent(sw))
	    ;
        (void) XtAddEventHandler(sw, (EventMask) StructureNotifyMask,
		False, handleProcessStructureNotifyEvent, (Opaque) w);
	(void) XtAddEventHandler(sw, (EventMask) PropertyChangeMask,
		False, handlePropertyChangeEvents, (Opaque) w);
    }

    if (tw->term.verticalScrollBar) {
	/* set up the callbacks for the scrollbar... */
	(void) InitializeVerticalScrollBar(w, True);
    }

    tw->term.log_on = False ;
    if (tw->term.logging) {
       _DtTermPrimStartLog(tw) ;
     }

    Debug('T', timeStamp("TermPrim Initialize() finished"));
    return;
}

static void
InitializeVerticalScrollBar(Widget w, Boolean initCallbacks)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    Arg arglist[20];
    int i;
    int value;

    if (initCallbacks) {
	/* set up the scrollbar callbacks... */
#ifdef	NOTDEF
	(void) XtAddCallback(tw->term.verticalScrollBar, XmNdecrementCallback,
		VerticalScrollBarCallback, (XtPointer) w);
	(void) XtAddCallback(tw->term.verticalScrollBar, XmNincrementCallback,
		VerticalScrollBarCallback, (XtPointer) w);

	(void) XtAddCallback(tw->term.verticalScrollBar,
		XmNpageDecrementCallback,
		VerticalScrollBarCallback, (XtPointer) w);
	(void) XtAddCallback(tw->term.verticalScrollBar,
		XmNpageIncrementCallback,
		VerticalScrollBarCallback, (XtPointer) w);

	(void) XtAddCallback(tw->term.verticalScrollBar,
		XmNtoTopCallback, VerticalScrollBarCallback, (XtPointer) w);
	(void) XtAddCallback(tw->term.verticalScrollBar,
		XmNtoBottomCallback, VerticalScrollBarCallback, (XtPointer) w);
#endif	/* NOTDEF */

	(void) XtAddCallback(tw->term.verticalScrollBar, XmNdragCallback,
		VerticalScrollBarCallback, (XtPointer) w);
	(void) XtAddCallback(tw->term.verticalScrollBar,
		XmNvalueChangedCallback, VerticalScrollBarCallback,
		(XtPointer) w);
    }

    if (!tpd->termBuffer) {
	/* no termBuffer yet, we set anything yet... */
	return;
    }

    /* update the scrollbar... */
    if (tw->term.verticalScrollBar) {
	/* we can scroll the height of the history buffer and the
	 * number of used rows less the protected areas...
	 */
	if (tpd->useHistoryBuffer) {
#define	NO_SCROLL_REGION_HISTORY_SCROLL
#ifdef	NO_SCROLL_REGION_HISTORY_SCROLL 
	    tw->term.verticalScrollBarMaximum = tw->term.rows;
	    if ((tpd->scrollLockTopRow <= 0) &&
		    (tpd->scrollLockBottomRow >= (tw->term.rows - 1))) {
		tw->term.verticalScrollBarMaximum += tpd->lastUsedHistoryRow;
	    }
#else	/* NO_SCROLL_REGION_HISTORY_SCROLL */
	    tw->term.verticalScrollBarMaximum = tw->term.rows +
		    tpd->lastUsedHistoryRow;
#endif	/* NO_SCROLL_REGION_HISTORY_SCROLL */
	} else {
	    tw->term.verticalScrollBarMaximum = tpd->lastUsedRow +
		    tpd->scrollLockTopRow -
		    (tw->term.rows - 1 - tpd->scrollLockBottomRow);

	    /* add in any non-existent rows below the last used row...
	     */
	    if (tpd->allowScrollBelowBuffer) {
		/* add in a full screen (less one line and protected areas)
		 * below the last used row...
		 */
		tw->term.verticalScrollBarMaximum += tw->term.rows - 1 -
			tpd->scrollLockTopRow -
			(tw->term.rows - 1 - tpd->scrollLockBottomRow);
	    }
	}

	if (tpd->useHistoryBuffer) {
	    tw->term.verticalScrollBarSliderSize = tw->term.rows;
	} else {
	    tw->term.verticalScrollBarSliderSize =
		    tw->term.rows - tpd->scrollLockTopRow -
		    (tw->term.rows - 1 - tpd->scrollLockBottomRow);
	}

	tw->term.verticalScrollBarPageIncrement =
		tw->term.verticalScrollBarSliderSize;

#ifdef	NO_SCROLL_REGION_HISTORY_SCROLL 
	tw->term.verticalScrollBarValue = tpd->topRow;
	if (tpd->useHistoryBuffer && (tpd->scrollLockTopRow <= 0) &&
		(tpd->scrollLockBottomRow >= (tw->term.rows - 1))) {
	    tw->term.verticalScrollBarValue += tpd->lastUsedHistoryRow;
	}
#else	/* NO_SCROLL_REGION_HISTORY_SCROLL */
	tw->term.verticalScrollBarValue = tpd->topRow + tpd->lastUsedHistoryRow;
#endif	/* NO_SCROLL_REGION_HISTORY_SCROLL */

	i = 0;
	(void) XtSetArg(arglist[i], XmNincrement, 1); i++;
	(void) XtSetArg(arglist[i], XmNminimum, 0); i++;
	(void) XtSetArg(arglist[i], XmNmaximum,
		tw->term.verticalScrollBarMaximum); i++;
	(void) XtSetArg(arglist[i], XmNpageIncrement,
		tw->term.verticalScrollBarPageIncrement); i++;
	(void) XtSetArg(arglist[i], XmNsliderSize,
		tw->term.verticalScrollBarSliderSize); i++;
	(void) XtSetArg(arglist[i], XmNvalue, tw->term.verticalScrollBarValue);
		i++;
	(void) XtSetValues(tw->term.verticalScrollBar, arglist, i);

	Debug('b', fprintf(stderr,
		"InitializeVerticalScrollBar: sb size=%d  min=%d  max=%d  value=%d  pginc=%d\n",
		tw->term.verticalScrollBarSliderSize,
		0,
		tw->term.verticalScrollBarMaximum,
		tw->term.verticalScrollBarValue,
		tw->term.verticalScrollBarPageIncrement));
    }
}

static void
InitOrResizeTermBuffer(Widget w)
{
    DtTermPrimitiveWidget	  tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData		  tpd = tw->term.tpd;
    DtTermPrimitiveClassPart	 *termClassPart = &(((DtTermPrimitiveClassRec *)
	    (tw->core.widget_class))->term_primitive_class);
    long		          lines;
    char       			 *c;
    short			  newColumns;
    short			  newRows;
    short			  reqColumns;
    short			  reqRows;
    short			  newBufferRows = tpd->bufferRows;
    short			  newHistoryBufferRows = tpd->historyBufferRows;
    int				  linesNeeded;
    int				  historyLinesNeeded;
    int				  i1;
    Boolean                       resizeTermBuffer;
    Boolean			  updateWindowSize = False;

    /* set rows and columns... */
    newColumns = MAX(1, ((int)(tw->core.width - (2 * (tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginWidth)))) /
	    tw->term.widthInc);
    newRows = MAX(1, ((int)(tw->core.height - (2 * (tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginHeight)))) /
	    tw->term.heightInc);

    if ((newColumns == tw->term.columns) && (newRows == tw->term.rows) &&
	    tpd->termBuffer) {
	/* we already have a buffer and the size didn't change so,
	 * no change...
	 */
	return;
    }

    /* resize/create the term buffer... */
    if (tpd->termBuffer) {
	/* restore the buffer-to-window ratio of our off-window buffer
	 * is less than 75% of the original off-window buffer...
	 */
	if (tpd->useHistoryBuffer) {
	    if ((100 * (tpd->bufferRows + tpd->historyBufferRows) /
		    newRows) < ((75 * tpd->bufferRowRatio) / 100)) {
		newBufferRows = (tpd->bufferRowRatio * newRows) / 100;
		newHistoryBufferRows = newBufferRows - newRows;
		newBufferRows = newRows;
	    } else {
		/* the buffer needs to be the length of the window... */
		newBufferRows = newRows;
	    }
	} else {
	    if ((100 * tpd->bufferRows / newRows) <
		    ((75 * tpd->bufferRowRatio) / 100)) {
		newBufferRows = (tpd->bufferRowRatio * newRows) / 100;
	    }
	}

	/* it needs to be at least the size of the window...
	 */
	if (newBufferRows < newRows) {
	    newBufferRows = newRows;
	}

        resizeTermBuffer = True;  /* until further notice */

	if (tpd->useHistoryBuffer && tpd->historyBufferRows > 0 )
	{
	    /*
	    ** if we are using a history buffer and have scrolled
	    ** into it, we need to snap back down before we do
	    ** anything...
	    */
	    if (tpd->topRow < 0)
	    {
		(void) _DtTermPrimScrollTextTo((Widget) tw, 0);
	    }

	    /* resize the history buffer first so that we can scroll
	     * data from the active buffer into it (if necessary) without
	     * loosing any data...
	     */
	    
	    if ((newHistoryBufferRows > tpd->historyBufferRows) ||
		(newColumns != tw->term.columns))
	    {
		reqRows    = newHistoryBufferRows;
		reqColumns = newColumns;
	    
		_DtTermPrimBufferResizeBuffer(&tpd->historyBuffer, &reqRows,
                                              &reqColumns);

		if ((reqColumns < newColumns) || 
                    (reqRows < newHistoryBufferRows)) 
		{
		    /*
		    ** we ran out of memory when we tried to resize the
		    ** history buffer, make the necessary adjustments
		    */
		    newColumns           = reqColumns;
		    newHistoryBufferRows = reqRows;
		    
		    /*
		    ** we ran out of memory, no need try and resize the
		    ** term buffer
		    */
		    resizeTermBuffer == False;
		}
	    }

	    if (resizeTermBuffer)
	    {
		if (newBufferRows < tpd->bufferRows)
		{
		    /* we are shrinking the window.  The first choice is to
		     * move lines above the cursor into the history buffer so
		     * that we don't loose any lines in the window.  If there
		     * are not enough lines above the cursor, we will drop
		     * some of the lines off the bottom of the window...
		     */
		    linesNeeded = tpd->bufferRows - newBufferRows;

		    /* clip this by the number of lines above the cursor... */
		    if (linesNeeded > tpd->cursorRow)
		    {
			linesNeeded = tpd->cursorRow;
		    }
		    
		    historyLinesNeeded = linesNeeded - (newHistoryBufferRows -
				                       tpd->lastUsedHistoryRow);

		    if (historyLinesNeeded > 0) {
			/* take them from the top of the history buffer... */
			(void) _DtTermPrimBufferInsertLineFromTB(
			    tpd->historyBuffer, tpd->historyBufferRows - 1,
			    historyLinesNeeded, insertFromTop);
			/* adjust everything... */
			tpd->lastUsedHistoryRow -= historyLinesNeeded;
		    }

		    /* copy the lines over... */
		    for (i1 = 0; i1 < linesNeeded; i1++) {
			termChar *c1;
			short length;
			termChar *overflowChars;
			short overflowCount;
			
			/* get the line from the active buffer... */
			length = _DtTermPrimBufferGetLineLength(tpd->termBuffer,
								i1);
			c1 = _DtTermPrimBufferGetCharacterPointer(tpd->termBuffer,
								  i1, 0);
			
			/* stuff it into the history buffer... */
			(void) _DtTermPrimBufferSetLineWidth(tpd->historyBuffer,
							     tpd->lastUsedHistoryRow,
							     0);
			overflowChars = (termChar *) XtMalloc(BUFSIZ * sizeof (termChar));
			(void) _DtTermPrimBufferInsert(tpd->historyBuffer,
						       tpd->lastUsedHistoryRow,
						       0, c1, length, False,
						       &overflowChars,
						       &overflowCount);
			(void) tpd->lastUsedHistoryRow++;
			(void) XtFree((char *) overflowChars);
		    }
		    
		    /* scroll up the active buffer... */
		    if (linesNeeded > 0) {
			(void) _DtTermPrimBufferInsertLineFromTB(tpd->termBuffer,
								 tpd->bufferRows - 1,
								 linesNeeded,
								 insertFromTop);
			/* adjust everything... */
			tpd->cursorRow -= linesNeeded;
		    }
		}

		if ((newBufferRows != tpd->bufferRows) ||
		    (newColumns != tw->term.columns))
		{
		    reqRows    = newBufferRows;
		    reqColumns = newColumns;
	    
		    _DtTermPrimBufferResizeBuffer(&tpd->termBuffer, &reqRows,
						  &reqColumns);
		    if ((reqColumns < newColumns) || (reqRows < newBufferRows)) 
		    {
			/*
			** we ran out of memory, resize the history buffer
			** to the same width as the terminal buffer,
			** and assume we will succeed...
			*/
			newColumns = reqColumns;
			reqRows    = newHistoryBufferRows;
			
			_DtTermPrimBufferResizeBuffer(&tpd->historyBuffer,
						      &reqRows, &reqColumns);
			newBufferRows = reqRows;
		    }
		}
	    }
	    tpd->historyBufferRows = newHistoryBufferRows;
	    tpd->bufferRows        = newBufferRows;
	}
        else
	{
	    /*
	    ** no history buffer to worry about...
	    */
	    _DtTermPrimBufferResizeBuffer(&tpd->termBuffer, &newBufferRows,
					  &newColumns);
	    tpd->bufferRows = newBufferRows;
	}

	/*
	** on the slight chance that the buffer shrank...
	*/
	newBufferRows = MIN(newBufferRows, newRows);

	/*
	** if we added rows, then malloc a new set of scrollRefreshRows...
	*/
	if (newBufferRows > tw->term.rows)
	{
	    (void) XtFree(tpd->scrollRefreshRows);
	    tpd->scrollRefreshRows = (Boolean *) XtMalloc(newBufferRows *
							  sizeof(Boolean));
	    (void) memset(tpd->scrollRefreshRows, '\0', newBufferRows *
			  sizeof(Boolean));
	}
	
	/* we got this far because we changed the buffer size.  We
	 * will have to update the winsize structure...
	 */
	updateWindowSize = True;

	tw->term.rows    = newBufferRows;
	tw->term.columns = newColumns;
	if (tpd->lastUsedRow > newRows)
	    tpd->lastUsedRow = newRows;

        /* adjust the insert point if necessary... */
        if (tpd->cursorColumn > tw->term.columns - 1)
            tpd->cursorColumn = tw->term.columns - 1;
        if (tpd->cursorRow > tw->term.rows - 1)
    	    tpd->cursorRow = tw->term.rows - 1;

    } else {
#ifdef	SET_WINDOW_SIZE_ONLY_IF_CHANGED
	/* if our rows and columns changed, we will have to update
	 * the winsize structure...
	 */
	if ((tw->term.rows != newRows) || (tw->term.columns != newColumns)) {
	    updateWindowSize = True;
	}
#else	/* SET_WINDOW_SIZE_ONLY_IF_CHANGED */
	/* we always need to update the winsize structure since it will not
	 * be done for us via $LINES and $COLUMNS...
	 */
	updateWindowSize = True;
#endif	/* SET_WINDOW_SIZE_ONLY_IF_CHANGED */

	tw->term.rows    = newRows;
	tw->term.columns = newColumns;

	/* this is the first time and we need to figure out our ratio for
	 * future resizing...
	 */
	if (tw->term.saveLines && *tw->term.saveLines) {
	    lines = strtol(tw->term.saveLines, &c, 0);
	    if (c && (*c == 's')) {
		/* in terms of screens... */
		tpd->bufferRows = (lines + 1) * tw->term.rows;
	    } else {
		tpd->bufferRows = (lines + tw->term.rows);
	    }
	} else {
	    tpd->bufferRows = 2 * tw->term.rows;
	}

	/* calculate the buffer ratio from bufferRows and rows... */
	tpd->bufferRowRatio = (100 * tpd->bufferRows) / tw->term.rows;
	    
	if (tpd->useHistoryBuffer) {
	    /* split up the buffer between the active and history buffers...
	     */
	    tpd->historyBufferRows = tpd->bufferRows - tw->term.rows;
	    tpd->bufferRows = tw->term.rows;
	}

	/* allocate the two buffers... */
	tpd->termBuffer = (*(termClassPart->buffer_create_proc))(
		w,
                tpd->bufferRows,
		tw->term.columns,
		termClassPart->sizeOfBuffer,
		termClassPart->sizeOfLine,
		termClassPart->sizeOfEnh);
	if (tpd->useHistoryBuffer) {
	    tpd->historyBuffer = (*(termClassPart->buffer_create_proc))(
                    w,
		    tpd->historyBufferRows,
		    tw->term.columns,
		    termClassPart->sizeOfBuffer,
		    termClassPart->sizeOfLine,
		    termClassPart->sizeOfEnh);
	    (void) _DtTermPrimBufferSetLinks(tpd->termBuffer,
		    tpd->historyBuffer,
		    NULL);
	    (void) _DtTermPrimBufferSetLinks(tpd->historyBuffer,
		    NULL,
		    tpd->termBuffer);
	}

	/* check for error... */
	if (!tpd->termBuffer ||
		(tpd->useHistoryBuffer && !tpd->historyBuffer)) {
	    /*DKS: this needs to be cleaned up... */
	    (void) fprintf(stderr, "unable to create termBuffer\n");
	    (void) exit(1);
	}

	/* malloc a set of scrollRefreshRows... */
	tpd->scrollRefreshRows = (Boolean *) XtMalloc(tw->term.rows *
		sizeof(Boolean));
	(void) memset(tpd->scrollRefreshRows, '\0', tw->term.rows *
		sizeof(Boolean));

	/* now that we have a term buffer, we can set up our input selector
	 * on our input source (pty or whatever)...
	 */
	(void) _DtTermPrimStartOrStopPtyInput(w);
        /* initialize the insertion point... */
        tpd->cursorColumn = 0;
        tpd->cursorRow = 0;
        tpd->topRow  = 0;
	tpd->lastUsedRow = 1;
	tpd->lastUsedHistoryRow = 0;
    }

    /*
    ** Resize (or creation) is complete, now update the relevant
    ** information.
    ** 
    ** reset scroll lock...
    */
    tpd->scrollLockMode = SCROLL_LOCKoff;
    tpd->scrollLockTopRow = 0;
    tpd->scrollLockBottomRow = newRows - 1;

    /* set the rows and columns for the terminal... */
    if ((tw->term.pty >= 0) && (updateWindowSize)) {
	(void) _DtTermPrimPtySetWindowSize(tw->term.pty,
		   newColumns * tw->term.widthInc + 
		   (2 * (tw->primitive.shadow_thickness +
			 tw->primitive.highlight_thickness + 
			 tw->term.marginWidth)),
		   newRows * tw->term.heightInc +
		   (2 * (tw->primitive.shadow_thickness +
			 tw->primitive.highlight_thickness +
			 tw->term.marginHeight)),
		   newRows, newColumns);
    }

    /* reset margins... */
    tpd->leftMargin = 0;
    tpd->rightMargin = newColumns - 1;

    (void) InitializeVerticalScrollBar(w, False);

    _DtTermPrimSelectResize(w) ;
}
    
static void
Resize(Widget w)
{
    DtTermPrimitiveWidget	  tw = (DtTermPrimitiveWidget) w;

    if (XtIsRealized(w)) {
	/* the first time through, if our size is changed during interactive
	 * placement, we will be called before redisplay and we will be
	 * initializing the buffer.  If we are sized to our requested size,
	 * the resize function will not be called...
	 */
	(void) InitOrResizeTermBuffer(w);

    }
}

/*ARGSUSED*/
static void
Redisplay(Widget w, XEvent *event, Region region)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    /* if this is the first time we are exposed, and we have never had our
     * size changed (i.e., no Resize() invoked), then we need to initialize
     * a few things, so let's force a resize...
     */
    if (XtIsRealized(w)) {
	if (!tw->term.tpd->termBuffer) {
	    InitOrResizeTermBuffer(w);
	}

	Debug('e', fprintf(stderr,
		">>Redisplay()  expose.x=%d  .y=%d  .width=%d  .height=%d\n",
		event->xexpose.x, event->xexpose.y, event->xexpose.width,
		event->xexpose.height));

	(void) _DtTermPrimDrawShadow(w);

	/* expose (refresh) the text... */
	_DtTermPrimCursorOff(w);
	(void) _DtTermPrimExposeText(w, event->xexpose.x, event->xexpose.y,
		event->xexpose.width, event->xexpose.height, True);
	if (event->xexpose.count == 0)
	    _DtTermPrimCursorOn(w);

	/* Envelop our superclass expose method */
	(*(xmPrimitiveClassRec.core_class.expose)) (w, event, region);	
    }
}

/*ARGSUSED*/
static void
handleNonMaskableEvents(Widget w, XtPointer eventData, XEvent *event,
	Boolean *cont)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    unsigned char *buffer = (unsigned char *) 0;
    int fd = tw->term.pty;

    switch (event->type) {
    case GraphicsExpose:
	/*DKS
	if (!tw->term.jumpScroll && tpd->scroll.nojump.scrollPending) {
	DKS*/
	    (void) _DtTermPrimExposeText(w, event->xexpose.x, event->xexpose.y,
		    event->xexpose.width, event->xexpose.height, False);
	/*DKS
	}
	DKS*/
	if (event->xgraphicsexpose.count > 0)
	    break;
	/* else fall through to noexpose... */

    case NoExpose:
	/* clear the scroll flag... */
	tpd->scrollInProgress = False;

	if (tpd->scroll.nojump.pendingScroll) {
	    (void) _DtTermPrimScrollComplete(w, False);
	}

	/* process any pending input... */
	if (TextIsPending(tpd->pendingRead)) {
	    (void) readPty((XtPointer) w, &fd, &tpd->ptyInputId);
	}

	/* reinstall the pty input select... */
	(void) _DtTermPrimStartOrStopPtyInput(w);
	    
	/* free the old buffer... */
	if (buffer) {
	    (void) XtFree((char *) buffer);
	}

	if ((tpd->scroll.nojump.pendingScroll == 0) &&
		!moreInput(tw->term.pty)) {
	    /* turn the cursor back on... */
	    (void) _DtTermPrimCursorOn(w);
	}
	break;
    }
}

static void
handleProcessStructureNotifyEvent(Widget w, XtPointer eventData, XEvent *event,
        Boolean *cont)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) eventData;
    DtTermPrimData tpd = tw->term.tpd;

    switch (event->type) {
    case MapNotify:
	tpd->windowMapped = True;
	if (tpd->mapWarningDialog && tpd->warningDialog) {
	    (void) XtManageChild(tpd->warningDialog);
	    tpd->warningDialogMapped = True;
	    tpd->mapWarningDialog = False;
	}
        break;

    case UnmapNotify:
	tpd->windowMapped = False;
        break;
    }
}

/*ARGSUSED*/
static void
handlePropertyChangeEvents(Widget w, XtPointer eventData, XEvent *event,
	Boolean *cont)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) eventData;
    static Boolean firstTime = True;
    static Atom xa_WM_STATE = None;
    struct _wmStateStruct {
	CARD32 state;
	Window icon;
    } *prop;
    Atom actualType;
    int actualFormat;
    unsigned long nItems;
    unsigned long bytesAfter;


    DebugF('p', 20,
	    fprintf(stderr, "handlePropertyChangeEvents() starting\n"));
    /* initialize things... */
    _DtTermProcessLock();
    if (firstTime) {
	xa_WM_STATE = XInternAtom(XtDisplay(w), "WM_STATE", True);
	firstTime = False;
    }
    _DtTermProcessUnlock();

    DebugF('p', 20,
	    fprintf(stderr, "event == %s\n",
	    XGetAtomName(((XPropertyEvent *) event)->display,
	    ((XPropertyEvent *) event)->atom)));
    /* is this a WM_STATE property change?... */
    if (((XPropertyEvent *) event)->atom == xa_WM_STATE) {
	/* if we have not yet initialized the buffer (and started listening
	 * to the pty), then let's see if we are iconfied and do so...
	 */
	if (!tw->term.tpd->termBuffer) {
	    if (Success == XGetWindowProperty(
		    ((XPropertyEvent *) event)->display,
		    ((XPropertyEvent *) event)->window,
		    xa_WM_STATE,
		    0,
		    (sizeof(struct _wmStateStruct) + 3) / 4,
		    False,
		    AnyPropertyType,
		    &actualType,
		    &actualFormat,
		    &nItems,
		    &bytesAfter,
		    (unsigned char **) &prop)) {
		if ((int) prop->state == IconicState) {
		    DebugF('p', 20,
			    fprintf(stderr, "event == IconicState\n"));
		    InitOrResizeTermBuffer((Widget) tw);
		}
		(void) XFree(prop);
	    }
	}
    }
    DebugF('p', 20,
	    fprintf(stderr, "handlePropertyChangeEvents() finished\n"));
}

/*ARGSUSED*/
static void
InvokeTerminationCallback(Widget w, pid_t pid, int *stat_loc)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermSubprocessTerminationCallbackStruct cb;

    (void) memset(&cb, '\0', sizeof(cb));
    cb.reason = DtCR_TERM_SUBPROCESS_TERMINATION; 
    cb.event = (XEvent *) 0;
    cb.pid = pid;
    cb.status = *stat_loc;

    if (tw->term.subprocessTerminationCallback) {
	(void) XtCallCallbackList(w,
		tw->term.subprocessTerminationCallback, (XtPointer) &cb);
    }
}

/* SetValues...
 */
/*ARGSUSED*/
static Boolean
SetValues(Widget cur_w, Widget ref_w, Widget w, ArgList args,
	Cardinal *num_args)
{
    DtTermPrimitiveWidget cur_tw = (DtTermPrimitiveWidget) cur_w;
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    Boolean flag = False;	/* return value... */

    /* DKS: this needs to be done later...
     */

    /* install/remove/modify the subprocess callback process... */
    if (cur_tw->term.subprocessPid != tw->term.subprocessPid) {
	/* if there was an old handler... */
	if (tw->term.subprocessId) {
	    /* remove the old handler... */
	    (void) _DtTermPrimSubprocRemoveSubproc(w, tw->term.subprocessId);
	    tw->term.subprocessId = (_termSubprocId) 0;
	}

	/* if there is a new handler... */
	if (tw->term.subprocessPid) {
	    /* add it old handler... */
	    tw->term.subprocessId = _DtTermPrimAddSubproc(w,
		    tw->term.subprocessPid, InvokeTerminationCallback, NULL);

	    /* now that we have a handler, we may need to invoke it... */
	    if (tw->term.subprocessTerminationCatch) {
		(void) _DtTermPrimSetChildSignalHandler();
	    }
	}
    }

    if (cur_tw->term.jumpScroll != tw->term.jumpScroll) {
	/* we need to sync up the scroll before we make the change... */
	Boolean newJumpScroll = tw->term.jumpScroll;

	tw->term.jumpScroll = cur_tw->term.jumpScroll;
	(void) _DtTermPrimScrollComplete(w, True);
	tw->term.jumpScroll = newJumpScroll;
    }

    if (cur_tw->term.verticalScrollBar != tw->term.verticalScrollBar) {
	if (tw->term.verticalScrollBar) {
	    /* set up the scrollbar values and callbacks... */
	    (void) InitializeVerticalScrollBar(w, True);
	}
    }

    /* change in rows or columns... */
    if ((cur_tw->term.rows != tw->term.rows) ||
	    (cur_tw->term.columns != tw->term.columns)) {
	XtWidgetGeometry request;
	XtWidgetGeometry reply;

	request.request_mode = (XtGeometryMask) 0;

	/* handle changes in rows... */
	if (cur_tw->term.rows != tw->term.rows) {
	    request.height = tw->term.rows * tw->term.heightInc +
		    2 * (tw->primitive.shadow_thickness +
		    tw->primitive.highlight_thickness + tw->term.marginWidth);
	    request.request_mode |= CWHeight;
	}

	/* handle changes in columns... */
	if (cur_tw->term.columns != tw->term.columns) {
	    request.width = tw->term.columns  * tw->term.widthInc +
		    2 * (tw->primitive.shadow_thickness +
		    tw->primitive.highlight_thickness + tw->term.marginHeight);
	    request.request_mode |= CWWidth;
	}

	/* ignore the return result.  The switch statement makes
	 * it possible to debug the result...
	 */
	switch(XtMakeGeometryRequest(w, &request, &reply)) {
	case XtGeometryAlmost:
	    break;

	case XtGeometryYes:
	    break;

	case XtGeometryNo:
	    break;
	}

	if (XtIsRealized(w)) {
	    /* Let's set the columns and rows back at this point.  They will
	     * be set for real when and if the XtMakeGeometryRequest is
	     * honoured...
	     */
	    tw->term.columns = cur_tw->term.columns;
	    tw->term.rows = cur_tw->term.rows;
	}
    }

    if (cur_tw->term.fontList != tw->term.fontList) {
	XtWidgetGeometry request;
	XtWidgetGeometry reply;

	/* reset the font in the GC's... */
	tw->term.tpd->renderGC.fid = (Font) 0;
	tw->term.tpd->renderReverseGC.fid = (Font) 0;
	tw->term.tpd->clearGC.fid = (Font) 0;

	/* Our font list changed on us.  We need to resize ourself and
	 * recompute our width and height increment values...
	 */
	tw->term.tpd->termFont = CreateRenderFont(w, tw->term.fontList,
		&tw->term.fontSet, &tw->term.font);

	/* look through our XFontSet or XFontStruct and adjust our
	 * width and height increments...
	 */
	(void) AdjustWindowUnits(w);
	request.height = tw->term.rows * tw->term.heightInc +
		2 * (tw->primitive.shadow_thickness +
		tw->primitive.highlight_thickness + tw->term.marginWidth);
	request.width = tw->term.columns  * tw->term.widthInc +
		2 * (tw->primitive.shadow_thickness +
		tw->primitive.highlight_thickness + tw->term.marginHeight);
	request.request_mode = CWWidth | CWHeight;

	/* ignore the return result... */
	(void) XtMakeGeometryRequest(w, &request, &reply);
        (void) XmImVaSetValues(w,
            XmNfontList, tw->term.fontList,
            NULL);
	/* clear the cursor position so that we will recalculate the
	 * im spotLocation with the new font metrics...
	 */
	tw->term.tpd->IMCursorRow = -1;
	tw->term.tpd->IMCursorColumn = -1;
    }

    if (cur_tw->term.boldFontList != tw->term.boldFontList) {
	/* Our bold font has been changed... */
	tw->term.tpd->boldTermFont = CreateRenderFont(w, tw->term.boldFontList,
		(XFontSet *) 0, (XFontStruct **) 0);
    }

    if (cur_tw->term.charCursorStyle != tw->term.charCursorStyle) {
	/* we need to refresh so that we won't get caught in the cursor
	 * transition...
	 */
	flag = True;
    }

    if (cur_tw->term.reverseVideo != tw->term.reverseVideo) {
	/* we need to refresh so that we will turn on or turn off
	 * reverse video...
	 */
	flag = True;
    }

    return(flag);
}

/*
** Input:
**    oldEnv   - pointer to a null terminated list of env strings
**    mergeEnv - pointer to a null terminated list of env strings to merge
** 
** Return:
**    a pointer to a new list of environment strings
** 
**    It is the calling function's responsibility to free the memory
**    allocated for the new list of strings.
**
**    If one of the merge environment strings already exists in the old
**    environment, then the new environment string replaces the old string,
**    otherwise it is appended to the new list of strings.
*/
static char **
_mergeEnv
(
    char **oldEnv,
    char **mergeEnv
)
{
    char **newEnv;
    char **ppChar;

    int    i1;
    int    numOld;
    int    numMerge;
    int    numNew;
    int    numReplace;
    int   *mergeIdx;
    
    /*
    ** count the number of new environment strings
    */
    for (numMerge = 0; mergeEnv[numMerge]; numMerge++)
	;
    
    /*
    ** create and initialize a list of indexs for each of the new strings
    ** (assume they will all be appended (idx == -1) and adjust later)...
    */
    mergeIdx = (int *) XtMalloc(numMerge * sizeof(int));
    for (i1 = 0; i1 < numMerge; i1++) 
    {
	mergeIdx[i1] = -1;
    }
    
    /*
    ** count the number of strings in old environment, and see how many
    ** of the merge strings match old strings
    */
    numReplace = 0;
    for (numOld = 0; oldEnv[numOld]; numOld++)
    {
	/*
	** how many old strings have to be replaced?
	*/
	if (numReplace < numMerge) 
	{
	    for (i1 = 0; i1 < numMerge; i1++)
	    {
		if (mergeIdx[i1] == -1)
		{
		    char *idx;
		    idx = strchr(mergeEnv[i1], '=');
		    
		    if (strncmp(mergeEnv[i1], oldEnv[numOld],
				idx - mergeEnv[i1] + 1) == 0)
		    {
			/*
			** we have a match, remember the index of
			** this string for later...
			*/
			mergeIdx[i1] = numOld;
			numReplace++;
			break;
		    }
		}
	    }
	}
    }
    numNew = numOld + numMerge - numReplace;
    
    /*
    ** make room for the appended strings...
    **
    ** NOTE:
    ** We use malloc here instead of XtMalloc to keep Sentinel from
    ** complaining if putenv reallocs this space.  (Sentinel prints a
    ** warning if memory allocation functions are not used symmetrically
    ** (i.e. memory that is XtMalloc'd should be either XtFree'd or
    ** XtRealloc'd, not free'd or realloc'd).)
    */
    newEnv = (char **) malloc((numNew + 1) * sizeof(char *));
    if (newEnv == NULL)
    {
	printf("L10n MALLOC ERROR\n");
    }

    /*
    ** copy the old environment into the new one, and null terminate the
    ** newEnv list...
    */
    memcpy(newEnv, oldEnv, numOld * sizeof(char *));
    newEnv[numNew] = (char *) NULL;
    
    /*
    ** now merge in the merge strings, the merge string will either replace
    ** the existing string (mergeIdx >= 0) or be appended to the list
    ** (mergeIdx < 0)
    */
    ppChar = newEnv + numOld;
    for (i1 = 0; i1 < numMerge; i1++)
    {
	if (mergeIdx[i1] < 0)
	{
	    /*
	    ** append it to the list
	    */
	    *ppChar = mergeEnv[i1];
	    ppChar++;
	}
	else
	{
	    /*
	    ** replace the existing string
	    */
	    newEnv[mergeIdx[i1]] = mergeEnv[i1];
	}
    }
    
    XtFree((char *)mergeIdx);
    
    return(newEnv);
}

/* Realize...
 */
static void
Realize(Widget w, XtValueMask *p_valueMask, XSetWindowAttributes *attributes)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    Mask valueMask = *p_valueMask;
    sigset_t sigNew;
    char buffer[BUFSIZ];
    char **newEnv = (char **) 0;
    char **oldEnv = (char **) 0;
    extern char **environ;
    char *newEnvStrings[4];
    int i1;
    char *ptyMasterName = (char *) 0;
    XPoint imPoint;
    Arg args[20];
    XIMCallback xim_cb[4];
    Cardinal n = 0;

    Debug('T', timeStamp("TermPrim Realize() starting"));

    /* adjust rows and columns to the window size...
     */
    tw->term.columns = ((int)(tw->core.width - (2 * (tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginWidth)))) /
	    tw->term.widthInc;
    tw->term.rows = ((int)(tw->core.height - (2 * (tw->primitive.shadow_thickness +
	    tw->primitive.highlight_thickness + tw->term.marginHeight)))) /
	    tw->term.heightInc;

    /*
    valueMask |= CWBitGravity | CWDontPropagate;
    attributes->bit_gravity = ForgetGravity;
    attributes->do_not_propagate_mask = ButtonPressMask |
	    ButtonReleaseMask | KeyPressMask | KeyReleaseMask |
	    PointerMotionMask;
	    */
    (void) XtCreateWindow(w, InputOutput, CopyFromParent, valueMask,
	    attributes);

    /*
     * register input method, and set callbacks for on the spot
     * support.
     */
    (void) XmImRegister(w, 0);
    imPoint.x = 0;
    imPoint.y = 0;

    n = 0;
    XtSetArg(args[n], XmNspotLocation, &imPoint); n++;
    XtSetArg(args[n], XmNfontList, tw->term.fontList); n++;
    XtSetArg(args[n], XmNbackground, tw->core.background_pixel); n++;
    XtSetArg(args[n], XmNforeground, tw->primitive.foreground); n++;
    XtSetArg(args[n], XmNbackgroundPixmap, tw->core.background_pixmap); n++;
    XtSetArg(args[n], XmNlineSpace, tw->term.heightInc); n++;

    /*
     * Register on the spot callbacks.
     */
    xim_cb[0].client_data = (XPointer)w;
    xim_cb[0].callback = (XIMProc)PreeditStart;
    xim_cb[1].client_data = (XPointer)w;
    xim_cb[1].callback = (XIMProc)PreeditDone;
    xim_cb[2].client_data = (XPointer)tw;
    xim_cb[2].callback = (XIMProc)PreeditDraw;
    xim_cb[3].client_data = (XPointer)w;
    xim_cb[3].callback = (XIMProc)PreeditCaret;
    XtSetArg(args[n], XmNpreeditStartCallback, &xim_cb[0]); n++;
    XtSetArg(args[n], XmNpreeditDoneCallback, &xim_cb[1]); n++;
    XtSetArg(args[n], XmNpreeditDrawCallback, &xim_cb[2]); n++;
    XtSetArg(args[n], XmNpreeditCaretCallback, &xim_cb[3]); n++;

    XmImSetValues(w, args, n);

    /* block sigchld while we do this... */
    (void) sigemptyset(&sigNew);
    (void) sigaddset(&sigNew, SIGCHLD);
    (void) sigprocmask(SIG_BLOCK, &sigNew, (sigset_t *) 0);

    /* get reference terminal modes before we open the pty (so we don't
     * inherit them from the pty), before we fork (so we only do this
     * once for however many instances we create), and before we break
     * our association from our controlling terminal (so we have something
     * to inherit from)...
     */
    (void) _DtTermPrimPtyGetDefaultModes();

    /* allocate a pty if appropriate... */
    if (tw->term.ptyAllocate) {
        int mode;

	/* turn on suid root...  */
	_DtTermPrimToggleSuidRoot(True);
	tw->term.pty = _DtTermPrimGetPty(&tw->term.ptySlaveName,
		&ptyMasterName);
	/* turn off suid root...  */
	_DtTermPrimToggleSuidRoot(False);

	if (tw->term.pty < 0) {
	    XmeWarning(w, "unable to get pty");

	    /* popup a warning dialog... */
	    (void) _DtTermPrimWarningDialog(w, "unable to get pty");
	}

	/* this is the Spec1170 way to do this.  We probably could 
	   consolidate the various _DtTermPrimGetPtys at this point,
	   but that's Truth & Beauty. */
	if (fcntl(tw->term.pty, F_SETFL, O_NONBLOCK |
		  fcntl(tw->term.pty, F_GETFL, 0)) == -1)
	{
	    XmeWarning(w, "unable to set non-blocking on pty");

	    /* popup a warning dialog... */
	    (void) _DtTermPrimWarningDialog(w, 
				       "unable to set non-blocking on pty");
	}


	if (ptyMasterName) {
	    (void) free(ptyMasterName);
	}
    }

    /* set the initial winsize structure before we kick off the
     * subprocess...
     */
    if (tw->term.pty >= 0) {
	(void) _DtTermPrimPtySetWindowSize(tw->term.pty,
		   tw->term.columns * tw->term.widthInc + 
		   (2 * (tw->primitive.shadow_thickness +
			 tw->primitive.highlight_thickness + 
			 tw->term.marginWidth)),
		   tw->term.rows * tw->term.heightInc +
		   (2 * (tw->primitive.shadow_thickness +
			 tw->primitive.highlight_thickness +
			 tw->term.marginHeight)),
		   tw->term.rows, tw->term.columns);
    }

    /* get the utmp line name to use for searching later... */
    if (tw->term.pty >= 0) {
	tw->term.tpd->utmpId = _DtTermPrimUtmpGetUtLine(-1,
		tw->term.ptySlaveName);
    }

    /*
    ** If pointerBlank is true, then turn on the pointer with blanking,
    *  else turn it on to always stay on.
    */
    if (tw->term.pointerBlank) {(void) _DtTermPrimPointerOn((Widget)tw); }
    else   XDefineCursor(XtDisplay(tw), XtWindow(tw), tw->term.pointerShape);
    _DtTermPrimRecolorPointer((Widget)tw) ;

    /* kick off a subprocess if appropriate.  Don't kick one off if
     * we didn't get a pty...
     */
    if (tw->term.subprocessExec && (tw->term.pty >= 0)) {

	/* DKS: maybe we need to start passing the cmd earlier and not
	 * fake it here...
	 */
	if (!tw->term.subprocessCmd && tw->term.subprocessArgv) {
	    tw->term.subprocessCmd = *tw->term.subprocessArgv;
	}

	/* modify the environment for our child process...
	 */
	/* set the environment variables for TERM, LINES, COLUMNS... */
	oldEnv = environ;

	i1 = 0;
#ifdef	SETENV_LINES_AND_COLS
	(void) sprintf(buffer, "LINES=%d", tw->term.rows);
	newEnvStrings[i1] = XtMalloc(strlen(buffer) + 1);
	(void) strcpy(newEnvStrings[i1++], buffer);

	(void) sprintf(buffer, "COLUMNS=%d", tw->term.columns);
	newEnvStrings[i1] = XtMalloc(strlen(buffer) + 1);
	(void) strcpy(newEnvStrings[i1++], buffer);
#endif	/* SETENV_LINES_AND_COLS */

	if (tw->term.termName && *tw->term.termName)
	{
	    char *fmt = "TERM=%s";

	    newEnvStrings[i1] =
	      XtMalloc(strlen(tw->term.termName) + strlen(fmt) + 1);
	    (void) sprintf(newEnvStrings[i1], fmt, tw->term.termName);
	    i1++;
	}
	/* null term the list of new env strings... */
	newEnvStrings[i1] = (char *) 0;
	
	environ = _mergeEnv(oldEnv, newEnvStrings);

	tw->term.subprocessPid = _DtTermPrimSubprocExec(w,
		tw->term.ptySlaveName,
		tw->term.consoleMode,
		tw->term.subprocessCWD,
		tw->term.subprocessCmd, tw->term.subprocessArgv,
		tw->term.subprocessLoginShell);

	/* free up the new environ... */
	for (i1 = 0; newEnvStrings[i1]; i1++)
	{
	    (void) XtFree(newEnvStrings[i1]);
	}

	if (environ) {
	    /*
	    ** free environ (rather than XtFree it) is since it was
	    ** malloc'd (not XtMalloc'd) in _mergeEnv...
	    */
	    (void) free((char *) environ);
	}
	environ = oldEnv;
    }

    /* set up a signal handler if appropriate... */
    if (tw->term.subprocessPid >= 0)
    {
	tw->term.subprocessId = _DtTermPrimAddSubproc(w,
			            tw->term.subprocessPid,
				    InvokeTerminationCallback, NULL);
	if (tw->term.subprocessTerminationCatch)
	{
	    (void)_DtTermPrimSetChildSignalHandler();
	}
    }

    /* unblock sigchld now that we are done... */
    (void) sigprocmask(SIG_UNBLOCK, &sigNew, (sigset_t *) 0);
    Debug('T', timeStamp("TermPrim Realize() finished"));
}

/* Destroy...
 */
static void
Destroy(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    /* remove our handlers on our shell widget... */
    { 
        Widget sw ;

        for (sw = w; !XtIsShell(sw); sw = XtParent(sw))
	    ;
        (void) XtRemoveEventHandler(sw, (EventMask) StructureNotifyMask,
		False, handleProcessStructureNotifyEvent, (Opaque) w);
	(void) XtRemoveEventHandler(sw, (EventMask) PropertyChangeMask,
		False, handlePropertyChangeEvents, (Opaque) w);
    }

    /*
    ** Unregister our input method (this fixes CMVC 8146).
    */
    XmImUnregister(w);

    /* clean up pty specific stuff...
     */
    /* remove the pty input handler... */
    if (tw->term.tpd && tw->term.tpd->ptyInputId) {
	(void) XtRemoveInput(tw->term.tpd->ptyInputId);
	tw->term.tpd->ptyInputId = (XtInputId) 0;
    }

    /* let's be proactive and send a SIGHUP to all the processes in this
     * instances process group.  For safety sake, let's not do anything
     * if the process id is not >1 (since use of 1 will send the signal
     * to all processes).
     */
    /* turn on suid root...
     */
    _DtTermPrimToggleSuidRoot(True);
    if (tw->term.subprocessPid > 1) {
	(void) kill(-tw->term.subprocessPid, SIGHUP);
    }
    /* turn off suid root...
     */
    _DtTermPrimToggleSuidRoot(False);

    /* clean up our utmp entry... */
    if (tw->term.tpd && tw->term.tpd->utmpId && *tw->term.tpd->utmpId) {
	_DtTermPrimUtmpEntryDestroy(w, tw->term.tpd->utmpId);
	(void) XtFree(tw->term.tpd->utmpId);
	tw->term.tpd->utmpId = (char *) 0;
    }

    /* close and release the...
     */
    if (tw->term.pty >= 0) {
	/* close the pty file descriptor so that:
	 *	    - the child will (passively) get SIGHUP'ed.
	 *	    - the child will get EOF on stdin.
	 */
	(void) close(tw->term.pty);
      /* release the pty (restore owner, group, mode)... */
      if (tw->term.ptySlaveName) {
	    (void) _DtTermPrimReleasePty(tw->term.ptySlaveName);
	}
    }

    /* remove the subproc termination callback... */
    if (tw->term.subprocessId) {
	_DtTermPrimSubprocRemoveSubproc(w, tw->term.subprocessId);
	tw->term.subprocessId = (_termSubprocId) 0;
    }

    /* free storage for current working directory name*/
    if ( tw->term.subprocessCWD) {
       XtFree(tw->term.subprocessCWD);
    }

    /* flush the log file */
    if (tw->term.logging ) {
      _DtTermPrimCloseLog(tw) ;
     }

    if (tw->term.boldFont) {
       (void) XFreeFont(XtDisplay(w), tw->term.boldFont);
     }
    if (tw->term.boldFontSet) {
       (void) XFreeFontSet(XtDisplay(w), tw->term.boldFontSet);
     }

    /* remove the termData structure contents, followed by the structure...
     */
    if (tw->term.tpd) {
	/* remove the cursor timeout... */
	if (tw->term.tpd->cursorTimeoutId) {
	    (void) XtRemoveTimeOut(tw->term.tpd->cursorTimeoutId);
	    tw->term.tpd->cursorTimeoutId = (XtIntervalId) 0;
	}

	/* free up all our GC's...
	 */
	/* render GC... */
	if (tw->term.tpd->renderGC.gc) {
	    (void) XFreeGC(XtDisplay(w), tw->term.tpd->renderGC.gc);
	    tw->term.tpd->renderGC.gc = (GC) 0;
	}
	if (tw->term.tpd->renderReverseGC.gc) {
	    (void) XFreeGC(XtDisplay(w), tw->term.tpd->renderReverseGC.gc);
	    tw->term.tpd->renderReverseGC.gc = (GC) 0;
	}
	/* clear GC... */
	if (tw->term.tpd->clearGC.gc) {
	    (void) XFreeGC(XtDisplay(w), tw->term.tpd->clearGC.gc);
	    tw->term.tpd->clearGC.gc = (GC) 0;
	}
	/* cursor GC... */
	if (tw->term.tpd->cursorGC.gc) {
	    (void) XFreeGC(XtDisplay(w), tw->term.tpd->cursorGC.gc);
	    tw->term.tpd->cursorGC.gc = (GC) 0;
	}

	/* free up our buffers... */
	if (tw->term.tpd->historyBuffer) {
	    _DtTermPrimBufferFreeBuffer(tw->term.tpd->historyBuffer);
	    tw->term.tpd->historyBuffer = NULL;
	}

	if (tw->term.tpd->termBuffer) {
	    _DtTermPrimBufferFreeBuffer(tw->term.tpd->termBuffer);
	    tw->term.tpd->termBuffer = NULL;
	}

	/* free up the scrollRefreshRows... */
	if (tw->term.tpd->scrollRefreshRows) {
	    (void) XtFree((char *) tw->term.tpd->scrollRefreshRows);
	    tw->term.tpd->scrollRefreshRows = (Boolean *) 0;
	}

        /* free up the selection information */
	if (tw->term.tpd->selectInfo) {
	    (void) _DtTermPrimSelectDestroy(w, tw->term.tpd->selectInfo);
	    tw->term.tpd->selectInfo = (TermSelectInfo) 0;
	}

	/* free up pending text... */
	if (tw->term.tpd->pendingRead) {
	    (void) _DtTermPrimPendingTextDestroy(tw->term.tpd->pendingRead);
	    tw->term.tpd->pendingRead = (PendingText) 0;
	}
	if (tw->term.tpd->pendingWrite) {
	    (void) _DtTermPrimPendingTextDestroy(tw->term.tpd->pendingWrite);
	    tw->term.tpd->pendingWrite = (PendingText) 0;
	}

        if (tw->term.tpd->capsLockKeyCodes)
                 (void) XtFree((char *)tw->term.tpd->capsLockKeyCodes) ;

        (void) _DtTermPrimDestroyFont(w,tw->term.tpd->boldTermFont) ;
        (void) _DtTermPrimDestroyFont(w,tw->term.tpd->termFont) ;

        if (tw->term.ptyAllocate && tw->term.ptySlaveName)
               (void) XtFree((char *)tw->term.ptySlaveName);
         

        if (tw->term.tpd->context)
                 (void) XtFree((char *)tw->term.tpd->context);

	/* free up the termData structure... */
	(void) XtFree((char *) tw->term.tpd);
	tw->term.tpd = (DtTermPrimData) 0;

    }
}

/* _DtTermPrimActionEnter...
 */
void
_DtTermPrimActionEnter(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    Debug('F', fprintf(stderr, ">>enterEvent starting\n"));
#ifdef	NOTDEF
    Debug('F', fprintf(stderr,
	    ">>  widget: name=\"%s\"  widget=0x%lx  window=0x%lx\n",
	    tw->core.name, tw, XtWindow(w)));
    Debug('F', fprintf(stderr,
	    ">>  event: window=0x%lx\n  mode=%d  detail=%d\n",
	    event->xcrossing.window, event->xcrossing.mode,
	    event->xcrossing.detail));
#endif	/* NOTDEF */
    Debug('F', _DtTermPrimDebugDumpEvent(stderr, w, event));

    if ((_XmGetFocusPolicy(w) != XmEXPLICIT) &&
	    !(tw->term.hasFocus) &&
	    event->xcrossing.focus &&
	    (event->xcrossing.detail != NotifyInferior)) {
	tw->term.hasFocus = True;
	_DtTermPrimCursorChangeFocus(w);
    }

    if ( tw->term.pointerBlank ) 
	_DtTermPrimPointerFreeze((Widget)tw, False);

    /* update the caps lock flag... */
    (void) CapsLockUpdate(w,
	    (event->xcrossing.state & LockMask) != 0);

    _XmPrimitiveEnter(w, event, params, num_params);
}


/* _DtTermPrimActionLeave...
 */
void
_DtTermPrimActionLeave(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    Debug('F', fprintf(stderr, ">>leaveEvent starting\n"));
#ifdef	NOTDEF
    Debug('F', fprintf(stderr,
	    ">>  widget: name=\"%s\"  widget=0x%lx  window=0x%lx\n",
	    tw->core.name, tw, XtWindow(w)));
    Debug('F', fprintf(stderr,
	    ">>  event: window=0x%lx\n  mode=%d  detail=%d\n",
	    event->xcrossing.window, event->xcrossing.mode,
	    event->xcrossing.detail));
#endif	/* NOTDEF */
    Debug('F', _DtTermPrimDebugDumpEvent(stderr, w, event));

    if ((_XmGetFocusPolicy(w) != XmEXPLICIT) &&
	    tw->term.hasFocus &&
	    event->xcrossing.focus &&
	    (event->xcrossing.detail != NotifyInferior)) {
	    tw->term.hasFocus = False;
	_DtTermPrimCursorChangeFocus(w);
    }

    if ( tw->term.pointerBlank ) 
	_DtTermPrimPointerFreeze((Widget)tw, True);

    /* update the caps lock flag... */
    (void) CapsLockUpdate(w,
	    (event->xcrossing.state & LockMask) != 0);

    _XmPrimitiveLeave(w, event, params, num_params);
}

static Boolean
moreInput(int pty)
{
    fd_set readFD;
    struct timeval timeout;

    /* do a non-blocking select to see if we have any more input.  If so,
     * we don't need to turn the cursor back on.
     */
    FD_ZERO(&readFD);
    FD_SET(pty, &readFD);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    if (!((select(pty + 1, &readFD, 0, 0, &timeout) > 0) &&
	  (FD_ISSET(pty, &readFD))))
	return(False);
    return(True);
}

/*ARGSUSED*/
static void
readPty(XtPointer client_data, int *source, XtInputId *id)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    DtTermPrimData tpd = tw->term.tpd;
    unsigned char buffer[BUFSIZ];
    int len;
    unsigned char *dangleBuffer;
    int dangleBufferLen;
    int retLen;
    PendingTextChunk chunk = (PendingTextChunk) 0;

    Debug('i', fprintf(stderr, ">>readPty() starting\n"));
    tpd->readInProgress = True;
    (void) _DtTermPrimCursorOff((Widget) tw);
    /* if we are using a history buffer and have scrolled into it, we
     * need to snap back down before we do anything...
     */
    if (tpd->useHistoryBuffer && (tpd->topRow < 0)) {
	(void) _DtTermPrimScrollTextTo((Widget) tw, 0);
	(void) _DtTermPrimScrollComplete((Widget) tw, True);
    }

    if (TextIsPending(tpd->pendingRead)) {
	/* take text from the pendingRead buffer instead of doing a read...
	 */
	chunk = _DtTermPrimPendingTextGetChunk(tpd->pendingRead);
	len = chunk->len;
	(void) memcpy(buffer, chunk->bufPtr, len);
    } else {
	len = read(*source, buffer, sizeof(buffer));
	Debug('i', fprintf(stderr, ">>readPty() read len=%d\n", len));
	if (isDebugFSet('i', 1)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	    int i1;

	    (void) fprintf(stderr,
		    ">>readPty() read %d bytes", len);
	    if (len > 0) {
		for (i1 = 0; i1 < len; i1++) {
		    if (!(i1 % 20))
			fputs("\n    ", stderr);
		    (void) fprintf(stderr, " %02x", buffer[i1]);
		}
		(void) fprintf(stderr, "\n");
	    }
	}
    }
	
    if (len > 0) {
        if (!tpd->windowMapped && tw->term.mapOnOutput) {
            /*
            ** map window unless it is too early...
            */
            if (tw->term.mapOnOutputDelay)
                if ((time((time_t *) 0) - tpd->creationTime) >
			tw->term.mapOnOutputDelay) {
                /*
                ** time is up
                */
                tw->term.mapOnOutputDelay = 0 ;
            }

            if (!tw->term.mapOnOutputDelay) {
                Widget sw;

                for (sw = (Widget)tw; !XtIsShell(sw); sw = XtParent(sw))
		    ;
                XtMapWidget(sw);
            }
        }
 
        if (tw->term.log_on) {
            _DtTermPrimWriteLog(tw, buffer, len) ;
        }

	if (tw->term.outputLogCallback) {
	    DtTermOutputLogCallbackStruct cb;

	    cb.reason = DtCR_TERM_OUTPUT_LOG;
	    cb.event = (XEvent *) 0;
	    cb.text = buffer;
	    cb.length = len;

	    (void) XtCallCallbackList((Widget) tw,
		    tw->term.outputLogCallback, &cb);
	}

	if (!_DtTermPrimParseInput((Widget) tw, buffer, len,
		&dangleBuffer, &dangleBufferLen)) {
	    /* we were not able to write out everything and
	     * we need to stuff away the pending text...
	     */
	    if (chunk) {
		/* we didn't finish up the pending text chunk we were
		 * working on, so update the pointers and continue...
		 */
		(void) _DtTermPrimPendingTextReplace(chunk, dangleBuffer,
			dangleBufferLen);
	    } else {
		(void) _DtTermPrimPendingTextAppend(tpd->pendingRead,
			dangleBuffer, dangleBufferLen);
	    }
	    (void) XtFree((char *) dangleBuffer);
	} else if (chunk) {
	    /* we finished a pending chunk, so let's move on... */
	    _DtTermPrimPendingTextRemoveChunk(tpd->pendingRead, chunk);
	}
	if (!tpd->ptyInputId) {
	    /* we need to wait until we get a graphicsexpose (count==0)
	     * or a noexpose...
	     */
	    /* we know we have more input, so we don't need to turn on
	     * the cursor...
	     */
	    tpd->readInProgress = False;
	    Debug('i', fprintf(stderr, ">>readPty() finished\n"));
	    return;
	}
    }

    if (!moreInput(tw->term.pty)) {
	/* we won't be getting an input select so we need to check on
	 * pending text and force a read if we still have some...
	 */
	if (TextIsPending(tpd->pendingRead)) {
	    (void) XtAppAddTimeOut(XtWidgetToApplicationContext((Widget) tw),
		    0, _DtTermPrimForcePtyRead, (XtPointer) tw);
	} else {
	    /* turn the cursor back on... */
	    (void) _DtTermPrimCursorOn((Widget) tw);
	}
    }
    tpd->readInProgress = False;
    Debug('i', fprintf(stderr, ">>readPty() finished\n"));
}

/*ARGSUSED*/
void
_DtTermPrimForcePtyRead(XtPointer client_data, XtIntervalId *id)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    DtTermPrimData tpd = tw->term.tpd;
    int fd = tw->term.pty;

    (void) _DtTermPrimStartOrStopPtyInput((Widget) tw);

    if (TextIsPending(tpd->pendingRead)) {
	(void) readPty(client_data, &fd, &tpd->ptyInputId);
    }
}

void
_DtTermPrimLoopBackData(Widget w, char *data, int dataLength)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    int fd = tw->term.pty;

    /* queue up the pending text... */
    (void) _DtTermPrimPendingTextAppend(tpd->pendingRead, (unsigned char *) data,
	    dataLength);

    /* if we have a read in process, we can return now... */
    if (tpd->readInProgress) {
	return;
    }

    /* if we are set up to select on pty input, then we can force a read
     * now...
     */
    if (tpd->ptyInputId) {
	(void) readPty((XtPointer) w, &fd, &tpd->ptyInputId);
    }

    /* if not, we will force a read when we turn select on input back
     * on...
     */
}
    
void
DtTermDisplaySend
(
    Widget		  w,
    unsigned char	 *buffer,
    int			  length
)
{
    _DtTermWidgetToAppContext(w);

    /* for now, it looks like _DtTermPrimLoopBackData will take care
     * of everything for us...
     */
    _DtTermAppLock(app);
    (void) _DtTermPrimLoopBackData(w, (char *) buffer, length);
    _DtTermAppUnlock(app);
}

/*ARGSUSED*/
static void
writePty(XtPointer client_data, int *source, XtInputId *id)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    DtTermPrimData tpd = tw->term.tpd;

    Debug('o', fprintf(stderr, ">>writePty() starting\n"));
    
    /* 
    ** write some text from list of pending text chunks
    */
    _DtTermPrimPendingTextWrite(tpd->pendingWrite, tw->term.pty);

    /* 
    ** turn off the write select as appropriate
    */
    _DtTermPrimStartOrStopPtyOutput((Widget)client_data);
    Debug('o', fprintf(stderr, ">>writePty() finished\n"));
}

/*ARGSUSED*/
void
_DtTermPrimActionKeyRelease(Widget w, XEvent *event, String *params,
	Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    XKeyEvent *keyEvent = (XKeyEvent *) event;
    int i;
    
    Debug('i', fprintf(stderr, ">>_DtTermPrimActionKeyRelease() starting\n"));
    if (keyEvent->type != KeyRelease) {
	(void) fprintf(stderr,
		"_DtTermPrimActionKeyRelease: incorrect event type %d\n",
		keyEvent->type);
	return;
    }

    /* check for caps lock... */
    for (i = 0; i < tpd->numCapsLockKeyCodes; i++) {
	if (tpd->capsLockKeyCodes[i] == keyEvent->keycode) {
	    /* caps lock has toggled... */
	    (void) CapsLockUpdate(w, !tpd->capsLock);
	    break;
	}
    }
}

/*ARGSUSED*/
void
_DtTermPrimActionKeyInput(Widget w, XEvent *event, String *params,
	Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    XKeyEvent *keyEvent = (XKeyEvent *) event;
    KeySym keysym;
    Status status;
    unsigned char string[BUFSIZ];
    int nbytes;
    int i;
    Boolean synEscape = False;  /* synthesize escape key from meta key */
                                /* -kshMode                            */

    Debug('i', fprintf(stderr, ">>_DtTermPrimActionKeyInput() starting\n"));
    if (keyEvent->type != KeyPress) {
	(void) fprintf(stderr, "_DtTermPrimActionKeyInput: incorrect event type %d\n",
		keyEvent->type);
	return;
    }
    if (KEYBOARD_LOCKED(tpd->keyboardLocked)) {
        /* keyboard locked -- ring the bell...
         */
        (void) _DtTermPrimBell(w);
        return;
     }

    if (tw->term.kshMode &&
	    ((keyEvent->state & tpd->metaMask) == tpd->metaMask)) {
       keyEvent->state &= ~tpd->metaMask;
       synEscape = True ;
     }
       
    /* check for caps lock... */
    for (i = 0; i < tpd->numCapsLockKeyCodes; i++) {
	if (tpd->capsLockKeyCodes[i] == keyEvent->keycode) {
	    /* caps lock has toggled... */
	    (void) CapsLockUpdate(w, !tpd->capsLock);
	    break;
	}
    }

    /* I've pulled the following from hpterm2.0's input.c.  I don't
     * understand any of it, and we need to get some documentation on
     * it.
     */

    nbytes = XmImMbLookupString(w, keyEvent, (char *) string, sizeof(string),
	    &keysym, &status);
    if (XBufferOverflow == status) {
	nbytes = 0;
    }

#ifdef	XOR_CAPS_LOCK
    /* if lock and shift are both pressed, let's invert the case of all
     * upper and lower case characters...
     */
    if ((keyEvent->state & (LockMask | ShiftMask)) == (LockMask | ShiftMask)) {
	for (i = 0; i < nbytes; i++) {
	    if (isupper(string[i])) {
		string[i] = _tolower(string[i]);
	    } else if (islower(string[i])) {
		string[i] = _toupper(string[i]);
	    }
	}
    }
#endif	/* XOR_CAPS_LOCK */

        
    if ((nbytes > 0) && (tw->term.inputVerifyCallback)) {
	DtTermInputVerifyCallbackStruct cb;

	cb.reason = DtCR_TERM_INPUT_VERIFY;
	cb.event = event;
	cb.doit = True;
	cb.text = string;
	cb.length = nbytes;

	/* invoke the callbacks... */
	(void) XtCallCallbackList(w, tw->term.inputVerifyCallback, &cb);

	/* if doit was turned off, forget about the string... */
	if (!cb.doit) {
	    nbytes = 0;
	}
    }

    if (nbytes > 0) {
	unsigned char *start;
	unsigned char *end;

	/* perform margin bell functionality if necessary... */
	if (tw->term.marginBell &&
		((tw->term.columns - tw->term.nMarginBell) ==
		    tpd->cursorColumn)) {
	    _DtTermPrimBell(w);
	}
		
        /* synthesize escape unless it was CR or Vertical Tab */
        if (synEscape && *string != '\r' && *string != 0x0B) 
            (void) _DtTermPrimSendInput(w, (unsigned char *) "\033", 1);
            
        /* for pointer blanking                               */
        if (tw->term.pointerBlank && *string != '\r' && *string != 0x0B) 
            _DtTermPrimPointerOff((Widget)tw,(XtIntervalId *)NULL) ;

	for (end = string; nbytes > 0; )  {
	    for (start = end; (nbytes > 0) && (*end != '\r'); nbytes--, end++)
		;
	    if ((nbytes > 0) && (*end == '\r')) {
		(void) end++;
		(void) nbytes--;
	    }
	    (void) _DtTermPrimSendInput(w, start, end - start);
	    if (tpd->autoLineFeed && (end[-1] == '\r')) {
		(void) _DtTermPrimSendInput(w, (unsigned char *) "\n", 1);
	    }
	}
    }

    Debug('i', fprintf(stderr, ">>_DtTermPrimActionKeyInput() finished\n"));
}

void
_DtTermPrimSendInput
(
    Widget		  w,
    unsigned char	 *buffer,
    int			  length
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData	  tpd = tw->term.tpd;

    if (length > 0) {
        (void) _DtTermPrimPendingTextAppend(tpd->pendingWrite, buffer, length);
        (void) _DtTermPrimStartOrStopPtyOutput(w);

	if (tpd->halfDuplex) {
	    DtTermDisplaySend(w, buffer, length);
	}
    }
}

void
DtTermSubprocSend
(
    Widget		  w,
    unsigned char	 *buffer,
    int			  length
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData	  tpd = tw->term.tpd;
    _DtTermWidgetToAppContext(w);

    /* queue up all text send from outside the widget... */
    _DtTermAppLock(app);
    if (length > 0) {
	(void) _DtTermPrimPendingTextAppend(tpd->pendingWrite, buffer, length);
	(void) _DtTermPrimStartOrStopPtyOutput(w);
	if (tpd->halfDuplex) {
	    DtTermDisplaySend(w, buffer, length);
	}
    }
    _DtTermAppUnlock(app);
}

/*ARGSUSED*/
void
_DtTermPrimActionFocusIn(Widget w, XEvent *event, String *params,
	Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    Debug('F', fprintf(stderr, ">>focusIn starting\n"));
#ifdef	NOTDEF
    Debug('F', fprintf(stderr,
	    ">>  widget: name=\"%s\"  widget=0x%lx  window=0x%lx\n",
	    tw->core.name, tw, XtWindow(w)));
    Debug('F', fprintf(stderr,
	    ">>  event: window=0x%lx\n  mode=%d  detail=%d\n",
	    event->xfocus.window, event->xfocus.mode, event->xfocus.detail));
#endif	/* NOTDEF */
    Debug('F', _DtTermPrimDebugDumpEvent(stderr, w, event));

    if (
#ifdef	MOTIF_TEXT_BUG
	    event->xfocus.send_event &&
#endif	/* MOTIF_TEXT_BUG */
	    !(tw->term.hasFocus)) {
	tw->term.hasFocus = True;
	_DtTermPrimCursorChangeFocus(w);
	if (_XmGetFocusPolicy(w) == XmEXPLICIT) {
	    if (((XmPrimitiveWidgetClass) XtClass(w))
		    ->primitive_class.border_highlight) {
		(void) (*((XmPrimitiveWidgetClass) XtClass(w))
			->primitive_class.border_highlight)(w);
	    }
	}
    }
    (void) _XmPrimitiveFocusIn(w, event, params, num_params);
}

/*ARGSUSED*/
void
_DtTermPrimActionFocusOut(Widget w, XEvent *event, String *params,
	Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    Debug('F', fprintf(stderr, ">>focusOut starting\n"));
#ifdef	NOTDEF
    Debug('F', fprintf(stderr,
	    ">>  widget: name=\"%s\"  widget=0x%lx  window=0x%lx\n",
	    tw->core.name, tw, XtWindow(w)));
    Debug('F', fprintf(stderr,
	    ">>  event: window=0x%lx\n  mode=%d  detail=%d\n",
	    event->xfocus.window, event->xfocus.mode, event->xfocus.detail));
#endif	/* NOTDEF */
    Debug('F', _DtTermPrimDebugDumpEvent(stderr, w, event));

    if (
#ifdef	MOTIF_TEXT_BUG
	    event->xfocus.send_event &&
#endif	/* MOTIF_TEXT_BUG */
	    tw->term.hasFocus) {
	tw->term.hasFocus = False;
	_DtTermPrimCursorChangeFocus(w);
	if (_XmGetFocusPolicy(w) == XmEXPLICIT) {
	    if (((XmPrimitiveWidgetClass) XtClass(w))
		    ->primitive_class.border_unhighlight) {
		(void) (*((XmPrimitiveWidgetClass) XtClass(w))
			->primitive_class.border_unhighlight)(w);
	    }
	}
    }
    (void) _XmPrimitiveFocusOut(w, event, params, num_params);
}

static void
KeyTranslator
(
    Display		 *display,
    KeyCode		  keyCode,
    Modifiers		  modifiers,
    Modifiers		 *modifiersReturn,
    KeySym		 *keysymReturn
)
{
    /* call the Xt translator to translate this event... */
    (void) XtTranslateKey(display, keyCode, modifiers, modifiersReturn,
	    keysymReturn);
    /* and reinstall the Motif translator for the next widget/event... */
    (void) XtSetKeyTranslator(display, (XtKeyProc) XmTranslateKey);
}
    
/*ARGSUSED*/
static void
handleKeyEvents(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    /* drop synthetic events... */
    if (!tw->term.allowSendEvents && event->xany.send_event) {
	*cont = False;
	return;
    }

#ifdef	KEY_TRANSLATE_HACK
    /* install our key translator... */
    (void) XtSetKeyTranslator(XtDisplay(w), (XtKeyProc) KeyTranslator);
#endif	/* KEY_TRANSLATE_HACK */
    /* and return... */
}

/*ARGSUSED*/
static void
handleButtonEvents(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    /* drop synthetic events... */
    if (!tw->term.allowSendEvents && event->xany.send_event) {
	*cont = False;
	return;
    }
}

static void
CapsLockUpdate(Widget w, Boolean capsLock)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    if (tpd->capsLock != capsLock) {
	tpd->capsLock = capsLock;

	/* invoke the caps lock callback... */
	if (tw->term.statusChangeCallback) {
	    _DtTermPrimInvokeStatusChangeCallback(w);
	}
    }
}

void
_DtTermPrimInsertCharUpdate(Widget w, DtTermInsertCharMode insertCharMode)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    if (tpd->insertCharMode != insertCharMode) {
	tpd->insertCharMode = insertCharMode;

	/* invoke the caps lock callback... */
	if (tw->term.statusChangeCallback) {
	    _DtTermPrimInvokeStatusChangeCallback(w);
	}
    }
}

/*ARGSUSED*/
void
_DtTermPrimActionStop(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;

    if (*num_params > 0) {
	if (!strcmp(params[0], "on")) {
	    tpd->outputStopped = False;
	} else if (!strcmp(params[0], "off")) {
	    tpd->outputStopped = True;
	} else {
	    /* toggle the output... */
	    tpd->outputStopped = !tpd->outputStopped;
	}
    } else {
	/* toggle the output... */
	tpd->outputStopped = !tpd->outputStopped;
    }

    (void) _DtTermPrimStartOrStopPtyInput(w);

    /* invoke the status update callback... */
    if (tw->term.statusChangeCallback) {
	_DtTermPrimInvokeStatusChangeCallback(w);
    }
}

void
_DtTermPrimInvokeStatusChangeCallback
(
    Widget			  w
)
{
    DtTermPrimitiveWidget	  tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData		  tpd = tw->term.tpd;
    DtTermStatusChangeCallbackStruct
				  cb;

    /* exit now if we don't need to do anything... */
    if (!tw->term.statusChangeCallback)
	return;

    (void) memset(&cb, '\0', sizeof(cb));
    cb.reason = DtCR_TERM_STATUS_CHANGE;
    cb.event = (XEvent *) 0;
    cb.cursorX = tpd->cursorColumn + 1;
    cb.cursorY = tpd->topRow + tpd->cursorRow + 1;
    cb.capsLock = tpd->capsLock;
    cb.stop = tpd->outputStopped;
    cb.insertCharMode = tpd->insertCharMode;
    cb.locked = tpd->keyboardLocked.escape;

    /* invoke them... */
    (void) XtCallCallbackList(w, tw->term.statusChangeCallback,
	    (XtPointer) &cb);
}

void
_DtTermPrimDrawShadow(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;

    if (XtIsRealized(w)) {
	XmeDrawShadows(XtDisplay(w), XtWindow(w),
		tw->primitive.top_shadow_GC,
		tw->primitive.bottom_shadow_GC,
		tw->primitive.highlight_thickness,
		tw->primitive.highlight_thickness,
		tw->core.width - 2 * tw->primitive.highlight_thickness,
		tw->core.height - 2 * tw->primitive.highlight_thickness,
		tw->primitive.shadow_thickness,
		tw->term.shadowType);
    }
}

Widget
DtCreateTermPrimitive(Widget parent, char *name, ArgList arglist,
	Cardinal argcount)
{
    Widget w;
    _DtTermWidgetToAppContext(parent);

    _DtTermAppLock(app);
    Debug('T', timeStamp("DtCreateTermPrimitive() starting"));
    w = XtCreateWidget(name, dtTermPrimitiveWidgetClass, parent, arglist,
	    argcount);
    Debug('T', timeStamp("DtCreateTermPrimitive() finished"));
    _DtTermAppUnlock(app);
    return(w);
}

void
_DtTermPrimStartOrStopPtyInput(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermPrimData tpd = tw->term.tpd;
    Boolean inputOn = True;

    /* This function will either turn on or turn off the pty input selector
     * depending on the status of:
     *
     *	    - the outputStopped flag,
     *	    - any pending scrolling operations.
     */

    Debug('o', fprintf(stderr, ">>_StartOrStopPtyInput() starting\n"));
    /* turn off input if there is a non-jump scroll pending... */
    if (!tw->term.jumpScroll && tpd->scroll.nojump.pendingScroll)
	inputOn = False;

    /* turn off input if we are stopped or paused... */
    if (tpd->outputStopped || tpd->oneSecondPause)
	inputOn = False;

    if (inputOn && !tpd->ptyInputId && (tw->term.pty >= 0)) {
	/* turn it on... */
	tpd->ptyInputId = 
		XtAppAddInput(XtWidgetToApplicationContext((Widget) tw),
		tw->term.pty, (XtPointer) XtInputReadMask, readPty,
                (Widget) tw);
        Debug('o', fprintf(stderr, "    adding pty read select\n"));
    } else if (!inputOn && tpd->ptyInputId) {
	/* turn it off... */
	(void) XtRemoveInput(tw->term.tpd->ptyInputId);
	tw->term.tpd->ptyInputId = (XtInputId) 0;
        Debug('o', fprintf(stderr, "    removing pty read select\n"));
    }
    Debug('o', fprintf(stderr, ">>_StartOrStopPtyInput() finished\n"));
}

void
_DtTermPrimStartOrStopPtyOutput(Widget w)
{
    DtTermPrimitiveWidget          tw      = (DtTermPrimitiveWidget) w;
    DtTermPrimData		tpd      = tw->term.tpd;

    Debug('o', fprintf(stderr, ">>_StartOrStopPtyOutput() starting\n"));
    /*
    ** This function will either turn on or turn off the 
    ** pty write selector depending whether any text is waiting
    ** to be written.
    */
    if (TextIsPending(tpd->pendingWrite))
    {   
        if (tpd->ptyOutputId == 0)
        {
            /*
            ** turn it on...
            */
            tpd->ptyOutputId = XtAppAddInput(XtWidgetToApplicationContext(w),
                                            tw->term.pty, 
                                            (XtPointer) XtInputWriteMask,
                                            writePty, w);

            Debug('o', fprintf(stderr, "    adding pty write select\n"));
        }
    } 
    else if (tpd->ptyOutputId != 0)
    {
        /*
        ** turn it off...
        */
        (void) XtRemoveInput(tw->term.tpd->ptyOutputId);
        tw->term.tpd->ptyOutputId = (XtInputId) 0;
        Debug('o', fprintf(stderr, "    removing pty write select\n"));
    }
    Debug('o', fprintf(stderr, ">>_StartOrStopPtyOutput() finished\n"));
}

/*ARGSUSED*/
static void VerticalScrollBarCallback(Widget wid, XtPointer client_data,
	XtPointer call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    XmScrollBarCallbackStruct *cb = (XmScrollBarCallbackStruct *) call_data;

    _DtTermPrimCursorOff((Widget) tw);

    (void) _DtTermPrimScrollTextTo((Widget) tw, cb->value -
	    (tw->term.tpd->useHistoryBuffer ?
	    tw->term.tpd->lastUsedHistoryRow : 0));
    /* we need to complete the scroll or it won't happen... */
    (void) _DtTermPrimScrollComplete((Widget) tw, True);
    if (cb->reason != XmCR_DRAG) {
	_DtTermPrimCursorOn((Widget) tw);
    }
}

/*ARGSUSED*/
static void WarningDialogOkCallback(Widget wid, XtPointer client_data,
	XtPointer call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    DtTermPrimData tpd = tw->term.tpd;

    tpd->warningDialogMapped = False;
}

/*ARGSUSED*/
static void WarningDialogWMDestroyCallback(Widget wid, XtPointer client_data,
	XtPointer call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    DtTermPrimData tpd = tw->term.tpd;

    /* unmap the window (this is necessary for wm destroy callbacks)... */
    (void) XtUnmanageChild(tpd->warningDialog);
    /* call the OK callback... */
    (void) WarningDialogOkCallback(tpd->warningDialog, client_data,
	    call_data);
}

void _DtTermPrimWarningDialog(Widget w, char *msg)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    Widget parent;
    DtTermPrimData tpd = tw->term.tpd;
    XmString msgString;
    XmString titleString;
    Arg arglist[10];
    int i;

    if (!tpd->warningDialog) {
	i = 0;

	(void) XtSetArg(arglist[i], XmNdialogStyle,
		XmDIALOG_PRIMARY_APPLICATION_MODAL); i++;
	titleString =
	  XmStringGenerate(
		GETMESSAGE(NL_SETN_Prim,2,"Terminal - Warning"),
		XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
	(void) XtSetArg(arglist[i], XmNdialogTitle, titleString); i++;
	tpd->warningDialog = XmCreateWarningDialog(w, "termWarning", arglist, i);
	(void) XmStringFree(titleString);

	(void) XtAddCallback(tpd->warningDialog,
		XmNokCallback, WarningDialogOkCallback, (XtPointer) w);
	/* find the shell widget so we can add a wmDestroy callback... */
	/*EMPTY*/
	for (parent = tpd->warningDialog; !XtIsShell(parent);
		parent = XtParent(parent))
	    ;
	(void) _DtTermPrimAddDeleteWindowCallback(parent,
		WarningDialogWMDestroyCallback, (XtPointer) w);
    }

    i = 0;
    msgString =
      XmStringGenerate(msg, XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, NULL);
    (void) XtSetArg(arglist[i], XmNmessageString, msgString); i++;
    (void) XtSetValues(tpd->warningDialog, arglist, i);
    (void) XmStringFree(msgString);

    if (!XtIsRealized(tpd->warningDialog)) {
	(void) XtRealizeWidget(tpd->warningDialog);
	(void) XtUnmanageChild(XmMessageBoxGetChild(tpd->warningDialog,
		XmDIALOG_CANCEL_BUTTON));
	(void) XtUnmanageChild(XmMessageBoxGetChild(tpd->warningDialog,
		XmDIALOG_HELP_BUTTON));
    }

    /* limit wm functions to move, decorations to menu, border and title...
     */
    i = 0;
    (void) XtSetArg(arglist[i], XmNmwmFunctions, MWM_FUNC_MOVE); i++;
    (void) XtSetArg(arglist[i], XmNmwmDecorations,
	    MWM_DECOR_MENU | MWM_DECOR_BORDER | MWM_DECOR_TITLE); i++;
    (void) XtSetValues(XtParent(tpd->warningDialog), arglist, i);

    if (tpd->windowMapped) {
	(void) XtManageChild(tpd->warningDialog);
	tpd->warningDialogMapped = True;
	tpd->mapWarningDialog = False;
    } else {
	tpd->mapWarningDialog = True;
    }
}

typedef void _ManagerConstraintInitializeProc(Widget request, Widget new_w,
	ArgList args, Cardinal *num_args);
static _ManagerConstraintInitializeProc *managerConstraintInitializeProc;

static void
ManagerConstraintInitializePatch(Widget request, Widget new_w, ArgList args,
	Cardinal *num_args)
{
    if (!XtIsSubclass(new_w, dtTermPrimitiveWidgetClass)) {
	(*managerConstraintInitializeProc)(request, new_w, args, num_args);
    }
}

void
DtTermInitialize()
{
    XmManagerClassRec *mw = (XmManagerClassRec *) xmManagerWidgetClass;
    CoreClassRec *core = (CoreClassRec *) coreWidgetClass;
    static Boolean initted = False;

    /* only do this once... */
    _DtTermProcessLock();
    if (initted) {
        _DtTermProcessUnlock();
	return;
    }

    if (core->core_class.class_inited) {
	(void) XtWarning("DtTermInitialize was not called before toolkit initialization\n");
    }
    managerConstraintInitializeProc = mw->constraint_class.initialize;
    mw->constraint_class.initialize = ManagerConstraintInitializePatch;
    initted = True;
    _DtTermProcessUnlock();
}

void
_DtTermPrimPutEnv(char *c1, char *c2)
{
    char buffer[BUFSIZ];
    char *c;

    (void) strcpy(buffer, c1);
    (void) strcat(buffer, c2);
    c = XtMalloc(strlen(buffer) + 1);
    (void) strcpy(c, buffer);
    (void) putenv(c);
}


#if	!defined(NL_CAT_LOCALE)
#define	NL_CAT_LOCALE 0
#endif	/* NL_CAT_LOCAL */

char *
_DtTermPrimGetMessage(
        char *filename,
        int set,
        int n,
        char *s )
{
        char *msg;
        char *lang;
        static int first = 1;
        static nl_catd nlmsg_fd;

	_DtTermProcessLock();
        if ( first )
        {
                first = 0;

                lang = (char *) getenv ("LANG");

                if (!lang || !(strcmp (lang, "C")))
                        /*
                         * If LANG is not set or if LANG=C, then there
                         * is no need to open the message catalog - just
                         * return the built-in string "s".
                         */
                        nlmsg_fd = (nl_catd) -1;
                else
                        nlmsg_fd = catopen(filename, NL_CAT_LOCALE);
        }
	_DtTermProcessUnlock();
        msg=catgets(nlmsg_fd,set,n,s);
        return (msg);
}



static int
PreeditStart(
    XIC xic,
    XPointer client_data,
    XPointer call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;

    PreLen(tw) = 0L;
    PreRow(tw) = tw->term.tpd->cursorRow;
    PreColumn(tw) = tw->term.tpd->cursorColumn;

    /* vertical writing dependency here */
    PreStart(tw) = tw->term.tpd->cursorColumn;
    PreEnd(tw) = tw->term.tpd->cursorColumn;

    PreUnder(tw) = True;
    return(-1);
}


static void
PreeditDone(
    XIC xic,
    XPointer client_data,
    XPointer call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;

    _DtTermPrimRefreshText((Widget)tw, PreColumn(tw), PreRow(tw),
			   tw->term.columns,
			   PreRow(tw));
    PreUnder(tw) = False;
    PreLen(tw) = 0L;
}


static void
PreeditDelete(
    DtTermPrimitiveWidget tw,
    XIMPreeditDrawCallbackStruct *call_data)
{
    short retcount, row, col, width;
    termChar *retchar;

    /*
    ** chg_first to chg_length in the preedit call_data 
    ** structure indicates what should be deleted out of 
    ** the preedit buffer, but this is terms of characters 
    ** not bytes. We have stored the byte value in the 
    ** term data structure, so we use that instead.
    */
    if (call_data->chg_length && PreLen(tw)) {
	row = PreRow(tw);
        col = PreColumn(tw) + call_data->chg_first;
        width = call_data->chg_length;
	_DtTermPrimBufferDelete(tw->term.tpd->termBuffer, &row, &col,
				&width, &retchar, &retcount);

	/*
	 ** We may want to consider freeing retcount @ retchar
	 */
	
	/*
	 ** Refresh the text buffer -
	 ** We must refresh to the rest of the line, because the
	 ** preedit buffer may be in the middle of a line.
	 */
	_DtTermPrimRefreshText((Widget)tw, PreColumn(tw), PreRow(tw),
			       tw->term.columns,
			       PreRow(tw));
    }
}


static void
PreeditHighlight(
    DtTermPrimitiveWidget tw,
    XIMPreeditDrawCallbackStruct *call_data)
{
    TermSelectInfo  selectInfo = tw->term.tpd->selectInfo;

    /* check for null feedback */
    if (call_data->text->feedback) {
	switch (*call_data->text->feedback) {
	  case XIMReverse:
	  case XIMUnderline:
	  case XIMHighlight:
	  case XIMPrimary:
	  case XIMSecondary:
	  case XIMTertiary:
	    /* clear any existing highlight first */
	    selectInfo->ownPrimary = False;
	    _DtTermPrimRenderRefreshTextLinear((Widget)tw, 
					       selectInfo->begin,
					       selectInfo->end - 1);
	    selectInfo->ownPrimary =True;
	    selectInfo->begin = rowColToPos(tw, PreRow(tw), PreColumn(tw));
	    selectInfo->end = selectInfo->begin + PreLen(tw);
	    _DtTermPrimRenderRefreshTextLinear((Widget)tw, 
					       selectInfo->begin,
					       selectInfo->end - 1);
	    break;
	  default:
	    /* no highlight set, clear */
	    selectInfo->ownPrimary = False;
	    _DtTermPrimRenderRefreshTextLinear((Widget)tw, 
					       selectInfo->begin,
					       selectInfo->end - 1);
	}
    }
}


static void
PreeditDraw(
    XIC xic,
    XPointer client_data,
    XIMPreeditDrawCallbackStruct *call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    DtTermInsertCharMode savmode;
    unsigned char *mb;
    wchar_t *wcs;
    size_t len;

    /* are we under preedit? */
    if (!PreUnder(tw))
	return;

    /*
    ** This indicates a preedit buffer deletion
    */
    if (!call_data->text) {
	PreeditHighlight(tw, call_data);
	PreeditDelete(tw, call_data);

	/* reset row/column position to beginning */
	tw->term.tpd->cursorRow = PreRow(tw);
	tw->term.tpd->cursorColumn = PreColumn(tw);
	_DtTermPrimCursorUpdate((Widget)tw);
	_DtTermPrimCursorOn((Widget)tw);
	return;
    }

    /*
    ** At this point, we know there is a string in the
    ** preedit buffer that we must render.
    */

    /* get preedit string */
    if (call_data->text->encoding_is_wchar) {

	/* preedit buffer is wchar, we must convert */
	wcs = call_data->text->string.wide_char;
	len = wcslen(wcs) * sizeof(wchar_t);
	mb = (unsigned char *)XtMalloc(len);
	
	/* check for invalid string */
	if (wcstombs((char *)mb, wcs, len) == -1)
	    return;
    } else
	mb = (unsigned char *)call_data->text->string.multi_byte;


    /*
    ** First we must destroy the previous contents of
    ** the preedit buffer, if any, before we redraw 
    ** the new one.
    */
    PreeditDelete(tw, call_data);

    /* set insertion point */
    tw->term.tpd->cursorRow = PreRow(tw);
    tw->term.tpd->cursorColumn = PreColumn(tw);

    /* render buffer */
    savmode = tw->term.tpd->insertCharMode;
    tw->term.tpd->insertCharMode = DtTERM_INSERT_CHAR_ON;
    len = strlen((char *)mb);
    _DtTermPrimInsertText((Widget)tw, mb, len);
    PreLen(tw) = len;
    tw->term.tpd->insertCharMode = savmode;

    /* check highlight */
    PreeditHighlight(tw, call_data);

    /* update cursor */
    _DtTermPrimCursorUpdate((Widget)tw);
    _DtTermPrimCursorOn((Widget)tw);
}


static void
PreeditCaret(
    XIC xic,
    XPointer client_data,
    XIMPreeditCaretCallbackStruct *call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    TermSelectInfo  selectInfo = tw->term.tpd->selectInfo;
    Boolean chgcursor = False;
    short newpos = 0;

    switch (call_data->style) {
      case XIMIsInvisible:
	_DtTermPrimCursorOff((Widget)tw);
	break;
      case XIMIsPrimary:
      case XIMIsSecondary:
	_DtTermPrimCursorOn((Widget)tw);
	break;
      default:
	break;
    }

    switch (call_data->direction) {
      case XIMForwardChar:
	newpos = tw->term.tpd->cursorColumn + 1;
	chgcursor = True;
	break;
      case XIMBackwardChar:
	newpos = tw->term.tpd->cursorColumn - 1;
	chgcursor = True;
	break;
      case XIMLineStart:
	newpos = PreStart(tw);
	chgcursor = True;
	break;
      case XIMLineEnd:
	newpos = PreEnd(tw);
	chgcursor = True;
	break;
      case XIMAbsolutePosition:
	newpos = PreColumn(tw) + call_data->position;
	chgcursor = True;
	break;

      /* do nothing we these movements */
      case XIMForwardWord:
      case XIMBackwardWord:
      case XIMCaretUp:
      case XIMCaretDown:
      case XIMNextLine:
      case XIMPreviousLine:
      case XIMDontChange:
	break;
      default:
	break;	/* NOTREACHED */
    }

    /*
     ** The input method shouldn't let us edit outside
     ** of the preedit buffer anyway, but we check just
     ** to be sure.
     */
    if (chgcursor && (newpos >= PreStart(tw)) && (newpos <= PreEnd(tw))) {
	tw->term.tpd->cursorColumn = newpos;
	_DtTermPrimCursorUpdate((Widget)tw);
	_DtTermPrimCursorOn((Widget)tw);
	/*
	** refresh highlight (if any) because cursor 
	** movement hoses it up
	*/
	_DtTermPrimRenderRefreshTextLinear((Widget)tw, 
					   selectInfo->begin,
					   selectInfo->end - 1);
    }
}

static void 
setThickness(
        Widget widget,
        int offset,
        XrmValue *value )
{
      XmDisplay        xmDisplay;
      static Dimension thickness;

      xmDisplay = (XmDisplay)XmGetXmDisplay(XtDisplay(widget));

      if (xmDisplay->display.enable_thin_thickness) {
              thickness = 1;
      }
      else {
              thickness = 2;
      }

      value->addr = (XPointer)&thickness;
}
