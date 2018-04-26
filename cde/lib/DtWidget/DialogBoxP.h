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
/* $XConsortium: DialogBoxP.h /main/5 1996/03/27 20:16:24 drk $ */
/**---------------------------------------------------------------------
***	
***	file:		DialogBoxP.h
***
***	project:	Motif Widgets
***
***	description:	Private include file for DtDialogBox class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtDialogBoxP_h
#define _DtDialogBoxP_h

#include <Xm/XmP.h>
#include <Xm/BulletinBP.h>
#include <Xm/FormP.h>
#include <Dt/DialogBox.h>

typedef void (*DlgGetSizeProc)(
	Widget,
	Dimension,
	Dimension,
	Dimension,
	Dimension,
	Dimension *,
	Dimension *
);
/*-------------------------------------------------------------
**	Class Structure
*/

/*	Class Part
*/
typedef struct _DtDialogBoxClassPart
{
	XtWidgetProc		create_children;
	XtWidgetProc		configure_children;
	DlgGetSizeProc		get_size;
	XtCallbackProc		button_callback;
	caddr_t			extension;
} DtDialogBoxClassPart;

/*	Full Class Record
*/
typedef struct _DtDialogBoxClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
	XmBulletinBoardClassPart	bulletin_board_class;
	XmFormClassPart		form_class;
	DtDialogBoxClassPart	dialog_box_class;
} DtDialogBoxClassRec;

/*	Actual Class
*/
extern DtDialogBoxClassRec dtDialogBoxClassRec;


/*-------------------------------------------------------------
**	Instance Structure
*/

/*	Instance Part
*/
typedef struct _DtDialogBoxPart
{
	XtCallbackList	callback;
	Widget		work_area;
	Widget		separator;
	WidgetList	button;
	Cardinal	button_count;	
	Boolean		minimize_buttons;
	XmStringTable	button_label_strings;
} DtDialogBoxPart;

/*	Full Instance Record
*/
typedef struct _DtDialogBoxRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	XmBulletinBoardPart	bulletin_board;
	XmFormPart		form;
	DtDialogBoxPart		dialog_box;
} DtDialogBoxRec;


/*-------------------------------------------------------------
**	Constraint Structure
*/

/*	Constraint Part
*/
typedef struct _DtDialogBoxConstraintPart
{
	unsigned char		child_type;
} DtDialogBoxConstraintPart, * DtDialogBoxConstraint;

/*	Full Constraint Record
*/
typedef struct _DtDialogBoxConstraintRec
{
	XmManagerConstraintPart		manager_constraint;
	XmFormConstraintPart		form_constraint;
 	DtDialogBoxConstraintPart	dialog_box_constraint;
} DtDialogBoxConstraintRec, * DtDialogBoxConstraintPtr;



/*-------------------------------------------------------------
**	Class and Instance Macros
*/

/*	DtDialogBox Class Macros
*/	
/**** WARNING: These macros are not thread-safe! ****/
#define C_CreateChildren(mc)	((mc) -> dialog_box_class.create_children)
#define C_ConfigureChildren(mc)	((mc) -> dialog_box_class.configure_children)
#define C_GetSize(mc)		((mc) -> dialog_box_class.get_size)
#define C_ButtonCallback(mc)	((mc) -> dialog_box_class.button_callback)

/*	DtDialogBox Instance Macros
*/
#define M_DialogBoxConstraint(w) \
 	(&((DtDialogBoxConstraintPtr) (w)->core.constraints) \
		-> dialog_box_constraint)
#define M_WorkArea(m)		(m -> dialog_box.work_area)
#define M_Separator(m)		(m -> dialog_box.separator)
#define M_MinimizeButtons(m)	(m -> dialog_box.minimize_buttons)
#define M_Button(m)		(m -> dialog_box.button)
#define M_ButtonCount(m)	(m -> dialog_box.button_count)
#define M_ButtonLabelStrings(m)	(m -> dialog_box.button_label_strings)
#define M_Callback(m)		(m -> dialog_box.callback)

#endif /* _DtDialogBoxP_h */

/* DON'T ADD ANYTHING AFTER THIS #endif */
