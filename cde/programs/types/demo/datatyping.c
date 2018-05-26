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
/* $XConsortium: datatyping.c /main/3 1995/11/03 16:13:13 rswiston $ */
/****************************************************************************/
/*****************************************************************************
 **
 **   File:         datatyping.c
 **
 **   Description:  This file contains the datatyping program that
 **		    demonstrates the use of the Data Typing API.
 **
 **		    This program will print the data type, icon
 **	            and supported actions for each file given.
 **                 The dtactiondemo program can then be used to
 **                 execute a supported action on the file.
 **
 **   		    The usage for datatyping is:
 **
 **		    Usage: datatyping file1 [ file2 ... ]
 **
 **
 **   (c) Copyright 1993, 1994 Hewlett-Packard Company
 **   (c) Copyright 1993, 1994 International Business Machines Corp.
 **   (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **   (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **       Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <Xm/Xm.h>
#include <Dt/Dts.h>
#include <Dt/DbUtil.h>


/***********************  Startup Routine ****************************/
static void
startup(int argc, char **argv)
{
	Widget          toplevel;
	Arg             args[5];
	DtDirPaths	*dirPaths;

	toplevel = XtInitialize(argv[0], "Dtdatatyping", NULL, 0,
		(int *) &argc, argv);

	XtSetArg(args[0], XmNallowShellResize, True);
	XtSetArg(args[1], XmNmappedWhenManaged, False);
	XtSetArg(args[2], XmNheight, 1);
	XtSetArg(args[3], XmNwidth, 1);
	XtSetValues(toplevel, args, 4);
	XtRealizeWidget(toplevel);

	if( DtInitialize(XtDisplay(toplevel), toplevel, argv[0],
			  "Dt_TYPE") == False)
	{
		printf(" couldn't initialize everything\n");
		exit(1);
	}

	DtDtsLoadDataTypes();
}



/***********************  Cleanup Routine ****************************/
static void
cleanup()
{
	DtDtsRelease();
}

main(int argc, char **argv)
{
	char		*file;
	char		*datatype;
	char		*icon;
	char		*actions;

	startup(argc, argv);

	printf("%-30s\t%-10s\t%-8s\t%-20s\n",
			"File",
			"DataType",
			"Icon",
			"Actions");
	printf("%-30s\t%-10s\t%-8s\t%-20s\n",
			"-------------------",
			"--------",
			"----",
			"-------");
	for(argv++; file = *argv; argv++)
	{
		/* find out the data type */
		datatype = DtDtsFileToDataType(file);

		if(datatype)
		{
			/* find the attributes for that data type */
			icon = DtDtsDataTypeToAttributeValue(datatype, 
				DtDTS_DA_ICON, file);
		}

		/* or an alternate/more direct way */
		actions = DtDtsFileToAttributeValue(file,
						DtDTS_DA_ACTION_LIST);
		
		printf("%-30s\t%-10s\t%-8s\t%-28s\n",
				file,
				datatype?datatype:"unknown",
				icon?icon:"unknown",
				actions?actions:"unknown");

		DtDtsFreeAttributeValue(icon);
		DtDtsFreeAttributeValue(actions);
		DtDtsFreeDataType(datatype);
	}

	cleanup();

	exit(0);
}

