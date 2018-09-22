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
/* lcWrap.c 1.1 - Fujitsu source for CDEnext    95/11/06 20:32:42 	*/
/* $XConsortium: _fallcWrap.c /main/1 1996/04/08 15:19:54 cde-fuj $ */
/*

Copyright (c) 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/
/*
 * Copyright 1991 by the Open Software Foundation
 * Copyright 1993 by the TOSHIBA Corp.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Open Software Foundation and TOSHIBA
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Open Software
 * Foundation and TOSHIBA make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * OPEN SOFTWARE FOUNDATION AND TOSHIBA DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OPEN SOFTWARE FOUNDATIONN OR TOSHIBA BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *		 M. Collins		OSF
 *
 *		 Katsuhisa Yano		TOSHIBA Corp.
 */

#include <stdint.h>
#include "_fallibint.h"
#include "_fallcint.h"
#if defined(__linux__)
#include <locale.h>
#else
#include <X11/Xlocale.h>
#endif
#include <X11/Xos.h>
#include "_falutil.h"

extern void _fallcInitLoader(
    void
);

#ifdef XTHREADS
LockInfoPtr _Xi18n_lock;
#endif

char *
falSetLocaleModifiers(const char   *modifiers)
{
    XLCd lcd = _fallcCurrentLC();
    char *user_mods;

    if (!lcd)
	return (char *) NULL;
    if (!modifiers)
	return lcd->core->modifiers;
    user_mods = getenv("XMODIFIERS");
    modifiers = (*lcd->methods->map_modifiers) (lcd,
						user_mods, (char *)modifiers);
    if (modifiers) {
	if (lcd->core->modifiers)
	    Xfree(lcd->core->modifiers);
	lcd->core->modifiers = (char *)modifiers;
    }
    return (char *)modifiers;
}

Bool
falSupportsLocale(void)
{
    return _fallcCurrentLC() != (XLCd)NULL;
}

Bool _fallcValidModSyntax(char *mods, char **valid_mods)
{
    int i;
    char **ptr;

    while (mods && (*mods == '@')) {
	mods++;
	if (*mods == '@')
	    break;
	for (ptr = valid_mods; *ptr; ptr++) {
	    i = strlen(*ptr);
	    if (strncmp(mods, *ptr, i) || ((mods[i] != '=')))
		continue;
	    mods = strchr(mods+i+1, '@');
	    break;
	}
    }
    return !mods || !*mods;
}

static const char *im_valid[] = {"im", (char *)NULL};

/*ARGSUSED*/
char *
_fallcDefaultMapModifiers (XLCd lcd, char *user_mods, char *prog_mods)
{
    int i;
    char *mods;

    if (!_fallcValidModSyntax(prog_mods, (char **)im_valid))
	return (char *)NULL;
    if (!_fallcValidModSyntax(user_mods, (char **)im_valid))
	return (char *)NULL;
    i = strlen(prog_mods) + 1;
    if (user_mods)
	i += strlen(user_mods);
    mods = Xmalloc(i);
    if (mods) {
	strcpy(mods, prog_mods);
	if (user_mods)
	    strcat(mods, user_mods);
    }
    return mods;
}

typedef struct _XLCdListRec {
    struct _XLCdListRec *next;
    XLCd lcd;
    int ref_count;
} XLCdListRec, *XLCdList;

static XLCdList lcd_list = NULL;

typedef struct _XlcLoaderListRec {
    struct _XlcLoaderListRec *next;
    XLCdLoadProc proc;
} XlcLoaderListRec, *XlcLoaderList;

static XlcLoaderList loader_list = NULL;

void
_fallcRemoveLoader(XLCdLoadProc proc)
{
    XlcLoaderList loader, prev;

    if (loader_list == NULL)
	return;

    prev = loader = loader_list;
    if (loader->proc == proc) {
	loader_list = loader->next;
	Xfree(loader);
	return;
    }

    while (loader = loader->next) {
	if (loader->proc == proc) {
	    prev->next = loader->next;
	    Xfree(loader);
	    return;
	}
	prev = loader;
    }

    return;
}

Bool
_fallcAddLoader(XLCdLoadProc proc, XlcPosition position)
{
    XlcLoaderList loader, last;

    _fallcRemoveLoader(proc);		/* remove old loader, if exist */

    loader = (XlcLoaderList) Xmalloc(sizeof(XlcLoaderListRec));
    if (loader == NULL)
	return False;

    loader->proc = proc;

    if (loader_list == NULL)
	position = XlcHead;

    if (position == XlcHead) {
	loader->next = loader_list;
	loader_list = loader;
    } else {
	last = loader_list;
	while (last->next)
	    last = last->next;

	loader->next = NULL;
	last->next = loader;
    }

    return True;
}

XLCd
_falOpenLC(char *name)
{
    XLCd lcd;
    XlcLoaderList loader;
    XLCdList cur;
#if !defined(X_NOT_STDC_ENV) && !defined(X_LOCALE)
    char siname[256];
    char *_fallcMapOSLocaleName();
#endif

    if (name == NULL) {
	name = setlocale (LC_CTYPE, (char *)NULL);
#if !defined(X_NOT_STDC_ENV) && !defined(X_LOCALE)
	name = _fallcMapOSLocaleName (name, siname);
#endif
    }

    _XLockMutex(_Xi18n_lock);

    /*
     * search for needed lcd, if found return it
     */
    for (cur = lcd_list; cur; cur = cur->next) {
	if (!strcmp (cur->lcd->core->name, name)) {
	    lcd = cur->lcd;
	    cur->ref_count++;
	    _XUnlockMutex(_Xi18n_lock);
            return lcd;
	}
    }

    if (!loader_list)
	_fallcInitLoader();

    /*
     * not there, so try to get and add to list
     */
    for (loader = loader_list; loader; loader = loader->next) {
	lcd = (*loader->proc)(name);
	if (lcd) {
	    cur = (XLCdList) Xmalloc (sizeof(XLCdListRec));
	    if (cur) {
		cur->lcd = lcd;
		cur->ref_count = 1;
		cur->next = lcd_list;
		lcd_list = cur;
	    } else {
		(*lcd->methods->close)(lcd);
		lcd = (XLCd) NULL;
	    }
	    break;
	}
    }
}

void
_falCloseLC(XLCd lcd)
{
    XLCdList cur, *prev;

    for (prev = &lcd_list; cur = *prev; prev = &cur->next) {
	if (cur->lcd == lcd) {
	    if (--cur->ref_count < 1) {
		(*lcd->methods->close)(lcd);
		*prev = cur->next;
		Xfree(cur);
	    }
	    break;
	}
    }
}

/*
 * Get the XLCd for the current locale
 */

XLCd
_fallcCurrentLC(void)
{
    XLCd lcd;
    static XLCd last_lcd = NULL;

    lcd = _falOpenLC((char *) NULL);

    if (last_lcd)
	_falCloseLC(last_lcd);

    last_lcd = lcd;

    return lcd;
}

XrmMethods
_falrmInitParseInfo(XPointer *state)
{
    XLCd lcd = _falOpenLC((char *) NULL);

    if (lcd == (XLCd) NULL)
	return (XrmMethods) NULL;

    return (*lcd->methods->init_parse_info)(lcd, state);
}

int
falmbTextPropertyToTextList(
    Display *dpy,
    XTextProperty *text_prop,
    char ***list_ret,
    int *count_ret)
{
    XLCd lcd = _fallcCurrentLC();

    if (lcd == NULL)
	return XLocaleNotSupported;

    return (*lcd->methods->mb_text_prop_to_list)(lcd, dpy, text_prop, list_ret,
						 count_ret);
}

int
falwcTextPropertyToTextList(
    Display *dpy,
    XTextProperty *text_prop,
    wchar_t ***list_ret,
    int *count_ret)
{
    XLCd lcd = _fallcCurrentLC();

    if (lcd == NULL)
	return XLocaleNotSupported;

    return (*lcd->methods->wc_text_prop_to_list)(lcd, dpy, text_prop, list_ret,
						 count_ret);
}

int
falmbTextListToTextProperty(
    Display *dpy,
    char **list,
    int count,
    XICCEncodingStyle style,
    XTextProperty *text_prop)
{
    XLCd lcd = _fallcCurrentLC();

    if (lcd == NULL)
	return XLocaleNotSupported;

    return (*lcd->methods->mb_text_list_to_prop)(lcd, dpy, list, count, style,
						 text_prop);
}

int
falwcTextListToTextProperty(
    Display *dpy,
    wchar_t **list,
    int count,
    XICCEncodingStyle style,
    XTextProperty *text_prop)
{
    XLCd lcd = _fallcCurrentLC();

    if (lcd == NULL)
	return XLocaleNotSupported;

    return (*lcd->methods->wc_text_list_to_prop)(lcd, dpy, list, count, style,
						 text_prop);
}

void
falwcFreeStringList(wchar_t **list)
{
    XLCd lcd = _fallcCurrentLC();

    if (lcd == NULL)
	return;

    (*lcd->methods->wc_free_string_list)(lcd, list);
}

char *
falDefaultString(void)
{
    XLCd lcd = _fallcCurrentLC();

    if (lcd == NULL)
	return (char *) NULL;

    return (*lcd->methods->default_string)(lcd);
}

void
_fallcCopyFromArg(char *src, char *dst, int size)
{
    if (size == sizeof(long))
	*((long *) dst) = (long) src;
#ifdef LONG64
    else if (size == sizeof(int))
	*((int *) dst) = (int) (intptr_t) src;
#endif
    else if (size == sizeof(short))
	*((short *) dst) = (short) (intptr_t) src;
    else if (size == sizeof(char))
	*((char *) dst) = (char) (intptr_t) src;
    else if (size == sizeof(XPointer))
	*((XPointer *) dst) = (XPointer) src;
    else if (size > sizeof(XPointer))
	memcpy(dst, (char *) src, size);
    else
	memcpy(dst, (char *) &src, size);
}

void
_fallcCopyToArg(char *src, char **dst, int size)
{
    if (size == sizeof(long))
	*((long *) *dst) = *((long *) src);
    else if (size == sizeof(short))
	*((short *) *dst) = *((short *) src);
    else if (size == sizeof(char))
	*((char *) *dst) = *((char *) src);
    else if (size == sizeof(XPointer))
	*((XPointer *) *dst) = *((XPointer *) src);
    else
	memcpy(*dst, src, size);
}

void
_fallcCountVaList(va_list var, int *count_ret)
{
    int count;

    for (count = 0; va_arg(var, char *); count++)
	va_arg(var, XPointer);

    *count_ret = count;
}

void
_fallcVaToArgList(va_list var, int count, XlcArgList *args_ret)
{
    XlcArgList args;

    *args_ret = args = (XlcArgList) Xmalloc(sizeof(XlcArg) * count);
    if (args == (XlcArgList) NULL)
	return;

    for ( ; count-- > 0; args++) {
	args->name = va_arg(var, char *);
	args->value = va_arg(var, XPointer);
    }
}

void
_fallcCompileResourceList(XlcResourceList resources, int num_resources)
{
    for ( ; num_resources-- > 0; resources++)
	resources->xrm_name = falrmPermStringToQuark(resources->name);
}

char *
_fallcGetValues(
    XPointer base,
    XlcResourceList resources,
    int num_resources,
    XlcArgList args,
    int num_args,
    unsigned long mask)
{
    XlcResourceList res;
    XrmQuark xrm_name;
    int count;

    for ( ; num_args-- > 0; args++) {
	res = resources;
	count = num_resources;
	xrm_name = falrmPermStringToQuark(args->name);

	for ( ; count-- > 0; res++) {
	    if (xrm_name == res->xrm_name && (mask & res->mask)) {
		    _fallcCopyToArg(base + res->offset, &args->value, res->size);
		break;
	    }
	}

	if (count < 0)
	    return args->name;
    }

    return NULL;
}

char *
_fallcSetValues(
    XPointer base,
    XlcResourceList resources,
    int num_resources,
    XlcArgList args,
    int num_args,
    unsigned long mask)
{
    XlcResourceList res;
    XrmQuark xrm_name;
    int count;

    for ( ; num_args-- > 0; args++) {
	res = resources;
	count = num_resources;
	xrm_name = falrmPermStringToQuark(args->name);

	for ( ; count-- > 0; res++) {
	    if (xrm_name == res->xrm_name && (mask & res->mask)) {
		_fallcCopyFromArg(args->value, base + res->offset, res->size);
		break;
	    }
	}

	if (count < 0)
	    return args->name;
    }

    return NULL;
}
