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
/* $XConsortium: WPixmap.h /main/3 1996/06/11 16:46:20 cde-hal $ */
#ifndef _WPixmap_h
#define _WPixmap_h

#include <WWL/WCore.h>
#include "Pixmap.h"

class WPixmap : public WCore {
public :
	DEFINE_INIT (WPixmap, WCore, pixmapWidgetClass)
	DEFINE_GETTER_SETTER(Picture, Pixmap, XtNpixmap)
};

#define	NULLWVWrapper		WVWrapper((Widget)0)

#endif /* _WVWrapper_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
