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
/* $XConsortium: HelpQuickDI.h /main/6 1996/04/05 14:41:11 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   HelpQuickDI.h
 **
 **  Project:  Cache Creek (Rivers) Project:
 **
 **  Description:  Internal header file for QuickHelpDialog Widget.
 **  -----------
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _HelpQuickDI_h
#define _HelpQuickDI_h

#include "DisplayAreaI.h"

/* General Define for Quick Help Dialog Widget */

#define DtHELP_TOPIC            1
#define DtHELP_STRING           2
#define DtMAN_TOPIC             3
#define DtFILE_TOPIC            4


/* Access File Expantion Definitions */

#define DtACCESS_PATH_TYPE             "help"
#define DtACCESS_PATH_SUFFIX           ".rp"


#define DtNULL_TOPIC_STRING            "Null Topic"



/*****************************************************************************
 * Function:	    void _DtHelpQuickDialogHypertextCB(
 *                              DtHelpDispAreaStruct *pDisplayAreaStruct,
 *                              XtPointer clientData,
 *                      	DtHyperTextCallbackStruct *hyperData)
 *
 * Parameters:      pDisplayAreaStruct  Specifies the curretn display are info.
 *
 *                  hyperData           Specifies the current hypertext info
 *                                      structure.
 *
 *                  clientData          Specifies the client data passed into
 *                                      the hypertext callback. 
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process all hypertext requests in a given Help Dialogs
 *                  display area.
 *
 *****************************************************************************/
extern void _DtHelpQuickDialogHypertextCB (
    XtPointer pDisplayAreaStruct,
    XtPointer clientData,
    DtHelpHyperTextStruct *hyperData);


#endif /* _HelpQuickDI_h */
/* Do not add anything after this endif. */
