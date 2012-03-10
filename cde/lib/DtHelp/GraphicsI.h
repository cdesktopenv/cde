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
