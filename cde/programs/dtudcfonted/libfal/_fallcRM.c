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
/* lcRM.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:40 	*/
/* $XConsortium: _fallcRM.c /main/1 1996/04/08 15:18:41 cde-fuj $ */
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
#include <stdio.h>

typedef struct _StateRec {
    XLCd lcd;
    XlcConv conv;
} StateRec, *State;

static void
mbinit(XPointer state)
{
    _fallcResetConverter(((State) state)->conv);
}

static char
mbchar(XPointer state, char *str, int *lenp)
{
    XlcConv conv = ((State) state)->conv;
    XlcCharSet charset;
    char *from, *to, buf[BUFSIZ];
    int from_left, to_left;
    XPointer args[1];

    from = str;
    *lenp = from_left = XLC_PUBLIC(((State) state)->lcd, mb_cur_max);
    to = buf;
    to_left = BUFSIZ;
    args[0] = (XPointer) &charset;

    _fallcConvert(conv, (XPointer *) &from, &from_left, (XPointer *) &to,
		&to_left, args, 1);

    *lenp -= from_left;

    /* XXX */
    return buf[0];
}

static void
mbfinish(XPointer state)
{
}

static char *
lcname(XPointer state)
{
    return ((State) state)->lcd->core->name;
}

static void
destroy(XPointer state)
{
    _fallcCloseConverter(((State) state)->conv);
    _falCloseLC(((State) state)->lcd);
    Xfree((char *) state);
}

static XrmMethodsRec rm_methods = {
    mbinit,
    mbchar,
    mbfinish,
    lcname,
    destroy
} ;

XrmMethods
_falrmDefaultInitParseInfo(XLCd lcd, XPointer *rm_state)
{
    State state;

    state = (State) Xmalloc(sizeof(StateRec));
    if (state == NULL)
	return (XrmMethods) NULL;

    state->lcd = lcd;
    state->conv = _fallcOpenConverter(lcd, XlcNMultiByte, lcd, XlcNChar);
    if (state->conv == NULL) {
	Xfree((char *) state);

	return (XrmMethods) NULL;
    }

    *rm_state = (XPointer) state;

    return &rm_methods;
}
