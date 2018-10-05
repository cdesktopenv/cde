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
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include "Term.h"
#include <string.h>		/* for strdup				*/
#include <errno.h>		/* for errno and sys_errlist[]		*/

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <X11/keysym.h>
#include <Xm/MwmUtil.h>
#include "TermViewGlobalDialog.h"
#include "TermViewTerminalDialog.h"
#include "TermViewMenu.h"
#include "TermViewP.h"
#include "TermFunction.h"
#include "TermPrimFunction.h"
#include "TermPrimMessageCatI.h"

static void HelpTerminalOptionsCallback(Widget w, XtPointer client_data,
	XtPointer call_data);


typedef struct _TerminalOptionsDialogType {
    Widget dtterm;
    Widget shell;
    Widget dialog;
    Widget form;
    struct 
    {
	struct 
	{
	    Widget normal;
	    Widget application;
	    Widget option;
	} cursor_key_mode;
	struct 
	{
	    Widget numeric;
	    Widget application;
	    Widget option;
	} keypad_mode;
	struct 
	{
	    Widget cr_return;
	    Widget return_lf;
	    Widget option;
	} newline_sequence;
	struct 
	{
	    Widget locked;
	    Widget unlocked;
	    Widget option;
	} key_lock;
    } keyboard_control;
    struct 
    {
	struct 
	{
	    Widget enabled;
	    Widget disabled;
	    Widget option;
	} columns;
	struct 
	{
	    Widget enabled;
	    Widget disabled;
	    Widget option;
	} autowrap;
	struct 
	{
	    Widget enabled;
	    Widget disabled;
	    Widget option;
	} reversewrap;
    } screen_control;
} TerminalOptionsDialogType;
	
static void
UpdateTerminalOptionsDialog
(
    TerminalOptionsDialogType	 *options
)
{
    Boolean		  cursorMode;
    Boolean		  keypadMode;
    Boolean		  autoWrap;
    Boolean		  reverseWrap;
    Boolean		  c132;
    Boolean		  lockState;
    Boolean		  autoLineFeed;
    Arg			  al[20];
    int			  ac;

    /* get the current widget values... */
    ac = 0;
    (void) XtSetArg(al[ac], DtNappCursorDefault, &cursorMode);	ac++;
    (void) XtSetArg(al[ac], DtNappKeypadDefault, &keypadMode);	ac++;
    (void) XtSetArg(al[ac], DtNautoWrap, &autoWrap);		ac++;
    (void) XtSetArg(al[ac], DtNreverseWrap, &reverseWrap);	ac++;
    (void) XtSetArg(al[ac], DtNc132, &c132);			ac++;
    (void) XtGetValues(options->dtterm, al, ac);
    lockState = _DtTermGetUserKeyLock(options->dtterm);
    autoLineFeed = _DtTermGetAutoLineFeed(options->dtterm);

    /* set the cursor mode... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (cursorMode) ? 
	    options->keyboard_control.cursor_key_mode.application :
	    options->keyboard_control.cursor_key_mode.normal);	ac++;
    (void) XtSetValues(options->keyboard_control.cursor_key_mode.option, al, ac);

    /* set the cursor mode... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (keypadMode) ? 
	    options->keyboard_control.keypad_mode.application :
	    options->keyboard_control.keypad_mode.numeric);	ac++;
    (void) XtSetValues(options->keyboard_control.keypad_mode.option, al, ac);
    /* set the lock mode... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (lockState) ? 
	    options->keyboard_control.key_lock.locked :
	    options->keyboard_control.key_lock.unlocked);	ac++;
    (void) XtSetValues(options->keyboard_control.key_lock.option, al, ac);

    /* set the auto line feed mode... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (autoLineFeed) ? 
	    options->keyboard_control.newline_sequence.return_lf :
	    options->keyboard_control.newline_sequence.cr_return);	ac++;
    (void) XtSetValues(options->keyboard_control.newline_sequence.option, al, ac);

    /* set the 132 mode... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (c132) ? 
	    options->screen_control.columns.enabled :
	    options->screen_control.columns.disabled);	ac++;
    (void) XtSetValues(options->screen_control.columns.option, al, ac);

    /* set the auto wrap mode... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (autoWrap) ? 
	    options->screen_control.autowrap.enabled :
	    options->screen_control.autowrap.disabled);	ac++;
    (void) XtSetValues(options->screen_control.autowrap.option, al, ac);
    /* set the reverse wrap mode... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (reverseWrap) ? 
	    options->screen_control.reversewrap.enabled :
	    options->screen_control.reversewrap.disabled);	ac++;
    (void) XtSetValues(options->screen_control.reversewrap.option, al, ac);

}

static void
ApplyTerminalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    TerminalOptionsDialogType	 *options = (TerminalOptionsDialogType *) clientData;
    Widget		  menuHistory;
    Arg			  al[20];
    int			  ac;
    Arg			  al2;
    char		 *c1;
    int			  i1;

    ac = 0;
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->keyboard_control.cursor_key_mode.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNappCursorDefault,
	    (menuHistory == options->keyboard_control.cursor_key_mode.application) ?
	    TRUE : FALSE);		ac++;

    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->keyboard_control.keypad_mode.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNappKeypadDefault,
	    (menuHistory == options->keyboard_control.keypad_mode.application) ?
	    TRUE : FALSE);		ac++;

    /* set the lock mode... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->keyboard_control.key_lock.option, &al2, 1);
     _DtTermSetUserKeyLock(options->dtterm, (menuHistory == options->keyboard_control.key_lock.locked));

    /* set the auto line feed mode... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->keyboard_control.newline_sequence.option, &al2, 1);
     _DtTermSetAutoLineFeed(options->dtterm, 
          (menuHistory == options->keyboard_control.newline_sequence.return_lf));


    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->screen_control.columns.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNc132,
	    (menuHistory == options->screen_control.columns.enabled) ?
	    TRUE : FALSE);		ac++;

    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->screen_control.autowrap.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNautoWrap,
	    (menuHistory == options->screen_control.autowrap.enabled) ?
	    TRUE : FALSE);		ac++;

    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->screen_control.reversewrap.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNreverseWrap,
	    (menuHistory == options->screen_control.reversewrap.enabled) ?
	    TRUE : FALSE);		ac++;

    if (ac > 0) {
	(void) XtSetValues(options->dtterm, al, ac);
    }
}

static void
OkTerminalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    TerminalOptionsDialogType	 *options = (TerminalOptionsDialogType *) clientData;

    /* apply the options... */
    (void) ApplyTerminalOptionsDialogCallback(w, clientData, callData);
    /* unmap the dialog... */
    (void) XtUnmanageChild(options->dialog);
}

static void
CancelTerminalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    TerminalOptionsDialogType	 *options = (TerminalOptionsDialogType *) clientData;

    /* unmap the dialog... */
    (void) XtUnmanageChild(options->dialog);
}

/*ARGSUSED*/
static void
HelpTerminalOptionsCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw = (DtTermViewWidget)client_data;
    (void) _DtTermViewMapHelp((Widget )tw, "Terminal", "TermOpts");
}

static void
MapTerminalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    TerminalOptionsDialogType	 *options = (TerminalOptionsDialogType *) clientData;

    (void) UpdateTerminalOptionsDialog(options);
}

Widget
_DtTermViewCreateTerminalOptionsDialog
(
    Widget		  parent
)
{
    TerminalOptionsDialogType	 *options;
    Widget		  KbdControlFrame;
    Widget		  KbdControlForm;
    Widget		  ScreenControlFrame;
    Widget		  ScreenControlForm;
    Widget		  LoggingFrame;
    Widget		  LoggingForm;
    Widget		  label;
    Widget		  pulldown;
    Widget		  tmp;
    Widget		  separator;
    Widget		  button;
    Widget		  cancel;
    Widget		  reset;
    XmString		  string;
    XmString		  resetString;
    XmString		  helpString;
    XmString		  okString;
    XmString		  cancelString;
    Arg			  al[20];
    int			  ac;
    _DtTermViewLineupList *lineupList;
    DtTermViewWidget      tw = (DtTermViewWidget) parent;
    
    options = (TerminalOptionsDialogType *) XtMalloc(sizeof(TerminalOptionsDialogType));
    (void) memset(options, '\0', sizeof(*options));

    options->dtterm = tw->termview.term;

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,26, "Terminal - Terminal Options")));
    okString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,2, "OK")));
    cancelString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,3, "Cancel")));
    helpString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,4, "Help")));
    resetString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,25, "Reset")));
    (void) XtSetArg(al[ac], XmNdialogTitle, string);			ac++;
    (void) XtSetArg(al[ac], XmNokLabelString, okString);		ac++;
    (void) XtSetArg(al[ac], XmNcancelLabelString, cancelString);	ac++;
    (void) XtSetArg(al[ac], XmNhelpLabelString, helpString);		ac++;
    (void) XtSetArg(al[ac], XmNautoUnmanage, False);			ac++;
    options->dialog = XmCreateTemplateDialog(parent, "terminal", al, ac);
    (void) XmStringFree(okString);
    (void) XmStringFree(cancelString);
    (void) XmStringFree(helpString);
    (void) XtSetArg(al[0], XmNlabelString, resetString);
    reset = XmCreatePushButtonGadget(options->dialog, "ResetButton", al, 1);
    XtManageChild(reset);
    (void) XmStringFree(resetString);
    (void) XtAddCallback(reset, XmNactivateCallback,
	    MapTerminalOptionsDialogCallback, (XtPointer) options);

    (void) XtAddCallback(options->dialog, XmNokCallback,
	    OkTerminalOptionsDialogCallback, (XtPointer) options);
    (void) XtAddCallback(options->dialog, XmNcancelCallback,
	    CancelTerminalOptionsDialogCallback, (XtPointer) options);
    (void) XtAddCallback(options->dialog, XmNmapCallback,
	    MapTerminalOptionsDialogCallback, (XtPointer) options);
    (void) XtAddCallback(options->dialog, XmNhelpCallback,
	    HelpTerminalOptionsCallback, (XtPointer) tw);

    options->shell = XtParent(options->dialog);

    ac = 0;
    (void) XtSetArg(al[ac], XmNallowShellResize, False);		ac++;
    (void) XtSetValues(options->shell, al, ac);

    ac = 0;
    options->form = XmCreateForm(options->dialog, "form", al, ac);
    (void) XtManageChild(options->form);

/****************
 *
 * Keyboard Control modes - cursor keys, keypad, newline and fn lock
 *
 ****************/
    ac = 0;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNmarginWidth, 5);				ac++;
    (void) XtSetArg(al[ac], XmNmarginHeight, 5);			ac++;
    KbdControlFrame = XmCreateFrame(options->form, "KbdControlFrame", al, ac);
    (void) XtManageChild(KbdControlFrame);

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,5, "Keyboard Control")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD);		ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);			ac++;
    label = XmCreateLabelGadget(KbdControlFrame, "KbdControlFrameLabel", al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(label);

    ac = 0;
    KbdControlForm = XmCreateForm(KbdControlFrame, "KbdControlForm", al, ac);

    /*
     * option menu: Cursor Key Mode [normal|application]
     */
    pulldown =
	    _DtTermViewCreatePulldown(KbdControlForm, "cursorKeyPulldown");
#ifdef CALLBACKS
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    CursorKeyModeCallback, (XtPointer) options);
#endif
    options->keyboard_control.cursor_key_mode.normal =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,6, "Normal")), NoSymbol, NULL, NULL, NULL, NULL);
    options->keyboard_control.cursor_key_mode.application =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,7, "Application")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(KbdControlForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,8, "Cursor Key Mode ")), NoSymbol, al, ac);
    options->keyboard_control.cursor_key_mode.option = tmp;

    /* add to the lineup list... */
    lineupList = _DtTermViewLineupListCreate();
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->keyboard_control.cursor_key_mode.option),
	    options->keyboard_control.cursor_key_mode.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * option menu: Keypad Mode [Numeric | Application]
     */
    pulldown =
	    _DtTermViewCreatePulldown(KbdControlForm, "keypadModePulldown");
#ifdef CALLBACKS
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    KeyPadModeCallback, (XtPointer) options);
#endif
    options->keyboard_control.keypad_mode.numeric =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,9, "Numeric")), NoSymbol, NULL, NULL, NULL, NULL);
    options->keyboard_control.keypad_mode.application =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,10, "Application")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(KbdControlForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,11, "Keypad Mode ")), NoSymbol, al, ac);
    options->keyboard_control.keypad_mode.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->keyboard_control.keypad_mode.option),
	    options->keyboard_control.keypad_mode.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * option menu: Newline Sequence: [CR | CRLF]
     */
    pulldown =
	    _DtTermViewCreatePulldown(KbdControlForm, "NewlineSeqPulldown");
#ifdef CALLBACKS
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    NewlineSequenceCallback, (XtPointer) options);
#endif
    options->keyboard_control.newline_sequence.cr_return =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,12, "Return Only")), NoSymbol, NULL, NULL, NULL, NULL);
    options->keyboard_control.newline_sequence.return_lf =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,13, "Return/Line Feed")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(KbdControlForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,14, "Newline Sequence ")), NoSymbol, al, ac);
    options->keyboard_control.newline_sequence.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->keyboard_control.newline_sequence.option),
	    options->keyboard_control.newline_sequence.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * option menu: Key Lock: [Enabled | Disabled]
     */
    pulldown =
	    _DtTermViewCreatePulldown(KbdControlForm, "KeyLockPulldown");
#ifdef CALLBACKS
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    FunctionKeyLockCallback, (XtPointer) options);
#endif
    options->keyboard_control.key_lock.locked =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,15, "Locked")), NoSymbol, NULL, NULL, NULL, NULL);
    options->keyboard_control.key_lock.unlocked =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,16, "Unlocked")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
    tmp = _DtTermViewCreateOptionMenu(KbdControlForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,17, "User Function Keys ")), NoSymbol, al, ac);
    options->keyboard_control.key_lock.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->keyboard_control.key_lock.option),
	    options->keyboard_control.key_lock.option,
	    CORRECTION_OPTION_MENU);


    /* manage the cursor form... */
    (void) XtManageChild(KbdControlForm);

/****************
 *
 * Screen Control modes - 132, autowrap, reversewrap
 *
 ****************/
    ac = 0;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, KbdControlFrame);		ac++;
    (void) XtSetArg(al[ac], XmNtopOffset, 5);				ac++;
    (void) XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
    (void) XtSetArg(al[ac], XmNmarginWidth, 5);				ac++;
    (void) XtSetArg(al[ac], XmNmarginHeight, 5);			ac++;
    ScreenControlFrame = XmCreateFrame(options->form, "ScreenControlFrame",
	    al, ac);
    (void) XtManageChild(ScreenControlFrame);

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,18, "Screen Control")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD);		ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);			ac++;
    label = XmCreateLabelGadget(ScreenControlFrame, "ScreenControlFrameLabel",
	    al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(label);

    ac = 0;
    ScreenControlForm = XmCreateForm(ScreenControlFrame, "ScreenControlForm", al, ac);

    /*
     * option menu: 132 column mode: [enabled | disabled]
     */

    pulldown =
	    _DtTermViewCreatePulldown(ScreenControlForm, "one32Pulldown");
#ifdef CALLBACKS
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    ColumnSwitchCallback, (XtPointer) options);
#endif
    options->screen_control.columns.enabled =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,19, "Enabled")), NoSymbol,
	    NULL, NULL, NULL, NULL);
    options->screen_control.columns.disabled =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,20, "Disabled")), NoSymbol,
	    NULL, NULL, NULL, NULL);
    
    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(ScreenControlForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,21, "132 Column Switching")), NoSymbol, al, ac);
    options->screen_control.columns.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->screen_control.columns.option),
	    options->screen_control.columns.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * option menu: Autowrap: [enabled|disabled]
     */
    pulldown =
	    _DtTermViewCreatePulldown(ScreenControlForm, "AutoWrapPulldown");
#ifdef CALLBACKS
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    AutoWrapCallback, (XtPointer) options);
#endif
    options->screen_control.autowrap.enabled =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,19, "Enabled")), NoSymbol, NULL, NULL, NULL, NULL);
    options->screen_control.autowrap.disabled =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,20, "Disabled")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(ScreenControlForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,22, "End-of-line Wrapping ")), NoSymbol, al, ac);
    options->screen_control.autowrap.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->screen_control.autowrap.option),
	    options->screen_control.autowrap.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * option menu: Reverse wrap: [enabled|disabled]
     */
    pulldown =
	    _DtTermViewCreatePulldown(ScreenControlForm, "reverseWrapPulldown");
#ifdef CALLBACKS
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    ReverseWrapCallback, (XtPointer) options);
#endif
    options->screen_control.reversewrap.enabled =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,19, "Enabled")), NoSymbol, NULL, NULL, NULL, NULL);
    options->screen_control.reversewrap.disabled =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,20, "Disabled")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
    tmp = _DtTermViewCreateOptionMenu(ScreenControlForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewTerminalDialog,23, "Reverse End-of-line Wrapping ")), NoSymbol, al, ac);
    options->screen_control.reversewrap.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->screen_control.reversewrap.option),
	    options->screen_control.reversewrap.option,
	    CORRECTION_OPTION_MENU);

    /* manage the cursor form... */
    (void) XtManageChild(ScreenControlForm);

    /* lineup all the labels... */
    (void) _DtTermViewLineupListLineup(lineupList);
    (void) _DtTermViewLineupListFree(lineupList);
    lineupList = (_DtTermViewLineupList *) 0;


    /* create the apply button... */
    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewTerminalDialog,24, "Apply")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    button = XmCreatePushButtonGadget(options->dialog, "apply", al, ac);
    (void) XmStringFree(string);
    (void) XtAddCallback(button, XmNactivateCallback,
	    ApplyTerminalOptionsDialogCallback, (XtPointer) options);
    (void) XtManageChild(button);

    ac = 0;
    (void) XtSetArg(al[ac], XmNmwmFunctions, MWM_FUNC_MOVE);		ac++;
    (void) XtSetArg(al[ac], XmNmwmDecorations,
	    MWM_DECOR_MENU | MWM_DECOR_BORDER | MWM_DECOR_TITLE);	ac++;
    (void) XtSetValues(options->shell, al, ac);

    (void) XtManageChild(options->dialog);


    return(options->dialog);
}
