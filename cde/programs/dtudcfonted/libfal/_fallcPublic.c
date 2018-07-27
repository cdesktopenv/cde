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
/* lcPublic.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:39 	*/
/* $XConsortium: _fallcPublic.c /main/1 1996/04/08 15:18:20 cde-fuj $ */
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
#include "_fallibint.h"
#include "_fallcPubI.h"

static char *default_string();

static XLCd create();
static Bool initialize();
static void destroy();
static char *get_values();

static XLCdPublicMethodsRec publicMethods = {
    {
	destroy,
	_fallcDefaultMapModifiers,
	NULL,
	NULL,
	_falrmDefaultInitParseInfo,
	_falmbTextPropertyToTextList,
	_falwcTextPropertyToTextList,
	_falmbTextListToTextProperty,
	_falwcTextListToTextProperty,
	_falwcFreeStringList,
	default_string,
	NULL,
	NULL
    },
    {
	NULL,
	create,
	initialize,
	destroy,
	get_values,
	_fallcGetLocaleDataBase
    }
};

XLCdMethods _fallcPublicMethods = (XLCdMethods) &publicMethods;

static char *
default_string(XLCd lcd)
{
    return XLC_PUBLIC(lcd, default_string);
}

static XLCd
create(char *name, XLCdMethods methods)
{
    XLCd lcd;
    XLCdPublicMethods new;

    lcd = (XLCd) Xmalloc(sizeof(XLCdRec));
    if (lcd == NULL)
        return (XLCd) NULL;
    bzero((char *) lcd, sizeof(XLCdRec));

    lcd->core = (XLCdCore) Xmalloc(sizeof(XLCdPublicRec));
    if (lcd->core == NULL){
	Xfree(lcd);
        return (XLCd) NULL;
	}
    bzero((char *) lcd->core, sizeof(XLCdPublicRec));

    new = (XLCdPublicMethods) Xmalloc(sizeof(XLCdPublicMethodsRec));
    if (new == NULL){
	Xfree(lcd);
        return (XLCd) NULL;
	}
    *new = *((XLCdPublicMethods) methods);
    lcd->methods = (XLCdMethods) new;

    return lcd;
}

static Bool
load_public(XLCd lcd)
{
    XLCdPublicPart *pub = XLC_PUBLIC_PART(lcd);
    char **values, *str;
    int num;

    if(_fallcCreateLocaleDataBase(lcd) == NULL)
	return False;

    _fallcGetResource(lcd, "XLC_XLOCALE", "mb_cur_max", &values, &num);
    if (num > 0) {
	pub->mb_cur_max = atoi(values[0]);
	if (pub->mb_cur_max < 1)
	    pub->mb_cur_max = 1;
    } else
	pub->mb_cur_max = 1;

    _fallcGetResource(lcd, "XLC_XLOCALE", "state_dependent", &values, &num);
    if (num > 0 && !_fallcCompareISOLatin1(values[0], "True"))
	pub->is_state_depend = True;
    else
	pub->is_state_depend = False;

    _fallcGetResource(lcd, "XLC_XLOCALE", "encoding_name", &values, &num);
    str = (num > 0) ? values[0] : "STRING";
    pub->encoding_name = (char*) Xmalloc(strlen(str) + 1);
    if (pub->encoding_name == NULL)
	return False;
    strcpy(pub->encoding_name, str);

    return True;
}

static Bool
initialize_core(XLCd lcd)
{
    XLCdMethods methods = lcd->methods;
    XLCdMethods core = &publicMethods.core;

    if (methods->close == NULL)
	methods->close = core->close;

    if (methods->map_modifiers == NULL)
	methods->map_modifiers = core->map_modifiers;

    if (methods->open_om == NULL)
	_falInitOM(lcd);

    if (methods->init_parse_info == NULL)
	methods->init_parse_info = core->init_parse_info;

    if (methods->mb_text_prop_to_list == NULL)
	methods->mb_text_prop_to_list = core->mb_text_prop_to_list;

    if (methods->wc_text_prop_to_list == NULL)
	methods->wc_text_prop_to_list = core->wc_text_prop_to_list;

    if (methods->mb_text_list_to_prop == NULL)
	methods->mb_text_list_to_prop = core->mb_text_list_to_prop;

    if (methods->wc_text_list_to_prop == NULL)
	methods->wc_text_list_to_prop = core->wc_text_list_to_prop;

    if (methods->wc_free_string_list == NULL)
	methods->wc_free_string_list = core->wc_free_string_list;

    if (methods->default_string == NULL)
	methods->default_string = core->default_string;

    return True;
}

extern Bool _fallcInitCTInfo();

static Bool
initialize(XLCd lcd)
{
    XLCdPublicMethodsPart *methods = XLC_PUBLIC_METHODS(lcd);
    XLCdPublicMethodsPart *pub_methods = &publicMethods.pub;
    XLCdPublicPart *pub = XLC_PUBLIC_PART(lcd);
    char lang[128], terr[128], code[128], *str;
    char *name;
#if !defined(X_NOT_STDC_ENV) && !defined(X_LOCALE)
    char siname[256];
    char *_fallcMapOSLocaleName();
#endif

    _fallcInitCTInfo();

    if (initialize_core(lcd) == False)
	return False;

    name = lcd->core->name;
#if !defined(X_NOT_STDC_ENV) && !defined(X_LOCALE)
    name = _fallcMapOSLocaleName(name, siname);
#endif

    if (_fallcResolveLocaleName(name, NULL, lang, terr, code) == 0)
	return False;

    str = (char*) Xmalloc(strlen(name) + strlen(lang) + strlen(terr) +
			  strlen(code) + 4);
    if (str == NULL)
	return False;

    strcpy(str, name);
    pub->siname = str;
    str += strlen(str) + 1;

    strcpy(str, lang);
    pub->language = str;
    str += strlen(str) + 1;

    strcpy(str, terr);
    pub->territory = str;
    str += strlen(str) + 1;

    strcpy(str, code);
    pub->codeset = str;

    if (pub->default_string == NULL)
	pub->default_string = "";

    if (methods->get_values == NULL)
	methods->get_values = pub_methods->get_values;

    if (methods->get_resource == NULL)
	methods->get_resource = pub_methods->get_resource;

    return load_public(lcd);
}

static void
destroy_core(XLCd lcd)
{
    if (lcd->core) {
	if (lcd->core->name)
            Xfree(lcd->core->name);
	Xfree(lcd->core);
    }

    if (lcd->methods)
	Xfree(lcd->methods);

    Xfree(lcd);
}

static void
destroy(XLCd lcd)
{
    XLCdPublicPart *pub = XLC_PUBLIC_PART(lcd);

    _fallcDestroyLocaleDataBase(lcd);

    if (pub->siname)
	Xfree(pub->siname);
    if (pub->encoding_name)
	Xfree(pub->encoding_name);

    destroy_core(lcd);
}

static XlcResource resources[] = {
    { XlcNCodeset, NULLQUARK, sizeof(char *),
      XOffsetOf(XLCdPublicRec, pub.codeset), XlcGetMask },
    { XlcNDefaultString, NULLQUARK, sizeof(char *),
      XOffsetOf(XLCdPublicRec, pub.default_string), XlcGetMask },
    { XlcNEncodingName, NULLQUARK, sizeof(char *),
      XOffsetOf(XLCdPublicRec, pub.encoding_name), XlcGetMask },
    { XlcNLanguage, NULLQUARK, sizeof(char *),
      XOffsetOf(XLCdPublicRec, pub.language), XlcGetMask },
    { XlcNMbCurMax, NULLQUARK, sizeof(int),
      XOffsetOf(XLCdPublicRec, pub.mb_cur_max), XlcGetMask },
    { XlcNStateDependentEncoding, NULLQUARK, sizeof(Bool),
      XOffsetOf(XLCdPublicRec, pub.is_state_depend), XlcGetMask },
    { XlcNTerritory, NULLQUARK, sizeof(char *),
      XOffsetOf(XLCdPublicRec, pub.territory), XlcGetMask }
};

static char *
get_values(XLCd lcd, XlcArgList args, int num_args)
{
    XLCdPublic pub = (XLCdPublic) lcd->core;

    if (resources[0].xrm_name == NULLQUARK)
	_fallcCompileResourceList(resources, XlcNumber(resources));

    return _fallcGetValues((XPointer) pub, resources, XlcNumber(resources), args,
			 num_args, XlcGetMask);
}
