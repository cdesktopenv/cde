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
/* $XConsortium: TitleBox.h /main/4 1995/10/26 09:34:50 rswiston $ */
/**---------------------------------------------------------------------
***	
***	file:		TitleBox.h
***
***	project:	MotifPlus Widgets
***
***	description:	Public include file for DtTitleBox class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtTitleBox_h
#define _DtTitleBox_h

#include <Xm/Xm.h>
#include <Dt/DtStrDefs.h>

#ifndef DtIsTitleBox
#define DtIsTitleBox(w) XtIsSubclass(w, DtTitleBoxClass)
#endif /* DtIsTitleBox */


/********    Public Function Declarations    ********/

extern Widget _DtCreateTitleBox( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;
extern Widget _DtTitleBoxGetTitleArea( 
                        Widget w) ;
extern Widget _DtTitleBoxGetWorkArea( 
                        Widget w) ;


extern WidgetClass	dtTitleBoxWidgetClass;

typedef struct _DtTitleBoxClassRec * DtTitleBoxWidgetClass;
typedef struct _DtTitleBoxRec      * DtTitleBoxWidget;


#define XmTITLE_TOP	0
#define XmTITLE_BOTTOM	1

#define XmTITLE_AREA	1


#endif /* _DtTitleBox_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
