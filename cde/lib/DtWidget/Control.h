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
/* $XConsortium: Control.h /main/4 1995/10/26 09:30:15 rswiston $ */
/**---------------------------------------------------------------------
***	
***	file:		Control.h
***
***	project:	MotifPlus Widgets
***
***	description:	Public include file for DtControl class.
***	
***	
***			(c) Copyright 1992 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtControl_h
#define _DtControl_h

#include <Xm/Xm.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define XmCONTROL_NONE		0
#define XmCONTROL_BLANK		1
#define XmCONTROL_BUSY		2
#define XmCONTROL_BUTTON	3
#define XmCONTROL_CLIENT	4
#define XmCONTROL_DATE		5
#define XmCONTROL_MAIL		6
#define XmCONTROL_MONITOR	7
#define XmCONTROL_SWITCH	8

#define NUM_LIST_ITEMS		10

#define XmCR_BUSY_START		60
#define XmCR_BUSY_STOP		61
#define XmCR_MONITOR		62

#define DtMONITOR_OFF           0
#define DtMONITOR_ON            1

#define XmNuseLabelAdjustment	"use_label_adjustment"

#ifndef DtIsControl
#define DtIsControl(w) XtIsSubclass(w, dtControlGadgetClass)
#endif /* DtIsControl */


extern Widget _DtCreateControl (Widget, String, ArgList, int);
extern void _DtControlSetFileChanged (Widget, Boolean);
extern void _DtControlSetBusy (Widget, Boolean);
extern void _DtControlAddDropAnimationImage (Widget, String, int);
extern void _DtControlDoDropAnimation (Widget);
extern void _DtControlAddPushAnimationImage (Widget, String, int);
extern void _DtControlDoPushAnimation (Widget);
extern char _DtControlGetMonitorState(Widget);


extern WidgetClass dtControlGadgetClass;

typedef struct _DtControlClassRec * DtControlClass;
typedef struct _DtControlRec      * DtControlGadget;

#define XmCR_FILE_CHANGED	51

typedef struct
    {
    int			 reason;
    XEvent		*event;
    Boolean		 set;
    unsigned char	 control_type;
    XtPointer		 push_function;
    XtPointer		 push_argument;
    Widget		 subpanel;
    long		 file_size;
    }	DtControlCallbackStruct;

#if defined(__cplusplus) || defined(c_plusplus)
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _DtControl_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
