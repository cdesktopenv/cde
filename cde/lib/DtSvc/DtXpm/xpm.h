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
