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
/* $XConsortium: JpegUtilsI.h /main/3 1996/10/06 19:37:31 rws $ */
#ifndef _DtJpegUtilsI_h
#define _DtJpegUtilsI_h

/* Include files */

#include <X11/Xlib.h>
#include "GraphicsP.h"

/* Function prototypes */

/* Creates a pixmap from a jpeg stream */
enum _DtGrLoadStatus jpeg_to_ximage (
    _DtGrStream           *stream,
    Screen                *screen,
    Visual                *visual,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    XImage                **ximage,
    XColor                **xcolors,
    int                    *ncolors,
    int                    *xres
);

#endif /* _DtJpegUtilsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
