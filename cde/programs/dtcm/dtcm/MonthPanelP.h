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
**  MonthPanelP.h
**
**  $XConsortium: MonthPanelP.h /main/4 1995/11/03 10:16:54 rswiston $
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
 * Month Panel widget private header file
 */
#ifndef _XmMonthPanelP_h
#define _XmMonthPanelP_h

#include "MonthPanel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* New fields for the MonthPanel widget class record */
typedef struct {
     int mumble;   /* No new procedures */
} XmMonthPanelClassPart;

/******************************
 *
 * Class record declaration
 *
 ******************************/
typedef struct _XmMonthPanelClassRec {
    CoreClassPart		core_class;
    CompositeClassPart  	composite_class;
    ConstraintClassPart 	constraint_class;
    XmManagerClassPart  	manager_class;
    XmMonthPanelClassPart	month_panel_class;
} XmMonthPanelClassRec;

externalref XmMonthPanelClassRec xmMonthPanelClassRec;

/************************************
 *
 * Month Panel instance structure.
 *
 ************************************/
typedef struct {
/* resource fields */
   int year;
   int month;
   XtCallbackList callback;
   String title_format;
   Boolean active_header;
   Boolean active_days;
   Boolean show_panel_header;
   Boolean show_column_headers;
   Boolean show_separator;
/* private fields */
   Widget header;
   Widget separator;
   Widget *day_labels;   
   Widget *days;   
   int    display_rows;
} XmMonthPanelPart;


/************************************************************************
 *									*
 * Full instance record declaration					*
 *									*
 ************************************************************************/

typedef struct _XmMonthPanelRec {
    CorePart	    core;
    CompositePart   composite;
    ConstraintPart constraint;
    XmManagerPart   manager;
    XmMonthPanelPart   month_panel;
} XmMonthPanelRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMonthPanelP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
