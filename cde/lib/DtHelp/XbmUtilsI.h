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
/* $XConsortium: XbmUtilsI.h /main/2 1996/05/09 03:45:01 drk $ */
#ifndef _DtXbmUtilsI_h
#define _DtXbmUtilsI_h

/* Include files */

#include <X11/Xlib.h>
#include "GraphicsP.h"

/* Function prototypes */

/* Reads an XBM stream into a data buffer */
int _DtGrReadBitmapStreamData (
    _DtGrStream *fstream,
    unsigned int *width,                /* RETURNED */
    unsigned int *height,               /* RETURNED */
    unsigned char **data,               /* RETURNED */
    int *x_hot,                         /* RETURNED */
    int *y_hot                          /* RETURNED */
);

/* Converts an XBM stream into an X pixmap */ 
int _DtGrReadBitmapStream (
    Display *display,
    Drawable d,
    _DtGrStream *stream,
    unsigned int *width,                /* RETURNED */
    unsigned int *height,               /* RETURNED */
    Pixmap *pixmap,                     /* RETURNED */
    int *x_hot,                         /* RETURNED */
    int *y_hot                          /* RETURNED */
);

#endif /* _DtXbmUtilsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
