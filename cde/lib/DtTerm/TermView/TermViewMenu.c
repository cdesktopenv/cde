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
static char rcs_id[] = "$XConsortium: TermViewMenu.c /main/4 1996/09/26 16:42:48 samborn $";
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

#define	PULLDOWN_ACCELERATORS
#define WINDOW_SIZE_TOGGLES

#include "TermHeader.h"
#include <string.h>		/* for strdup				*/
#include <errno.h>		/* for errno and sys_errlist[]		*/

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <Xm/SelectioB.h>
#include <X11/keysym.h>
#include "TermViewMenu.h"
#include "TermViewP.h"
#include "TermPrimFunction.h"
#include "TermViewGlobalDialog.h"
#include "TermViewTerminalDialog.h"
#include "TermPrimMessageCatI.h"
#include "TermPrimSelect.h"
#include "TermFunction.h"

static Widget currentWidget = (Widget ) 0;
					/* widget for current menu
					 * context
					 */
static Widget scrollBarToggle;
static Widget menuBarToggle;
static Widget *fontSizeToggles;
static int fontSizeTogglesDefault = -1;
#ifdef	WINDOW_SIZE_TOGGLES
static Widget *windowSizeToggles;
#endif	/* WINDOW_SIZE_TOGGLES */
static Widget newButton;

/* forward declarations...
 */
static Widget CreateMenu(Widget termView, Widget parent, Boolean menuBar,
	Arg menuArglist[], int menuArgcount);
static void removeFromPostFromListCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void setContext(Widget w, XtPointer client_data, XtPointer call_data);
static void exitCallback(Widget w, XtPointer client_data, XtPointer call_data);
static void scrollBarToggleCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void menuBarToggleCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void cloneCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
#ifdef	HPVUE
static void helpVersionCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpIntroCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpTasksCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpReferenceCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpOnHelpCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
#else	/* HPVUE */
static void helpOverviewCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpTableOfContentsCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpTasksCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpReferenceCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpKeyboardCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpUsingHelpCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void helpAboutDttermCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
#endif	/* HPVUE */
static void globalOptionsCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void terminalOptionsCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void sizeChangeCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void defaultSizeCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void fontChangeCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void defaultFontCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void softResetCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void hardResetCallback(Widget w, XtPointer client_data,
	XtPointer call_data);
static void copyClipboardCallback(Widget w, XtPointer client_data,
        XtPointer call_data);
static void pasteClipboardCallback(Widget w, XtPointer client_data,
        XtPointer call_data);


/**************************************************************************/
#ifdef	TOKEN_CALLBACK
static void
tokenCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (client_data) {
	(void) printf("%s: button pressed\n", (char *) client_data);
    }
}
#endif	/* TOKEN_CALLBACK */

/*ARGSUSED*/
static void
postMenu(Widget w, XtPointer client_data, XEvent *event, Boolean *cont)
{
    Widget popupMenu = (Widget) client_data;

    /* now position and manage the menu... */
    (void) XmMenuPosition(popupMenu, &event->xbutton);
    (void) XtManageChild(popupMenu);
}

static Widget
createMenuWidget(WidgetClass widgetClass, char *callbackName,
	Widget parent, Widget subMenuId, char *label, KeySym mnemonic,
	char *accelerator, char *acceleratorText, 
	XtCallbackProc callback, XtPointer clientData)
{
    Widget w;
    XmString string;
    XmString acceleratorString = (XmString) 0;
    Arg arglist[20];
    int i;

    i = 0;

    /* create the label string and stuff it... */
    string = XmStringCreateLocalized(label);
    (void) XtSetArg(arglist[i], XmNlabelString, string); i++;

    /* if a mnemonic was specified, set the mnemonic and mnemonic charset... */
    if (mnemonic != NoSymbol) {
	(void) XtSetArg(arglist[i], XmNmnemonic, mnemonic); i++;
	(void) XtSetArg(arglist[i], XmNmnemonicCharSet, XmFONTLIST_DEFAULT_TAG);
		i++;
    }

    /* if an accelerator was specified, stuff it... */
    if (accelerator && *accelerator) {
	(void) XtSetArg(arglist[i], XmNaccelerator, accelerator); i++;
    }

    /* if acceleratorText was specified, create the XmString and stuff it... */
    if (acceleratorText && *acceleratorText) {
	acceleratorString = XmStringCreateLocalized(acceleratorText);
	(void) XtSetArg(arglist[i], XmNacceleratorText, acceleratorString); i++;
    }

    /* if subMenuId was specified, stuff it... */
    if (subMenuId) {
	(void) XtSetArg(arglist[i], XmNsubMenuId, subMenuId); i++;
    }

    /* create the widget... */
    w = XtCreateManagedWidget(label, widgetClass, parent, arglist, i);
    if (callback) {
	(void) XtAddCallback(w, callbackName, callback, clientData);
    }

#ifdef	TOKEN_CALLBACK
    /*DKS -- this is to help debug the menu stuff... */
    if (!strcmp(callbackName, XmNactivateCallback)) {
	(void) XtAddCallback(w, callbackName, tokenCallback, strdup(label));
    }
#endif	/* TOKEN_CALLBACK */

    /* free up storage... */
    (void) XmStringFree(string);
    if (acceleratorString)
	(void) XmStringFree(acceleratorString);

    /* return the widget... */
    return(w);
}

Widget
_DtTermViewCreateCascadeButton(Widget parent, Widget subMenuId, char *label,
	KeySym mnemonic, char *accelerator, char *acceleratorText, 
	XtCallbackProc callback, XtPointer clientData)
{
    return(createMenuWidget(xmCascadeButtonGadgetClass, XmNactivateCallback,
	parent, subMenuId, label, mnemonic, accelerator, acceleratorText,
	callback, clientData));
}

Widget
_DtTermViewCreatePushButton(Widget parent, char *label, KeySym mnemonic,
	char *accelerator, char *acceleratorText,
	XtCallbackProc callback, XtPointer clientData)
{
    return(createMenuWidget(xmPushButtonGadgetClass, XmNactivateCallback,
	parent, NULL, label, mnemonic, accelerator, acceleratorText,
	callback, clientData));
}

Widget
_DtTermViewCreateToggleButton(Widget parent, char *label, KeySym mnemonic,
	char *accelerator, char *acceleratorText,
	XtCallbackProc callback, XtPointer clientData)
{
    return(createMenuWidget(xmToggleButtonGadgetClass, XmNvalueChangedCallback,
	parent, NULL, label, mnemonic, accelerator, acceleratorText,
	callback, clientData));
}

Widget
_DtTermViewCreateLabel(Widget parent, char *label)
{
    return(createMenuWidget(xmLabelGadgetClass, NULL,
	parent, NULL, label, 0, NULL, NULL, NULL, NULL));
}

Widget
_DtTermViewCreateSeparator(Widget parent, char *label)
{
    return(createMenuWidget(xmSeparatorGadgetClass, NULL,
	parent, NULL, label, 0, NULL, NULL, NULL, NULL));
}

static Widget
createPulldown(Widget parent, char *name, Arg *arglist, int argcnt)
{
    Widget w;

    w = XmCreatePulldownMenu(parent, name, arglist, argcnt);
    (void) XtAddCallback(w, XmNmapCallback, setContext, (XtPointer) NULL);
    return(w);
}

/*ARGSUSED*/
static void
removeFromPostFromListCallback(Widget w, XtPointer client_data,
	XtPointer call_data)
{
    (void) XmRemoveFromPostFromList((Widget) client_data, w);
}

static char *
mallocGETMESSAGE(int msgset, int msg, char *defaultString)
{
    char *c1;
    char *c2;

    c2 = GETMESSAGE(msgset, msg, defaultString);
    c1 = XtMalloc(strlen(c2) + 1);
    (void) strcpy(c1, c2);
    return(c1);
}

static void
createSizeMenu
(
    Widget		  w,
    Widget		  menu
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Widget		  submenu;
    Widget		  button;
    long		  i1;
    char		 *c1;
    char		 *c2;
#ifdef	NOTDEF
    char		  mnemonics[BUFSIZ];
#endif	/* NOTDEF */
    char		  buffer[BUFSIZ];
    KeySym		  ks;
    Arg			  al[20];
    int			  ac;

    ac = 0;
    (void) XtSetArg(al[ac], XmNradioBehavior, True); ac++;
    submenu = createPulldown(menu, "Window Size", al, ac);

#ifdef	NOTDEF
    /* clear out mnemonics string... */
    *mnemonics = '\0';
    c1 = mnemonics;
#endif	/* NOTDEF */

    /* create the size buttons... */
#ifdef	WINDOW_SIZE_TOGGLES
    windowSizeToggles = (Widget *)
	    XtMalloc((1 + tw->termview.sizeList.numSizes) *sizeof(Widget));
#endif	/* WINDOW_SIZE_TOGGLES */
    for (i1 = 0; i1 < tw->termview.sizeList.numSizes; i1++) {
	*buffer = '\0';
	if (tw->termview.sizeList.sizes[i1].columns > 0) {
	    (void) sprintf(buffer + strlen(buffer), "%hd",
		    tw->termview.sizeList.sizes[i1].columns);
	}
	(void) strcat(buffer, "x");
	if (tw->termview.sizeList.sizes[i1].rows > 0) {
	    (void) sprintf(buffer + strlen(buffer), "%hd",
		    tw->termview.sizeList.sizes[i1].rows);
	}
#ifdef NOTDEF
	for (c2 = buffer; *c2; c2++) {
	    if (!strchr(mnemonics, *c2) && !isspace(*c2)) {
		break;
	    }
	}
	if (*c2) {
	    /* add it to the mnemonics list... */
	    *c1++ = *c2;
	    ks = XK_A + *c2 - 'A';
	} else {
	    ks = NULL;
	}
#endif
#ifdef	WINDOW_SIZE_TOGGLES
	windowSizeToggles[i1] = _DtTermViewCreateToggleButton(submenu, buffer,
		0, NULL, NULL, sizeChangeCallback, (XtPointer) i1);
#else	/* WINDOW_SIZE_TOGGLES */
	button = _DtTermViewCreatePushButton(submenu, buffer,
		0, NULL, NULL, sizeChangeCallback, (XtPointer) i1);
#endif	/* WINDOW_SIZE_TOGGLES */
    }

    /* get a mnemonic for "Default"... */
    (void) strcpy(buffer, (GETMESSAGE(NL_SETN_ViewMenu,1, "Default")));
#ifdef NOTDEF
    for (c2 = buffer; *c2; c2++) {
	if (!strchr(mnemonics, *c2) && !isspace(*c2))
	    break;
    }
    if (*c2) {
	/* add it to the mnemonics list... */
	*c1++ = *c2;
	ks = XK_A + *c2 - 'A';
    } else {
	ks = NULL;
    }
#endif 
#ifdef	WINDOW_SIZE_TOGGLES
    windowSizeToggles[i1] = _DtTermViewCreateToggleButton(submenu, buffer,
	    0, NULL, NULL, defaultSizeCallback, NULL);
#else	/* WINDOW_SIZE_TOGGLES */
    button = _DtTermViewCreatePushButton(submenu, buffer,
	    0, NULL, NULL, defaultSizeCallback, NULL);
#endif	/* WINDOW_SIZE_TOGGLES */

    ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,3, "W"));
    (void) _DtTermViewCreateCascadeButton(menu,
	    submenu, (GETMESSAGE(NL_SETN_ViewMenu,2, "Window Size")), 
	              ks, 
		      NULL, NULL, NULL, NULL);
}

extern char _DtTermViewMenuDefaultFonts[];

typedef struct _fontArrayType {
    char *labelName;
    char *fontName;
    XmFontList fontList;
} fontArrayType;

static fontArrayType *fontArray = (fontArrayType *) 0;
static short fontArrayCount = 0;

static void
createFontMenu
(
    Widget		  w,
    Widget		  menu
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    Widget		  submenu;
    Widget		  button;
    long		  i1;
    int			  i2;
    char		 *c1;
    char		 *c2;
    char		  mnemonics[BUFSIZ];
    char		  buffer[BUFSIZ];
    char		  fontName[BUFSIZ];
    KeySym		  ks;
    int			  dpi;
    char		**fontNames;
    int			  fontNameCount;
    float		  pointSize;
    Arg			  al[20];
    int			  ac;

    if (!tw->termview.userFontList || !*tw->termview.userFontList) {
	tw->termview.userFontList = _DtTermViewMenuDefaultFonts;
    }

    /* calculate dots per inch... */
    dpi = HeightOfScreen(XtScreen(w)) / (HeightMMOfScreen(XtScreen(w)) / 25.4);

    ac = 0;
    (void) XtSetArg(al[ac], XmNradioBehavior, True); ac++;
    submenu = createPulldown(menu, "Font Size", al, ac);

    /* find out how many newlines there are in the userFontList so
     * that we can build an array big enough to hold them...
     */
    for (i1 = 1, c1 = tw->termview.userFontList; *c1; ) {
	if (*c1++ == '\n') {
	    (void) i1++;
	}
    }
    /* malloc out an array... */
    fontArray = (fontArrayType *) XtMalloc(sizeof(fontArrayType) *
	    i1);
    for (i1 = 0, c1 = tw->termview.userFontList; *c1; ) {
	/* copy over the userFontList up to the end, or a newline... */
	for (c2 = buffer; *c1 && (*c1 != '\n'); ) {
	    *c2++ = *c1++;
	}
	/* null term the copy and skip over the newline... */
	*c2++ = '\0';
	if (*c1 == '\n') {
	    (void) c1++;
	}

	/* if this entry is empty, skip it... */
	if (!*buffer) {
	    continue;
	}

	fontArray[i1].labelName = XtMalloc(strlen(buffer) + 1);
	(void) strcpy(fontArray[i1].labelName, buffer);
	/* look for a separating '/'... */
	if ((c2 = strchr(fontArray[i1].labelName, '/'))) {
	    /* found, null it out... */
	    *c2++ = '\0';
	    /* and assign it to the fontName... */
	    fontArray[i1].fontName = c2;
	} else {
	    /* calculate the pixelsize for the font... */
	    fontArray[i1].fontName = fontArray[i1].labelName;
	    (void) strcpy(fontName, fontArray[i1].labelName);

	    /* clear out the .labelName... */
	    fontArray[i1].labelName = (char *) 0;

	    /* is it a fontset?... */
	    if (fontName[strlen(fontName) - 1] == ':') {
		/* let's turn in into an iso8859-1 name for the query... */
		fontName[strlen(fontName) - 1] = '\0';
		/* strip off a '-' before the ':' (should not have been
		 * one, but)...
		 */
		if (fontName[strlen(fontName) - 1] == '-') {
		    fontName[strlen(fontName) - 1] = '\0';
		}
		(void) strcat(fontName, (GETMESSAGE(NL_SETN_ViewMenu,4, "-iso8859-1")));
	    }
	    if ((fontNames =
		    XListFonts(XtDisplay(w), fontName, 1, &fontNameCount))) {
		c2 = *fontNames;
		for (i2 = 0; i2 < 7; i2++) {
		    while (*c2 && (*c2 != '-')) {
			c2++;
		    }
		    if (!*c2) {
			break;
		    }
		    /* skip over the '-'... */
		    (void) c2++;
		}
		if (i2 == 7) {
		    pointSize = ((float) strtol(c2, (char **) 0, 0)) *
			    72 / dpi;
		    /* this was taken from the style manager... */
		    if (dpi <= 72) {
			/* whole points... */
			(void) sprintf(fontName, (GETMESSAGE(NL_SETN_ViewMenu,5, "%d point")),
				(int) (pointSize + 0.5));
		    } else if (dpi <= 144) {
			/* half points... */
			(void) sprintf(fontName, (GETMESSAGE(NL_SETN_ViewMenu,6, "%.1f point")),
				((int) (pointSize * 2.0 + 0.5)) / 2.0);
		    } else if (dpi <= 720) {
			/* tenth point... */
			(void) sprintf(fontName, (GETMESSAGE(NL_SETN_ViewMenu,7, "%.1f point")),
				((int) (pointSize * 10.0 + 0.5)) / 10.0);
		    } else {
			/* hundredth point... */
			(void) sprintf(fontName, (GETMESSAGE(NL_SETN_ViewMenu,8, "%.2f point")),
				((int) (pointSize * 100.0 + 0.5)) / 100.0);
		    }
		    fontArray[i1].labelName = XtMalloc(strlen(fontName) + 1);
		    (void) strcpy(fontArray[i1].labelName, fontName);
		}
		/* free up the fontNames... */
		(void) XFreeFontNames(fontNames);
	    }
	    if (!fontArray[i1].labelName) {
		fontArray[i1].labelName = fontArray[i1].fontName;
	    }
	}
	/* bump the count... */
	(void) i1++;
    }

    /* we have our list... */
    fontArrayCount = i1;

    /* clear out mnemonics string... */
    *mnemonics = '\0';
    c1 = mnemonics;

    /* create the font buttons... */
    fontSizeToggles = (Widget *) XtMalloc((fontArrayCount + 1) * sizeof(Widget));

    for (i1 = 0; i1 < fontArrayCount; i1++) {
#ifdef NOTDEF
	for (c2 = fontArray[i1].labelName; *c2; c2++) {
	    if (!strchr(mnemonics, *c2) && !isspace(*c2))
		break;
	}
	if (*c2) {
	    /* add it to the mnemonics list... */
	    *c1++ = *c2;
	    ks = XK_A + *c2 - 'A';
	} else {
	    ks = NULL;
	}
#endif 
	fontSizeToggles[i1] = _DtTermViewCreateToggleButton(submenu,
		fontArray[i1].labelName,
		0, NULL, NULL, fontChangeCallback, (XtPointer) i1);
    }

    (void) strcpy(buffer, (GETMESSAGE(NL_SETN_ViewMenu,9, "Default")));
    fontSizeToggles[i1] = _DtTermViewCreateToggleButton(submenu, buffer,
	    0, NULL, NULL, defaultFontCallback, NULL);
    fontSizeTogglesDefault = i1;

    ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,11, "F"));
    (void) _DtTermViewCreateCascadeButton(menu, submenu,
	    (GETMESSAGE(NL_SETN_ViewMenu,10, "Font Size")), 
	    ks, 
	    NULL, NULL, NULL, NULL);
}

#define	numPulldowns	5
static Widget
CreateMenu(Widget termView, Widget parent, Boolean menuBar,
	Arg menuArglist[], int menuArgcount)
{
    Widget menu;
    Widget cascade;
    static Widget topLevel = (Widget) 0;
    static Boolean first = True;
    static Boolean firstPopup = True;
    static Widget pulldown[numPulldowns];
    static Widget popupMenu = (Widget) 0;
    int pc;
    Widget button;
    Widget submenu;
    Arg arglist[20];
    Arg *newArglist;
    int i;
    KeySym		  ks;
    char		 *accelerator;
    char		 *acceleratorText;

    /* if this is the first time, we will need a topLevel widget... */
    if (first) {
	i = 0;
	topLevel = XtAppCreateShell((char *) 0, "Dtterm",
		applicationShellWidgetClass, XtDisplay(parent), arglist, i);
    }
	
    newArglist = (Arg *) XtMalloc((menuArgcount + 1) * sizeof(Arg));
    for (i = 0; i < menuArgcount; i++) {
	newArglist[i].name = menuArglist[i].name;
	newArglist[i].value = menuArglist[i].value;
    }
    if (menuBar) {
	/* we will create the menubar unmanaged so that our parent can mange
	 * it when it wants to...
	 */
	menu = XmCreateMenuBar(parent, "menu_pulldown", newArglist, i);
    } else {
	if (firstPopup) {
	    popupMenu = XmCreatePopupMenu(topLevel, "menu_popup", newArglist,
		    i);
	}
	menu = popupMenu;
	button = _DtTermViewCreateLabel(menu,
	          (GETMESSAGE(NL_SETN_ViewMenu,83, "Terminal")));
	button = _DtTermViewCreateSeparator(menu, "Separator");
    }
    (void) XtFree((char *) newArglist);

    pc = 0;
    if (first) {
	/* create the "Window" pulldown... */
	i = 0;
	pulldown[pc] = createPulldown(topLevel, "Window", arglist, i);
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,13, "N"));
	newButton = _DtTermViewCreatePushButton(pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,12, "New")),
		ks,
		NULL, NULL, cloneCallback, NULL);
#ifdef	NOTDEF
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,15, "P"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,14, "Print")),
		ks,
		NULL, NULL, NULL, NULL);
	(void) XtSetSensitive(button, False);
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,17, "r"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,16, "Print...")),
		ks,
		NULL, NULL, NULL, NULL);
	(void) XtSetSensitive(button, False);
#endif	/* NOTDEF */
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,19, "C"));
	accelerator = mallocGETMESSAGE(NL_SETN_ViewMenu,62, "Alt F4");
	acceleratorText = mallocGETMESSAGE(NL_SETN_ViewMenu,62, "Alt+F4");
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,18, "Close")),
		ks,
		accelerator,
		acceleratorText,
		exitCallback, NULL);
	(void) XtFree(accelerator);
	(void) XtFree(acceleratorText);
    }
    if (menuBar || (!menuBar && firstPopup))
#ifdef	PULLDOWN_ACCELERATORS
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,21, "W"));
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	          (GETMESSAGE(NL_SETN_ViewMenu,20, "Window")),
		  ks,
		  NULL, NULL, NULL, NULL);
#else	/* PULLDOWN_ACCELERATORS */
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,20, "Window")),
		NoSymbol, NULL, NULL, NULL, NULL);
#endif	/* PULLDOWN_ACCELERATORS */

    (void) pc++;
    if (first) {
	i = 0;
	pulldown[pc] = createPulldown(topLevel, "Edit", arglist, i);
        accelerator = mallocGETMESSAGE(NL_SETN_ViewMenu,24,
		"Ctrl <Key>osfInsert");
	acceleratorText = mallocGETMESSAGE(NL_SETN_ViewMenu,25, "Ctrl+Insert");
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,23, "C"));
	button = _DtTermViewCreatePushButton(pulldown[pc],
		  (GETMESSAGE(NL_SETN_ViewMenu,22, "Copy")),
		  ks,
		  accelerator,
		  acceleratorText,
                  copyClipboardCallback, NULL);
	(void) XtFree(accelerator);
	(void) XtFree(acceleratorText);

        ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,27, "P"));
        accelerator = mallocGETMESSAGE(NL_SETN_ViewMenu,28,
		"Shift <Key>osfInsert");
        acceleratorText = mallocGETMESSAGE(NL_SETN_ViewMenu,29, "Shift+Insert");
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	          (GETMESSAGE(NL_SETN_ViewMenu,26, "Paste")),
		  ks,
		  accelerator,
		  acceleratorText,
                  pasteClipboardCallback, NULL);
	(void) XtFree(accelerator);
	(void) XtFree(acceleratorText);
    }
    if (menuBar || (!menuBar && firstPopup))
#ifdef	PULLDOWN_ACCELERATORS
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,31, "E"));
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	          (GETMESSAGE(NL_SETN_ViewMenu,30, "Edit")),
		  ks,
		  NULL, NULL, NULL, NULL);
#else	/* PULLDOWN_ACCELERATORS */
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,30, "Edit")),
		NoSymbol, NULL, NULL, NULL, NULL);
#endif	/* PULLDOWN_ACCELERATORS */

    (void) pc++;
    if (first) {
	i = 0;
	pulldown[pc] = createPulldown(topLevel, "Options", arglist, i);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,33, "M"));
	menuBarToggle = _DtTermViewCreateToggleButton(pulldown[pc], 
	                (GETMESSAGE(NL_SETN_ViewMenu,32, "Menu Bar")), 
		         ks,
			 NULL, NULL, menuBarToggleCallback, NULL);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,35, "S"));
	scrollBarToggle = _DtTermViewCreateToggleButton(pulldown[pc], 
	                 (GETMESSAGE(NL_SETN_ViewMenu,34, "Scroll Bar")),
		          ks,
			  NULL, NULL, scrollBarToggleCallback, NULL);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,37, "G"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,36, "Global...")),
		  ks,
		  NULL, NULL, globalOptionsCallback, NULL);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,39, "T"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	          (GETMESSAGE(NL_SETN_ViewMenu,38, "Terminal...")),
		  ks,
		  NULL, NULL, terminalOptionsCallback, NULL);

	(void) createSizeMenu(termView, pulldown[pc]);

	(void) createFontMenu(termView, pulldown[pc]);

	i = 0;
	submenu = createPulldown(pulldown[pc], "Reset", arglist, i);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,41, "S"));
	button = _DtTermViewCreatePushButton(submenu, 
	         (GETMESSAGE(NL_SETN_ViewMenu,40, "Soft Reset")),
		 ks,
		NULL, NULL, softResetCallback, NULL);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,43, "H"));
	button = _DtTermViewCreatePushButton(submenu, 
	         (GETMESSAGE(NL_SETN_ViewMenu,42, "Hard Reset")),
		  ks,
		  NULL, NULL, hardResetCallback, NULL);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,45, "R"));
	(void) _DtTermViewCreateCascadeButton(pulldown[pc], submenu, 
	        (GETMESSAGE(NL_SETN_ViewMenu,44, "Reset")),
		ks,
		NULL, NULL, NULL, NULL);
    }
    if (menuBar || (!menuBar && firstPopup))
#ifdef	PULLDOWN_ACCELERATORS
        ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,47, "O"));
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	          (GETMESSAGE(NL_SETN_ViewMenu,46, "Options")),
		   ks,
		   NULL, NULL, NULL, NULL);
#else	/* PULLDOWN_ACCELERATORS */
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	          (GETMESSAGE(NL_SETN_ViewMenu,46, "Options")),
		NoSymbol, NULL, NULL, NULL, NULL);
#endif	/* PULLDOWN_ACCELERATORS */

    (void) pc++;
    if (first) {
	i = 0;
	pulldown[pc] = createPulldown(topLevel, "Help", arglist, i);

#ifdef	HPVUE
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,49, "O"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,48, "Overview")),
		 ks,
		 NULL, NULL, helpIntroCallback, NULL);

	button = _DtTermViewCreateSeparator(pulldown[pc], "Separator");

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,51,"T"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,50, "Tasks")),
		 ks,
		 NULL, NULL, helpTasksCallback, NULL);
	(void) XtSetSensitive(button, False);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,53,"R"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,52, "Reference")),
		 ks,
		 NULL, NULL, helpReferenceCallback, NULL) ;

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,55,"O"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,54, "On Item")),
		 ks,
		 NULL, NULL, NULL, NULL);
	(void) XtSetSensitive(button, False);

	button = _DtTermViewCreateSeparator(pulldown[pc], "Separator");

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,57,"U"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,56, "Using Help")),
		ks,
		NULL, NULL, helpOnHelpCallback, NULL);

	button = _DtTermViewCreateSeparator(pulldown[pc], "Separator");

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,59,"A"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,82, "About Terminal")),
		ks,
		NULL, NULL, helpVersionCallback, NULL);
    }

    if (menuBar || (!menuBar && firstPopup))
#ifdef	PULLDOWN_ACCELERATORS
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,61,"H"));
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,60, "Help")),
		ks,
		NULL, NULL, NULL, NULL);
#else	/* PULLDOWN_ACCELERATORS */
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,60, "Help")),
		NoSymbol, NULL, NULL, NULL, NULL);
#endif	/* PULLDOWN_ACCELERATORS */


#else	/* HPVUE */
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,65, "v"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,64, "Overview")),
		 ks,
		 NULL, NULL, helpOverviewCallback, NULL);

	button = _DtTermViewCreateSeparator(pulldown[pc], "Separator");

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,69,"C"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,68, "Table Of Contents")),
		 ks,
		 NULL, NULL, helpTableOfContentsCallback, NULL);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,71,"T"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,70, "Tasks")),
		 ks,
		 NULL, NULL, helpTasksCallback, NULL);

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,73,"R"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,72, "Reference")),
		 ks,
		 NULL, NULL, helpReferenceCallback, NULL) ;

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,75,"K"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,74, "Keyboard")),
		 ks,
		 NULL, NULL, helpKeyboardCallback, NULL) ;

	button = _DtTermViewCreateSeparator(pulldown[pc], "Separator");

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,77,"U"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,76, "Using Help")),
		ks,
		NULL, NULL, helpUsingHelpCallback, NULL);

	button = _DtTermViewCreateSeparator(pulldown[pc], "Separator");

	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,79,"A"));
	button = _DtTermViewCreatePushButton(pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,83, "About Terminal")),
		ks,
		NULL, NULL, helpAboutDttermCallback, NULL);
    }

    if (menuBar || (!menuBar && firstPopup))
#ifdef	PULLDOWN_ACCELERATORS
	ks = XStringToKeysym(GETMESSAGE(NL_SETN_ViewMenu,81,"H"));
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	         (GETMESSAGE(NL_SETN_ViewMenu,80, "Help")),
		ks,
		NULL, NULL, NULL, NULL);
#else	/* PULLDOWN_ACCELERATORS */
	cascade = _DtTermViewCreateCascadeButton(menu, pulldown[pc], 
	        (GETMESSAGE(NL_SETN_ViewMenu,80, "Help")),
		NoSymbol, NULL, NULL, NULL, NULL);
#endif	/* PULLDOWN_ACCELERATORS */
#endif	/* HPVUE */
    if (menuBar) {
	/* this is the help button... */
	i = 0;
	(void) XtSetArg(arglist[i], XmNmenuHelpWidget, cascade); i++;
	(void) XtSetValues(menu, arglist, i);
    }

    if (!menuBar && firstPopup)
	firstPopup = False;

    if (first)
	first = False;

    return(menu);
}

Widget
_DtTermViewCreatePulldownMenu
(
    Widget		  termView,
    Widget		  parent,
    Arg		 	  menuArglist[],
    int			  menuArgcount
)
{
    Widget w;
    
    _DtTermProcessLock();
    w = CreateMenu(termView, parent, True, menuArglist, menuArgcount);
    _DtTermProcessUnlock();
    return(w);
}

Widget
_DtTermViewCreatePopupMenu
(
    Widget		  termView,
    Widget		  parent,
    Arg			  menuArglist[],
    int			  menuArgcount
)
{
    static Widget popupMenu = (Widget) 0;	/* popup widget...	*/

    _DtTermProcessLock();
    if (!popupMenu) {
	popupMenu = CreateMenu(termView, parent, False,
		menuArglist, menuArgcount);
    }
    _DtTermProcessUnlock();

    /* set up a handler to post the menu... */
    (void) XtAddEventHandler(parent, ButtonPressMask, False,
	    postMenu, (XtPointer) popupMenu);
    /* and add us to the postfrom list... */
    (void) XmAddToPostFromList(popupMenu, parent);
    /* set up a callback to remove the post from on destruction... */
    (void) XtAddCallback(parent, XmNdestroyCallback,
	    removeFromPostFromListCallback, (XtPointer) popupMenu);
    return(popupMenu);
}

/*ARGSUSED*/
static void
setContext(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget	  tw;
    Arg arglist[20];
    int i;
    Boolean menuBarState;
    Boolean scrollBarState;
#ifdef	WINDOW_SIZE_TOGGLES
    short rows;
    short columns;
#endif	/* WINDOW_SIZE_TOGGLES */

    _DtTermProcessLock();
    /* get the widget we were posted from...  */
    while (w && !XmIsRowColumn(w)) {
	w = XtParent(w);
    }
    if (!w) {
        _DtTermProcessUnlock();
	return;
    }
    w = XmGetPostedFromWidget(w);
    /* walk up the tree until we find an DtTermView widget... */
    while (w && !XtIsShell(w) && !XtIsSubclass(w, dtTermViewWidgetClass)) {
	w = XtParent(w);
    }
    if (!w) {
        _DtTermProcessUnlock();
	return;
    }
    /* make sure it is an DtTermView widget... */
    if (!XtIsSubclass(w, dtTermViewWidgetClass)) {
	/* problem... */
	(void) fprintf(stderr,
		"unable to find dtTermViewWidgetClass parent for menu\n");
        _DtTermProcessUnlock();
	return;
    }

    tw = (DtTermViewWidget) w;
    currentWidget = w;

    /* update the menu toggle buttons... */
    i = 0;
    (void) XtSetArg(arglist[i], DtNscrollBar, &scrollBarState); i++;
    (void) XtSetArg(arglist[i], DtNmenuBar, &menuBarState); i++;
#ifdef	WINDOW_SIZE_TOGGLES
    (void) XtSetArg(arglist[i], DtNrows, &rows); i++;
    (void) XtSetArg(arglist[i], DtNcolumns, &columns); i++;
#endif	/* WINDOW_SIZE_TOGGLES */
    (void) XtGetValues(w, arglist, i);

    /* set the toggles... */
    (void) XmToggleButtonGadgetSetState(scrollBarToggle, scrollBarState, False);
    (void) XmToggleButtonGadgetSetState(menuBarToggle, menuBarState, False);

    /* set the font toggles...
     */
    for (i = 0; i < fontSizeTogglesDefault; i++) {
	if (tw->termview.currentFontToggleButtonIndex == i) {
	    (void) XmToggleButtonGadgetSetState(fontSizeToggles[i],
		    True, False);
	} else {
	    (void) XmToggleButtonGadgetSetState(fontSizeToggles[i],
		    False, False);
	}
    }
    if ((tw->termview.currentFontToggleButtonIndex >= fontSizeTogglesDefault) ||
	    (tw->termview.currentFontToggleButtonIndex < 0)) {
	(void) XmToggleButtonGadgetSetState(fontSizeToggles[i],
		True, False);
    } else {
	(void) XmToggleButtonGadgetSetState(fontSizeToggles[i],
		False, False);
    }

#ifdef	WINDOW_SIZE_TOGGLES
    /* set the window size toggles...
     */
    for (i = 0; i < tw->termview.sizeList.numSizes; i++) {
	if (((tw->termview.sizeList.sizes[i].rows <= 0) ||
		(rows == tw->termview.sizeList.sizes[i].rows)) &&
		((tw->termview.sizeList.sizes[i].columns <= 0) ||
		(columns == tw->termview.sizeList.sizes[i].columns))) {
	    (void) XmToggleButtonGadgetSetState(windowSizeToggles[i],
		    True, False);
	} else {
	    (void) XmToggleButtonGadgetSetState(windowSizeToggles[i],
		    False, False);
	}
    }
    if (((tw->termview.sizeDefault.rows <= 0) ||
	    (tw->termview.sizeDefault.rows == rows)) &&
	    ((tw->termview.sizeDefault.columns <= 0) ||
	    (tw->termview.sizeDefault.columns == columns))) {
	(void) XmToggleButtonGadgetSetState(windowSizeToggles[i],
		True, False);
    } else {
	(void) XmToggleButtonGadgetSetState(windowSizeToggles[i],
		False, False);
    }
#endif	/* WINDOW_SIZE_TOGGLES */

    /* set the sensitivity on the new button... */
    (void) XtSetSensitive(newButton, DtTermViewGetCloneEnabled(w));
    _DtTermProcessUnlock();
}


/*************************************************************************
 *
 *  menu callbacks
 */
/*ARGSUSED*/
static void
exitCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    /* walk up the widget tree to find the shell... */

    _DtTermProcessLock();
    w = currentWidget;
    _DtTermProcessUnlock();

    /* invoke the callbacks for this terminal emulator... */
    (void) XtCallCallbacks(w, DtNsubprocessTerminationCallback, (XtPointer) 0);

#ifdef	NOTDEF
    /* make sure things are dead by destroying the interface... */
    while (w && !XtIsShell(w)) {
	w = XtParent(w);
    }

    /* destroy the interface... */
    (void) XtDestroyWidget(w);
#endif	/* NOTDEF */
}

/*ARGSUSED*/
static void
scrollBarToggleCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg arglist;
    XmToggleButtonCallbackStruct *cb =
	    (XmToggleButtonCallbackStruct *) call_data;

    _DtTermProcessLock();
    (void) XtSetArg(arglist, DtNscrollBar, cb->set);
    (void) XtSetValues(currentWidget, &arglist, 1);
    _DtTermProcessUnlock();
}

void
_DtTermViewMenuToggleMenuBar
(
    Widget		  w
)
{
    Arg al;
    Boolean toggle;

    (void) XtSetArg(al, DtNmenuBar, &toggle);
    (void) XtGetValues(w, &al, 1);

    (void) XtSetArg(al, DtNmenuBar, !toggle);
    (void) XtSetValues(w, &al, 1);
}

/*ARGSUSED*/
static void
menuBarToggleCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg arglist;
    XmToggleButtonCallbackStruct *cb =
	    (XmToggleButtonCallbackStruct *) call_data;

    _DtTermProcessLock();
    (void) XtSetArg(arglist, DtNmenuBar, cb->set);
    (void) XtSetValues(currentWidget, &arglist, 1);
    _DtTermProcessUnlock();
}

/*ARGSUSED*/
static void
cloneCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) DtTermViewCloneCallback(cw, client_data, call_data);
}

#ifdef	HPVUE
/*ARGSUSED*/
static void
helpVersionCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "_copyright");
}

/*ARGSUSED*/
static void
helpIntroCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "_hometopic");
}

/*ARGSUSED*/
static void
helpTasksCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "UsingTermEmulators");
}

/*ARGSUSED*/
static void
helpReferenceCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "dtterm1x");
}

/*ARGSUSED*/
static void
helpOnHelpCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Help4Help", "_hometopic");
}
#else	/* HPVUE */

/*ARGSUSED*/
static void
helpOverviewCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
     Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

   (void) _DtTermViewMapHelp(cw, "Terminal", "_hometopic");
}

/*ARGSUSED*/
static void
helpTableOfContentsCallback(Widget w, XtPointer client_data,
	XtPointer call_data)
{
     Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "TableOfContents");
}

/*ARGSUSED*/
static void
helpTasksCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
     Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "Tasks");
}

/*ARGSUSED*/
static void
helpReferenceCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
     Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "Reference");
}

/*ARGSUSED*/
static void
helpKeyboardCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
     Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "Keyboard");
}

/*ARGSUSED*/
static void
helpUsingHelpCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
     Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Help4Help", "_hometopic");
}

/*ARGSUSED*/
static void
helpAboutDttermCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
     Widget cw;

    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermViewMapHelp(cw, "Terminal", "_copyright");
}
#endif	/* HPVUE */

static void
raiseDialog(Widget w)
{
    while (w && !XtIsShell(w)) {
	w = XtParent(w);
    }

    if (w) {
	(void) XRaiseWindow(XtDisplay(w), XtWindow(w));
    }
}

/*ARGSUSED*/
static void
globalOptionsCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;
    _DtTermProcessUnlock();

    if (tw->termview.globalOptionsDialog) {
	(void) XtManageChild(tw->termview.globalOptionsDialog);
	(void) raiseDialog(tw->termview.globalOptionsDialog);
    } else {
	tw->termview.globalOptionsDialog =
		_DtTermViewCreateGlobalOptionsDialog((Widget)tw);
	(void) raiseDialog(tw->termview.globalOptionsDialog);
    }
}

/*ARGSUSED*/
static void
terminalOptionsCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;
    _DtTermProcessUnlock();

    if (tw->termview.terminalOptionsDialog) {
	(void) XtManageChild(tw->termview.terminalOptionsDialog);
	(void) raiseDialog(tw->termview.terminalOptionsDialog);
    } else {
	tw->termview.terminalOptionsDialog =
		_DtTermViewCreateTerminalOptionsDialog((Widget) tw);
	(void) raiseDialog(tw->termview.terminalOptionsDialog);
    }
}

/*ARGSUSED*/
static void
sizeChangeCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;
    Arg al[10];
    int ac;
    long i1;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;
    _DtTermProcessUnlock();

    i1 = (long) client_data;
    ac = 0;
    if (tw->termview.sizeList.sizes[i1].columns > 0) {
	(void) XtSetArg(al[ac], DtNcolumns,
		tw->termview.sizeList.sizes[i1].columns); ac++;
    }
    if (tw->termview.sizeList.sizes[i1].rows > 0) {
	(void) XtSetArg(al[ac], DtNrows,
		tw->termview.sizeList.sizes[i1].rows); ac++;
    }
    if (ac > 0) {
	(void) XtSetValues((Widget) tw, al, ac);
    }
}

/*ARGSUSED*/
static void
defaultSizeCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;
    Arg al[10];
    int ac;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;
    _DtTermProcessUnlock();

    ac = 0;
    if ((tw->termview.sizeDefault.columns > 0) &&
	    (tw->termview.sizeDefault.rows > 0)) {
	(void) XtSetArg(al[ac], DtNcolumns, tw->termview.sizeDefault.columns);
		ac++;
	(void) XtSetArg(al[ac], DtNrows, tw->termview.sizeDefault.rows); ac++;
	(void) XtSetValues((Widget) tw, al, ac);
    }
}

/*ARGSUSED*/
static void
fontChangeCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;
    XrmValue from;
    XrmValue to;
    Arg al[10];
    int ac;
    long i1;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;

    i1 = (long) client_data;

    /* generate the fontlist from the font... */
    from.size = strlen(fontArray[i1].fontName);
    from.addr = (XtPointer) fontArray[i1].fontName;
    to.size = sizeof(fontArray[i1].fontList);
    to.addr = (XtPointer) &fontArray[i1].fontList;
    if (!XtConvertAndStore((Widget) tw, XmRString, &from, XmRFontList,
	    &to)) {
	/* Unable to convert to a fontlist.  For now, let's just
	 * ignore it and return...
	 */
        _DtTermProcessUnlock();
	return;
    }

    if (tw->termview.userFontName)
        XtFree(tw->termview.userFontName);
    tw->termview.userFontName = XtNewString(fontArray[i1].fontName);

    if (tw->termview.fontList != fontArray[i1].fontList) {
	ac = 0;
	(void) XtSetArg(al[ac], DtNuserFont, fontArray[i1].fontList); ac++;
	(void) XtSetValues((Widget) tw, al, ac);
    }
    tw->termview.currentFontToggleButtonIndex = i1;
    _DtTermProcessUnlock();
}

/*ARGSUSED*/
static void
defaultFontCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;
    Arg al[10];
    int ac;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;
    _DtTermProcessUnlock();

    if (tw->termview.userFontName)
	    XtFree(tw->termview.userFontName);
    tw->termview.userFontName = NULL;

    if (tw->termview.fontList != tw->termview.defaultFontList) {
	ac = 0;
	(void) XtSetArg(al[ac], DtNuserFont, tw->termview.defaultFontList);
		ac++;
	(void) XtSetValues((Widget) tw, al, ac);
    }
    tw->termview.currentFontToggleButtonIndex = fontSizeTogglesDefault;
}
/*ARGSUSED*/
static void
softResetCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermFuncSoftReset(_DtTermViewGetChild((Widget) tw,
	    DtTermTERM_WIDGET), 1, fromMenu);
}

/*ARGSUSED*/
static void
hardResetCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    DtTermViewWidget tw;

    _DtTermProcessLock();
    tw = (DtTermViewWidget) currentWidget;
    _DtTermProcessUnlock();

    (void) _DtTermFuncHardReset(_DtTermViewGetChild((Widget) tw,
	    DtTermTERM_WIDGET), 1, fromMenu);
}

/*ARGSUSED*/
static void
copyClipboardCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget tw, cw;

    /* since this may have been called from an accelerator, we need
     * to insure that our context was set...
     */
    (void) setContext(w, client_data, call_data);
    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();
    tw = _DtTermViewGetChild(cw, DtTermTERM_WIDGET);
    (void) _DtTermPrimSelectCopyClipboard(tw,
                     XtLastTimestampProcessed(XtDisplay(tw)));
}

/*ARGSUSED*/
static void
pasteClipboardCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget tw, cw;

    /* since this may have been called from an accelerator, we need
     * to insure that our context was set...
     */
    (void) setContext(w, client_data, call_data);
    _DtTermProcessLock();
    cw = currentWidget;
    _DtTermProcessUnlock();
    tw = _DtTermViewGetChild(cw, DtTermTERM_WIDGET);
    (void) _DtTermPrimSelectPasteClipboard(tw);
}

int
_DtTermViewGetUserFontListIndex
(
    Widget		  w
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;

    /* use 0 for default, index + 1 for specific buttons... */
    if (tw->termview.currentFontToggleButtonIndex == fontSizeTogglesDefault) {
	return(0);
    } else {
	return(tw->termview.currentFontToggleButtonIndex + 1);
    }
}

void
_DtTermViewSetUserFontListIndex
(
    Widget		  w,
    long		  i1
)
{
    DtTermViewWidget	  tw = (DtTermViewWidget) w;
    _DtTermWidgetToAppContext(w);

    _DtTermAppLock(app);

    /* map from 0 for default, index + 1 for specific buttons to currect
     * desired button...
     */
    if (i1 == 0) {
	i1 = fontSizeTogglesDefault;
    } else {
	i1 -= 1;
    }

    /* range check... */
    if ((i1 < 0) || (i1 > fontSizeTogglesDefault)) {
	i1 = fontSizeTogglesDefault;
    }

    /* set up for the callback... */
    _DtTermProcessLock();
    currentWidget = w;
    if (i1 == fontSizeTogglesDefault) {
	(void) defaultFontCallback(w, (XtPointer) i1, (XtPointer) 0);
    } else {
	(void) fontChangeCallback(w, (XtPointer) i1, (XtPointer) 0);
    }
    _DtTermProcessUnlock();
    _DtTermAppUnlock(app);
}

