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
/* lcUtil.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:42 	*/
/* $XConsortium: _fallcUtil.c /main/1 1996/04/08 15:19:43 cde-fuj $ */
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

#include <stdio.h>
#include <ctype.h>
#include <X11/Xos.h>
#include "_fallibint.h"

#ifdef X_NOT_STDC_ENV
#ifndef toupper
#define toupper(c)      ((int)(c) - 'a' + 'A')
#endif
#endif

int
_fallcCompareISOLatin1(char *str1, char *str2)
{
    char ch1, ch2;

    for ( ; (ch1 = *str1) && (ch2 = *str2); str1++, str2++) {
        if (islower(ch1))
            ch1 = toupper(ch1);
        if (islower(ch2))
            ch2 = toupper(ch2);

        if (ch1 != ch2)
            break;
    }

    return *str1 - *str2;
}

int
_fallcNCompareISOLatin1(char *str1, char *str2, int len)
{
    char ch1, ch2;

    for ( ; (ch1 = *str1) && (ch2 = *str2) && len; str1++, str2++, len--) {
        if (islower(ch1))
            ch1 = toupper(ch1);
        if (islower(ch2))
            ch2 = toupper(ch2);

        if (ch1 != ch2)
            break;
    }

    if (len == 0)
        return 0;

    return *str1 - *str2;
}
