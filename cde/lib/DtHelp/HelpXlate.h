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
