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
/* XlcPubI.h 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:05 	*/
/* $XConsortium: _fallcPubI.h /main/1 1996/04/08 15:17:59 cde-fuj $ */
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

#ifndef _XLCPUBLICI_H_
#define _XLCPUBLICI_H_

#include "_fallcPublic.h"

#define XLC_PUBLIC(lcd, x)	(((XLCdPublic) lcd->core)->pub.x)
#define XLC_PUBLIC_PART(lcd)	(&(((XLCdPublic) lcd->core)->pub))
#define XLC_PUBLIC_METHODS(lcd)	(&(((XLCdPublicMethods) lcd->methods)->pub))

/*
 * XLCd public methods
 */

typedef struct _XLCdPublicMethodsRec *XLCdPublicMethods;

typedef XLCd (*XlcPubCreateProc)(
    char*		/* name */,
    XLCdMethods		/* methods */
);

typedef Bool (*XlcPubInitializeProc)(
    XLCd		/* lcd */
);

typedef void (*XlcPubDestroyProc)(
    XLCd		/* lcd */
);

typedef char* (*XlcPubGetValuesProc)(
    XLCd		/* lcd */,
    XlcArgList		/* args */,
    int			/* num_args */
);

typedef void (*XlcPubGetResourceProc)(
    XLCd		/* lcd */,
    char*		/* category */,
    char*		/* class */,
    char***		/* value */,
    int*		/* count */
);

typedef struct _XLCdPublicMethodsPart {
    XLCdPublicMethods superclass;
    XlcPubCreateProc create;
    XlcPubInitializeProc initialize;
    XlcPubDestroyProc destroy;
    XlcPubGetValuesProc get_values;
    XlcPubGetResourceProc get_resource;
} XLCdPublicMethodsPart;

typedef struct _XLCdPublicMethodsRec {
    XLCdMethodsRec core;
    XLCdPublicMethodsPart pub;
} XLCdPublicMethodsRec;

/*
 * XLCd public data
 */

typedef struct _XLCdPublicPart {
    char *siname;			/* for _fallcMapOSLocaleName() */
    char *language;			/* language part of locale name */
    char *territory;			/* territory part of locale name */
    char *codeset;			/* codeset part of locale name */
    char *encoding_name;		/* encoding name */
    int mb_cur_max;			/* ANSI C MB_CUR_MAX */
    Bool is_state_depend;		/* state-depend encoding */
    char *default_string;		/* for falDefaultString() */
    XPointer xlocale_db;
} XLCdPublicPart;

typedef struct _XLCdPublicRec {
    XLCdCoreRec core;
    XLCdPublicPart pub;
} XLCdPublicRec, *XLCdPublic;

extern XLCdMethods _fallcPublicMethods;

_XFUNCPROTOBEGIN

extern XLCd _fallcCreateLC(
    char*		/* name */,
    XLCdMethods		/* methods */
);

extern void _fallcDestroyLC(
    XLCd		/* lcd */
);

extern Bool _fallcParseCharSet(
    XlcCharSet		/* charset */
);

extern XlcCharSet _fallcCreateDefaultCharSet(
    char*		/* name */,
    char*		/* control_sequence */
);

extern XlcCharSet _fallcAddCT(
    char*		/* name */,
    char*		/* control_sequence */
);

extern XrmMethods _falrmDefaultInitParseInfo(
    XLCd		/* lcd */,
    XPointer*		/* state */
);

extern int _falmbTextPropertyToTextList(
    XLCd		/* lcd */,
    Display*		/* dpy */,
    XTextProperty*	/* text_prop */,
    char***		/* list_ret */,
    int*		/* count_ret */
);

extern int _falwcTextPropertyToTextList(
    XLCd		/* lcd */,
    Display*		/* dpy */,
    XTextProperty*	/* text_prop */,
    wchar_t***		/* list_ret */,
    int*		/* count_ret */
);

extern int _falmbTextListToTextProperty(
    XLCd		/* lcd */,
    Display*		/* dpy */,
    char**		/* list */,
    int			/* count */,
    XICCEncodingStyle	/* style */,
    XTextProperty*	/* text_prop */
);

extern int _falwcTextListToTextProperty(
    XLCd		/* lcd */,
    Display*		/* dpy */,
    wchar_t**		/* list */,
    int			/* count */,
    XICCEncodingStyle	/* style */,
    XTextProperty*	/* text_prop */
);

extern void _falwcFreeStringList(
    XLCd		/* lcd */,
    wchar_t**		/* list */
);

extern int _fallcResolveLocaleName(
    char*		/* lc_name */,
    char*		/* full_name */,
    char*		/* language */,
    char*		/* territory */,
    char*		/* codeset */
);

extern int _fallcResolveDBName(
    char*		/* lc_name */,
    char*		/* file_name */
);

extern int _fallcResolveI18NPath(
    char*		/* path_name */
);

extern XPointer _fallcCreateLocaleDataBase(
    XLCd		/* lcd */
);

extern void _fallcDestroyLocaleDataBase(
    XLCd		/* lcd */
);

extern void _fallcGetLocaleDataBase(
    XLCd		/* lcd */,
    char*		/* category */,
    char*		/* name */,
    char***		/* value */,
    int*		/* count */
);

_XFUNCPROTOEND

#endif  /* _XLCPUBLICI_H_ */
