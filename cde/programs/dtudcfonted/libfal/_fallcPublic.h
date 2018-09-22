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
/* XlcPublic.h 1.3 - Fujitsu source for CDEnext    95/12/07 10:53:08 	*/
/* $XConsortium: _fallcPublic.h /main/1 1996/04/08 15:18:31 cde-fuj $ */
/*
 * Copyright 1992, 1993 by TOSHIBA Corp.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TOSHIBA not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. TOSHIBA make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * TOSHIBA DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * Author: Katsuhisa Yano	TOSHIBA Corp.
 *			   	mopi@osa.ilab.toshiba.co.jp
 */
/*
 * Copyright 1995 by FUJITSU LIMITED
 * This is source code modified by FUJITSU LIMITED under the Joint
 * Development Agreement for the CDEnext PST.
 * This is unpublished proprietry source code of FUJITSU LIMITED
 *
 * Modifier: Takanori Tateno   FUJITSU LIMITED
 *
 */

#ifndef _XLCPUBLIC_H_
#define _XLCPUBLIC_H_

#include "_fallcint.h"

#define XlcNCharSize 		"charSize"
#define XlcNCodeset 		"codeset"
#define XlcNControlSequence 	"controlSequence"
#define XlcNDefaultString 	"defaultString"
#define XlcNEncodingName 	"encodingName"
#define XlcNLanguage 		"language"
#define XlcNMbCurMax 		"mbCurMax"
#define XlcNName 		"name"
#define XlcNSetSize 		"setSize"
#define XlcNSide 		"side"
#define XlcNStateDependentEncoding "stateDependentEncoding"
#define XlcNTerritory 		"territory"

typedef enum {
    XlcUnknown, XlcC0, XlcGL, XlcC1, XlcGR, XlcGLGR, XlcOther, XlcNONE
} XlcSide;

typedef struct _FonScope {
        unsigned long   start;
        unsigned long   end;
        unsigned long   shift;
        unsigned long   shift_direction;
} FontScopeRec, *FontScope;

typedef struct _UDCArea {
        unsigned long 	start,end;
} UDCAreaRec, *UDCArea;

typedef struct _XlcCharSetRec *XlcCharSet;

typedef char* (*XlcGetCSValuesProc)(
    XlcCharSet		/* charset */,
    XlcArgList		/* args */,
    int			/* num_args */
);

typedef struct _XlcCharSetRec {
    char 		*name;		/* character set name */
    XrmQuark 		xrm_name;
    char 		*encoding_name;	/* XLFD encoding name */
    XrmQuark 		xrm_encoding_name;
    XlcSide 		side;		/* GL, GR or others */
    int 		char_size;	/* number of bytes per character */
    int 		set_size;	/* graphic character sets */
    char 		*ct_sequence;	/* control sequence of CT */
    XlcGetCSValuesProc 	get_values;
    /* UDC */
    Bool        	string_encoding;
    UDCArea 		udc_area;
    int     		udc_area_num;
} XlcCharSetRec;

/*
 * conversion methods
 */

typedef struct _XlcConvRec *XlcConv;

typedef XlcConv (*XlcOpenConverterProc)(
    XLCd		/* from_lcd */,
    char*		/* from_type */,
    XLCd		/* to_lcd */,
    char*		/* to_type */
);

typedef void (*XlcCloseConverterProc)(
    XlcConv		/* conv */
);

typedef int (*XlcConvertProc)(
    XlcConv		/* conv */,
    XPointer*		/* from */,
    int*		/* from_left */,
    XPointer*		/* to */,
    int*		/* to_left */,
    XPointer*		/* args */,
    int			/* num_args */
);

typedef void (*XlcResetConverterProc)(
    XlcConv		/* conv */
);

typedef struct _XlcConvMethodsRec{
    XlcCloseConverterProc 	close;
    XlcConvertProc 		convert;
    XlcResetConverterProc 	reset;
} XlcConvMethodsRec, *XlcConvMethods;

/*
 * conversion data
 */

#define XlcNMultiByte 		"multiByte"
#define XlcNWideChar 		"wideChar"
#define XlcNCompoundText 	"compoundText"
#define XlcNString 		"string"
#define XlcNCharSet 		"charSet"
#define XlcNCTCharSet 		"CTcharSet"
#define XlcNChar 		"char"

typedef struct _XlcConvRec {
    XlcConvMethods 		methods;
    XPointer 			state;
} XlcConvRec;


_XFUNCPROTOBEGIN

extern Bool _falInitOM(
    XLCd		/* lcd */
);

extern Bool _XInitIM(
    XLCd		/* lcd */
);

extern char *_falGetLCValues(
    XLCd		/* lcd */,
    ...
);

extern XlcCharSet _fallcGetCharSet(
    char*		/* name */
);

extern Bool _fallcAddCharSet(
    XlcCharSet		/* charset */
);

extern char *_fallcGetCSValues(
    XlcCharSet		/* charset */,
    ...
);

extern XlcConv _fallcOpenConverter(
    XLCd		/* from_lcd */,
    char*		/* from_type */,
    XLCd		/* to_lcd */,
    char*		/* to_type */
);

extern void _fallcCloseConverter(
    XlcConv		/* conv */
);

extern int _fallcConvert(
    XlcConv		/* conv */,
    XPointer*		/* from */,
    int*		/* from_left */,
    XPointer*		/* to */,
    int*		/* to_left */,
    XPointer*		/* args */,
    int			/* num_args */
);

extern void _fallcResetConverter(
    XlcConv		/* conv */
);

extern Bool _fallcSetConverter(
    XLCd			/* from_lcd */,
    char*			/* from_type */,
    XLCd			/* to_lcd */,
    char*			/* to_type */,
    XlcOpenConverterProc	/* open_converter */
);

extern void _fallcGetResource(
    XLCd		/* lcd */,
    char*		/* category */,
    char*		/* class */,
    char***		/* value */,
    int*		/* count */
);

extern char *_fallcFileName(
    XLCd		/* lcd */,
    char*		/* category */
);

extern int _falwcslen(
    wchar_t*		/* wstr */
);

extern wchar_t *_falwcscpy(
    wchar_t*		/* wstr1 */,
    wchar_t*		/* wstr2 */
);

extern int _fallcCompareISOLatin1(
    char*		/* str1 */,
    char*		/* str2 */
);

extern int _fallcNCompareISOLatin1(
    char*		/* str1 */,
    char*		/* str2 */,
    int			/* len */
);

_XFUNCPROTOEND

#endif  /* _XLCPUBLIC_H_ */
