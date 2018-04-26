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
/*********************************************************************************
**  MonthPanel.h
**
**  $XConsortium: MonthPanel.h /main/4 1995/11/03 10:16:43 rswiston $
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

/*
 * Month Panel widget public header file
 */
#ifndef _XmMonthPanel_h
#define _XmMonthPanel_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XmIsMonthPanel
#define XmIsMonthPanel(w)	XtIsSubclass(w, xmMonthPanelWidgetClass)
#endif /* XmIsMonthPanel */

externalref WidgetClass xmMonthPanelWidgetClass;

typedef struct _XmMonthPanelClassRec * XmMonthPanelWidgetClass;
typedef struct _XmMonthPanelRec      * XmMonthPanelWidget;

typedef enum { MONTH_SELECTION, DAY_SELECTION } XmMonthPanelSelectionType;

typedef struct {
   XmMonthPanelSelectionType	type;
   int day;
} XmMonthPanelCallbackStruct;

/********    Public Function Declarations    ********/

extern Widget XmCreateMonthPanel( 
                        Widget parent,
                        char *name,
                        ArgList args,
                        Cardinal argCount) ;

/********    End Public Function Declarations    ********/

/********    Resource Strings    ********/
#define	XmNmonth		"month"
#define	XmCMonth		"Month"
#define	XmNyear			"year"
#define	XmCYear			"Year"
#define XmNtitleFormat		"titleFormat"
#define XmCTitleFormat		"TitleFormat"
#define XmNactiveDays		"activeDays"
#define XmCActiveDays		"ActiveDays"
#define XmNactiveHeader		"activeHeader"
#define XmCActiveHeader		"ActiveHeader"
#define XmNshowPanelHeader	"showPanelHeader"
#define XmCShowPanelHeader	"ShowPanelHeader"
#define XmNshowColumnHeaders	"showColumnHeaders"
#define XmCShowColumnHeaders	"ShowColumnHeaders"
/*
#define XmNshowSeparator	"showSeparator"
#define XmCShowSeparator	"ShowSeparator"
*/
/********    End Resource Strings    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMonthPanel_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
