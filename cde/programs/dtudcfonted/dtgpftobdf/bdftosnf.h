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
/* $XConsortium: bdftosnf.h /main/4 1996/06/25 20:13:29 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include<X11/Xfuncs.h>

#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

typedef struct _GlyphMap {
    char	*bits;
    int		h;
    int		w;
    int		widthBytes;
} GlyphMap;

/*
 * a structure to hold all the pointers to make it easy to pass them all
 * around. Much like the FONT structure in the server.
 */

typedef struct _TempFont {
    FontInfoPtr pFI;
    CharInfoPtr pCI;
    unsigned char *pGlyphs;
    FontPropPtr pFP;
    CharInfoPtr pInkCI;
    CharInfoPtr pInkMin;
    CharInfoPtr pInkMax;
} TempFont; /* not called font since collides with type in X.h */

#ifdef vax
#	define DEFAULTGLPAD 	1		/* default padding for glyphs */
#	define DEFAULTBITORDER 	LSBFirst	/* default bitmap bit order */
#	define DEFAULTBYTEORDER LSBFirst	/* default bitmap byte order */
#	define DEFAULTSCANUNIT	1		/* default bitmap scan unit */

#elif defined(sun)

#  if (sun386 || sun5)
#	define DEFAULTGLPAD 	4		/* default padding for glyphs */
#	define DEFAULTBITORDER 	LSBFirst	/* default bitmap bit order */
#	define DEFAULTBYTEORDER LSBFirst	/* default bitmap byte order */
#	define DEFAULTSCANUNIT	1		/* default bitmap scan unit */
#  else
#	define DEFAULTGLPAD 	4		/* default padding for glyphs */
#	define DEFAULTBITORDER 	MSBFirst	/* default bitmap bit order */
#	define DEFAULTBYTEORDER MSBFirst	/* default bitmap byte order */
#	define DEFAULTSCANUNIT	1		/* default bitmap scan unit */
#  endif

#elif defined(ibm032)

#	define DEFAULTGLPAD 	1		/* default padding for glyphs */
#	define DEFAULTBITORDER 	MSBFirst	/* default bitmap bit order */
#	define DEFAULTBYTEORDER MSBFirst	/* default bitmap byte order */
#	define DEFAULTSCANUNIT	1		/* default bitmap scan unit */

#elif defined(hpux)

#	define DEFAULTGLPAD 	2		/* default padding for glyphs */
#	define DEFAULTBITORDER 	MSBFirst	/* default bitmap bit order */
#	define DEFAULTBYTEORDER MSBFirst	/* default bitmap byte order */
#	define DEFAULTSCANUNIT	1		/* default bitmap scan unit */

#elif defined(mips)
#    ifdef MIPSEL

#	  define DEFAULTGLPAD 	  4		/* default padding for glyphs */
#	  define DEFAULTBITORDER  LSBFirst	/* default bitmap bit order */
#	  define DEFAULTBYTEORDER LSBFirst	/* default bitmap byte order */
#	  define DEFAULTSCANUNIT  1		/* default bitmap scan unit */

#     else
#	  define DEFAULTGLPAD 	  4		/* default padding for glyphs */
#	  define DEFAULTBITORDER  MSBFirst	/* default bitmap bit order */
#	  define DEFAULTBYTEORDER MSBFirst	/* default bitmap byte order */
#	  define DEFAULTSCANUNIT  1		/* default bitmap scan unit */
#     endif

#else
#	  define DEFAULTGLPAD     1		/* default padding for glyphs */
#	  define DEFAULTBITORDER  MSBFirst	/* default bitmap bit order */
#	  define DEFAULTBYTEORDER MSBFirst	/* default bitmap byte order */
#	  define DEFAULTSCANUNIT  1		/* default bitmap scan unit */
#endif

#define GLWIDTHBYTESPADDED(bits,nbytes) \
	((nbytes) == 1 ? (((bits)+7)>>3)	/* pad to 1 byte */ \
	:(nbytes) == 2 ? ((((bits)+15)>>3)&~1)	/* pad to 2 bytes */ \
	:(nbytes) == 4 ? ((((bits)+31)>>3)&~3)	/* pad to 4 bytes */ \
	:(nbytes) == 8 ? ((((bits)+63)>>3)&~7)	/* pad to 8 bytes */ \
	: 0)
