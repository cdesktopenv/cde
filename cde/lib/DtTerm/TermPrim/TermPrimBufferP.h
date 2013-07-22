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
** $TOG: TermPrimBufferP.h /main/2 1997/04/17 18:05:11 samborn $
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef   _Dt_TermPrimBufferP_h
#define   _Dt_TermPrimBufferP_h

#include  "TermPrimBuffer.h"
#include  "TermPrimSelect.h"

#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */

#define VALID_ROW(tb, r)    (((r) >= 0) && ((r) < (tb)->term_buffer.rows))
#define VALID_COL(tb, c)    (((c) >= 0) && ((c) < (tb)->term_buffer.cols))
#define VALID_LENGTH(tb, l) (((l) >= 0) && ((l) <= (tb)->term_buffer.cols))

/*
** Make it easier to access members of the Terminal Buffer
*/
#define LINES(tb)          ((tb)->term_buffer.lines)
#define LINE_OF_TBUF(tb, row)  (LINES(tb)[row])
#define ROWS(tb)           ((tb)->term_buffer.rows)
#define COLS(tb)           ((tb)->term_buffer.cols)
#define MAX_ROWS(tb)       ((tb)->term_buffer.maxRows)
#define MAX_COLS(tb)       ((tb)->term_buffer.maxCols)
#define TABS(tb)           ((tb)->term_buffer.tabs)
#define NUM_ENH_FIELDS(tb) ((tb)->term_buffer.numFields)
#define SIZE_OF_BUFFER(tb) ((tb)->term_buffer.sizeOfBuffer)
#define SIZE_OF_LINE(tb)   ((tb)->term_buffer.sizeOfLine)
#define SIZE_OF_ENH(tb)    ((tb)->term_buffer.sizeOfEnh)
#define VALUE_LIST(tb)     ((tb)->term_buffer.valueList)
#define WIDGET(tb)         ((tb)->term_buffer.widget)

/*
** Make it easy to access the term buffer methods
*/
#define BUFFER_CREATE(tb)  ((tb)->term_buffer.buffer_create_proc)
#define BUFFER_FREE(tb)    ((tb)->term_buffer.buffer_free_proc)
#define BUFFER_RESIZE(tb)  ((tb)->term_buffer.buffer_resize_proc)
#define ENH_PROC(tb)       ((tb)->term_buffer.enh_proc)
#define CLEAR_ENH(tb)      ((tb)->term_buffer.clear_enh)
#define INSERT_ENH(tb)     ((tb)->term_buffer.insert_enh)
#define DELETE_ENH(tb)     ((tb)->term_buffer.delete_enh)
#define SET_ENH(tb)        ((tb)->term_buffer.set_enh)
#define GET_ENH(tb)        ((tb)->term_buffer.get_enh)
#define SET_LINE_WIDTH(tb) ((tb)->term_buffer.set_line_width)
#define SET_LINE_LENGTH(tb)((tb)->term_buffer.set_line_len)
#define CLEAR_LINE(tb)     ((tb)->term_buffer.clear_line)
#define NEXT_BUFFER(tb)    ((tb)->term_buffer.nextBuffer)
#define PREV_BUFFER(tb)    ((tb)->term_buffer.prevBuffer)
#define BUFFER_SELECT_INFO(tb) ((tb)->term_buffer.selectInfo)
#define WIDTH(l)           ((l)->term_line.width)
#define LENGTH(l)          ((l)->term_line.length)
#define BUFFER(l)          ((l)->term_line.buffer)
#define WRAPPED(l)         ((l)->term_line.wrapped)
#define	IN_SELECTION(l)	   ((l)->term_line.inSelection)
#define	IS_IN_SELECTION(l,s,e) \
		   (!(((l)->term_line.inSelection.selectionStart > e) || \
		   ((l)->term_line.inSelection.selectionEnd < s)))
#define START_SELECTION(l) ((l)->term_line.inSelection.selectionStart)
#define END_SELECTION(l)   ((l)->term_line.inSelection.selectionEnd)
#define	MIN_SELECTION_COL  (0)
#define	MAX_SELECTION_COL  (32700)
#define	NON_SELECTION_COL  (-1)
#define BYTES_PER_CHAR(tb) ((tb)->term_buffer.bytesPerChar) 

/* 
** declare the private enhancement information
*/
typedef struct _TermEnhPart
{
    int foo;    /* placate the compiler */
} TermEnhPart;

typedef struct _TermEnhRec
{
    TermEnhPart term_enh;
} TermEnhRec, *TermEnh;

typedef struct _TermLineSelectionRec
{
    short	    selectionStart;
    short	    selectionEnd;
} TermLineSelectionRec;

/* 
** Lines have a column length, and a character length, these two numbers are
** not always equal, since, in some locales (i.e. Japanese) the character
** glyphs can be two columns wide.  We also want to know if this line wrapped
** to the following line.
*/
typedef struct _TermLinePart
{
    short           width;   /* single width columns */
    short           length;  /* in characters        */
    TermLineSelectionRec
		    inSelection;
    Boolean         wrapped; /* did this line wrap to the following line? */
    termChar       *buffer;
} TermLinePart;

typedef struct _TermLineRec
{
    TermLinePart term_line;
} TermLineRec, *TermLine;

/* 
** This structure defines the buffer instance part
*/
typedef struct _TermBufferPart
{
    short               rows;  /* height (in chars) of character buffer      */
    short               cols;  /* width  (in single width cols) of buffer    */
    short               maxRows;  /* maximum height (before any resize)      */
    short               maxCols;  /* maximum width  (before any resize)      */
    short               bytesPerChar; /* bytes per character */
    Boolean            *tabs;         /* array of flags showing tab positions*/
    unsigned char       numFields;    /* number of fields in an enhancement  */
    short               sizeOfBuffer; /* bytes per buffer record             */
    short               sizeOfLine;   /* bytes per enhancement record        */
    short               sizeOfEnh;    /* bytes per line record               */
    enhValues           valueList;    /* local storage for enhancement values*/
    TermLine           *lines;
    TermBuffer          nextBuffer;   /* next term buffer in list            */
    TermBuffer          prevBuffer;   /* previous term buffer in list        */
    TermSelectInfo      selectInfo;   /* current select info record          */
    BufferCreateProc    buffer_create_proc;
    BufferResizeProc    buffer_resize_proc;
    BufferFreeProc      buffer_free_proc;
    TermEnhProc         enh_proc;
    TermEnhClear        clear_enh;
    TermEnhInsert       insert_enh;
    TermEnhDelete       delete_enh;
    TermEnhSet          set_enh;
    TermEnhGet          get_enh;
    TermLineSetLen      set_line_len;
    TermLineSetWidth    set_line_width;
    TermLineClear       clear_line;
    Widget		widget;	      /* the widget that created this buffer */
} TermBufferPart;

typedef struct _TermBufferRec
{
    TermBufferPart term_buffer;
} TermBufferRec;

extern
short
_DtTermPrimBufferInsertWc
(
    const TermBuffer  tb,
    const short       row,
    const short       col,
          wchar_t    *newChars,
          short       numChars,
          Boolean     insertFlag,
          termChar  **returnChars,
          short      *returnLength
);

extern
Boolean
_DtTermPrimBufferClearLineWc
(
    const TermBuffer  tb,
    const short       row,
          short       newWidth
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimBufferP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
