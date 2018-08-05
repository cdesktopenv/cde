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
/* $XConsortium: font.h /main/1 1995/09/14 20:54:34 cde-fuj2 $ */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

(c) Copyright 1995 FUJITSU LIMITED
This is source code modified by FUJITSU LIMITED under the Joint
Development Agreement for the CDEnext PST.
This is unpublished proprietary source code of FUJITSU LIMITED

******************************************************************/
#ifndef FONT_H
#define FONT_H 1

#include "servermd.h"

#define NullCharInfo ((CharInfoPtr)0)
#define NullFontInfo ((FontInfoPtr)0)
#define LeftToRight 0
#define RightToLeft 1
/*
 * for linear char sets
 */
#define n1dChars(pfi) ((pfi)->lastCol - (pfi)->firstCol + 1)
#define chFirst firstCol	/* usage:  pfi->chFirst */
#define chLast lastCol		/* usage:  pfi->chLast */

/*
 * for 2D char sets
 */
#define n2dChars(pfi)	(((pfi)->lastCol - (pfi)->firstCol + 1) * \
			 ((pfi)->lastRow - (pfi)->firstRow + 1))

#define ADDRXTHISCHARINFO( pf, ch ) \
        ((CharInfoRec *) &((pf)->pCI[(ch) - (pf)->pFI->chFirst]))

#define	GLWIDTHPIXELS(pci) \
	((pci)->metrics.rightSideBearing - (pci)->metrics.leftSideBearing)
#define	GLHEIGHTPIXELS(pci) \
	((pci)->metrics.ascent + (pci)->metrics.descent)


#define	GLYPHWIDTHBYTES(pci)	(((GLYPHWIDTHPIXELS(pci))+7) >> 3)
#define	GLYPHHEIGHTPIXELS(pci)	(pci->metrics.ascent + pci->metrics.descent)
#define	GLYPHWIDTHPIXELS(pci)	(pci->metrics.rightSideBearing \
				    - pci->metrics.leftSideBearing)
#define GLWIDTHPADDED( bc)	((bc+7) & ~0x7)

#if GLYPHPADBYTES == 0 || GLYPHPADBYTES == 1
#define	GLYPHWIDTHBYTESPADDED(pci)	(GLYPHWIDTHBYTES(pci))
#define	PADGLYPHWIDTHBYTES(w)		(((w)+7)>>3)
#endif

#if GLYPHPADBYTES == 2
#define	GLYPHWIDTHBYTESPADDED(pci)	((GLYPHWIDTHBYTES(pci)+1) & ~0x1)
#define	PADGLYPHWIDTHBYTES(w)		(((((w)+7)>>3)+1) & ~0x1)
#endif

#if GLYPHPADBYTES == 4
#define	GLYPHWIDTHBYTESPADDED(pci)	((GLYPHWIDTHBYTES(pci)+3) & ~0x3)
#define	PADGLYPHWIDTHBYTES(w)		(((((w)+7)>>3)+3) & ~0x3)
#endif

#if GLYPHPADBYTES == 8 /* for a cray? */
#define	GLYPHWIDTHBYTESPADDED(pci)	((GLYPHWIDTHBYTES(pci)+7) & ~0x7)
#define	PADGLYPHWIDTHBYTES(w)		(((((w)+7)>>3)+7) & ~0x7)
#endif

typedef struct _FontProp *FontPropPtr;
typedef struct _CharInfo *CharInfoPtr;
typedef struct _FontInfo *FontInfoPtr;
typedef unsigned int DrawDirection;
typedef struct _ExtentInfo *ExtentInfoPtr;


#endif /* FONT_H */
