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
/* $XConsortium: Main.c /main/6 1996/06/07 12:04:30 daniel $ */
#include "PrintDemo.h"

/*
 * VideoShell structure definition
 */
typedef struct _VideoShell
{
    Widget widget;
    Boolean print_only;
    String file_name;
} VideoShell;

/*
 * application-level resources
 */
static XrmOptionDescRec XrmOptions[] =
{
   {"-print", "printOnly", XrmoptionNoArg, (caddr_t)"True"},
   {"-fileName", "fileName", XrmoptionSepArg, (caddr_t)NULL},
};
static XtResource VideoResources[] =
{
  {"printOnly", "PrintOnly", XmRBoolean, sizeof (Boolean),
	XtOffsetOf (VideoShell, print_only), XmRImmediate, (XtPointer)False,
  },
  {"fileName", "FileName", XmRString, sizeof (char *),
	XtOffsetOf (VideoShell, file_name), XmRImmediate, (XtPointer)NULL,
  },
};

/*
 * static function declarations
 */
static VideoShell* VideoShell_new(Display* display);

/*
 * ------------------------------------------------------------------------
 * Name: VideoShell_new
 *
 * Description:
 *
 *     Allocates a new VideoShell data structure.
 *
 *     This function creates a top level application shell on the passed
 *     video display.
 *
 * Return value:
 *
 *     A pointer to the new VideoShell structure.
 */
static VideoShell*
VideoShell_new(Display* display)
{
    VideoShell* me = (VideoShell*)XtCalloc(1, sizeof(VideoShell));

    me->widget = XtVaAppCreateShell(NULL, APP_CLASS,
				    applicationShellWidgetClass,
				    display,
				    XmNtitle, "DtPrint Demo",
				    NULL);
    XtGetApplicationResources(me->widget, me,
			      VideoResources, XtNumber(VideoResources),
			      NULL, 0);
    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: CloseProgramCB
 *
 * Description:
 *
 *     Exit the program.
 *
 * Return value:
 *
 *     None.
 */
void
CloseProgramCB(
		Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    AppPrintData * p = (AppPrintData *) client_data ;
    /* we want to wait for the current job to complete before exiting */

    /* if a job is running, just unmap the windows and install itself
       as endjob callback, which will be called when printed_lines is
       back to zero */
    if (p->printed_lines) {
	/* put up a dialog saying it's waiting for the job
	   to complete */
	XtAddCallback(p->print_shell, XmNendJobCallback, CloseProgramCB, p);
    } else {
	exit(0);
    }
}

static String fallbacks[] = {
"Dtprint.Print*background:white",
"Dtprint.Print*renderTable:-dt-application-bold-r-normal-serif-0-0-0-0-p-0-iso8859-1",
"Dtprint.Print*shadowThickness:0",
"Dtprint.Print*highlightThickness:0",
"Dtprint.Print*pform.marginHeight: 1in",
"Dtprint.Print*pform.marginWidth: 1in",
"Dtprint.Print*ptext.Attachment:attach_form",
NULL
};

/*
 * ------------------------------------------------------------------------
 * Name: main
 *
 * Description:
 *
 *     "main" function for the DtPrint demo program.
 *
 *
 */
int main(int argc, char* argv[])
{
    XtAppContext app_context;
    VideoShell* video_shell;
    MainWindow* main_window;
    Display* video_display;
    AppPrintData* p;
    /*
     * attempt to open the X video display
     */
    XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    video_display = XtOpenDisplay(app_context, NULL, NULL, APP_CLASS,
				  XrmOptions, XtNumber(XrmOptions),
				  &argc, argv);
    XtAppSetFallbackResources(app_context, fallbacks);
    if(video_display == (Display*)NULL)
    {
	/*
	 * parse command line and determine if "GUI-less" printing is
	 * desired
	 */

	/*
	 * XXX  exit for now
	 */
	fprintf(stderr, "unable to open display\n");
	return 1;
    }
    /*
     * Create the top level video shell
     */
    video_shell = VideoShell_new(video_display);
    /*
     * one AppPrintData object per app
     */
    p = AppPrintData_new();
    p->print_only = video_shell->print_only;
    /*
     * check to see if we're running the app, or just printing (e.g. from
     * within a print action)
     */
    if(video_shell->print_only)
    {
	/*
	 * create the application-specific object, and add it to the
	 * AppPrintData structure.
	 */
	p->app_object = AppObject_new((Widget)NULL, video_shell->file_name);
	/*
	 * create the print setup box as the child of the top level shell
	 */
	CreatePrintSetup(video_shell->widget, p);
	/*
	 * set the cancel button to exit the program
	 */
	XtAddCallback(p->print_dialog, DtNcancelCallback, CloseProgramCB, p);
	/*
	 * manage the print setup box
	 */
	XtManageChild(p->print_dialog);
    }
    else
    {
	/*
	 * create the main window
	 */
	main_window = MainWindow_new(video_shell->widget);
	/*
	 * add callbacks to the main window
	 */
	XtAddCallback(main_window->print_menu_button, XmNactivateCallback,
		      PrintMenuCB, p);
	p->pr_button = main_window->print_menu_button;

	XtAddCallback(main_window->quick_print_button, XmNactivateCallback,
		      QuickPrintCB, p);
	XtAddCallback(main_window->exit_button, XmNactivateCallback,
		      CloseProgramCB, p);
	/*
	 * create the application-specific object, and add it to the
	 * AppPrintData structure.
	 */
	p->app_object =
	    AppObject_new(main_window->widget, video_shell->file_name);
	/*
	 * manage the main window
	 */
	XtManageChild(main_window->widget);
    }
    /*
     * main loop
     */
    XtRealizeWidget(video_shell->widget);
    XtAppMainLoop(app_context);
    /*
     * we never get here, but this makes the compiler happy
     */
    return 0;
}
