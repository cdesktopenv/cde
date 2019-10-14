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
/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

extern char _DtTermPullInTermWhatString[];
static char *termWhatString = _DtTermPullInTermWhatString;

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermP.h"
#include "TermPrimAction.h"
#include "TermData.h"
#include "TermBufferP.h"
#include "TermAction.h"
#include "TermParseTable.h"
#include "TermPrimLineFontP.h"
#include "TermLineData.h"
#include "TermColor.h"
#include "TermFunctionKey.h"
#include "TermPrimRenderLineDraw.h"
#include "TermPrimMessageCatI.h"
#include "TermPrimSelectP.h"
#include <nl_types.h>


static int TextInsertProc(Widget w, unsigned char *buffer, int length);
static void Resize(Widget w);
static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass wc);
static void Initialize(Widget rw, Widget nw, Arg *args, Cardinal *num_args);
static Boolean SetValues(Widget cw, Widget rw, Widget nw, ArgList args,
	Cardinal *num_args);
static void Realize(Widget w, XtValueMask *p_valueMask,
	XSetWindowAttributes *attributes);
static void Destroy(Widget w);
static void InitializeVerticalScrollBar(Widget w, Boolean initCallbacks);
static void VerticalScrollBarCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void StatusChangeNotify(Widget w, unsigned long mask);
static void PositionUpdate(Widget w, short row, short column);
static void GetAutoWrap( 
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;
static XmImportOperator SetAutoWrap( 
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;
static void GetReverseWrap( 
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;
static XmImportOperator SetReverseWrap( 
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;
static void GetAppCursorMode(
		        Widget wid,
		        int offset,
		        XtArgVal *value );
static XmImportOperator SetAppCursorMode(
		        Widget wid,
		        int offset,
		        XtArgVal *value );
static void GetAppKeypadMode(
		        Widget wid,
		        int offset,
		        XtArgVal *value );
static XmImportOperator SetAppKeypadMode(
		        Widget wid,
		        int offset,
		        XtArgVal *value );



/* action list for class: Term... */

static XtActionsRec actionsList[] = {
    {"beginning-of-buffer",	_DtTermActionBeginningOfBuffer},
    {"vt-break",		_DtTermActionBreak},
    {"end-of-buffer",		_DtTermActionEndOfBuffer},
    {"vt-function-key-execute",	_DtTermActionFunctionKeyExecute},
    {"hard-reset",		_DtTermActionHardReset},
    {"insert-line",		_DtTermActionInsertLine},
    {"keypad-key-execute",	_DtTermActionKeypadKeyExecute},
    {"move-cursor",		_DtTermActionMoveCursor},
    {"scroll",			_DtTermActionScroll},
    {"soft-reset",		_DtTermActionSoftReset},
    {"tab",			_DtTermActionTab},
    {"vt-edit-key",        	_DtTermActionEditKeyExecute},
    {"process-press",      	_DtTermPrimSelect2ButtonMouse},
};

/* the resource list for Term... */
static XtResource resources[] =
{
    {
	DtNsunFunctionKeys, DtCSunFunctionKeys, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.sunFunctionKeys),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNautoWrap, DtCAutoWrap, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.autoWrap),
	XtRImmediate, (XtPointer) True 
    },
    {
	DtNreverseWrap, DtCReverseWrap, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.reverseWrap),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNemulationId, DtCEmulationId, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.emulationId),
	XtRImmediate, (XtPointer) "DtTermWidget"
    },
    {
	DtNtermId, DtCTermId, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.termId),
	XtRImmediate, (XtPointer) "vt220"
    },
    {
	DtNtermName, DtCTermName, XmRString, sizeof(char *),
	XtOffsetOf( struct _DtTermPrimitiveRec, term.termName),
	XtRImmediate, (XtPointer) "dtterm"
    },
    {
	DtNc132, DtCC132, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.c132),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNappCursorDefault, DtCAppCursorDefault, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.appCursorMode),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNappKeypadDefault, DtCAppKeypadDefault, XmRBoolean, sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.appKeypadMode),
	XtRImmediate, (XtPointer) False
    },
#ifdef	DKS
    {
	DtNstickyPrevCursor, DtCStickyCursor, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.stickyPrevCursor),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNstickyNextCursor, DtCStickyCursor, XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(struct _DtTermRec, vt.stickyNextCursor),
	XtRImmediate, (XtPointer) False
    },
    {
	DtNstatusChangeCallback, DtCCallback, XmRCallback,
	sizeof(XtCallbackList),
	XtOffsetOf( struct _DtTermRec, vt.statusChangeCallback),
	XmRPointer, (XtPointer) NULL
    },
    {
	DtNverticalScrollBar, DtCVerticalScrollBar, XmRWidget, sizeof(Widget),
	XtOffsetOf( struct _DtTermRec, vt.verticalScrollBar),
	XmRImmediate, (XtPointer) NULL
    },
#endif	/* DKS */
};


/****************
 *
 * Synthetic resources for those ugly wart variables...
 *
 ****************/
static XmSyntheticResource GetResources[] =
{
   { DtNautoWrap,
     sizeof (Boolean),
     XtOffsetOf(struct _DtTermRec, vt.autoWrap),
     GetAutoWrap,
     SetAutoWrap },
   { DtNreverseWrap,
     sizeof (Boolean),
     XtOffsetOf(struct _DtTermRec, vt.reverseWrap),
     GetReverseWrap,
     SetReverseWrap },
   { DtNappCursorDefault,
     sizeof (Boolean),
     XtOffsetOf(struct _DtTermRec, vt.appCursorMode),
     GetAppCursorMode,
     SetAppCursorMode },
   { DtNappKeypadDefault,
     sizeof (Boolean),
     XtOffsetOf(struct _DtTermRec, vt.appKeypadMode),
     GetAppKeypadMode,
     SetAppKeypadMode },
    
};

/*
 * Goofy debug'ed code by HP.
 * IBM's JP kbd accepts Shift+KP_Multiply as a valid key sequence.
 * I don't know why HP added non-exposed(internal) Xt's action ???
 * Anyway, remove KP_Multiply entry from defaultTranslations[]
 */
static char defaultTranslations[] = "\
        :<Key>osfCancel:         process-cancel()\n\
	:<Key>osfCopy:		copy-clipboard()\n\
	:<Key>osfCut:		copy-clipboard()\n\
	:<Key>osfPaste:		paste-clipboard()\n\
        :<Key>osfBeginLine:      beginning-of-buffer()\n\
        <Key>Home:              beginning-of-buffer()\n\
        :<Key>osfEndLine:        end-of-buffer()\n\
        <Key>End:               end-of-buffer()\n\
	Shift<Key>osfUp:	scroll(1,line)\n\
	Shift<Key>Up:	        scroll(1,line)\n\
	Shift<Key>osfDown:	scroll(-1,line)\n\
	Shift<Key>Down:	        scroll(-1,line)\n\
	:<Key>osfUp:		move-cursor(up)\n\
	<Key>Up:		move-cursor(up)\n\
	:<Key>osfDown:		move-cursor(down)\n\
	<Key>Down:		move-cursor(down)\n\
	:<Key>osfLeft:		move-cursor(backward)\n\
	<Key>Left:		move-cursor(backward)\n\
	:<Key>osfRight:		move-cursor(forward)\n\
	<Key>Right:		move-cursor(forward)\n\
	<Key>Find:	        vt-edit-key(find)\n\
	:<Key>osfInsert:         vt-edit-key(insert)\n\
	<Key>Insert:            vt-edit-key(insert)\n\
	:<Key>osfDelete:         vt-edit-key(delete)\n\
	:<Key>Delete:            vt-edit-key(delete)\n\
	:<Key>osfSelect:	        vt-edit-key(select)\n\
	<Key>Select:	        vt-edit-key(select)\n\
	:~Shift<Key>osfPageUp:	vt-edit-key(prior)\n\
	~Shift<Key>Prior:	vt-edit-key(prior)\n\
	:~Shift<Key>osfPageDown:	vt-edit-key(next)\n\
	~Shift<Key>Next:	vt-edit-key(next)\n\
	<Key>Help:		vt-edit-key(help)\n\
	<Key>Menu:		vt-edit-key(do)\n\
	:<Key>osfPageUp:		scroll(-1,page)\n\
	<Key>Prior:	        scroll(-1,page)\n\
	:<Key>osfPageDown:	scroll(1,page)\n\
	<Key>Next:	        scroll(1,page)\n\
	Mod1<Key>Break:		soft-reset()\n\
	Shift<Key>Break:	hard-reset()\n\
	~Shift ~Mod1<Key>Break:	vt-break(long)\n\
	Ctrl<Key>Cancel:	stop(long)\n\
	~Ctrl<Key>Cancel:	stop()\n\
	~Shift<Key>Tab:		tab()\n\
        <Key>osfActivate:	keypad-key-execute(enter)\n\
        <Key>KP_Space:		keypad-key-execute(space)\n\
	<Key>KP_Tab:		keypad-key-execute(tab)\n\
        <Key>KP_Enter:		keypad-key-execute(enter)\n\
        <Key>KP_F1:		keypad-key-execute(f1)\n\
        <Key>KP_F2:		keypad-key-execute(f2)\n\
        <Key>KP_F3:		keypad-key-execute(f3)\n\
        <Key>KP_F4:		keypad-key-execute(f4)\n\
        <Key>KP_Equal:		keypad-key-execute(equal)\n\
        <Key>KP_Multiply:	keypad-key-execute(multiply)\n\
        <Key>KP_Add:		keypad-key-execute(add)\n\
        <Key>KP_Separator:	keypad-key-execute(separator)\n\
        <Key>KP_Subtract:	keypad-key-execute(subtract)\n\
        <Key>KP_Decimal:	keypad-key-execute(decimal)\n\
        <Key>KP_Divide:		keypad-key-execute(divide)\n\
	!Mod1<Key>KP_0:		insert()\n\
	!Mod1<Key>KP_1:		insert()\n\
	!Mod1<Key>KP_2:		insert()\n\
	!Mod1<Key>KP_3:		insert()\n\
	!Mod1<Key>KP_4:		insert()\n\
	!Mod1<Key>KP_5:		insert()\n\
	!Mod1<Key>KP_6:		insert()\n\
	!Mod1<Key>KP_7:		insert()\n\
	!Mod1<Key>KP_8:		insert()\n\
	!Mod1<Key>KP_9:		insert()\n\
        <Key>KP_0:		keypad-key-execute(0)\n\
        <Key>KP_1:		keypad-key-execute(1)\n\
        <Key>KP_2:		keypad-key-execute(2)\n\
        <Key>KP_3:		keypad-key-execute(3)\n\
        <Key>KP_4:		keypad-key-execute(4)\n\
        <Key>KP_5:		keypad-key-execute(5)\n\
        <Key>KP_6:		keypad-key-execute(6)\n\
        <Key>KP_7:		keypad-key-execute(7)\n\
        <Key>KP_8:		keypad-key-execute(8)\n\
        <Key>KP_9:		keypad-key-execute(9)\n\
	Shift<Key>F1:		vt-function-key-execute(1, UDK)\n\
	Shift<Key>F2:		vt-function-key-execute(2, UDK)\n\
	Shift<Key>F3:		vt-function-key-execute(3, UDK)\n\
	Shift<Key>F4:		vt-function-key-execute(4, UDK)\n\
	Shift<Key>F5:		vt-function-key-execute(5, UDK)\n\
	Shift<Key>F6:		vt-function-key-execute(6, UDK)\n\
	Shift<Key>F7:		vt-function-key-execute(7, UDK)\n\
	Shift<Key>F8:		vt-function-key-execute(8, UDK)\n\
	Shift<Key>F9:		vt-function-key-execute(9, UDK)\n\
	Shift<Key>F10:		vt-function-key-execute(10, UDK)\n\
	Shift<Key>F11:		vt-function-key-execute(11, UDK)\n\
	Shift<Key>F12:		vt-function-key-execute(12, UDK)\n\
	Shift<Key>F13:		vt-function-key-execute(13, UDK)\n\
	Shift<Key>F14:		vt-function-key-execute(14, UDK)\n\
	Shift<Key>F15:		vt-function-key-execute(15, UDK)\n\
	Shift<Key>F16:		vt-function-key-execute(16, UDK)\n\
	Shift<Key>F17:		vt-function-key-execute(17, UDK)\n\
	Shift<Key>F18:		vt-function-key-execute(18, UDK)\n\
	Shift<Key>F19:		vt-function-key-execute(19, UDK)\n\
	Shift<Key>F20:		vt-function-key-execute(20, UDK)\n\
	Shift<Key>F21:		vt-function-key-execute(21, UDK)\n\
	Shift<Key>F22:		vt-function-key-execute(22, UDK)\n\
	Shift<Key>F23:		vt-function-key-execute(23, UDK)\n\
	Shift<Key>F24:		vt-function-key-execute(24, UDK)\n\
	Shift<Key>F25:		vt-function-key-execute(25, UDK)\n\
	Shift<Key>F26:		vt-function-key-execute(26, UDK)\n\
	Shift<Key>F27:		vt-function-key-execute(27, UDK)\n\
	Shift<Key>F28:		vt-function-key-execute(28, UDK)\n\
	Shift<Key>F29:		vt-function-key-execute(29, UDK)\n\
	Shift<Key>F30:		vt-function-key-execute(30, UDK)\n\
	Shift<Key>F31:		vt-function-key-execute(31, UDK)\n\
	Shift<Key>F32:		vt-function-key-execute(32, UDK)\n\
	Shift<Key>F33:		vt-function-key-execute(33, UDK)\n\
	Shift<Key>F34:		vt-function-key-execute(34, UDK)\n\
	Shift<Key>F35:		vt-function-key-execute(35, UDK)\n\
	~Shift<Key>F1:		vt-function-key-execute(1, function)\n\
	~Shift<Key>F2:		vt-function-key-execute(2, function)\n\
	~Shift<Key>F3:		vt-function-key-execute(3, function)\n\
	~Shift<Key>F4:		vt-function-key-execute(4, function)\n\
	~Shift<Key>F5:		vt-function-key-execute(5, function)\n\
	~Shift<Key>F6:		vt-function-key-execute(6, function)\n\
	~Shift<Key>F7:		vt-function-key-execute(7, function)\n\
	~Shift<Key>F8:		vt-function-key-execute(8, function)\n\
	~Shift<Key>F9:		vt-function-key-execute(9, function)\n\
	~Shift<Key>F10:		vt-function-key-execute(10, function)\n\
	~Shift<Key>F11:		vt-function-key-execute(11, function)\n\
	~Shift<Key>F12:		vt-function-key-execute(12, function)\n\
	~Shift<Key>F13:		vt-function-key-execute(13, function)\n\
	~Shift<Key>F14:		vt-function-key-execute(14, function)\n\
	~Shift<Key>F15:		vt-function-key-execute(15, function)\n\
	~Shift<Key>F16:		vt-function-key-execute(16, function)\n\
	~Shift<Key>F17:		vt-function-key-execute(17, function)\n\
	~Shift<Key>F18:		vt-function-key-execute(18, function)\n\
	~Shift<Key>F19:		vt-function-key-execute(19, function)\n\
	~Shift<Key>F20:		vt-function-key-execute(20, function)\n\
	~Shift<Key>F21:		vt-function-key-execute(21, function)\n\
	~Shift<Key>F22:		vt-function-key-execute(22, function)\n\
	~Shift<Key>F23:		vt-function-key-execute(23, function)\n\
	~Shift<Key>F24:		vt-function-key-execute(24, function)\n\
	~Shift<Key>F25:		vt-function-key-execute(25, function)\n\
	~Shift<Key>F26:		vt-function-key-execute(26, function)\n\
	~Shift<Key>F27:		vt-function-key-execute(27, function)\n\
	~Shift<Key>F28:		vt-function-key-execute(28, function)\n\
	~Shift<Key>F29:		vt-function-key-execute(29, function)\n\
	~Shift<Key>F30:		vt-function-key-execute(30, function)\n\
	~Shift<Key>F31:		vt-function-key-execute(31, function)\n\
	~Shift<Key>F32:		vt-function-key-execute(32, function)\n\
	~Shift<Key>F33:		vt-function-key-execute(33, function)\n\
	~Shift<Key>F34:		vt-function-key-execute(34, function)\n\
	~Shift<Key>F35:		vt-function-key-execute(35, function)\n\
	<KeyRelease>:		key-release()\n\
	<KeyPress>:		insert()\n\
        ~Shift~Ctrl<Btn1Down>:	grab-focus()\n\
        Shift~Ctrl<Btn1Down>:   extend-start()\n\
	~Ctrl<Btn1Motion>:      select-adjust()\n\
	~Ctrl<Btn1Up>:		extend-end()\n\
        ~Shift<Btn2Down>:	process-bdrag()\n\
        ~Shift<Btn2Up>:		copy-to()\n\
	~Shift ~Ctrl <Btn4Up>:	scroll(-1,line)\n\
	~Shift ~Ctrl <Btn5Up>:	scroll(1,line)\n\
	Shift ~Ctrl <Btn4Up>:	scroll(-3,line)\n\
	Shift ~Ctrl <Btn5Up>:	scroll(3,line)\n\
	Ctrl ~Shift <Btn4Up>:	scroll(-1,page)\n\
	Ctrl ~Shift <Btn5Up>:	scroll(1,page)\n\
	<EnterWindow>:		enter()\n\
	<LeaveWindow>:		leave()\n\
	<FocusIn>:		focus-in()\n\
	<FocusOut>:		focus-out()\n\
	";

/* global class record for instances of class: Vt
 */

externaldef(vtclassrec) DtTermClassRec dtTermClassRec =
{
    /* core class record */
    {
	/* superclass		*/	(WidgetClass) &dtTermPrimitiveClassRec,
	/* class_name		*/	"DtTerm",
	/* widget_size		*/	sizeof(DtTermRec),
	/* class_initialize	*/	ClassInitialize,
	/* class_part_init	*/	ClassPartInitialize,
	/* class_inited		*/	FALSE,
	/* initialize		*/	Initialize,
	/* initialize_hook	*/	(XtArgsProc) NULL,
	/* realize		*/	XtInheritRealize,
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
	/* resize		*/	XtInheritResize, /* Resize */
	/* expose		*/	XtInheritExpose,
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

    /* primitive class rec */
    {
	/* Primitive border_highlight	*/  XmInheritWidgetProc,
	/* Primitive border_unhighlight	*/  XmInheritWidgetProc,
	/* translations			*/  "" /*NULL*/ /*XtInheritTranslations*/,
	/* arm_and_activate		*/  NULL,
	/* get resources		*/  (XmSyntheticResource *) GetResources,
	/* num get_resources		*/  XtNumber(GetResources),
	/* extension			*/  (XtPointer) NULL,
    },

    /* term class record */
    {
        /* parser_start_state           */  &_DtTermStateStart,
	/* use_history_buffer           */  True,
	/* allow_scroll_below_buffer    */  False,
	/* wrap_right_after_insert	*/  False,
        /* buffer_create_proc           */  _DtTermBufferCreateBuffer,
	/* buffer_free_proc		*/  _DtTermBufferFreeBuffer,
	/* term_insert_proc		*/  TextInsertProc,
        /* sizeOfBuffer                 */  (short) sizeof(DtTermBufferRec),
        /* sizeOfLine                   */  (short) sizeof(DtTermLineRec),
        /* sizeOfEnh                    */  (short) sizeof(DtTermEnhPart),
                        
    },

    /* vt class record */
    {
	0
    }
};

externaldef(vtwidgetclass) WidgetClass dtTermWidgetClass =
	(WidgetClass)&dtTermClassRec;

/* Class Initialize...
 */
static void
ClassInitialize(void)
{
    return;
}

static void
ClassPartInitialize(WidgetClass w_class)
{
}


/* 
** Initialize the modes to their default values...
*/
static void
initializeModes
(
    DtTermData td
)
{
    td->applicationMode  = False; /* Normal mode */
    td->applicationKPMode= False; /* Normal mode */
    td->tpd->transmitFunctions = True;  /* ALWAYS true for ANSI */
    td->originMode   = False;        /* true when origin mode is active  */
    td->S8C1TMode    = False;        /* use 7 bit c1 codes               */
    td->enhFieldState = FIELD_UNPROTECT; /* Vt selective erase mode          */
    td->fixCursesMode = False;         /* xterm fix curses */
    td->userKeysLocked = False;        /* are User Defined Keys locked? */
    td->needToLockUserKeys = False;        /* Parser state */
    td->saveCursor.cursorRow = 0;    /* init for Save Cursor (Esc 7) */
    td->saveCursor.cursorColumn = 0;    /* init for Save Cursor (Esc 7) */
    td->saveCursor.originMode = False;  /* init for Save Cursor (Esc 7) */
    td->saveCursor.wrapMode  = True;    /* init for Save Cursor (Esc 7) */
    td->saveCursor.enhVideoState = 0;   /* init for Save Cursor (Esc 7) */
    td->saveCursor.enhFieldState = FIELD_UNPROTECT;
    td->saveCursor.enhFgColorState = 0;
    td->saveCursor.enhBgColorState = 0; /* init for Save Cursor (Esc 7) */
    td->saveCursor.GL = td->GL;		/* init for Save Cursor (Esc 7) */
    td->saveCursor.GR = td->GR;		/* init for Save Cursor (Esc 7) */
    td->saveCursor.G0 = td->G0;		/* init for Save Cursor (Esc 7) */
    td->saveCursor.G1 = td->G1;		/* init for Save Cursor (Esc 7) */
    td->saveCursor.G2 = td->G2;		/* init for Save Cursor (Esc 7) */
    td->saveCursor.G3 = td->G3;		/* init for Save Cursor (Esc 7) */
    td->saveCursor.singleShiftFont = td->singleShiftFont;
    td->saveCursor.singleShiftPending = td->singleShiftPending;
    td->saveDECMode.allow80_132ColMode = False;  
    td->saveDECMode.col132Mode         = False;  
    td->saveDECMode.applicationMode    = False;  
    td->saveDECMode.applicationKPMode  = False;  
    td->saveDECMode.originMode         = False;  
    td->saveDECMode.wrapMode           = False;  
    td->saveDECMode.jumpScrollMode     = True;  
    td->saveDECMode.inverseVideoMode   = False; 
    td->saveDECMode.fixCursesMode     = False;
    td->saveDECMode.reverseWrapMode   = False;
    td->saveDECMode.marginBellMode    = False;
    td->saveDECMode.cursorVisible     = True;
    td->saveDECMode.logging           = False;

}

unsigned char
_DtTermGetRenderFontIndex(Widget w, int id)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;
    int i;

    /* search for the index that matches this id... */
    for (i = 0; i < (sizeof(td->renderFonts) / sizeof(td->renderFonts[0]));
	    i++) {
	if (td->renderFonts[i].id == id) {
	    return(i);
	}
    }

    /* default to base if not found... */
    return(0);
}
    
/*ARGSUSED*/
static void
Initialize(Widget ref_w, Widget w, Arg *args, Cardinal *num_args)
{
    DtTermPrimitiveWidget tpw = (DtTermPrimitiveWidget) w;
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td;
    int i;

    Debug('T', timeStamp("Term Initialize() starting"));

    /* initialize...
     */

    /* malloc Vt dataspace... */
    tw->vt.td =
	    (DtTermData) XtMalloc(sizeof(DtTermDataRec));
    td = tw->vt.td;

    /* clear malloc'ed memory... */
    (void) memset(td, '\0', sizeof(DtTermDataRec));

    /* stick a pointer to the DtTermPrimData struct in our DtTermData... */
    td->tpd = tw->term.tpd;

    /* set up our fonts...
     */
    for (i = 0; i < (sizeof(td->renderFonts) / sizeof(td->renderFonts[0]));
	    i++) {
	td->renderFonts[i].termFont = (TermFont) 0;
	td->renderFonts[i].id = -1;
    }
    td->renderFonts[0].termFont = td->tpd->defaultTermFont;
    td->renderFonts[0].id = 'B';
    td->baseFontIndex = 0;

    /* allocate a line drawing font... */
    td->renderFonts[1].termFont =
	    _DtTermPrimRenderLineDrawCreate(w,	/* Widget		*/
	    _DtTermGlyphs,			/* glyphInfo		*/
	    _DtTermNumGlyphs,			/* numGlyphs		*/
	    tw->term.widthInc,			/* width		*/
	    tw->term.ascent,			/* ascent		*/
	    tw->term.heightInc - tw->term.ascent);
						/* descent		*/
    td->renderFonts[1].id = '0';
    td->altFontIndex = 1;

    td->renderFonts[2].termFont = td->tpd->defaultBoldTermFont;

    td->G0 = RENDER_FONT_NORMAL;
    td->G1 = RENDER_FONT_LINEDRAW;		/* DKS!!! change this	*/
    td->G2 = RENDER_FONT_NORMAL;
    td->G3 = RENDER_FONT_NORMAL;

    td->GL = &td->G0;
    td->GR = &td->G0;

    td->singleShiftFont = RENDER_FONT_NORMAL;
    td->singleShiftPending = False;

    _DtTermFunctionKeyInit(w);

    /* 
    ** Initialize modes...
    */
    initializeModes(td);

    tpw->term.tpd->autoWrapRight = tw->vt.autoWrap ;
    tw->vt.td->reverseWrapMode = tw->vt.reverseWrap ;

    /* Initialize color...
     */
    (void) _DtTermColorInit(w);

    if ( !strcmp(tpw->term.termId,"vt100")) td->terminalId  = 100 ;
    else if ( !strcmp(tpw->term.termId,"vt101")) td->terminalId  = 101 ;
    else if ( !strcmp(tpw->term.termId,"vt102")) td->terminalId  = 102 ;
    else td->terminalId  = 220 ;               /* vt200  */
    if ( td->terminalId < 200 ) td->compatLevel = 1;
    else td->compatLevel = 2;

    _DtTermPrimSelectInitBtnEvents(w) ;  /* set button events */

    Debug('T', timeStamp("Term Initialize() finished"));
    return;
}

#ifdef	DKS
static void
InitializeVerticalScrollBar(Widget w, Boolean initCallbacks)
{
    DtTermWidget tw = (DtTermWidget) w;
    Arg arglist[20];
    int i;

    if (initCallbacks) {
	/* set up the scrollbar callbacks... */
	(void) XtAddCallback(tw->vt.verticalScrollBar, XmNdragCallback,
		VerticalScrollBarCallback, (XtPointer) w);
	(void) XtAddCallback(tw->vt.verticalScrollBar,
		XmNvalueChangedCallback, VerticalScrollBarCallback,
		(XtPointer) w);
    }

    if (!tw->vt.td) {
	/* no termBuffer yet, we set anything yet... */
	return;
    }

    /* update the scrollbar... */
    if (tw->vt.verticalScrollBar) {
	tw->vt.verticalScrollBarMaximum =
		(tw->term.tpd->lastUsedRow - tw->term.tpd->memoryLockRow) +
		(tw->term.tpd->term.rows - tw->term.tpd->memoryLockRow);
	tw->vt.verticalScrollBarSliderSize =
		tw->term.rows - tw->term.tpd->memoryLockRow;
	tw->vt.verticalScrollBarPageIncrement =
		tw->term.rows - tw->term.tpd->memoryLockRow;
	tw->vt.verticalScrollBarValue = tw->term.tpd->topRow;

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
    }
}
#endif	/* DKS */

static void
Resize(Widget w)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;

    /* let our superclass (the Term Widget) perform the resize... */
    tw->core.widget_class->core_class.superclass->core_class.resize(w);

#ifdef	DKS
    /* and update the scrollbar... */
    (void) InitializeVerticalScrollBar(w, False);
#endif	/* DKS */
}
/***************************************************************************
 *									   *
 * Import and Export functions for those resources that can't really be	   *
 * accessed through the widget DS.					   *
 *									   *
 ***************************************************************************/
 /* ARGSUSED */
static void 
GetAutoWrap(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    (*value) = (XtArgVal) tw->term.tpd->autoWrapRight;
}

/* ARGSUSED */
static XmImportOperator 
SetAutoWrap(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    tw->term.tpd->autoWrapRight =  (* (int *) value);
/*    return (XmSYNTHETIC_LOAD);*/
    return (XmSYNTHETIC_NONE);
}
 /* ARGSUSED */
static void 
GetReverseWrap(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    (*value) = (XtArgVal) tw->vt.td->reverseWrapMode;;
}

/* ARGSUSED */
static XmImportOperator 
SetReverseWrap(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    tw->vt.td->reverseWrapMode =  (* (int *) value);
/*    return (XmSYNTHETIC_LOAD);*/
    return (XmSYNTHETIC_NONE);
}

 /* ARGSUSED */
static void 
GetAppCursorMode(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    (*value) = (XtArgVal) tw->vt.td->applicationMode;;
}

/* ARGSUSED */
static XmImportOperator 
SetAppCursorMode(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    tw->vt.td->applicationMode =  (* (int *) value);
    return (XmSYNTHETIC_NONE);
}

 /* ARGSUSED */
static void 
GetAppKeypadMode(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    (*value) = (XtArgVal) tw->vt.td->applicationKPMode;;
}

/* ARGSUSED */
static XmImportOperator 
SetAppKeypadMode(
        Widget wid,
        int offset,
        XtArgVal *value )
{
     DtTermWidget tw = (DtTermWidget) wid ;
    tw->vt.td->applicationKPMode =  (* (int *) value);
    return (XmSYNTHETIC_NONE);
}
    
/* SetValues...
 */
/*ARGSUSED*/
static Boolean
SetValues(Widget cur_w, Widget ref_w, Widget w, ArgList args,
	Cardinal *num_args)
{
    DtTermWidget cur_tw = (DtTermWidget) cur_w;
    DtTermWidget tw = (DtTermWidget) w;
    Boolean flag = False;	/* return value... */


#ifdef	DKS
    /* DKS: this needs to be done later...
     */

    if (cur_tw->vt.verticalScrollBar != tw->vt.verticalScrollBar) {
	if (tw->vt.verticalScrollBar) {
	    /* set up the scrollbar values and callbacks... */
	    (void) InitializeVerticalScrollBar(w, True);
	}
    }
#endif	/* DKS */

    if (tw->term.fontList != cur_tw->term.fontList) {
	/* the font has been changed... */
	XmFontList fontList;
	XFontSet fontSet;
	XFontStruct *font;

	/* replace the base font...
	 */
	tw->vt.td->renderFonts[0].termFont = tw->term.tpd->termFont;

	/* replace the linedraw font...
	 */
	/* release the old font... */
	(void) _DtTermPrimDestroyFont(w, tw->vt.td->renderFonts[1].termFont);

	/* replace the font... */
	tw->vt.td->renderFonts[1].termFont =
		_DtTermPrimRenderLineDrawCreate(w,
		_DtTermGlyphs,
		_DtTermNumGlyphs,
		tw->term.widthInc,
		tw->term.ascent,
		tw->term.heightInc - tw->term.ascent);

	/* replace the bold font.  If the base font is the default, then
	 * use the default bold font that we generated.  Else, null it out
	 * and we will fake by overstriking...
	 */
	if (tw->vt.td->renderFonts[0].termFont ==
		tw->vt.td->tpd->defaultTermFont) {
	    tw->vt.td->renderFonts[2].termFont =
		    tw->vt.td->tpd->defaultBoldTermFont;
	} else {
	    tw->vt.td->renderFonts[2].termFont = (TermFont) 0;
	}

	flag = True;
    }

    return(flag);
}

/* Destroy...
 */
static void
Destroy(Widget w)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;

    /* remove the DtTermData structure contents, followed by the structure...
     */
    if (tw->vt.td) {
        /*  free up any fonts we allocated for this widget... */
        /*  don't free the others because they're done in TermPrim Destroy()*/
	(void) _DtTermPrimDestroyFont(w, tw->vt.td->renderFonts[1].termFont);

        /* free up any color cells that we allocated for this widget... */
        (void) _DtTermColorDestroy(w);

	/* free up our function key related resources... */
	(void) _DtTermFunctionKeyClear(w);

        /* 
        ** free up the block mode stuff...
        _vtBlockModeDestroy(td->blockModeInfo);
        td->blockModeInfo = (BlockModeInfo) 0;
        */

        /* free up the DtTermData structure... */
        (void) XtFree((char *) tw->vt.td);
        tw->vt.td = (DtTermData) 0;
    }
}

#ifdef	DKS
static void
StatusChangeNotify(Widget w, unsigned long mask)
{
    DtTermWidget tw = (DtTermWidget) w;
    struct termData *tpd = tw->term.tpd;
    struct vtData *td = tw->vt.td;
    TermVtCallbackStruct cb;

    if (!tw->vt.statusChangeCallback) {
	/* no callback, nothing to do... */
	return;
    }

    /*****************************************************************
     *  Caps Lock
     */
    if (mask & TermSTATUS_CHANGE_NOTIFY_CAPS_LOCK) {
	(void) memset(&cb, '\0', sizeof(cb));
	cb.reason = DtTermCR_CAPS_LOCK_CHANGE;
	cb.capsLock = tpd->capsLock;
	(void) XtCallCallbackList(w,
		tw->vt.statusChangeCallback, (XtPointer) &cb);
    }

    /*****************************************************************
     *  Insert Char Mode
     */
    if (mask & TermSTATUS_CHANGE_NOTIFY_INSERT_MODE) {
	(void) memset(&cb, '\0', sizeof(cb));
	cb.reason = DtTermCR_INSERT_MODE_CHANGE;
	cb.insertCharMode = tpd->insertCharMode;
	(void) XtCallCallbackList(w,
		tw->vt.statusChangeCallback, (XtPointer) &cb);
    }

    /*****************************************************************
     *  Stop
     */
    if (mask & TermSTATUS_CHANGE_NOTIFY_STOP) {
	(void) memset(&cb, '\0', sizeof(cb));
	cb.reason = DtTermCR_STOP_CHANGE;
	cb.stop = tpd->stop;
	(void) XtCallCallbackList(w,
		tw->vt.statusChangeCallback, (XtPointer) &cb);
    }
}

static void PositionUpdate(Widget w, short row, short column)
{
    DtTermWidget tw = (DtTermWidget) w;
    struct termData *tpd = tw->term.tpd;
    struct vtData *td = tw->vt.td;
    TermVtCallbackStruct cb;

    if (!tw->vt.statusChangeCallback) {
	/* no callback, nothing to do... */
	return;
    }

    cb.reason = DtTermCR_CURSOR_MOTION;
    cb.cursorColumn = tpd->cursorColumn;
    cb.cursorRow = tpd->cursorRow = tpd->topRow;
    (void) XtCallCallbackList(w,
	    tw->vt.statusChangeCallback, (XtPointer) &cb);
}
#endif	/* DKS */

Widget
DtCreateTerm(Widget parent, char *name, ArgList arglist, Cardinal argcount)
{
    Widget w;
    _DtTermWidgetToAppContext(parent);

    _DtTermAppLock(app);
    Debug('T', timeStamp("DtCreateTerm() starting"));
    w = XtCreateWidget(name, dtTermWidgetClass, parent, arglist, argcount);
    Debug('T', timeStamp("DtCreateTerm() finished"));
    _DtTermAppUnlock(app);
    return(w);
}

#ifdef	DKS
static void VerticalScrollBarCallback(Widget wid, XtPointer client_data,
	XtPointer call_data)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) client_data;
    struct termData *tpd = tw->term.tpd;
    XmScrollBarCallbackStruct *cb = (XmScrollBarCallbackStruct *) call_data;

    (void) _DtTermPrimScrollTextTo((Widget) tw, cb->value);
    /* we need to complete the scroll or it won't happen... */
    (void) _DtTermPrimScrollComplete((Widget) tw, True);
    if (cb->reason != XmCR_DRAG) {
	_DtTermPrimCursorOn((Widget) tw);
    }
}
#endif	/* DKS */

static int
TextInsertProc
(
    Widget		  w,
    unsigned char	 *buffer,
    int			  length
)
{
    DtTermWidget	  tw = (DtTermWidget) w;
    DtTermPrimData	  tpd = tw->term.tpd;
    DtTermData		  td = tw->vt.td;
    int			  partialLen = 0;
    int			  holdFont;

    if (td->singleShiftPending) {
	/* save away the active GL font... */
	holdFont = *td->GL;
	/* make it the single shift font... */
	*td->GL = td->singleShiftFont;
	(void) _DtTermPrimBufferSetEnhancement(tpd->termBuffer,
		tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		enhFont, *td->GL);

	/* insert the first character... */
	partialLen = _DtTermPrimInsertText(w, buffer, 1);

	/* restore the active GL font... */
	*td->GL = holdFont;
	(void) _DtTermPrimBufferSetEnhancement(tpd->termBuffer,
		tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		enhFont, *td->GL);

	if (partialLen == 1) {
	    /* successful insert...
	     */
	    /* clear the pending flag... */
	    td->singleShiftPending = False;
	    /* skip over the character... */
	    (void) buffer++;
	    (void) length--;
	} else {
	    return(partialLen);
	}
    }

    if (length > 0) {
	partialLen += _DtTermPrimInsertText(w, buffer, length);
    }

    return(partialLen);
}
