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
/*******************************************************************************
**
**  datefield.c
**
**  $XConsortium: datefield.c /main/3 1995/11/03 10:21:12 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static  char sccsid[] = "@(#)datefield.c 1.7 94/11/07 Copyr 1993 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include "util.h"
#include "datefield.h"
#include "misc.h"

/*
**  Returns a date string the parser can handle
*/
extern char*
get_date_from_widget(Tick t, Widget w, OrderingType order,
		     SeparatorType separator) {
        char		*date = NULL;
	WidgetClass	wc;
        static char	buf[80];
 
	memset(buf, '\0', 80);

	if (!w)
		format_tick(t, order, separator, buf);
	else {
		wc = XtClass(w);
		if (wc == xmTextWidgetClass)
			date = XmTextGetString(w);
		else if (wc == xmTextFieldWidgetClass)
			date = XmTextFieldGetString(w);
		else
			return NULL;

		if (!date || *date == '\0')
			format_tick(t, order, separator, buf);
		else if (!datestr2mdy(date, order, separator, buf)) {
			XtFree(date);
			return NULL;
		}
		if (date)
			XtFree(date);
	}

	return buf;
}

extern void
set_date_in_widget(Tick t, Widget w, OrderingType order,
		   SeparatorType separator) {
        char		buf[80];
	XmString	xmstr;
	WidgetClass	wc = XtClass(w);

        format_tick(t, order, separator, buf);
	if (wc == xmLabelGadgetClass || wc == xmLabelWidgetClass) {
		xmstr = XmStringCreateLocalized(buf);
		XtVaSetValues(w, XmNlabelString, xmstr,
			NULL);
		XmStringFree(xmstr);
	} else if (wc == xmTextWidgetClass)
		XmTextSetString(w, buf);
	else if (wc == xmTextFieldWidgetClass)
		XmTextFieldSetString(w, buf);
}
