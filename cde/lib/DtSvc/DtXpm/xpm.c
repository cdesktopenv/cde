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
    return XmeXpmCreatePixmapFromData(display, d, data,
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
    return XmeXpmReadFileToPixmap(display, d, filename,
				  pixmap_return, shapemask_return,
				  attributes);
}

int _DtXpmWriteFileFromPixmap(Display *display,
			      char *filename,
			      Pixmap pixmap,
			      Pixmap shapemask,
			      XpmAttributes *attributes)
{
    return XmeXpmWriteFileFromPixmap(display, filename,
				     pixmap, shapemask,
				     attributes);
}

int _DtXpmReadFileToImage(Display *display,
			  char *filename,
			  XImage **image_return,
			  XImage **shapeimage_return,
			  XpmAttributes *attributes)
{
    return XmeXpmReadFileToImage(display, filename,
				 image_return, shapeimage_return,
				 attributes);
}

int _DtXpmWriteFileFromImage(Display *display,
			     char *filename,
			     XImage *image,
			     XImage *shapeimage,
			     XpmAttributes *attributes)
{
    return XmeXpmWriteFileFromImage(display, filename,
				    image, shapeimage,
				    attributes);
}

void _DtXpmFreeAttributes(XpmAttributes *attributes)
{
    XmeXpmFreeAttributes(attributes);
}
