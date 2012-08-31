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
/* $TOG: Main.c /main/11 1998/04/20 12:58:06 mgreess $ */
/*
 * dtpdm/Main.c
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <stdlib.h>
#include <Xm/XmAll.h>
#include <Xm/ColorObjP.h>       /* for Xme Color Obj access API */
#include <Dt/dtpdmd.h>
#include <Dt/EnvControlP.h>

#include "MainWindow.h"
#include "PrinterBox.h"
#include "JobBox.h"
#include "PdmMsgs.h"

/*
 * PDM Shell
 */
typedef struct _PdmShell
{
    /*
     * application shell widget data
     */
    XtAppContext app_context;
    Display* display;
    Window parent_window;
    XWindowAttributes parent_attr;
    Widget widget;
    /*
     * command line parameters
     */
    String print_display_spec;
    String print_context_str;
    
} PdmShell;

/*
 * application resources
 */
static XrmOptionDescRec PdmOptions[] =
{
   { "-window", "parentWindowStr", XrmoptionSepArg, (caddr_t)0    },
   { "-pdisplay", "printDisplaySpec", XrmoptionSepArg, (caddr_t)NULL },
   { "-pcontext", "printContextStr", XrmoptionSepArg, (caddr_t)0 }
};

static XtResource PdmResources[] =
{
    { "printDisplaySpec", "PrintDisplaySpec", XmRString, sizeof(String),
      XtOffsetOf(PdmShell, print_display_spec), XmRImmediate, (XtPointer)NULL,
    },
    { "printContextStr", "PrintContextStr", XmRString, sizeof(String),
      XtOffsetOf(PdmShell, print_context_str), XmRImmediate, (XtPointer)NULL,
    }
};

/*
 * fallback resources
 */
static String PdmShellFallbackResources[] =
{
    "Dtpdm.title: Print Setup"
};

/*
 * static function declarations
 */
static PdmShell* PdmShellNew();
static int PdmShellCreate(PdmShell*, const String*, int*, String*);
static void PdmShellPresent(PdmShell* me);
static Boolean PdmStartOkWorkProc(XtPointer client_data);

/*
 * ------------------------------------------------------------------------
 * Name: PdmShellNew
 *
 * Description:
 *
 *     Allocates a new PdmShell data structure.
 *
 * Return value:
 *
 *     A pointer to the new PdmShell structure.
 *
 */
static PdmShell*
PdmShellNew()
{
    return (PdmShell*)XtCalloc(1, sizeof(PdmShell));
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmShellCreate
 *
 * Description:
 *
 *     This function creates a top level application shell.
 *
 * Return value:
 *
 *     0 if successful; a PDM_EXIT code if not.
 *
 */
static int
PdmShellCreate(PdmShell* me,
	       const String* fallback_resources,
	       int* argc_in_out, String* argv_in_out)
{
    String app_class;
    String app_name;
    XmPixelSet pixelSets[XmCO_NUM_COLORS];
    XrmClass class_list[3];
    XrmDatabase db;
    XrmName name_list[3];
    XrmRepresentation rep_type;
    XrmValue value;
    int decor;
    int funcs;
    short secondary_id;
    /*
     * create the application context and open the video display
     */
    XtToolkitInitialize();
    XtSetLanguageProc((XtAppContext)NULL, (XtLanguageProc)NULL,
		      (XtPointer)NULL);
    me->app_context = XtCreateApplicationContext();

    XtAppSetFallbackResources(me->app_context, (String*)fallback_resources);

    me->display =
	XtOpenDisplay(me->app_context, (String)NULL,
		      (String)NULL, "Dtpdm",
		      PdmOptions, XtNumber(PdmOptions),
		      argc_in_out, argv_in_out);
    if((Display*)NULL == me->display)
	return PDM_EXIT_VXAUTH;
    XtGetApplicationNameAndClass(me->display, &app_name, &app_class);
    /*
     * the fallback resources are only intended for the video display, so
     * remove them from the application context
     */
    XtAppSetFallbackResources(me->app_context, (String*)NULL);
    /*
     * get the parent video window id from the display's resource database
     */
    db = XtDatabase(me->display);
    name_list[0] = XrmStringToQuark(app_name);
    name_list[1] = XrmStringToQuark("parentWindowStr");
    name_list[2] = NULLQUARK;
    class_list[0] = XrmStringToQuark(app_class);
    class_list[1] = XrmStringToQuark("ParentWindowStr");
    class_list[2] = NULLQUARK;
    XrmQGetResource(db, name_list, class_list, &rep_type, &value);
    if(!value.addr) return PDM_EXIT_ERROR;
    me->parent_window = (Window)strtoul((char*)value.addr, (char**)NULL, 0);
    /*
     * obtain the parent video window's attributes
     */
    if(!XGetWindowAttributes(me->display, me->parent_window, &me->parent_attr))
	return PDM_EXIT_ERROR;
    /*
     * register interest in the DestroyNotify event for the parent window
     */
    XSelectInput(me->display, me->parent_window, StructureNotifyMask);
    /*
     * create the application shell
     */
    decor = MWM_DECOR_ALL | MWM_DECOR_RESIZEH | MWM_DECOR_MINIMIZE
	| MWM_DECOR_MAXIMIZE;
    funcs = MWM_FUNC_ALL | MWM_FUNC_RESIZE | MWM_FUNC_MINIMIZE
	| MWM_FUNC_MAXIMIZE;
    me->widget = 
	XtVaAppCreateShell(app_name, app_class,
			   applicationShellWidgetClass,
			   me->display,
			   XmNdeleteResponse, XmDESTROY,
			   XmNmappedWhenManaged, False,
			   XmNmwmDecorations, decor,
			   XmNmwmFunctions, funcs,
			   XmNmwmInputMode,MWM_INPUT_PRIMARY_APPLICATION_MODAL,
			   XmNscreen, me->parent_attr.screen,
			   XmNtransient, True,
			   NULL);
    /*
     * pick up the secondary color set, so that the PDM presents the same
     * colors as a secondary window. (the XmColorObj is created by the
     * the XmDisplay object, which is created when the app shell is
     * created)
     */
    if(XmeGetColorObjData(XtScreen(me->widget), (int*)NULL,
			  pixelSets, XmCO_NUM_COLORS,
			  (short*)NULL, (short*)NULL, (short*)NULL,
			  &secondary_id, (short*)NULL))
    {
	/*
	 * In the Xrm database, reassign primary color resource values
	 * with the corresponding secondary color values. This will cause
	 * subsequently created widgets that utilize colors from the
	 * primary set to actually present secondary colors.
	 */
	db = XtScreenDatabase(XtScreen(me->widget));
	value.size = sizeof(Pixel);
	value.addr = (char*)&pixelSets[secondary_id].bg;
	XrmPutResource (&db, "*background", "Pixel", &value);
	XrmPutResource (&db, "*frameBackground", "Pixel", &value);
	value.addr = (char*)&pixelSets[secondary_id].fg;
	XrmPutResource (&db, "*foreground", "Pixel", &value);
	XrmPutResource (&db, "*backPageForeground", "Pixel", &value);
	value.addr = (char*)&pixelSets[secondary_id].bs;
	XrmPutResource (&db, "*bottomShadowColor", "Pixel", &value);
	value.addr = (char*)&pixelSets[secondary_id].ts;
	XrmPutResource (&db, "*topShadowColor", "Pixel", &value);
    }
    /*
     * get the application resources
     */
    XtGetApplicationResources(me->widget, me,
			      PdmResources, XtNumber(PdmResources),
			      NULL, 0);
    /*
     * return
     */
    return 0;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmShellPresent
 *
 * Description:
 *
 *     Displays the application shell as a transient for the window
 *     passed via the -window command line parameter.
 *
 *
 * Return value:
 *
 *     True if successful.
 *
 */
static void
PdmShellPresent(PdmShell* me)
{
    int parent_abs_x, parent_abs_y;
    Position x, y;
    Dimension width, height;
    Window dummy_child;
    /*
     * get the absolute screen coordinates of the parent window
     */
    XTranslateCoordinates(me->display, me->parent_window,
			  me->parent_attr.root, 
			  0, 0,
			  &parent_abs_x,
			  &parent_abs_y,
			  &dummy_child);
    /*
     * get the dimensions of the PDM window
     */
    XtRealizeWidget(me->widget);
    XtVaGetValues(me->widget, XmNwidth, &width, XmNheight, &height, NULL);
    /*
     * determine the origin of the PDM popup such that it is
     * centered w.r.t. the parent window
     */
    x = parent_abs_x + me->parent_attr.width/2 - width/2;
    y = parent_abs_y + me->parent_attr.height/2 - height/2;
    /*
     * update the app shell position
     */
    XtVaSetValues(me->widget, XmNx, x, XmNy, y, NULL);
    /*
     * present the PDM as transient for the passed video window
     *
     * Note: this only works if the passed video window is a
     *       top-level window; i.e. one that the window manager
     *       manages.
     */
    XSetTransientForHint(me->display, XtWindow(me->widget),
			 me->parent_window);
    /*
     * map the PDM popup
     */
    XtMapWidget(me->widget);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmStartOkWorkProc
 *
 * Description:
 *
 *     Send the PDM_START_OK message to the dtpdmd on stderr. This
 *     routine is an Xt work proc that is intended to be invoked by Xt
 *     following the dispatch of all events generated during
 *     startup. This function is only called once during the life of the
 *     app.
 *
 * Return value:
 *
 *     True, which causes Xt to remove this work proc.
 *
 */
static Boolean
PdmStartOkWorkProc(XtPointer client_data)
{
    fprintf(stderr, "PDM_START_OK");
    fflush(stderr);
    return True;
}

/*
 * ------------------------------------------------------------------------
 * Name: main
 *
 * Description:
 *
 *     main function for the DtPrint Dialog Manager.
 *
 * Return value:
 *
 *     This function never returns.
 *
 */
int main(int argc, char* argv[])
{
    PdmShell* pdm_shell;
    PdmMainWin* pdm_main_window;
    String* fallback_resources;
    int status;
    XEvent event;

    /*
     * Initialize environment so we can find message catalog.
     */
    _DtEnvControl(DT_ENV_SET);

    /*
     * ensure we can convert Xp attributes for this locale
     */
    if(!XSupportsLocale())
    {
	fprintf(stderr, "%s\n", PDM_MSG_ERR_X_LOCALE);
	exit(PDM_EXIT_ERROR);
    }
    /*
     * create the PDM objects for the app shell, the main window, and the
     * setup boxes
     */
    pdm_shell = PdmShellNew();
    pdm_main_window = PdmMainWinNew();
    PdmMainWinAddSetupBox(pdm_main_window, PdmPrinterBoxNew());
    PdmMainWinAddSetupBox(pdm_main_window, PdmJobBoxNew());
    /*
     * create the application shell widget
     */
    fallback_resources =
	PdmMainWinMergeFallbacks(pdm_main_window,
				 PdmShellFallbackResources,
				 XtNumber(PdmShellFallbackResources));
    status = PdmShellCreate(pdm_shell, fallback_resources, &argc, argv);
    if(0 != status)
    {
	PdmMainWinDelete(pdm_main_window);
	exit(status);
    }
    XtFree((char*)fallback_resources);
    /*
     * create the main PDM window widget hierarchy
     */
    PdmMainWinCreate(pdm_main_window,
		     pdm_shell->widget,
		     pdm_shell->print_display_spec,
		     pdm_shell->print_context_str);
    /*
     * display the PDM
     */
    PdmShellPresent(pdm_shell);
    /*
     * signal the PDM is up following startup event processing
     */
    XtAppAddWorkProc(pdm_shell->app_context,
		     PdmStartOkWorkProc, (XtPointer)NULL);
    /*
     * main loop
     */
    for(;;)
    {
	/*
	 * get the next event
	 */
	XtAppNextEvent(pdm_shell->app_context, &event);
	/*
	 * check to see if the parent video window has been destroyed
	 */
	if(event.xany.display == pdm_shell->display
	   &&
	   event.xany.window == pdm_shell->parent_window
	   &&
	   event.xany.type == DestroyNotify)
	{
	    PdmMainWinDelete(pdm_main_window);
	    exit(PDM_EXIT_CANCEL);
	}
	else
	    XtDispatchEvent(&event);
    }
    /*
     * not reached
     */
    return 0;
}
