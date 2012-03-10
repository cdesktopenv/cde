/* lcInit.c 1.4 - Fujitsu source for CDEnext    96/03/11 17:13:15 	*/ 
/* $XConsortium: _fallcInit.c /main/1 1996/04/08 15:17:36 cde-fuj $ */
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
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 *
 *   Modifier: Masayoshi Shimamura      FUJITSU LIMITED 
 *
 */

#include "_fallibint.h"
#include "_fallcint.h"

#define USE_GENERIC_LOADER
#define USE_DEFAULT_LOADER
/*** #define USE_EUC_LOADER ***/
/*** #define USE_SJIS_LOADER ***/
/*** #define USE_JIS_LOADER ***/
/*** #define USE_UTF_LOADER ***/

extern XLCd _fallcDefaultLoader(
#if NeedFunctionPrototypes
    char*
#endif
);

#ifdef DYNAMIC_LOAD
#ifdef sun
extern XLCd _falsunOsDynamicLoad(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif /* sun */

#ifdef AIXV3
extern XLCd _falaixOsDynamicLoad(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif /* AIXV3 */
#endif

#ifdef USE_GENERIC_LOADER
extern XLCd _fallcGenericLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_UTF_LOADER
extern XLCd _fallcUtfLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_EUC_LOADER
extern XLCd _fallcEucLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_SJIS_LOADER
extern XLCd _fallcSjisLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_JIS_LOADER
extern XLCd _XlcJisLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

#ifdef USE_DYNAMIC_LOADER
extern XLCd _fallcDynamicLoader(
#if NeedFunctionPrototypes
    char*
#endif
);
#endif

/*
 * The _fallcInitLoader function initializes the locale object loader list
 * with vendor specific manner.
 */

void
_fallcInitLoader()
{
#ifdef USE_GENERIC_LOADER
    _fallcAddLoader(_fallcGenericLoader, XlcHead);
#endif

#ifdef USE_DEFAULT_LOADER
    _fallcAddLoader(_fallcDefaultLoader, XlcHead);
#endif

#ifdef USE_EUC_LOADER
    _fallcAddLoader(_fallcEucLoader, XlcHead);
#endif

#ifdef USE_SJIS_LOADER
    _fallcAddLoader(_fallcSjisLoader, XlcHead);
#endif

#ifdef USE_JIS_LOADER
    _fallcAddLoader(_XlcJisLoader, XlcHead);
#endif

#ifdef USE_UTF_LOADER
    _fallcAddLoader(_fallcUtfLoader, XlcHead);
#endif

#ifdef DYNAMIC_LOAD
#ifdef sun
    _fallcAddLoader(_falsunOsDynamicLoad, XlcHead);
#endif /* sun */

#ifdef AIXV3
    _fallcAddLoader(_falaixOsDynamicLoad, XlcHead);
#endif /* AIXV3 */
#endif /* DYNAMIC_LOAD */

#ifdef USE_DYNAMIC_LOADER
    _fallcAddLoader(_fallcDynamicLoader, XlcHead);
#endif
}
