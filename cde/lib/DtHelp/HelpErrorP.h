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
/* $XConsortium: HelpErrorP.h /main/3 1995/10/26 12:24:36 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        DtErrnoP.h
 **
 **   Project:     CacheCreeek (Rivers) Project.
 **
 **   
 **   Description: Private defines Dt errors.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_DtErrnoP_h
#define	_DtErrnoP_h

#include "CanvasError.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * library generated errors in the system variable 'errno'
 */
#define	DtErrorReported		CEErrorReported
#define	DtErrorExceedMaxSize		CEErrorExceedMaxSize
#define	DtErrorIllegalDatabaseFile	CEErrorIllegalDatabaseFile
#define	DtErrorIllegalKeyword		CEErrorIllegalKeyword
#define	DtErrorIllegalPath		CEErrorIllegalPath
#define	DtErrorIllegalResource		CEErrorIllegalResource
#define	DtErrorLocIdNotFound		CEErrorLocIdNotFound
#define	DtErrorMalloc			CEErrorMalloc

#define	DtErrorMissingAbstractRes	CEErrorMissingAbstractRes
#define	DtErrorMissingFilenameRes	CEErrorMissingFilenameRes
#define	DtErrorMissingFileposRes	CEErrorMissingFileposRes
#define	DtErrorMissingKeywordsRes	CEErrorMissingKeywordsRes
#define	DtErrorMissingTitleRes		CEErrorMissingTitleRes
#define	DtErrorMissingTopTopicRes	CEErrorMissingTopTopicRes
#define	DtErrorMissingTopicList	CEErrorMissingTopicList
#define	DtErrorNoKeywordList		CEErrorNoKeywordList

/*
 * Formatting errors.
 */
#define	DtErrorMissingTopicCmd		CEErrorMissingTopicCmd
#define	DtErrorMissingTitleCmd		CEErrorMissingTitleCmd

#define	DtErrorFormattingCmd		CEErrorFormattingCmd
#define	DtErrorFormattingId		CEErrorFormattingId
#define	DtErrorFormattingLabel		CEErrorFormattingLabel
#define	DtErrorFormattingLink		CEErrorFormattingLink
#define	DtErrorFormattingTitle		CEErrorFormattingTitle
#define	DtErrorFormattingValue		CEErrorFormattingValue
#define	DtErrorFormattingOption	CEErrorFormattingOption

#define	DtErrorHyperType		CEErrorHyperType
#define	DtErrorHyperSpec		CEErrorHyperSpec
#define	DtErrorHyperNotFound		CEErrorHyperNotFound
#define	DtErrorHyperTitle		CEErrorHyperTitle

#define	DtErrorIllegalInfo		CEErrorIllegalInfo
#define	DtErrorReadEmpty		CEErrorReadEmpty
#define DtErrorFileSeek		CEErrorFileSeek

#define	DtErrorParagraphValue		CEErrorParagraphValue
#define	DtErrorParagraphOption		CEErrorParagraphOption
#define	DtErrorTopicSyntax		CEErrorTopicSyntax
#define	DtErrorFontSpec		CEErrorFontSpec

#define	DtErrorAbbrevSyntax		CEErrorAbbrevSyntax
#define	DtErrorTitleSyntax		CEErrorTitleSyntax
#define	DtErrorFigureSyntax		CEErrorFigureSyntax
#define	DtErrorGraphicSyntax		CEErrorGraphicSyntax
#define	DtErrorFontSyntax		CEErrorFontSyntax
#define	DtErrorIdSyntax		CEErrorIdSyntax
#define	DtErrorLabelSyntax		CEErrorLabelSyntax
#define	DtErrorLinkSyntax		CEErrorLinkSyntax
#define	DtErrorNewLineSyntax		CEErrorNewLineSyntax
#define	DtErrorParagraphSyntax		CEErrorParagraphSyntax
#define	DtErrorOctalSyntax		CEErrorOctalSyntax


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtErrnoP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
