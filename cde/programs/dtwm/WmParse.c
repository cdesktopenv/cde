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
#ifdef VERBOSE_REV_INFO
static char rcsid[] = "$XConsortium: WmParse.c /main/5 1996/10/30 11:36:06 drk $";
#endif /* VERBOSE_REV_INFO */
/******************************<+>*************************************
 **********************************************************************
 **
 **  File:        WmParse.c
 **
 **  Project:     HP/Motif Workspace Manager (dtwm)
 **
 **  Description:
 **  -----------
 **  This file contains the generic parsing routines
 **
 **
 *********************************************************************
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

/* ANSI C definitions,  This should be the first thing in WmGlobal.h */
#ifdef __STDC__
#define Const const
#else
#define Const /**/
#endif


/*
 * Included Files:
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include "WmGlobal.h"
#include "WmParse.h"
#include "WmParseP.h"
#ifdef MULTIBYTE
#include <stdlib.h>
#endif /* MULTIBYTE */
#include <ctype.h>

/*
 * Internal routines
 */

static DtWmpParseBuf * _DtWmpIncBuf (
    DtWmpParseBuf *pWmPB);



#ifndef MAXLINE
#define MAXLINE		(MAXWMPATH+1)
#endif
#define MAX_QUOTE_DEPTH	10

/*
 *  This flags non-OSF code in those sections that were lifted
 *  from mwm.
 */
#define PARSE_LIB


/*
 * Defines used to maintain code similarity between OSF/mwm source
 * routines and these routines.
 */
#define cfileP			((pWmPB)->pFile)
#define linec   		((pWmPB)->lineNumber)
#define line			((pWmPB)->pchLine)
#define parseP			((pWmPB)->pchNext)
#define ScanWhitespace(s)	(_DtWmParseSkipWhitespaceC(s))
#define PeekAhead(s,l)		(_DtWmParsePeekAhead(s,l))


/*************************************<->*************************************
 *
 *  _DtWmParseSkipWhitespace(pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Scan the current string, skipping over all white space characters.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB = ptr to parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  pWmPB = parse buffer modified; current line ptr may be moved.
 *
 *
 *  Comments:
 *  --------
 *  Assumes there's a current line in the parse buffer
 * 
 *************************************<->***********************************/

void _DtWmParseSkipWhitespace(DtWmpParseBuf *pWmPB)
{
    _DtWmParseSkipWhitespaceC (&(pWmPB->pchNext));

} /* END OF FUNCTION _DtWmParseSkipWhitespace */


/*************************************<->*************************************
 *
 *  _DtWmParseNextToken (pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Returns the next quoted or whitespace-terminated nonquoted string in the
 *  current line buffer.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= ptr to parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= ptr to null terminated string.
 *  pWmPB	= current line modified internally.
 *
 *
 *  Comments:
 *  --------
 *  May alter the line buffer contents.
 *  Handles quoted strings and characters, removing trailing whitespace from
 *  quoted strings.
 *  Returns NULL string if the line is empty or is a comment.
 *  Does not use session manager style algorithm for dealing with
 *  quoted strings.
 * 
 *************************************<->***********************************/

unsigned char *
_DtWmParseNextToken (
    DtWmpParseBuf *pWmPB 
    )
{
    return (_DtWmParseNextTokenC (&(pWmPB->pchNext), False));
}


/*************************************<->*************************************
 *
 *  _DtWmParseNextTokenExpand (pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Returns the next quoted or whitespace-terminated nonquoted string in the
 *  current line buffer. Environment variables found in the are expanded.
 *  Characters quoted by '\' are passed through unaffected with the 
 *  quoting '\' removed.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= ptr to parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= ptr to null terminated string.
 *		  Free this string with XtFree().
 *  pWmPB	= current line modified internally.
 *
 *
 *  Comments:
 *  --------
 *  May alter the line buffer contents.
 *  Handles quoted strings and characters, removing trailing whitespace from
 *  quoted strings.
 *  Returns NULL string if the line is empty or is a comment.
 * 
 *************************************<->***********************************/

unsigned char *
_DtWmParseNextTokenExpand (
    DtWmpParseBuf *pWmPB 
    )
{
    unsigned char *pch;
    unsigned char *pchReturn = NULL;

    /* isolate the next token */
    pch = _DtWmParseNextTokenC (&(pWmPB->pchNext), False);

    /* expand environment variables, a copy of the string is returned */
    pchReturn = _DtWmParseExpandEnvironmentVariables (pch, NULL);

    /* 
     * If a token was found, but no copy returned, there were no 
     * environment variables. This routine needs to return a copy, 
     * so make one now.
     */
    if (pch && !pchReturn) 
	pchReturn = (unsigned char *) XtNewString ((String) pch);

    return (pchReturn);
}


/*************************************<->*************************************
 *
 *  _DtWmParseBackUp (pWmPB, pchTok)
 *
 *
 *  Description:
 *  -----------
 *  Backs up to the previous token (the one before pchTok) 
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= ptr to parse buffer
 *  pchTok	= ptr to a token in the parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Returns	= ptr to prev token
 *
 *
 *  Comments:
 *  --------
 *  Operates on the line buffer in the pWmPB structure. Backs up
 *  the next pointer and writes a space over the interpolated
 *  NULL (if any).
 * 
 *************************************<->***********************************/

unsigned char *
_DtWmParseBackUp (
    DtWmpParseBuf *pWmPB,
    unsigned char *pchTok
    )
{
    if ((pchTok > pWmPB->pchLine) &&
	(pchTok < (pWmPB->pchLine +  pWmPB->cLineSize)))
    {
#ifdef MULTIBYTE
	unsigned char *pch;
	unsigned char *pchLast;
	int chlen;

	pch = pchLast = pWmPB->pchLine;

	/*
	 * Search from beginning (because of multibyte chars) to
	 * find the token before the string we're interested in.
	 */
	while ((pch < pchTok))
	{
	    chlen = mblen ((char *)pch, MB_CUR_MAX);
	    if (*pch == '\0')
	    {
		pch++;
		if (pch == pchTok)
		{
		    /*
		     * Found the NULL preceding the string passed in!
		     * Replace it with a blank and return the previous
		     * token (pointed to by pchLast).
		     */
		    *(pch - 1) = DTWM_CHAR_SPACE;
		    break;
		}
		else
		{
		    /* 
		     * Remember the beginning of this token.
		     */
		    pchLast = pch;
		}
	    }
	    else if (chlen < 1)
	    {
		break;
	    }
	    else
	    {
		pch += chlen;
	    }
	}

	pWmPB->pchNext = pchLast;
#else /* MULTIBYTE */

	/* 
	 * Replace preceding NULL with a space.
	 */
        pchTok--;

	if (*pchTok == '\0')
	{
	    *pchTok = DTWM_CHAR_SPACE;
	}

	/* 
	 * Back up to next NULL or beginning of line.
	 */
	while ((pchTok >= pWmPB->pchLine) && *pchTok)
	{
	    pchTok--;
	}

	pWmPB->pchNext = pchTok + 1;

#endif  /* MULTIBYTE */
    }
    return (pWmPB->pchNext);
}



/*************************************<->*************************************
 *
 *  _DtWmParseSkipWhitespaceC(linePP)
 *
 *
 *  Description:
 *  -----------
 *  Scan the string, skipping over all white space characters.
 *
 *
 *  Inputs:
 *  ------
 *  linePP = nonNULL pointer to current line buffer pointer
 *
 * 
 *  Outputs:
 *  -------
 *  linePP = nonNULL pointer to revised line buffer pointer
 *
 *
 *  Comments:
 *  --------
 *  Assumes linePP is nonNULL
 * 
 *************************************<->***********************************/

void _DtWmParseSkipWhitespaceC(unsigned char  **linePP)
{
#ifdef MULTIBYTE
    while (*linePP && 
	   (mblen ((char *)*linePP, MB_CUR_MAX) == 1) && 
	   isspace (**linePP))
#else
    while (*linePP && isspace (**linePP))
#endif
    {
        (*linePP)++;
    }

} /* END OF FUNCTION _DtWmParseSkipWhitespaceC */



/*************************************<->*************************************
 *
 *  _DtWmParseNextTokenC (linePP, SmBehavior)
 *
 *
 *  Description:
 *  -----------
 *  Returns the next quoted or whitespace-terminated nonquoted string in the
 *  line buffer.
 *  Additional functionality added to GetString in that anything in a
 *  quoted string is considered sacred and nothing will be stripped from
 *  the middle of a quoted string.
 *
 *
 *  Inputs:
 *  ------
 *  linePP =  pointer to current line buffer pointer.
 *  SmBehavior = flag that enables parsing session manager hints  if True.
 *               If False, this behaves as the normal OSF mwm GetString
 *               routine.
 *
 * 
 *  Outputs:
 *  -------
 *  linePP =  pointer to revised line buffer pointer.
 *  Return =  string 
 *
 *
 *  Comments:
 *  --------
 *  May alter the line buffer contents.
 *  Handles quoted strings and characters, removing trailing whitespace from
 *  quoted strings.
 *  Returns NULL string if the line is empty or is a comment.
 *  Code stolen from dtmwm.
 * 
 *************************************<->***********************************/

unsigned char *
_DtWmParseNextTokenC (
    unsigned char **linePP, 
    Boolean SmBehavior 
    )
{
/***********************************************************************
 *
 * The following code is duplicated from WmResParse.c (GetStringC)
 * GetStringC is the HP DT version of GetString. 
 *
 * It works here through the magic of #defines.
 *
 ***********************************************************************/
    unsigned char *lineP = *linePP;
    unsigned char *endP;
    unsigned char *curP;
    unsigned char *lnwsP;
    unsigned int  level = 0, checkLev, i, quoteLevel[MAX_QUOTE_DEPTH];
#ifdef MULTIBYTE
    int            chlen;

    /* get rid of leading white space */
    ScanWhitespace (&lineP);

    /*
     * Return NULL if line is empty, whitespace, or begins with a comment.
     */
    if (
	*lineP == '\0' ||
	((chlen = mblen ((char *)lineP, MB_CUR_MAX)) < 1) ||
        ((chlen == 1) && ((*lineP == '!') || 
			  ((!SmBehavior) && (*lineP == '#'))))
       )
    {
        *linePP = lineP;
        return (NULL);
    }

    if ((chlen == 1) && (*lineP == '"'))
    /* Quoted string */
    {
	quoteLevel[level] = 1;	
	/*
	 * Start beyond double quote and find the end of the quoted string.
	 * '\' quotes the next character.
	 * Otherwise,  matching double quote or NULL terminates the string.
	 *
	 * We use lnwsP to point to the last non-whitespace character in the
	 * quoted string.  When we have found the end of the quoted string,
	 * increment lnwsP and if lnwsP < endP, write NULL into *lnwsP.
	 * This removes any trailing whitespace without overwriting the 
	 * matching quote, needed later.  If the quoted string was all 
	 * whitespace, then this will write a NULL at the beginning of the 
	 * string that will be returned -- OK.
	 */
	lnwsP = lineP++;                /* lnwsP points to first '"' */
	curP = endP = lineP;            /* other pointers point beyond */

        while ((*endP = *curP) &&
               ((chlen = mblen ((char *)curP, MB_CUR_MAX)) > 0) &&
	       ((chlen > 1) || (*curP != '"')))
	/* Haven't found matching quote yet.
	 * First byte of next character has been copied to endP.
	 */
        {
	    curP++;
	    if ((chlen == 1) && (*endP == '\\') && 
		((chlen = mblen ((char *)curP, MB_CUR_MAX)) > 0))
	    /* character quote:
	     * copy first byte of quoted nonNULL character down.
	     * point curP to next byte
	     */
	    {
		if (SmBehavior)
		{
		    /*
		     * Check to see if this is a quoted quote - if it is
		     * strip off a level - if not - it's sacred leave it alone
		     */
		    checkLev = PeekAhead((curP - 1), quoteLevel[level]);
		    if(checkLev > 0)
		    {
			if(quoteLevel[level] >= checkLev)
			{
			    if (level > 0) level--;
			}
			else if (level < MAX_QUOTE_DEPTH)
			{
			    level++;
			    quoteLevel[level] = checkLev;
			}
			
			for(i = 0;i < (checkLev - 2);i++)
			{
			    *endP++ = *curP++;curP++;
			}
			*endP = *curP++;
		    }
		}
		else 
		{
		*endP = *curP++;
		}
            }

	    if (chlen == 1)
	    /* Singlebyte character:  character copy finished. */
	    {
	        if (isspace (*endP))
	        /* whitespace character:  leave lnwsP unchanged. */
	        {
	            endP++;
	        }
	        else
	        /* non-whitespace character:  point lnwsP to it. */
	        {
	            lnwsP = endP++;
	        }
	    }
	    else if (chlen > 1)
	    /* Multibyte (nonwhitespace) character:  point lnwsP to it.
	     * Finish character byte copy.
	     */
	    {
	        lnwsP = endP++;
		while (--chlen)
		{
		    *endP++ = *curP++;
		    lnwsP++;
		}
	    }
        }
#else /* MULTIBYTE */

    /* get rid of leading white space */
    ScanWhitespace (&lineP);

    /* Return NULL if line is empty, whitespace, or begins with a comment. */
    if ((lineP == NULL || *lineP == '\0') ||
	(!SmBehavior && (*lineP == '#')))
    {
        *linePP = lineP;
        return (NULL);
    }

    if (*lineP == '"')
    /* Quoted string */
    {
	quoteLevel[level] = 1;	
	/*
	 * Start beyond double quote and find the end of the quoted string.
	 * '\' quotes the next character.
	 * Otherwise,  matching double quote or NULL terminates the string.
	 *
	 * We use lnwsP to point to the last non-whitespace character in the
	 * quoted string.  When we have found the end of the quoted string,
	 * increment lnwsP and if lnwsP < endP, write NULL into *lnwsP.
	 * This removes any trailing whitespace without overwriting the 
	 * matching quote, needed later.  If the quoted string was all 
	 * whitespace, then this will write a NULL at the beginning of the 
	 * string that will be returned -- OK.
	 */
	lnwsP = lineP++;                /* lnwsP points to first '"' */
	curP = endP = lineP;            /* other pointers point beyond */

        while ((*endP = *curP) && (*endP != '"'))
	/* haven't found matching quote yet */
        {
	    /* point curP to next character */
	    curP++;
	    if ((*endP == '\\') && (*curP != NULL))
	    /* shift quoted nonNULL character down and curP ahead */
	    {
		if (SmBehavior)
		{
		    /*
		     * Check to see if this is a quoted quote - if it is
		     * strip off a level - if not - it's sacred leave it alone
		     */
		    checkLev = PeekAhead((curP - 1), quoteLevel[level]);
		    if(checkLev > 0)
		    {
			if(quoteLevel[level] >= checkLev)
			{
			    if (level > 0) level--;
			}
			else if (level < MAX_QUOTE_DEPTH)
			{
			    level++;
			    quoteLevel[level] = checkLev;
			}
			
			for(i = 0;i < (checkLev - 2);i++)
			{
			    *endP++ = *curP++;curP++;
			}
			*endP = *curP++;
		    }
		}
		else 
		{
		*endP = *curP++;
		}
            }
	    if (isspace (*endP))
	    /* whitespace character:  leave lnwsP unchanged. */
	    {
	        endP++;
	    }
	    else
	    /* non-whitespace character:  point lnwsP to it. */
	    {
	        lnwsP = endP++;
	    }
        }
#endif /* MULTIBYTE */

	/*
	 *  Found matching quote or NULL.  
	 *  NULL out any trailing whitespace.
	 */

	lnwsP++;
	if (lnwsP < endP)
        {
	    *lnwsP = '\0';
        }
    }

    else
    /* Unquoted string */
    {
        /* 
	 * Find the end of the nonquoted string.
	 * '\' quotes the next character.
	 * Otherwise,  whitespace, NULL, or '#' terminates the string.
	 */
        curP = endP = lineP;

#ifdef MULTIBYTE
        while ((*endP = *curP) &&
               ((chlen = mblen ((char *)curP, MB_CUR_MAX)) > 0) &&
               ((chlen > 1) || (!isspace (*curP) && 
			        (SmBehavior || (*curP != '#')))))
	/* Haven't found whitespace or '#' yet.
	 * First byte of next character has been copied to endP.
	 */
        {
	    curP++;
	    if ((chlen == 1) && (*endP == '\\') && 
		((chlen = mblen ((char *)curP, MB_CUR_MAX)) > 0))
	    /* character quote:
	     * copy first byte of quoted nonNULL character down.
	     * point curP to next byte
	     */
	    {
		*endP = *curP++;
            }
	    endP++;
	    if (chlen > 1)
	    /* Multibyte character:  finish character copy. */
	    {
		while (--chlen)
		{
		    *endP++ = *curP++;
		}
	    }
        }
#else /* MULTIBYTE */
        while ((*endP = *curP) && !isspace (*endP) && 
					(SmBehavior || (*endP != '#')))
        {
	    /* point curP to next character */
	    curP++;
	    if ((*endP == '\\') && (*curP != '\0'))
	    /* shift quoted nonNULL character down and curP ahead */
	    {
		*endP = *curP++;
            }
	    endP++;
        }
#endif /* MULTIBYTE */
    }

    /*
     * Three cases for *endP:
     *   '#' --> write NULL over # and point to NULL
     *   whitespace or
     *     matching quote -> write NULL over char and point beyond
     *   NULL -> point to NULL 
     */

    if (!SmBehavior && (*endP == '#'))
    {
	*endP = '\0';       /* write NULL over '#' */
	*linePP = endP;     /* point to NULL */
    }
    else if (*endP != '\0')
    {
	*endP = '\0';       /* write NULL over terminator */
	*linePP = ++curP;   /* point beyond terminator */
    }
    else
    {
	*linePP = endP;
    }
    return ((unsigned char *)lineP);

} /* END OF FUNCTION _DtWmParseNextTokenC */



/*************************************<->*************************************
 *
 *  (DtWmParseBuf *) _DtWmParseNewBuf (void)
 *
 *
 *  Description:
 *  -----------
 *  Allocates a new parse record for parsing. 
 *
 *  Inputs:
 *  ------
 *  none 
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= ptr to parse buffer record, NULL if memory allocation
 *		  error.
 *
 *
 *  Comments:
 *  --------
 *  Call this first before using the other DtWmp routines that require
 *  a parse buffer. Treat this as an opaque type; use the provided 
 *  routines to create, access, and destroy this structure.
 * 
 *************************************<->***********************************/

DtWmpParseBuf * 
_DtWmParseNewBuf ( void )

{
    DtWmpParseBuf *pWmPB;

    pWmPB = (DtWmpParseBuf *) XtMalloc (sizeof (DtWmpParseBuf));
    if (pWmPB)
    {
	pWmPB->pchLine = (unsigned char *) XtMalloc (MAXLINE+1);
	if (!pWmPB->pchLine)
	{
	    XtFree ((char *)pWmPB);
	    pWmPB = NULL;
	}
    }

    if (pWmPB)
    {
	pWmPB->lineNumber = 0;
	pWmPB->pchNext = pWmPB->pchLine;
	pWmPB->cLineSize = MAXLINE+1;
	pWmPB->pFile = NULL;
	*(pWmPB->pchLine) = '\0';
    }

    return (pWmPB);

} /* END OF FUNCTION _DtWmParseNewBuf */


/*************************************<->*************************************
 *
 *  (DtWmParseBuf *) _DtWmpIncBuf (pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Increases the size of the line buffer in the parse buffer
 *
 *  Inputs:
 *  ------
 *  pWmPB 	= pointer to a parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= ptr to parse buffer record, NULL if memory allocation
 *		  error.
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

static DtWmpParseBuf * 
_DtWmpIncBuf (
    DtWmpParseBuf *pWmPB)

{
    if (pWmPB)
    {
	int ix;

	if (pWmPB->pFile)
	{
	    /* save index into old string */
	    ix = pWmPB->pchNext - pWmPB->pchLine;
	}

	pWmPB->pchLine = (unsigned char *) 
	    XtRealloc ((char *)pWmPB->pchLine, (pWmPB->cLineSize + MAXLINE));
	if (pWmPB->pchLine)
	{
	    pWmPB->cLineSize += MAXLINE;

	    if (pWmPB->pFile)
	    {
		/* restore index into new string */
		pWmPB->pchNext = pWmPB->pchLine + ix;
	    }
	}
    }

    return (pWmPB);

} /* END OF FUNCTION _DtWmpIncBuf */


/*************************************<->*************************************
 *
 *  _DtWmParseDestroyBuf (pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Destroys a parse buffer record, freeing any allocated memory.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= ptr to previously allocated parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  none
 *
 *
 *  Comments:
 *  --------
 *  Destroys parse buffers allocated by _DtWmParseNewBuf.
 * 
 *************************************<->***********************************/

void 
_DtWmParseDestroyBuf (
    DtWmpParseBuf *pWmPB
    )

{
    if (pWmPB)
    {
	if (pWmPB->pchLine)
	{
	    XtFree ((char *) pWmPB->pchLine);
	}
	XtFree ((char *) pWmPB);
    }

} /* END OF FUNCTION _DtWmParseDestroyBuf */


/*************************************<->*************************************
 *
 *  (unsigned char *) _DtWmParseSetLine (pWmPB, pch)
 *
 *
 *  Description:
 *  -----------
 *  Sets a line into the parse buffer structure. This is used in cases
 *  where parsing of an embedded string, usually a default, is done
 *  instead of parsing out of a file.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= previously allocated parse buffer 
 *  pch		= ptr to unsigned char string (zero terminated)
 * 
 *  Outputs:
 *  -------
 *  Return	= pch
 *
 *
 *  Comments:
 *  --------
 *  This resets any previous setting of the file pointer. EOF wil be 
 *  returned when the string pointed to by pch is exhausted.
 *
 *  Resets line number count.
 * 
 *************************************<->***********************************/

void 
_DtWmParseSetLine (
    DtWmpParseBuf  *pWmPB,
    unsigned char  *pch
    )

{
    if (pWmPB)
    {
	pWmPB->pchLine = pch;
	pWmPB->pchNext = pWmPB->pchLine;
	pWmPB->pFile = NULL;
	pWmPB->lineNumber = 0;
    }

} /* END OF FUNCTION _DtWmParseSetLine */


/*************************************<->*************************************
 *
 *  (FILE *) _DtWmParseSetFile (pWmPB, pFile)
 *
 *
 *  Description:
 *  -----------
 *  Sets the file pointer in a parse buffer. This is used when parsing 
 *  from a file is required. 
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= pointer to a parse buffer
 *  pFile	= pointer to an opened FILE
 * 
 *  Outputs:
 *  -------
 *  Return	= pFile
 *
 *
 *  Comments:
 *  --------
 *  You fopen the file first, then pass in the FILE * returned to this
 *  routine.
 *
 *  Resets line number count.
 * 
 *************************************<->***********************************/

void 
_DtWmParseSetFile (
    DtWmpParseBuf *pWmPB,
    FILE *pFile
    )

{
    if (pWmPB)
    {
	pWmPB->pchLine[0] = '\0';
	pWmPB->pchNext = NULL;
	pWmPB->pFile = pFile;
	pWmPB->lineNumber = 0;
    }

} /* END OF FUNCTION _DtWmParseSetFile */


/*************************************<->*************************************
 *
 *  (unsigned char *) _DtWmParseNextLine ( pWmPB )
 *
 *
 *  Description:
 *  -----------
 *  Returns a pointer to the next line to parse.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= pointer to a parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= pointer to next line to parse or NULL on EOF.
 *
 *
 *  Comments:
 *  --------
 *  
 * 
 *************************************<->***********************************/

unsigned char * 
_DtWmParseNextLine (
    DtWmpParseBuf *pWmPB
    )

{
/***********************************************************************
 *
 * The following code is duplicated from WmResParse.c (GetNextLine)
 * It works here through the magic of #defines.
 *
 ***********************************************************************/

    register unsigned char	*string;
    int				len;

#ifdef MULTIBYTE
    int   chlen;
    wchar_t last;
    wchar_t wdelim;
    char delim;
    int lastlen;
#endif

    if (cfileP != NULL)
    /* read fopened file */
    {
	if ((string = (unsigned char *) 
		      fgets ((char *)line, MAXLINE, cfileP)) != NULL)
	{
#ifdef PARSE_LIB
	    if (strlen((char *)string) > (size_t)pWmPB->cLineSize)
	    {
		/*
		 * Bump size of destination buffer
		 */
		pWmPB->cLineSize = 1 + strlen((char *)string);
		pWmPB->pchLine = (unsigned char *) 
		    XtRealloc ((char *)pWmPB->pchLine, 
			       (pWmPB->cLineSize));
	    }
#endif /* PARSE_LIB */
#ifndef NO_MULTIBYTE

	    lastlen = 0;
	    while (*string &&
		   ((len = mblen((char *)string, MB_CUR_MAX)) > 0))
	    {
		mbtowc(&last, (char *)string, MB_CUR_MAX);
		lastlen = len;
		string += len;
	    }
	    delim = '\\';
	    mbtowc(&wdelim, &delim, MB_CUR_MAX);
	    if (lastlen == 1 && last == wdelim)
	    {
		do
		{
		    if (!fgets((char *)string, MAXLINE - (string - line), cfileP))
			break;

		    lastlen = 0;
		    while (*string &&
			   ((len = mblen((char *)string, MB_CUR_MAX)) > 0))
		    {
			mbtowc(&last, (char *)string, MB_CUR_MAX);
			lastlen = len;
			string += len;
		    }
		    linec++;
		}
		while (lastlen == 1 && last == wdelim);
	    }
	    string = line;
#else
	    len = strlen((char *)string) - 2;
	    if ((len > 0) && string[len] == '\\')
	    {
		do {
		    string = &string[len];
		    if (fgets((char *)string, 
		 	      MAXLINE - (string-line), cfileP) == NULL)
		       break;
		    len = strlen((char *)string) - 2;
		    linec++;
		} while ((len >= 0) && string[len] == '\\');
		string = line;
	    }
#endif
	}
    }
    else if ((parseP != NULL) && (*parseP != '\0'))
    /* read parse string */
    {
#ifdef PARSE_LIB
        if (strlen((char *)parseP) > (size_t)pWmPB->cLineSize)
	{
	    /*
	     * Bump size of destination buffer
	     */
	    pWmPB->cLineSize = 1 + strlen((char *)parseP);
	    pWmPB->pchLine = (unsigned char *) 
		XtRealloc ((char *)pWmPB->pchLine, 
			   (pWmPB->cLineSize));
	}
#endif /* PARSE_LIB */
	string = line;
#ifdef MULTIBYTE
	while ((*parseP != '\0') &&
               ((chlen = mblen ((char *)parseP, MB_CUR_MAX)) != 0) &&
	       (*parseP != '\n'))
	/* copy all but end-of-line and newlines to line buffer */
	{
            if (chlen == -1)
	       *(parseP)++;
	    else
            {
                while (chlen--)
                {
                   *(string++) = *(parseP++);
                }
            }
        }
#else
	while ((*parseP != '\0') && (*parseP != '\n'))
	/* copy all but end-of-line and newlines to line buffer */
	{
	    *(string++) = *(parseP++);
        }
#endif
	*string = '\0';
	if (*parseP == '\n')
	{
	    parseP++;
	}
    }
    else
    {
	string = NULL;
    }

    linec++;
#ifdef PARSE_LIB
    if (cfileP)
    {
	/* update pchNext to get next line */
	pWmPB->pchNext = string;
    }
#endif /* PARSE_LIB */
    return (string);


} /* END OF FUNCTION _DtWmParseNextLine */


/*************************************<->*************************************
 *
 *  (unsigned char *) _DtWmParseCurrentChar (pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Returns a pointer to the rest of the current line.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= pointer to a parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= pointer to the rest of the current line
 *
 *
 *  Comments:
 *  --------
 *  Useful in cases where you want to look at a char before getting the
 *  next token or if you want to treat the rest of the line as a
 *  single token.
 * 
 *************************************<->***********************************/

unsigned char * 
_DtWmParseCurrentChar (
    DtWmpParseBuf *pWmPB
    )

{
    return (pWmPB ? pWmPB->pchNext : (unsigned char *)NULL);

} /* END OF FUNCTION _DtWmParseCurrentChar */



/*************************************<->*************************************
 *
 *  (unsigned char *) _DtWmParseNextChar (pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Advances the pointer to the next char and returns a pointer
 *  to the new current char.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= pointer to a parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= pointer to the rest of the current line
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

unsigned char * 
_DtWmParseNextChar (
    DtWmpParseBuf *pWmPB
    )

{
    unsigned char *pch = NULL;
    int chlen;

#ifdef MULTIBYTE
    if (pWmPB &&
	pWmPB->pchNext &&
	(chlen = mblen((char *)pWmPB->pchNext, MB_CUR_MAX) > 0))
    {
	pch = (pWmPB->pchNext += chlen);
    }
#else /* MULTIBYTE */
    if (pWmPB && pWmPB->pchNext && *pWmPB->pchNext)
    {
	pch = ++pWmPB->pchNext;
    }
#endif /* MULTIBYTE */

    return (pch);
}


/*************************************<->*************************************
 *
 *  (int) _DtWmParseLineNumber (pWmPB)
 *
 *
 *  Description:
 *  -----------
 *  Returns the number of the current line of what's being parsed.
 *
 *
 *  Inputs:
 *  ------
 *  pWmPB	= ptr to parse buffer
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= number of current line
 *
 *
 *  Comments:
 *  --------
 *  Used for error reporting.
 *
 *  The line number is computed by counting '\n' characters.
 * 
 *************************************<->***********************************/

int 
_DtWmParseLineNumber (
    DtWmpParseBuf *pWmPB
    )

{
    return (pWmPB ? pWmPB->lineNumber : 0);

} /* END OF FUNCTION _DtWmParseLineNumber */


/*************************************<->*************************************
 *
 *  _DtWmParseToLower (string)
 *
 *
 *  Description:
 *  -----------
 *  Lower all characters in a string.
 *
 *
 *  Inputs:
 *  ------
 *  string = NULL-terminated character string or NULL
 *
 * 
 *  Outputs:
 *  -------
 *  string = NULL-terminated lower case character string or NULL
 *
 *
 *  Comments:
 *  --------
 *  Can handle multi-byte characters
 * 
 *************************************<->***********************************/

void _DtWmParseToLower (unsigned char  *string)
{
    unsigned char *pch = string;
#ifdef MULTIBYTE
    int            chlen;

    while ((chlen = mblen ((char *)pch, MB_CUR_MAX)) > 0)
    {
        if ((chlen == 1) && (isupper (*pch)))
	{
	    *pch = tolower(*pch);
	}
	pch += chlen;
    }
#else
    while (*pch != NULL)
    {
        if (isupper (*pch))
	{
	    *pch = tolower(*pch);
	}
	pch++;
    }
#endif

} /* END OF FUNCTION _DtWmParseToLower */



/*************************************<->*************************************
 *
 *  _DtWmParsePeekAhead (currentChar, currentLev)
 *
 *
 *  Description:
 *  -----------
 *  Returns a new level value if this is a new nesting level of quoted string
 *  Otherwise it returns a zero
 *
 *
 *  Inputs:
 *  ------
 *  currentChar = current position in the string
 *  currentLev = current level of nesting
 *
 * 
 *  Outputs:
 *  -------
 *  Returns either a new level of nesting or zero if the character is copied in
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
unsigned int _DtWmParsePeekAhead(unsigned char *currentChar,
		       unsigned int currentLev)


{
    Boolean		done = False;
    unsigned int 	tmpLev = 1;
#ifdef MULTIBYTE
    unsigned int	chlen;

    while (((chlen = mblen ((char *)currentChar, MB_CUR_MAX)) > 0) &&
	   (chlen == 1) && ((*currentChar == '"') || (*currentChar == '\\'))
	   && (done == False))
    {
	currentChar++;

	if(((chlen = mblen ((char *)currentChar, MB_CUR_MAX)) > 0) && 
	   (chlen == 1) &&
	   ((*currentChar == '"') || (*currentChar == '\\')))
	{
	    tmpLev++;
	    if(*currentChar == '"')
	    {
		done = True;
	    }
	    else
	    {
		currentChar++;
	    }
	}
    }
#else
    while((*currentChar != NULL) && (done == False) &&
	  ((*currentChar == '"') || (*currentChar == '\\')))
    {
	currentChar++;
	if((*currentChar != NULL) &&
	   ((*currentChar == '"') || (*currentChar == '\\')))
	{
	    tmpLev++;
	    if(*currentChar == '"')
	    {
		done = True;
	    }
	    else
	    {
		currentChar++;
	    }
	}
    }
#endif /*MULTIBYTE*/

    /*
     * Figure out if this is truly a new level of nesting - else ignore it
     * This section probably could do some error checking and return -1
	 * If so, change type of routine from unsigned int to int
     */
    if(done == True)
    {
	return(tmpLev);
    }
    else
    {
	return(0);
    }
} /* END OF FUNCTION _DtWmParsePeekAhead */



/*************************************<->*************************************
 *
 *  (unsigned char *) _DtWmParseFilenameExpand (pchFilename)
 *
 *
 *  Description:
 *  -----------
 *  Returns a copy of a file name with environment variables
 *  expanded.
 *
 *
 *  Inputs:
 *  ------
 *  pchFilename	= ptr to a zero terminated character string (filename)
 *
 * 
 *  Outputs:
 *  -------
 *  Return	= ptr to a new file name with environment variables 
 *                expanded.
 *
 *
 *  Comments:
 *  --------
 *  The passed in string is temporarily modified inside here.
 * 
 *  Free the returned string with XtFree().
 *
 *  Returns NULL on a memory allocation error.
 *
 *  Environment variables that can't be expanded are removed from
 *  the returned copy.
 *
 *  If no environment variables, you get a copy of the string back.
 * 
 *************************************<->***********************************/
unsigned char *
_DtWmParseFilenameExpand (
    unsigned char *pchFilename
    )

{
    unsigned char *pchN, *pchNew, *pchO;
    unsigned char *pchEnv, *pchEnv0, *pchEnv1;
    unsigned char chSave;
    int len, n, nx, ix;
    unsigned char pchBrk[] = { DTWM_CHAR_ENVIRONMENT,
			       DTWM_CHAR_DIRECTORY,
			       '\0'
			     };

    len = strlen ((char *)pchFilename);
    pchNew = (unsigned char *) XtMalloc (1+len);
    pchO = pchFilename;
    chSave = '\0';
    ix = 0;

    while (pchNew && pchO && *pchO)
    {
	/* find next environment variable */
	pchEnv0 = (unsigned char *) 
		strchr ((char *)pchO, (int) DTWM_CHAR_ENVIRONMENT);
	if (pchEnv0)
	{
	    /* length to this point */
	    n = pchEnv0 - pchO;

	    /* copy up to environment character */
	    if (n) 
	    {
		memcpy (&pchNew[ix], pchO, n);
		ix += n;
	    }

	    /* skip environment character */
	    pchEnv0++;	

	    /* end of variable is at one of:
	     *   start of next variable,
	     *   start of next directory,
	     *   end of string 
	     */
	    pchEnv1 = (unsigned char *) 
			strpbrk ((char *)pchEnv0, (char *)pchBrk);

	    if (pchEnv1)
	    {
		/* next string starts after this one */
		pchO = pchEnv1; 
		n = pchEnv1 - pchEnv0 + 1;

		/* replace this char with NULL for now */
		chSave = *pchEnv1;
		*pchEnv1 = '\0';
	    }
	    else
	    {
		/* This environment variable is the last thing on
		 * the line. Signal all done.
		 */
		n = strlen ((char *) pchO);
		pchO += n;
	    }

	    pchEnv = (unsigned char *) getenv ((char *)pchEnv0);
	    if (pchEnv) 
	    {
	        nx = strlen ((char *) pchEnv);
	        if (nx > n)
	        {
		    len += nx - n;
		    pchNew = (unsigned char *) 
			XtRealloc ((char *)pchNew, 1+len);
		}
		if (pchNew)
		{
		    memcpy (&pchNew[ix], pchEnv, nx);
		    ix += nx;
		}
		else 
		{
		    continue;
		}
	    }

	    if (chSave)
	    {
		*pchO = chSave;
		chSave = '\0';
	    }
	    /* keep a kosher string */
	    pchNew[ix] = '\0';
	}
	else
	{
	    /* copy the rest of the string */
	    n = strlen ((char *) pchO);
	    memcpy (&pchNew[ix], pchO, n);
	    pchO += n;

	    /* remember the NULL! (a famous battle cry) */
	    pchNew[ix + n] = '\0';
	}
    }

    return (pchNew);

} /* END OF FUNCTION _DtWmParseFilenameExpand */


/*************************************<->*************************************
 *
 *  unsigned char * _DtWmParseExpandEnvironmentVariables (pch, pchBrk)
 *
 *
 *  Description:
 *  -----------
 *  Expands environment variables in a string.
 *
 *
 *  Inputs:
 *  ------
 *  pch   = ptr to a zero terminated character string 
 *  pchBrk = array of "break" characters (see strpbrk()).
 * 	     defaults are used if this is NULL.
 * 
 *  Outputs:
 *  -------
 *  Return = string with expanded environment variables. (free with XtFree)
 *	     NULL string if no environment variables or backslashes
 *	     found in the string passed in.
 *
 *
 *  Comments:
 *  --------
 *  Free returned string with XtFree()
 *
 *  Environment variables that can't be expanded are removed from
 *  the returned copy.
 *
 *  Default delimiter set is [Space], [Tab], '$', [Newline], '\', '/'.
 *
 *  Variables of form $(..) and ${..} supported.
 *
 *  A backslash '\' in front of any character quotes it. The backslash 
 *  is removed in the returned string. A literal backslash needs to be
 *  quoted with a backslash.
 * 
 *************************************<->***********************************/
unsigned char *
_DtWmParseExpandEnvironmentVariables (
    unsigned char *pch,
    unsigned char *pchBrk
    )

{
#ifdef MULTIBYTE
    int chlen;
#else /* MULTIBYTE */
    int chlen = 1;	/* length of character is always '1' */
#endif /* MULTIBYTE */
    unsigned char *pchStart;
    unsigned char chSave;
    unsigned char *pchEnvStart;
    unsigned char *pchEnvValue;
    unsigned char *pchReturn = NULL;
    unsigned char *pchNext;
    unsigned char *pchBreak;
    Boolean bEatBreak;
    Boolean bAlreadyAdvanced;
    int lenOriginal;
    int lenNonEnv;
    int lenEnvVar;
    int lenEnvValue;
    int lenReturn;
    int lenSave;
    static unsigned char pchDefaultBrk[] = { 
				DTWM_CHAR_ENVIRONMENT,
				DTWM_CHAR_SPACE,
				DTWM_CHAR_TAB,
				DTWM_CHAR_NEW_LINE,
				DTWM_CHAR_BACKSLASH,
			        DTWM_CHAR_DIRECTORY,
				'\0' };
    unsigned char pchParenBrk[] = {   
				DTWM_CHAR_R_PAREN,
				'\0' };
    unsigned char pchBraceBrk[] = {   
				DTWM_CHAR_R_BRACE,
				'\0' };

    /* There needs to be something to look at */
    if (!pch)
	return (NULL);

    pchStart = pch;
    lenOriginal = strlen ((char *)pch);
#ifdef MULTIBYTE
    chlen = mblen ((char *)pch, MB_CUR_MAX);
#endif /* MULTIBYTE */
    chSave = '\0';

    while (*pch && (chlen > 0))
    {
	if (chlen == 1)
	{
	    bAlreadyAdvanced = False;
	    switch (*pch)
	    {
		case DTWM_CHAR_BACKSLASH:
		    /*
		     * Copy up to start of quoted char
		     */
		    if (!pchReturn)
		    {
			lenReturn = lenOriginal + 1;
			pchReturn = (unsigned char *) 
			    XtMalloc (lenReturn * sizeof (unsigned char));
			pchReturn[0] = '\0';
		    }
		    chSave = *pch;
		    *pch = '\0';
		    strcat ((char *) pchReturn, (char *)pchStart);
		    *pch = chSave;
		    chSave = '\0';

		    /* 
		     * The next character is "escaped", skip over it.
		     */
		    pchStart = pch += chlen;
#ifdef MULTIBYTE
		    chlen = mblen ((char *)pch, MB_CUR_MAX);
#endif /* MULTIBYTE */
		    break;

		case DTWM_CHAR_ENVIRONMENT:
		    /* save start of environment variable */
		    pchEnvStart = pch;
		    pch += chlen;
#ifdef MULTIBYTE
		    chlen = mblen ((char *)pch, MB_CUR_MAX);
#endif /* MULTIBYTE */

		    /*
		     * Copy up to start of environment variable 
		     */
		    if (!pchReturn)
		    {
			lenReturn = lenOriginal + 1;
			pchReturn = (unsigned char *) 
			    XtMalloc (lenReturn * sizeof (unsigned char));
			pchReturn[0] = '\0';
			lenSave = 0;
		    }
		    else
		    {
			lenSave = strlen ((char *)pchReturn);
		    }
		    lenNonEnv = pchEnvStart - pchStart;
		    memcpy (&pchReturn[lenSave], pchStart, lenNonEnv);
		    pchReturn[lenSave+lenNonEnv] = '\0';

		    /* 
		     * Determine how we find the end of this 
		     * environment variable.
		     */
		    bEatBreak = False;
		    if ((chlen == 1)  &&
			(*pch == DTWM_CHAR_L_PAREN))
		    {
			pch += chlen;
#ifdef MULTIBYTE
			chlen = mblen ((char *)pch, MB_CUR_MAX);
#endif /* MULTIBYTE */
			pchBreak = pchParenBrk;
			bEatBreak = True;
		    }
		    else if ((chlen == 1)  &&
			(*pch == DTWM_CHAR_L_BRACE))
		    {
			pch += chlen;
#ifdef MULTIBYTE
			chlen = mblen ((char *)pch, MB_CUR_MAX);
#endif /* MULTIBYTE */
			pchBreak = pchBraceBrk;
			bEatBreak = True;
		    }
		    else if (pchBrk && *pchBrk)
		    {
			pchBreak = pchBrk;
		    }
		    else
		    {
			pchBreak = pchDefaultBrk;
		    }

		    /* 
		     * Look for end of environment variable
		     */
		    pchNext = (unsigned char *)
			    strpbrk ((char *)pch, (char *)pchBreak);

		    if (!pchNext)
		    {
			/* it's the rest of the string */
			chSave = NULL;
			bEatBreak = False;
			pchNext = pch + strlen ((char *) pch);
		    }
		    else
		    {
			/* temporarily put a string terminator here */
			chSave = *pchNext;
			*pchNext = '\0';
		    }

		    /*
		     * Lookup environment variable
		     */
		    lenEnvVar = strlen ((char *)pch);
		    pchEnvValue = (unsigned char *) getenv ((char *)pch);

		    if (pchEnvValue)
		    {
			/*
			 * Insure there's enough room in the return string
			 */
			lenEnvValue = strlen ((char *)pchEnvValue);
			if (!pchReturn)
			{
			    lenReturn = lenOriginal + 1 - lenEnvVar +
						lenEnvValue;
			    pchReturn = (unsigned char *) 
				XtMalloc (lenReturn * sizeof (unsigned char));
			    pchReturn[0] = '\0';
			}
			else
			{
			    lenReturn = lenReturn + 1 - lenEnvVar +
						lenEnvValue;
			    pchReturn = (unsigned char *) 
				XtRealloc ((char *)pchReturn, 
				    lenReturn * sizeof (unsigned char));
			}

			/*
			 * Tack it onto the return string
			 */
			strcat ((char *)pchReturn, (char *)pchEnvValue);
		    }

		    /*
		     * Advance the pointer for the next pass
		     */
		    if (chSave)
		    {
			/* restore saved character */
			*pchNext = chSave;
			chSave = '\0';

			/* 
			 *  If this was a closing paren, then skip it
			 */
			if (bEatBreak)
			{
#ifdef MULTIBYTE
			    chlen = mblen ((char *)pchNext, MB_CUR_MAX);
#endif /* MULTIBYTE */
			    pchNext += chlen;
			}
		    }
		    pchStart = pch = pchNext;
#ifdef MULTIBYTE
		    chlen = mblen ((char *)pch, MB_CUR_MAX);
#endif /* MULTIBYTE */
		    /*
		     * We're already pointing at the next character
		     * to process, don't advance again!
		     */
		    bAlreadyAdvanced = True;

		    break;

		default:
		    /* this character is not interesting */
		    break;
	    }

	    /*
	     * Move to the next character if we're not already
 	     * there.
	     */
	    if (!bAlreadyAdvanced)
	    {
		pch += chlen; 
#ifdef MULTIBYTE
		chlen = mblen ((char *)pch, MB_CUR_MAX);
#endif /* MULTIBYTE */
	    }
	}
#ifdef MULTIBYTE
	else
	{
	    pch += chlen;
            chlen = mblen ((char *)pch, MB_CUR_MAX);
	}
#endif /* MULTIBYTE */
    }

    if (pchReturn && *pchStart)
    {
	/*
	 * Copy remaining parts of the string
	 */
	strcat ((char *)pchReturn, (char *)pchStart);
    }

    return (pchReturn);

} /* END OF FUNCTION _DtWmParseExpandEnvironmentVariables */



/******************************<->*************************************
 *
 *  _DtWmParseMakeQuotedString (pchLine)
 *
 *
 *  Description:
 *  -----------
 *  Encapsulates the passed in "line" into a string argument quoted
 *  by double quotes. Special characters are "escaped" as needed.
 *
 *  Inputs:
 *  ------
 *  pchLine = ptr to string to enclose in quotes
 *
 *  Outputs:
 *  -------
 *  Return = ptr to quoted string 
 *
 *  Comment:
 *  -------
 *  Returned string should be freed with XtFree().
 * 
 ******************************<->***********************************/

unsigned char *
_DtWmParseMakeQuotedString (unsigned char *pchLine)
{
    unsigned char *pchRet;
    int iLen0, iLen1;
    int cSpecial;
    int i,j;
#ifdef MULTIBYTE
    int   chlen;
#endif

    iLen0 = strlen ((char *)pchLine);
    iLen1 = iLen0 + 2; /* for starting, ending quotes */

    for (i=0; i < iLen0; i++)
    {
        /*
	 * Count special chars to get estimate of new length
	 */
#ifdef MULTIBYTE
        chlen = mblen ((char *) &pchLine[i], MB_CUR_MAX);
	if ((chlen == 1) &&
	    ((pchLine[i] == '\\') ||
	     (pchLine[i] == '"')))
	{
	    iLen1++;
	}
	else if (chlen < 1)
	{
	    break;
	}
	else 
	{
	    i += chlen-1;
	}
#else /* MULTIBYTE */
	if ((pchLine[i] == '\\') ||
	    (pchLine[i] == '"'))
	{
	    iLen1++;
	}
#endif /* MULTIBYTE */
    }

    pchRet = (unsigned char *) XtMalloc (1+iLen1);

    if (pchRet)
    {
	pchRet[0] = '"';	/* starting quote */
	/*
	 * Copy chars from old string to new one
	 */
	for (i=0, j=1; i < iLen0; i++, j++)
	{
#ifdef MULTIBYTE
	    chlen = mblen ((char *) &pchLine[i], MB_CUR_MAX);
	    if ((chlen == 1) &&
		((pchLine[i] == '\\') ||
		 (pchLine[i] == '"')))
	    {
		/* quote next char */
		pchRet[j++] = '\\';
	    }
	    else if (chlen < 1)
	    {
		break;
	    }
	    else while (chlen > 1)
	    {
		/* copy first bytes of multibyte char */
		pchRet[j++] = pchLine[i++];
		chlen--;
	    }
#else /* MULTIBYTE */
	    if ((pchLine[i] == '\\') ||
		(pchLine[i] == '"'))
	    {
		/* quote next char */
		pchRet[j++] = '\\';
	    }
#endif /* MULTIBYTE */
	    /* copy char */
            pchRet[j] = pchLine[i];
	}
	pchRet[j++] = '"'; 	/* ending quote */
	pchRet[j] = '\0';	/* end of string */
    }

    return (pchRet);

} /* END OF FUNCTION _DtWmParseMakeQuotedString */


/*==================== END OF FILE   WmParse.c ====================*/
