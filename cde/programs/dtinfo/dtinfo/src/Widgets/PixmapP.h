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
/* $XConsortium: PixmapP.h /main/3 1996/06/11 16:45:43 cde-hal $ */
#ifndef _PixmapP_h
#define _PixmapP_h

#include <X11/CoreP.h>

#include "Pixmap.h"

typedef struct {
  int		ignore ;
} PixmapClassPart;

typedef struct _PixmapClassRec {
    CoreClassPart	core_class;
    PixmapClassPart	pixmap_class;
} PixmapClassRec;

extern PixmapClassRec pixmapClassRec;

typedef struct {
  Pixmap	pixmap ;
  /* private */
  GC		gc ;
  Dimension	width ;
  Dimension	height ;
} PixmapPart;

typedef struct _PixmapRec {
    CorePart	core;
    PixmapPart	pixmap;
} PixmapRec;

#endif 
