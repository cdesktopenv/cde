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
static char rcs_id[] = "$XConsortium: TermPrimBufferWc.c /main/1 1996/04/21 19:16:59 drk $";
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
#include "TermPrimOSDepI.h"
#include "TermPrimBufferP.h"
#include "TermPrimDebug.h"

static void
_termBufferValidateLineWc
(
    const TermBuffer tb,
    const short row
);

static void
_patchUpChar
(
    TermBuffer   tb,
    short        row,
    short       *col,
    TermCharInfo charInfo
);

static void
_countWidth
(
          wchar_t *wcBuffer,
          short    length,
          short    maxWidth,
          short   *retCount,
          short   *retWidth 
);

static void
_primBufferInsertWc
(
    const TermBuffer  tb,
    const short       row,
          short      *col,
          wchar_t    *newChars,
          short       numChars,
          short      *lengthInc,
          short      *widthInc,
          short      *widthInsert,  /* width of inserted characters           */
          termChar   *returnChars,  /* pointer to overflow buffer             */
          short      *returnCount   /* count of characters in overflow buffer */
);

static void
_primBufferOverwriteWc
(
    const TermBuffer  tb,
    const short       row,
          short      *col,
          wchar_t    *newChars,
          short       numChars,
          short      *lengthInc,
          short      *widthInc,
          short      *widthInsert,  /* width of inserted characters           */
          termChar   *returnChars,  /* pointer to overflow buffer             */
          short      *returnCount   /* count of characters in overflow buffer */
);

#ifdef USE_SUN_WCWIDTH_PATCH
#include <limits.h>
/* 
**  A small workaround for systems that don't have wcwidth...
*/
int
sun_wcwidth
(
    const wchar_t wc
)
{
    int status;
    char mbstr[MB_LEN_MAX + 1]; 

    status = wctomb(mbstr, wc);

    if (status > 0)
       return(euccol(mbstr));
    else
       return(status);
}
#endif /*  USE_SUN_WCWIDTH_PATCH */

#ifdef USE_USL_WCWIDTH_PATCH
/* 
**  A small workaround for systems that don't have wcwidth...
*/
int
usl_wcwidth
(
    const wchar_t wc
)
{
    return(1);
}
#endif /*  USE_USL_WCWIDTH_PATCH */

short
_DtTermPrimBufferGetTextWc
(
    const TermBuffer      tb,
    const short           row,
    const short           col,
    const short           length,
          char           *buffer,
    const Boolean         needWideChar
)
{
    short   len;
    
    if (!VALID_ROW(tb, row) || !VALID_COL(tb, col))
    {
        return(0);
    }

    len = MIN(length, LENGTH(LINES(tb)[row]) - col);

    if (length > 0)
    {
        memcpy(buffer, BUFFER(LINES(tb)[row]) + col, len);
    }
    return(len);
}

/*
** this is a helper function, that replaces a 2 column character with
** two spaces if "col" falls on column 2 of 2...
*/
static void
_patchUpChar
(
    TermBuffer   tb,
    short        row,
    short       *col,
    TermCharInfo charInfo
)
{
    TermLine line;
    
    line = LINE_OF_TBUF(tb, row);

    /*
    ** make sure we are not trying to overwrite the second column
    ** of a two column character.
    */
    _DtTermPrimGetCharacterInfo(tb, row, *col, charInfo);
    if ((charInfo->width == 2) &&
	(charInfo->startCol == *col - 1))
    {
	/*
	** we are on the second column of a two column character,
	** replace the character with 2 spaces before we proceed
	**
	** first replace the character with a space, then slide the
	** characters one to the right and bump the LENGTH
	** (the width remains constant)
	*/
	*charInfo->u.pwc = L' ';
	memmove(charInfo->u.pwc + 1, charInfo->u.pwc, 
		(LENGTH(line) - charInfo->idx) * sizeof(wchar_t));
	LENGTH(line) += 1;

	/*
	** remember the old starting column for later...
	*/
	(*col)--;
	
	/*
	** now update the charInfo (since we replaced the 2 column
	** character with 2 single column spaces)...
	*/
	charInfo->startCol++;
	charInfo->u.pwc++;
	charInfo->idx++;
	charInfo->width    = 1;
    }
}

/*
** determine how many wide characters in the buffer we can have before the 
** maxWidth is exceeded...
*/
static void
_countWidth
(
          wchar_t *wcBuffer, /* buffer of wide characters                     */
          short    wcLen,    /* number of wide chars in buffer                */
          short    maxWidth, /* maximum width desired                         */
          short   *length,   /* max num of chars with total width <= maxWidth */
          short   *width     /* actual width of the retCount wide chars       */
)
{
          wchar_t *pwc;
          short    charWidth;

    *width = 0;
    for (pwc = wcBuffer; pwc < wcBuffer + wcLen; pwc++)
    {
        switch (charWidth = wcwidth(*pwc))
        {
          case -1:
            /* 
            ** invalid character, (but this should have been handled earlier)
            */
	    /* replace the character with a space... */
	    *pwc = L' ';
	    /* and set the width to 1... */
	    charWidth = 1;
            break;
          case 0:
            /* 
            ** its a null character, but is still has a width for our
            ** purposes...
            */
            charWidth == 1;
	    break;
          default:
            break;
        }    
	if (*width + charWidth > maxWidth)
	{
	    break;
	}
	else
	{
	    *width += charWidth;
	}
    }
    *length = pwc - wcBuffer;
}

/*
** This is a helper function that inserts characters into the specified
** buffer in insert mode.
*/
static void
_primBufferInsertWc
(
    const TermBuffer  tb,
    const short       row,
          short      *col,
          wchar_t    *newChars,
          short       numChars,
          short      *lengthInc,
          short      *widthInc,
	  short      *widthInsert,  /* width of inserted characters      */
          termChar   *returnChars,  /* pointer to overflow buffer             */
          short      *returnLength  /* count of characters in overflow buffer */
)
{
    short             charWidth;
    short             lengthInsert;
    short             insertOverflow; /* # of newChars that would overflow */
    short             overflowLength;
    short             overflowWidth;
    short             localCol;
    TermLine          line;
    wchar_t          *pwc = NULL;
    TermCharInfoRec   charInfo;

    /*
    ** make a copy of *col because it may be modified by
    ** _patchUpChar()
    */
    localCol = *col;
    
    /*
    ** first decide how many characters we can insert before
    ** running off the end of the line...
    */
    _countWidth(newChars, numChars, COLS(tb) - localCol, &lengthInsert,
	        widthInsert);
	   
    /*
    ** return any extra characters...
    */
    *returnLength = numChars - lengthInsert;
	   
    if (*returnLength > 0)
    {
        /*
        ** we have some overflow...
	*/
	memcpy(returnChars, newChars + lengthInsert,
	       *returnLength * sizeof(wchar_t));
    }
	
    /*
    ** make sure we are not trying to overwrite the second column
    ** of a two column character.
    */
    _patchUpChar(tb, row, col, &charInfo);
	   
    /*
    ** Decide how many characters we can insert before running off the
    ** end of the buffer...
    */
    line = LINE_OF_TBUF(tb, row);
    if (WIDTH(line) + *widthInsert <= COLS(tb))
    {
	/*
	** there is no overflow, we can insert all "lengthInsert" characters...
	*/
	*widthInc      = *widthInsert;
	*lengthInc     = lengthInsert;
	overflowLength = 0;
    }
    else
    {
	overflowWidth = WIDTH(line) + *widthInsert - COLS(tb);
	
	/* 
	** inserting the new characters will overflow the line buffer,
	** remove as many of the current characters on the line
	** as necessary to prevent buffer overflow
	*/
	if (overflowWidth > 0)
	{
	    *widthInc = *widthInsert - overflowWidth;
	    
	    for (pwc = ((wchar_t *)BUFFER(line)) + LENGTH(line) - 1; 
		 pwc >= charInfo.u.pwc; pwc--)
	    {
		overflowWidth -= MAX(1, wcwidth(*pwc));

		if (overflowWidth <= 0)
		{
		    /* 
		    ** we've removed enough characters, pwc points to the
		    ** first character to remove...
		    */
		    break;
		}
	    }

	    /*
	    ** final adjustment to widthInc (at this point overflowWidth
	    ** is either 0 (we removed exactly "overflowWidth" worth of
	    ** characters) or -1 (we removed "overflowWidth + 1" worth of
	    ** characters because we removed some 2 column characters)
	    */
	    *widthInc += overflowWidth;
        }

	overflowLength = ((wchar_t *)BUFFER(line)) + LENGTH(line) - pwc; 

	if (overflowLength > 0)
	{
	    *lengthInc = lengthInsert - overflowLength;
	    
	    /* 
	    ** copy the displaced characters from the line to the 
	    ** overflow buffer...
	    */
	    memcpy(returnChars + *returnLength, pwc,
		   overflowLength * sizeof(wchar_t));
	    *returnLength += overflowLength;
        }
    }

    /* 
    ** Any overflow has been taken care of, now it's time to make
    ** room for the new characters...
    **
    ** at this point:
    **      charInfo.pchar points to the character at col
    */
    memmove(charInfo.u.pwc + lengthInsert, charInfo.u.pwc,
	    (((LENGTH(line) - charInfo.idx) - overflowLength) * 
	     sizeof(wchar_t)));

    /*
    ** copy the new characters into the buffer
    */	   
    memcpy(charInfo.u.pwc, newChars, lengthInsert * sizeof(wchar_t));
}

/*
** This is a helper function that inserts characters into the specified
** buffer in overwrite mode.
*/
static void
_primBufferOverwriteWc
(
    const TermBuffer  tb,
    const short       row,
          short      *col,
          wchar_t    *newChars,
          short       numChars,
          short      *lengthInc,
          short      *widthInc,
          short      *widthInsert,  /* width of inserted characters           */
          termChar   *returnChars,  /* pointer to overflow buffer             */
          short      *returnLength  /* count of characters in overflow buffer */
)
{
           short        charWidth;
           short        insertOverflow; /* # of newChars that would overflow */
           short        lengthInsert;
           short        localCol;
           TermLine     line;
    const  char        *pStart;
    TermCharInfoRec     charInfo;
    TermCharInfoRec     startCharInfo;
	   
    line = LINE_OF_TBUF(tb, row);

    /*
    ** make a copy of *col because it may be modified by
    ** _patchUpChar()
    */
    localCol = *col;
    
    /*
    ** first decide how many characters we can overwrite before
    ** running off the end of the line.
    */
    _countWidth(newChars, numChars, COLS(tb) - localCol, &lengthInsert,
	        widthInsert);
	   
    /* 
    ** We are overwriting:
    **      - determine the length and width increments
    **      - put any extra new characters into the overflow buffer
    */
    if (localCol == WIDTH(line))
    {
	/*
	** we are appending to the end of the line, this is easy...
	*/
	*widthInc  = *widthInsert;
        *lengthInc = lengthInsert;
	pStart    = (char *)BUFFER(line) +  (LENGTH(line) * sizeof(wchar_t));
    }
    else
    {
	/*
	** we are overwriting characters in the middle of the line...
	**
	** make sure we are not trying to overwrite the second column
	** of a two column character.
	*/
	_patchUpChar(tb, row, col, &startCharInfo);
	
	/*
	** see if we have to deal with the end of the line...
	*/
	if (localCol + *widthInsert < WIDTH(line))
	{
	    /*
	    ** the line width will remain constant, but the length may 
	    ** change...
	    */
	    *widthInc = 0;

	    /*
	    ** make sure we are not trying to overwrite the first column
	    ** of a two column character.
	    */
	    _DtTermPrimGetCharacterInfo(tb, row, localCol + *widthInsert,
					&charInfo);
	    if ((charInfo.width == 2) &&
		(charInfo.startCol == localCol + *widthInsert - 1))
	    {
		/*
		** We are about to overwrite column 1 of a 2 column
		** character.  Replace it with a space before we proceed
		** (we make adjustments later to make it look like we
		** replaced the second column with a space).
		*/
		*charInfo.u.pwc = L' ';
	    
		/*
		** now update the charInfo (since we replaced the 2 column
		** character with a single column space)...
		*/
		charInfo.width = 1;
	    }

	    /*
	    ** at this point, startCharInfo points to the first
	    ** character to replace, now we want charInfo.u.pwc
	    ** to point to the character one past the last one
	    ** we want to replace
	    */
	    *lengthInc = lengthInsert - (charInfo.u.pwc - startCharInfo.u.pwc);
	    
	    if (*lengthInc != 0)
	    {
		memmove(charInfo.u.pwc + *lengthInc, charInfo.u.pwc,
			(LENGTH(line) - charInfo.idx) * sizeof(wchar_t));
	    }
	}
	else
	{
	    /*
	    ** the line may get wider and longer...
	    */
	    *widthInc  = localCol + *widthInsert - WIDTH(line);
	    *lengthInc = startCharInfo.idx + lengthInsert - LENGTH(line);
	}
	pStart = startCharInfo.u.pc;
    }

    /*
    ** now insert the new characters...
    */
    memcpy((void *)pStart, newChars, lengthInsert * sizeof(wchar_t));

    /*
    ** put any overflow into the overflow buffer
    */
    *returnLength = numChars - lengthInsert;
    if (*returnLength > 0)
    {
	memcpy(returnChars, newChars + lengthInsert,
	       *returnLength * sizeof(wchar_t));
    }
}

/*
** Insert as many characters as possible at the specified row,col
** return a count of the number of characters bumped off the end of the line
** in 'returnLength' and a pointer to a buffer containing those characters
** 'returnChars'.
** 
** The the new column width of the line is returned as the value of the
** function.
**
** NOTES:
**      We are trying to implement mechanism and not policy.  This
**      routine does a minimum of checking for boundary conditions.
*/
short
_DtTermPrimBufferInsertWc
(
    const TermBuffer  tb,
    const short       row,
    const short       col,
          wchar_t    *newChars,
          short       numChars,
          Boolean     insertFlag,   /* if TRUE, insert, else overwrite        */
          termChar  **returnChars,  /* pointer to overflow buffer             */
          short      *returnLength  /* count of characters in overflow buffer */
)   
{
           short    widthInc;       /* incremental change in line width  */
           short    lengthInc;      /* incremental change in line length */
           short    widthInsert;    /* column width of chars inserted    */
	   short    localCol;
           TermLine line;
    
    if (!VALID_ROW(tb, row) || !VALID_COL(tb, col))
    {
        *returnLength = 0;
        return(0);
    }
    
    if (isDebugFSet('i', 1)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) _termBufferValidateLineWc(tb, row);
    }

    line     = LINE_OF_TBUF(tb, row);
    localCol = col;
	  
    if (WIDTH(line) < col)
    {
        /*
        ** We're adding characters past the current end of line,
        ** pad it out.
        */
        _DtTermPrimBufferPadLineWc(tb, row, col);
    }

    /*
    ** It doesn't matter if we're overwriting, or inserting at the end 
    ** of the line, the result is the same...
    */
    if (insertFlag == False || col == WIDTH(line))
    {
	_primBufferOverwriteWc(tb, row, &localCol, newChars, numChars,
			       &lengthInc, &widthInc, &widthInsert,
			       *returnChars, returnLength);
    }
    else
    {
	_primBufferInsertWc(tb, row, &localCol, newChars, numChars,
			    &lengthInc, &widthInc, &widthInsert,
			    *returnChars, returnLength);
    }

    /* 
    ** Everything's ready:
    **     - put the characters into the line
    **     - adjust the line width (_DtTermPrimBufferSetLineWidth won't
    **       let the line get shorter)...
    **     - adjust the line length
    **     - update the enhancements
    */
    WIDTH(line)  += widthInc;
    LENGTH(line) += lengthInc;
    
    /*
    ** fix up the enhancments...
    */
    if (INSERT_ENH(tb))
    {
        (*INSERT_ENH(tb))(tb, row, col, widthInsert, insertFlag);
    }

    if (isDebugFSet('i', 1)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) _termBufferValidateLineWc(tb, row);
    }

    return(WIDTH(line));
}

void
_DtTermPrimBufferDeleteWc
(
    TermBuffer  tb,
    short      *row,
    short      *col,
    short      *width,
    termChar  **returnChars,  /* pointer to delete buffer        */
    short      *returnCount  /* count of bytes in delete buffer */
)
{
    int      copyCount;
    TermLine line;
    short    localRow;
    short    localCol;
    TermCharInfoRec startCharInfo;
    TermCharInfoRec stopCharInfo;

    if (!VALID_ROW(tb, *row) || !VALID_COL(tb, *col))
    {
        if (returnChars)
	{
	    *returnChars = NULL;
	    *returnCount = 0;
	}
        return;	
    }

    /*
    ** save some local copies, to originals may get modified
    */
    localRow = *row;
    localCol = *col;
    line     = LINE_OF_TBUF(tb, *row);
    *width   = MAX(0, MIN(WIDTH(line) - localCol, *width));

    /* 
    ** there are 3 cases of deleting a character from a line:
    **     Case 1:
    **       the cursor is at least 2 positions past the end of the
    **       line (col - WIDTH(line) > 0)
    **
    **     Case 2:
    **       the cursor is in the middle of the line (copyCount > 0)
    **          - move the remaining characters to the left
    **          - deleteEnhancement
    **          - adjust WIDTH and LENGTH
    **
    **     Case 3:
    **       the cursor is at the end of the line (copyCount == 0 and
    **       col == WIDTH(line))
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

    /*
    ** make any necessary adjustments to 2 column characters...
    */
    _patchUpChar(tb, *row, col, &startCharInfo);

    (void) _DtTermPrimGetCharacterInfo(tb, localRow,
               MIN(WIDTH(line), localCol + *width), &stopCharInfo);

    if ((stopCharInfo.width == 2) && 
	(stopCharInfo.startCol == localCol + *width - 1))
    {
	/*
	** do not try to delete column 1 of 2...
	**
	** replace the 2 column character with two one column spaces...
	*/
	*stopCharInfo.u.pwc = L' ';
	memmove(stopCharInfo.u.pwc + 1, stopCharInfo.u.pwc,
		(LENGTH(line) - stopCharInfo.idx) * sizeof(wchar_t));

	/*
	** now make stopCharInfo point at the new space
	*/
	stopCharInfo.width = 1;
	stopCharInfo.startCol++;
	stopCharInfo.u.pwc++;
	stopCharInfo.idx++;
	LENGTH(line)++;
    }
    
    /* 
    ** Save the current characters before we overwrite them
    ** (if returnChars is non-NULL 0)
    */
    if (returnChars != NULL)
    {
	*returnCount = (stopCharInfo.idx - startCharInfo.idx) * sizeof(wchar_t);
	*returnChars = (termChar *)XtMalloc(*returnCount);
	memmove(*returnChars, startCharInfo.u.pwc, *returnCount);
    }
    
    /* 
    ** Cases 2, 3, and 4 require that we delete the enhancement...
    */
    if (DELETE_ENH(tb))
    {
        (*DELETE_ENH(tb))(tb, localRow, localCol, *width);
    }

    copyCount = MAX(0, LENGTH(line) - stopCharInfo.idx);
    if (copyCount > 0)
    {
        /* 
        ** handle case 2
        */
        memmove(startCharInfo.u.pwc, stopCharInfo.u.pwc,
                copyCount * sizeof(wchar_t));
    }

    /* 
    ** Cases 2 and 3 require that we decrement the line length...
    */
    WIDTH(line)  -= *width;
    LENGTH(line) -= (stopCharInfo.idx - startCharInfo.idx);

    /*
    ** update info we need to return...
    */
    *width = stopCharInfo.startCol - startCharInfo.startCol;
}

/*
** Pad the requested row from the current width to 'newWidth' with spaces...
*/
void
_DtTermPrimBufferPadLineWc
(
    const TermBuffer  tb,
    const short       row,
    const short       width
)
{
    short       i;
    short       widthInc;
    TermLine    line;
    wchar_t    *pwc;
    
    line = LINE_OF_TBUF(tb, row);

    if (isDebugFSet('i', 1)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	(void) _termBufferValidateLineWc(tb, row);
    }

    /*
    ** if this line is part of the selection, disown the selection...
    */
    if (IS_IN_SELECTION(line, MIN(width, WIDTH(line)),
			MAX(width, WIDTH(line))))
    {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    widthInc = MIN(COLS(tb), width) - WIDTH(line);

    for (i = 0, pwc = (wchar_t *)BUFFER(line) + MAX(0, LENGTH(line));
         i < widthInc;
         i++, pwc++)
    {
        *pwc = L' ';
	LENGTH(line)++;
    }
    if (CLEAR_ENH(tb))
    {
        (*CLEAR_ENH(tb))(tb, row, WIDTH(line), widthInc);
    }
    _DtTermPrimBufferSetLineWidth(tb, row, WIDTH(line) + widthInc);
    if (isDebugFSet('i', 1)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	_termBufferValidateLineWc(tb, row);
    }
}

/*
** Clear the line to the new width (just reset the line width).
*/
Boolean
_DtTermPrimBufferClearLineWc
(
    const TermBuffer  tb,
    const short       row,
          short       newWidth
)
{
    TermLine        line;
    TermCharInfoRec charInfo;
    short           newLength;

    /*
    ** Some simple bounds checking.
    */
    if (!VALID_ROW(tb, row))
    {
        return(False);
    }

    /*
    ** force the width to the desired value
    **
    ** (We take the direct approach because _DtTermPrimBufferSetLineWidth
    **  doesn't allow the line width to decrease.)
    */
    line = LINE_OF_TBUF(tb, row);

    /*
    ** if this line is part of the selection, disown the selection...
    */
    if (IS_IN_SELECTION(line, MIN(newWidth, WIDTH(line)),
			MAX(newWidth, WIDTH(line))))
    {
	(void) _DtTermPrimSelectDisown(WIDGET(tb));
    }

    /*
    ** Clip the new width to the buffer width.
    */
    newWidth = MIN(newWidth, COLS(tb));

    if (newWidth < WIDTH(line))
    {
	if (newWidth == 0)
	{
	    newLength = 0;
	}
	else
	{
	    /*
	    ** handle the case of clearing the second column of a two column
	    ** character...
	    */
	    _DtTermPrimGetCharacterInfo(tb, row, MAX(0, newWidth - 1),
					&charInfo);
	    
	    if ((charInfo.width == 2 ) && 
		(charInfo.startCol == MAX(0, newWidth - 1)))
	    {
		/*
		** we are clearing column 2 of 2, replace column 1 of 1 with
		** a space...
		*/
		*charInfo.u.pwc = L' ';
	    }
	    newLength = charInfo.idx + 1;
	}
	/* 
	** Call the helper function if it exists
	*/
	if (CLEAR_LINE(tb))
	{
	    (*CLEAR_LINE(tb))(tb, row, newWidth);
	}
	WRAPPED(line) = False;
	WIDTH(line)   = newWidth;
	LENGTH(line)  = newLength;
    }
    return(True);
}

/*
** replace all characters from startCol upto stopCol with spaces,
**    
** NOTE:
**    we are dealing with double width characters, the width needs
**    to remain constant, but the length may have to change since we
**    may replace a double width character with a single width
**    character.
*/
void
_DtTermPrimBufferEraseWc
(
    TermBuffer tb,
    short      row,
    short      startCol,
    short      stopCol
)
{
    TermCharInfoRec       startCharInfo;
    TermCharInfoRec       stopCharInfo;
    short                 localCol;
    TermLine              line;
    wchar_t              *pwchar;
    short                 lengthErase;
    short                 lengthInc;
    
    /*
    ** make sure we are not trying to erase the second column
    ** of a two column character.
    */
    localCol = startCol;
    _patchUpChar(tb, row, &localCol, &startCharInfo);
    
    /*
    ** make sure we are not trying to erase the first column
    ** of a two column character.
    */
    _DtTermPrimGetCharacterInfo(tb, row, stopCol, &stopCharInfo);

    if ((stopCharInfo.width == 2) &&
	(stopCharInfo.startCol == stopCol))
    {
#ifdef	NOTDEF
	/*
	** We are about to overwrite column 1 of a 2 column
	** character.  Replace it with a space before we proceed
	** (we make adjustments later to make it look like we
	** replaced the second column with a space).
	*/
	*stopCharInfo.u.pwc = L' ';
	
	/*
	** now update the charInfo (since we replaced the 2 column
	** character with a single column space)...
	*/
	stopCharInfo.width = 1;
#endif	/* NOTDEF */
	(void) stopCol++;
    }

    /*
    ** at this point, startCharInfo points to the first character
    ** to erase, and stopCharInfo points to the last character
    ** we want to erase, make sure there is enough space between
    ** the two to accomodate the replacement spaces...
    */
    lengthErase = stopCol - startCol + 1;
    lengthInc   = lengthErase - (stopCharInfo.u.pwc - startCharInfo.u.pwc + 1);

    if (lengthInc != 0)
    {
	/*
	** the length will have to change, make the necessary adjustments
	*/
	memmove(stopCharInfo.u.pwc + lengthInc, stopCharInfo.u.pwc,
		(LENGTH(LINE_OF_TBUF(tb, row)) - stopCharInfo.idx) *
		sizeof(wchar_t));
	LENGTH(LINE_OF_TBUF(tb, row)) += lengthInc;
    }
    
    /*
    ** replace the characters with spaces...
    */
    for (pwchar = startCharInfo.u.pwc;
	 pwchar < startCharInfo.u.pwc + lengthErase;
	 pwchar++)
    {
	*pwchar = L' ';
    }
}

#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
static void
_termBufferValidateLineWc
(
    const TermBuffer tb,
    const short row
)
{
    wchar_t    *pwc;
    TermLine    line;
    
    line = LINE_OF_TBUF(tb, row);
    for (pwc = (wchar_t *)BUFFER(line);
         pwc < (wchar_t *)BUFFER(line) + LENGTH(line);
         pwc++)
    {
        if (wcwidth(*pwc) == -1)
        {
            fprintf(stderr, "_termBufferValidateLineWc: invalid wide char\n");
	    /* replace the character with a space... */
	    *pwc = L' ';
            break;
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
    wchar_t   *pChar;
    short       j;

    printf("Line: %d\n", row);

    line = LINE_OF_TBUF(tb, row);
    printf("    length: %3d\n", WIDTH(line));
    if (WIDTH(line) > 0)
    {
        printf("    buffer: <");
        pChar = BUFFER(line);
        for (j = 0; j < WIDTH(line); j++)
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

