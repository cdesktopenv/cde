/* $XConsortium: Pixmap.h /main/3 1996/06/11 16:45:38 cde-hal $ */
#ifndef _Pixmap_h
#define _Pixmap_h

#include <X11/StringDefs.h>

_XFUNCPROTOBEGIN

externalref WidgetClass pixmapWidgetClass;

typedef struct _PixmapClassRec * PixmapWidgetClass;
typedef struct _PixmapRec      * PixmapWidget;

#define IsPixmapWidget(w) XtIsSubclass(w, pixmapWidgetClass)

/* access to pixmap width/height */
extern void PixmapWidgetDimensions(Widget w, Dimension *width, Dimension *height);


_XFUNCPROTOEND

#endif 
/* DON'T ADD STUFF AFTER THIS #endif */
