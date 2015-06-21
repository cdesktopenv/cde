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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermPrimBuffer.c /main/1 1996/04/21 19:16:49 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#define	USE_MEMCPY	/* use memcpy for line movement... */

#include <stdlib.h>
#include <wchar.h>
#include <Xm/Xm.h>
#include "TermHeader.h"       /* for MIN/MAX */
#include "TermPrim.h"
#include "TermPrimBuffer.h"
#include "TermPrimBufferP.h"
#include "TermPrimSelect.h"
#include "TermPrimDebug.h"

/*
** Allocate and initialize a new terminal buffer.
*/
TermBuffer
_DtTermPrimBufferCreateBuffer
(
    const Widget  w,
    const short   rows,
    const short   cols,
    const short   sizeOfBuffer,
    const short   sizeOfLine,
    const short   sizeOfEnh
)
{
    int             i;
    TermLine       *newTL;
    TermBuffer      newTB;
    Boolean        *tabs;
    int             sizeOfChar;
    
    /*
    ** malloc new a TermBuffer and an array of TermLine pointers
    */
    newTB = (TermBuffer) malloc(sizeOfBuffer);
    /* the following MIN ensures that on systems where malloc of 0 returns
     * NULL, we won't confuse a 0 size buffer with a malloc failure and an
     * invalid buffer...
     */
    newTL  = (TermLine *) malloc((unsigned) MAX(rows, 1) * sizeof(TermLine));

    if (!newTB || !newTL)
    {
        /*
        ** clean up and leave if either of the previous malloc's failed.
        */
        if (newTB)
        {
            free(newTB);
        }
        if (newTL)
        {
            free(newTL);
        }
        return((TermBuffer)NULL);
    }

    /*
    ** malloc a tabs array...
    */
    tabs = (Boolean *) malloc((unsigned) cols * sizeof(Boolean));
    if (!tabs)
    {
        free(newTB);
        free(newTL);
        /*
        ** clean up and leave if either of the previous malloc's failed.
        */
        return((TermBuffer)NULL);
    }
    /*
    ** initialize the tabs...
    */
    for (i = 0; i < cols; i++) {
	tabs[i] = 0 == i % 8;
    }

    /* 
    ** decide how many bytes to allocate per character...
    */
    BYTES_PER_CHAR(newTB) = MB_CUR_MAX == 1 ? 1 : sizeof(wchar_t);

    /* 
    ** setting debug flag m:1 forces us into wide char mode...
    */
    DebugF('m', 1, BYTES_PER_CHAR(newTB) = sizeof(wchar_t));

    /*
    ** now malloc the individual lines...
    */
    for (i = 0; i < rows; i++)
    {
        newTL[i] = (TermLine) malloc(sizeOfLine);
        if (newTL[i])
        {
            BUFFER(newTL[i]) = (termChar *) 
                                      malloc((unsigned) 
                                             cols * BYTES_PER_CHAR(newTB));

            if (BUFFER(newTL[i]) == NULL)
            {
                /*
                ** we couldn't malloc a line buffer
                */
                break;
            }
            LENGTH(newTL[i])  = 0;
            WIDTH(newTL[i])   = 0;
            WRAPPED(newTL[i]) = False;
	    START_SELECTION(newTL[i]) = NON_SELECTION_COL;
	    END_SELECTION(newTL[i]) = NON_SELECTION_COL;
        }
        else
        {
            /*
            ** we couldn't malloc a line
            */
            break;
        }
    }

    /*
    ** If we were unable to malloc a full set of lines (i != rows),
    ** then cleanup and leave.
    */
    if (i != rows)
    {
        int j;

        for (j = 0; j < i; j++)
        {
            free(BUFFER(newTL[j]));
            free(newTL[j]);
        }
        free(newTL);
        free(newTB);
        free(tabs);
        return((TermBuffer)NULL);
    }

    /*
    ** Initialize the new TermBuffer.
    */
    LINES(newTB)                = newTL;
    TABS(newTB)                 = tabs;
    ROWS(newTB)                 = rows;
    COLS(newTB)                 = cols;
    MAX_ROWS(newTB)             = rows;
    MAX_COLS(newTB)             = cols;
    SIZE_OF_BUFFER(newTB)       = sizeOfBuffer;
    SIZE_OF_LINE(newTB)         = sizeOfLine;
    SIZE_OF_ENH(newTB)          = sizeOfEnh;
    BUFFER_CREATE(newTB)        = _DtTermPrimBufferCreateBuffer;
    BUFFER_FREE(newTB)          = NULL;
    BUFFER_RESIZE(newTB)        = NULL;
    ENH_PROC(newTB)             = NULL;
    CLEAR_ENH(newTB)            = NULL;
    INSERT_ENH(newTB)           = NULL;
    DELETE_ENH(newTB)           = NULL;
    SET_ENH(newTB)              = NULL;
    GET_ENH(newTB)              = NULL;
    SET_LINE_WIDTH(newTB)       = NULL;
    CLEAR_LINE(newTB)           = NULL;
    NEXT_BUFFER(newTB)          = NULL;
    PREV_BUFFER(newTB)          = NULL;
    BUFFER_SELECT_INFO(newTB)   = NULL;
    WIDGET(newTB)		= w;
    return (newTB);
}

void
_DtTermPrimBufferFreeBuffer
(
    const TermBuffer tb
)
{
    int i;

    /* check for null buffer... */
    if (!tb) {
	return;
    }

    /*
    ** Free any buffer-specific info first...
    */
    if (BUFFER_FREE(tb))
    {
	(*BUFFER_FREE(tb))(tb);
    }
	
    /*
    ** free the old buffer...
    */
    for (i = 0; i < MAX_ROWS(tb); i++)
    {
        (void) free(BUFFER(LINES(tb)[i]));
	(void) free(LINES(tb)[i]);
    }
    (void) free(TABS(tb));
    (void) free(LINES(tb));
    (void) free(tb);
}

/*
** Resize the terminal buffer, and try to be smart about it.  If the buffer
** shrinks, then simply adjust the size, no reallocs necessary (that way if
** the user returns to the previous size, no data is lost).
**
** NOTE:
**    Given this implementation, the memory allocated to the buffer will 
**    never decrease it simply increases to accomodate the largest size
**    that has ever been requested.
*/
void
_DtTermPrimBufferResizeBuffer
(
    TermBuffer *oldTB,
    short      *newRows,
    short      *newCols
)
{
    int         i;
    short	thisTabWidth;
    short	tabWidth;
    TermLine   *newTL;
    Boolean    *tabs;
    
    /*
    ** enforce some minimum size constraints...
    */
    *newRows = MAX(*newRows, 1);
    *newCols = MAX(*newCols, 1);

    /*
    ** the number of cols is increasing, start small and adjust the tab
    ** stops first...
    */
    if (*newCols > MAX_COLS(*oldTB)) 
    {
	tabs = (Boolean *) malloc((unsigned) *newCols * sizeof(Boolean));
	if (tabs)
	{
	    /* copy over the tab stops... */
	    (void) memcpy(tabs, TABS(*oldTB), COLS(*oldTB) * sizeof(Boolean));
	    free(TABS(*oldTB));
	    TABS(*oldTB) = tabs;
	    
	    /*
	    ** we need to extend the tab stops...
	    **
	    ** when tabWidth == 0, it has not been initialized.  When
	    ** it is < 0, we have encountered unequal tab stops...
	    */
	    tabWidth = 0;
	    for (i = 0, thisTabWidth = 0; i < COLS(*oldTB); i++, thisTabWidth++)
	    {
		if (TABS(*oldTB)[i])
		{
		    if (tabWidth == 0)
		    {
			/* first tab stop, set tabWidth... */
			tabWidth = thisTabWidth;
		    } 
		    else if (tabWidth != thisTabWidth)
		    {
			/* tab stops differ, set tabWidth to -1... */
			tabWidth = -1;
		    }
		    /* reset for next tab stop... */
		    thisTabWidth = 0;
		}
	    }
	    
	    if (tabWidth > 0)
	    {
		/*
		** we have consistent tab stops.  Extend the buffer...
		*/
		for (i = COLS(*oldTB); i < *newCols; i++)
		{
		    TABS(*oldTB)[i] = (0 == (i % tabWidth));
		}
	    }
	    else
	    {
		/*
		** we don't have consistent tab stops, so clear the rest...
		*/
		(void) memset(&(TABS(*oldTB)[COLS(*oldTB)]), '\0',
			      (*newCols - COLS(*oldTB)) * sizeof(Boolean));
	    }
	}
        else 
	{
	    /*
	    ** the malloc failed, adjust newCols and newRows and leave...
	    */
	    *newCols     = MAX_COLS(*oldTB);
            *newRows     = MIN(*newRows, MAX_ROWS(*oldTB));
	    COLS(*oldTB) = *newCols;
	    ROWS(*oldTB) = *newRows;
	    return;
	}
    }
    else
    {
	/*
	** take care of any shrinkage...
	*/
	COLS(*oldTB) = *newCols;
    }

    /*
    ** Now give the resize helper a shot at resizing the buffer
    */
    if (BUFFER_RESIZE(*oldTB))
    {
	/* 
	** Call the resize helper function if it exists...
	*/
	(*BUFFER_RESIZE(*oldTB))(*oldTB, newRows, newCols);
    }
    else
    {
	/*
	** There is no helper function, do it ourselves...
	** NOTE:
	**     This might cause some duplication of code, but due
	**     to time constraints, it is the most expeditious route.
	*/
	/*
	** make any necessary width adjustments first...
	**
	** NOTE:
	**    We do not take any action if the new column width is less
	**    than the current column width.  It is the responsibility of
	**    the rendering code to make sure that two column characters
	**    are handled properly if the second column falls past the last
	**    column in the window.
	*/
	if (*newCols > MAX_COLS(*oldTB)) 
	{
	    termChar *newLineBuffer;
	    
	    /*
	    ** now extend the line buffers for all lines, (even lines that
	    ** are not being used at the moment (ROWS < MAX_ROWS))...
	    */
	    newTL = LINES(*oldTB);
	    for (i = 0; i < MAX_ROWS(*oldTB); i++)
	    {
		newLineBuffer = (termChar *) malloc((unsigned) 
						    *newCols * BYTES_PER_CHAR(*oldTB));
		
		if (newLineBuffer == NULL)
		{
		    /*
		    ** line buffer malloc failed, we can only increase the
		    ** width to the current maximum...
		    */
		    *newCols = MAX_COLS(*oldTB);
		    *newRows = MIN(*newRows, MAX_ROWS(*oldTB));
		    break;
		}
		memcpy(newLineBuffer, BUFFER(newTL[i]), LENGTH(newTL[i]));
		free(BUFFER(newTL[i]));
		BUFFER(newTL[i])  = newLineBuffer;
		WRAPPED(newTL[i]) = False;
	    }
	    MAX_COLS(*oldTB) = *newCols;
	}
	COLS(*oldTB) = *newCols;
	
	/*
	** now adjust the length of the buffer as necessary...
	*/
	if (*newRows > MAX_ROWS(*oldTB)) 
	{
	    /*
	    ** the number of rows is increasing
	    */
	    newTL = (TermLine *) malloc((unsigned) *newRows * 
					sizeof(TermLine));
	    if (newTL != NULL)
	    {
		/*
		** the malloc succeeded, copy the old information, and
		** then free it...
		*/
		memcpy(newTL, LINES(*oldTB), sizeof(TermLine) * 
		       ROWS(*oldTB));
		free(LINES(*oldTB));
		LINES(*oldTB) = newTL;
		
		/*
		** now initialize the new lines...
		*/
		for (i = ROWS(*oldTB); i < *newRows; i++)
		{
		    newTL[i] = (TermLine) malloc(SIZE_OF_LINE(*oldTB));
		    if (newTL[i])
		    {
			BUFFER(newTL[i]) = (termChar *) 
			    malloc((unsigned) COLS(*oldTB) * 
				   BYTES_PER_CHAR(*oldTB));
			if (BUFFER(newTL[i]) == NULL)
			{
			    /*
			    ** line buffer malloc failed...
			    */
			    *newRows = i;
			    break;
			}
			LENGTH(newTL[i])  = 0;
			WIDTH(newTL[i])   = 0;
			WRAPPED(newTL[i]) = False;
			START_SELECTION(newTL[i]) = NON_SELECTION_COL;
			END_SELECTION(newTL[i]) = NON_SELECTION_COL;
		    }
		    else
		    {
			/*
			** line malloc failed, lets limit the
			** number of rows
			*/
			*newRows = i;
			break;
		    }
		}
		/*
		** its conceivable that MAX_ROWS could actually decrease if
		** we are low on memory...
		*/
		MAX_ROWS(*oldTB) = MIN(MAX_ROWS(*oldTB), *newRows);
	    }
	    else
	    {
		/*
		** the row buffer malloc failed, revert back to MAX_ROWS
		*/
		*newRows = MAX_ROWS(*oldTB);
	    }
	}
	ROWS(*oldTB) = *newRows;
    }
}

#ifdef NOCODE

/*
** Create an enhancement block at the specified row and column.
**
** NOTE:  For the time being, we simply allocate an entire row's worth
**        of enhancement blocks if it doesn't exist already.  We may
**        get smarter later on.
*/
/*ARGSUSED*/
Boolean
_DtTermPrimBufferCreateEnhancement
(
    const TermBuffer  tb,
    const short       row,
    const short       col
)
{
    return(True);
}

/*
** Clear the enhancement block at the specified row and column.
*/
static
void
clearEnhancement
(
    const TermBuffer  tb,
    const short       row,
    const short       col
)
{

}
#endif /* NOCODE */

/* 
** Call the emulations specific function to insert the desired number of
** enhancements at the specified position...
**
** Assumptions:
**   - insertCount has been properly clipped to insure that we don't go
**     out of bounds
**
** Notes:
**   - results are undefined if this function is called when the specified
**     column is at the end of the line.
*/
static void
insertEnhancements
(
    const TermBuffer  tb, 
    const short       row,
    const short       col,
    const short       insertCount,
    const Boolean     insertFlag
)
{
    /* 
    ** call the insert function only when it exists and we're in 
    ** insert mode...
    */
    if (INSERT_ENH(tb))
    {
        (*INSERT_ENH(tb))(tb, row, col, insertCount, insertFlag);
    }
}

/*
** Free the enhancement block at the specified row and column.
**
** NOTE: We may get smarter later on.
*/
Boolean
_DtTermPrimBufferFreeEnhancement
(
    const TermBuffer  tb,
    const short       row,
    const short       col
)
{
    return(True);
}

#ifdef	BBA
#pragma	BBA_IGNORE
#endif	/*BBA*/
static void
RememberTermBuffer
(
    const TermBuffer  tb,
    TermLine **referenceLines,
    int	*referenceLineCount
)
{
    int i1;
    int i2;

    *referenceLineCount = ROWS(tb);
    *referenceLines = (TermLine *) XtMalloc(ROWS(tb) * sizeof(TermLine));

    /* copy over the lines...
     */
    for (i1 = 0; i1 < ROWS(tb); i1++) {
	(*referenceLines)[i1] = LINE_OF_TBUF(tb, i1);
    }

    /* verify that they are all unique...
     */

    /* do a brute force check... */
    for (i1 = 0; i1 < ROWS(tb) - 1; i1++) {
	for (i2 = i1 + 1; i2 < ROWS(tb); i2++) {
	    if ((*referenceLines)[i1] == (*referenceLines)[i2]) {
		(void) fprintf(stderr,
			">>RememberTermBuffer: dup ptrs lines %d and %d\n",
			i1, i2);
	    }
	}
    }
}

#ifdef	BBA
#pragma	BBA_IGNORE
#endif	/*BBA*/
static void
CheckTermBuffer
(
    const TermBuffer  tb, 
    TermLine *referenceLines,
    int	referenceLineCount
)
{
    int i1;
    int i2;
    int matchedLine;

    if (referenceLineCount != ROWS(tb)) {
	for (i1 = 0; i1 < ROWS(tb); i1++) {
	    matchedLine = -1;
	    for (i2 = 0; i2 < ROWS(tb); i2++) {
		if (referenceLines[i1] == LINE_OF_TBUF(tb, i2)) {
		    if (matchedLine == -1) {
			matchedLine = i2;
		    } else {
			/* duplicate pointer... */
			fprintf(stderr,
				">>CheckTermBuffer: duplicate pointer src=%d, dest=%d and %d\n",
				i1, matchedLine, i2);
		    }
		}
	    }
	    if (matchedLine == -1) {
		fprintf(stderr,
			">>CheckTermBuffer: dropped pointer src=%d\n", i1);
	    }
	}
    }
    
    if (referenceLines) {
      XtFree((char *) referenceLines);
    }
}

short
_DtTermPrimBufferGetText
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
    const short           width,
          char           *buffer,
    const Boolean         needWideChar
)
{
    short           retLen;
    short           colWidth;
    short           mbLen;
    char           *pChar;
    wchar_t        *pwc;
    TermCharInfoRec charInfoFirst;
    TermCharInfoRec charInfoLast;
    
    if (!VALID_ROW(tb, row) || !VALID_COL(tb, col))
    {
        return(0);
    }
    
    colWidth = MIN(width, WIDTH(LINE_OF_TBUF(tb, row)) - col);
    if (colWidth > 0)
    {
	_DtTermPrimGetCharacterInfo(tb, row, col, &charInfoFirst);
	_DtTermPrimGetCharacterInfo(tb, row, col + colWidth - 1, &charInfoLast);
	
	if ((BYTES_PER_CHAR(tb) > 1) && !needWideChar)
	{
	    /*
	    ** our caller wants multi-byte characters...
	    */
	    retLen = 0;
	    pChar  = buffer;
	    
	    for (pwc = charInfoFirst.u.pwc; pwc <= charInfoLast.u.pwc; pwc++)
	    {
		mbLen = wctomb(pChar, *pwc);
		if (mbLen > 0)
		{
		    pChar  += mbLen;
		    retLen += mbLen;
		}
		else
		{
		    fprintf(stderr, "_DtTermPrimBufferGetText: invalid wide char.\n");
		}
	    }
	}
	else
	{
	    retLen = charInfoLast.idx - charInfoFirst.idx + 1;
	    memcpy(buffer, charInfoFirst.u.pc, retLen);
	}
    }
    else
    {
	/*
	** colWidth == 0, there is nothing to return
	*/
	retLen = 0;
    }
    
    return(retLen);
}

short
_DtTermPrimBufferGetRows
(
    const TermBuffer  tb
)
{
    return(ROWS(tb));
}

short
_DtTermPrimBufferGetCols
(
    const TermBuffer  tb
)
{
    return(COLS(tb));
}

void
_DtTermPrimBufferSetLinks
(
    const TermBuffer  tb,
    const TermBuffer  prev,
    const TermBuffer  next
)
{
    PREV_BUFFER(tb) = prev;
    NEXT_BUFFER(tb) = next;
}    

void
_DtTermPrimBufferSetSelectInfo
(
    const TermBuffer        tb,
    const TermSelectInfo    si
)
{
    BUFFER_SELECT_INFO(tb) = si;
}

/*
** Set the desired enhancement; simply call the emulator specific
** function.
*/
int
_DtTermPrimBufferSetEnhancement
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
    const unsigned char   id,
    const enhValue        value
)
{
    if (SET_ENH(tb))
    {
        return((*SET_ENH(tb))(tb, row, col, id, value));
    }
    else
    {
        return(0);
    }
}

/*
** Get the enhancements for the desired row and column by calling the
** emulator specific routine.
*/
Boolean
_DtTermPrimBufferGetEnhancement
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
          enhValue      **values,
          short          *count,
    const countSpec       countWhich
)
{
    if (GET_ENH(tb))
    {
        return((*GET_ENH(tb))(tb, row, col, values, count, countWhich));
    }
    else
    {
        *count = WIDTH(LINE_OF_TBUF(tb, row));
        return(True);
    }
}

/*
** Return a pointer to the character at the specified row,col.
*/
termChar *
_DtTermPrimBufferGetCharacterPointer
(
    const TermBuffer  tb,
    const short       row,
    const short       col
)
{
    TermCharInfoRec    charInfo;
    
    if (VALID_ROW(tb, row) && VALID_COL(tb, col))
    {
        _DtTermPrimGetCharacterInfo(tb, row, col, &charInfo);
        return((termChar *)charInfo.u.pc);
    }
    else
    {
        return(NULL);
    }
}

/* 
** Get the character info for the specified character.  If the
** specified column exceeds the width of the line, then make up
** some reasonable information, allowing the calling routine to
** continue in a reasonable fashion.
**
** XXX: this doesn't get the emulator specific information yet (like
**      enhancements
*/
Boolean
_DtTermPrimGetCharacterInfo
(
    TermBuffer      tb,
    short           row,
    short           col,
    TermCharInfo    charInfo
)
{
    wchar_t    *pwc;
    short       charWidth;  
    TermLine    line;
    short       totalWidth;

    if (!VALID_ROW(tb, row))
    {
        return(False);
    }    

    line = LINE_OF_TBUF(tb, row);

    /*
    ** handle the easy cases first...
    */
    if (col >= WIDTH(line))
    {
        /* 
        ** make up some reasonable values when col equals or 
	** exceeds the width of the line
        */
        charInfo->u.ptc    = NULL;
        charInfo->idx      = LENGTH(line);
        charInfo->len      = BYTES_PER_CHAR(tb);
        charInfo->startCol = col;
        charInfo->width    = 1;
        charInfo->enh      = 0;
        charInfo->enhLen   = 0;
    }
    else if (BYTES_PER_CHAR(tb) == 1)
    {
        /* 
        ** this is easy in a 1 byte locale...
        */
        charInfo->u.ptc    = BUFFER(line) + col;
        charInfo->idx      = col;
        charInfo->len      = BYTES_PER_CHAR(tb);
        charInfo->startCol = col;
        charInfo->width    = 1;
        charInfo->enh      = 0;
        charInfo->enhLen   = 0;
    }
    else if (col == 0)
    {
        charInfo->u.pwc    = (wchar_t *)BUFFER(line);
        charInfo->idx      = 0;
        charInfo->len      = BYTES_PER_CHAR(tb);
        charInfo->startCol = 0;
        charInfo->width    = MAX(1, wcwidth(*((wchar_t *)BUFFER(line))));
        charInfo->enh      = 0;
        charInfo->enhLen   = 0;
    }
    else
    {
        /* 
        ** not so easy when we're dealing with wchars...
        */
        if (col <= WIDTH(line) / 2)
        {
            /*
            ** work from left to right
            */
            totalWidth = 0;
            for (pwc = (wchar_t *)BUFFER(line);
                 pwc < (wchar_t *)BUFFER(line) + LENGTH(line);
                 pwc++)
            {
                charWidth = MAX(1, wcwidth(*pwc));
                totalWidth += charWidth;
                if (col < totalWidth)
                {
                    /* 
                    ** we've found the character, now fill in the info...
                    */
                    charInfo->u.pwc    = pwc;
                    charInfo->idx      = pwc - (wchar_t *)BUFFER(line);
                    charInfo->startCol = totalWidth - charWidth;
                    charInfo->width    = charWidth;
                    break;
                }
            }
        }
        else
        {
            /*
            ** work from right to left
            */
            totalWidth = WIDTH(line);
            for (pwc = (wchar_t *)BUFFER(line) + (LENGTH(line) - 1);
                 pwc >= (wchar_t *)BUFFER(line);
                 pwc--)
            {
                charWidth = MAX(1, wcwidth(*pwc));
                totalWidth -= charWidth;
                if (totalWidth <= col)
                {
                    /* 
                    ** we've found the character, now fill in the info...
                    */
                    charInfo->u.pwc = pwc;
                    charInfo->idx   = pwc - (wchar_t *)BUFFER(line);
                    if (totalWidth == col)
                    {
                        charInfo->startCol = col;
                    }
                    else
                    {
                        charInfo->startCol = col - 1;
                    }
                    charInfo->width = charWidth;
                    break;
                }
            }
        }
        charInfo->len    = BYTES_PER_CHAR(tb);
        charInfo->enh    = 0;
        charInfo->enhLen = 0;
    }
    return(True);
}

/*
** Insert as many characters as possible at the specified row,col
** return a count of the number of characters bumped off the end of the line
** in 'returnCount' and a pointer to a buffer containing those characters
** 'returnChars'.
** 
** The the new width of the line is returned as the value of the function.
*/
short
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
)   
{
           short        insertCount;
           short        newWidth;
           short        insertOverflow; /* # of newChars that would overflow */
           TermLine     line;
           termChar    *start;
           short        copyCount;   /* number of bytes to copy */
    
    if (!VALID_ROW(tb, row) || !VALID_COL(tb, col))
    {
        *returnCount = 0;
        return(0);
    }
    
    line = LINE_OF_TBUF(tb, row);

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /* if this line is part of the selection, disown the selection... */
    if (IS_IN_SELECTION(line, col, MAX_SELECTION_COL)) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    if (BYTES_PER_CHAR(tb) > 1)
    {
	/*
	** we do not handle multibyte characters here...
	*/
	_DtTermPrimBufferInsertWc(tb, row, col, (wchar_t *)newChars,
				  numChars, insertFlag,
				  returnChars, returnCount);
	return 0;
    }

    if (WIDTH(line) < col)
    {
        /*
        ** We're adding characters past the current end of line,
        ** pad it out.
        */
        _DtTermPrimBufferPadLine(tb, row, col);
    }
    
    insertCount  = MIN(numChars, COLS(tb) - col);
    copyCount    = insertCount * BYTES_PER_CHAR(tb);
    start        = BUFFER(line) + col;
    
    /*
    ** It doesn't matter if we're overwriting, or inserting at the end 
    ** of the line, the result is the same...
    */
    if (col == WIDTH(line))
    {
        insertFlag = False;
    }

    if (insertFlag == False)
    {
        /* 
        ** We're overwriting:
        **      - determine the new line width
        **      - put any extra new characters into the overflow buffer
        ** 
        ** The following will be done later:
        **      - copy the newChars into the line buffer 
        **      - adjust line width (_DtTermPrimBufferSetLineWidth won't
        **        let the line get shorter)...
        */
        newWidth     = MAX(WIDTH(line), col + insertCount);
        *returnCount = numChars - insertCount;
        if (*returnCount > 0)
        {
            memcpy(*returnChars, newChars + insertCount,
                   *returnCount * BYTES_PER_CHAR(tb));
        }
    }
    else
    {
        /* 
        ** we're inserting text in the middle of the line...
        */
        *returnCount = MAX(0, (WIDTH(line) + numChars) - COLS(tb));
        if (*returnCount > 0)
        {
            /* 
            ** we'll have some overflow, figure out how many (if any)
            ** of the new characters will overflow and put them into
            ** the overflow buffer...
            */
            insertOverflow = numChars - insertCount;
            memcpy(*returnChars, newChars + insertCount,
                   insertOverflow * BYTES_PER_CHAR(tb));

            /* 
            ** copy the displaced characters from the line to the 
            ** overflow buffer as well...
            */
            memcpy(*returnChars + insertOverflow, BUFFER(line) + 
                   (WIDTH(line) - MAX(0, *returnCount - insertOverflow)),
                   MAX(0, *returnCount - insertOverflow) 
                   * BYTES_PER_CHAR(tb));
        }
        /* 
        ** Any overflow has been taken care of, now it's time to make
        ** room for the new characters...
        */
        memmove(start + insertCount, start,
                MAX(0, WIDTH(line) - col - *returnCount));

        /* 
        ** Compute the new line width...
        */
        newWidth = WIDTH(line) + insertCount;
    }

    /* 
    ** Everything's ready:
    **     - put the characters into the line
    **     - adjust the line width (_DtTermPrimBufferSetLineWidth won't
    **       let the line get shorter)...
    **     - update the enhancements
    */
    memcpy(start, newChars, copyCount);
    _DtTermPrimBufferSetLineWidth(tb, row, newWidth);
    insertEnhancements(tb, row, col, insertCount, insertFlag);
    return(WIDTH(line));
}

/* 
** Delete enough characters from the buffer to exceed width.
**
** If returnChars != NULL then the deleted characters are returned
** in a buffer pointed to by returnChars.  It is the responsibility
** of the calling function to XtFree the buffer containing returned
** characters when they are no longer needed.
*/
void
_DtTermPrimBufferDelete
(
    TermBuffer  tb,
    short      *row,
    short      *col,
    short      *width,
    termChar  **returnChars,  /* pointer to delete buffer        */
    short      *returnCount   /* count of bytes in delete buffer */
)
{
    int      copyCount;
    TermLine line;
    short    localRow;
    short    localCol;    
    TermCharInfoRec charInfo;
    

    if (!VALID_ROW(tb, *row) || !VALID_COL(tb, *col))
    {
        if (returnChars)
	{
	    *returnChars = NULL;
	    *returnCount = 0;
	}
        return;	
    }

    line     = LINE_OF_TBUF(tb, *row);

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /* if this line is part of the selection, disown the selection... */
    if (IS_IN_SELECTION(line, *col, MAX_SELECTION_COL)) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    if (BYTES_PER_CHAR(tb) > 1)
    {
	/*
	** we do not handle multibyte characters here...
	*/
	_DtTermPrimBufferDeleteWc(tb, row, col, width, returnChars,
				  returnCount);
	return;
    }
    localRow = *row;
    localCol = *col;
    *width   = MAX(0, MIN(WIDTH(line) - localCol, *width));

    /* 
    ** there are 4 cases of deleting a character from a line:
    **     Case 1:
    **       the cursor is past the end of the line (col >= WIDTH(line))
    **
    **     Case 2:
    **       the cursor is in the middle of the line (copyCount > 0)
    **          - move the remaining characters to the left
    **          - deleteEnhancement
    **
    **     Case 3:
    **       the cursor is at the end of the line (copyCount == 0 and
    **       col == WIDTH(line) - 1)
    **          - deleteEnhancement
    **          - adjust WIDTH and LENGTH
    */
    if (localCol >= WIDTH(line) || *width == 0)
    {
        /* 
        ** Handle Case 1...
        */
        if (returnChars)
	{
	    *returnChars = NULL;
	    *returnCount = 0;
	}
        return;
    }

    _DtTermPrimGetCharacterInfo(tb, localRow, localCol, &charInfo);
    
    /* 
    ** Save the current characters before we overwrite them...
    */
    if (returnChars != NULL)
    {
	*returnCount = (*width * BYTES_PER_CHAR(tb));
	*returnChars = (termChar *)XtMalloc(*returnCount);
	memcpy(*returnChars, charInfo.u.pc, *returnCount);
    }

    /* 
    ** Cases 2 and 3 require that we delete the enhancement...
    */
    if (DELETE_ENH(tb))
    {
        (*DELETE_ENH(tb))(tb, localRow, localCol, *width);
    }

    copyCount = MAX(0, WIDTH(line) - *width - localCol);
    if (copyCount > 0)
    {
        /* 
        ** handle case 2
        */
        memmove(charInfo.u.pc, charInfo.u.pc + *width,
                copyCount * BYTES_PER_CHAR(tb));
    }

    /* 
    ** Case 2 and 3 require that we adjust the line width
    */
    WIDTH(line)  -= *width;
    LENGTH(line) -= *width;
}

/*
** replace all characters between startCol, and stopCol with spaces,
** if startCol > stopCol, then simply return
*/
void
_DtTermPrimBufferErase
(
    TermBuffer tb,
    short      row,
    short      startCol,
    short      stopCol
)
{
    TermCharInfoRec       charInfo;
    char                 *pchar;

    /*
    ** Make sure we have a valid row, and if we have a valid
    ** row, make sure that startCol is <= stopCol
    */
    if (!(VALID_ROW(tb, row)) || (startCol > stopCol))
    {
        return;
    }

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /* if this line is part of the selection, disown the selection... */
    if (IS_IN_SELECTION(LINE_OF_TBUF(tb, row), startCol, stopCol)) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    if (BYTES_PER_CHAR(tb) > 1)
    {
	_DtTermPrimBufferEraseWc(tb, row, startCol, stopCol);
    }
    else
    {
	_DtTermPrimGetCharacterInfo(tb, row, startCol, &charInfo);
	pchar = charInfo.u.pc;
	
	_DtTermPrimGetCharacterInfo(tb, row, stopCol, &charInfo);
	
	/*
	** replace the characters with spaces...
	*/
	while(pchar <= charInfo.u.pc)
	{
	    *pchar = ' ';
	    pchar++;
	}
    }
}


/*
** Adjust the line width, (the line can never be shortened)
*/
Boolean
_DtTermPrimBufferSetLineWidth
(
    const TermBuffer    tb,
    const short         row,
          short         newWidth
)
{
    TermLine    line;
    
    /*
    ** Make sure we have a valid row.
    */
    if (!VALID_ROW(tb, row))
    {
        return(False);
    }

    line = LINE_OF_TBUF(tb, row);

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /* if this line is part of the selection, disown the selection... */
    if (IS_IN_SELECTION(line, newWidth, MAX_SELECTION_COL)) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    /*
    ** Clip the new width to the buffer width.
    */
    newWidth = MIN(newWidth, COLS(tb));

    /*
    ** Make sure we actually have something to do.
    */
    if (WIDTH(line) >= newWidth)
    {
        return(True);
    }

    WIDTH(line) = newWidth;
    if (BYTES_PER_CHAR(tb) == 1)
    {
	/*
	** in single byte locales, we go ahead and set the line length too,
	** it is the responsibility of wide-character routines to set the
	** line length themselves...
	*/ 
	LENGTH(line) = newWidth; /* works in single byte locales */
    }
    
    return(True);
}

/*
** the line length is the lesser of the actual length, or the length
** necessary to get to the buffer width
** (in the case that the buffer shrank after the line was full)
*/
short
_DtTermPrimBufferGetLineLength
(
    const TermBuffer    tb,
    const short         row
)
{
    short           retLen;
    short           width;
    TermCharInfoRec charInfo;
    
    if (VALID_ROW(tb, row)) 
    {
	width = MIN(WIDTH(LINE_OF_TBUF(tb, row)), COLS(tb));
	if (width > 0)
	{
	    
	    (void)_DtTermPrimGetCharacterInfo(tb, row, width - 1, &charInfo);
    
	    /*
	    ** a line can not end on column 1 of 2.  Note that we must
	    ** add 1 to the column index to make it the line length...
	    */
	    if ((charInfo.width == 2) && (charInfo.startCol != width - 2)) {
		/* only half of charInfo.idx fits in the line, so drop
		 * it...
		 */
		retLen = charInfo.idx;
	    } else {
		/* single column character or double column character that
		 * fits on the line...
		 */
		retLen = charInfo.idx + 1;
	    }
	}
	else
	{
	    retLen = 0;
	}
    }
    else
    {
	retLen = 0;
    }
    
    return(retLen);

}

/*
** the line width is the lesser of the actual width, or the buffer width
** (in the case that the buffer shrank after the line was full)
*/
short
_DtTermPrimBufferGetLineWidth
(
    const TermBuffer    tb,
    const short         row
)
{
    short           width = 0;
    TermCharInfoRec charInfo;
    
    if (VALID_ROW(tb, row)) 
    {
	width = MIN(WIDTH(LINE_OF_TBUF(tb, row)), COLS(tb));
	if (width > 0)
	{
	    
	    (void)_DtTermPrimGetCharacterInfo(tb, row, width - 1, &charInfo);
    
	    /*
	    ** a line can not end on column 1 of 2...
	    */
	    if ((charInfo.width == 2) && (charInfo.startCol != width - 2)) {
		/* only half of charInfo.idx fits in the line, so drop
		 * it...
		 */
		(void) width--;
	    } else {
		/* single column character or double column character that
		 * fits on the line...
		 */
	    }
	}
    }
    
    return(width);
}


/**************************************************************************
 *  Function:
 *	_DtTermPrimBufferMoveLockArea():
 *		move locked screen memory up or down in the screen buffer.
 *
 *  Parameters:
 *	TermBuffer tb: term buffer to use
 *	short dest: point to move the locked region to
 *	short src: point to move the locked region from
 *	short length: size of the locked region
 *
 *  Returns:
 *	<nothing>
 *
 *  Notes:
 */
void
_DtTermPrimBufferMoveLockArea
(
    const TermBuffer tb, 
    const short dest,
    const short src,
    const short length
)
{
    TermLine lineCache[10];
    TermLine *holdLines = lineCache;
    TermLine *destPtr;
    TermLine *srcPtr;
    short cacheSize;
    short distance;
    int i;
    int refLineCount;
    TermLine *refLines;

    DebugF('B', 1, RememberTermBuffer(tb,&refLines,&refLineCount));

    /* don't bother if there is we are being asked to do nothing...
     */
    if ((length <= 0) || (dest == src))
	return;

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /* before we modify the buffer, check and see if the leading or
     * trailing edge of the buffer crosses the selection and if the
     * destination line crosses the selection.  If it does, we need
     * to disown it...
     */
    /* leading edge of src... */
    if ((src > 0) &&
	    IS_IN_SELECTION(LINE_OF_TBUF(tb, src - 1), MIN_SELECTION_COL,
	    MAX_SELECTION_COL) &&
	    IS_IN_SELECTION(LINE_OF_TBUF(tb, src), MIN_SELECTION_COL,
	    MAX_SELECTION_COL)) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    /* trailing edge of src... */
    if (((src + length) < ROWS(tb)) &&
	    IS_IN_SELECTION(LINE_OF_TBUF(tb, src + length - 1),
	    MIN_SELECTION_COL, MAX_SELECTION_COL) &&
	    IS_IN_SELECTION(LINE_OF_TBUF(tb, src + length), MIN_SELECTION_COL,
	    MAX_SELECTION_COL)) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    /* destination... */
    if ((dest > 0) && IS_IN_SELECTION(LINE_OF_TBUF(tb, dest - 1),
	    MIN_SELECTION_COL, MAX_SELECTION_COL) &&
	    IS_IN_SELECTION(LINE_OF_TBUF(tb, dest),
	    MIN_SELECTION_COL, MAX_SELECTION_COL)) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }
	
    /* we need to save the line positions of the smaller of the length
     * of the chunk to move, or the distance to move the chunk.
     */
    distance = (dest > src) ? (dest - src) : (src - dest);
    cacheSize = MIN(length, distance);

    /* if we are moving more lines than will fit in the lineCache, we need
     * to malloc (and free) storage for the termLineRecs...
     */
    if (cacheSize > (sizeof(lineCache) / sizeof(lineCache[0]))) {
	holdLines = (TermLine *) XtMalloc(cacheSize * sizeof(TermLine));
    }

     /*
     ** clear the wrap flag for the line preceding the block to be
     ** moved, and the last line of the block to be moved
     */
     _DtTermPrimBufferSetLineWrapFlag(tb, src - 1, FALSE);
     _DtTermPrimBufferSetLineWrapFlag(tb, src + length, FALSE);

    /* cache them away... */
    if (distance > length) {
	/* save away the locked area... */
	destPtr = holdLines;
	srcPtr = &(LINE_OF_TBUF(tb, src));

	for (i = 0; i < length; i++) {
	    *destPtr++ = *srcPtr++;
	}
	/* move the area above/below the locked area... */
	if (dest > src) {
	    /* we are moving the locked area down, so we must move
	     * the scroll area up...
	     */
	    destPtr = &(LINE_OF_TBUF(tb, src));
	    srcPtr = &(LINE_OF_TBUF(tb, src + length));
	    for (i = 0; i < distance; i++) {
		*destPtr++ = *srcPtr++;
	    }
	} else {
	    /* we are moving the locked area up, so we must move
	     * the scroll area down...
	     */
	    destPtr = &(LINE_OF_TBUF(tb, src + length - 1));
	    srcPtr = &(LINE_OF_TBUF(tb, src - 1));
	    for (i = 0; i < distance; i++) {
		*destPtr-- = *srcPtr--;
	    }
	}
	/* restore the cached lock area... */
	destPtr = &(LINE_OF_TBUF(tb, dest));
	srcPtr = holdLines;
	for (i = 0; i < length; i++) {
	    *destPtr++ = *srcPtr++;
	}
    } else {
	/* save away the area we will move the lock area over...
	 */
	if (dest > src) {
	    /* moving it down.  Save the area under the lock area...
	     */
	    destPtr = holdLines;
	    srcPtr = &(LINE_OF_TBUF(tb, src + length));
	    for (i = 0; i < distance; i++) {
		*destPtr++ = *srcPtr++;
	    }
	    /* move the lock area down... */
	    destPtr = &(LINE_OF_TBUF(tb, dest + length - 1));
	    srcPtr = &(LINE_OF_TBUF(tb, src + length - 1));
	    for (i = 0; i < length; i++) {
		*destPtr-- = *srcPtr--;
	    }
	    /* restore the area under (that is now over) the lock area... */
	    destPtr = &(LINE_OF_TBUF(tb, src));
	    srcPtr = holdLines;
	    for (i = 0; i < distance; i++) {
		*destPtr++ = *srcPtr++;
	    }
	} else {
	    /* moving it up.  Save the area over the lock area...
	     */
	    destPtr = holdLines;
	    srcPtr = &(LINE_OF_TBUF(tb, dest));
	    for (i = 0; i < distance; i++) {
		*destPtr++ = *srcPtr++;
	    }
	    /* move the lock area up... */
	    destPtr = &(LINE_OF_TBUF(tb, dest));
	    srcPtr = &(LINE_OF_TBUF(tb, src));
	    for (i = 0; i < length; i++) {
		*destPtr++ = *srcPtr++;
	    }
	    /* restore the area over (that is now under) the lock area... */
	    destPtr = &(LINE_OF_TBUF(tb, dest + length));
	    srcPtr = holdLines;
	    for (i = 0; i < distance; i++) {
		*destPtr++ = *srcPtr++;
	    }
	}
    }

    /* free up the holdLines (if we malloc'ed it)... */
    if (holdLines != lineCache) {
	(void) XtFree((char *) holdLines);
    }

    DebugF('B', 1, CheckTermBuffer(tb,refLines,refLineCount));
}


/**************************************************************************
 *  Function:
 *	_DtTermPrimBufferInsertLine():  insert one or more lines of text from
 *		the end of the buffer below the insertion point, or from
 *		the beginning of the buffer above the insertion point.
 *
 *  Parameters:
 *	TermBuffer tb: term buffer to insert end/begining into
 *	short dest: point to insert below/above
 *	short length: number of lines to insert
 *	short src: source point of insertion
 *
 *  Returns:
 *	<nothing>
 *
 *  Notes:
 */
void
_DtTermPrimBufferInsertLine
(
    const TermBuffer tb,
    const short dest,
    const short length,
    const short src
)
{
    TermLine lineCache[10];
    TermLine *holdLines = lineCache;
    TermLine *destPtr;
    TermLine *srcPtr;
    short distance;
    int i;
    int refLineCount;
    TermLine *refLines;

    DebugF('B', 1, RememberTermBuffer(tb,&refLines,&refLineCount));

    /* don't bother if there is we are being asked to do nothing...
     */
    if (length <= 0)
	return;

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /* before we modify the buffer, check and see if the
     * destination line crosses the selection.  If it does, we need
     * to disown it...
     */
    /* destination... */
    if (dest > src) {
	if (((dest + length) < ROWS(tb)) &&
		IS_IN_SELECTION(LINE_OF_TBUF(tb, dest + length),
		MIN_SELECTION_COL, MAX_SELECTION_COL) &&
		IS_IN_SELECTION(LINE_OF_TBUF(tb, dest + length + 1),
		MIN_SELECTION_COL, MAX_SELECTION_COL)) {
	    (void) _DtTermPrimSelectDisown(WIDGET(tb));
	}
    } else {
	if (dest > 1) {
	    if (IS_IN_SELECTION(LINE_OF_TBUF(tb, dest),
		    MIN_SELECTION_COL, MAX_SELECTION_COL) &&
		    IS_IN_SELECTION(LINE_OF_TBUF(tb, dest - 1),
		    MIN_SELECTION_COL, MAX_SELECTION_COL)) {
		(void) _DtTermPrimSelectDisown(WIDGET(tb));
	    }
	}
    }

    /* if we are being asked to move all (or more) the lines in the
     * buffer, then we can just clear them all out and return...
     */
    if (length >= ROWS(tb)) {
	for (i = 0; i < ROWS(tb); i++) {
            _DtTermPrimBufferClearLine(tb, i, 0);
	}
	return;
    }

    /* if dest and src match, we can just clear them out and return...
     */
    if (src == dest) {
	for (i = 0; (i < length) && ((i + dest) < ROWS(tb)); i++) {
            _DtTermPrimBufferClearLine(tb, src + i, 0);
	}
	return;
    }

    /* if we are moving more lines than will fit in the lineCache, we need
     * to malloc (and free) storage for the termLineRecs...
     */
    if (length > (sizeof(lineCache) / sizeof(lineCache[0]))) {
	holdLines = (TermLine *) XtMalloc(length * sizeof(TermLine));
    }

    if (dest > src) {
	/* our src is above the destination.  Copy the lines to insert,
	 * move the lines above the insertion pointer up, and insert
	 * the saved lines...
	 */

	/* save away the top length lines... */
	destPtr = holdLines;
	srcPtr = &(LINE_OF_TBUF(tb, src));
	for (i = 0; i < length; i++) {
	    /* these lines will be cleared, so we don't need to track
	     * movement of them...
	     */
	    *destPtr++ = *srcPtr++;
	}
	/* copy the lines above the insertion point up... */
	destPtr = &(LINE_OF_TBUF(tb, src));
	srcPtr = &(LINE_OF_TBUF(tb, src + length));
	for (i = src; i < dest - length + 1; i++) {
	    *destPtr++ = *srcPtr++;
	}
	/* restore the saved lines above the insertion point... */
	destPtr = &(LINE_OF_TBUF(tb, dest - length + 1));
	srcPtr = holdLines;
	for (i = 0; i < length; i++) {
	    *destPtr = *srcPtr;
	    /* clear the line... */
            _DtTermPrimBufferClearLine(tb, dest - length + 1 + i, 0);
	    *destPtr++;
            *srcPtr++;
	}

#ifdef	DONT_DO_THIS_ANY_MORE
        /* Adjust or disown selection */
        _DtTermPrimSelectDeleteLine(tb,length) ;
#endif	/* DONT_DO_THIS_ANY_MORE */
    } else {
	/* our src is below the destination.  Copy the lines to insert,
	 * move the lines below the insertion pointer down, and insert
	 * the saved lines...
	 */
	/* save away the bottom length lines... */
	destPtr = holdLines;
	srcPtr = &(LINE_OF_TBUF(tb, src));
	for (i = 0; i < length; i++) {
	    *destPtr++ = *srcPtr++;
	}
	/* copy the lines below the insertion point down... */
	destPtr = &(LINE_OF_TBUF(tb, src + length - 1));
	srcPtr = &(LINE_OF_TBUF(tb, src - 1));
	for (i = 0; i < src - dest; i++) {
	    *destPtr-- = *srcPtr--;
	}
	/* restore the saved lines below the insertion point... */
	destPtr = &(LINE_OF_TBUF(tb, dest));
	srcPtr = holdLines;
	for (i = 0; i < length; i++) {
	    *destPtr = *srcPtr;
	    /* clear the line... */
            _DtTermPrimBufferClearLine(tb, dest + i, 0);
	    *destPtr++;
            *srcPtr++;
	}
    }

    /* free up the holdLines (if we malloc'ed it)... */
    if (holdLines != lineCache) {
	(void) XtFree((char *) holdLines);
    }

    DebugF('B', 1, CheckTermBuffer(tb,refLines,refLineCount));
}


/**************************************************************************
 *  Function:
 *	_DtTermPrimBufferInsertLineFromTB():  insert one or more lines
 *		of text from the end of the buffer below the insertion
 *		point, or from the beginning of the buffer above the
 *		insertion point.
 *
 *  Parameters:
 *	TermBuffer tb: term buffer to insert end/begining into
 *	short dest: point to insert below/above
 *	short length: number of lines to insert
 *	InsertSource: source of insertion (end or beginning)
 *
 *  Returns:
 *	<nothing>
 *
 *  Notes:
 */
void
_DtTermPrimBufferInsertLineFromTB
(
    const TermBuffer tb,
    const short dest,
    const short length,
    const InsertSource insertSource
)
{
    if (insertSource == insertFromTop) {
	(void) _DtTermPrimBufferInsertLine(tb, dest, length, 0);
    } else {
	(void) _DtTermPrimBufferInsertLine(tb, dest, length,
		ROWS(tb) - length);
    }
}


#ifdef	NOTDONE
/**************************************************************************
 *  Function:
 *	_DtTermPrimBufferScrollToHistory():  scroll one or more lines
 *		of text from the beginning of the active buffer into
 *		the history buffer.
 *
 *  Parameters:
 *	TermBuffer tb: term buffer to scroll out of
 *	short length: number of lines to scroll
 *
 *  Returns:
 *	the number of lines scrolled (removed from active buffer)
 *
 *  Notes:
 */
int
_DtTermPrimBufferScrollToHistory()
(
    const TermBuffer tb,
    const short length,
)
{
    if (PREV_BUFFER(tb)) {
    
}
#endif	/* NOTDONE */


/**************************************************************************
 *  Function:
 *	_DtTermPrimBufferDeleteLine():  deletes one or more lines of text from
 *		the deletion poing and sticks them at the end of the
 *		line buffer.  It does not clear the line.  That is
 *		normally done when the line is inserted.
 *
 *  Parameters:
 *	TermBuffer tb: term buffer to insert end/begining into
 *	short source: point to insert below/above
 *	short length: number of lines to insert
 *	short lastUsedLine: number of last line in the buffer we need
 *		to keep
 *
 *  Returns:
 *	<nothing>
 *
 *  Notes:
 */
void
_DtTermPrimBufferDeleteLine
(   
    const TermBuffer tb, 
    const short source,
    const short length,
    const short lastUsedRow
)
{
    TermLine lineCache[10];
    TermLine *holdLines = lineCache;
    TermLine *destPtr;
    TermLine *srcPtr;
    short copyLength;
    int i;
    int refLineCount;
    TermLine *refLines;

    DebugF('B', 1, RememberTermBuffer(tb,&refLines,&refLineCount));

    /* don't bother if there is we are being asked to do nothing...
     */
    if (length <= 0)
	return;
    /* if we are being asked to move the last usedLines, we don't have
     * to do anything either...
     */
    if (source + length >= lastUsedRow)
	return;

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /* if we are moving more lines than will fit in the lineCache, we need
     * to malloc (and free) storage for the termLineRecs...
     */
    if (length > (sizeof(lineCache) / sizeof(lineCache[0]))) {
	holdLines = (TermLine *) XtMalloc(length * sizeof(TermLine));
    }

    /* clear the deleted lines... */
    for (i = 0; i < length; i++) {
	(void) _DtTermPrimBufferClearLine(tb, source + i, 0);
    }

#ifdef	USE_MEMCPY
    (void) memcpy(holdLines, &(LINE_OF_TBUF(tb, source)), length * 
                  sizeof(TermLine));
#else	/* USE_MEMCPY */
    destPtr = holdLines;
    srcPtr = &(LINE_OF_TBUF(tb, source));
    for (i = 0; i < length; i++) {
	*destPtr++ = *srcPtr++;
    }
#endif	/* USE_MEMCPY */

    /* ripple up the lines... */
    copyLength = MAX(0, MIN(ROWS(tb), lastUsedRow) - source - length);
    if (copyLength > 0) {
#ifdef	USE_MEMCPY
	(void) memmove(&(LINE_OF_TBUF(tb, source)),
                      &(LINE_OF_TBUF(tb, source + length)),
		      copyLength * sizeof(TermLine));
#else	/* USE_MEMCPY */
	destPtr = &(LINE_OF_TBUF(tb, source));
	srcPtr = &(LINE_OF_TBUF(tb, source + length));
	for (i = 0; i < copyLength; i++) {
	    *destPtr++ = *srcPtr++;
	}
#endif	/* USE_MEMCPY */
    }

    /* copy back the deleted (cached) lines... */
#ifdef	USE_MEMCPY
    (void) memcpy(&(LINE_OF_TBUF(tb, source + copyLength)), holdLines,
	    length * sizeof(TermLine));
#else	/* USE_MEMCPY */
    destPtr = &(LINE_OF_TBUF(tb, source + copyLength));
    srcPtr = holdLines;
    for (i = 0; i < length; i++) {
	*destPtr++ = *srcPtr++;
    }
#endif	/* USE_MEMCPY */

    /* free up the holdLines (if we malloc'ed it)... */
    if (holdLines != lineCache) {
	(void) XtFree((char *) holdLines);
    }

    DebugF('B', 1, CheckTermBuffer(tb,refLines,refLineCount));
}

/*
** Pad the requested row from the current width to 'newWidth' with spaces...
*/
void
_DtTermPrimBufferPadLine
(
    const TermBuffer  tb,
    const short       row,
    const short       newWidth
)
{
    TermLine    line;
    
#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    line = LINE_OF_TBUF(tb, row);

    /* if this line is part of the selection, disown the selection... */
    if (IS_IN_SELECTION(line, MIN(newWidth, WIDTH(line)),
	    MAX(newWidth, WIDTH(line)))) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    (void)memset(BUFFER(line) + WIDTH(line),
                 0x20, (newWidth - WIDTH(line)) * BYTES_PER_CHAR(tb));
    if (CLEAR_ENH(tb))
    {
        (*CLEAR_ENH(tb))(tb, row, WIDTH(line), newWidth - WIDTH(line));
    }
    _DtTermPrimBufferSetLineWidth(tb, row, newWidth);
}

/*
** Clear the line to the new width (just reset the line width).
** NOTES:
**     We also clear the wrapped flag as well.
*/
Boolean
_DtTermPrimBufferClearLine
(
    const TermBuffer  tb,
    const short       row,
          short       newWidth
)
{
    TermLine    line;

#ifdef	OLD_CODE
    /* before we modify the buffer, disown the selection... */
    (void) _DtTermPrimSelectDisownIfNecessary(WIDGET(tb));
#endif	/* OLD_CODE */

    /*
    ** handle multi-byte locales...
    */
    if (BYTES_PER_CHAR(tb) > 1)
    {
	return(_DtTermPrimBufferClearLineWc(tb, row, newWidth));
    }
	
    /*
    ** Some simple bounds checking.
    */
    if (!VALID_ROW(tb, row))
    {
        return(False);
    }

    line = LINE_OF_TBUF(tb, row);

    /* if this line is part of the selection, disown the selection... */
    if (IS_IN_SELECTION(line, MIN(newWidth, WIDTH(line)),
	    MAX(newWidth, WIDTH(line)))) {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    /*
    ** Clip the new width to the buffer width.
    */
    newWidth = MIN(newWidth, COLS(tb));

    /*
    ** force width to the desired value
    **
    ** (We take the direct approach because _DtTermPrimBufferSetLineWidth
    **  doesn't allow the line width to decrease.)
    */
    if (newWidth < WIDTH(line))
    {
	/* 
	** Call the helper function if it exists
	*/
	if (CLEAR_LINE(tb))
	{
	    (*CLEAR_LINE(tb))(tb, row, newWidth);
	}
	WRAPPED(line) = False;
	WIDTH(line)   = newWidth;
	LENGTH(line)  = newWidth; /* this works in single-byte locales */
    }
    return(True);
}

short
_DtTermPrimBufferGetNextTab
(
    const TermBuffer    tb,
          short         col
)
{
    if (!VALID_COL(tb, col))
    {
        return(0);
    }

    /* find the next set tab stop... */
    for (col++; (col < COLS(tb)) && (!TABS(tb)[col]); col++)
	;

    /* did we go to the end of the line w/out hitting one?... */
    if (col >= COLS(tb)) {
	return(0);
    } else {
	return(col);
    }
}


short
_DtTermPrimBufferGetPreviousTab
(
    const TermBuffer    tb,
          short         col
)
{
    if (!VALID_COL(tb, col - 1))
    {
        return(0);
    }

    /* find the next set tab stop... */
    for (col--; (col > 0) && (!TABS(tb)[col]); col--)
	;

    /* we can just return col.  If ther was not another tab stop, it will
     * be -1...
     */
    return(col);
}


Boolean
_DtTermPrimBufferSetTab
(
    const TermBuffer    tb,
    const short         col
)
{
    if (VALID_COL(tb, col))
    {
        TABS(tb)[col] = True;
	return(True);
    }
    return(False);
}


Boolean
_DtTermPrimBufferClearTab
(
    const TermBuffer    tb,
    const short         col
)
{
    if (VALID_COL(tb, col))
    {
        TABS(tb)[col] = False;
	return(True);
    }
    return(False);
}


Boolean
_DtTermPrimBufferClearAllTabs
(
    const TermBuffer      tb
)
{
    (void) memset(TABS(tb), '\0', COLS(tb) * sizeof(Boolean));
    return(True);
}

/*
** Set the line wrap flag to the desired state.
*/
void
_DtTermPrimBufferSetLineWrapFlag
(
    TermBuffer  tb,
    short       row,
    Boolean     state
)
{
    if (VALID_ROW(tb, row))
    {
	WRAPPED(LINE_OF_TBUF(tb, row)) = state;
    }
    return;
}

/*
** Return the state of the line wrap flag.
*/
Boolean
_DtTermPrimBufferTestLineWrapFlag
(
    TermBuffer  tb,
    short       row
)
{
    if (VALID_ROW(tb, row))
    {
	return(WRAPPED(LINE_OF_TBUF(tb, row)));
    }
    else 
    {
        return(FALSE);
    }
}

/*
** Set the in selection flag to the desired state.
*/
void
_DtTermPrimBufferSetInSelectionFlag
(
    TermBuffer  tb,
    short       row,
    TermLineSelection     state
)
{
    if (VALID_ROW(tb, row))
    {
	if (state) {
	    START_SELECTION(LINE_OF_TBUF(tb, row)) = state->selectionStart;
	    END_SELECTION(LINE_OF_TBUF(tb, row)) = state->selectionEnd;
	} else {
	    START_SELECTION(LINE_OF_TBUF(tb, row)) = NON_SELECTION_COL;
	    END_SELECTION(LINE_OF_TBUF(tb, row)) = NON_SELECTION_COL;
	}
    }
    return;
}

/*
** Return the state of the in selection flag.
*/
TermLineSelection
_DtTermPrimBufferGetInSelectionFlag
(
    TermBuffer  tb,
    short       row
)
{
    if (VALID_ROW(tb, row))
    {
	if ((START_SELECTION(LINE_OF_TBUF(tb, row)) != NON_SELECTION_COL) &&
		(END_SELECTION(LINE_OF_TBUF(tb, row)) != NON_SELECTION_COL)) {
	    return(&IN_SELECTION(LINE_OF_TBUF(tb, row)));
	}
    }
    return((TermLineSelection) 0);
}

void
_DtTermPrimBufferSetSelectLines
(
    TermBuffer tb,
    short beginRow,
    short beginCol,
    short endRow,
    short endCol
)
{
    int			  i1;

    for (i1 = 0; i1 < ROWS(tb); i1++) {
	if ((i1 >= beginRow) && (i1 <= endRow)) {
	    if (i1 == beginRow) {
		START_SELECTION(LINE_OF_TBUF(tb, i1)) = beginCol;
	    } else {
		START_SELECTION(LINE_OF_TBUF(tb, i1)) = MIN_SELECTION_COL;
	    }
	    if (i1 == endRow) {
		END_SELECTION(LINE_OF_TBUF(tb, i1)) = endCol;
	    } else {
		END_SELECTION(LINE_OF_TBUF(tb, i1)) = MAX_SELECTION_COL;
	    }
	} else {
	    START_SELECTION(LINE_OF_TBUF(tb, i1)) = NON_SELECTION_COL;
	    END_SELECTION(LINE_OF_TBUF(tb, i1)) = NON_SELECTION_COL;
	}
    }
}

#if (defined(TEST) || defined(__CODECENTER__) || defined(DEBUG))
static void
_termBufferPrintLine
(
    const TermBuffer tb,
    const short row
)
{
    TermLine    line;
    termChar   *pChar;
    short       j;

    printf("Line: %d\n", row);

    line = LINE_OF_TBUF(tb, row];
    printf("    width: %3d\n", WIDTH(line));
    printf("    length: %3d\n", LENGTH(line));
    if (LENGTH(line) > 0)
    {
        printf("    buffer: <");
        pChar = BUFFER(line);
        for (j = 0; j < LENGTH(line); j++)
        {
            printf("%X", *pChar++);
        }
        printf(">\n");
    }
}

/*
** Print the contents of the TermBuffer.
*/
static void
_termBufferPrintBuffer
(
    const TermBuffer tb
)
{
    short i;
    short j;
    short k;

    if (tb == NULL) {
        printf("TermBuffer has been freed.\n");
        return;
    }

    printf("TermBuffer dimensions:\n");
    printf("    rows: %d\n", ROWS(tb));
    printf("    cols: %d\n", COLS(tb));

    for (i = 0; i < ROWS(tb); i++)
    {
        _termBufferPrintLine(tb, i);
    }
}
#endif /* (defined(TEST) || defined(__CODECENTER__)) */

#ifdef TEST
/*
** Some simple tests of the termBuffer.
*/
/* the following is to allow for a single main function in the code... */
#define       termBufMain     main
termBufMain()
{
    const TermBuffer  myTB;

    printf("Sizeof termEnhRec    : %d\n", sizeof(struct _termEnhRec));
    printf("Sizeof termBufferRec : %d\n", sizeof(struct _TermBufferRec));

    myTB = _DtTermPrimBufferCreateBuffer(12, 80);
    _termBufferPrintBuffer(myTB);

    printf("[0,0] %d\n", _DtTermPrimBufferSetEnhancement(myTB, 0, 0, enhVideo, BLINK));
    _termBufferPrintEnhancement(myTB, 0, 0);
    printf("[0,1] %d\n", _DtTermPrimBufferSetEnhancement(myTB, 0, 1, enhVideo, INVERSE));
    _termBufferPrintEnhancement(myTB, 0, 0);
    _termBufferPrintEnhancement(myTB, 0, 1);
    printf("[0,9] %d\n", _DtTermPrimBufferSetEnhancement(myTB, 0, 9, enhVideo, UNDERLINE));
    _termBufferPrintEnhancement(myTB, 0, 0);
    _termBufferPrintEnhancement(myTB, 0, 1);
    _termBufferPrintEnhancement(myTB, 0, 9);
    printf("[0,6] %d\n", _DtTermPrimBufferSetEnhancement(myTB, 0, 6, enhVideo, HALF_BRIGHT));
    _termBufferPrintEnhancement(myTB, 0, 0);
    _termBufferPrintEnhancement(myTB, 0, 1);
    _termBufferPrintEnhancement(myTB, 0, 6);
    _termBufferPrintEnhancement(myTB, 0, 9);
    _termBufferPrintBuffer(myTB);

    _DtTermPrimBufferSetEnhancement(myTB, 10, 10, enhVideo, BLINK);
    _DtTermPrimBufferSetEnhancement(myTB, 10, 20, enhColor, 3);
    _termBufferPrintBuffer(myTB);

    _DtTermPrimBufferResizeBuffer(&myTB,  6, 40);
    _termBufferPrintBuffer(myTB);

    _DtTermPrimBufferSetEnhancement(myTB, 10, 10, enhVideo, BLINK);
    _DtTermPrimBufferResizeBuffer(&myTB, 12, 80);
    _termBufferPrintBuffer(myTB);

    _DtTermPrimBufferFreeBuffer(myTB);
}
#endif /* TEST */

