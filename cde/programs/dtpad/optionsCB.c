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
/* $XConsortium: optionsCB.c /main/3 1995/11/01 10:39:09 rswiston $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        optionsCB.c
**
**  Project:     DT dtpad, a memo maker type editor based on the Dt Editor
**               widget.
**
**  Description:
**  -----------
**
**	This file contains the callbacks for the [Options] menu items.
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1991, 1992.  All rights are
**  reserved.  Copying or other reproduction of this program
**  except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
**
********************************************************************
**  (c) Copyright 1993, 1994 Hewlett-Packard Company
**  (c) Copyright 1993, 1994 International Business Machines Corp.
**  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
**  (c) Copyright 1993, 1994 Novell, Inc.
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/
#include "dtpad.h"
#include <Dt/HourGlass.h>


/************************************************************************
 * OverstrikeCB - set Editor widget overstike and "Options" menu
 *	"Overstrike" radio button state based on value passed in
 *	call_data->set.
 *
 *	  NOTE:	The default overstrike state for the entire (pPad) edit
 *		session is also reset.
 ************************************************************************/
/* ARGSUSED */
void
OverstrikeCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    Arg al[1];
    Editor *pPad = (Editor *)client_data;
    XmToggleButtonCallbackStruct *cb = (XmToggleButtonCallbackStruct *) 
				       call_data;
				       
    XtSetArg(al[0], DtNoverstrike, (Boolean) cb->set);
    XtSetValues(pPad->editor, al, 1);
    pPad->xrdb.overstrike = (Boolean) cb->set;	/* reset edit session default */

}


/************************************************************************
 * WordWrapCB - set Editor widget word wrap state and the default state
 *	for the new line radio button in the "Save As" File menu dialog
 *	based on value passed in call_data->set.
 *
 *	  NOTE:	The default word wrap state for the entire (pPad) edit
 *		session is also reset.
 ************************************************************************/
/* ARGSUSED */
void
WordWrapCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    Arg al[1];
    Editor *pPad = (Editor *)client_data;
    XmToggleButtonCallbackStruct *cb = (XmToggleButtonCallbackStruct *) 
				       call_data;
    SaveAs *pSaveAs = &pPad->fileStuff.fileWidgets.saveAs;
    Select *pSelect = &pPad->fileStuff.fileWidgets.select;

    XtSetArg(al[0], DtNwordWrap, (Boolean) cb->set);
    XtSetValues(pPad->editor, al, 1);

    pPad->xrdb.wordWrap = (Boolean) cb->set;	/* reset edit session default */

    /* -----> set the default state for the "add new lines?" radio boxes in the
     *        "Save As" file selection box and the "Save" prompt dialog
     *        (these radio boxes are included only if word wrap is on) */
    if (pSaveAs->toggleWidgets.newl_radio != (Widget) 0) {
	if (pPad->xrdb.wordWrap == True) {
	    XtManageChild(pSaveAs->toggleWidgets.newl_radio);
	} else {
	    XtUnmanageChild(pSaveAs->toggleWidgets.newl_radio);
	}
    }
    if (pSelect->toggleWidgets.newl_radio != (Widget) 0) {
	if (pPad->xrdb.wordWrap == True) {
	    XtManageChild(pSelect->separator);
	    XtManageChild(pSelect->toggleWidgets.newl_radio);
	} else {
	    XtUnmanageChild(pSelect->separator);
	    XtUnmanageChild(pSelect->toggleWidgets.newl_radio);
	}
    }

}


/************************************************************************
 * StatusLineCB - set Editor widget statusLine based on value passed in
 *	call_data->set.
 *
 *	  NOTE:	The default statusLine state for the entire (pPad) edit
 *		session is also reset.
 ************************************************************************/
/* ARGSUSED */
void
StatusLineCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    Arg al[1];
    Editor *pPad = (Editor *)client_data;
    XmToggleButtonCallbackStruct *cb = (XmToggleButtonCallbackStruct *) 
				       call_data;
				       
    XtSetArg(al[0], DtNshowStatusLine, (Boolean) cb->set);
    XtSetValues(pPad->editor, al, 1);
    pPad->xrdb.statusLine = (Boolean) cb->set;	/* reset edit session default */
 
    /* Reset the resize increment and minimum window size properties. */
    SetAppShellResizeHints(pPad);
}
