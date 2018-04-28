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
/* $XConsortium: TitleBoxP.h /main/3 1995/10/26 09:34:57 rswiston $ */
/**---------------------------------------------------------------------
***	
***	file:		TitleBoxP.h
***
***	project:	MotifPlus Widgets
***
***	description:	Private include file for DtTitleBox class.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


#ifndef _DtTitleBoxP_h
#define _DtTitleBoxP_h

#include <Xm/XmP.h>
#include <Dt/TitleBox.h>



/*-------------------------------------------------------------
**	Class Structure
*/

/*	Class Part
*/
typedef struct _DtTitleBoxClassPart
{
	caddr_t extension;
} DtTitleBoxClassPart;

/*	Full Class Record
*/
typedef struct _DtTitleBoxClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
	DtTitleBoxClassPart	title_box_class;
} DtTitleBoxClassRec;

/*	Actual Class
*/
extern DtTitleBoxClassRec dtTitleBoxClassRec;




/*-------------------------------------------------------------
**	Instance Structure
*/

/*	Instance Part
*/
typedef struct _DtTitleBoxPart
{
	Widget		work_area;
	Widget		title_area;
	XmString	title_string;
	XmFontList	font_list;
	Dimension	margin_width;
	Dimension	margin_height;
	Dimension	old_width;
	Dimension	old_height;
	Dimension	old_shadow_thickness;
	Dimension	title_spacing;
	unsigned char	title_position;
	unsigned char	title_alignment;
	unsigned char	shadow_type;
} DtTitleBoxPart;

/*	Full Instance Record
*/
typedef struct _DtTitleBoxRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	DtTitleBoxPart		title_box;
} DtTitleBoxRec;



/*-------------------------------------------------------------
**	Constraint Structure
*/

/*	Constraint Part
*/
typedef struct _DtTitleBoxConstraintPart
{
	unsigned char	child_type;
} DtTitleBoxConstraintPart, * DtTitleBoxConstraint;

/*	Full Constraint Record
*/
typedef struct _DtTitleBoxConstraintRec
{
	XmManagerConstraintPart		manager_constraint;
 	DtTitleBoxConstraintPart	title_box_constraint;
} DtTitleBoxConstraintRec, * DtTitleBoxConstraintPtr;


/*-------------------------------------------------------------
**	Class and Instance Macros
*/

/*	DtTitleBox Class Macros
*/

/*	DtTitleBox Instance Macros
*/
#define M_TitleBoxConstraint(w) \
	(&((DtTitleBoxConstraintPtr) (w)->core.constraints)->title_box_constraint)
#define M_WorkArea(m)		(m -> title_box.work_area)
#define M_TitleArea(m)		(m -> title_box.title_area)
#define M_TitleString(m)	(m -> title_box.title_string)
#define M_TitleSpacing(m)	(m -> title_box.title_spacing)
#define M_TitlePosition(m)	(m -> title_box.title_position)
#define M_TitleAlignment(m)	(m -> title_box.title_alignment)
#define M_FontList(m)		(m -> title_box.font_list)
#define M_ShadowType(m)		(m -> title_box.shadow_type)
#define M_MarginWidth(m)	(m -> title_box.margin_width)
#define M_MarginHeight(m)	(m -> title_box.margin_height)
#define M_OldWidth(m)		(m -> title_box.old_width)
#define M_OldHeight(m)		(m -> title_box.old_height)
#define M_OldShadowThickness(m)	(m -> title_box.old_shadow_thickness)


#endif /* _DtTitleBoxP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
