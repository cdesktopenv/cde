/* $XConsortium: spec.h /main/6 1996/07/01 15:35:25 cde-hal $ */
/* specification for the external entry point for the CGM->Pixmap library */
#ifndef spec_h
#define spec_h

/*
#include "GraphicsP.h"
*/
#include <DtI/GraphicsP.h>

extern enum _DtGrLoadStatus processCGM(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context);
#endif
