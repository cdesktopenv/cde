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
/* $XConsortium: icon.h /main/3 1995/10/27 10:39:18 rswiston $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         icon.h
 **
 **   Description:  Header for icon handling portion of CDE Drag & Drop Demo.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <sys/param.h>
#include <stdio.h>

/*
 * Icon Information Structure
 */

typedef struct _IconInfo {
	XRectangle	icon;
	char		*name;
	void		*data;
	int		dataCount;
	Pixmap		bitmap;
	Pixmap		mask;
	Widget		dragIcon;
	int		type;
	struct _IconInfo *next;
	struct _IconInfo *prev;
} IconInfo;

/*
 * Icon Typedefs
 */

typedef enum {
	IconBitmap,
	IconMask
} IconBitmapPart;

typedef enum {
	IconByFile,
	IconByData,
	IconByType
} IconTyping;

/*
 * Public Icon Handling Function Declarations
 */

void 		IconCreateDouble(Widget, IconInfo*, int, int, IconInfo*,
			short, short);
void		IconDelete(Widget, IconInfo*);
void		IconDraw(Widget, IconInfo*);
void		IconInitialize(Widget, IconInfo*, short, short, void*, int, 
			char*, IconTyping);
IconInfo	*IconNew();

