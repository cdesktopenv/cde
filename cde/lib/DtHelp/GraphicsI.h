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
/* $XConsortium: GraphicsI.h /main/6 1996/03/27 16:27:58 lehors $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        GraphicsI.h
 **
 **   Project:     Cde Help System
 **
 **   Description: Internal header file for Graphics.c
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _DtGraphicsI_h
#define _DtGraphicsI_h

/*
 * Semi-Public Routines
 */
extern	Pixmap	 _DtHelpProcessGraphic(
			Display         *dpy,
			Drawable         drawable,
			int		 screen,
			int		 depth,
			GC               gc,
			Pixmap          *def_pix,
			Dimension       *def_pix_width,
			Dimension       *def_pix_height,
			_DtGrContext    *context,
			Colormap	 colormap,
			Visual		*visual,
			Pixel		 fore_ground,
			Pixel		 back_ground,
			char            *filename,
			unsigned short media_resolution,
			Dimension       *pixmapWidth,
			Dimension       *pixmapHeight,
			Pixmap		*ret_mask,
			Pixel          **ret_colors,
			int             *ret_number );

#endif /* _DtGraphisI.h */
