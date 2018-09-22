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
/* lcPubWrap.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:39 	*/
/* $XConsortium: _fallcPubWrap.c /main/1 1996/04/08 15:18:09 cde-fuj $ */
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

char *
_falGetLCValues(XLCd lcd, ...)
{
    va_list var;
    XlcArgList args;
    char *ret;
    int num_args;
    XLCdPublicMethodsPart *methods = XLC_PUBLIC_METHODS(lcd);

    Va_start(var, lcd);
    _fallcCountVaList(var, &num_args);
    va_end(var);

    Va_start(var, lcd);
    _fallcVaToArgList(var, num_args, &args);
    va_end(var);

    if (args == (XlcArgList) NULL)
	return (char *) NULL;

    ret = (*methods->get_values)(lcd, args, num_args);

    Xfree(args);

    return ret;
}

void
_fallcDestroyLC(XLCd lcd)
{
    XLCdPublicMethods methods = (XLCdPublicMethods) lcd->methods;

    (*methods->pub.destroy)(lcd);
}

XLCd
_fallcCreateLC(char *name, XLCdMethods methods)
{
    XLCdPublicMethods pub_methods = (XLCdPublicMethods) methods;
    XLCd lcd;

    lcd = (*pub_methods->pub.create)(name, methods);
    if (lcd == NULL)
	return (XLCd) NULL;

    if (lcd->core->name == NULL) {
	lcd->core->name = (char*) Xmalloc(strlen(name) + 1);
	if (lcd->core->name == NULL) {
	        _fallcDestroyLC(lcd);
		return (XLCd) NULL;
	}

	strcpy(lcd->core->name, name);
    }

    if (lcd->methods == NULL)
	lcd->methods = methods;

    if ((*pub_methods->pub.initialize)(lcd) == False) {
	    _fallcDestroyLC(lcd);
	    return (XLCd) NULL;
    }

    return lcd;
}
