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
/* $XConsortium: IconicPathP.h /main/4 1995/11/02 14:40:56 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           IconicPathP.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Private header file for IconicPath.c
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _DtIconicPathP_h
#define _DtIconicPathP_h

#include <Xm/ManagerP.h>
#include "IconicPath.h"

#ifdef __cplusplus
extern "C" {
#endif


/*  New fields for the IconicPath widget class record  */

typedef struct
{
   int mumble;   /* No new procedures */
} DtIconicPathClassPart;


/* Full class record declaration */

typedef struct _DtIconicPathClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XmManagerClassPart	manager_class;
	DtIconicPathClassPart	iconic_path_class;
} DtIconicPathClassRec;

externalref DtIconicPathClassRec dtIconicPathClassRec;


/* New fields for the IconicPath widget record */

typedef struct
{
	Dimension		margin_width;
	Dimension		margin_height;
	Dimension		spacing;
	Dimension		small_min_width;
	Dimension		large_min_width;
	Boolean			force_small_icons;
	Boolean			buttons;
	Boolean			dropzone;
	Boolean			status_msg;
	Boolean			force_large_icons;
	Boolean			icons_changed;

	char * file_mgr_rec;
	char * current_directory;
	Boolean large_icons;
	char *msg_text;
	
	char * directory_shown;
	Boolean large_shown;
	Widget dotdot_button;
	Widget dropzone_icon;
	Widget status_label;
	int num_components;
	struct _IconicPathComponent {
		char *path;
		char *icon_name;
		Widget icon;
		Widget button;
		int width;
	} *components;
	int left_component;
	GC gc;

} DtIconicPathPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _DtIconicPathRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	XmManagerPart		manager;
	DtIconicPathPart	iconic_path;
} DtIconicPathRec;



/********    Private Function Declarations    ********/

extern void _DtIconicPathInput(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtIconicPathP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
