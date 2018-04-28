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
/* $XConsortium: HelpXlate.h /main/1 1996/08/22 09:16:20 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HelpXlate.h
 **
 **   Project:     DtXlate
 **
 **   Description: DtHelp-specific table-based translation services
 **
 **   (c) Copyright 1993, 1994 Hewlett-Packard Company
 **   (c) Copyright 1993, 1994 International Business Machines Corp.
 **   (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **   (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _DtHELP_XLATE_I
#define _DtHELP_XLATE_I

#include <LocaleXlate.h>
#include <XlationSvc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Functions */
void _DtHelpCeXlateOpToStdLocale(
	char       *operation,
	char       *opLocale,
	char       **ret_locale,
	char       **ret_lang,
	char       **ret_set);
void _DtHelpCeXlateStdToOpLocale(
	char       *operation,
	char       *stdLocale,
	char       *dflt_opLocale,
	char       **ret_opLocale);

#ifdef __cplusplus
}
#endif

#endif /*_DtHELP_XLATE_I*/
/********* do not put anything below this line ********/
