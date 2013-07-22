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
** $XConsortium: TermPrimBuffer.h /main/1 1996/04/21 19:16:53 drk $
*/

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef   _Dt_TermPrimBuffer_h
#define   _Dt_TermPrimBuffer_h

typedef struct _TermLineSelectionRec *TermLineSelection;
typedef struct _TermBufferRec   *TermBuffer;
typedef struct _TermEnhInfoRec  *TermEnhInfo;
typedef struct _TermCharInfoRec *TermCharInfo;

#include  <Xm/Xm.h>
#include  "TermPrimOSDepI.h"
#include  "TermPrimRender.h"
#include  "TermPrimSelect.h"

#define	TermENH_SECURE			(1 << 0)
#define	TermENH_UNDERLINE		(1 << 1)
#define	TermENH_OVERSTRIKE		(1 << 2)

#define	TermIS_SECURE(flags)		((flags) & TermENH_SECURE)
#define	TermIS_UNDERLINE(flags)		((flags) & TermENH_UNDERLINE)
#define	TermIS_OVERSTRIKE(flags)	((flags) & TermENH_OVERSTRIKE)

typedef unsigned char   enhValue;
typedef enhValue       *enhValues;

typedef enum _countSpec
{
    countAll, countNew
} countSpec;

#ifdef	   __cplusplus
extern "C" {
#endif	/* __cplusplus */

typedef unsigned char termChar;

typedef struct _TermEnhInfoRec {
    Pixel fg;
    Pixel bg;
    TermFont font;
    unsigned long flags;
} TermEnhInfoRec;

typedef struct _TermCharInfoRec {
    union 
    {
	termChar *ptc;
	char     *pc;
	wchar_t  *pwc;
    } u;
    short       idx;        /* index into line buffer   */
    short       len;        /* bytes per character      */
    short       startCol;
    short       width;
    enhValue    enh;
    short       enhLen;
} TermCharInfoRec;

typedef TermBuffer  
(*BufferCreateProc)
(
    const Widget,
    const short,
    const short,
    const short,
    const short,
    const short
);

typedef void
(*BufferFreeProc)
(
    const TermBuffer
);

typedef void
(*BufferResizeProc)
(
    TermBuffer, short *, short *
);

typedef void
(*TermEnhProc)
(
    Widget, enhValues, TermEnhInfo
);

typedef void
(*TermEnhClear)
(
    TermBuffer,
    short,
    short,
    short
);

typedef void
(*TermEnhInsert)
(
    const TermBuffer,
    const short,
    const short, 
    short,
    const Boolean
);

typedef void
(*TermEnhDelete)
(
    TermBuffer,
    short,
    short,
    short
);

typedef int
(*TermEnhSet)
(
    TermBuffer,
    short,
    short,
    unsigned char,
    enhValue
);

typedef Boolean 
(*TermEnhGet)
(
    const TermBuffer,
    const short,
    const short,
          enhValue **,
          short *, 
    const countSpec
);

typedef Boolean
(*TermLineSetLen)
(
    TermBuffer,
    short,
    short
);

typedef Boolean
(*TermLineSetWidth)
(
    TermBuffer,
    short,
    short
);

typedef Boolean
(*TermLineClear)
(
    TermBuffer,
    short,
    short
);

#ifdef USE_SUN_WCWIDTH_PATCH
/* 
**  A small workaround for systems that don't have wcwidth...
*/
int
sun_wcwidth
(
    const wchar_t wc
);
#endif /*  USE_SUN_WCWIDTH_PATCH */

#ifdef USE_USL_WCWIDTH_PATCH
/* 
**  A small workaround for systems that don't have wcwidth...
*/
int
usl_wcwidth
(
    const wchar_t wc
);
#endif /*  USE_USL_WCWIDTH_PATCH */


/* 
** Truncate the specified line to the desired width, return 
**
** Inputs:
**      tb  - term buffer to act on
**      row - row to truncate
**
** Returns:
**      True/False to indicate success or failure
*/
extern Boolean
_DtTermPrimBufferClearLine
(
    const TermBuffer  tb,
    const short       row,
          short       newWidth
);

/* 
** Create a new term buffer of the desired dimensions.
** 
** Inputs:
**      rows - desired number of rows
**      cols - desired number of columns
**      sizeOfBuffer - bytes per 
**      sizeOfLine   - bytes per line record
**      sizeOfEnh    - bytes per enhancement chunk
**
** Returns:
**      pointer to new TermBuffer, else NULL
*/
extern TermBuffer
_DtTermPrimBufferCreateBuffer
(
    const Widget  w,
    const short   rows,
    const short   cols,
    const short   sizeOfBuffer,
    const short   sizeOfLine,
    const short   sizeOfEnh
);
    
/*
** replace all characters between startCol, and endCol with spaces,
*/
void
_DtTermPrimBufferErase
(
    TermBuffer tb,
    short      row,
    short      startCol,
    short      stopCol
);

/*
** replace all wide characters between startCol, and endCol with spaces,
*/
void
_DtTermPrimBufferEraseWc
(
    TermBuffer tb,
    short      row,
    short      startCol,
    short      stopCol
);


/* 
** Free all storage allocated to the specified buffer.
**
** Inputs:
**      tb - TermBuffer to free
*/
extern void
_DtTermPrimBufferFreeBuffer
(
    const TermBuffer  tb
);
    
/* 
** Change the dimensions of the specified buffer to the new size.
**
** Inputs:
**      *tb      - pointer to term buffer to resize
**      *newRows - number of rows desired
**      *newCols - number of cols desired
**      
** Returns:
**      potentially adjusted values for newRows and newCols
*/
extern void
_DtTermPrimBufferResizeBuffer
(
    TermBuffer *tb,
    short      *newRows,
    short      *newCols
);

/* 
** Create an enhancement chunk at the specified position.
**
** Inputs:
**      tb      - term buffer
**      row,col - coordinates of new enhancement chunk
**
** Returns:
**      True/False depending on success/failure
*/
extern Boolean
_DtTermPrimBufferCreateEnhancement
(
    TermBuffer  tb,
    short       row,
    short       col
);

/* 
** Free the enhancement chunk at the specified position.
**
** Inputs:
**      tb      - term buffer
**      row,col - coordinates of new enhancement chunk
**
** Returns:
**      True/False depending on success/failure
*/
extern Boolean
_DtTermPrimBufferFreeEnhancement
(
    const TermBuffer  tb,
    const short       row,
    const short       col
);   

/* 
** Get text from the buffer.
** 
** Inputs:
**      tb           - term buffer
**      row,col      - coordinates to get text from
**      length       - length of buffer
**      needWideChar - true if need wide characters (not multi-byte chars)
**
** Outputs:
**      buffer  - pointer to buffer containing text
**
** Returns:
**      the actual number of characters in the buffer
*/
extern short
_DtTermPrimBufferGetText
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
    const short           length,
    char                 *buffer,
    const Boolean         needWideChar
);

/*
** Return a pointer to the character at the specified position.
**
** Inputs:
**      tb      - term buffer
**      row,col - coordinates of desired character
**
** Returns:
**      pointer to desired character
*/
extern termChar *
_DtTermPrimBufferGetCharacterPointer
(
    const TermBuffer  tb,
    const short       row,
    const short       col
);

/* 
** Insert 'numChar' characters into the buffer at the specified position.
** 
** 
*/
extern short
_DtTermPrimBufferInsert
(
    const TermBuffer  tb,
    const short       row,
    const short       col,
    const termChar   *newChars,
          short       numChars,
          Boolean     insertFlag,   /* if TRUE, insert, else overwrite        */
          termChar  **returnChars,  /* pointer to overflow buffer             */
          short      *returnCount   /* count of characters in overflow buffer */
);

/* 
** Delete enough characters from the buffer to exceed width.
**
** If returnChars != NULL then the deleted characters are returned
** in a buffer pointed to by returnChars.  It is the responsibility
** of the calling function to XtFree the buffer containing returned
** characters when they are no longer needed.
*/
extern void
_DtTermPrimBufferDelete
(
    TermBuffer  tb,
    short      *row,
    short      *col,
    short      *count,
    termChar  **returnChars,  /* pointer to delete buffer        */
    short      *returnCount   /* count of bytes in delete buffer */
);

extern void
_DtTermPrimBufferDeleteWc
(
    TermBuffer  tb,
    short      *row,
    short      *col,
    short      *count,
    termChar  **returnChars,  /* pointer to delete buffer        */
    short      *returnCount   /* count of bytes in delete buffer */
);

extern Boolean
_DtTermPrimBufferSetLineWidth
(
    const TermBuffer      tb,
    const short           row,
          short           newWidth
);

extern short
_DtTermPrimBufferGetLineLength
(
    const TermBuffer      tb,
    const short           row
);

extern short
_DtTermPrimBufferGetLineWidth
(
    const TermBuffer      tb,
    const short           row
);

extern int
_DtTermPrimBufferSetEnhancement
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
    const unsigned char   id,
    const enhValue        value
);

extern Boolean
_DtTermPrimBufferGetEnhancement
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
          enhValue      **values,
          short          *count,
    const countSpec       countWhich
);

/*
** Return the number of rows allocated to the term buffer.
*/
extern short
_DtTermPrimBufferGetRows
(
    const TermBuffer      tb
);

/*
** Return the number of columns allocated to the term buffer.
*/
extern short
_DtTermPrimBufferGetCols
(
    const TermBuffer    tb
);

/*
** Set the value of the term buffer's nextBuffer and prevBuffer pointers.
*/
extern void
_DtTermPrimBufferSetLinks
(
    const TermBuffer    tb,
    const TermBuffer    prev,
    const TermBuffer    next
);

/*
** Set the value of the term buffer's selectInfo pointer.
*/
extern void
_DtTermPrimBufferSetSelectInfo
(
    const TermBuffer        tb,
    const TermSelectInfo    si
);

extern void
_DtTermPrimBufferMoveLockArea
(
    const TermBuffer	    tb,
    const short	            dest,
    const short	            src,
    const short	            length
);

typedef enum {
    insertFromTop,
    insertFromBottom
} InsertSource;

extern void
_DtTermPrimBufferInsertLine
(
    const TermBuffer	    tb,
    const short	            dest,
    const short	            length,
    const short	            src
);

extern void
_DtTermPrimBufferInsertLineFromTB
(
    const TermBuffer	    tb,
    const short	            dest,
    const short	            length,
    const InsertSource    insertSource
);

extern void
_DtTermPrimBufferDeleteLine
(
    const TermBuffer	    tb,
    const short	            source,
    const short	            length,
    const short	            lastUsedRow
);

extern void
_DtTermPrimBufferPadLine
(
    const TermBuffer  tb,
    const short       row,
    const short       newWidth
);

extern void
_DtTermPrimBufferPadLineWc
(
    const TermBuffer  tb,
    const short       row,
    const short       newWidth
);

extern Boolean
_DtTermPrimGetCharacterInfo
(
    TermBuffer      tb,
    short           row,
    short           col,
    TermCharInfo    charInfo
);

extern short
_DtTermPrimBufferGetNextTab
(
    const TermBuffer    tb,
          short         col
);

extern short
_DtTermPrimBufferGetPreviousTab
(
    const TermBuffer    tb,
          short         col
);

extern Boolean
_DtTermPrimBufferSetTab
(
    const TermBuffer	tb,
    const short	        col
);

extern Boolean
_DtTermPrimBufferClearTab
(
    const TermBuffer	tb,
    const short	        col
);

extern Boolean
_DtTermPrimBufferClearAllTabs
(
    const TermBuffer	tb
);

/*
** Return the value of the line wrap flag for the current line
*/
extern Boolean
_DtTermPrimBufferTestLineWrapFlag
(
    TermBuffer  tb,
    short       row
);

/*
** set the line wrap flag to the desired state
*/
void
_DtTermPrimBufferSetLineWrapFlag
(
    TermBuffer  tb,
    short       row,
    Boolean     state
);

/*
** Return the value of the in selection flag for the current line
*/
extern TermLineSelection
_DtTermPrimBufferGetInSelectionFlag
(
    TermBuffer  tb,
    short       row
);

/*
** set the in selection flag to the desired state
*/
void
_DtTermPrimBufferSetInSelectionFlag
(
    TermBuffer  tb,
    short       row,
    TermLineSelection     state
);

extern void
_DtTermPrimBufferSetSelectLines
(
    TermBuffer tb,
    short beginRow,
    short beginCol,
    short endRow,
    short endCol
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif /* _Dt_TermPrimBuffer_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
