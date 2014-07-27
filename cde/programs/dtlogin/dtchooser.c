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
/* $TOG: dtchooser.c /main/8 1998/04/20 12:56:18 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 */

 /****************************************************************************
 **
 **   File:        dtchooser.c
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Main line code for Dtgreet application
 **
 **                These routines initialize the toolkit, create the widgets,
 **		   set up callbacks, and wait for events.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/



/***************************************************************************
 *
 *  Includes
 *
 ***************************************************************************/

#include	<stdio.h>
#include        <setjmp.h>
#include	<stdlib.h>
#include	<unistd.h>
#include        <sys/signal.h>
#include        <sys/stat.h>
#include	<locale.h>

#include	<Xm/Xm.h>
#include	<X11/Shell.h>
#include        <X11/cursorfont.h>
#include	<Xm/DragC.h>
#include	<Xm/DrawingA.h>
#include	<Xm/Frame.h>
#include	<Xm/Form.h>
#include	<Xm/Label.h>
#include	<Xm/LabelG.h>
#include	<Xm/List.h>
#include        <Xm/Text.h>
#include	<Xm/TextF.h>
#include	<Xm/PushB.h>
#include	<Xm/PushBG.h>
#include	<Xm/MessageB.h>
#include	<Xm/RowColumn.h>
#include	<Xm/SeparatoG.h>
#include	<Xm/ToggleBG.h>
#include	<Xm/CascadeBG.h>
#include	<Dt/EnvControlP.h>
#include	"vg.h"
#include	"vgmsg.h"
#include        <Dt/MenuButton.h>

#ifdef USE_XINERAMA
#include <DtXinerama.h>
#endif


/***************************************************************************
 *
 *  External declarations
 *
 ***************************************************************************/
extern char    password[];  /* pswd string value */
extern int     password_length;  /* pswd string length */

extern void DoAccept (Widget w, XtPointer client_data, XtPointer call_data);
extern void DoPing (Widget w, XtPointer client_data, XtPointer call_data);

/***************************************************************************
 *
 *  Procedure declarations
 *
 ***************************************************************************/

static SIGVAL syncTimeout( int arg ) ;
static Widget InitToolKit( int argc, char **argv) ;
void MakeRootCursor( void ) ;
void MakeBackground( void ) ;
void MakeButtons( void ) ;
void MakeLogin( void ) ;
void MakeOptionsProc( XtPointer data, XtIntervalId *id) ;
static int  ErrorHandler( Display *dpy, XErrorEvent *event) ;
static SIGVAL Terminate( int arg ) ;
extern void DoCancel (Widget w, XtPointer client_data, XtPointer call_data);





/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/


AppInfo		appInfo;	/* application resources		   */
Arg		argt[100];	/* used for resources			   */
DisplayInfo	dpyinfo;	/* information about the display	   */
XmString	xmstr;		/* used for compound strings		   */
char		*langenv;	/* current value of LANG environment var.  */
char		*errorLogFile;	/* current value of environment var.	   */
int		showVerifyError;/* display a Verify() error dialog	   */
int		fullScreen = 0;	/* Obscure the entire screen.              */


/******************************************************************************
**
**      WIDGET LAYOUT
**
** toplevel                 "main"                      (toplevel)
**  login_shell              "login_shell"              (overrideShell)
**   table                    "table"                   (DrawingAreaWidget)
**     copyright_msg            "copyright_msg"         (MessageBox)
**     error_message            "error_message"         (MessageBox)
**     help_message             "help_message"          (MessageBox)
**     passwd_message           "passwd_message"        (MessageBox)
**     hostname_message         "hostname_msg"          (MessageBox)
**     matte                   "matte"                  (FormWidget)
**      logo                   "logo"                   (FrameWidget)
**       logo_pixmap            "logo_pixmap"           (LabelGadget)
**      matteFrame         "matteFrame"         (FrameWidget)
**       matte1                "matte1"                 (FormWidget)
**        dt_label                "dt_label"            (LabelGadget)
**        help_button             "help_button"         (PushButtonGadget)
**        greeting               "greeting"             (LabelGadget)
**        login_form               "login_form"         (FormWidget)
**          login_label              "login_label"      (LabelGadget)
**          login_text               "login_text"       (TextField)
**        ok_button               "ok_button"           (PushButtonGadget)
**        clear_button            "clear_button"        (PushButtonGadget)
**        options_button                  "options_button"      (PushButton)
**          options_menu                    "options_menu"      (PopupMenu)
**            options_item[0]         "options_languages"    (CascadeButtonGadget)
**            options_item[1]         "options_sep2"         (SeparatorGadget)
**            options_item[2]         "session_menus"        (CascadeButtonGadget)
**            options_item[3]         "options_sep1"         (SeparatorGadget)
**            options_item[4]         "options_noWindows"    (PushButtonGadget)
**            options_item[5]         "options_restartServer"(PushButtonGadget)
**            options_item[6]         "options_sep1"         (SeparatorGadget)
**            options_item[7]         "options_Copyright"    (PushButtonGadget)
**            session_menu            "session_menu"         (PulldownMenu)
**              options_dt            "options_dt"             (ToggleButtonGadget)
**              options_failsafe      "options_failsafe"       (ToggleButtonGadget)
**            lang_menu               "lang_menu"       (PulldownMenu)
**           (lang items)               (lang items)    (ToggleButtonGadget)
**      ...
**
*/


extern Widget toplevel;		/* top level shell widget		   */
Widget login_shell;		/* shell for the main login widgets.	   */
Widget table;			/* black background for everything	   */
Widget matte;			/* main level form widget		   */
Widget matteFrame;		/* main level form widget		   */
Widget matte1;			/* second level form widget		   */

Widget greeting;		/* Welcome message			   */

Widget logo1;			/* frame around the Corporate logo	   */
Widget logo_pixmap;		/* Corporate logo			   */
Widget logo_shadow;		/* drop shadow under the Corporate logo	   */

Widget login_matte;		/* bulletin board for login/password	   */
Widget dt_label;                /* desktop label showing the current session */
Widget login_form = NULL;	/* form containing the login widgets	   */
Widget login_label;		/* label to left of login text widget	   */
Widget login_text;		/* login text widget			   */

Widget ok_button;		/* accept name/password text button	   */
Widget clear_button;		/* clear name/password text button	   */
Widget options_button;		/* login options button			   */
Widget help_button;		/* help button				   */

Widget copyright_msg    = NULL;	/* copyright notice widget		   */
Widget help_message     = NULL;	/* the help message box			   */
Widget error_message    = NULL;	/* the error message box		   */
Widget hostname_message = NULL;	/* the invalid hostname message box	   */
Widget passwd_message   = NULL;	/* the expired password message box	   */

Widget options_menu = NULL;	/* pop-up menu on options button	   */
Widget options_item[10];	/* items on options pop_up menu	  	   */
Widget options_nowindows;	/* nowindows pane on options pop_up menu   */
Widget options_failsafe;	/* failsafe pane on options pop_up menu	   */
Widget options_dtlite;		/* dtlite  pane on options pop_up menu	   */
Widget options_dt;		/* dt regular pane on options pop_up menu  */

Widget *alt_dts;
Widget options_sp_dt;           /* dt button for special desktop          */
Widget options_last_dt;         /* user's last dt                         */



Widget lang_menu = NULL;	/* cascading menu on "Language" option	   */
Widget session_menu = NULL;     /* cascading menu on "Session" option      */

Widget chooser_list;		/* list of hosts displayed by chooser      */
Widget list_head;		/* the heading for the chooser list        */



static
    XtResource AppResources[] = {
    { "workspaceCursor", "WorkspaceCursor", 
	XtRBoolean, sizeof(Boolean), XtOffset(AppInfoPtr, workspaceCursor),
	XtRImmediate, (caddr_t)False					},

    { "labelFont", "LabelFont", 
	XmRFontList, sizeof(XmFontList), XtOffset(AppInfoPtr, labelFont),
	XmRString, "Fixed"						},

    { "textFont", "TextFont", 
	XmRFontList, sizeof(XmFontList), XtOffset(AppInfoPtr, textFont),
	XmRString, "Fixed"						},

    { "chlistFont", "ChlistFont",
        XmRFontList, sizeof(XmFontList), XtOffset(AppInfoPtr, chlistFont),
        XmRString, "Fixed"                                              },

    { "optionsDelay", "OptionsDelay", 
	XtRInt, sizeof(int), XtOffset(AppInfoPtr, optionsDelay),
	XtRImmediate, (XtPointer) 0					},

    {"languageList", "LanguageList",
	XtRString, sizeof(char *), XtOffset(AppInfoPtr, languageList),
	XtRString, NULL							},

#if defined(USE_XINERAMA)
    { "xineramaPreferredScreen",        "XineramaPreferredScreen",
        XtRInt, sizeof(int), XtOffset(AppInfoPtr, xineramaPreferredScreen),
        XtRImmediate, (XtPointer) 0
        },
#endif

#if defined (ENABLE_DYNAMIC_LANGLIST)
    {"languageListCmd", "LanguageListCmd",
        XtRString, sizeof(char *), XtOffset(AppInfoPtr, languageListCmd),
        XtRString, NULL                                                 },
#endif /* ENABLE_DYNAMIC_LANGLIST */

    };








/***************************************************************************
 *
 *  MakeRootCursor
 *
 *  Widgets: none
 ***************************************************************************/

void 
MakeRootCursor( void )
{
	Cursor vg_cursor;

	vg_cursor = XCreateFontCursor (dpyinfo.dpy, XC_left_ptr);

	XDefineCursor (dpyinfo.dpy, dpyinfo.root, vg_cursor);

	return;
}

/***************************************************************************
 *
 *  MakeBackground
 *
 *  Widgets: login_shell, table, matte
 ***************************************************************************/

void 
MakeBackground( void )
{
    register int i;

    /*
     * Get resources first
     */
    SetResourceDatabase();
    XtGetApplicationResources(toplevel, &appInfo, AppResources,
                                XtNumber(AppResources), NULL, 0);
    
    
    /* 
     * create the login shell widget...
     */

    i = 0;

    /*		CORE resource set					*/
    XtSetArg(argt[i], XmNancestorSensitive,	True			); i++;
    XtSetArg(argt[i], XmNbackgroundPixmap,	XmUNSPECIFIED_PIXMAP	); i++;
    XtSetArg(argt[i], XmNborderWidth,		0			); i++;
    XtSetArg(argt[i], XmNmappedWhenManaged,	False			); i++;
    XtSetArg(argt[i], XmNsensitive,		True			); i++;
    XtSetArg(argt[i], XmNtranslations,		NULL			); i++;

    /*		COMPOSITE resource set					*/
    XtSetArg(argt[i], XmNinsertPosition,	NULL			); i++;

    /*		SHELL resource set (set to avoid interference by user)	*/
    XtSetArg(argt[i], XmNallowShellResize,	False			); i++;
    XtSetArg(argt[i], XmNcreatePopupChildProc,	NULL			); i++;
    XtSetArg(argt[i], XmNgeometry,		NULL			); i++;
    XtSetArg(argt[i], XmNpopupCallback,		NULL			); i++;
    XtSetArg(argt[i], XmNpopdownCallback,	NULL			); i++;
    XtSetArg(argt[i], XmNoverrideRedirect,	False			); i++;
    XtSetArg(argt[i], XmNsaveUnder,		False			); i++;

    login_shell = XtCreatePopupShell("login_shell", transientShellWidgetClass,
				     toplevel, argt, i);
    XtAddCallback(login_shell, XmNpopupCallback, LayoutCB, NULL);

    /* Fix to display Input Method's status area. */
    XtSetArg(argt[0], XmNheight, dpyinfo.height);
    XtSetValues(login_shell, argt, 1);


    /* 
     * create the full-screen drawing area...
     */

    i = InitArg(DrawingA);
    XtSetArg(argt[i], XmNwidth,			dpyinfo.width		); i++;
    XtSetArg(argt[i], XmNheight,		dpyinfo.height		); i++;
    XtSetArg(argt[i], XmNunitType,		XmPIXELS		); i++;

    table = XtCreateManagedWidget("table", xmDrawingAreaWidgetClass,
				   login_shell, argt, i);

    XtAddEventHandler(table, ButtonPressMask, False, RefreshEH, NULL);



    /* 
     * create the main matte...
     */

    i = InitArg(Form);
    /*		      XmNwidth,			(set by user)		*/
    /*		      XmNheight,		(set by user)		*/
    XtSetArg(argt[i], XmNshadowThickness,       SHADOW_THICKNESS        ); i++;
/*
    XtSetArg(argt[i], XmNshadowType,	XmSHADOW_OUT	); i++;
    XtSetArg(argt[i], XmNshadowThickness,	5	); i++;
*/

    matte = XmCreateForm(table, "matte", argt, i);
    XtManageChild(matte);

    i = 0;
	XtSetArg(argt[i], XmNshadowType, XmSHADOW_OUT); i++;
	XtSetArg(argt[i], XmNshadowThickness, 2); i++;
	XtSetArg(argt[i], XmNtopAttachment, XmATTACH_FORM); i++;
	XtSetArg(argt[i], XmNbottomAttachment, XmATTACH_FORM); i++;
	XtSetArg(argt[i], XmNleftAttachment, XmATTACH_FORM); i++;
	/*
	XtSetArg(argt[i], XmNrightAttachment, XmATTACH_FORM); i++;
	*/
	XtSetArg(argt[i], XmNtopOffset, 15); i++;
	XtSetArg(argt[i], XmNbottomOffset, 15); i++;
	XtSetArg(argt[i], XmNleftOffset, 15); i++;
	/*
	XtSetArg(argt[i], XmNrightOffset, 15); i++;
	*/
	matteFrame = XmCreateFrame(matte, "matteFrame", argt, i);
    XtManageChild(matteFrame);

	i = 0;
	matte1 = XmCreateForm(matteFrame, "matte1", argt, i);
    XtManageChild(matte1);
	
}




/***************************************************************************
 *
 *  MakeButtons
 *
 *  Widgets:	ok_button, clear_button, options_button, help_button
 ***************************************************************************/

void 
MakeButtons( void )
{
    register int i;

    Dimension	width;

    Dimension	max_width;	/* maximum width  of a set of widgets	   */
    Dimension	max_height;	/* maximum height of a set of widgets	   */

    int		origin;		/* horizontal origin for button placement  */
    int		spacing;	/* spacing between buttons (width/32)      */

     
    /* 
     * create the buttons...
     */

    /* ok button */
    
    i = InitArg(PushBG);
    XtSetArg(argt[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(argt[i], XmNbottomPosition, 95); i++;
    XtSetArg(argt[i], XmNtraversalOn, True); i++;
    XtSetArg(argt[i], XmNleftAttachment,   XmATTACH_FORM);	i++;

    xmstr = ReadCatalogXms(MC_LABEL_SET, MC_OK_LABEL, MC_DEF_OK_LABEL );
    XtSetArg(argt[i], XmNlabelString,			xmstr		); i++;

    ok_button = XmCreatePushButtonGadget(matte1, "ok_button", argt, i);

    XmStringFree(xmstr);
    XtManageChild(ok_button);

    XtAddCallback(ok_button, XmNactivateCallback, DoAccept, NULL);    


    /* clear button */

    i -= 2;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(argt[i], XmNleftWidget,		ok_button);		i++;
    xmstr = ReadCatalogXms(MC_CHOOSER_SET, MC_UPDATE_LABEL, MC_DEF_UPDATE_LABEL);
    XtSetArg(argt[i], XmNlabelString,		xmstr); 		i++;

    clear_button = XmCreatePushButtonGadget(matte1, "clear_button", argt, i);

    XmStringFree(xmstr);
    XtManageChild(clear_button);
    XtAddCallback(clear_button, XmNactivateCallback, DoPing,
                  (XtPointer) 0);   



    /* help button */
    
    i -= 3;
    xmstr = ReadCatalogXms(MC_LABEL_SET, MC_HELP_LABEL, MC_DEF_HELP_LABEL);
    XtSetArg(argt[i], XmNlabelString,			xmstr		); i++;

    help_button = XmCreatePushButtonGadget(matte1, "help_button", argt, i);
    XtAddCallback(help_button, XmNactivateCallback, ShowDialogCB, 
    		  (XtPointer) help_chooser);
    XmStringFree(xmstr);
    XtManageChild(help_button);


    i = InitArg(Label);
    XtSetArg(argt[i], XmNbottomAttachment, XmATTACH_POSITION); i++;
    XtSetArg(argt[i], XmNbottomPosition, 95); i++;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(argt[i], XmNleftWidget,		clear_button);		i++;
    xmstr = ReadCatalogXms(MC_LABEL_SET, MC_OPTIONS_LABEL, MC_DEF_OPTIONS_LABEL );
    XtSetArg(argt[i], XmNlabelString,			xmstr		); i++;

    options_button = DtCreateMenuButton(matte1, "options_button", argt, i);

    XmStringFree(xmstr);
    XtManageChild(options_button);

    /** set attachment for help button **/
    i = 0;
    XtSetArg(argt[i], XmNleftAttachment,	XmATTACH_WIDGET);	i++;
    XtSetArg(argt[i], XmNleftWidget,		options_button);	i++;
    XtSetArg(argt[i], XmNrightAttachment,	XmATTACH_FORM);		i++;
    XtSetValues(help_button, argt, i);


    /*
     *  tell form that ok_button is the default button...
     */

    i = 0;
    XtSetArg(argt[i], XmNdefaultButton,		ok_button		); i++;
    XtSetValues(matte1,  argt, i);

    

    /*
     * make all buttons the same size...
     */
     

    max_width = max_height = 0;
    GetBiggest(ok_button, &max_width, &max_height);
    GetBiggest(clear_button,   &max_width, &max_height);
    GetBiggest(options_button, &max_width, &max_height);
    GetBiggest(help_button,    &max_width, &max_height);

    if ( max_width < MIN_BUTTON_SIZE) max_width = MIN_BUTTON_SIZE;
    
    i = 0;
    XtSetArg(argt[i], XmNwidth,			max_width		); i++;
    XtSetArg(argt[i], XmNheight,		max_height		); i++;
    XtSetArg(argt[i], XmNrecomputeSize,		False			); i++;

    XtSetValues(ok_button,      argt, i);
    XtSetValues(clear_button,   argt, i);
    XtSetValues(options_button, argt, i);
    XtSetValues(help_button,    argt, i);
}


/***************************************************************************
 *
 *  MakeChooser
 *
 *  Widgets: greeting, list_head, chooser_list
 ***************************************************************************/
typedef  struct {
    XmFontList  fontList;
} GreetInfo, *GreetInfoPtr;

static GreetInfo greetInfo;

static  XtResource greetResources[] = {
    {XmNfontList, XmCFontList,
        XmRFontList, sizeof(XmFontList),
        XtOffset(GreetInfoPtr, fontList), XtRString, NULL               }
};

void
MakeChooser( void )
{
    int i;

    /** greeting **/
    XtGetSubresources(table, &greetInfo, "greeting", "Greeting",
                      greetResources, XtNumber(greetResources), NULL, 0);
    i = InitArg(LabelG);
    xmstr = ReadCatalogXms(MC_CHOOSER_SET, MC_CHOOSER_TITLE, MC_DEF_CHOOSER_TITLE);
    XtSetArg(argt[i], XmNtraversalOn,      False); i++;
    XtSetArg(argt[i], XmNlabelString,      xmstr); i++;
    XtSetArg(argt[i], XmNtopAttachment,    XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNtopOffset,	   10); i++;
    XtSetArg(argt[i], XmNleftAttachment,   XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNrightAttachment,  XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNalignment,        XmALIGNMENT_CENTER); i++;
    if ( greetInfo.fontList != NULL ) {
        XtSetArg(argt[i], XmNfontList,     greetInfo.fontList); i++;
    }
    greeting = XmCreateLabel(matte1, "greeting", argt, i);
    XtManageChild(greeting);
    XmStringFree(xmstr);

    /** list head **/
    i = InitArg(LabelG);
    xmstr = ReadCatalogXms(MC_CHOOSER_SET, MC_CHOOSER_HEADING,
                        MC_DEF_CHOOSER_HEADING);
    XtSetArg(argt[i], XmNtraversalOn,      False); i++;
    XtSetArg(argt[i], XmNlabelString,      xmstr); i++;
    XtSetArg(argt[i], XmNtopAttachment,    XmATTACH_WIDGET); i++;
    XtSetArg(argt[i], XmNtopWidget,        greeting); i++;
    XtSetArg(argt[i], XmNtopOffset,        10); i++;
    XtSetArg(argt[i], XmNleftAttachment,   XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNleftOffset,       13); i++;
    if (appInfo.chlistFont != NULL) {
	XtSetArg(argt[i], XmNfontList,     appInfo.chlistFont); i++;
    }
    list_head = XmCreateLabelGadget(matte1, "list_head", argt, i);
    XtManageChild(list_head);
    XmStringFree(xmstr);

    i = 0;
    XtSetArg(argt[i], XmNresizable, True);
    XtSetValues(matte, argt, i);
    XtSetValues(matte1, argt, i);

    /** chooser_list **/
    i = 0;
    XtSetArg(argt[i], XmNleftAttachment,   XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNleftOffset,       10); i++;
    XtSetArg(argt[i], XmNrightAttachment,  XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNrightOffset,      10); i++;
    XtSetArg(argt[i], XmNtopAttachment,    XmATTACH_WIDGET); i++;
    XtSetArg(argt[i], XmNtopWidget,        list_head); i++;
    XtSetArg(argt[i], XmNtopOffset,        1); i++;
    XtSetArg(argt[i], XmNbottomAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(argt[i], XmNbottomWidget,     ok_button); i++;
    XtSetArg(argt[i], XmNbottomOffset,     10); i++;
    XtSetArg(argt[i], XmNallowShellResize, True); i++;
    XtSetArg(argt[i], XmNlistSizePolicy,   XmCONSTANT); i++;
    XtSetArg(argt[i], XmNscrollBarDisplayPolicy, XmSTATIC); i++;
    if (appInfo.chlistFont != NULL) {
        XtSetArg(argt[i], XmNfontList,     appInfo.chlistFont); i++;
    }
    chooser_list = XmCreateScrolledList(matte1, "chooser_list", argt, i);
    XtAddCallback(chooser_list, XmNdefaultActionCallback, DoAccept, NULL);
    XtManageChild(chooser_list);

    if (appInfo.workspaceCursor)
    {
        MakeRootCursor();
    }
    else
    {
        XUndefineCursor(dpyinfo.dpy, dpyinfo.root);
    }
}


/***************************************************************************
 *
 *  MakeDialog
 *
 *  Widgets: error_message, help_message, copyright_msg, hostname_message,
 *	     passwd_message
 ***************************************************************************/

void 
MakeDialog( DialogType dtype )
{
    register int i, j;

    int		width;
    
    FILE	*fp, *fopen();
    char	buffer[128];
    char	*str;

    Widget	w, text;
    Dimension	txt_width, txt_height;
    XmString	ok, cancel, nw, sv;

    Widget      tlev;

    /*
     *  do things common to all dialogs...
     */

    ok     = ReadCatalogXms(MC_LABEL_SET, MC_OK_LABEL, MC_DEF_OK_LABEL);
    cancel = ReadCatalogXms(MC_LABEL_SET, MC_CANCEL_LABEL, MC_DEF_CANCEL_LABEL);

    i = InitArg(MessageBox);
    XtSetArg(argt[i], XmNmarginHeight,		MBOX_MARGIN_HEIGHT	); i++;
    XtSetArg(argt[i], XmNmarginWidth,		MBOX_MARGIN_WIDTH	); i++;
    XtSetArg(argt[i], XmNshadowThickness,	SHADOW_THICKNESS	); i++;
    XtSetArg(argt[i], XmNokLabelString,		ok			); i++;
    XtSetArg(argt[i], XmNcancelLabelString,	cancel			); i++;
    XtSetArg(argt[i], XmNnoResize,		False			); i++;
    XtSetArg(argt[i], XmNresizePolicy,		XmRESIZE_ANY		); i++;

    /*
     *  create the various dialogs...
     */

    /* JET - check the matte widget, and if non-null, well use that as
     * the parent for dialogs.  Otherwise use table (the original
     * toplevel widget for this func).  This is useful for Xinerama so
     * that child dialogs are centered on the matte, and not the whole
     * SLS screen.
     */
    if (matte != (Widget)NULL)
      tlev = matte;
    else
      tlev = table;


    switch (dtype) {

    case error:
	xmstr = ReadCatalogXms(MC_ERROR_SET, MC_LOGIN, "");
	XtSetArg(argt[i], XmNmessageString,		xmstr		); i++;

	w = XmCreateErrorDialog(tlev, "error_message", argt, i);
	XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));

	error_message = w;
	break;


    case help:
	xmstr = ReadCatalogXms(MC_HELP_SET, MC_HELP, MC_DEF_HELP);
	XtSetArg(argt[i], XmNmessageString,		xmstr		); i++;
	w = XmCreateInformationDialog(tlev, "help_message", argt, i);
	XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));

        txt_width = DisplayWidth (XtDisplay(w), DefaultScreen(XtDisplay(w)));
        txt_height = DisplayHeight (XtDisplay(w), DefaultScreen(XtDisplay(w)));
        txt_width = (txt_width > 850) ? 800 : txt_width - 50;
        txt_height = (txt_height > 900) ? 750 : txt_height - 150;
        i = InitArg(Text);
        XtSetArg(argt[i], XmNheight, txt_height); i++;
        XtSetArg(argt[i], XmNwidth, txt_width); i++;
        XtSetArg(argt[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
        XtSetArg(argt[i], XmNscrollBarDisplayPolicy, XmAS_NEEDED); i++;
        XtSetArg(argt[i], XmNscrollingPolicy, XmAUTOMATIC); i++;
        XtSetArg(argt[i], XmNeditable, False); i++;
	str = (char*) ReadCatalog(MC_HELP_SET, MC_HELP, MC_DEF_HELP);
        XtSetArg(argt[i], XmNvalue, strdup(str)); i++;
        text = XmCreateScrolledText(w, "help_message_text", argt, i);

        XtManageChild(text);
        XtManageChild(w);
        help_message = w;
	break;


    case copyright:
	if ((fp = fopen(COPYRIGHT,"r")) == NULL)
#if defined( __hp_osf )
	    xmstr = XmStringCreate("Cannot open copyright file '/usr/X11/copyright'.",
				XmFONTLIST_DEFAULT_TAG);
#else
	    xmstr = XmStringCreate("Cannot open copyright file '/etc/copyright'.",
				XmFONTLIST_DEFAULT_TAG);
#endif
	else {
	    xmstr = (XmString) NULL;
	
	    while (fgets(buffer, 128, fp) != NULL) {
		j = strlen(buffer);
		if ( buffer[j-1] == '\n' ) buffer[j-1] = '\0';
	    
		if ( xmstr != NULL )
		    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());

		xmstr = XmStringConcat(xmstr,
	    			       XmStringCreate(buffer,
				       XmFONTLIST_DEFAULT_TAG));
	    }	    
	}

	fclose(fp);
	XtSetArg(argt[i], XmNmessageString,		xmstr		); i++;

	w = XmCreateInformationDialog(tlev, "copyright_msg", argt, i);
	XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));
	
	XtAddCallback(w, XmNokCallback, CopyrightCB, (XtPointer) 0);    

	copyright_msg = w;
	break;
    

    case hostname:
	    
	nw = ReadCatalogXms(MC_LABEL_SET, MC_NW_LABEL,  MC_DEF_NW_LABEL);
	sv = ReadCatalogXms(MC_LABEL_SET, MC_START_LABEL, MC_DEF_START_LABEL);

	xmstr = ReadCatalogXms(MC_HELP_SET, MC_SYSTEM, MC_DEF_SYSTEM);
	XtSetArg(argt[i], XmNmessageString,		xmstr		); i++;
	XtSetArg(argt[i], XmNokLabelString,		nw		); i++;
	XtSetArg(argt[i], XmNcancelLabelString,		sv		); i++;

	w = XmCreateWarningDialog(tlev, "hostname_msg", argt, i);

	XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));

	XmStringFree(nw);
	XmStringFree(sv);

	hostname_message = w;
	break;


    case expassword:

	xmstr = ReadCatalogXms(MC_ERROR_SET, MC_PASSWD_EXPIRED, 
			    MC_DEF_PASSWD_EXPIRED);
	XtSetArg(argt[i], XmNmessageString,		xmstr		); i++;

	w = XmCreateQuestionDialog(tlev, "password_msg", argt, i);

	XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));

	passwd_message = w;
	break;


    case help_chooser:
	xmstr = ReadCatalogXms(MC_HELP_SET, MC_HELP_CHOOSER, MC_DEF_HELP_CHOOSER);

	w = XmCreateInformationDialog(tlev, "help_message", argt, i);
        XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(w,XmDIALOG_HELP_BUTTON));

        txt_width = DisplayWidth (XtDisplay(w), DefaultScreen(XtDisplay(w)));
        txt_height = DisplayHeight (XtDisplay(w), DefaultScreen(XtDisplay(w)));
        txt_width = (txt_width > 850) ? 800 : txt_width - 50;
        txt_height = (txt_height > 900) ? 750 : txt_height - 150;
        i = InitArg(Text);
        XtSetArg(argt[i], XmNheight, txt_height); i++;
        XtSetArg(argt[i], XmNwidth, txt_width); i++;
        XtSetArg(argt[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
        XtSetArg(argt[i], XmNscrollBarDisplayPolicy, XmAS_NEEDED); i++;
        XtSetArg(argt[i], XmNscrollingPolicy, XmAUTOMATIC); i++;
        XtSetArg(argt[i], XmNeditable, False); i++;
        str = (char*)
	      ReadCatalog(MC_HELP_SET, MC_HELP_CHOOSER, MC_DEF_HELP_CHOOSER);
        XtSetArg(argt[i], XmNvalue, strdup(str)); i++;
        text = XmCreateScrolledText(w, "help_message_text", argt, i);

        XtManageChild(text);
        XtManageChild(w);
        help_message = w;
        break;
    }

    /*
     *  finish up...
     */

    switch (dtype) {
      case error:
      case hostname:
      case expassword:
        XtAddCallback(w, XmNokCallback,     RespondDialogCB, NULL);
        XtAddCallback(w, XmNcancelCallback, RespondDialogCB, NULL);
        break;
    }


    XtSetArg(argt[0], XmNdialogStyle,	XmDIALOG_APPLICATION_MODAL	); i++;
    XtSetValues(w, argt, 1);

    XmStringFree(xmstr);
    XmStringFree(ok);
    XmStringFree(cancel);


    /*
     *  adjust the width of the "ok" button on the dialogs...
     */

    width = (dtype == hostname ? FromMM(4000) : MIN_BUTTON_SIZE);
    
    i = 0;
    XtSetArg(argt[i], XmNrecomputeSize,			False		); i++;
    XtSetArg(argt[i], XmNwidth,				width		); i++;

    XtSetValues(XmMessageBoxGetChild(w, XmDIALOG_OK_BUTTON), argt, i);

}



/***************************************************************************
 *
 *  MakeOptionsMenu
 *
 *  Widgets: options_menu, options_item[]
 ***************************************************************************/

void 
MakeOptionsMenu( void )
{
    int 	i, j, k;

    struct stat	statb;


    /*
     * get the built-in pop_up menu from the DtMenuButton...
     */
     
    XtVaGetValues(options_button, DtNsubMenuId, &options_menu, NULL);

    /*
     *  create language cascade menus...
     */

    if ( lang_menu == NULL )
	MakeLangMenu();


    /*
     *  create first level menu items...
     */
    j = 0;

    /*
     *  build [ Language ] menu pane if there are languages to choose from...
     */
    if ( lang_menu != NULL ) {
        /*
         *  [ Language ] menu pane...
         *  attach language cascade menu to this pane
         */
        i = InitArg(CascadeBG);
        xmstr = ReadCatalogXms(MC_LABEL_SET, MC_LANG_LABEL, MC_DEF_LANG_LABEL);
        XtSetArg(argt[i], XmNlabelString,               xmstr           ); i++;
        XtSetArg(argt[i], XmNsubMenuId,                 lang_menu       ); i++;
        XtSetArg(argt[i], XmNrecomputeSize,             True            ); i++;
        options_item[j] = XmCreateCascadeButtonGadget(options_menu,
                                "options_languages", argt, i);
        XmStringFree(xmstr);
        j++;

        /*
         *  separator...
         */
        i = InitArg(SeparatorG);
        options_item[j] = XmCreateSeparatorGadget(options_menu,
                                                    "options_sep2",
                                                     argt, i);
        j++;
    }


    /* 
     *  [ Restart Server ] menu pane...
     */
    i = k = InitArg(PushBG);
    xmstr = ReadCatalogXms(MC_LABEL_SET, MC_RS_LABEL, MC_DEF_RS_LABEL);
    XtSetArg(argt[i], XmNlabelString,			xmstr		); i++;
    options_item[j] = XmCreatePushButtonGadget(options_menu,
    						 "options_restartServer",
    						 argt, i); 
    XmStringFree(xmstr);
    XtAddCallback(options_item[j], XmNactivateCallback, 
    		  DoCancel, (XtPointer) OB_RESTART_SERVER);
    j++;

#ifdef copyright_option
    /*
     *  separator...
     */
    i = InitArg(SeparatorG);
    options_item[j] = XmCreateSeparatorGadget(options_menu, "options_sep1",
                                                argt, i);
    j++;


    /* 
     *  [ Copyright ] menu pane...
     */
    i = k = InitArg(PushBG);
    xmstr = ReadCatalogXms(MC_LABEL_SET, MC_COPY_LABEL, MC_DEF_COPY_LABEL);
    XtSetArg(argt[i], XmNlabelString,			xmstr		); i++;
    options_item[j] = XmCreatePushButtonGadget(options_menu, 
    						 "options_copyright",
    						 argt, i);
    XmStringFree(xmstr);
    XtAddCallback(options_item[j], XmNactivateCallback, 
    		  MenuItemCB, (XtPointer) OB_COPYRIGHT);
    j++;
#endif

    /*
     *  manage the [Options] menu...
     */
    XtManageChildren(options_item, j);



    /*
     *  If the DT Lite Session Manager is not available, remove the DT Lite
     *  and DT menu panes. The actual widgets must still be created since
     *  other code (ex. MenuItemCB()) tries to obtain some of their resources.
     */
     
    if  ( stat(DTLITESESSION, &statb) != 0 ||
	  ((statb.st_mode & S_IXOTH) != S_IXOTH) ) {

/*
	XtUnmanageChild(options_dt);
*/
    }

    if ( getenv(PINGINTERVAL) != NULL )
	XtUnmanageChild(options_nowindows);
	

}





/***************************************************************************
 *
 *  ErrorHandler
 *
 *  X protocol error handler to override the default
 ***************************************************************************/

static int 
ErrorHandler( Display *dpy, XErrorEvent *event )
{
    return 0;
}




/***************************************************************************
 *
 *  MakeOptionsProc
 *
 *  Timeout routine to build options menu
 ***************************************************************************/

void 
MakeOptionsProc( XtPointer data, XtIntervalId *id )
{

    if (options_menu == NULL)
	MakeOptionsMenu();
    
    return;
}




/***************************************************************************
 *
 *  Terminate
 *
 *  Catch a SIGTERM and unmanage display
 ***************************************************************************/

static SIGVAL
Terminate( int arg )

{
    write(1, "terminate", 9);
    CleanupAndExit(NULL, NOTIFY_ABORT);
}

#ifdef SIA
/*
 * Chooser doesn't use SIA but it does link with vgcallback.o.  We just need
 * this symbol to keep the linker happy.
 */

void SiaForm()
{
return;
}

#endif
