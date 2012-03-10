/*
 * $XConsortium: TermPrimRenderLineDraw.h /main/1 1996/04/21 19:19:02 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimRenderLineDraw_h
#define	_Dt_TermPrimRenderLineDraw_h

#include "TermPrimRender.h"

extern TermFont _DtTermPrimRenderLineDrawCreate(
    Widget		  w,
    GlyphInfo		  glyphInfo,
    int			  numGlyphs,
    int			  width,
    int			  ascent,
    int			  descent
);

#endif	/* _Dt_TermPrimRenderLineDraw_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
