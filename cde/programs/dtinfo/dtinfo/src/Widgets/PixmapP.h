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
