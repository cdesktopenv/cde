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
/*
 *+SNOTICE
 *
 *	$XConsortium: Image.C /main/4 1996/04/21 19:42:11 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifdef DEAD_WOOD

#include "Image.h"
#include "stdlib.h"
Image::Image( Widget w, char *filename )
{
  GC gc;
  Pixmap bitmap,pixmap;
  int xhot,yhot;
  XGCValues    values;
  Display *display=XtDisplay( w );

  _filename=strdup(filename);



  XReadBitmapFile( display,
		  RootWindowOfScreen(XtScreen(w)),
		  _filename,
		  &_width,
		  &_height,
		  &bitmap,
		  &xhot,
		  &yhot );

  pixmap = XCreatePixmap( display, 
			 RootWindowOfScreen( XtScreen(w) ),
			 _width,
			 _height,
			 DefaultDepthOfScreen( XtScreen(w) ));

 // XtVaGetValues( w,
	//	XtNfontColor, &values.foreground,
	//	XtNbackground, &values.background,
	//	NULL );

  gc = XtGetGC( w, GCForeground | GCBackground, &values );

  XCopyPlane( display, bitmap, pixmap, gc, 0, 0, _width, _height, 0 ,0 ,1 ); 
  _image = XGetImage( display, pixmap,
		     0, 0,
		     _width, _height,
		     AllPlanes, ZPixmap );
  
}

Image::~Image(){
free ( _filename );
}

#endif /* DEAD_WOOD */
