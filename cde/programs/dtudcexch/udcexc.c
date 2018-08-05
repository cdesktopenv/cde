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
/* $XConsortium: udcexc.c /main/4 1996/09/02 18:50:32 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "excutil.h"
#include "xoakufont.h"
#include "selectxlfd.h"
#include <locale.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/MessageB.h>
#include <Xm/MainW.h>
void callexpCB();
void callimpCB();
void exitCB();
void helpCB();
void udcexp();
void udcimp();

#define ERROR_1 errmsg_org[fal_utyerrno & 0xff]
/*#define ERROR_1 fal_errmsg_org[fal_utyerrno & 0xff]*/
/*#define ERROR_2 fal_errmsg_func[fal_utyerrno >>8]*/

Widget toplevel, mainw;

Exc_data *ed = NULL;
ListData	*ld = NULL;
char *maintitle;
/*char *terminatemsg = "Do you want to terminate dtudcexch";*/

/*
 * Error messages(by origin)
 * fal_errmsg_org
 */
char	*errmsg_org[0xff];

Rsrc resource ;

#define Rchar(name, class, member,def){ \
        name, class, XtRString, sizeof(char *), \
        XtOffsetOf(Rsrc, member), XtRString, (XtPointer)def }

Widget excCreateRadioButton(Widget parent, String name, String label,
			    XtCallbackProc callback_func,
			    XtPointer client_data)
{
    Widget	widget;
    XmString	xms;

    widget = XtVaCreateManagedWidget(name,
				     xmToggleButtonWidgetClass, parent,
				     XmNindicatorType, XmONE_OF_MANY,
				     NULL);
    XtAddCallback(widget, XmNvalueChangedCallback,
		  (XtCallbackProc) callback_func,
		  (XtPointer) client_data);

    xms = XmStringCreateLocalized(label);
    XtVaSetValues(widget,
		  XmNlabelString, xms,
		  NULL);

    XmStringFree(xms);

    return (widget);
}

int main(int argc, char * argv[])
{
    XtAppContext	app_context;
    Widget		row, expfunc, impfunc, exit/*, help*/;
    Arg			args[20];
    Cardinal		n;
    static char		*explabel;
    static char		*implabel;
    static char		*exitlabel;
/*    static char		helplabel[] = "Help";*/
/*    char                msg1[] = "Failed in the start of dtudcexch";*/
    char	err[128];
    int                 ans;

    int mask = FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED;
    FalFontData key;
    FalFontDataList *fontlist;

/* appinitialize */
    XtSetLanguageProc(NULL,NULL,NULL);
    _DtEnvControl(0);
    n = 0;
    toplevel = XtAppInitialize(&app_context, "UDCexchange",
			       (XrmOptionDescList) NULL,
			       0, (int *) &argc, argv, (String *) NULL,
			       args, n);

    maintitle = GETMESSAGE(8, 2, "UDC data exchange utitlity");

    errmsg_org[0] = GETMESSAGE(8, 4, "Fatal error occurred.");
    errmsg_org[1] = GETMESSAGE(8, 6, "Cannot open the font file.");
    errmsg_org[2] = GETMESSAGE(8, 8, "Cannot read the font file.");
    errmsg_org[3] = GETMESSAGE(8, 10, "There is no more memory.");
    errmsg_org[4] = GETMESSAGE(8, 12, "Fatal error occurred.");
    errmsg_org[5] = GETMESSAGE(8, 14, "The specified font file does not exist.");
    errmsg_org[6] = GETMESSAGE(8, 16, "This font is not a pcf or snf font.");
    errmsg_org[7] = GETMESSAGE(8, 18, "Cannot open fonts.list file.");
    errmsg_org[8] = GETMESSAGE(8, 20, "The format of fonts.list file is illegal.");
    errmsg_org[9] = GETMESSAGE(8, 22, "The descriptions of the fonts.list file are incorrect.");
    errmsg_org[10] = GETMESSAGE(8, 24, "The format of fonts.list file is illegal.");
    errmsg_org[11] = GETMESSAGE(8, 26, "Cannot open fonts.dir file.");
    errmsg_org[12] = GETMESSAGE(8, 28, "Cannot read fonts.dir file.");
    errmsg_org[13] = GETMESSAGE(8, 30, "Cannot read font properties.");
    errmsg_org[14] = GETMESSAGE(8, 32, "Cannot get FONT property.");
    errmsg_org[15] = GETMESSAGE(8, 34, "Cannot get FAMILY_NAME property.");
    errmsg_org[16] = GETMESSAGE(8, 36, "This font file is already opened by other application.");
    errmsg_org[17] = GETMESSAGE(8, 38, "Cannot lock font file.");
    errmsg_org[18] = GETMESSAGE(8, 40, "Cannot unlock font file.");
    errmsg_org[19] = GETMESSAGE(8, 42, "Cannot get lock information from the font file.");
    errmsg_org[20] = GETMESSAGE(8, 44, "Cannot find the specified font file.");
    errmsg_org[21] = GETMESSAGE(8, 46, "Cannot read NLS database.");
    errmsg_org[22] = GETMESSAGE(8, 48, "Cannot get charset names from NLS database.");
    errmsg_org[23] = GETMESSAGE(8, 50, "Charset name not defined in NLS database.");
    errmsg_org[24] = GETMESSAGE(8, 52, "The specified font has not been opened.");
    errmsg_org[25] = GETMESSAGE(8, 54, "Fatal error occurred.");

    resource.xlfd_label = GETMESSAGE(8, 56, "XLFD name          :" ),
    resource.copy_xlfd_label = GETMESSAGE(8, 58, "Original font (XLFD name)  :");
    resource.code_label = GETMESSAGE(8, 60, "UDC code area      :");
    resource.style_label = GETMESSAGE(8, 62, "Style              :");
    resource.size_label = GETMESSAGE(8, 64, "Size (letter size) :");
    resource.exec_label = GETMESSAGE(8, 66, "Open");
    resource.quit_label = GETMESSAGE(8, 68, "Cancel");
    resource.ok_label = GETMESSAGE(8, 70, "OK");
    resource.cancel_label = GETMESSAGE(8, 72, "cancel");
    resource.copy_orgin = GETMESSAGE(8, 74, "Copy orgin code(hex)  :");
    resource.copy_target = GETMESSAGE(8, 76, "Copy target code(hex) :");
    resource.copy_label = GETMESSAGE(8, 78, "Copy");
    resource.overlay_label = GETMESSAGE(8, 80, "Overlay");

    explabel = GETMESSAGE(8, 82, "export function");
    implabel = GETMESSAGE(8, 84, "import function");
    exitlabel = GETMESSAGE(8, 86, "Cancel");

/* initialize ed */
    if ((ed = (Exc_data *) malloc(sizeof(Exc_data))) == NULL) {
	excerror(ed, EXCERRMALLOC, "main", "exit");
    }
    memset(ed,0x00,sizeof(Exc_data));

    ed->app = app_context;
    ed->toplevel = toplevel;

    if (FalGetFontList(&key, mask, &fontlist) == FAL_ERROR) {
	fprintf( stderr, "GetFontList Err\n" );
	sprintf(err, "%s", ERROR_1);
	AskUser(ed->toplevel, ed, err, &ans, "error");
/*	AskUser(ed->toplevel, ed, msg1, &ans, "error");*/
	excexit(ed);
    }
    FalFreeFontList(fontlist);


/* make main window */
    n = 0;
    XtSetArg( args[n], XmNautoUnmanage, False ) ; n++ ;
    XtSetArg(args[n], XmNtitle, maintitle); n++;
    mainw = XmCreateTemplateDialog(toplevel, "mainw", args, n);

    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, mainw,
				  XmNradioBehavior, True,
				  XmNradioAlwaysOne, True,
				  NULL);
    expfunc = excCreateRadioButton(row, "expfunc", explabel,
				   (XtCallbackProc) callexpCB, (XtPointer) ed);

    impfunc = excCreateRadioButton(row, "impfunc", implabel,
				   (XtCallbackProc) callimpCB, (XtPointer) ed);

    exit = excCreatePushButton(mainw, "exit", exitlabel,
			       (XtCallbackProc) exitCB, (XtPointer) ed);

/*    help = excCreatePushButton(mainw, "help", helplabel,
			       (XtCallbackProc) helpCB, (XtPointer) ed);
*/
    XtVaSetValues(mainw,
		  XmNmessageWindow, exit,
		  NULL);

    XtManageChild(mainw);

/*    XtRealizeWidget(toplevel);*/

    XtAppMainLoop(app_context);
}

void callexpCB(Widget widget, Exc_data * ed, XtPointer call_data)
{
    XtUnmanageChild(mainw);
    udcexp(ed);
}
void callimpCB(Widget widget, Exc_data * ed, XtPointer call_data)
{
    XtUnmanageChild(mainw);
    udcimp(ed);
}
void exitCB(Widget widget, Exc_data * ed, XtPointer call_data)
{
    excexit(ed);
}
/*void helpCB(Widget widget, Exc_data * ed, XtPointer call_data)
{
}*/
