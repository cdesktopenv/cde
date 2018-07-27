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
/* lcTxtPr.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:41 	*/
/* $XConsortium: _fallcTxtPr.c /main/1 1996/04/08 15:19:12 cde-fuj $ */
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
#include "_falutil.h"
#include <X11/Xatom.h>
#include <stdio.h>

static int
get_buf_size(Bool is_wide_char, XPointer list, int count)
{
    int length = 0;
    char **mb_list;
    wchar_t **wc_list;

    if (list == NULL)
	return 0;

    if (is_wide_char) {
	wc_list = (wchar_t **) list;
	for ( ; count-- > 0; wc_list++) {
	    if (*wc_list)
		length += _falwcslen(*wc_list) + 1;
	}
	length *= 5;	/* XXX */
    } else {
	mb_list = (char **) list;
	for ( ; count-- > 0; mb_list++) {
	    if (*mb_list)
		length += strlen(*mb_list) + 1;
	}
	length *= 3;	/* XXX */
    }
    length = (length / BUFSIZ + 1) * BUFSIZ;	/* XXX */

    return length;
}

static int
_XTextListToTextProperty(
    XLCd lcd,
    Display *dpy,
    char *from_type,
    XPointer list,
    int count,
    XICCEncodingStyle style,
    XTextProperty *text_prop)
{
    Atom encoding;
    XlcConv conv;
    char *to_type;
    char **mb_list;
    wchar_t **wc_list;
    XPointer from;
    char *to, *buf, *value;
    int from_left, to_left, buf_len, nitems, unconv_num, ret, i, retry, done =0;
    Bool is_wide_char = False;

    if (strcmp(XlcNWideChar, from_type) == 0)
	is_wide_char = True;

    buf_len = get_buf_size(is_wide_char, list, count);
    if ((buf = (char *) Xmalloc(buf_len)) == NULL)
	return XNoMemory;

    switch (style) {
	case XStringStyle:
	case XStdICCTextStyle:
	    encoding = XA_STRING;
	    to_type = XlcNString;
	    break;
	case XCompoundTextStyle:
	    encoding = falInternAtom(dpy, "COMPOUND_TEXT", False);
	    to_type = XlcNCompoundText;
	    break;
	case XTextStyle:
	    encoding = falInternAtom(dpy, XLC_PUBLIC(lcd, encoding_name), False);
	    to_type = XlcNMultiByte;
	    if (is_wide_char == False) {
		nitems = 0;
		mb_list = (char **) list;
		to = buf;
		for (i = 0; i < count; i++) {
		    strcpy(to, *mb_list);
		    from_left = strlen(*mb_list) + 1;
		    nitems += from_left;
		    to += from_left;
		    mb_list++;
		}
		unconv_num = 0;
		done++;
	    }
	    break;
	default:
	    Xfree(buf);
	    return XConverterNotFound;
    }

    if (count < 1) {
	nitems = 0;
	done++;
    }

    if(done == 1){
	if (nitems <= 0)
		nitems = 1;
	value = (char *) Xmalloc(nitems);
	if (value == NULL) {
	    Xfree(buf);
	    return XNoMemory;
	}
	if (nitems == 1)
	    *value = 0;
	else
	    memcpy(value, buf, nitems);
	nitems--;
	Xfree(buf);

	text_prop->value = (unsigned char *) value;
	text_prop->encoding = encoding;
	text_prop->format = 8;
	text_prop->nitems = nitems;

	return unconv_num;
    }

    do{
	retry = 0;
	conv = _fallcOpenConverter(lcd, from_type, lcd, to_type);
	if (conv == NULL) {
	    Xfree(buf);
	    return XConverterNotFound;
	}

	if (is_wide_char)
	    wc_list = (wchar_t **) list;
	else
	    mb_list = (char **) list;

	to = buf;
	to_left = buf_len;

	unconv_num = 0;

	for (i = 1; to_left > 0; i++) {
	    if (is_wide_char) {
		from = (XPointer) *wc_list;
		from_left = _falwcslen(*wc_list);
		wc_list++;
	    } else {
		from = (XPointer) *mb_list;
		from_left = strlen(*mb_list);
		mb_list++;
	    }

	    ret = _fallcConvert(conv, &from, &from_left, (XPointer *) &to, &to_left,
			  NULL, 0);

	    if (ret < 0)
		continue;

	    if (ret > 0 && style == XStdICCTextStyle && encoding == XA_STRING) {
		_fallcCloseConverter(conv);
		encoding = falInternAtom(dpy, "COMPOUND_TEXT", False);
		to_type = XlcNCompoundText;
	        retry++;
	    }

	    unconv_num += ret;
	    *to++ = '\0';
	    to_left--;

	    if (i >= count)
		break;

	    _fallcResetConverter(conv);
	}

    _fallcCloseConverter(conv);

    nitems = to - buf;
    }while (retry == 1);
}

int
_falmbTextListToTextProperty(
    XLCd lcd,
    Display *dpy,
    char **list,
    int count,
    XICCEncodingStyle style,
    XTextProperty *text_prop)
{
    return _XTextListToTextProperty(lcd, dpy, XlcNMultiByte, (XPointer) list,
				    count, style, text_prop);
}

int
_falwcTextListToTextProperty(
    XLCd lcd,
    Display *dpy,
    wchar_t **list,
    int count,
    XICCEncodingStyle style,
    XTextProperty *text_prop)
{
    return _XTextListToTextProperty(lcd, dpy, XlcNWideChar, (XPointer) list,
				    count, style, text_prop);
}
