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
/* $XConsortium: DialogBox.h /main/4 1995/10/26 09:30:36 rswiston $ */
/**---------------------------------------------------------------------
***	
***	file:		DialogBox.h
***
***	project:	MotifPlus Widgets
***
***	description:	Public include file for DtDialogBox class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtDialogBox_h
#define _DtDialogBox_h

#include <Xm/Xm.h>
#include <Dt/DtStrDefs.h>

#ifndef DtIsDialogBox
#define DtIsDialogBox(w) XtIsSubclass(w, dtDialogBoxWidgetClass)
#endif /* XmIsDialogBox */


extern Widget _DtCreateDialogBox( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern Widget __DtCreateDialogBoxDialog( 
                        Widget ds_p,
                        String name,
                        ArgList db_args,
                        Cardinal db_n) ;
extern Widget _DtDialogBoxGetButton( 
                        Widget w,
                        Cardinal pos) ;
extern Widget _DtDialogBoxGetWorkArea( 
                        Widget w) ;


extern WidgetClass dtDialogBoxWidgetClass;

typedef struct _DtDialogBoxClassRec * DtDialogBoxWidgetClass;
typedef struct _DtDialogBoxRec      * DtDialogBoxWidget;


#define XmBUTTON	11


#define XmCR_DIALOG_BUTTON	100

typedef struct
{
	int		reason;
	XEvent *	event;
	Cardinal	button_position;
	Widget		button;
} DtDialogBoxCallbackStruct;


#endif /* _DtDialogBox_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
