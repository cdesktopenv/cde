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
/* lcDefConv.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:35 	*/
/* $XConsortium: _fallcDefConv.c /main/1 1996/04/08 15:16:17 cde-fuj $ */
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

#include "_fallibint.h"
#include "_fallcPubI.h"

typedef struct _StateRec {
    XlcCharSet charset;
    XlcCharSet GL_charset;
    XlcCharSet GR_charset;
    XlcConv ct_conv;
    int (*to_converter)();
} StateRec, *State;

static int
strtostr(
    XlcConv conv,
    XPointer *from,
    int *from_left,
    XPointer *to,
    int *to_left,
    XPointer *args,
    int num_args)
{
    char *src, *dst;
    unsigned char side;
    int length;

    if (from == NULL || *from == NULL)
	return 0;

    src = (char *) *from;
    dst = (char *) *to;

    length = min(*from_left, *to_left);

    if (num_args > 0) {
	side = *((unsigned char *) src) & 0x80;
	while (side == (*((unsigned char *) src) & 0x80) && length-- > 0)
	    *dst++ = *src++;
    } else {
	while (length-- > 0)
	    *dst++ = *src++;
    }

    *from_left -= src - (char *) *from;
    *from = (XPointer) src;
    *to_left -= dst - (char *) *to;
    *to = (XPointer) dst;

    if (num_args > 0) {
	State state = (State) conv->state;

	*((XlcCharSet *)args[0]) = side ? state->GR_charset : state->GL_charset;
    }

    return 0;
}

static int
wcstostr(
    XlcConv conv,
    XPointer *from,
    int *from_left,
    XPointer *to,
    int *to_left,
    XPointer *args,
    int num_args)
{
    wchar_t *src, side;
    char *dst;
    int length;

    if (from == NULL || *from == NULL)
	return 0;

    src = (wchar_t *) *from;
    dst = (char *) *to;

    length = min(*from_left, *to_left);

    if (num_args > 0) {
	side = *src & 0x80;
	while (side == (*src & 0x80) && length-- > 0)
	    *dst++ = *src++;
    } else {
	while (length-- > 0)
	    *dst++ = *src++;
    }

    *from_left -= src - (wchar_t *) *from;
    *from = (XPointer) src;
    *to_left -= dst - (char *) *to;
    *to = (XPointer) dst;

    if (num_args > 0) {
	State state = (State) conv->state;

	*((XlcCharSet *)args[0]) = side ? state->GR_charset : state->GL_charset;
    }

    return 0;
}

static int
cstostr(
    XlcConv conv,
    XPointer *from,
    int *from_left,
    XPointer *to,
    int *to_left,
    XPointer *args,
    int num_args)
{
    char *src, *dst;
    unsigned char side;
    int length;

    if (from == NULL || *from == NULL)
	return 0;

    if (num_args > 0) {
	State state = (State) conv->state;
	XlcCharSet charset = (XlcCharSet) args[0];

	if (charset != state->GL_charset && charset != state->GR_charset)
	    return -1;
    }

    src = (char *) *from;
    dst = (char *) *to;

    length = min(*from_left, *to_left);

    if (num_args > 0) {
	side = *((unsigned char *) src) & 0x80;
	while (side == (*((unsigned char *) src) & 0x80) && length-- > 0)
	    *dst++ = *src++;
    } else {
	while (length-- > 0)
	    *dst++ = *src++;
    }

    *from_left -= src - (char *) *from;
    *from = (XPointer) src;
    *to_left -= dst - (char *) *to;
    *to = (XPointer) dst;

    if (num_args > 0) {
	State state = (State) conv->state;

	*((XlcCharSet *)args[0]) = side ? state->GR_charset : state->GL_charset;
    }

    return 0;
}

static int
strtowcs(
    XlcConv conv,
    XPointer *from,
    int *from_left,
    XPointer *to,
    int *to_left,
    XPointer *args,
    int num_args)
{
    char *src;
    wchar_t *dst;
    int length;

    if (from == NULL || *from == NULL)
	return 0;

    if (num_args > 0) {
	State state = (State) conv->state;
	XlcCharSet charset = (XlcCharSet) args[0];

	if (charset != state->GL_charset && charset != state->GR_charset)
	    return -1;
    }

    src = (char *) *from;
    dst = (wchar_t *) *to;

    length = min(*from_left, *to_left);

    while (length-- > 0)
	*dst++ = (wchar_t) *src++;

    *from_left -= src - (char *) *from;
    *from = (XPointer) src;
    *to_left -= dst - (wchar_t *) *to;
    *to = (XPointer) dst;

    return 0;
}


static void
close_converter(XlcConv conv)
{
    if (conv->state)
	Xfree((char *) conv->state);

    Xfree((char *) conv);
}

static XlcConv
create_conv(XlcConvMethods methods)
{
    XlcConv conv;
    State state;
    static XlcCharSet GL_charset = NULL;
    static XlcCharSet GR_charset = NULL;

    if (GL_charset == NULL) {
	GL_charset = _fallcGetCharSet("ISO8859-1:GL");
	GR_charset = _fallcGetCharSet("ISO8859-1:GR");
    }

    conv = (XlcConv) Xmalloc(sizeof(XlcConvRec));
    if (conv == NULL)
	return (XlcConv) NULL;

    conv->state = NULL;

    state = (State) Xmalloc(sizeof(StateRec));
    if (state == NULL){
	close_converter(conv);
	return (XlcConv) NULL;
    }

    state->GL_charset = state->charset = GL_charset;
    state->GR_charset = GR_charset;

    conv->methods = methods;
    conv->state = (XPointer) state;

    return conv;
}

static XlcConvMethodsRec strtostr_methods = {
    close_converter,
    strtostr,
    NULL
} ;

static XlcConv
open_strtostr( XLCd from_lcd, char *from_type, XLCd to_lcd, char *to_type )
{
    return create_conv(&strtostr_methods);
}

static XlcConvMethodsRec wcstostr_methods = {
    close_converter,
    wcstostr,
    NULL
} ;

static XlcConv
open_wcstostr( XLCd from_lcd, char *from_type, XLCd to_lcd, char *to_type )
{
    return create_conv(&wcstostr_methods);
}

static XlcConvMethodsRec cstostr_methods = {
    close_converter,
    cstostr,
    NULL
} ;

static XlcConv
open_cstostr( XLCd from_lcd, char *from_type, XLCd to_lcd, char *to_type )
{
    return create_conv(&cstostr_methods);
}

static XlcConvMethodsRec strtowcs_methods = {
    close_converter,
    strtowcs,
    NULL
} ;

static XlcConv
open_strtowcs( XLCd from_lcd, char *from_type, XLCd to_lcd, char *to_type )
{
    return create_conv(&strtowcs_methods);
}

XLCd
_fallcDefaultLoader(char *name)
{
    XLCd lcd;

    if (strcmp(name, "C"))
	return (XLCd) NULL;

    lcd = _fallcCreateLC(name, _fallcPublicMethods);

    _fallcSetConverter(lcd, XlcNMultiByte, lcd, XlcNWideChar, open_strtowcs);
    _fallcSetConverter(lcd, XlcNMultiByte, lcd, XlcNCompoundText, open_strtostr);
    _fallcSetConverter(lcd, XlcNMultiByte, lcd, XlcNString, open_strtostr);
    _fallcSetConverter(lcd, XlcNMultiByte, lcd, XlcNCharSet, open_strtostr);
    _fallcSetConverter(lcd, XlcNMultiByte, lcd, XlcNChar, open_strtostr);/* XXX */

    _fallcSetConverter(lcd, XlcNWideChar, lcd, XlcNMultiByte, open_wcstostr);
    _fallcSetConverter(lcd, XlcNWideChar, lcd, XlcNCompoundText, open_wcstostr);
    _fallcSetConverter(lcd, XlcNWideChar, lcd, XlcNString, open_wcstostr);
    _fallcSetConverter(lcd, XlcNWideChar, lcd, XlcNCharSet, open_wcstostr);

    _fallcSetConverter(lcd, XlcNString, lcd, XlcNMultiByte, open_strtostr);
    _fallcSetConverter(lcd, XlcNString, lcd, XlcNWideChar, open_strtowcs);

    _fallcSetConverter(lcd, XlcNCharSet, lcd, XlcNMultiByte, open_cstostr);
    _fallcSetConverter(lcd, XlcNCharSet, lcd, XlcNWideChar, open_strtowcs);

    return lcd;
}
