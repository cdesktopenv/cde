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
/* $XConsortium: xpm.c /main/1 1996/02/19 16:41:55 lehors $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*****************************************************************************\
*                                                                             *
* This is only a wrapper around the Xpm functions which now come as part of   *
* Motif. This is provided for backward compatibility only and therefore only  *
* supports symbols declared as private in the elist and none of the internal  *
* ones.                                                                       *
*                                                                             *
\*****************************************************************************/

#include <Xm/XpmP.h>

int _DtXpmCreatePixmapFromData(Display *display,
			       Drawable d,
			       char **data,
			       Pixmap *pixmap_return,
			       Pixmap *shapemask_return,
			       XpmAttributes *attributes)
{
    return XpmCreatePixmapFromData(display, d, data,
				      pixmap_return, shapemask_return,
				      attributes);
}

int _DtXpmReadFileToPixmap(Display *display,
			   Drawable d,
			   char *filename,
			   Pixmap *pixmap_return,
			   Pixmap *shapemask_return,
			   XpmAttributes *attributes)
{
    return XpmReadFileToPixmap(display, d, filename,
				  pixmap_return, shapemask_return,
				  attributes);
}

int _DtXpmWriteFileFromPixmap(Display *display,
			      char *filename,
			      Pixmap pixmap,
			      Pixmap shapemask,
			      XpmAttributes *attributes)
{
    return XpmWriteFileFromPixmap(display, filename,
				     pixmap, shapemask,
				     attributes);
}

int _DtXpmReadFileToImage(Display *display,
			  char *filename,
			  XImage **image_return,
			  XImage **shapeimage_return,
			  XpmAttributes *attributes)
{
    return XpmReadFileToImage(display, filename,
				 image_return, shapeimage_return,
				 attributes);
}

int _DtXpmWriteFileFromImage(Display *display,
			     char *filename,
			     XImage *image,
			     XImage *shapeimage,
			     XpmAttributes *attributes)
{
    return XpmWriteFileFromImage(display, filename,
				    image, shapeimage,
				    attributes);
}

void _DtXpmFreeAttributes(XpmAttributes *attributes)
{
    XpmFreeAttributes(attributes);
}
