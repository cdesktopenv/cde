/* 
** $XConsortium: TermPrimRenderP.h /main/1 1996/04/21 19:19:08 drk $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef   _Dt_TermPrimRenderP_h
#define   _Dt_TermPrimRenderP_h

#include  "TermPrimRender.h"

#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */

typedef void (*TermFontRenderFunction)(
    Widget		  w,
    TermFont		  font,
    Pixel		  fg,
    Pixel		  bg,
    unsigned long	  flags,
    int			  x,
    int			  y,
    unsigned char	 *string,
    int			  len
);

typedef void (*TermFontDestroyFunction)(
    Widget		  w,
    TermFont		  font
);

typedef void (*TermFontExtentsFunction)(
    Widget		  w,
    TermFont		  font,
    unsigned char	 *string,
    int			  len,
    int			 *widthReturn,
    int			 *heightReturn,
    int			 *ascentReturn
);

typedef struct _TermFontRec {
    TermFontRenderFunction	renderFunction;
    TermFontDestroyFunction	destroyFunction;
    TermFontExtentsFunction	extentsFunction;
    XtPointer			fontInfo;
} TermFontRec;

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimRenderP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
