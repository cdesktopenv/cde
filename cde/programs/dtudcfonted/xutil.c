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
/* xutil.c 1.2 - Fujitsu source for CDEnext    96/02/29 17:30:54      */
/* $XConsortium: xutil.c /main/2 1996/04/08 16:03:37 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <nl_types.h>

#include <Xm/Xm.h>

#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/MessageB.h>

#include "selectxlfd.h"
#include "xoakufont.h"

/*
 * parameters
 */
void	xlfdPopupDialog(Widget w);

extern	int			getmask ;

extern Widget	toplevel ;
extern Widget	slctBLabel[BUTTONITEMS],
		slctBText[BUTTONITEMS],	slctButton[BUTTONITEMS],
		listPop[BUTTONITEMS],	listW[BUTTONITEMS], errorMD, msgBox ;

/****************************************************************
 * common functions
****************************************************************/


/*
 * pop up and down dialog box
 */

void
xlfdPopupDialog(Widget w)
{
    if (! XtIsManaged(w))
        XtManageChild(w);
    else
        XRaiseWindow(XtDisplayOfObject(w), XtWindow(XtParent(w)));
}
