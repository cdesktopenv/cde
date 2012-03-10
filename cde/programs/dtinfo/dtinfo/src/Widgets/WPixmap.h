/* $XConsortium: WPixmap.h /main/3 1996/06/11 16:46:20 cde-hal $ */
#ifndef _WPixmap_h
#define _WPixmap_h

#include <WWL/WCore.h>
#include "Pixmap.h"

class WPixmap : public WCore {
public :
	DEFINE_INIT (WPixmap, WCore, pixmapWidgetClass)
	DEFINE_GETTER_SETTER(Picture, Pixmap, XtNpixmap)
};

#define	NULLWVWrapper		WVWrapper((Widget)0)

#endif /* _WVWrapper_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
