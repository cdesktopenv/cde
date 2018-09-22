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
/* lcConv.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:34 	*/
/* $XConsortium: _fallcConv.c /main/1 1996/04/08 15:15:57 cde-fuj $ */
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

typedef XlcConv (*XlcConverter)();

typedef struct _fallcConverterListRec {
    XLCd from_lcd;
    char *from;
    XrmQuark from_type;
    XLCd to_lcd;
    char *to;
    XrmQuark to_type;
    XlcConverter converter;
    struct _fallcConverterListRec *next;
} XlcConverterListRec, *XlcConverterList;

static XlcConverterList conv_list = NULL;

static void
close_converter(XlcConv conv)
{
    (*conv->methods->close)(conv);
}

static XlcConv
get_converter(
    XLCd from_lcd,
    XrmQuark from_type,
    XLCd to_lcd,
    XrmQuark to_type)
{
    XlcConverterList list, prev = NULL;
    XlcConv conv;

    for (list = conv_list; list; list = list->next) {
	if (list->from_lcd == from_lcd && list->to_lcd == to_lcd
	    && list->from_type == from_type && list->to_type == to_type) {

	    if (prev && prev != conv_list) {	/* XXX */
		prev->next = list->next;
		list->next = conv_list;
		conv_list = list;
	    }

	    return (*list->converter)(from_lcd, list->from, to_lcd, list->to);
	}

	prev = list;
    }

    return (XlcConv) NULL;
}

Bool
_fallcSetConverter(
    XLCd from_lcd,
    char *from,
    XLCd to_lcd,
    char *to,
    XlcOpenConverterProc converter)
{
    XlcConverterList list;
    XrmQuark from_type, to_type;

    from_type = falrmStringToQuark(from);
    to_type = falrmStringToQuark(to);

    for (list = conv_list; list; list = list->next) {
	if (list->from_lcd == from_lcd && list->to_lcd == to_lcd
	    && list->from_type == from_type && list->to_type == to_type) {

	    list->converter = converter;
	    return True;
	}
    }

    list = (XlcConverterList) Xmalloc(sizeof(XlcConverterListRec));
    if (list == NULL)
	return False;

    list->from_lcd = from_lcd;
    list->from = from;
    list->from_type = from_type;
    list->to_lcd = to_lcd;
    list->to = to;
    list->to_type = to_type;
    list->converter = converter;
    list->next = conv_list;
    conv_list = list;

    return True;
}

typedef struct _ConvRec {
    XlcConv from_conv;
    XlcConv to_conv;
} ConvRec, *Conv;

static int
indirect_convert(
    XlcConv lc_conv,
    XPointer *from,
    int *from_left,
    XPointer *to,
    int *to_left,
    XPointer *args,
    int num_args)
{
    Conv conv = (Conv) lc_conv->state;
    XlcConv from_conv = conv->from_conv;
    XlcConv to_conv = conv->to_conv;
    XlcCharSet charset;
    char buf[BUFSIZ], *cs;
    XPointer tmp_args[1];
    int cs_left, ret, length, unconv_num = 0;

    if (from == NULL || *from == NULL) {
	if (from_conv->methods->reset)
	    (*from_conv->methods->reset)(from_conv);

	if (to_conv->methods->reset)
	    (*to_conv->methods->reset)(to_conv);

	return 0;
    }

    while (*from_left > 0) {
	cs = buf;
	cs_left = BUFSIZ;
	tmp_args[0] = (XPointer) &charset;

	ret = (*from_conv->methods->convert)(from_conv, from, from_left, &cs,
					     &cs_left, tmp_args, 1);
	if (ret < 0)
	    break;

	length = cs_left = cs - buf;
	cs = buf;

	tmp_args[0] = (XPointer) charset;

	ret = (*to_conv->methods->convert)(to_conv, &cs, &cs_left, to, to_left,
					   tmp_args, 1);
	if (ret < 0) {
	    unconv_num += length / charset->char_size;
	    continue;
	}

	if (*to_left < 1)
	    break;
    }

    return unconv_num;
}

static void
close_indirect_converter(XlcConv lc_conv)
{
    Conv conv = (Conv) lc_conv->state;

    if (conv) {
	if (conv->from_conv)
	    close_converter(conv->from_conv);
	if (conv->to_conv)
	    close_converter(conv->to_conv);

	Xfree((char *) conv);
    }

    Xfree((char *) lc_conv);
}

static void
reset_indirect_converter(XlcConv lc_conv)
{
    Conv conv = (Conv) lc_conv->state;

    if (conv) {
	if (conv->from_conv && conv->from_conv->methods->reset)
	    (*conv->from_conv->methods->reset)(conv->from_conv);
	if (conv->to_conv && conv->to_conv->methods->reset)
	    (*conv->to_conv->methods->reset)(conv->to_conv);
    }
}

static XlcConvMethodsRec conv_methods = {
    close_indirect_converter,
    indirect_convert,
    reset_indirect_converter
} ;

static XlcConv
open_indirect_converter(XLCd from_lcd, char *from, XLCd to_lcd, char *to)
{
    XlcConv lc_conv, from_conv, to_conv;
    Conv conv;
    XrmQuark from_type, to_type;
    static XrmQuark QChar, QCharSet, QCTCharSet = (XrmQuark) 0;

    if (QCTCharSet == (XrmQuark) 0) {
	QCTCharSet = falrmStringToQuark(XlcNCTCharSet);
	QCharSet = falrmStringToQuark(XlcNCharSet);
	QChar = falrmStringToQuark(XlcNChar);
    }

    from_type = falrmStringToQuark(from);
    to_type = falrmStringToQuark(to);

    if (from_type == QCharSet || from_type == QChar || to_type == QCharSet ||
	to_type == QChar)
	return (XlcConv) NULL;

    lc_conv = (XlcConv) Xmalloc(sizeof(XlcConvRec));
    if (lc_conv == NULL)
	return (XlcConv) NULL;

    lc_conv->methods = &conv_methods;

    lc_conv->state = (XPointer) Xmalloc(sizeof(ConvRec));
    if (lc_conv->state == NULL){
	close_indirect_converter(lc_conv);
	return (XlcConv) NULL;
    }

    conv = (Conv) lc_conv->state;

    from_conv = get_converter(from_lcd, from_type, from_lcd, QCTCharSet);
    if (from_conv == NULL)
	from_conv = get_converter(from_lcd, from_type, from_lcd, QCharSet);
    if (from_conv == NULL)
	from_conv = get_converter((XLCd)NULL, from_type, (XLCd)NULL, QCharSet);
    if (from_conv == NULL)
	from_conv = get_converter(from_lcd, from_type, from_lcd, QChar);
    if (from_conv == NULL){
	close_indirect_converter(lc_conv);
	return (XlcConv) NULL;
    }
    conv->from_conv = from_conv;

    to_conv = get_converter(to_lcd, QCTCharSet, to_lcd, to_type);
    if (to_conv == NULL)
	to_conv = get_converter(to_lcd, QCharSet, to_lcd, to_type);
    if (to_conv == NULL)
	to_conv = get_converter((XLCd) NULL, QCharSet, (XLCd) NULL, to_type);
    if (to_conv == NULL){
	close_indirect_converter(lc_conv);
	return (XlcConv) NULL;
    }
    conv->to_conv = to_conv;

    return lc_conv;
}

XlcConv
_fallcOpenConverter(XLCd from_lcd, char *from, XLCd to_lcd, char *to)
{
    XlcConv conv;
    XrmQuark from_type, to_type;

    from_type = falrmStringToQuark(from);
    to_type = falrmStringToQuark(to);

    if (conv = get_converter(from_lcd, from_type, to_lcd, to_type))
	return conv;

    return open_indirect_converter(from_lcd, from, to_lcd, to);
}

void
_fallcCloseConverter(XlcConv conv)
{
    close_converter(conv);
}

int
_fallcConvert(
    XlcConv conv,
    XPointer *from,
    int *from_left,
    XPointer *to,
    int *to_left,
    XPointer *args,
    int num_args)
{
    return (*conv->methods->convert)(conv, from, from_left, to, to_left, args,
				     num_args);
}

void
_fallcResetConverter(XlcConv conv)
{
    if (conv->methods->reset)
	(*conv->methods->reset)(conv);
}
