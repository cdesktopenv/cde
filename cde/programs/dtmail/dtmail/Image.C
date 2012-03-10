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
