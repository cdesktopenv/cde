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
/* $XConsortium: xpm.h /main/4 1996/02/19 16:42:00 lehors $ */
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

#ifndef xpm_h
#define xpm_h

#include <Xm/XpmP.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int _DtXpmCreatePixmapFromData(Display *display,
				      Drawable d,
				      char **data,
				      Pixmap *pixmap_return,
				      Pixmap *shapemask_return,
				      XpmAttributes *attributes);

extern int _DtXpmReadFileToPixmap(Display *display,
				  Drawable d,
				  char *filename,
				  Pixmap *pixmap_return,
				  Pixmap *shapemask_return,
				  XpmAttributes *attributes);

extern int _DtXpmWriteFileFromPixmap(Display *display,
				     char *filename,
				     Pixmap pixmap,
				     Pixmap shapemask,
				     XpmAttributes *attributes);

extern int _DtXpmReadFileToImage(Display *display,
				 char *filename,
				 XImage **image_return,
				 XImage **shapeimage_return,
				 XpmAttributes *attributes);

extern int _DtXpmWriteFileFromImage(Display *display,
				    char *filename,
				    XImage *image,
				    XImage *shapeimage,
				    XpmAttributes *attributes);

extern void _DtXpmFreeAttributes(XpmAttributes *attributes);

#ifdef __cplusplus
}					/* for C++ V2.0 */
#endif

#endif
