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
static char rcs_id[] = "$XConsortium: TermViewGlobalDialog.c /main/2 1996/08/28 16:49:57 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
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
#include "TermViewMenu.h"
#include "TermViewP.h"
#include "TermPrimCursor.h"
#include "TermPrimFunction.h"
#include "TermPrimMessageCatI.h"

static void HelpTerminalOptionsCallback(Widget w, XtPointer client_data,
	XtPointer call_data);



_DtTermViewLineupList *
_DtTermViewLineupListCreate
(
)
{
    _DtTermViewLineupList *list;

    list = (_DtTermViewLineupList *) XtMalloc(sizeof(_DtTermViewLineupList));
    list->next = (_DtTermViewLineupList *) 0;
    list->left = (Widget) 0;
    list->right = (Widget) 0;

    return(list);
}

void
_DtTermViewLineupListAddItem
(
    _DtTermViewLineupList *list,
    Widget		  left,
    Widget		  right,
    Dimension		  correction
)
{
    _DtTermViewLineupList *tmp;

    tmp = (_DtTermViewLineupList *) XtMalloc(sizeof(_DtTermViewLineupList));
    tmp->left = left;
    tmp->right = right;
    tmp->correction = correction;
    tmp->next = list->next;
    list->next = tmp;
}

void
_DtTermViewLineupListLineup
(
    _DtTermViewLineupList *list
)
{
    _DtTermViewLineupList *ptr;
    Arg al[20];
    int ac;
    int widthMax = 0;
    int marginLeftMax = 0;
    int marginRightMax = 0;
    int marginWidthMax = 0;
    int marginLeft = 0;

    /* run through the list and figure out the maximum left and right
     * widths...
     */
    for (ptr = list->next; ptr; ptr = ptr->next) {
	if (ptr->left) {
	    ac = 0;
	    (void) XtSetArg(al[ac], XmNwidth, &ptr->widthLeft);		ac++;
	    (void) XtSetArg(al[ac], XmNmarginLeft, &ptr->marginLeft);	ac++;
	    (void) XtSetArg(al[ac], XmNmarginRight, &ptr->marginRight);	ac++;
	    (void) XtSetArg(al[ac], DtNmarginWidth, &ptr->marginWidth);	ac++;
	    (void) XtGetValues(ptr->left, al, ac);
	    if (((int)(ptr->widthLeft + ptr->correction)) > widthMax) {
		widthMax = ptr->widthLeft + ptr->correction;
	    }
	    if ((int)ptr->marginLeft > marginLeftMax) {
		marginLeftMax = ptr->marginLeft;
	    }
	    if ((int)ptr->marginRight > marginRightMax) {
		marginRightMax = ptr->marginRight;
	    }
	    if ((int)ptr->marginWidth > marginWidthMax) {
		marginWidthMax = ptr->marginWidth;
	    }
	}
    }

    /* run back throught the list and set the left margin accordingly... */
    if (widthMax > 0) {
	for (ptr = list->next; ptr; ptr = ptr->next) {
	    if (ptr->left) {
		marginLeft = widthMax + marginLeftMax - ptr->widthLeft +
			ptr->correction;
		ac = 0;
		(void) XtSetArg(al[ac], XmNmarginLeft, marginLeft);
									ac++;
		(void) XtSetArg(al[ac], DtNmarginWidth, marginWidthMax);
									ac++;
		(void) XtSetArg(al[ac], XmNmarginRight, marginRightMax);
									ac++;
		(void) XtSetValues(ptr->left, al, ac);
	    }
	}
    }
}

void
_DtTermViewLineupListFree
(
    _DtTermViewLineupList *list
)
{
    _DtTermViewLineupList *ptr;

    while (list) {
	ptr = list;
	list = list->next;

	(void) XtFree((char *) ptr);
    }
}

Widget
_DtTermViewCreatePulldown
(
    Widget		  parent,
    char		 *name
)
{
    Arg arglist[20];
    int i;
    Widget w;

    i = 0;
    w = XmCreatePulldownMenu(parent, name, arglist, i);
    return(w);
}

Widget
_DtTermViewCreateOptionMenu
(
    Widget		  parent,
    Widget		  submenu,
    char		 *label,
    KeySym		  mnemonic,
    Arg			  al[],
    int			  ac
)
{
    Widget		  w;
    XmString		  string;

    if (label) {
	string = XmStringCreateLocalized(label);
	(void) XtSetArg(al[ac], XmNlabelString, string);		ac++;
    }
    if (mnemonic != NoSymbol) {
	(void) XtSetArg(al[ac], XmNmnemonic, mnemonic);			ac++;
	(void) XtSetArg(al[ac], XmNmnemonicCharSet, XmFONTLIST_DEFAULT_TAG);
									ac++;
    }
    (void) XtSetArg(al[ac], XmNsubMenuId, submenu);			ac++;
    
    w = XmCreateOptionMenu(parent, label, al, ac);
    (void) XtManageChild(w);

    if (label) {
	(void) XmStringFree(string);
    }
    return(w);
}

typedef struct _OptionsDialogType {
    Widget dtterm;
    Widget shell;
    Widget dialog;
    Widget form;
    struct {
	struct {
	    Widget box;
	    Widget underline;
    	    Widget invisible;
	    Widget option;
	} type;
	struct {
	    Widget on;
	    Widget off;
	    Widget option;
	} blink;
	struct {
	    Widget label;
	    Widget text;
	    int oldBlinkRate;
	} blinkRate;
    } cursor;
    struct {
	struct {
	    Widget on;
	    Widget off;
	    Widget option;
	} inverse;
    } background;
    struct {
	struct {
	    Widget jump;
	    Widget smooth;
	    Widget option;
	} type;
    } scroll;
    struct {
	struct {
	    Widget audible;
	    Widget visual;
	    Widget option;
	} type;
	struct {
	    Widget on;
	    Widget off;
	    Widget option;
	} bellMargin;
	struct {
	    Widget label;
	    Widget text;
	} bellMarginDistance;
    } bell;
} OptionsDialogType;
	
void
_DtTermViewEqualizeHeights
(
    Widget		  label,
    Widget		  text
)
{
    Arg			  al[10];
    int			  ac;
    Dimension		  labelHeight;
    Dimension		  textHeight;

    /* make the label and textfield the same height... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNheight, &labelHeight);			ac++;
    (void) XtGetValues(label, al, ac);

    ac = 0;
    (void) XtSetArg(al[ac], XmNheight, &textHeight);			ac++;
    (void) XtGetValues(text, al, ac);

    if (textHeight > labelHeight) {
	ac = 0;
	(void) XtSetArg(al[ac], XmNheight, textHeight);			ac++;
	(void) XtSetValues(label, al, ac);
    } else if (textHeight < labelHeight) {
	ac = 0;
	(void) XtSetArg(al[ac], XmNheight, labelHeight);		ac++;
	(void) XtSetValues(text, al, ac);
    }
}

static void
CursorBlinkCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    OptionsDialogType	 *options = (OptionsDialogType *) clientData;
    XmRowColumnCallbackStruct
			 *cb = (XmRowColumnCallbackStruct *) callData;
    char		  buffer[BUFSIZ];
    char		 *c1;

    if (cb->widget == options->cursor.blink.on) {
	(void) XtSetSensitive(options->cursor.blinkRate.label, True);
	(void) XtSetSensitive(options->cursor.blinkRate.text, True);
	c1 = XmTextFieldGetString(options->cursor.blinkRate.text);
	if (atoi(c1) <= 0) {
	    if (options->cursor.blinkRate.oldBlinkRate <= 0) {
		options->cursor.blinkRate.oldBlinkRate = 250;
	    }
	    (void) sprintf(buffer, "%d",
		    options->cursor.blinkRate.oldBlinkRate);
	    (void) XmTextFieldSetString(options->cursor.blinkRate.text, buffer);
	}
    } else {
	(void) XtSetSensitive(options->cursor.blinkRate.label, False);
	(void) XtSetSensitive(options->cursor.blinkRate.text, False);
	c1 = XmTextFieldGetString(options->cursor.blinkRate.text);
	options->cursor.blinkRate.oldBlinkRate = atoi(c1);
    }
}

static void
BellMarginCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    OptionsDialogType	 *options = (OptionsDialogType *) clientData;
    XmRowColumnCallbackStruct
			 *cb = (XmRowColumnCallbackStruct *) callData;

    if (cb->widget == options->bell.bellMargin.on) {
	(void) XtSetSensitive(options->bell.bellMarginDistance.label, True);
	(void) XtSetSensitive(options->bell.bellMarginDistance.text, True);
    } else {
	(void) XtSetSensitive(options->bell.bellMarginDistance.label, False);
	(void) XtSetSensitive(options->bell.bellMarginDistance.text, False);
    }
}

static void
UpdateGlobalOptionsDialog
(
    OptionsDialogType	 *options
)
{
    unsigned char	  charCursorStyle;
    int			  blinkRate;
    Boolean		  jumpScroll;
    Boolean		  marginBell;
    int			  nMarginBell;
    Boolean		  visualBell;
    Boolean		  reverseVideo;
    Arg			  al[20];
    int			  ac;
    char		  buffer[BUFSIZ];

    /* get the current widget values... */
    ac = 0;
    (void) XtSetArg(al[ac], DtNcharCursorStyle, &charCursorStyle);	ac++;
    (void) XtSetArg(al[ac], DtNblinkRate, &blinkRate);			ac++;
    (void) XtSetArg(al[ac], DtNjumpScroll, &jumpScroll);		ac++;
    (void) XtSetArg(al[ac], DtNmarginBell, &marginBell);		ac++;
    (void) XtSetArg(al[ac], DtNnMarginBell, &nMarginBell);		ac++;
    (void) XtSetArg(al[ac], DtNvisualBell, &visualBell);		ac++;
    (void) XtSetArg(al[ac], DtNreverseVideo, &reverseVideo);		ac++;
    (void) XtGetValues(options->dtterm, al, ac);

    /* set the cursor type... */
    ac = 0;
    
    if (_DtTermPrimGetCursorVisible(options->dtterm))
        (void) XtSetArg(al[ac], XmNmenuHistory,
	    (charCursorStyle == DtTERM_CHAR_CURSOR_BOX) ?
	    options->cursor.type.box : options->cursor.type.underline);	
    else 
        (void) XtSetArg(al[ac], XmNmenuHistory, options->cursor.type.invisible);	
    ac++;
    (void) XtSetValues(options->cursor.type.option, al, ac);

    /* set the blink info... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    (blinkRate == 0) ?
	    options->cursor.blink.off : options->cursor.blink.on);	ac++;
    (void) XtSetValues(options->cursor.blink.option, al, ac);
    if (blinkRate == 0) {
	(void) XtSetSensitive(options->cursor.blinkRate.label, False);
	(void) XtSetSensitive(options->cursor.blinkRate.text, False);
	if (options->cursor.blinkRate.oldBlinkRate <= 0) {
	    (void) XmTextFieldSetString(options->cursor.blinkRate.text, "");
	} else {
	    (void) sprintf(buffer, "%d",
		    options->cursor.blinkRate.oldBlinkRate);
	    (void) XmTextFieldSetString(options->cursor.blinkRate.text, buffer);
	}
    } else {
	(void) XtSetSensitive(options->cursor.blinkRate.label, True);
	(void) XtSetSensitive(options->cursor.blinkRate.text, True);
	(void) sprintf(buffer, "%d", blinkRate);
	(void) XmTextFieldSetString(options->cursor.blinkRate.text, buffer);
	if (blinkRate > 0) {
	    options->cursor.blinkRate.oldBlinkRate = blinkRate;
	}
    }

    /* set the scroll type... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    jumpScroll ?
	    options->scroll.type.jump : options->scroll.type.smooth);	ac++;
    (void) XtSetValues(options->scroll.type.option, al, ac);


    /* set the reverse video... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    reverseVideo ?
	    options->background.inverse.on : options->background.inverse.off);	ac++;
    (void) XtSetValues(options->background.inverse.option, al, ac);

    /* set the visual bell... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    visualBell ?
	    options->bell.type.visual : options->bell.type.audible);
									ac++;
    (void) XtSetValues(options->bell.type.option, al, ac);

    /* set the margin bell... */
    ac = 0;
    (void) XtSetArg(al[ac], XmNmenuHistory,
	    marginBell ?
	    options->bell.bellMargin.on : options->bell.bellMargin.off);
									ac++;
    (void) XtSetValues(options->bell.bellMargin.option, al, ac);
    if (marginBell) {
	(void) XtSetSensitive(options->bell.bellMarginDistance.label, True);
	(void) XtSetSensitive(options->bell.bellMarginDistance.text, True);
    } else {
	(void) XtSetSensitive(options->bell.bellMarginDistance.label, False);
	(void) XtSetSensitive(options->bell.bellMarginDistance.text, False);
    }
    (void) sprintf(buffer, "%d", nMarginBell);
    (void) XmTextFieldSetString(options->bell.bellMarginDistance.text, buffer);
}

static void
ApplyGlobalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    OptionsDialogType	 *options = (OptionsDialogType *) clientData;
    Widget		  menuHistory;
    Arg			  al[20];
    int			  ac;
    Arg			  al2;
    char		 *c1;
    int			  i1;
    int			  blinkRate;

    ac = 0;
    /* char cursor style... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->cursor.type.option, &al2, 1);
    if (menuHistory == options->cursor.type.invisible)
             _DtTermPrimSetCursorVisible(options->dtterm, False);
    else 
    {
        _DtTermPrimSetCursorVisible(options->dtterm, True);
        _DtTermPrimCursorOn(options->dtterm);
    	if (menuHistory == options->cursor.type.box)
             (void) XtSetArg(al[ac], DtNcharCursorStyle,DtTERM_CHAR_CURSOR_BOX );
        else 
	     (void) XtSetArg(al[ac], DtNcharCursorStyle, DtTERM_CHAR_CURSOR_BAR);		
        ac++;
    }


    /* cursor blink rate... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->cursor.blink.option, &al2, 1);
    if (menuHistory == options->cursor.blink.off) {
	(void) XtSetArg(al[ac], DtNblinkRate, 0);			ac++;
    } else {
	c1 = XmTextFieldGetString(options->cursor.blinkRate.text);
	blinkRate = atoi(c1);
	(void) XtSetArg(al[ac], DtNblinkRate, blinkRate);		ac++;
	if (blinkRate > 0) {
	    options->cursor.blinkRate.oldBlinkRate = blinkRate;
	}
    }

    /* jump scroll... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->scroll.type.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNjumpScroll,
	    (menuHistory == options->scroll.type.jump) ?
	    True : False);						ac++;

    /* reverse video... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->background.inverse.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNreverseVideo,
	    (menuHistory == options->background.inverse.on) ?
	    True : False);						ac++;

    /* visual bell... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->bell.type.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNvisualBell,
	    (menuHistory == options->bell.type.visual) ?
	    True : False);						ac++;

    /* margin bell... */
    (void) XtSetArg(al2, XmNmenuHistory, &menuHistory);
    (void) XtGetValues(options->bell.bellMargin.option, &al2, 1);
    (void) XtSetArg(al[ac], DtNmarginBell,
	    (menuHistory == options->bell.bellMargin.on) ?
	    True : False);						ac++;

    /* margin bell margin... */
    if (menuHistory == options->bell.bellMargin.on) {
	c1 = XmTextFieldGetString(options->bell.bellMarginDistance.text);
	(void) XtSetArg(al[ac], DtNnMarginBell, atoi(c1));		ac++;
    }

    /* and set all the current options... */
    if (ac > 0) {
	(void) XtSetValues(options->dtterm, al, ac);
    }
}

static void
OkGlobalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    OptionsDialogType	 *options = (OptionsDialogType *) clientData;

    /* apply the options... */
    (void) ApplyGlobalOptionsDialogCallback(w, clientData, callData);
    /* unmap the dialog... */
    (void) XtUnmanageChild(options->dialog);
}

static void
CancelGlobalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    OptionsDialogType	 *options = (OptionsDialogType *) clientData;

    /* unmap the dialog... */
    (void) XtUnmanageChild(options->dialog);
}

/*ARGSUSED*/
static void
HelpTerminalOptionsCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw = (DtTermViewWidget)client_data;
    (void) _DtTermViewMapHelp((Widget )tw, "Terminal", "GlobOpts");
}

static void
MapGlobalOptionsDialogCallback
(
    Widget		  w,
    XtPointer		  clientData,
    XtPointer		  callData
)
{
    OptionsDialogType	 *options = (OptionsDialogType *) clientData;

    (void) UpdateGlobalOptionsDialog(options);
}

Widget
_DtTermViewCreateGlobalOptionsDialog
(
    Widget		  parent
)
{
    OptionsDialogType	 *options;
    Widget		  cursorFrame;
    Widget		  cursorForm;
    Widget		  backgroundFrame;
    Widget		  backgroundForm;
    Widget		  scrollFrame;
    Widget		  scrollForm;
    Widget		  bellFrame;
    Widget		  bellForm;
    Widget		  label;
    Widget		  pulldown;
    Widget		  tmp;
    Widget		  separator;
    Widget		  button;
    Widget		  cancel;
    Widget		  reset;
    XmString		  string;
    XmString		  helpString;
    XmString		  okString;
    XmString		  cancelString;
    XmString		  resetString;
    Arg			  al[20];
    int			  ac;
    _DtTermViewLineupList *lineupList;
    DtTermViewWidget      tw = (DtTermViewWidget) parent;
    
    options = (OptionsDialogType *) XtMalloc(sizeof(OptionsDialogType));
    (void) memset(options, '\0', sizeof(*options));

    options->dtterm = tw->termview.term;

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,34, "Terminal - Global Options")));
    okString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,2, "OK")));
    cancelString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,3, "Cancel")));
    helpString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,4, "Help")));
    resetString = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,33, "Reset")));
    (void) XtSetArg(al[ac], XmNdialogTitle, string);			ac++;
    (void) XtSetArg(al[ac], XmNokLabelString, okString);		ac++;
    (void) XtSetArg(al[ac], XmNcancelLabelString, cancelString);	ac++;
    (void) XtSetArg(al[ac], XmNhelpLabelString, helpString);		ac++;
    (void) XtSetArg(al[ac], XmNautoUnmanage, False);			ac++;
    options->dialog = XmCreateTemplateDialog(parent, (GETMESSAGE(NL_SETN_ViewGlobalDialog,5, "global")), al, ac);
    (void) XmStringFree(okString);
    (void) XmStringFree(cancelString);
    (void) XmStringFree(helpString);
    (void) XtSetArg(al[0], XmNlabelString, resetString);
    reset = XmCreatePushButtonGadget(options->dialog, "ResetButton", al, 1);
    XtManageChild(reset);
    (void) XmStringFree(resetString);
    (void) XtAddCallback(reset, XmNactivateCallback,
	    MapGlobalOptionsDialogCallback, (XtPointer) options);
    (void) XtAddCallback(options->dialog, XmNokCallback,
	    OkGlobalOptionsDialogCallback, (XtPointer) options);
    (void) XtAddCallback(options->dialog, XmNcancelCallback,
	    CancelGlobalOptionsDialogCallback, (XtPointer) options);
    (void) XtAddCallback(options->dialog, XmNmapCallback,
	    MapGlobalOptionsDialogCallback, (XtPointer) options);
    (void) XtAddCallback(options->dialog, XmNhelpCallback,
	    HelpTerminalOptionsCallback, (XtPointer) tw);

    options->shell = XtParent(options->dialog);

    ac = 0;
    (void) XtSetArg(al[ac], XmNallowShellResize, False);		ac++;
    (void) XtSetValues(options->shell, al, ac);

    ac = 0;
    options->form = XmCreateForm(options->dialog, "form", al, ac);
    (void) XtManageChild(options->form);

    /****************************************************************
     ****************************************************************
     *** Cursor parameters...
     ***/
    ac = 0;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], DtNmarginWidth, 5);				ac++;
    (void) XtSetArg(al[ac], DtNmarginHeight, 5);			ac++;
    cursorFrame = XmCreateFrame(options->form, "cursorFrame", al, ac);
    (void) XtManageChild(cursorFrame);

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,6, "Cursor Control")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD);		ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);			ac++;
    label = XmCreateLabelGadget(cursorFrame, "cursorFrameLabel", al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(label);

    ac = 0;
    cursorForm = XmCreateForm(cursorFrame, "cursorForm", al, ac);

    /****************************************************************
     * option menu: type [ box | underline ]
     */
    pulldown =
	    _DtTermViewCreatePulldown(cursorForm, "cursorTypePulldown");
    options->cursor.type.box =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,7, "Box")), NoSymbol, NULL, NULL, NULL, NULL);
    options->cursor.type.underline =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,8, "Underline")), NoSymbol, NULL, NULL, NULL, NULL);
    options->cursor.type.invisible =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,14, "Invisible")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(cursorForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,9, "Cursor Style")), NoSymbol, al, ac);
    options->cursor.type.option = tmp;

    /* add to the lineup list... */
    lineupList = _DtTermViewLineupListCreate();
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->cursor.type.option),
	    options->cursor.type.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * option menu: blink [ on | off ]
     */
    pulldown =
	    _DtTermViewCreatePulldown(cursorForm, "cursorBlinkPulldown");
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    CursorBlinkCallback, (XtPointer) options);
    options->cursor.blink.on =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,10, "Enabled")), NoSymbol, NULL, NULL, NULL, NULL);
    options->cursor.blink.off =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,11, "Disabled")), NoSymbol, NULL, NULL, NULL, NULL);

    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(cursorForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,12, "Blinking Cursor")), NoSymbol, al, ac);
    options->cursor.blink.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->cursor.blink.option),
	    options->cursor.blink.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * text field: blink rate _250_
     */

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,13, "Blink Rate (milliseconds)")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);	ac++;
    options->cursor.blinkRate.label =
	    XmCreateLabelGadget(cursorForm, "blinkRateLabel", al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(options->cursor.blinkRate.label);

    ac = 0;
    (void) XtSetArg(al[ac], DtNcolumns, 4);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET);	ac++;
    (void) XtSetArg(al[ac], XmNleftWidget, options->cursor.blinkRate.label);
									ac++;
    (void) XtSetArg(al[ac], XmNleftOffset, CORRECTION_TEXT_OFFSET);	ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNnavigationType, XmNONE);  		ac++;
    options->cursor.blinkRate.text =
	    XmCreateTextField(cursorForm, (GETMESSAGE(NL_SETN_ViewGlobalDialog,15, "blinkRate")), al, ac);
    (void) XtManageChild(options->cursor.blinkRate.text);
    options->cursor.blinkRate.oldBlinkRate = 0;

    /* make the label and textfield the same height... */
    _DtTermViewEqualizeHeights(options->cursor.blinkRate.label,
	    options->cursor.blinkRate.text);

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    options->cursor.blinkRate.label,
	    options->cursor.blinkRate.text,
	    CORRECTION_LABEL);

    /* manage the cursor form... */
    (void) XtManageChild(cursorForm);


    /****************************************************************
     ****************************************************************
     **** Background parameters...
     ***/
    ac = 0;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, cursorFrame);		ac++;
    (void) XtSetArg(al[ac], XmNtopOffset, 5);				ac++;
    (void) XtSetArg(al[ac], DtNmarginWidth, 5);				ac++;
    (void) XtSetArg(al[ac], DtNmarginHeight, 5);			ac++;
    backgroundFrame = XmCreateFrame(options->form, "backgroundFrame",
	    al, ac);
    (void) XtManageChild(backgroundFrame);

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,16, "Color Control")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD);		ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);			ac++;
    label = XmCreateLabelGadget(backgroundFrame, "backgroundFrameLabel",
	    al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(label);

    ac = 0;
    backgroundForm = XmCreateForm(backgroundFrame, "backgroundForm", al, ac);

    /****************************************************************
     * option menu: inverse video [on | off ]
     */

    pulldown =
	    _DtTermViewCreatePulldown(backgroundForm, "inverseVideoPulldown");
    options->background.inverse.off =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,18, "Normal")), NoSymbol,
	    NULL, NULL, NULL, NULL);
    options->background.inverse.on =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,17, "Inverse")), NoSymbol,
	    NULL, NULL, NULL, NULL);
    
    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
    tmp = _DtTermViewCreateOptionMenu(backgroundForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,19, "Window Background")), NoSymbol, al, ac);
    options->background.inverse.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->background.inverse.option),
	    options->background.inverse.option,
	    CORRECTION_OPTION_MENU);

    /* manage the cursor form... */
    (void) XtManageChild(backgroundForm);

    
    /****************************************************************
     ****************************************************************
     **** Scroll parameters...
     ***/
    ac = 0;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, backgroundFrame);		ac++;
    (void) XtSetArg(al[ac], XmNtopOffset, 5);				ac++;
    (void) XtSetArg(al[ac], DtNmarginWidth, 5);				ac++;
    (void) XtSetArg(al[ac], DtNmarginHeight, 5);			ac++;
    scrollFrame = XmCreateFrame(options->form, "scrollFrame", al, ac);
    (void) XtManageChild(scrollFrame);

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,20, "Scroll Behavior")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD);		ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);			ac++;
    label = XmCreateLabelGadget(scrollFrame, "scrollFrameLabel", al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(label);

    ac = 0;
    scrollForm = XmCreateForm(scrollFrame, "scrollForm", al, ac);

    /****************************************************************
     * option menu: type [jump | smooth ]
     */
    pulldown =
	    _DtTermViewCreatePulldown(scrollForm, "ScrollTypePulldown");
    options->scroll.type.jump =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,21, "Disabled")), NoSymbol, NULL, NULL, NULL, NULL);
    options->scroll.type.smooth =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,22, "Enabled")), NoSymbol, NULL, NULL, NULL, NULL);
    
    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
    tmp = _DtTermViewCreateOptionMenu(scrollForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,23, "Smooth Scrolling")), NoSymbol, al, ac);
    options->scroll.type.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->scroll.type.option),
	    options->scroll.type.option,
	    CORRECTION_OPTION_MENU);

    /* manage the cursor form... */
    (void) XtManageChild(scrollForm);


    /****************************************************************
     ****************************************************************
     *** Bell parameters...
     ***/
    ac = 0;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, scrollFrame);			ac++;
    (void) XtSetArg(al[ac], XmNtopOffset, 5);				ac++;
    (void) XtSetArg(al[ac], DtNmarginWidth, 5);				ac++;
    (void) XtSetArg(al[ac], DtNmarginHeight, 5);			ac++;
    bellFrame = XmCreateFrame(options->form, "bellFrame",
	    al, ac);
    (void) XtManageChild(bellFrame);

    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,24, "Bell Control")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNchildType, XmFRAME_TITLE_CHILD);		ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);			ac++;
    label = XmCreateLabelGadget(bellFrame, "bellFrameLabel", al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(label);

    ac = 0;
    bellForm = XmCreateForm(bellFrame, "bellForm", al, ac);

    /****************************************************************
     * option menu: Type [audible | visible ]
     */
    pulldown =
	    _DtTermViewCreatePulldown(bellForm, "typePulldown");
    options->bell.type.audible =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,25, "Audible")), NoSymbol, NULL, NULL, NULL, NULL);
    options->bell.type.visual =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,26, "Visible")), NoSymbol, NULL, NULL, NULL, NULL);
    
    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(bellForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,27, "Bell Type")), NoSymbol, al, ac);
    options->bell.type.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->bell.type.option),
	    options->bell.type.option,
	    CORRECTION_OPTION_MENU);


    /****************************************************************
     * option menu: Margin Bell [on | off ]
     */
    pulldown =
	    _DtTermViewCreatePulldown(bellForm, "bellMarginPulldown");
    (void) XtAddCallback(pulldown, XmNentryCallback,
	    BellMarginCallback, (XtPointer) options);
    options->bell.bellMargin.on =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,28, "Enabled")), NoSymbol, NULL, NULL, NULL, NULL);
    options->bell.bellMargin.off =
	    _DtTermViewCreatePushButton(pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,29, "Disabled")), NoSymbol, NULL, NULL, NULL, NULL);
    
    ac = 0;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
    tmp = _DtTermViewCreateOptionMenu(bellForm, pulldown,
	    (GETMESSAGE(NL_SETN_ViewGlobalDialog,30, "Margin Warning")), NoSymbol, al, ac);
    options->bell.bellMargin.option = tmp;

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    XmOptionLabelGadget(options->bell.bellMargin.option),
	    options->bell.bellMargin.option,
	    CORRECTION_OPTION_MENU);

    /****************************************************************
     * text field: Margin _8_
     */
    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,31, "Margin Distance")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM);		ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
    (void) XtSetArg(al[ac], XmNtraversalOn, False);	ac++;
    options->bell.bellMarginDistance.label =
	    XmCreateLabelGadget(bellForm, "bellMarginDistanceLabel", al, ac);
    (void) XmStringFree(string);
    (void) XtManageChild(options->bell.bellMarginDistance.label);

    ac = 0;
    (void) XtSetArg(al[ac], DtNcolumns, 3);				ac++;
    (void) XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET);	ac++;
    (void) XtSetArg(al[ac], XmNleftWidget,
	    options->bell.bellMarginDistance.label);			ac++;
    (void) XtSetArg(al[ac], XmNleftOffset, CORRECTION_TEXT_OFFSET);	ac++;
    (void) XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET);		ac++;
    (void) XtSetArg(al[ac], XmNtopWidget, tmp);				ac++;
    (void) XtSetArg(al[ac], XmNnavigationType, XmNONE);  		ac++;
    options->bell.bellMarginDistance.text =
	    XmCreateTextField(bellForm, "margin", al, ac);
    (void) XtManageChild(options->bell.bellMarginDistance.text);

    /* make the label and textfield the same height... */
    _DtTermViewEqualizeHeights(options->bell.bellMarginDistance.label,
	    options->bell.bellMarginDistance.text);

    /* add to the lineup list... */
    (void) _DtTermViewLineupListAddItem(lineupList,
	    options->bell.bellMarginDistance.label,
	    options->bell.bellMarginDistance.text,
	    CORRECTION_LABEL);

    /* manage the margin form... */
    (void) XtManageChild(bellForm);

    /* lineup all the labels... */
    (void) _DtTermViewLineupListLineup(lineupList);
    (void) _DtTermViewLineupListFree(lineupList);
    lineupList = (_DtTermViewLineupList *) 0;

    /* create the apply button... */
    ac = 0;
    string = XmStringCreateLocalized((GETMESSAGE(NL_SETN_ViewGlobalDialog,32, "Apply")));
    (void) XtSetArg(al[ac], XmNlabelString, string);			ac++;
    button = XmCreatePushButtonGadget(options->dialog, "apply", al, ac);
    (void) XmStringFree(string);
    (void) XtAddCallback(button, XmNactivateCallback,
	    ApplyGlobalOptionsDialogCallback, (XtPointer) options);
    (void) XtManageChild(button);

    ac = 0;
    (void) XtSetArg(al[ac], XmNmwmFunctions, MWM_FUNC_MOVE);		ac++;
    (void) XtSetArg(al[ac], XmNmwmDecorations,
	    MWM_DECOR_MENU | MWM_DECOR_BORDER | MWM_DECOR_TITLE);	ac++;
    (void) XtSetValues(options->shell, al, ac);

    (void) XtManageChild(options->dialog);


    return(options->dialog);
}
