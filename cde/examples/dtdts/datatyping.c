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
/* $XConsortium: datatyping.c /main/3 1995/10/27 10:39:52 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Dt/Dts.h>

#define ApplicationClass "DtDatatyping"

static Widget text;

static void DisplayTypeInfo(int, char**);

int main(int argc, char **argv)
{
    XtAppContext appContext;
    Widget toplevel, form;
    Arg args[20];
    int n;

    toplevel = XtAppInitialize(&appContext, ApplicationClass, NULL, 0,
 						&argc, argv, NULL, NULL, 0);

    if (argc == 1) {
	printf("%s: No files specified.\n", argv[0]);
	exit(1);
    }

    form = XmCreateForm(toplevel, "form", NULL, 0);
    XtManageChild(form);
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNeditable, False); n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg(args[n], XmNrows, 25); n++;
    XtSetArg(args[n], XmNcolumns, 90); n++;
    text = XmCreateScrolledText(form, "text", args, n);
    XtManageChild(text);

    XtRealizeWidget(toplevel);

    if (DtAppInitialize(appContext, XtDisplay(toplevel), toplevel, argv[0],
						ApplicationClass) == False) {
    	printf("%s: Couldn't initialize Dt\n", argv[0]);
    	exit(1);
    }

    DtDbLoad();

    DisplayTypeInfo(argc, argv);

    XtAppMainLoop(appContext);
}

static void DisplayTypeInfo(int argc, char **argv)
{
    char *file;
    char *datatype;
    char *icon;
    char *actions;
    char str[100];
    int i;

    sprintf(str, "%-30s\t%-10s\t%-8s\t%-20s\n",
    		"File",
    		"DataType",
    		"Icon",
    		"Actions");
    XmTextInsert(text, XmTextGetLastPosition(text), str);

    sprintf(str, "%-30s\t%-10s\t%-8s\t%-20s\n",
    		"-------------------",
    		"--------",
    		"----",
    		"-------");
    XmTextInsert(text, XmTextGetLastPosition(text), str);

    for(i=1; i < argc; i++) {
	char *file = argv[i];

    	/* find out the Dts data type */
    	datatype = DtDtsFileToDataType(file);

    	if(datatype) {
	    /* find the icon attribute for the data type */
	    icon = DtDtsDataTypeToAttributeValue(datatype, DtDTS_DA_ICON, file);
    	}

    	/*  Directly find the action attribute for a file */

    	actions = DtDtsFileToAttributeValue(file, DtDTS_DA_ACTION_LIST);
    	
    	sprintf(str, "%-30s\t%-10s\t%-8s\t%s\n",
			file,
			datatype?datatype:"unknown",
			icon?icon:"unknown",
			actions?actions:"unknown");
	XmTextInsert(text, XmTextGetLastPosition(text), str);

	/* Free the space allocated by Dts */

    	DtDtsFreeAttributeValue(icon);
    	DtDtsFreeAttributeValue(actions);
    	DtDtsFreeDataType(datatype);
    }
}

