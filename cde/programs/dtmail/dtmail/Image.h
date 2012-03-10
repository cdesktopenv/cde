/*
 *+SNOTICE
 *
 *	$XConsortium: Image.h /main/4 1996/04/21 19:42:14 drk $
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

#ifndef IMAGE_H
#define IMAGE_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>


class Image {
 private:
  XImage	*_image;
  unsigned int	_width,_height;
  char 		*_filename;
 public:
  Image( Widget, char * );
  ~Image();
  XImage *image() { return _image; }
};

#endif
