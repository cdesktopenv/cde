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
/* $XConsortium: View.h /main/3 1995/10/26 09:35:11 rswiston $ */
/**---------------------------------------------------------------------
***	
***	file:		View.h
***
***	project:	MotifPlus Widgets
***
***	description:	Public include file for DtView class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtView_h
#define _DtView_h

#include <Xm/Xm.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*  Temporary location for resource definition.  Need to  */
/*  move into the string defs file.                       */

#define XmNsubpanelUnpostOnSelect "subpanelUnpostOnSelect"
#define XmCSubpanelUnpostOnSelect "SubpanelUnpostOnSelect"
#define XmNsubpanelTorn "subpanelTorn"
#define XmCSubpanelTorn "SubpanelTorn"

/* These macros are also defined in DtStrDefs.h */
/* Control->View.h */
/* Control.c->Control.h->Icon.h->DtStrDefs.h */

#ifndef XmNleftInset
#define XmNleftInset            "leftInset"
#endif /* XmNleftInset */

#ifndef XmNrightInset
#define XmNrightInset           "rightInset"
#endif /* XmNrightInset */

#ifndef XmNtopInset
#define XmNtopInset             "topInset"
#endif /* XmNtopInset */

#ifndef XmNbottomInset
#define XmNbottomInset          "bottomInset"
#endif /* XmNbottomInset */

#ifndef XmNwidthIncrement
#define XmNwidthIncrement       "widthIncrement"
#endif /* XmNwidthIncrement */

#ifndef XmNheightIncrement
#define XmNheightIncrement      "heightIncrement"
#endif /* XmNheightIncrement */

#ifndef XmNboxType
#define XmNboxType              "boxType"
#endif /* XmNboxType */

#ifndef XmCBoxType
#define XmCBoxType              "BoxType"
#endif /* XmCBoxType */

#ifndef XmRBoxType
#define XmRBoxType              "BoxType"
#endif /* XmRBoxType */

#define XmBOX_NONE		0
#define XmBOX_PRIMARY		1
#define XmBOX_SECONDARY		2
#define XmBOX_SUBPANEL		3
#define XmBOX_SWITCH		4

#ifndef DtIsView
#define DtIsView(w) XtIsSubclass(w, dtViewWidgetClass)
#endif /* XmIsView */


extern Widget _DtCreateView(); 

extern WidgetClass dtViewWidgetClass;

typedef struct _DtViewClassRec * DtViewWidgetClass;
typedef struct _DtViewRec      * DtViewWidget;


typedef struct
{
	int		reason;
	XEvent *	event;
	int		category_index;
	XmString	category;
	int		item_index;
	XmString	item;
} DtViewCallbackStruct;

#define XmCR_SELECT	XmCR_SINGLE_SELECT

#if defined(__cplusplus) || defined(c_plusplus)
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtView_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
