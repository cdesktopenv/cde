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
static char rcs_id[] = "$TOG: TermBuffer.c /main/2 1997/04/17 18:04:41 samborn $";
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

#include <Xm/Xm.h>
#include "TermPrim.h"
#include "TermHeader.h"       /* for MIN/MAX */
#include "TermBufferP.h"
#include "TermEnhance.h"

/*
** clear "count" enhancements starting at startCol
*/
static void
clearEnhancements
(
    TermBuffer tb,
    short      row,
    short      startCol,
    short      count
);

static void
insertEnhancements
(
    TermBuffer tb,
    short      row,
    short      col,
    short      insertCount
);

static void
deleteEnhancement
(
    TermBuffer tb, 
    short      row,
    short      col
);

static void
_DtTermClearEnhancements
(
    TermBuffer tb,
    short      row,
    short      col,
    short      count
);

static int
_DtTermSetEnhancement
(
    TermBuffer      tb,
    short           row,
    short           col,
    unsigned char   id,
    enhValue        value
);

static Boolean
_DtTermGetEnhancement
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
    enhValue            **enhancements,
    short                *count,
    const countSpec       countWhich
);

static termChar *
_DtTermGetCharacterPointer
(
    TermBuffer  tb,
    short       row,
    short       col
);

static void
_DtTermInsertEnhancements
(
    const TermBuffer  tb, 
    const short       row,
    const short       col,
    short             insertCount,
    const Boolean     insertFlag
);

static Boolean
_DtTermBufferCreateEnhancement
(
    TermBuffer  tb,
    short       row,
    short       col
);

static void
_DtTermBufferResize
(
    TermBuffer  newTb,
    short      *newRows,
    short      *newCols
);

static void
_DtTermDeleteEnhancement
(
    TermBuffer  tb, 
    short       row,
    short       col,
    short       width
);

static short
_DtTermInsert
(
    TermBuffer  tb,
    short       row,
    short       col,
    termChar   *newChars,
    short       numChars,
    Boolean     insertFlag,   /* if TRUE, insert, else overwrite        */
    termChar  **returnChars,  /* pointer to overflow buffer             */
    short      *returnCount   /* count of characters in overflow buffer */
);

static Boolean
_DtTermSetLineLength
(
    TermBuffer      tb,
    short           row,
    short           newLength
);

static short
_DtTermGetLineLength
(
    TermBuffer      tb,
    short           row
);

static Boolean
_DtTermClearLine
(
    TermBuffer  tb,
    short       row,
    short       newLength
);

#if (defined(TEST) || defined(__CODECENTER__) || defined(VALIDATE_ENH))
static void
validateEnhancements
(
    TermBuffer  tb,
    short       row
);    
#    define VALIDATE_ENHANCEMENTS(tb, row) validateEnhancements((tb), (row))
#else    
#    define VALIDATE_ENHANCEMENTS(tb, row)
#endif /* TEST || __CODECENTER__ || VALIDATE_ENH */

/*
** A blank enhancement structure, it will come in handy.
*/
static DtTermEnhPart blankEnh = {0, 0, 0, 0, 0};

/* 
** Create and initialize the Dt-specific parts of the term buffer.
*/
TermBuffer
_DtTermBufferCreateBuffer
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
    TermBuffer      newTB;
    DtLine    *lines;

    newTB = _DtTermPrimBufferCreateBuffer(w, rows, cols, 
                                    sizeOfBuffer, sizeOfLine, sizeOfEnh);
    
    
    if (newTB)
    {
        VALUE_LIST(newTB) = (enhValues) malloc(NUM_ENHANCEMENT_FIELDS *
					       sizeof(enhValue));

	if (!VALUE_LIST(newTB))
	{
	    _DtTermPrimBufferFreeBuffer(newTB);
	    return((TermBuffer) NULL);
	}

	NUM_ENH_FIELDS(newTB) = NUM_ENHANCEMENT_FIELDS;
        for (lines = DT_LINES(newTB);
             lines < DT_LINES(newTB) + ROWS(newTB);
	     lines++)
        {
            DT_ENH(*lines) = (DtEnh) NULL;
        }        
        DT_ENH_STATE(newTB)    = blankEnh;
        DT_ENH_DIRTY(newTB)    = 0;
        ENH_PROC(newTB)        = _DtTermEnhProc;
        BUFFER_CREATE(newTB)   = _DtTermBufferCreateBuffer;
        BUFFER_FREE(newTB)     = _DtTermBufferFreeBuffer;
        BUFFER_RESIZE(newTB)   = _DtTermBufferResize;
        CLEAR_ENH(newTB)       = _DtTermClearEnhancements;
        INSERT_ENH(newTB)      = _DtTermInsertEnhancements;
        DELETE_ENH(newTB)      = _DtTermDeleteEnhancement;
        SET_ENH(newTB)         = _DtTermSetEnhancement;
        GET_ENH(newTB)         = _DtTermGetEnhancement;
        SET_LINE_LENGTH(newTB) = _DtTermSetLineLength;
        CLEAR_LINE(newTB)      = _DtTermClearLine;
    }
    return(newTB);
}                    

/* 
** Resize buffer, this is a helper function, if malloc fails, then the
** appropriate dimensions are forced to the current maximums
*/
static void
_DtTermBufferResize
(
    TermBuffer  tb,
    short      *newRows,
    short      *newCols
)
{
    short   i;
    DtEnh   enh;
    DtLine *lines;
    
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
    if (*newCols > MAX_COLS(tb)) 
    {
	termChar *newLineBuffer;

	/*
	** now extend the line buffers and enhancements for all lines,
	** (even lines that are not being used at the moment
	** (ROWS < MAX_ROWS))...
	*/
	lines = DT_LINES(tb);
	for (i = 0; i < MAX_ROWS(tb); i++)
	{
	    /*
	    ** only copy the enhancement information when it exists
	    */
	    if (DT_ENH(lines[i]))
	    {
		enh = (DtEnh) malloc(*newCols * sizeof(DtTermEnhPart));

		if (enh)
		{
		    /*
		    ** copy the enhancment info for all characters on
		    ** the line, zero out the rest
		    */
		    (void) memcpy(enh, DT_ENH(lines[i]),
				  WIDTH(lines[i]) * sizeof(DtTermEnhPart));
		    (void) memset(&enh[WIDTH(lines[i])], 0,
				  (*newCols - WIDTH(lines[i])) *
				  sizeof(DtTermEnhPart));
		    free(DT_ENH(lines[i]));
		    DT_ENH(lines[i]) = enh;
		}
                else
		{
		    /*
		    ** the malloc failed, revert back to MAX_COLS
		    */
		    *newCols = MAX_COLS(tb);
		    *newRows = MIN(*newRows, MAX_ROWS(tb));
		    break;
		}
	    }
	    newLineBuffer = (termChar *) malloc((unsigned) 
					*newCols * BYTES_PER_CHAR(tb));
	    
	    if (newLineBuffer == NULL)
	    {
		/*
		** line buffer malloc failed, we can only increase the
		** width to the current maximum...
		*/
		*newCols = MAX_COLS(tb);
                *newRows = MIN(*newRows, MAX_ROWS(tb));
		break;
	    }
	    memcpy(newLineBuffer, BUFFER(lines[i]), LENGTH(lines[i]) *
		   BYTES_PER_CHAR(tb));
	    free(BUFFER(lines[i]));
	    BUFFER(lines[i])  = newLineBuffer;
	    WRAPPED(lines[i]) = False;
	}
	MAX_COLS(tb) = *newCols;
    }
    COLS(tb) = *newCols;

    /*
    ** now adjust the length of the buffer as necessary...
    */
    if (*newRows > MAX_ROWS(tb)) 
    {
	/*
	** the number of rows is increasing
	*/
	lines = (DtLine *) malloc((unsigned) *newRows * sizeof(DtLine));
	if (lines != NULL)
	{
	    /*
	    ** the malloc succeeded, copy the old information, and then
	    ** free it...
	    */
	    memcpy(lines, DT_LINES(tb), sizeof(DtLine) * MAX_ROWS(tb));
	    free(DT_LINES(tb));
	    LINES(tb) = (TermLine *)lines;
	    
	    /*
	    ** now initialize the new lines...
	    **
	    ** NOTE:
	    **     If we are unable to malloc any part of a line, adjust
	    **     "newRows" and break.
	    */
	    for (i = MAX_ROWS(tb); i < *newRows; i++)
	    {
		lines[i] = (DtLine) malloc(SIZE_OF_LINE(tb));
		if (lines[i])
		{
		    DT_ENH(lines[i])    = NULL;
		    BUFFER(lines[i]) = (termChar *) malloc((unsigned) COLS(tb) *
							   BYTES_PER_CHAR(tb));
		    if (BUFFER(lines[i]))
		    {
			LENGTH(lines[i])  = 0;
			WIDTH(lines[i])   = 0;
			WRAPPED(lines[i]) = False;
		    }
		    else 
		    {
			/*
			** the line buffer malloc failed...
			*/
			*newRows = i;
			break;
		    }
		}
		else
		{
		    /*
		    ** we couldn't malloc this line...
		    */
		    *newRows = i;
		    break;
		}
	    }
	    MAX_ROWS(tb) = *newRows;
	}
	else
	{
	    /*
	    ** the malloc for the row buffer failed, revert back to MAX_ROWS
	    */
	    *newRows = MAX_ROWS(tb);
	}
    }
    ROWS(tb) = *newRows;
}    

/*
** Free the buffer.
**
** NOTE: This is a helper function, and should only be called by
**       TermPrimBufferFreeBuffer.
*/
void
_DtTermBufferFreeBuffer
(
    const TermBuffer tb
)
{
    DtLine     *lines;

    /*
    ** Free the Dt-specific buffer info...
    */
    for (lines = DT_LINES(tb); lines < DT_LINES(tb) + ROWS(tb); lines++)
    {
        if (DT_ENH(*lines))
        {
            (void) free(DT_ENH(*lines));
        }
    }
    if (VALUE_LIST(tb))
    {
        (void) free(VALUE_LIST(tb));
    }
}

/*
** clear all the enhancements from startCol through stopCol
*/
static void
clearEnhancements
(
    TermBuffer tb,
    short      row,
    short      col,
    short      count
)
{
    DtEnh  enh;
    int    i;
    

    enh  = DT_ENH(DT_LINE_OF_TBUF(tb, row));
    
    if (enh)
    {
	enh += col;
	for(i = 0; i < count; i++)
	{
	    /*
	    ** Clear all of the enhancement information.
	    */
	    *enh = blankEnh;
	    enh++;
	}
    }
}

/*
** Clear "count" enhancement blocks, starting at the specified row and column.
*/
static void
_DtTermClearEnhancements
(
    TermBuffer  tb,
    short       row,
    short       col,
    short       count
)
{
    clearEnhancements(tb, row, col, count);
}

/* 
** Insert the desired number of enhancements at the specified
** position...
**
** NOTE:
**   We depend on the calling function to insure that insertCount
**   has been properly clipped to insure that we don't go out of
**   bounds.
**   Results are undefined if this function is called when the specified
**   column is at the end of the line.
*/
static void
_DtTermInsertEnhancements
(
    const TermBuffer  tb, 
    const short       row,
    const short       col,
    short             insertCount,
    const Boolean     insertFlag
)
{

    DtTermEnhPart   fillEnh;
    DtEnh           enh;
    DtLine          line;
    int             i;    
    int             copyCount;
    
    line = DT_LINE_OF_TBUF(tb, row);
    enh  = DT_ENH(line);

    /* 
    ** There's nothing to do if we're past the end of the line or
    ** the dirty bit is clear and there are no ehancements on
    ** this line...
    */
    if ((col < WIDTH(line)) && ((DT_ENH_DIRTY(tb)) || (enh != NULL)))
    {
        if ((enh == NULL))
        {
            /* 
            ** there are currently no enhancements on this line, 
            ** allocate an enhancement buffer, and reset 'enh'...
            */
            _DtTermBufferCreateEnhancement(tb, row, col);
            enh = DT_ENH(line);
        }

        /* 
        ** get a copy of the current enhancement (we'll insert 'copyCount'
        ** copies of it into the enhancement buffer)
        */
        fillEnh = DT_ENH_STATE(tb);

        if (insertFlag)
        {
            /* 
            ** we're in insert mode, move any existing enhancements...
            */
	    copyCount = MIN((WIDTH(line) - col),
		    (COLS(tb) - col - insertCount));
	    copyCount = MAX(0, copyCount);
            memmove(enh + col + insertCount, enh + col, 
                    copyCount * sizeof(DtTermEnhPart));
        }

#ifdef    NOCODE
        /* 
        ** insert insertCount copies of fillEnh into the enhancement buffer
        ** starting at line->enh[col + 1]...
        */
        enh += col + 1;
        for (i = 0; i < insertCount; i++)
        {
            *enh = fillEnh;
            enh++;
        }
#else  /* NOCODE */
        /* 
        ** insert insertCount copies of fillEnh into the enhancement buffer
        ** starting at line->enh[col + 1]...
        */
        enh += col;
        for (i = 0; i < insertCount; i++)
        {
            *enh = fillEnh;
            enh++;
        }
#endif /* NOCODE */

    }
}

void
_DtTermBufferDelete
(
    TermBuffer  tb,
    short      *row,
    short      *col,
    short      *count        /* number of columns to delete     */
)
{
    _DtTermPrimBufferDelete(tb, row, col, count, NULL, NULL);
}

/* 
** delete the desired enhancements starting the specified position...
*/
static void
_DtTermDeleteEnhancement
(
    TermBuffer  tb, 
    short       row,
    short       col,
    short       width
)
{
    DtEnh           enh;
    DtTermEnhPart   fillEnh;
    DtLine          line;
    int             copyCount;
    
    line = DT_LINE_OF_TBUF(tb, row);
    enh  = DT_ENH(line);

    if ((enh == NULL) || (WIDTH(line) <= col))
    {
        /* 
        ** no enhancements, or at (or past) the end of the line, return
        */
        return;
    }

    /* 
    ** get a copy of the current enhancement
    */
    fillEnh = enh[col];

    /* 
    ** move all of the enhancement blocks between col + width and and the
    ** end of the line to col
    */
    copyCount = WIDTH(line) - (col + width);
    memcpy(enh + col , enh + col + width,
           copyCount * sizeof(DtTermEnhPart));

}


/*
** Create an enhancement block at the specified row and column.
**
** NOTE:  For the time being, we simply allocate an entire row's worth
**        of enhancement blocks if it doesn't exist already.  We may
**        get smarter later on.
*/
/*ARGSUSED*/
static Boolean
_DtTermBufferCreateEnhancement
(
    TermBuffer  tb,
    short       row,
    short       col
)
{
    DtLine  line;
    DtEnh  *enh;
    
    if (!VALID_ROW(tb, row) || !VALID_COL(tb, col))
    {
        return(False);
    }
                        
    line = DT_LINE_OF_TBUF(tb, row);
    enh  = &(DT_ENH(line));

    /*
    ** If this row already has enhancement blocks allocated, return.
    */
    if (*enh == NULL)
    {
        /*
        ** Otherwise, allocate and initialize a row of enhancement blocks.
        */
        *enh = (DtEnh) malloc(MAX_COLS(tb) * sizeof(DtTermEnhPart));
        if (*enh == NULL)
        {
            return(False);
        }

        /*
        ** Clear all the enhancements...
        */
        (void) memset(*enh, 0, MAX_COLS(tb) * sizeof(DtTermEnhPart));
    }
    return(True);
}

/*
** Free the enhancement block at the specified row and column.
**
** NOTE: We may get smarter later on.
*/
Boolean
_DtTermBufferFreeEnhancement
(
    TermBuffer  tb,
    short       row,
    short       col
)
{
    return(True);
}

/*
** Set the desired enhancement.
**
**  This function does the right thing (as far as I can tell regarding
**  propagating enhancements).
**
** Return:
**     -1   : the enhancement was not set
**     >= 0 : the number of characters (as opposed to character positions)
**            that the enhancement affects
*/
static int
_DtTermSetEnhancement
(
    TermBuffer      tb,
    short           row,
    short           col,
    unsigned char   id,
    enhValue        value
)
{
    int     i;
    DtEnh   enhState;
    
    enhState = (DtEnh) &(DT_ENH_STATE(tb));
    
    /*
    ** Set the value.
    */
    switch (id)
    {
      case enhVideo:
        enhState->video   = (value &= VIDEO_MASK);
        break;
      case enhField:
        enhState->field   = (value &= FIELD_MASK);
        break;
      case enhFgColor:
        enhState->fgColor = (value &= COLOR_MASK);
        break;
      case enhBgColor:
        enhState->bgColor = (value &= COLOR_MASK);
        break;
      case enhFont:
        enhState->font    = (value &= FONT_MASK);
        break;
      default:
        return(-1);
    }
    
    /* 
    ** We've set the value, now decide if this anything but the blank
    ** enhancement.
    */
    DT_ENH_DIRTY(tb) = ((enhState->video   != blankEnh.video  ) ||
                        (enhState->field   != blankEnh.field  ) ||
                        (enhState->fgColor != blankEnh.fgColor) ||
                        (enhState->bgColor != blankEnh.bgColor) ||
                        (enhState->font    != blankEnh.font   ));
    /* 
    ** return the correct count (which in this case will always be 0)
    */
    return(0);
}

/*
** Get the enhancements for the desired row and column.
**
**  enhValues:
**      a pointer to an array of the current enhancement values
**
**  count:
**      the number of columns for which the enhancements are
**      valid, or the number of columns until the enhancements
**      were modified by an escape sequence (see 'countWhich' below)
**
**  countWhich:
**      if 'countWhich' is countAll then count until any enhancement value
**      changes
**      if 'countAll' is countNew then count until a new enhancement value
**      starts (regardless of whether it is the same or not).
**
** Return:
**     True if row and col are valid
*/
static Boolean
_DtTermGetEnhancement
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
          enhValue      **values,
          short          *count,
    const countSpec       countWhich
)
{
    /*
    ** store the current enhancement values here
    */
    int    i;
    DtLine line;
    DtEnh  enh;

    /*
    ** First we do some simple bounds checking
    */
    VALIDATE_ENHANCEMENTS(tb, row);
    if (!VALID_ROW(tb, row) || !VALID_COL(tb, col))
    {
        return(False);
    }
    line = DT_LINE_OF_TBUF(tb, row);

    /*
    ** point to the correct enhancement chunk.
    */
    if (DT_ENH(line) == NULL || (WIDTH(line) <= col))
    {
        /*
        ** There are either no enhancements allocated for this line, 
        ** or we're past the end of the line, in either case return
        ** a blank enhancement.
        */
        enh = &blankEnh;
    }
    else
    {
        /*
        ** We're in the line get the current enhancement values
        */
        enh = &(DT_ENH(line)[col]);
    }

    /*
    ** Shove the enhancement values into their correct locations...
    */
    *values                    = (VALUE_LIST(tb));
    (*values)[(int)enhVideo  ] = (enh->video   & VIDEO_MASK);
    (*values)[(int)enhField  ] = (enh->field   & FIELD_MASK);
    (*values)[(int)enhFgColor] = (enh->fgColor & COLOR_MASK);
    (*values)[(int)enhBgColor] = (enh->bgColor & COLOR_MASK);
    (*values)[(int)enhFont   ] = (enh->font    & FONT_MASK );

    /*
    ** Now count how many characters are affected by the enhancements.
    */
    if (DT_ENH(line) == NULL)
    {
        /*
        ** There no enhancements allocated for this line, the default
        ** enhancement is active for the remainder of the line.
        ** NOTE: Make sure count >= 0.
        */
        *count = MAX(0, WIDTH(line) - col);
    }
    else if (WIDTH(line) <= col)
    {
        /*
        ** We're past the end of the line, count will always == 0;
        */
        *count = 0;
    }
    else
    {
        /*
        ** We're in the line, determine the number of characters that
        ** these enhancements apply to.
        */
        switch (countWhich)
        {
          case countNew:
            /*
            **  For Vt220 emulation, countNew is the same as countAll...
            **  JRM 08/30/93
            */
          case countAll:
            /*
            ** count until an enhancement value changes
            */
            for (i = 0; i < (WIDTH(line) - col); i++)
            {
                if (((*values)[(int)enhVideo  ] != (enh->video   & VIDEO_MASK))||
                    ((*values)[(int)enhField  ] != (enh->field   & FIELD_MASK))||
                    ((*values)[(int)enhFgColor] != (enh->fgColor & COLOR_MASK))||
                    ((*values)[(int)enhBgColor] != (enh->bgColor & COLOR_MASK))||
                    ((*values)[(int)enhFont   ] != (enh->font    & FONT_MASK ))  )
                {
                    /*
                    ** the enhancements differ; break out
                    */
                    break;
                }
                enh++;
            }
            break;
          default:
            VALIDATE_ENHANCEMENTS(tb, row);
            return(False);
        }
        *count = i;
    }
    VALIDATE_ENHANCEMENTS(tb, row);
    return(True);
}

/*
** This is a vt-specific helper function for setting the line length.
** By the time the function is called, termBufferSetLineLength()
** as already validated the newLength, the row, and insured that the
** new length is > the current length
**
** This function propagates the correct enhancement to the new end of line.
*/
static Boolean
_DtTermSetLineLength
(
    TermBuffer      tb,
    short           row,
    short           newLength
)
{
    return(True);
}

/*
** _DtTermPrimBufferClearLine will reset the line width, this is
** our chance to clear any enhancements that may exist on this line...
*/
static Boolean
_DtTermClearLine
(
    TermBuffer  tb,
    short       row,
    short       newWidth
)
{
    DtLine  line;
    DtEnh   enh;
    
    line = DT_LINE_OF_TBUF(tb, row);
    enh  = DT_ENH(line);
    
    if (enh != NULL)
    {
        /*
        ** We have enhancements, clear all those between the current
        ** and the new length...
        */
	
        (void) memset(&enh[newWidth], 0,
                      (WIDTH(line) - newWidth) * sizeof(DtTermEnhPart));
    }
    return(True);
}

/*
** Erase characters on the specified row (and clear the enhancements) with
** the appropriate semantics.  For VT class emulators, there are several
** ways to erase characters:
**      - from the active position to the end of line
**      - erase the specified number of characters starting at the current
**        cursor position
**      - from the start of the line to the active position
**      - erase the entire line
**      - from the active position to the end of the buffer
**      - erase the specified number of lines starting at the current
**        cursor position
**      - from the start of the buffer to the active position
**      - erase the entire buffer
*/
void
_DtTermBufferErase
(
    TermBuffer  tb,
    short       row,
    short       col,
    short       count,
    DtEraseMode eraseSwitch
)
{
    short  startCol;
    short  lastCol;
    
    switch(eraseSwitch)
    {
      case eraseFromCol0:
	/*
	** erase from col 0 to the current cursor position
	*/
	startCol = 0;
	lastCol  = MIN(col, WIDTH(DT_LINE_OF_TBUF(tb, row)) - 1);
	break;
	
      case eraseCharCount:
	/*
	** erase "count" characters from the current cursor position
	*/
	startCol = col;
	lastCol  = MIN(col + count - 1, WIDTH(DT_LINE_OF_TBUF(tb, row)) - 1);
	break;

      case eraseLineCount:
      case eraseToEOL:
      case eraseLine:
      case eraseBuffer:
      case eraseFromRow0Col0:
      case eraseToEOB:
	/*
	** These cases should have been handled by _DtTermFuncErase()
	*/
	return;
    }

    _DtTermPrimBufferErase(tb, row, startCol, lastCol);
    /*
    ** now clear the corresponding enhancements...
    */
    clearEnhancements(tb, row, startCol, lastCol - startCol + 1);
}

#if (defined(TEST) || defined(__CODECENTER__) || defined(VALIDATE_ENH))
static void
_DtTermValidateEnhancements
(
    TermBuffer  tb,
    short       row
)
{
    DtTermEnhPart  refEnh;
    DtEnh        thisEnh;
    DtLine    thisLine;
    short          col;
    Boolean validatePassed;

    validatePassed = True;

    thisLine = DT_LINE_OF_TBUF(tb, row);
    if (DT_ENH(thisLine))
    {
        /* 
        ** Initialize the reference enhancement
        */
        refEnh = blankEnh;
        refEnh.videoStart = 0;
        refEnh.fieldStart = 0;
        refEnh.colorStart = 0;
        refEnh.fontStart  = 0;

        for(col = 0, thisEnh = DT_ENH(thisLine); 
            col < WIDTH(thisLine) + 
                  (DANGLE(thisLine) >= 0 ? 1 : 0);
            col++, thisEnh++)
        {
            if (thisEnh->videoStart)
            {
                refEnh.video = thisEnh->video;
            }
            else if (refEnh.video != thisEnh->video)
            {
                fprintf(stderr, "Video enhancements don't match:");
                fprintf(stderr, "    row : %3.3d, col : %3.3d\n", row, col);
                fprintf(stderr, "    refEnh.video : %d\n", (int)(refEnh.video  & VIDEO_MASK));
                fprintf(stderr, "    thisEnh->video: %d\n", (int)(thisEnh->video & VIDEO_MASK));
                validatePassed = False;
            }    

            if (thisEnh->fieldStart)
            {
                refEnh.field = thisEnh->field;
            }
            else if (refEnh.field != thisEnh->field)
            {
                fprintf(stderr, "Field enhancements don't match:");
                fprintf(stderr, "    row : %3.3d, col : %3.3d\n", row, col);
                fprintf(stderr, "    refEnh.field : %d\n", (int)(refEnh.field  & FIELD_MASK));
                fprintf(stderr, "    thisEnh->field: %d\n", (int)(thisEnh->field & FIELD_MASK));
                validatePassed = False;
            }    
            if (thisEnh->colorStart)
            {
                refEnh.color = thisEnh->color;
            }
            else if (refEnh.color != thisEnh->color)
            {
                fprintf(stderr, "Color enhancements don't match:");
                fprintf(stderr, "    row : %3.3d, col : %3.3d\n", row, col);
                fprintf(stderr, "    refEnh.color : %d\n", (int)(refEnh.color  & COLOR_MASK));
                fprintf(stderr, "    thisEnh->color: %d\n", (int)(thisEnh->color & COLOR_MASK));
                validatePassed = False;
            }    
            if (thisEnh->font <= 2)
            {
                if (thisEnh->fontStart)
                {
                    refEnh.font = thisEnh->font;
                }
                else if (refEnh.font != thisEnh->font)
                {
                    fprintf(stderr, "Font enhancements don't match:");
                    fprintf(stderr, "    row : %3.3d, col : %3.3d\n", row, col);
                    fprintf(stderr, "    refEnh.font : %d\n", (int)(refEnh.font  & FONT_MASK));
                    fprintf(stderr, "    thisEnh->font: %d\n", (int)(thisEnh->font & FONT_MASK));
                    validatePassed = False;
                }    
            }
            else
            {
                fprintf(stderr, "Font enhancement out of range:");
                fprintf(stderr, "    row : %3.3d, col : %3.3d\n", row, col);
                fprintf(stderr, "    thisEnh->font: %d\n", (int)(thisEnh->font & FONT_MASK));
                validatePassed = False;
            }
        }
    }
    if (validatePassed == False)
    {
        fprintf(stderr, "validateEnhancement failed\n");
    }
}
#endif /* TEST || __CODECENTER__ || DEBUG || VALIDATE_ENH */

#if (defined(TEST) || defined(__CODECENTER__) || defined(DEBUG))
#ifdef NOCODE
static void
_printLine
(
    TermBuffer tb,
    short row
)
{
    DtLine line;
    termChar   *pChar;
    short       j;

    printf("Line: %d\n", row);

    line = tb->lines[row];
    printf("    length: %3d\n", line->length);
    if (line->length > 0)
    {
        printf("    buffer: <");
        pChar = line->buffer;
        for (j = 0; j < line->length; j++)
        {
            printf("%X", *pChar++);
        }
        printf(">\n");
    }
}

static int
_DtTermPrintEnhancement
(
    TermBuffer tb,
    short row,
    short col
)
{
    enhValue        enhancements[NUM_ENHANCEMENT_FIELDS];
    short           enhCount;


    _DtTermPrimBufferGetEnhancement(tb, row, col, enhancements, &enhCount, countNew);
    printf("    col  : %d\n", col);
    printf("    Count: %d\n", enhCount);
    printf("        Video: '_");
    printf("%s", IS_BOLD(enhancements[enhVideo])        != 0 ? "B":"b");
    printf("_");
    printf("%s", IS_SECURE(enhancements[enhVideo])      != 0 ? "S":"s");
    printf("_");
    printf("%s", IS_HALF_BRIGHT(enhancements[enhVideo]) != 0 ? "H":"h");
    printf("_");
    printf("%s", IS_UNDERLINE(enhancements[enhVideo])   != 0 ? "U":"u");
    printf("_");
    printf("%s", IS_INVERSE(enhancements[enhVideo])     != 0 ? "I":"i");
    printf("_");
    printf("%s", IS_BLINK(enhancements[enhVideo])       != 0 ? "B":"b");
    printf("_'\n");

    /*
    ** Field type
    */
    printf("        Field: ");
    switch (enhancements[enhField])
    {
      case FIELD_PROTECT:
        printf("PROTECT");
        break;
      case FIELD_UNPROTECT:
        printf("UNPROTECT");
        break;
      case FIELD_TRANSMIT:
        printf("TRANSMIT");
        break;
      case FIELD_END:
        printf("END");
        break;
    }
    printf("\n");

    /*
    ** Color id
    */
    printf("        Color: %1d\n", enhancements[enhFont]);

    /*
    ** Font id
    */
    printf("        Font : ");
    switch (enhancements[enhFont])
    {
      case FONT_NORMAL:
        printf("NORMAL");
        break;
      case FONT_LINEDRAW:
        printf("LINEDRAW");
        break;
    }
    printf("\n");
    return(enhCount);
}
#endif /* NOCODE */

static void
printEnh
(
    DtEnh enh
)
{
    printf(" video  : %d\n", enh->video);
    printf(" field  : %d\n", enh->field);
    printf(" fgColor: %d\n", enh->fgColor);
    printf(" bgColor: %d\n", enh->bgColor);
    printf(" font   : %d\n", enh->font);
}

/*
** Print the contents of the TermBuffer.
*/
static void
_DtTermPrintBuffer
(
    DtTermBuffer tb
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
    printf("    enhDirty: %d\n", DT_ENH_DIRTY(tb));
    printf("    enhState:\n");
    printEnh(&(DT_ENH_STATE(tb)));
#ifdef NOCODE
    for (i = 0; i < ROWS(tb); i++)
    {
        _printLine(tb, i);
        j = 0;
        do
        {
            k = _termBufferPrintEnhancement(tb, i, j);
            if (k == 0)
            {
                break;
            }
            j += k;
        } while (j < COLS(tb));
    }
#endif /* NOCODE */
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
    TermBuffer  myTB;

    printf("Sizeof DtTermEnhPart : %d\n", sizeof(struct _DtTermEnhPart));
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









