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
/* $XConsortium: MenuButton.h /main/3 1995/10/26 09:33:10 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _Dt_MenuButton_h
#define _Dt_MenuButton_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Constants
 */

/* Resources */

#ifndef DtNcascadingCallback
#define DtNcascadingCallback	XmNcascadingCallback
#endif
#ifndef DtNsubMenuId
#define DtNsubMenuId		XmNsubMenuId
#endif
#ifndef DtNcascadePixmap
#define DtNcascadePixmap	XmNcascadePixmap
#endif

#ifndef DtCCallback
#define DtCCallback		XmCCallback
#endif
#ifndef DtCPixmap
#define DtCPixmap		XmCPixmap
#endif
#ifndef DtCMenuWidget
#define DtCMenuWidget		XmCMenuWidget
#endif

#ifndef DtCR_CASCADING
#define DtCR_CASCADING		XmCR_CASCADING
#endif


/*
 * Types
 */

/* Widget class and instance */

typedef struct _DtMenuButtonRec      * DtMenuButtonWidget;
typedef struct _DtMenuButtonClassRec * DtMenuButtonWidgetClass;


/*
 * Data
 */

/* Widget class record */

externalref WidgetClass dtMenuButtonWidgetClass;


/*
 * Functions
 */

extern Widget DtCreateMenuButton( 
		Widget		parent,
		char		*name,
		ArgList		arglist,
		Cardinal	argcount);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_MenuButton_h */
