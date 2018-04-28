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
/* $XConsortium: MenuButtonP.h /main/3 1995/10/26 09:33:17 rswiston $ */
/*
 *        Copyright (C) 1986,1991  Sun Microsystems, Inc
 *                    All rights reserved.
 *          Notice of copyright on this source code
 *          product does not indicate publication.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by
 * the U.S. Government is subject to restrictions as set forth
 * in subparagraph (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software Clause at DFARS 252.227-7013 (Oct. 1988)
 * and FAR 52.227-19 (c) (June 1987).
 *
 *    Sun Microsystems, Inc., 2550 Garcia Avenue,
 *    Mountain View, California 94043.
 *
 */

#ifndef  _DtMenuButtonP_h
#define  _DtMenuButtonP_h

#include "MenuButton.h"
#include <Xm/LabelP.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
 *
 *	Message	Defines
 *
 ****************************************************************/
 
#define MB_POST		DTWIDGET_GETMESSAGE( \
                        MS_MenuButton, MENU_POST, _DtMsgMenuButton_0000)
 
#define MB_PARENT	DTWIDGET_GETMESSAGE( \
                        MS_MenuButton, MENU_PARENT, _DtMsgMenuButton_0001)
 
#define MB_SUBMENU	DTWIDGET_GETMESSAGE( \
                        MS_MenuButton, MENU_SUBMENU, _DtMsgMenuButton_0002)
 

/* The MenuButton instance record */

typedef	struct 
{	/* resources */
    XtCallbackList	cascading_callback;	/* Cascading callback  */
    Widget		submenu;	/* the menu to pull down */
    Pixmap		menu_pixmap;	/* pixmap for the menu */

	/* internal fields */

    Boolean	armed;		/* armed flag */
    Boolean     popped_up;	/* submenu popped up flag*/
    XRectangle  menu_rect;	/* location of menu*/
    Time	last_timestamp;	/* last time submenu popped down */
    Boolean	private_submenu;
    GC		gc;
} DtMenuButtonPart;


/* Full instance record declaration */

typedef struct _DtMenuButtonRec
{
    CorePart		core;
	XmPrimitivePart	primitive;
	XmLabelPart		label;
	DtMenuButtonPart	menu_button;
} DtMenuButtonRec;


/* MenuButton class structure */

typedef struct {
	XtPointer	extension;	/* Pointer to extension record */
} DtMenuButtonClassPart;


/* Full class record declaration for MenuButton class */

typedef struct _DtMenuButtonClassRec {
	CoreClassPart	    core_class;
	XmPrimitiveClassPart	primitive_class;
	XmLabelClassPart		label_class;
	DtMenuButtonClassPart menu_button_class;
} DtMenuButtonClassRec;


extern DtMenuButtonClassRec   dtMenuButtonClassRec;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif  /* _DtMenuButtonP_h */
/* DON'T ADD STUFF AFTER THIS #endif */

