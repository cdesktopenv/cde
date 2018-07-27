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
/* lcStd.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:41 	*/
/* $XConsortium: _fallcStd.c /main/1 1996/04/08 15:19:02 cde-fuj $ */
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

int
_fallcmbtowc(XLCd lcd, wchar_t *wstr, char *str, int len)
{
    static XLCd last_lcd = NULL;
    static XlcConv conv = NULL;
    XPointer from, to;
    int from_left, to_left;
    wchar_t tmp_wc;

    if (lcd == NULL) {
	lcd = _fallcCurrentLC();
	if (lcd == NULL)
	    return -1;
    }
    if (str == NULL)
	return XLC_PUBLIC(lcd, is_state_depend);

    if (conv && lcd != last_lcd) {
	_fallcCloseConverter(conv);
	conv = NULL;
    }

    last_lcd = lcd;

    if (conv == NULL) {
	conv = _fallcOpenConverter(lcd, XlcNMultiByte, lcd, XlcNWideChar);
	if (conv == NULL)
	    return -1;
    }

    from = (XPointer) str;
    from_left = len;
    to = (XPointer) (wstr ? wstr : &tmp_wc);
    to_left = 1;

    if (_fallcConvert(conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
	return -1;

    return (len - from_left);
}

int
_fallcwctomb(XLCd lcd, char *str, wchar_t wc)
{
    static XLCd last_lcd = NULL;
    static XlcConv conv = NULL;
    XPointer from, to;
    int from_left, to_left, length;

    if (lcd == NULL) {
	lcd = _fallcCurrentLC();
	if (lcd == NULL)
	    return -1;
    }
    if (str == NULL)
	return XLC_PUBLIC(lcd, is_state_depend);

    if (conv && lcd != last_lcd) {
	_fallcCloseConverter(conv);
	conv = NULL;
    }

    last_lcd = lcd;

    if (conv == NULL) {
	conv = _fallcOpenConverter(lcd, XlcNWideChar, lcd, XlcNMultiByte);
	if (conv == NULL)
	    return -1;
    }

    from = (XPointer) &wc;
    from_left = 1;
    to = (XPointer) str;
    length = to_left = XLC_PUBLIC(lcd, mb_cur_max);

    if (_fallcConvert(conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
	return -1;

    return (length - to_left);
}

int
_fallcmbstowcs(XLCd lcd, wchar_t *wstr, char *str, int len)
{
    XlcConv conv;
    XPointer from, to;
    int from_left, to_left, ret;

    if (lcd == NULL) {
	lcd = _fallcCurrentLC();
	if (lcd == NULL)
	    return -1;
    }

    conv = _fallcOpenConverter(lcd, XlcNMultiByte, lcd, XlcNWideChar);
    if (conv == NULL)
	return -1;

    from = (XPointer) str;
    from_left = strlen(str);
    to = (XPointer) wstr;
    to_left = len;

    if (_fallcConvert(conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
	ret = -1;
    else {
	ret = len - to_left;
	if (wstr && to_left > 0)
	    wstr[ret] = (wchar_t) 0;
    }

    _fallcCloseConverter(conv);

    return ret;
}

int
_fallcwcstombs(XLCd lcd, char *str, wchar_t *wstr, int len)
{
    XlcConv conv;
    XPointer from, to;
    int from_left, to_left, ret;

    if (lcd == NULL) {
	lcd = _fallcCurrentLC();
	if (lcd == NULL)
	    return -1;
    }

    conv = _fallcOpenConverter(lcd, XlcNWideChar, lcd, XlcNMultiByte);
    if (conv == NULL)
	return -1;

    from = (XPointer) wstr;
    from_left = _falwcslen(wstr);
    to = (XPointer) str;
    to_left = len;

    if (_fallcConvert(conv, &from, &from_left, &to, &to_left, NULL, 0) < 0)
	ret = -1;
    else {
	ret = len - to_left;
	if (str && to_left > 0)
	    str[ret] = '\0';
    }

    _fallcCloseConverter(conv);

    return ret;
}


int
_falmbtowc(wchar_t *wstr, char *str, int len)
{
    return _fallcmbtowc((XLCd) NULL, wstr, str, len);
}

int
_falmblen(char *str, int len)
{
    return _falmbtowc((wchar_t *) NULL, str, len);
}

int
_falwctomb(char *str, wchar_t wc)
{
    return _fallcwctomb((XLCd) NULL, str, wc);
}

int
_falmbstowcs(wchar_t *wstr, char *str, int len)
{
    return _fallcmbstowcs((XLCd) NULL, wstr, str, len);
}

int
_falwcstombs(char *str, wchar_t *wstr, int len)
{
    return _fallcwcstombs((XLCd) NULL, str, wstr, len);
}

wchar_t *
_falwcscpy(wchar_t *wstr1, wchar_t *wstr2)
{
    wchar_t *wstr_tmp = wstr1;

    while (*wstr1++ = *wstr2++)
	;

    return wstr_tmp;
}

wchar_t *
_falwcsncpy(wchar_t *wstr1, wchar_t *wstr2, int len)
{
    wchar_t *wstr_tmp = wstr1;

    while (len-- > 0)
	if (!(*wstr1++ = *wstr2++))
	    break;

    while (len-- > 0)
	*wstr1++ = (wchar_t) 0;

    return wstr_tmp;
}

int
_falwcslen(wchar_t *wstr)
{
    wchar_t *wstr_ptr = wstr;

    while (*wstr_ptr)
	wstr_ptr++;

    return wstr_ptr - wstr;
}

int
_falwcscmp(wchar_t *wstr1, wchar_t *wstr2)
{
    for ( ; *wstr1 && *wstr2; wstr1++, wstr2++)
	if (*wstr1 != *wstr2)
	    break;

    return *wstr1 - *wstr2;
}

int
_falwcsncmp(wchar_t *wstr1, wchar_t *wstr2, int len)
{
    for ( ; *wstr1 && *wstr2 && len > 0; wstr1++, wstr2++, len--)
	if (*wstr1 != *wstr2)
	    break;

    if (len <= 0)
	return 0;

    return *wstr1 - *wstr2;
}
