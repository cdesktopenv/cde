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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: XmWrap.c /main/10 1998/09/21 10:57:11 mgreess $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#include <Xm/XmP.h>
#include <Xm/IconFile.h>
#include <Xm/XmPrivate.h>

#ifdef DtUse_XmFunctions

/* Prototypes for internal functions we steal from <Xm/ImageCachI.h>. */
extern Boolean _XmInstallImage( 
                        XImage *image,
                        char *image_name,
                        int hot_x,
                        int hot_y) ;
extern Boolean _XmGetImage( 
                        Screen *screen,
                        char *image_name,
                        XImage **image) ;
extern Boolean _XmCachePixmap( 
                              Pixmap pixmap,
                              Screen *screen,
                              char *image_name,
                              Pixel foreground,
                              Pixel background,
                              int depth,
                              Dimension width,
                              Dimension height) ;
#endif

/************************************************************************
 *
 *  _DtGetPixmapByDepth
 *      Public wrapper around __DtGetPixmap with parameter order changed.
 *
 ************************************************************************/
Pixmap
_DtGetPixmapByDepth(
                     Screen *screen,
                     char *image_name,
                     Pixel foreground,
                     Pixel background,
                     int depth)
{
    return(XmGetPixmapByDepth(screen, image_name,
			      foreground, background, depth));
}

/************************************************************************
*
*  _DtGetPixmap
*       Create a pixmap of screen depth, using the image referenced
*       by the name and the foreground and background colors
*       specified.  Ensure that multiple pixmaps of the same attributes
*       are not created by maintaining a cache of the pixmaps.
*
************************************************************************/
Pixmap
_DtGetPixmap(
        Screen *screen,
        char *image_name,
        Pixel foreground,
        Pixel background )
{
    return (XmGetPixmap(screen, image_name, foreground, background));
}

/************************************************************************
*
*  _DtGetMask
*
************************************************************************/
Pixmap 
_DtGetMask(
        Screen *screen,
        char *image_name)
{
    return XmeGetMask( screen, image_name );
}

/************************************************************************
 *
 *  __DtInstallPixmap
 *      Install a pixmap into the pixmap cache.  This is used to add
 *      cached pixmaps which have no image associated with them.
 *
 ************************************************************************/
Boolean
__DtInstallPixmap(
        Pixmap pixmap,
        Screen *screen,
        char *image_name,
        Pixel foreground,
        Pixel background )
{
#ifdef DtUse_XmFunctions
    return _XmCachePixmap(pixmap, screen, image_name, foreground, background, 
			  0, 0, 0);
#else
    XtWarning("__DtInstallPixmap: unsupported interface");
    return False;
#endif
}

/************************************************************************
 *
 *  _DtDestroyPixmap
 *      Locate a pixmap in the cache and decrement its reference count.
 *      When the reference count is at zero, free the pixmap.
 *
 ************************************************************************/
Boolean
_DtDestroyPixmap(
        Screen *screen,
        Pixmap pixmap )
{
    return XmDestroyPixmap( screen, pixmap );
}

/************************************************************************
 *
 *  _DtInstallImage
 *      Add the provided image for the image set and return an
 *      tile id to be used for further referencing.  Keep the
 *      allocation of the image_set array straight.
 *
 ************************************************************************/
Boolean
_DtInstallImage(
        XImage *image,
        char *image_name,
        int hot_x,
        int hot_y)
{
#ifdef DtUse_XmFunctions
    return _XmInstallImage(image, image_name, hot_x, hot_y);
#else
    XtWarning("_DtInstallImage: unsupported interface");
    return False;
#endif
}

/************************************************************************
 *
 *  _DtUninstallImage
 *      Remove an image from the image set.
 *      Return a boolean (True) if the uninstall succeeded.  Return
 *      a boolean (False) if an error condition occurs.
 *
 ************************************************************************/
Boolean
_DtUninstallImage(
        XImage *image )
{
    return XmUninstallImage( image );
}

XImage *
_DtGetImage(
        Screen *screen,
        char *image_name,
        XImage **image)
{
    Boolean result;

#ifdef DtUse_XmFunctions
    if( (result=_XmGetImage(screen, image_name, image)) == False )
	return False;
    else
	return *image;
#else
    XtWarning("_DtGetImage: unsupported interface");
    return False;
#endif
}

/*
 * see if this pixmap is in the cache. If it is then return all the
 * gory details about it
 */
/*ARGSUSED*/
Boolean
__DtGetPixmapData(
                   Screen *screen,
                   Pixmap pixmap,
                   char **image_name,
                   int *depth,
                   Pixel *foreground,
                   Pixel *background,
                   int *hot_x,
                   int *hot_y,
                   unsigned int *width,
                   unsigned int *height)
{
    return XmeGetPixmapData(screen, pixmap, image_name, depth, foreground,
			    background, hot_x, hot_y, width, height);
}

String
_DtGetIconFileName(
    Screen      *screen,
    String      imageInstanceName,
    String      imageClassName,
    String      hostPrefix,
    unsigned int size)
{
	return XmGetIconFileName(screen, imageInstanceName, imageClassName, 
				 hostPrefix, size);
}

void
 _DtFlushIconFileCache(String   path)
{
	XmeFlushIconFileCache(path);
}
