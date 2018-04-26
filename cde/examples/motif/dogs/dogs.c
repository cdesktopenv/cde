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
/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: dogs.c /main/3 1995/10/27 10:43:03 rswiston $"
#endif
#endif

/*****************************************************************************
*
*  dogs.c - Square & Dog widget demo source file.
*  
******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include "Square.h"
#include "Dog.h"

XtAppContext  app_context;

#define k_dog1_id 1
#define k_dog2_id 2
#define k_dog3_id 3
#define k_help_id 4

static void create_cb();
static void bark_cb();
static void tb_cb();
static void scale_cb();
static void help_cb();
static void exit_cb();

static MrmHierarchy mrm_id = 0;
static char *mrm_vec[]={"dogs.uid", NULL};
static MrmCode mrm_class;
static MRMRegisterArg mrm_names[] = {
        {"create_cb", (XtPointer)create_cb },
        {"bark_cb", (XtPointer)bark_cb },
        {"tb_cb", (XtPointer)tb_cb },
        {"scale_cb", (XtPointer)scale_cb },
        {"help_cb", (XtPointer)help_cb },
        {"exit_cb", (XtPointer)exit_cb }
};

static Widget dog1_id;
static Widget dog2_id;
static Widget dog3_id;
static Widget help_id;

main(argc, argv)
    int argc;
    char **argv;
{
    Widget shell;
    Display *display;
    Widget app_main = NULL;
    Arg args[3];

    MrmInitialize ();
    SquareMrmInitialize();
    DogMrmInitialize();

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    display = XtOpenDisplay(app_context, NULL, argv[0], "Dogs",
			NULL, 0, &argc, argv);
    
    if (display == NULL) {
	    fprintf(stderr, "%s:  Can't open display\n", argv[0]);
	    exit(1);
    }

    XtSetArg (args[0], XtNallowShellResize, True);
    XtSetArg (args[1], XtNminWidth, 620);
    XtSetArg (args[2], XtNminHeight, 370);
    shell = XtAppCreateShell(argv[0], NULL, applicationShellWidgetClass,
			  display, args, 3);

    if (MrmOpenHierarchy(1, mrm_vec, NULL, &mrm_id) != MrmSUCCESS) exit(0);
    MrmRegisterNames(mrm_names, XtNumber(mrm_names));
    MrmFetchWidget (mrm_id, "app_main", shell, &app_main, &mrm_class);

    XtManageChild(app_main);
    XtRealizeWidget(shell);
    XtAppMainLoop(app_context);
}

static void create_cb(w, id, reason)
    Widget w;
    int *id;
    unsigned long *reason;
{
    switch (*id) {
        case k_dog1_id: dog1_id = w; break;
        case k_dog2_id: dog2_id = w; break;
        case k_dog3_id: dog3_id = w; break;
        case k_help_id:
	    help_id = w;
	    XtUnmanageChild((Widget)XmMessageBoxGetChild(help_id,
				XmDIALOG_CANCEL_BUTTON));
	    XtUnmanageChild((Widget)XmMessageBoxGetChild(help_id,
				XmDIALOG_HELP_BUTTON));
	    break;
    }
}

static void bark_cb (w, volume, cb)
    Widget w;
    int *volume;
    XtPointer cb;
{
    XBell(XtDisplay(w), *volume);
}

static void tb_cb (w, tag, cb)
    Widget w;
    int *tag;
    XmToggleButtonCallbackStruct *cb;
{
    Arg args[1];
    Widget dog;

    switch (*tag) {
	case(1) : dog = dog1_id; break;
	case(2) : dog = dog2_id; break;	
	case(3) : dog = dog3_id; break;	
    }
    XtSetArg(args[0], SquareNmakeSquare, cb->set);
    XtSetValues(dog, args, 1);
}

static void scale_cb(w, tag, cb)
    Widget w;
    int *tag;
    XmScaleCallbackStruct *cb;
{
    Arg args[1];
    Widget dog;

    switch (*tag) {
	case(1) : dog = dog1_id; break;
	case(2) : dog = dog2_id; break;	
	case(3) : dog = dog3_id; break;	
    }
    XtSetArg(args[0], DogNwagTime, cb->value);
    XtSetValues(dog, args, 1);
}

static void help_cb (w, name, cb)
    Widget w;
    XmString name;
    XtPointer cb;
{
    Arg args[1];

    if (name == NULL) return;
    XtSetArg (args[0], XmNmessageString, name);
    XtSetValues(help_id, args, 1);
    XtManageChild(help_id);
}

static void exit_cb (w, name, cb)
    Widget w;
    XmString name;
    XtPointer cb;
{
    exit(0);
}

