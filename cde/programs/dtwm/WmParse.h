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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: WmParse.h /main/4 1995/11/01 11:46:40 rswiston $
 */
/******************************<+>*************************************
 **********************************************************************
 **
 **  File:        WmParse.h
 **
 **  Project:     HP/Motif Workspace Manager (dtwm)
 **
 **  Description:
 **  -----------
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 **********************************************************************
 **
 ** (c) Copyright 1987, 1988, 1989, 1990, 1991 HEWLETT-PACKARD COMPANY
 ** ALL RIGHTS RESERVED
 **
 **********************************************************************
 **********************************************************************
 **
 **
 **********************************************************************
 ******************************<+>*************************************/
#ifndef _Dt_WmParse_h
#define _Dt_WmParse_h
#include <stdio.h>
#include <string.h>

/*
 * Definitions
 */
typedef struct _DtWmpParseBuf
{

    FILE *		pFile;		/* FILE variable */
    unsigned char *	pchLine;	/* ptr to current line */
    int			cLineSize;	/* # of bytes in pchLine */
    unsigned char *	pchNext;	/* ptr to next char in pchLine */
    int			lineNumber;	/* # of current line being parsed */

} DtWmpParseBuf;

/********    Public Function Declarations    ********/

extern void _DtWmParseSkipWhitespace( 
                        DtWmpParseBuf *pWmPB) ;
extern void _DtWmParseSkipWhitespaceC( 
                        unsigned char **linePP) ;
extern unsigned char * _DtWmParseNextToken (
			DtWmpParseBuf *pWmPB) ;
extern unsigned char * _DtWmParseNextTokenC( 
			unsigned char **linePP, 
                        Boolean SmBehavior) ;
extern DtWmpParseBuf * _DtWmParseNewBuf( void ) ;
extern void _DtWmParseDestroyBuf( 
                        DtWmpParseBuf *pWmPB) ;
extern void _DtWmParseSetLine( 
                        DtWmpParseBuf *pWmPB,
                        unsigned char *pch) ;
extern void _DtWmParseSetFile( 
                        DtWmpParseBuf *pWmPB,
                        FILE *pFile) ;
extern unsigned char * _DtWmParseNextLine( 
                        DtWmpParseBuf *pWmPB) ;
extern unsigned char * _DtWmParseCurrentChar( 
                        DtWmpParseBuf *pWmPB) ;
extern unsigned char * _DtWmParseNextChar( 
                        DtWmpParseBuf *pWmPB) ;
extern int _DtWmParseLineNumber( 
                        DtWmpParseBuf *pWmPB) ;
extern void _DtWmParseToLower (
			char  *string);
extern unsigned int _DtWmParsePeekAhead(
			unsigned char *currentChar,
			unsigned int currentLev) ;
extern unsigned char * _DtWmParseBackUp ( 
			DtWmpParseBuf *pWmPB, 
			unsigned char *pchTok) ;
extern unsigned char * _DtWmParseFilenameExpand (
			unsigned char *pchFilename
			);
extern unsigned char * _DtWmParseMakeQuotedString(
			unsigned char *pchLine);
extern unsigned char * _DtWmParseExpandEnvironmentVariables (
			unsigned char *pch,
			unsigned char *pchBrk
			);
extern unsigned char * _DtWmParseNextTokenExpand (
			DtWmpParseBuf *pWmPB 
			);
#endif  /* _Dt_WmParse_h */
