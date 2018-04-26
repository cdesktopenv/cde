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
/* $XConsortium: LocaleXlate.h /main/4 1995/10/26 12:29:54 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        LocaleXlate.h
 **
 **   Project:     DtLcx
 **
 **   Description: locale translation services
 **
 **   (c) Copyright 1993, 1994 Hewlett-Packard Company
 **   (c) Copyright 1993, 1994 International Business Machines Corp.
 **   (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **   (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/


#ifndef _DtLCX_XLATE_LOCALE_I
#define _DtLCX_XLATE_LOCALE_I

#include "XlationSvc.h"     /* FIX: move to <> */

#ifdef __cplusplus
extern "C" {
#endif

#if DOC
/*========================================================*/
$CONSTBEG$: _DtLCX_OPER_xxx
$1LINER$:  Constants for specifying operations
$SUMMARY$:
The _DtLCX_OPER_xxx are constants that produce strings
used in the translation specifications when specifying
the operation of a translation.

The operation string name must be identical both in the 
source code and in the translation table.
These constants should be used whenever referencing
operations as part of a translation.
/*================================================$SKIP$==*/
#endif
/* $DEF$, Operation constants */
#define DtLCX_OPER_STD        		"standard"
#define DtLCX_OPER_ICONV1     		"iconv1"
#define DtLCX_OPER_ICONV3     		"iconv3"
#define DtLCX_OPER_NLLANGINFO_CODESET	"nl_langinfo(CODESET)"
#define DtLCX_OPER_SETLOCALE  		"setlocale"
#define DtLCX_OPER_MULTIBYTE  		"multibyte"
#define DtLCX_OPER_CCDF       		"ccdf"
#define DtLCX_OPER_XLFD       		"xlfd"
#define DtLCX_OPER_MIME       		"mime"
#define DtLCX_OPER_INTERCHANGE_CODESET  "interchangeCodeset"
/*$END$*/

/* Functions */
#define _DtLcxCloseDb(io_db)    _DtXlateCloseDb(io_db)
int  _DtLcxOpenAllDbs(
       _DtXlateDb *    ret_db);

int _DtLcxXlateOpToStd(
       const _DtXlateDb   xlationDb,
       const char *       platform,
       const int          version,
       const char *       operation,
       const char *       opValue,
       char * *           ret_stdLocale,
       char * *           ret_stdLangTerr,
       char * *           ret_stdCodeset,
       char * *           ret_stdModifier);

int _DtLcxXlateStdToOp(
       const _DtXlateDb   xlationDb,
       const char *       platform,
       const int          version,
       const char *       operation,
       const char *       stdLocale,
       const char *       stdLangTerr,
       const char *       stdCodeset,
       const char *       stdModifier,
       char * *           ret_opValue);

#ifdef __cplusplus
}
#endif

#endif /*_DtLCX_XLATE_LOCALE_I*/
/********* do not put anything below this line ********/
