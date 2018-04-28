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
/* $XConsortium: CanvasError.h /main/3 1995/10/26 12:18:11 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        CanvasError.h
 **
 **   Project:     CacheCreeek (Rivers) Project.
 **
 **   
 **   Description: Private defines CE errors.
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
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef	_CECanvasError_h
#define	_CECanvasError_h

#ifdef __cplusplus
extern "C" {
#endif


/*
 * library generated errors in the system variable 'errno'
 */
#define	CEErrorReported			  0
#define	CEErrorExceedMaxSize		 -1
#define	CEErrorIllegalDatabaseFile	 -2
#define	CEErrorIllegalKeyword		 -3
#define	CEErrorIllegalPath		 -4
#define	CEErrorIllegalResource		 -5
#define	CEErrorLocIdNotFound		 -6
#define	CEErrorMalloc			 -7

#define	CEErrorMissingAbstractRes	-10
#define	CEErrorMissingFilenameRes	-11
#define	CEErrorMissingFileposRes	-12
#define	CEErrorMissingKeywordsRes	-13
#define	CEErrorMissingTitleRes		-14
#define	CEErrorMissingTopTopicRes	-15
#define	CEErrorMissingTopicList		-16
#define	CEErrorNoKeywordList		-17

/*
 * Formatting errors.
 */
#define	CEErrorMissingTopicCmd		-20
#define	CEErrorMissingTitleCmd		-21

#define	CEErrorFormattingCmd		-30
#define	CEErrorFormattingId		-31
#define	CEErrorFormattingLabel		-32
#define	CEErrorFormattingLink		-33
#define	CEErrorFormattingTitle		-34
#define	CEErrorFormattingValue		-35
#define	CEErrorFormattingOption		-36

#define	CEErrorHyperType		-40
#define	CEErrorHyperSpec		-41
#define	CEErrorHyperNotFound		-42
#define	CEErrorHyperTitle		-43

#define	CEErrorIllegalInfo		-45
#define	CEErrorReadEmpty		-46
#define CEErrorFileSeek			-47

#define	CEErrorParagraphValue		-50
#define	CEErrorParagraphOption		-51
#define	CEErrorTopicSyntax		-52
#define	CEErrorFontSpec			-53

#define	CEErrorAbbrevSyntax		-60
#define	CEErrorTitleSyntax		-61
#define	CEErrorFigureSyntax		-62
#define	CEErrorGraphicSyntax		-63
#define	CEErrorFontSyntax		-64
#define	CEErrorIdSyntax			-65
#define	CEErrorLabelSyntax		-66
#define	CEErrorLinkSyntax		-67
#define	CEErrorNewLineSyntax		-68
#define	CEErrorParagraphSyntax		-69
#define	CEErrorOctalSyntax		-70


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _CECanvasError_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
