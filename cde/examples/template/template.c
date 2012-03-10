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
/* $TOG: template.c /main/6 1999/09/20 15:48:19 mgreess $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company	
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */


/*
 * template.c
 *
 * Example code for typical Dt application
 *
 * Simple spray-can drawing tool
 */

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <nl_types.h>
#include <Xm/XmAll.h>
#include <Dt/Dnd.h>
#include <Dt/Dts.h>
#include <Dt/HelpDialog.h>
#include <Tt/tttk.h>

/* default brush - used when the brush bitmap file is not found at run-time. */
#include "default_brush.bm"

/* Portability for catopen */
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif


#define ApplicationClass "Template"
#define MessageCatalog "template.cat"
#define HelpVolume "template"
#define HelpTopic "_hometopic"
#define IconPixmap "template_icon.pm"
#define IconMask "template_icon_m.bm"
#define Suffix ".template"
#define SuffixLength 9
#define UnnamedFile "unnamed.template"
#define SearchPattern "*.template"
#define DataType "TemplateData"
#define ToolTalkPType "DT_Example_Template"
#define FileSignature "@template@\n"

#define MallocInc 10

typedef struct _WindowData {
    int			npoints;
    int			nalloc;
    XPoint		*points;
    Widget		shell;
    Widget		openDialog;
    Widget		saveDialog;
    char		*name;		/* NULL if no file */
    struct _WindowData	*next;
} WindowData;

static XtAppContext appContext;
static nl_catd msgCatalog;
static Widget appShell;
static WindowData *windowList = NULL;
static XContext wdContext;
static Atom WM_DELETE_WINDOW;
static Atom WM_SAVE_YOURSELF;
static char *appnameString;
static char *separatorString;
static char *untitledString;
static char *argv0;
static char *programName;

/* ToolTalk stuff */
static int ttfd;
static char *procid;
static Tt_pattern *ttpat;
static Tt_status ttrc;
static Tt_message HandleTtMedia(Tt_message, void *, Tttk_op, Tt_status,
	unsigned char *, int, char *, char *);

#define DrawingTranslations "#replace\
	<Btn1Down>: DrawingAreaInput()\n\
	<Btn1Motion>: DrawingAreaInput()"

static void ClearCb(Widget, XtPointer, XtPointer);
static void HelpCb(Widget, XtPointer, XtPointer);
static void NewCb(Widget, XtPointer, XtPointer);
static void OpenCb(Widget, XtPointer, XtPointer);
static void OpenOkCb(Widget, XtPointer, XtPointer);
static void SaveCb(Widget, XtPointer, XtPointer);
static void SaveOkCb(Widget, XtPointer, XtPointer);
static void PrintCb(Widget, XtPointer, XtPointer);
static void ExitCb(Widget, XtPointer, XtPointer);
static void ExposeCb(Widget, XtPointer, XtPointer);
static void InputCb(Widget, XtPointer, XtPointer);
static void DropTransferCb(Widget, XtPointer, XtPointer);
static void SaveSessionCb(Widget, XtPointer, XtPointer);

typedef enum {
    LOAD_EMPTY, LOAD_FILE, LOAD_BUFFER
} LoadType;

static void Fatal(char*);
static void ReallyExit(int);
static void SetTitle(Widget,char*);
static Boolean NewWindow(LoadType,char*,int);
static WindowData *NewData(void);
static void AssocData(WindowData*,Widget);
static WindowData *FindData(Widget);
static void DestroyData(WindowData*);
static Boolean LoadFile(WindowData*,char*);
static Boolean LoadBuffer(WindowData*,void*,int);
static void FreeData(WindowData*);
static void PrintData(WindowData*);
static void AddPoint(WindowData*, int, int);
static void DrawPoint(Widget, int, int);
static char* AppendString(char*,char*);

static XtCallbackRec DropTransferCbList[] = {
	{ DropTransferCb, NULL},
	{ NULL, NULL}
};

static XrmOptionDescRec optionTable[] = {
    {"-print",    ".printMode",   XrmoptionIsArg, NULL},
    {"-server",   ".serverMode",  XrmoptionIsArg, NULL},
};

typedef struct {
    String printMode;
    String serverMode;
} appResourceRec;

static XtResource appResources[] = {
    { "printMode", "PrintMode", XtRString, sizeof(String),
	XtOffsetOf(appResourceRec, printMode), XtRString, NULL },
    { "serverMode", "ServerMode", XtRString, sizeof(String),
	XtOffsetOf(appResourceRec, serverMode), XtRString, NULL },
};

main(int argc, char **argv)
{
    int i;
    appResourceRec argvals;
    char *errfmt, *errmsg, *statmsg;

    argv0 = argv[0];
    programName = strrchr(argv[0], '/');
    if (programName == NULL)
	programName = argv[0];
    else
	++programName;

    XtSetLanguageProc(NULL, NULL, NULL);

    appShell = XtAppInitialize(&appContext, ApplicationClass,
			       optionTable, XtNumber(optionTable),
			       &argc, argv, NULL, NULL, 0);

    msgCatalog = catopen(MessageCatalog, NL_CAT_LOCALE);

    XtGetApplicationResources(appShell, &argvals,
			      appResources, XtNumber(appResources),
			      NULL, 0);

    wdContext = XUniqueContext();

    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(appShell), "WM_DELETE_WINDOW",
				    False);
    WM_SAVE_YOURSELF = XmInternAtom(XtDisplay(appShell), "WM_SAVE_YOURSELF",
				    False);

    appnameString = catgets(msgCatalog, 1, 1, "Template");
    separatorString = catgets(msgCatalog, 1, 5, " - ");
    untitledString = catgets(msgCatalog, 1, 6, "(untitled)");

    if (argvals.printMode != NULL) {
	/* Load up each file and print it, then exit */
	WindowData *wd = NewData();
	for (i = 1; i < argc; ++i) {
	    if (LoadFile(wd, argv[i]))
		PrintData(wd);
	    else
		fprintf(stderr,
		    catgets(msgCatalog, 1, 10, "template: can't open %s\n"),
		    argv[i]);
	}
	DestroyData(wd);
	exit(0);
    }

    /* Initialize Data Typing and ToolTalk */

    DtDtsLoadDataTypes();

    procid = ttdt_open(&ttfd, appnameString, "CDE", "1.0", True);
    if ((ttrc = tt_ptr_error(procid)) != TT_OK) {
	errfmt = catgets(msgCatalog, 1, 7, "ttdt_open failed:\n%s");
        statmsg = tt_status_message(ttrc);
        errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
        sprintf(errmsg, errfmt, statmsg);

	Fatal(errmsg);
	XtFree(errmsg);
    }

    ttrc = ttmedia_ptype_declare(ToolTalkPType, 0, HandleTtMedia,
				 NULL, True);
    if (tt_is_err(ttrc)) {
	errfmt = catgets(msgCatalog, 1, 8, "ttmedia_ptype_declare failed:\n%s");
        statmsg = tt_status_message(status);
        errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
        sprintf(errmsg, errfmt, statmsg);

	Fatal(errmsg);
	XtFree(errmsg);
    }

    ttpat = ttdt_session_join(NULL, NULL, NULL, NULL, True);
    if ((ttrc = tt_ptr_error(ttpat)) != TT_OK) {
	errfmt = catgets(msgCatalog, 1, 9, "ttdt_session_join failed:\n%s");
        statmsg = tt_status_message(status);
        errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
        sprintf(errmsg, errfmt, statmsg);

	Fatal(errmsg);
	XtFree(errmsg);
    }

    XtAppAddInput(appContext, ttfd, (XtPointer)XtInputReadMask,
		  tttk_Xt_input_handler, NULL);

    if (argvals.serverMode != NULL) {
	/*
	 * We're in server mode.  Thus do nothing until requested to do so
	 * through ToolTalk.
	 */
    } else if (argc < 2) {
	/* No files given, so put up an untitled window. */
	(void) NewWindow(LOAD_EMPTY, NULL, 0);
    } else {
	/* Load each file into its own window. */
	for (i = 1; i < argc; ++i)
	    (void) NewWindow(LOAD_FILE, argv[i], 0);
    }

    /*
     * Start the GUI.  Note that we explicitly do not realize the appShell
     * widget, since it is the unmapped parent of all of the top-level shells 
     * we pop up.
     */

    XtAppMainLoop(appContext);
    ReallyExit(0);
}


/*
 * Close ToolTalk and exit.
 */
static void ReallyExit(int rc)
{
    tt_close();
    exit(rc);
}


/*
 * Issue an error message and exit.
 */
static void Fatal(char *msg)
{
    fprintf(stderr, "%s: %s\n", programName, msg);
    exit(1);
}


/*
 * Create a new top-level window.  If loadtype is LOAD_EMPTY, name_or_buf and 
 * len are ignored.  If loadtype is LOAD_FILE, name_or_buf should point to the 
 * name of the file to load, and len is ignored.  If loadtype is LOAD_BUFFER,
 * name_or_buf is a pointer to the data buffer and len is its length.
 */
static Boolean
NewWindow(LoadType loadtype, char *name_or_buf, int len)
{
    Widget toplevel, mainWindow, menuBar, frame1, frame2,
	drawingArea, pd, cb, pb;
    char *title;
    XmString labelString;
    XtTranslations drawingTranslations;
    Pixmap iconPixmap;
    Pixmap iconMask;
    Pixel fg, bg;
    Arg args[20];
    int n;
    WindowData *wd;

    wd = NewData();

    n = 0;
    toplevel = XtCreatePopupShell("foo", topLevelShellWidgetClass,
		appShell, args, n);

    /* Create main window */

    n = 0;
    mainWindow = XmCreateMainWindow(toplevel, "mainWindow", args, n);
    XtManageChild(mainWindow);

    /* Set window manager title and icon */

    XtVaGetValues(mainWindow, XmNforeground, &fg, XmNbackground, &bg, NULL);
    iconPixmap = XmGetPixmap(XtScreen(toplevel), IconPixmap, fg, bg);
    iconMask = XmGetPixmapByDepth(XtScreen(toplevel), IconMask, 1, 0, 1);
    XtVaSetValues(toplevel,
		XmNiconName, appnameString,
		XmNiconPixmap, iconPixmap,
		XmNiconMask, iconMask,
		NULL);

    /* Create the GUI */

    menuBar = XmCreateMenuBar(mainWindow, "menuBar", NULL, 0);
    XtManageChild(menuBar);

    /* File menu */

    pd = XmCreatePulldownMenu(menuBar, "fileMenu", NULL, 0);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 1, "File"));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'F'); n++;
    XtSetArg(args[n], XmNsubMenuId, pd); n++;
    cb = XmCreateCascadeButton(menuBar, "fileCascade", args, n);
    XtManageChild(cb);
    XmStringFree(labelString);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 9, "New..."));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'N'); n++;
    pb = XmCreatePushButton(pd, "newButton", args, n);
    XtManageChild(pb);
    XtAddCallback(pb, XmNactivateCallback, NewCb, NULL);
    XmStringFree(labelString);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 2, "Open..."));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'O'); n++;
    pb = XmCreatePushButton(pd, "openButton", args, n);
    XtManageChild(pb);
    XtAddCallback(pb, XmNactivateCallback, OpenCb, (XtPointer)wd);
    XmStringFree(labelString);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 3,
								"Save As..."));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'S'); n++;
    pb = XmCreatePushButton(pd, "saveButton", args, n);
    XtManageChild(pb);
    XtAddCallback(pb, XmNactivateCallback, SaveCb, (XtPointer)wd);
    XmStringFree(labelString);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 4, "Print"));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'P'); n++;
    pb = XmCreatePushButton(pd, "printButton", args, n);
    XtManageChild(pb);
    XtAddCallback(pb, XmNactivateCallback, PrintCb, (XtPointer)wd);
    XmStringFree(labelString);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 5, "Clear"));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'C'); n++;
    pb = XmCreatePushButton(pd, "clearButton", args, n);
    XtManageChild(pb);
    XtAddCallback(pb, XmNactivateCallback, ClearCb, (XtPointer)wd);
    XmStringFree(labelString);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 6, "Exit"));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'E'); n++;
    pb = XmCreatePushButton(pd, "exitButton", args, n);
    XtManageChild(pb);
    XtAddCallback(pb, XmNactivateCallback, ExitCb, (XtPointer)wd);
    XmStringFree(labelString);

    /* Help menu */

    pd = XmCreatePulldownMenu(menuBar, "helpMenu", NULL, 0);

    labelString = XmStringCreateLocalized(catgets(msgCatalog, 2, 7, "Help"));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'H'); n++;
    XtSetArg(args[n], XmNsubMenuId, pd); n++;
    cb = XmCreateCascadeButton(menuBar, "helpCascade", args, n);
    XtManageChild(cb);
    XmStringFree(labelString);

    XtVaSetValues(menuBar, XmNmenuHelpWidget, cb, NULL);

    labelString = XmStringCreateLocalized(catgets(msgCatalog,2,8, "Overview..."));
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    XtSetArg(args[n], XmNmnemonic, 'O'); n++;
    pb = XmCreatePushButton(pd, "helpButton", args, n);
    XtManageChild(pb);
    XtAddCallback(pb, XmNactivateCallback, HelpCb, NULL);
    XmStringFree(labelString);

    /* Drawing work area */

    n = 0;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNmarginWidth, 20); n++;
    XtSetArg(args[n], XmNmarginHeight, 20); n++;
    frame1 = XmCreateFrame(mainWindow, "frame1", args, n);
    XtManageChild(frame1);

    n = 0;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_ETCHED_OUT); n++;
    frame2 = XmCreateFrame(frame1, "frame2", args, n);
    XtManageChild(frame2);

    drawingTranslations = XtParseTranslationTable(DrawingTranslations);

    n = 0;
    XtSetArg(args[n], XmNunitType, Xm100TH_MILLIMETERS); n++;
    XtSetArg(args[n], XmNwidth, 12000); n++;
    XtSetArg(args[n], XmNheight, 12000); n++;
    XtSetArg(args[n], XmNtranslations, drawingTranslations); n++;
    drawingArea = XmCreateDrawingArea(frame2, "drawingArea", args, n);
    XtManageChild(drawingArea);
    XtAddCallback(drawingArea, XmNexposeCallback, ExposeCb, NULL);
    XtAddCallback(drawingArea, XmNinputCallback, InputCb, NULL);

    DtDndDropRegister(drawingArea,
		      DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER,
		      XmDROP_COPY, DropTransferCbList, NULL, 0);

    XmAddWMProtocolCallback(toplevel, WM_DELETE_WINDOW, ExitCb, (XtPointer)wd);
    XmAddWMProtocolCallback(toplevel, WM_SAVE_YOURSELF, SaveSessionCb,
			    (XtPointer)NULL);

    XtRealizeWidget(toplevel);
    XtPopup(toplevel, XtGrabNone);

    AssocData(wd, toplevel);

    switch (loadtype) {
    case LOAD_EMPTY:
	SetTitle(toplevel, NULL);
	return True;
    case LOAD_FILE:
        return LoadFile(wd, name_or_buf);
    case LOAD_BUFFER:
	return LoadBuffer(wd, name_or_buf, len);
    }
    /*NOTREACHED*/
}


/*
 * Display the help system. On the first call, create the help widget.
 */

static void HelpCb(Widget w, XtPointer cd, XtPointer cb)
{
    static Widget helpDialog = NULL;

    if (helpDialog == NULL) {
	char *title;
	Arg args[10];
	int n;

	n = 0;
	XtSetArg(args[n], DtNhelpVolume, HelpVolume); n++;
	XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
	XtSetArg(args[n], DtNlocationId, HelpTopic); n++;
	helpDialog = DtCreateHelpDialog(appShell, "helpDialog", args, n);

	title = catgets(msgCatalog, 1, 4, "Template Help");
	XtVaSetValues(XtParent(helpDialog), XmNtitle, title, NULL);
    } else {
	XtVaSetValues(helpDialog,
		      DtNhelpVolume, HelpVolume,
		      DtNlocationId, HelpTopic,
		      NULL);
    }
    XtManageChild(helpDialog);
}


/*
 * Clear the display and drawing data.
 */

static void ClearCb(Widget w, XtPointer cd, XtPointer cb)
{
    WindowData *wd = (WindowData *)cd;
    Widget drawingArea = XtNameToWidget(wd->shell, "*drawingArea");

    FreeData(wd);
    XClearWindow(XtDisplay(drawingArea), XtWindow(drawingArea));
}


/*
 * Create a new top-level window.
 */
static void NewCb(Widget w, XtPointer cd, XtPointer cb)
{
    (void) NewWindow(LOAD_EMPTY, NULL, 0);
}


/*
 * Display a File dialog. On the first call, create the dialog.
 */

static void OpenCb(Widget w, XtPointer cd, XtPointer cb)
{
    WindowData *wd = (WindowData *)cd;

    if (wd->openDialog == NULL) {
	XmString pattern;
	XmString dialogTitle;
	Arg args[20];
	int n;

	dialogTitle = XmStringCreateLocalized(catgets(msgCatalog, 1, 2,
					"Template Open"));
	pattern = XmStringCreateLocalized(SearchPattern);
	n = 0;
	XtSetArg(args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);n++;
	XtSetArg(args[n], XmNautoUnmanage, True); n++;
	XtSetArg(args[n], XmNpattern, pattern); n++;
	XtSetArg(args[n], XmNdialogTitle, dialogTitle); n++;
	wd->openDialog = XmCreateFileSelectionDialog(wd->shell, "openDialog",
						     args, n);
	XtUnmanageChild(XtNameToWidget(wd->openDialog, "*Help"));
	XtAddCallback(wd->openDialog, XmNokCallback, OpenOkCb, cd);
	XmStringFree(pattern);
	XmStringFree(dialogTitle);
    }
    XtManageChild(wd->openDialog);
}

static void OpenOkCb(Widget w, XtPointer cd, XtPointer cb)
{
    char *fileName;
    WindowData *wd = (WindowData *)cd;
    XmFileSelectionBoxCallbackStruct *fsbcs =
	(XmFileSelectionBoxCallbackStruct *) cb;
    Widget drawingArea = XtNameToWidget(wd->shell, "*drawingArea");

    fileName = XmStringUnparse(fsbcs->value, NULL, XmCHARSET_TEXT,
                               XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    (void) LoadFile(wd, fileName);
    XtFree(fileName);

    XClearArea(XtDisplay(drawingArea), XtWindow(drawingArea), 0, 0, 0, 0, True);
}


/*
 * Display a File dialog. On the first call, create the dialog.
 */

static void SaveCb(Widget w, XtPointer cd, XtPointer cb)
{
    WindowData *wd = (WindowData *)cd;

    if (wd->saveDialog == NULL) {
	XmString pattern;
	XmString dialogTitle;
	Arg args[20];
	int n;

	dialogTitle = XmStringCreateLocalized(catgets(msgCatalog, 1, 3,
					"Template Save As"));
	pattern = XmStringCreateLocalized(SearchPattern);
	n = 0;
	XtSetArg(args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);n++;
	XtSetArg(args[n], XmNautoUnmanage, True); n++;
	XtSetArg(args[n], XmNpattern, pattern); n++;
	XtSetArg(args[n], XmNdialogTitle, dialogTitle); n++;
	wd->saveDialog = XmCreateFileSelectionDialog(appShell, "saveAsDialog",
								args, n);
	XtUnmanageChild(XtNameToWidget(wd->saveDialog, "*Help"));
	XtAddCallback(wd->saveDialog, XmNokCallback, SaveOkCb, cd);
	XmStringFree(pattern);
	XmStringFree(dialogTitle);
    }

    XtManageChild(wd->saveDialog);
}

static void SaveOkCb(Widget w, XtPointer cd, XtPointer cb)
{
    FILE *fp;
    int i;
    char * fileName;
    int fileLength;
    WindowData *wd = (WindowData *)cd;

    XmFileSelectionBoxCallbackStruct *fsbcs =
	(XmFileSelectionBoxCallbackStruct *) cb;
    XmStringGetLtoR(fsbcs->value, XmFONTLIST_DEFAULT_TAG, &fileName);
    fileLength = strlen(fileName);

    if (fileName[fileLength-1]== '/') {
	fileName = AppendString(fileName, UnnamedFile);
	fileLength = strlen(fileName);
    }

    if (fileLength > SuffixLength) {
	if (strcmp(fileName+fileLength-SuffixLength, Suffix) != 0) {
	    fileName = AppendString(fileName,Suffix);
	}
    }
    else {
	fileName = AppendString(fileName, Suffix);
    }

    if ((fp = fopen(fileName, "w")) != NULL) {
	fputs(FileSignature, fp);
	for (i=0; i < wd->npoints; i++)
	    fprintf(fp, "%d %d\n", wd->points[i].x, wd->points[i].y);
	fclose(fp);
	XtFree(wd->name);
	wd->name = XtNewString(fileName);
	SetTitle(wd->shell, fileName);
    }

    XtFree(fileName);
}

static char* AppendString(char *base, char *suffix)
{
    char *file;

    file = XtMalloc(strlen(base)+strlen(suffix)+1);
    strcpy(file, base);
    strcat(file, suffix);
    XtFree(base);
    return(file);
}


/*
 * Respond to the TemplatePrint action by printing the data file
 * specified to the '-print' option.
 */

static void PrintCb(Widget w, XtPointer cd, XtPointer cb)
{
    WindowData *wd = (WindowData *)cd;
    PrintData(wd);
}


/*
 * Redraw the display when exposed.
 */

static void ExposeCb(Widget w, XtPointer cd, XtPointer cb)
{
    XmDrawingAreaCallbackStruct *dcb = (XmDrawingAreaCallbackStruct*)cb;
    WindowData *wd;
    int i;

    if (dcb->event != NULL && dcb->event->xexpose.count > 0) return;

    wd = FindData(w);

    for (i = 0; i < wd->npoints; i++)
	DrawPoint(w, wd->points[i].x, wd->points[i].y);
}


/*
 * Process mouse input.
 */

static void InputCb(Widget w, XtPointer cd, XtPointer cb)
{
    XmDrawingAreaCallbackStruct *dcb = (XmDrawingAreaCallbackStruct*)cb;
    WindowData *wd;

    if (dcb->event->xany.type != ButtonPress &&
	dcb->event->xany.type != MotionNotify)
	return;

    wd = FindData(w);

    AddPoint(wd, dcb->event->xbutton.x, dcb->event->xbutton.y);
    DrawPoint(w, dcb->event->xbutton.x, dcb->event->xbutton.y);
}


/*
 * Delete the current window.  If there are no more windows, exit the 
 * application.
 */

static void ExitCb(Widget w, XtPointer cd, XtPointer cb)
{
    WindowData *wd = (WindowData *)cd;
    DestroyData(wd);
    if (windowList == NULL)
	ReallyExit(0);
}


/*
 * Handle WM_SAVE_YOURSELF by updating the command line with all the files
 * currently being edited.
 */
static void SaveSessionCb(Widget w, XtPointer cd, XtPointer cb)
{
    char **command;
    int argcount = 1;		/* starts at 1 for command name */
    WindowData *wd = windowList;
    int i;
    Widget first;

    /* count the number of windows bound to files */
    for (wd = windowList; wd != NULL; wd = wd->next) {
	if (wd->name != NULL)
	    ++argcount;
    }

    command = (char **)XtMalloc(argcount*sizeof(char*));
    command[0] = argv0;
    i = 1;
    for (wd = windowList; wd != NULL; wd = wd->next) {
	if (wd->name != NULL)
	    command[i++] = wd->name;
    }

    first = windowList->shell;
	XSetCommand(XtDisplay(first), XtWindow(first), command, i);
    if (w != first)
	XChangeProperty(XtDisplay(w), XtWindow(w), XA_WM_COMMAND, XA_STRING, 8,
			PropModeReplace, NULL, 0);
    XtFree((char *)command);
}


/*
 * Accept .template files dropped on the drawing window.
 */

static void DropTransferCb(Widget drawingArea, XtPointer cd, XtPointer cb)
{
    DtDndTransferCallbackStruct *dcb = (DtDndTransferCallbackStruct*)cb;
    WindowData *wd = FindData(drawingArea);
    char *fileName;
    char *dataType;
    void *bufPtr;
    int bufLen;

    dcb->status = DtDND_FAILURE;

    if (dcb->dropData->numItems > 1)
	return;

    switch (dcb->dropData->protocol) {

    case DtDND_FILENAME_TRANSFER:
 	fileName = dcb->dropData->data.files[0];
	dataType = DtDtsFileToDataType(fileName);
        if (strcmp(dataType, DataType) != 0) {
	    DtDtsFreeDataType(dataType);
	    return;
        }
        if (LoadFile(wd, fileName))
            dcb->status = DtDND_SUCCESS;
	DtDtsFreeDataType(dataType);
	break;

    case DtDND_BUFFER_TRANSFER:
	bufPtr   = dcb->dropData->data.buffers[0].bp;
	bufLen   = dcb->dropData->data.buffers[0].size;
	dataType = DtDtsBufferToDataType(bufPtr, bufLen, NULL);
	if (strcmp(dataType, DataType) != 0) {
	    DtDtsFreeDataType(dataType);
	    return;
	}
	if (LoadBuffer(wd, bufPtr, bufLen))
            dcb->status = DtDND_SUCCESS;
	DtDtsFreeDataType(dataType);
	break;
    }

    XClearArea(XtDisplay(drawingArea), XtWindow(drawingArea),
	0, 0, 0, 0, True);
}


/*
 * Set the widget's title to the program name followed by a separator followed
 * by the trailing pathname component of the filename.  The widget must be a
 * shell.  If name is NULL, uses "(untitled)" instead.
 */
static void
SetTitle(Widget w, char *name)
{
    char buf[1000];
    char *p;

    if (name == NULL) {
	p = untitledString;
    } else {
	p = strrchr(name, '/');
	if (p == NULL)
	    p = name;
	else
	    p++;
    }
    sprintf(buf, "%s%s%s", appnameString, separatorString, p);
    XtVaSetValues(w, XtNtitle, buf, NULL);
}


/*
 * Create a new WindowData structure.  The widget passed in should be the 
 * shell associated with this data.  It must be realized.
 */
static WindowData *
NewData(void)
{
    WindowData *wd;

    wd = XtNew(WindowData);
    wd->npoints = 0;
    wd->nalloc = 0;
    wd->points = NULL;
    wd->shell = NULL;
    wd->openDialog = NULL;
    wd->saveDialog = NULL;
    wd->name = NULL;

    /* push it onto the front of the global list */
    wd->next = windowList;
    windowList = wd;

    return wd;
}


/*
 * Associate a top-level shell with a WindowData structure.  The shell must be 
 * realized.
 */
static void
AssocData(WindowData *wd, Widget w)
{
    wd->shell = w;
    XSaveContext(XtDisplay(w), XtWindow(w), wdContext, (XPointer)wd);
}


/*
 * Given a widget, find the WindowData structure associated with it.  First it 
 * finds the shell ancestor of this widget, and then it extracts the window 
 * data from using the X Context Manager.
 */
static WindowData *
FindData(Widget w)
{
    WindowData *wd = NULL;

    while (! XtIsShell(w))
	w = XtParent(w);
    XFindContext(XtDisplay(w), XtWindow(w), wdContext, &wd);
    return wd;
}


/*
 * Destroy a WindowData structure.
 */
static void
DestroyData(WindowData *wd)
{
    WindowData **p;

    FreeData(wd);

    if (wd->shell != NULL)
	XtDestroyWidget(wd->shell);
    if (wd->openDialog != NULL)
	XtDestroyWidget(wd->openDialog);
    if (wd->saveDialog != NULL)
	XtDestroyWidget(wd->saveDialog);

    /* remove from the global list */

    p = &windowList;
    while (*p != NULL) {
	if (*p == wd) {
	    *p = wd->next;
	    break;
	}
	p = &((*p)->next);
    }

    XtFree((char *)wd);
}

/*
 * Load a .template data file
 */

static Boolean LoadFile(WindowData *wd, char *fileName)
{
    FILE *fp;
    int np, i, x, y;
    char sig[100];

    if ((fp = fopen(fileName, "r")) == NULL)
	return False;

    if (fgets(sig, sizeof(sig), fp) == NULL) {
	fclose(fp);
	return False;
    }

    if (strcmp(sig, FileSignature) != 0) {
	fclose(fp);
	return False;
    }

    FreeData(wd);
    while (fscanf(fp, "%d %d", &x, &y) != EOF)
	AddPoint(wd, x, y);
    fclose(fp);
    if (wd->shell != NULL)
	SetTitle(wd->shell, fileName);
    wd->name = XtNewString(fileName);
    return True;
}


/*
 * Load a .template buffer
 */

static Boolean LoadBuffer(WindowData *wd, void *buf, int len)
{
    char *bufp = (char *)buf;
    char *endp = bufp + len;
    int r, x, y, l;
    char sig[100];

    (void) strncpy(sig, bufp, sizeof(FileSignature) - 1);
    sig[sizeof(FileSignature) - 1] = '\0';
    if (strcmp(sig, FileSignature) != 0)
	return False;
    bufp += sizeof(FileSignature) - 1;

    FreeData(wd);
    while (bufp < endp) {
	if (sscanf(bufp, "%d %d\n%n", &x, &y, &l) != 2)
	    return False;
	AddPoint(wd, x, y);
	bufp += l;
    }
    if (wd->shell != NULL)
	SetTitle(wd->shell, NULL);
    return True;
}


static Tt_message
HandleTtMedia(
    Tt_message		msg,
    void		*clientdata,
    Tttk_op		op,
    Tt_status		diagnosis,
    unsigned char	*contents,
    int			len,
    char		*file,
    char		*docname)
{
    int mark = tt_mark();
    char *opstr = tt_message_op(msg);

    if (strcmp(opstr, "Edit") == 0) {
	if (file != NULL) {
	    if (NewWindow(LOAD_FILE, file, 0))
		tt_message_reply(msg);
	    else
		tttk_message_fail(msg, TT_ERR_OP, "open failed", False);
	} else if (contents != NULL && len > 0) {
	    if (NewWindow(LOAD_BUFFER, (char *)contents, len))
		tt_message_reply(msg);
	    else
		tttk_message_fail(msg, TT_ERR_OP, "load buffer failed", False);
	} else {
	    tttk_message_fail(msg, TT_ERR_OP, "no file or buffer", False);
	}
    } else {
	tttk_message_fail(msg, TT_ERR_OP, "unsupported message", False);
    }

    tt_release(mark);
    tt_free((char *)contents);
    tt_free(file);
    tt_free(docname);
    tt_message_destroy(msg);
    return 0;
}


/*
 * Print the drawing data.
 *
 * This function is intentionally left blank.
 */

static void PrintData(WindowData *wd)
{
    FILE *PS;
    int i;

    PS = popen("/usr/bin/lp", "w");

    fputs("%!\n", PS);
    fputs("clippath pathbbox 0 exch translate pop pop pop\n", PS);
    fputs("1 -1 scale 72 72 translate\n", PS);
    fputs("newpath 0.5 setgray\n", PS);
    fputs("/S { 10 0 360 arc fill } def\n", PS);

    for (i = 0; i < wd->npoints; i++)
	fprintf(PS, "%d %d S\n", wd->points[i].x, wd->points[i].y);

    fputs("showpage\n", PS);
    (void) pclose(PS);
}


/*
 * Free drawing data structure
 */

static void FreeData(WindowData *wd)
{
    XtFree((char *)wd->points);
    XtFree(wd->name);
    wd->points = NULL;
    wd->name = NULL;
    wd->npoints = 0;
    wd->nalloc = 0;
    if (wd->shell != NULL)
	SetTitle(wd->shell, NULL);
}


/*
 * Add a point to the end of the drawing data structure.
 */

static void AddPoint(WindowData *wd, int x, int y)
{
    if (wd->npoints == wd->nalloc) {
	wd->nalloc += MallocInc;
	wd->points =
	    (XPoint*)XtRealloc((char*)wd->points, wd->nalloc * sizeof(XPoint));
    }
    wd->points[wd->npoints].x = x;
    wd->points[wd->npoints].y = y;
    wd->npoints += 1;
}


/*
 * Draw an airbrush at (x,y)
 */

static void DrawPoint(Widget w, int x, int y)
{
    static Boolean initialized = False;
    static GC gc;
    static Pixmap pixmap;
    static unsigned int pixmapWidth, pixmapHeight;

    if (XtIsRealized(w) == False) return;

    if (initialized == False) {
	Pixel fg, bg;
	unsigned int tmpu;
	Window tmpw;
	int tmpi;
	XGCValues gcv;
	unsigned long gcm;

	XtVaGetValues(w, XmNforeground, &fg, XmNbackground, &bg, NULL);
	pixmap = XmGetPixmapByDepth(XtScreen(w), "template-brush.bm", 1, 0, 1);
	if (pixmap == XmUNSPECIFIED_PIXMAP) {
	    XtAppWarning(appContext, "template:template-brush.bm not installed!");
	    /* use hardcoded fallback bitmap */
	    pixmap = XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				(char*)default_brush_bits,
				default_brush_width,
				default_brush_height,
				1, 0, 1);
	}
	XGetGeometry(XtDisplay(w), pixmap, &tmpw, &tmpi, &tmpi,
			&pixmapWidth, &pixmapHeight, &tmpu, &tmpu);

	gcm = GCForeground | GCBackground | GCFillStyle | GCStipple;
	gcv.foreground = fg;
	gcv.background = bg;
	gcv.fill_style = FillStippled;
	gcv.stipple = pixmap;
	gc = XCreateGC(XtDisplay(w), XtWindow(w), gcm, &gcv);

	initialized = True;
    };

    XSetTSOrigin(XtDisplay(w), gc, x-pixmapWidth/2, y-pixmapWidth/2);
    XFillRectangle(XtDisplay(w), XtWindow(w), gc,
			x-pixmapWidth/2, y-pixmapWidth/2,
			pixmapWidth, pixmapHeight);
}

