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
