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
/* 
** $XConsortium: TermBufferP.h /main/1 1996/04/21 19:15:29 drk $
*/

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef   _Dt_TermBufferP_h
#define   _Dt_TermBufferP_h
#include  "TermBuffer.h"
#include  "TermPrimBufferP.h"

#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* 
** This is the enhancement structure, one per character.
*/
typedef struct _DtEnh
{
    /* ANSI C requires bit fields to be int, signed int or unsigned int type */

    /* 
    ** The video enhancements.
    */
    unsigned int    video         :6;

    /*  
    ** FIELD_UNPROTECT  0
    ** FIELD_PROTECT    1
    ** FIELD_TRANSMIT   2
    ** FIELD_END        3
    */
    unsigned int    field         :2;

    /* 
    ** index into color[0..15], on for foreground, on for background
    */
    unsigned int    fgColor       :4;
    unsigned int    bgColor       :4;

    /* 
    ** FONT_NORMAL   0
    ** FONT_LINEDRAW 1
    */
    unsigned int    font          :1;
} DtTermEnhPart, *DtEnh;

/* 
** This a line, the character buffer, and the enhancements.
*/
typedef struct _DtLinePart
{
    DtEnh      enh;
} DtLinePart;

typedef struct _DtTermLineRec
{
    TermLinePart    term_line;
    DtLinePart      dt_line;
} DtTermLineRec, *DtLine;

typedef struct _DtTermBufferPart
{
    DtTermEnhPart   enhState;
    Boolean         enhDirty;   /* true if the enhState is non default */
} DtTermBufferPart;

typedef struct _DtTermBufferRec
{
    TermBufferPart      term_buffer;
    DtTermBufferPart    dt_buffer;
} DtTermBufferRec, *DtTermBuffer;

/* 
** Define some useful DT counter parts for the TermPrimBufferP.h macros
*/
#define DT_LINE_OF_TBUF(tb, row) ((DtLine)(LINE_OF_TBUF(tb, row)))
#define DT_LINES(tb)             ((DtLine*)(LINES(tb)))
#define DT_ENH_STATE(tb)         (((DtTermBuffer)(tb))->dt_buffer.enhState)
#define DT_ENH_DIRTY(tb)         (((DtTermBuffer)(tb))->dt_buffer.enhDirty)
#define DT_ENH(line)             ((line)->dt_line.enh)
#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermBufferP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
