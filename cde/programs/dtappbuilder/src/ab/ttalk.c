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
 *	$XConsortium: ttalk.c /main/3 1995/11/06 17:55:44 rswiston $
 *
 * @(#)ttalk.c	1.9 17 Feb 1994
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


#include <stdio.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/TextF.h>
#include <Xm/BulletinB.h>
#include <Xm/DialogS.h>
#include <ab_private/ui_util.h>
#include <ab_private/ab.h>
#include <ab_private/tooltalk_ui.h>

extern Widget		AB_ttalk_dialog;

static void             ttalk_applyCB(
                            Widget      widget,
                            XtPointer   client_data,
                            XtPointer   call_data
                        );

static void             ttalk_resetCB(
                            Widget      widget,
                            XtPointer   client_data,
                            XtPointer   call_data
                        );  

static void             ttalk_cancelCB(
                            Widget      widget,
                            XtPointer   client_data,
                            XtPointer   call_data
                        );

static void		list_add_msgs(
			    Widget      widget,
                            XtPointer   client_data,
                            XtPointer   call_data
                        ); 

static void             reg_msg_setting_id(
                            Widget      widget,
                            XtPointer   client_data,
                            XtPointer   call_data
                        );  

static void		ttalk_mode_changedCB(
			    Widget	widget,
			    XtPointer   client_data,
			    XmToggleButtonCallbackStruct *calldata
			);

static void             ttalk_rcv_changedCB(
                            Widget      widget,
                            XtPointer   client_data,
                            XmToggleButtonCallbackStruct *calldata
                        );  

static char *message_strs[] = 
{
     "Do_Command", 	"Get_Environment", "Get_Geometry", "Get_iconified",
     "Get_Locale", 	"Get_Mapped",      "Get_Mofified", "Get_Situation",
     "Get_Status", 	"Get_Sysinfo",     "Get_XInfo",    "Lower",
     "Quit",       	"Raise",           "Revert",       "Save",
     "Set_Environment", "Set_Geometry",    "Set_Iconified","Set_Locale",
     "Set_Mapped", 	"Set_Situation",   "Set_XInfo",    "Signal"
};

static Widget	tt_msg_list;
static Widget   tt_msg_setting;	

void
ttalk_show_dialog(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    extern Widget	AB_toplevel;

    /* If there is no ToolTalk Editor, create it */
    if (AB_ttalk_dialog == (Widget)NULL)
    {
	/* Create the dialog and set the global handle */
	tooltalk_tooltalk_editor_clear(&tooltalk_tooltalk_editor);
	tooltalk_tooltalk_editor_initialize(&tooltalk_tooltalk_editor,
		AB_toplevel);
	AB_ttalk_dialog = tooltalk_tooltalk_editor.tooltalk_editor_pw;
	tt_msg_list =     tooltalk_tooltalk_editor.tt_msg_list_list;
	tt_msg_setting =  tooltalk_tooltalk_editor.tt_rcv_mode;

	/* Register ab dialog control callbacks */
	XtAddCallback(XtParent(AB_ttalk_dialog), XtNpopupCallback,
		ab_dialog_popupCB, (XtPointer)TTEditorUp);
	XtAddCallback(XtParent(AB_ttalk_dialog), XtNpopdownCallback,
		ab_dialog_popdownCB, (XtPointer)TTEditorUp);

        /* These will go away when connections work */
        {
           XtAddCallback(tooltalk_tooltalk_editor.ok_button,
             XmNactivateCallback,(XtCallbackProc)ttalk_applyCB,(XtPointer)NULL);
           XtAddCallback(tooltalk_tooltalk_editor.cancel_button,
             XmNactivateCallback,(XtCallbackProc)ttalk_cancelCB,
	     (XtPointer)NULL); 
        }
	/* This will go away with inactive initial states work */
	{
	   XtSetSensitive(tt_msg_setting,False);
	   XtSetSensitive(tt_msg_list,False);
	}
    }
    XtManageChild(AB_ttalk_dialog);
}

static void
list_add_msgs(
    Widget      w,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    XmString xmitem;
    int i;

    tt_msg_list = w;

    /* Initialize List will Desktop Message Strings */
    for (i = 0; i < XtNumber(message_strs); i++)
    {
	xmitem = XmStringCreateLocalized(message_strs[i]);
	XmListAddItem(tt_msg_list, xmitem, 0);
	XmStringFree(xmitem);
    }

}

static void
ttalk_mode_changedCB(
    Widget	widget,
    XtPointer	client_data,
    XmToggleButtonCallbackStruct *calldata
)
{
    int on;

    util_dprintf(2,"ttalk_mode_changed()\n");
    if (calldata->set)
    {
	XtVaGetValues(widget, XmNuserData, &on, NULL);

	ui_set_active(tt_msg_setting, on? TRUE : FALSE);
	ui_set_active(XtParent(tt_msg_list), on? TRUE : FALSE);
	ui_set_active(tt_msg_list, on? TRUE : FALSE);

    }

}

static void
ttalk_rcv_changedCB(
    Widget      widget,
    XtPointer   client_data,
    XmToggleButtonCallbackStruct *calldata
)
{
    int all;
    int i;

    if (calldata->set)
    {
        XtVaGetValues(widget, XmNuserData, &all, NULL);

	if (all)
	    for(i = 0; i < XtNumber(message_strs); i++)
		XmListSelectPos(tt_msg_list, i+1, FALSE);


    }

}

static void
ttalk_applyCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    util_dprintf(2,"ttalk_applyCB()\n");
    XtUnmanageChild(AB_ttalk_dialog);

}

static void
ttalk_resetCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    util_dprintf(2,"ttalk_resetCB()\n"); 
 
}

static void
ttalk_cancelCB(
    Widget      w,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    util_dprintf(2,"ttalk_cancelCB()\n"); 
    XtUnmanageChild(AB_ttalk_dialog);
}
