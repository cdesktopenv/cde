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
/* $XConsortium: HelpQuickD.h /main/3 1995/10/26 12:25:25 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_HelpQuickD_h
#define _Dt_HelpQuickD_h

#include <Dt/Help.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* Quick help dialog-specific Resources */

#ifndef DtNbackLabelString
#define DtNbackLabelString		"backLabelString"
#endif
#ifndef DtNhelpLabelString
#define DtNhelpLabelString		XmNhelpLabelString
#endif
#ifndef DtNmoreLabelString
#define DtNmoreLabelString		"moreLabelString"
#endif
#ifndef DtNcloseLabelString
#define DtNcloseLabelString		"closeLabelString"
#endif
#ifndef DtNprintLabelString
#define DtNprintLabelString		"printLabelString"
#endif

#ifndef DtCCallback
#define DtCCallback                     XmCCallback
#endif

#ifndef DtCBackLabelString
#define DtCBackLabelString		"BackLabelString"
#endif
#ifndef DtCHelpLabelString
#define DtCHelpLabelString		XmCHelpLabelString
#endif
#ifndef DtCMoreLabelString
#define DtCMoreLabelString		"MoreLabelString"
#endif
#ifndef DtCCloseLabelString
#define DtCCloseLabelString		"CloseLabelString"
#endif
#ifndef DtCPrintLabelString
#define DtCPrintLabelString		"PrintLabelString"
#endif


/*
 * Types
 */

/* Widget class and instance */

typedef struct _DtHelpQuickDialogWidgetClassRec * DtHelpQuickDialogWidgetClass;
typedef struct _DtHelpQuickDialogWidgetRec   * DtHelpQuickDialogWidget;


/*
 * Data
 */

/* Widget class record */

externalref WidgetClass dtHelpQuickDialogWidgetClass;


/*
 * Functions
 */

/* tmp backwards compat */
#define DtCreateQuickHelpDialog  DtCreateHelpQuickDialog

extern Widget DtCreateHelpQuickDialog(
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

extern Widget DtHelpQuickDialogGetChild(
		Widget		widget,
		unsigned char	child);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_HelpQuickD_h */
