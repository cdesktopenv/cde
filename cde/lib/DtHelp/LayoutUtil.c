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
/* $XConsortium: LayoutUtil.c /main/26 1996/11/06 12:25:09 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:       LayoutUtil.c
 **
 **   Project:    Cde DtHelp
 **
 **   Description:
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

/*
 * system includes
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "CanvasI.h"
#include "CvStringI.h"
#include "LayoutUtilI.h"
#include "StringFuncsI.h"
#include "VirtFuncsI.h"

#if defined(NLS16) || !defined(NO_MESSAGE_CATALOG)
#include <nl_types.h>
#endif

#ifndef NL_CAT_LOCALE
static const int NL_CAT_LOCALE = 0;
#endif

/******************************************************************************
 *
 * Private Defines
 *
 *****************************************************************************/
#define	GROW_SIZE	10
#define	CheckFormat(x) \
	(((x)->format_y == -1 || (x)->format_y > (x)->y_pos) ? False : True)

/******************************************************************************
 *
 * Private Variables
 *
 *****************************************************************************/
static  char *OneByteCantBeginList = "])}`\"\'.,;?:!";
static  char *OneByteCantEndList   = "[({`\"";

static	_DtCvLayoutInfo	DefLayInfo =
    {
	NULL, 		/* _DtCvSegmentI *line_seg;		*/
	 0,		/* unsigned int	 line_start;		*/
	 0,		/* unsigned int	 line_bytes;		*/
	 0,		/* _DtCvUnit	 cur_len;		*/
	 0,		/* _DtCvUnit	 max_x_pos;		*/
	 0,		/* _DtCvUnit	 cur_max_x;		*/
	 0,		/* _DtCvUnit	 y_pos;			*/
	 0,		/* _DtCvUnit	 text_x_pos;		*/
	 0,		/* _DtCvUnit	 leading;		*/
	-1,		/* int		 lst_hyper;		*/
	_CEFORMAT_ALL,	/* int		 format_y;		*/
	-1,		/* int		 join_line;		*/
	FALSE,		/* _DtCvValue	 lst_vis;		*/
	FALSE,		/* _DtCvValue	 join;			*/
	FALSE,		/* _DtCvValue	 align_flag;		*/
	NULL,		/* const char	*align_char;		*/
	-1,		/* _DtCvUnit	 align_pos;		*/
	0,		/* int		 delayed_search_saves	*/
    };

static const _DtCvSelectData DefSelectData = { -1, -1, -1, -1 };
static const _DtCvTraversalInfo DefTravData =
  {
    _DtCvFALSE		/* active   */,
    _DtCvTraversalNone	/* type     */,
    -1			/* idx      */,
    0			/* x_pos    */,
    0			/* y_pos    */,
    0			/* width    */,
    0			/* height   */,
    NULL		/* *seg_ptr */
  };

/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:	IsTrueMultiByte
 *
 * Returns:	True	if the character is a multibyte character
 *		False	if the character is a single byte character.
 *****************************************************************************/
static _DtCvValue
IsTrueMultiByte (wchar_t wc_char)
{
    char buf[MB_LEN_MAX];

    /*
     * check to see if this is a one byte character
     * There might not be a multibyte list for this locale.
     * Can't break on single byte characters.
     */
    if (1 != wctomb(buf, wc_char))
	return True;

    return False;
}

/******************************************************************************
 * Function:	CheckList
 *
 * Returns:	True	if the character matches one of the characters in
 *			the MultiCantEndList.
 *		False	if the character does not match an item in
 *			the MultiCantEndList.
 *****************************************************************************/
static _DtCvValue
CheckList (
    wchar_t		 wc_char,
    const wchar_t	*list)
{
    /*
     * check the multibyte list for the character
     */
    if (list != NULL)
      {
        while ('\0' != *list)
	  {
	    /*
	     * it matches, return true
	     */
	    if (*list == wc_char)
	        return True;
	    list++;
          }
      }

    return False;
}

/*****************************************************************************
 * Function:    static int CompareTraversalPos (_DtCvHandle canvas);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static int
CompareTraversalPos (
    const void  *a,
    const void  *b)
{
    _DtCvTraversalInfo *linkA = (_DtCvTraversalInfo *) a;
    _DtCvTraversalInfo *linkB = (_DtCvTraversalInfo *) b;
    _DtCvUnit           centA = linkA->y_pos + (linkA->height >> 1);
    _DtCvUnit           centB = linkB->y_pos + (linkB->height >> 1);

    if (linkA->y_pos + linkA->height < centB && centA < linkB->y_pos)
        return -1;

    if (linkB->y_pos + linkB->height < centA && centB < linkA->y_pos)
        return 1;

    if (linkA->x_pos != linkB->x_pos)
        return ((linkA->x_pos < linkB->x_pos) ? -1 : 1);

    if (linkA->y_pos != linkB->y_pos)
        return ((linkA->y_pos < linkB->y_pos) ? -1 : 1);

    if (linkA->height != linkB->height)
        return ((linkA->height < linkB->height) ? -1 : 1);

    if (linkA->width != linkB->width)
        return ((linkA->width < linkB->width) ? -1 : 1);

    return 0;
}

/******************************************************************************
 *
 * Private Layout Utility Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:    void _DtCvInitLayoutInfo ()
 *
 * Parameters:
 *
 * Returns:     Nothing.
 *
 *****************************************************************************/
void
_DtCvInitLayoutInfo (
    _DtCanvasStruct      *canvas,
    _DtCvLayoutInfo	*layout)
{
   *layout = DefLayInfo;

   layout->y_pos = canvas->metrics.top_margin;
}

/******************************************************************************
 * Function:    int _DtCvGetTraversalWidth ()
 *
 * Parameters:
 *
 * Returns:     The total amount of space to add before and after the
 *		segment to take into account traversal/link metrics on
 *		this segment including any necessary to 'close' out the
 *		link on the previous segment.
 *
 *****************************************************************************/
int
_DtCvGetTraversalWidth (
    _DtCanvasStruct      *canvas,
    _DtCvSegmentI        *p_seg,
    int			 lst_hyper)
{
    int  value = 0;
    int  lnkBefore = 0;
    int  lnkAfter  = 0;

    /*
     * does this segment have a different link than the previous one?
     */
    if (lst_hyper != p_seg->link_idx)
      {
	/*
	 * is the link visible?
	 */
        if (_DtCvIsSegVisibleLink(p_seg))
	  {
	    /*
	     * get the visible link metrics
	     */
	    lnkBefore = canvas->link_info.space_before;
	    lnkAfter  = canvas->link_info.space_after;
	  }
        if (_DtCvIsSegALink(p_seg))
          {
	    /*
	     * if the last 'link' was really a link, close it out by
	     * leaving room for the traversal and link end indicators
	     */
            if (lst_hyper != -1)
                value += (canvas->traversal_info.space_after + lnkAfter);

	    /*
	     * leave space for the traversal/link begin and end
	     * indicators for this segment.
	     */
            value += (canvas->traversal_info.space_before
				+ canvas->traversal_info.space_after
				+ lnkBefore
				+ lnkAfter);
          }
      }

    return value;
}

/******************************************************************************
 * Function: _DtCvAddLines
 *
 * makes sure the last x number of lines are blank.
 *****************************************************************************/
void
_DtCvAddSpace (
    _DtCvUnit		 number,
    _DtCvUnit		*ret_y)
{
    /*
     * anything to do?
     */
    if (0 >= number)
        return;

    /*
     * adjust the global Y position to allow the extra room
     */
    *ret_y = *ret_y + number;
}

/******************************************************************************
 * Function:	CheckOneByteCantEndList
 *
 * Returns:	True	if the character matches one of the characters in
 *			the OneByteCantEndList.
 *		False	if the character does not match an item in
 *			the OneByteCantEndList.
 *****************************************************************************/
_DtCvValue
_DtCvCheckOneByteCantEndList (
    char	 c,
    char	*cant_end_list)
{
    int  i;

    for (i = 0; cant_end_list[i]; i++)
	if (cant_end_list[i] == c)
	    return True;

    return False;
}

/******************************************************************************
 * Function:	CheckOneByteCantBeginList
 *
 * Returns:	True	if the character matches one of the characters in
 *			the OneByteCantBeginList.
 *		False	if the character does not match an item in
 *			the OneByteCantBeginList.
 *****************************************************************************/
_DtCvValue
_DtCvCheckOneByteCantBeginList (
    char	 c,
    char	*cant_begin_list)
{
    int  i;

    for (i = 0; cant_begin_list[i]; i++)
	if (cant_begin_list[i] == c)
	    return True;

    return False;
}

/******************************************************************************
 * Function:	CheckLineSyntax
 *
 * Returns:	True	if the segment can end a line.
 *		False	if the segment can not end a line.
 *
 * Purpose:	Checks the line syntax. Will not allow a segment to end
 *		a line if:
 *			the segment does not end with a hypen.
 *			the segment does not end with a space and the
 *				next segment does not begin with a space.
 *			the segment ends with a two byte characters that
 *				can not end a line.
 *			The next segment starts with a two byte character
 *				that can not begin a line.
 *			the segment ends with an one-byte open type and
 *				the next segment starts with a
 *				two byte character.
 *			the segment ends with a two byte character and
 *				the next segment starts with a one-byte
 *				close type.
 *			the next segment is a non-breaking string or region.
 *
 *****************************************************************************/
_DtCvValue
_DtCvCheckLineSyntax (
    _DtCanvasStruct	 *canvas,
    _DtCvSegmentI	 *pSeg,
    int			  start,
    int			  str_len,
    _DtCvValue		  skip_hypen_ck)
{
    int     myStrLen;
    int     wcFlag;
    void    *pChar;
    wchar_t  nextChar;
    wchar_t  lastChar;
    _DtCvValue lstCharMb = False;
    _DtCvValue nxtCharMb = False;

    /*
     * while this is a marker or a noop without a end-of-line, go to the
     * next segment.
     */
    while (NULL != pSeg && (_DtCvIsSegMarker(pSeg) ||
				(_DtCvIsSegNoop(pSeg) && !_DtCvIsSegNewLine(pSeg))))
	pSeg = pSeg->next_seg;

    /*
     * if this segment is null or not a string or region, stop the
     * test right now.
     */
    if (pSeg == NULL || !(_DtCvIsSegString(pSeg) || _DtCvIsSegRegion(pSeg)))
        return True;

    /*
     * Get the string segment stats
     */
    if (_DtCvIsSegString(pSeg))
      {
        wcFlag   = _DtCvIsSegWideChar(pSeg);
        pChar    = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg), wcFlag, start);
        myStrLen = _DtCvStrLen (pChar, wcFlag);
      }

    /*
     * if this is a region or a string segment (at the end of its string)
     * and it has a newline on it, then it can end a line.
     */
    if ((_DtCvIsSegRegion(pSeg) ||
			(_DtCvIsSegString(pSeg) && myStrLen == str_len))
		&& (_DtCvIsSegNewLine (pSeg) || pSeg->next_seg == NULL))
        return True;

    /*
     * if this is a region, then check it's breaking flag.
     */
    if (_DtCvIsSegRegion(pSeg))
      {
	if (_DtCvIsSegNonBreakingChar(pSeg))
	    return False;
	return True;
      }

    /*
     * so, to get this far, this is a string segment.
     *
     * Problems with indexing?
     */
    if (str_len <= 0)
	return True;

    /*
     * do we need to check the last character in the string?
     * If skip_hypen_ck is true, it means that 'lastChar' would be a hypen.
     */
    if (False == skip_hypen_ck)
      {
        /*
         * this region is a string, get its string information.
         */
        lastChar = _DtCvChar(pChar, wcFlag, str_len - 1);

        /*
         * check to make sure the last character is a valid last
         * character.
         */
        if (' ' == lastChar || '-' == lastChar)
	    return True;

        /*
	 * If this string is a multi-byte, check the list of multi-bytes
	 * that can't end a line.  If one is found it can't end a line.
         */
        if (wcFlag &&
		CheckList(lastChar, canvas->locale.cant_end_chars) == True)
	    return False;

	/*
	 * so at the end of these tests, the last character is
	 *    -) not a blank.
	 *    -) not a hypen.
	 *    -) either a single byte character or multibyte character
	 *       that can end the line (including a single byte in
	 *       wide char form).
	 *
	 * set the flag for the type of character lastChar is.
	 * if skip_hypen_ck was True, then lstCharMb remains False
	 * which is logical since it means that the caller has already
	 * processed a hypen (a single byte character).
	 */
	lstCharMb = IsTrueMultiByte(lastChar);
      }

    /*
     * Check for more characters in the string and
     * check its next character for breakable space.
     */
    if (myStrLen > str_len)
      {
	/*
	 * go to the next character.
	 */
	nextChar = _DtCvChar(pChar, wcFlag, str_len);

	/*
	 * Is it a valid break point?
	 */
	if (' ' == nextChar)
	    return True;

	/*
	 * set the multibyte flag for the next character
	 */
	nxtCharMb = IsTrueMultiByte(nextChar);

	/*
	 * If this is wide char string, check the list of multi-byte
	 * that can't begin a line.
	 * 
	 * But only if the last character wasn't a hypen!  Otherwise
	 * it's a character after a hypen and should not be broken on.
	 * 
	 * if this character is in the 'cannot begin line' list, then it
	 * can't be broken on (a return value of true).  (A wide char
	 * encoding of a single byte character should come back as False
	 * as long as the character is not in the list.)
	 *
	 * Have to double check to make sure it is a multibyte
	 * character (want it to go through the CheckMulti list just in
	 * case it's specified in there, before eliminating it).
	 */
	if (False == skip_hypen_ck && wcFlag
		&& CheckList(nextChar,canvas->locale.cant_begin_chars) == False
		&& True == nxtCharMb)
	    return True;

	/*
	 * either the character is after a hypen (starting a line) OR it
	 * is a multibyte character in the 'cannot begin a line' list OR
	 * it is a single byte character.  Therefore, this is a
	 * nonbreakable character.
	 */
	return False;
      }

    /*
     * We were at the last character of the string.
     * go to the next segment and see if it can start a new line.
     */
    do
      {
        pSeg = pSeg->next_seg;
      } while (pSeg != NULL && (_DtCvIsSegMarker(pSeg) ||
			(_DtCvIsSegNoop (pSeg) && !(_DtCvIsSegNewLine(pSeg)))));
    /*
     * If there isn't another valid segment, then the original segment
     * can end the line.
     */
    if (pSeg == NULL || !(_DtCvIsSegString(pSeg) || _DtCvIsSegRegion(pSeg)))
        return True;

    /*
     * if the last if fell through, then pSeg is a string or region.
     * check to see if you can break on it.
     */
    if (_DtCvIsSegNonBreakingChar(pSeg))
        return False;

    /*
     * if the last if fell through, then this is a breaking string or
     * region. Therefore, if a region, you can break on it.
     */
    if (_DtCvIsSegRegion(pSeg))
        return True;

    /*
     * To get this far, the next segment must be a string.  Check the
     * first character of the string to see if it can start a new line.
     */
    nextChar = _DtCvChar(_DtCvStringOfStringSeg(pSeg),
						_DtCvIsSegWideChar(pSeg), 0);
    if (' ' == nextChar)
            return True;

    /*
     * If the previous character was a single byte character (or a hypen),
     * it couldn't end a line. If this is a single byte string, then
     * this string can't start a line......
     */
    if (_DtCvIsSegRegChar(pSeg) &&
				(True == skip_hypen_ck || False == lstCharMb))
        return False;

    /*
     * If this is multi-byte, check the list of multi-byte
     * that can't begin a line.
     */
    if (_DtCvIsSegWideChar(pSeg))
      {
	/*
	 * plus checking the 'can not begin a line' list, check
	 * if the previous character was a hypen, then this can't be
	 * broken on either.
	 */
        if (True == skip_hypen_ck ||
		CheckList(nextChar, canvas->locale.cant_begin_chars) == True)
            return False;

        /*
         * if the previous character was a multi-byte and this
	 * character is a multibyte, then it is a valid break.
         */
	nxtCharMb = IsTrueMultiByte(nextChar);
        if (True == lstCharMb && True == nxtCharMb)
            return True;
      }

    /*
     * if the last character was a single byte character, then there
     * is still more to check - 1 byte punctuation around multi-byte.
     */
    if (False == lstCharMb &&
	_DtCvCheckOneByteCantEndList((char)lastChar,OneByteCantEndList) == True)
        return False;
    
    /*
     * or was the last character a multibyte and is followed by single byte
     * punctuation?
     */
    if (True == lstCharMb && False == nxtCharMb &&
	_DtCvCheckOneByteCantBeginList((char)nextChar, OneByteCantBeginList)
									== True)
        return False;

    return True;
}

/******************************************************************************
 * Function:	_DtCvGetNextWidth
 *
 * Purpose:	Determines the width of the next legal segment.
 *
 * Returns:	The width of the next legal segment.
 *
 *****************************************************************************/
int
_DtCvGetNextWidth (
    _DtCanvasStruct	 *canvas,
    int			  old_type,
    int			  lst_hyper,
    _DtCvSegmentI	 *pSeg,
    int			  start,
    _DtCvSegmentI	 *prev_seg,
    _DtCvSegmentI	**nextSeg,
    int			 *nextStart,
    int			 *widthCount)
{
    int      result;
    int      len = 0;
    int      tLen;
    int      wcFlag;
    int      curWidth;
    int      myLength;
    int      nextLen = 0;
    void    *pChar;
    char    *tChar;
    _DtCvValue  good_len;

    /*
     * pass over noops that don't have newlines and markers
     */
    while (pSeg != NULL && (_DtCvIsSegMarker(pSeg) ||
			(_DtCvIsSegNoop (pSeg) && !(_DtCvIsSegNewLine(pSeg)))))
      {
	pSeg = pSeg->next_seg;
	start = 0;
      }

    if (nextSeg != NULL)
        *nextSeg = pSeg;
    if (nextStart != NULL)
        *nextStart = start;

    /*
     * if the next segment is null or anything else but a string or region;
     * return that there is no more after this segment.
     */
    if (pSeg == NULL || !(_DtCvIsSegString(pSeg) || _DtCvIsSegRegion(pSeg)))
	return 0;

    /*
     * this segment is a region or string
     * check for region...anything left is a string.
     */
    if (_DtCvIsSegRegion(pSeg))
      {
	/*
	 * can I break on this region
	 */
	if (_DtCvIsSegNonBreakingChar(pSeg))
	  {
	    /*
	     * no...set the lengths and continue
	     */
	    len      = 1;
	    curWidth = _DtCvWidthOfRegionSeg(pSeg);
	  }
	else
	    return 0;
      }
    /*
     * is this a non breaking string?
     */
    else if (_DtCvIsSegNonBreakingChar(pSeg))
      {
	pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg),
					_DtCvIsSegWideChar(pSeg), start);
	len   = _DtCvStrLen (pChar, _DtCvIsSegWideChar(pSeg));

	curWidth = _DtCvGetStringWidth (canvas, pSeg, pChar, len)
			+ _DtCvGetTraversalWidth(canvas, pSeg, lst_hyper);
      }
    /*
     * so this is a string with possible breaks in it.
     */
    else
      {
	/*
	 * get the string stats
	 */
	wcFlag   = _DtCvIsSegWideChar (pSeg);
	pChar    = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg), wcFlag, start);
	myLength = _DtCvStrLen (pChar, wcFlag);
    
	    /*
	 * if a single byte string, zoom through it looking for
	 * specific breaking characters.
	     */
	if (0 == wcFlag)
	  {
	    tChar = pChar;
	    len = 0;
	    do
	      {
		/*
		 * checking for a hypen or space
		 */
		good_len = True;
		result = _DtCvStrcspn ((void *) tChar, " -", 0, &tLen);
		len += tLen;
    
		/*
		 * check for '-'. Some of the possible combinations are:
		 *    -text
		 *    - text
		 *    -/text/
		 *    text/-text/
		 *    text-text
		 *    text text
		 *
		 * if it is the first character to check and there is no
		 * previous segment, then it is starting a line and can
		 * not be broken on.
		 *
		 * _DtCvStrcpn return 0 if one of the characters in the
		 * test string was found.
		 */
		if (0 == result && '-' == tChar[tLen] && 0 == len &&
		    NULL == prev_seg &&
		    _DtCvCheckLineSyntax(canvas,pSeg,start,1,True) == False)
		  {
		    len++;
		    tLen++;
		    tChar += tLen;
		    good_len = False;
		  }
	      } while (!good_len);
    
	    /*
	     * found either a space or a hypen or null byte.
	     * If we found a hypen, include it.
	     */
	    if ('-' == *tChar)
		len++;
    
	    curWidth = _DtCvGetStringWidth (canvas, pSeg, pChar, len)
			+ _DtCvGetTraversalWidth(canvas, pSeg, lst_hyper);
    
	    /*
	     * Did we find a space or hypen?
	     * If not, can this segment stand alone?
	     */
	    if (result == 0 ||
		    _DtCvCheckLineSyntax(canvas,pSeg,start,len,False) == True)
	      {
		if (nextSeg != NULL)
		    *nextSeg   = pSeg;
		if (nextStart != NULL)
		    *nextStart = start + len;
		if (widthCount != NULL)
		    *widthCount = len;
		 return curWidth;
	      }
	  }
	/*
	 * multibyte (wide char string), look for a break the hard way.
	 */
	else
	  {
	    len = 0;
	    while (len < myLength)
	      {
		len++;
		if (_DtCvCheckLineSyntax(canvas,pSeg,start,len,False) == True)
		  {
		    pChar    = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg),
					_DtCvIsSegWideChar(pSeg), start);
		    curWidth = _DtCvGetStringWidth(canvas,pSeg,pChar,len)
			    + _DtCvGetTraversalWidth(canvas,pSeg,lst_hyper);
    
		    if (nextSeg != NULL)
		        *nextSeg    = pSeg;
		    if (nextStart != NULL)
		        *nextStart  = start + len;
		    if (widthCount != NULL)
		        *widthCount = len;
		    return curWidth;
		  }
	      }
    
	    /*
	     * Didn't find a smaller segment that satisfied the requirements.
	     * Determine the length of the current segment.
	     */
	    curWidth = _DtCvGetStringWidth (canvas, pSeg, pChar, len)
				    + _DtCvGetTraversalWidth(canvas, pSeg,
							lst_hyper);
	  }
      }

    /*
     * sigh...need to go further...this segment can't end a line
     * either.
     */
    prev_seg = pSeg;
    pSeg = pSeg->next_seg;
    if (pSeg != NULL)
      {
        start = 0;
        curWidth += _DtCvGetNextWidth (canvas, 
			_DtCvPrimaryTypeOfSeg (prev_seg), lst_hyper,
				pSeg, start, prev_seg,
				nextSeg, nextStart, &nextLen);

      }

    if (widthCount != NULL)
        *widthCount = len + nextLen;
    return (curWidth);
}

/******************************************************************************
 * Function: _DtCvSaveInfo
 *
 * Initializes a line table element to the segment it should display.
 *****************************************************************************/
void
_DtCvSaveInfo (
    _DtCanvasStruct	*canvas,
    _DtCvLayoutInfo	*layout,
    _DtCvUnit		 max_width,
    _DtCvUnit		 r_margin,
    _DtCvFrmtOption	 txt_justify)
{
    /*****************************************************************
     * The ascent for a line is described as the number of units
     * above the baseline.
     *
     * The descent for a line is described as the number of units
     * below the baseline.
     * 
     * Neither the ascent or decent value includes the baseline
     ****************************************************************/
    int    len;
    int    start      = layout->line_start;
    int    count      = layout->line_bytes;
    int    txtCnt     = canvas->txt_cnt;
    _DtCvUnit   maxAscent  = 0;
    _DtCvUnit   maxDescent = 0;
    _DtCvUnit   maxRegion  = 0;
    _DtCvUnit   superY     = 0;
    _DtCvUnit   subY       = 0;
    _DtCvUnit   fontAscent;
    _DtCvUnit   fontDescent;
    _DtCvValue fndLnk = False;
    _DtCvValue visLnk = False;

    void  *pChar;

    _DtCvSegmentI       *pSeg = layout->line_seg;

    if (txtCnt >= canvas->txt_max)
      {
	canvas->txt_max += GROW_SIZE;
	if (canvas->txt_lst)
	    canvas->txt_lst = (_DtCvDspLine *) realloc (
				(void *) canvas->txt_lst,
				(sizeof(_DtCvDspLine) * canvas->txt_max));
	else
	    canvas->txt_lst = (_DtCvDspLine *) malloc (
				(sizeof(_DtCvDspLine) * canvas->txt_max));
/*
 * NOTE....should this routine return a value?
 * If (re)alloc error occurs, this simply ignores the problem.
 */
	if (canvas->txt_lst == NULL)
	  {
	    canvas->txt_max = 0;
	    canvas->txt_cnt = 0;
	    return;
	  }
      }

    while (pSeg != NULL && count > 0)
      {
	/*
	 * set which line will this segment sit on, iff this is the
	 * first access to the segment.
	 */
	if ((void *) -1 == pSeg->internal_use)
	    pSeg->internal_use = (void *) txtCnt;
	
	/*
	 * now get the segment's sizing so we can determine
	 * the height and depth of the line.
	 */
	len = 1;
        fontAscent  = 0;
        fontDescent = 0;

	if (_DtCvIsSegVisibleLink(pSeg))
	    visLnk = True;

	if (_DtCvIsSegALink(pSeg))
	    fndLnk = True;

	/*
	 * get the ascent and descent of the segment along with a length
	 */
	if (_DtCvIsSegString(pSeg))
	  {
	    _DtCvFontMetrics(canvas,_DtCvFontOfStringSeg(pSeg),
			&fontAscent, &fontDescent, NULL, NULL, NULL);

	    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg),
					_DtCvIsSegWideChar(pSeg), start);
	    len   = _DtCvStrLen (pChar, _DtCvIsSegWideChar(pSeg));

	    if (len > count)
		len = count;
	  }
	else if (_DtCvIsSegRegion(pSeg))
	  {
	    if (-1 == _DtCvAscentOfRegionSeg(pSeg))
	      {
		if (maxRegion < _DtCvHeightOfRegionSeg(pSeg))
		    maxRegion = _DtCvHeightOfRegionSeg(pSeg);
	      }
	    else
	      {
		fontAscent  = _DtCvAscentOfRegionSeg(pSeg);
		fontDescent = _DtCvHeightOfRegionSeg(pSeg) - fontAscent;
	      }
	  }

	/*
	 * adjust the ascent and descent values by their subscript
	 * or superscript adjustments.
	 */
	if (_DtCvIsSegSuperScript(pSeg))
	  {
	    fontAscent  += superY;
	    fontDescent -= superY;

	    if (_DtCvIsSegRegion(pSeg) && -1 == _DtCvAscentOfRegionSeg(pSeg)
		&& maxRegion < _DtCvHeightOfRegionSeg(pSeg) + superY)
		maxRegion = _DtCvHeightOfRegionSeg(pSeg) + superY;
	  }
	else if (_DtCvIsSegSubScript(pSeg))
	  {
	    fontAscent  -= subY;
	    fontDescent += subY;
	    if (_DtCvIsSegRegion(pSeg) && -1 == _DtCvAscentOfRegionSeg(pSeg)
		&& maxRegion < _DtCvHeightOfRegionSeg(pSeg) + subY)
		maxRegion = _DtCvHeightOfRegionSeg(pSeg) + subY;
	  }
	else /* not a subscript or superscript */
	  {
	    /*
	     * set up the super and sub script offsets for following
	     * segments.
	     */
	    if (_DtCvIsSegString (pSeg))
	        _DtCvFontMetrics(canvas,_DtCvFontOfStringSeg(pSeg),
					NULL, NULL, NULL, &superY, &subY);
	    else if (_DtCvIsSegRegion(pSeg))
	      {
		superY = _DtCvHeightOfRegionSeg(pSeg) * 4 / 10;
		subY   = superY;
	      }
	  }

	/*
	 * now determine the maximums for ascent and descent.
	 */
	if (fontAscent > maxAscent)
	    maxAscent = fontAscent;
	if (fontDescent > maxDescent)
	    maxDescent = fontDescent;

	/*
	 * decrement the count
	 */
	count -= len;

	/*
	 * If this segment terminates the paragraph
	 * force the end of the loop.
	 */
        pSeg  = pSeg->next_disp;
	start = 0;
      }

    if (txt_justify == _DtCvJUSTIFY_RIGHT || _DtCvJUSTIFY_CENTER == txt_justify)
      {
	/*
	 * justify the line.
	 */
	_DtCvUnit   workWidth = max_width - layout->text_x_pos -
						r_margin - layout->cur_len;
	if (txt_justify == _DtCvJUSTIFY_CENTER)
	    workWidth = workWidth / 2;

	if (workWidth < 0)
	    workWidth = 0;

	layout->text_x_pos += workWidth;
      }

    /*
     * adjust for any special characters found
     */
    if (maxRegion > maxAscent + maxDescent + 1)
	maxAscent = maxRegion - maxDescent - 1;

    /*
     * check to see if the max values have even been touched.
     */
    if (layout->line_bytes == 0 && maxAscent == 0 && maxDescent == 0)
	maxAscent = canvas->metrics.line_height;

    /*
     * adjust ascent and descent by the traversal and link info
     */
    maxDescent += layout->leading;
    if (fndLnk)
      {
	maxAscent  += canvas->traversal_info.space_above;
        maxDescent += canvas->traversal_info.space_below;
	if (visLnk)
	  {
	    maxAscent  += canvas->link_info.space_above;
            maxDescent += canvas->link_info.space_below;
	  }
      }

    /*
     * save the line information, if there is a string here.
     */
    if (layout->line_bytes > 0)
      {
        canvas->txt_lst[txtCnt].processed  = _DtCvFALSE;
        canvas->txt_lst[txtCnt].text_x     = layout->text_x_pos;
        canvas->txt_lst[txtCnt].max_x      = layout->text_x_pos;
        canvas->txt_lst[txtCnt].baseline   = layout->y_pos + maxAscent;
        canvas->txt_lst[txtCnt].descent    = maxDescent;
        canvas->txt_lst[txtCnt].ascent     = maxAscent;
        canvas->txt_lst[txtCnt].byte_index = layout->line_start;
        canvas->txt_lst[txtCnt].length     = layout->line_bytes;
        canvas->txt_lst[txtCnt].seg_ptr    = layout->line_seg;

        canvas->txt_cnt++;
      }
    /*
     * blank line is one half the normal size line
     */
    else
      {
	maxAscent  = (maxAscent + maxDescent) / 2;
	maxDescent = 0;
      }

    if (layout->text_x_pos + layout->cur_len > layout->cur_max_x)
	layout->cur_max_x = layout->text_x_pos + layout->cur_len;

    if (layout->text_x_pos + layout->cur_len > layout->max_x_pos)
	layout->max_x_pos = layout->text_x_pos + layout->cur_len;

    /*
     * zero the string info
     */
    layout->line_bytes = 0;
    layout->cur_len    = 0;
    layout->lst_hyper  = -1;
    layout->lst_vis    = False;

    _DtCvSetJoinInfo(layout, False, -1);

    /*
     * adjust where the next line is positioned.
     */
    layout->y_pos = layout->y_pos + maxAscent + maxDescent + 1;
}

/******************************************************************************
 * Function: _DtCvCheckAddHyperToTravList
 *
 *****************************************************************************/
void
_DtCvCheckAddHyperToTravList (
    _DtCanvasStruct	*canvas,
    _DtCvSegmentI	*p_seg,
    _DtCvValue		 flag,
    _DtCvValue		*lst_vis,
    int			*lst_hyper,
    _DtCvUnit		*cur_len)
{
    int   nxtHyper;
    int   prevIdx;
    _DtCvValue	junk;
    _DtCvUnit	retLen = *cur_len;

    if (_DtCvIsSegALink (p_seg))
      {
	nxtHyper = _DtCvGetNextTravEntry(canvas);
	if (-1 == nxtHyper)
/*
 * NOTE....should this routine return a value?
 * If (re)alloc error occurs, this simply ignores the problem.
 */
		return;

	prevIdx = nxtHyper - 1;
	if (prevIdx < 0
	     || _DtCvTraversalLink != canvas->trav_lst[prevIdx].type
	     || p_seg->link_idx != canvas->trav_lst[prevIdx].seg_ptr->link_idx)
	  {
	    /*
	     * save this hypertext link in the traversal list
	     */
	    _DtCvSetTravEntryInfo (canvas, nxtHyper, _DtCvTraversalLink, p_seg,
					canvas->txt_cnt, _DtCvTRUE);
	  }
      }

    /*
     * take into account the link metrics.
     */
    junk = _DtCvIsSegVisibleLink(p_seg);
    *lst_vis = _DtCvModifyXpos(canvas->link_info, p_seg, junk,
			*lst_vis, *lst_hyper,
			&retLen);
    /*
     * take into account the traversal metrics
     */
    junk = _DtCvIsSegALink(p_seg);
    (void) _DtCvModifyXpos(canvas->traversal_info, p_seg, junk,
			((_DtCvValue) True), *lst_hyper,
			&retLen);

    *lst_hyper = p_seg->link_idx;

    if (_DtCvTRUE == flag)
	*cur_len = retLen;
}

/******************************************************************************
 * Function: ProcessStringSegment
 *
 * chops a string segment up until its completely used.
 *
 * Returns:
 *	0	if the entire string segment was processed.
 *	1	if the required number of lines were processed.
 *****************************************************************************/
int
_DtCvProcessStringSegment(
    _DtCanvasStruct	*canvas,
    _DtCvLayoutInfo	*lay_info,
    _DtCvUnit		 max_width,
    _DtCvUnit		 l_margin,
    _DtCvUnit		 r_margin,
    _DtCvSegmentI	*cur_seg,
    unsigned int	*cur_start,
    _DtCvFrmtOption	 txt_justify,
    _DtCvValue		 stat_flag)
{
    _DtCvUnit   workWidth;
    _DtCvUnit   stringLen;
    _DtCvUnit   textWidth;
    _DtCvUnit   nWidth;
    _DtCvUnit   spaceSize = 0;
    int	   oldType;
    int	   retStart;
    int	   retCount;
    wchar_t	 *wcp;
    void	 *pChar;
    char	 *strPtr;
    _DtCvValue    done    = False;
    _DtCvSegmentI *retSeg;

    if (NULL != _DtCvStringOfStringSeg(cur_seg))
      {
	if (lay_info->cur_len == 0)
	  {
	    lay_info->line_seg   = cur_seg;
	    lay_info->line_start = *cur_start;
	  }

	if (*cur_start == 0 && (cur_seg->type & _DtCvSEARCH_FLAG))
	    lay_info->delayed_search_saves++;

	oldType = _DtCvPrimaryTypeOfSeg (cur_seg);

	/*
	 * is alignment in effect?
	 */
	if (TRUE == lay_info->align_flag)
	  {
	    pChar     = _DtCvStrPtr(_DtCvStringOfStringSeg(cur_seg),
				_DtCvIsSegWideChar(cur_seg), *cur_start);
	    nWidth = _DtCvStrcspn (pChar, lay_info->align_char,
					_DtCvIsSegWideChar(cur_seg),
					&stringLen);
	    if (-1 == nWidth)
		return -1;

	    /*
	     * we got a valid length back, calculate the length
	     */
	    textWidth = 0;
	    if (0 != stringLen)
	        textWidth = _DtCvGetStringWidth(canvas,cur_seg,pChar,stringLen);

	    /*
	     * check to see if this a hypertext that needs
	     * to be remembered.
	     */
	    _DtCvCheckAddHyperToTravList (canvas, cur_seg, _DtCvTRUE,
					&(lay_info->lst_vis),
					&(lay_info->lst_hyper),
					&(lay_info->cur_len));

	    /*
	     * update the length and position information
	     * to skip past the characters before the alignment character.
	     */
	    lay_info->line_bytes += stringLen;
	    lay_info->cur_len    += (textWidth
					+ _DtCvGetTraversalWidth(canvas,
						cur_seg, lay_info->lst_hyper));

	    *cur_start += stringLen;

	    /*
	     * if we didn't find the character, check to see if this forces
	     * a newline - honor it if it does. We'll check the next
	     * string segment for the alignment character.
	     */
	    if (1 == nWidth && _DtCvIsSegNewLine (cur_seg)
						&& lay_info->line_bytes)
	      {
		_DtCvSaveInfo (canvas,lay_info,max_width,r_margin,txt_justify);

		while (lay_info->delayed_search_saves > 0) {
		    _DtCvSetSearchEntryInfo(canvas, canvas->txt_cnt - 1);
		    lay_info->delayed_search_saves--;
		}
		return 0;
	      }

	    /*
	     * so we found the character, now get it's width.
	     */
	    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(cur_seg),
				_DtCvIsSegWideChar(cur_seg), *cur_start);
	    textWidth = _DtCvGetStringWidth(canvas, cur_seg, pChar, 1)
			+ _DtCvGetTraversalWidth(canvas, cur_seg,
					lay_info->lst_hyper);
	    /*
	     * is this the second or more align position?
	     * if so, need to shift the character to align with others.
	     */
	    if (lay_info->align_pos >
		      lay_info->text_x_pos + lay_info->cur_len + textWidth / 2)
		lay_info->text_x_pos = lay_info->align_pos - lay_info->cur_len
					- textWidth / 2;
	    /*
	     * otherwise, does this exceed the previous alignments?
	     * if so, the table processing should catch that we've
	     * changed the alignment position and re-format the others.
	     */
	    else if (lay_info->align_pos <
		      lay_info->text_x_pos + lay_info->cur_len + textWidth / 2)
		lay_info->align_pos =
		      lay_info->text_x_pos + lay_info->cur_len + textWidth / 2;

	    /*
	     * indicate that the character has been found.
	     */
	    lay_info->align_flag = False;

	    /*
	     * check to see if this item can end a line.
	     * if can't end the line, force a join for the next segment or
	     * for the rest of this segment.
	     */
	    if (False == _DtCvCheckLineSyntax(canvas,cur_seg,*cur_start,1,False))
		lay_info->join = True;

	    /*
	     * update the length and position information to
	     * include the character.
	     */
	    lay_info->line_bytes++;
	    lay_info->cur_len += textWidth;

	    *cur_start += 1;

	    /*
	     * check to see if this is the end of the segment.
	     */
	    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(cur_seg),
				_DtCvIsSegWideChar(cur_seg), *cur_start);
	    if ((_DtCvIsSegWideChar(cur_seg) && 0 == *((wchar_t *) pChar))
				||
		(_DtCvIsSegRegChar(cur_seg) && '\0' == *((char *) pChar)))
		return 0;
	  }

	while (1)
	  {
	    /*
	     * recalculate the width
	     */
	    workWidth = max_width - lay_info->text_x_pos -
						lay_info->cur_len - r_margin;

	    /*
	     * adjust the character pointer and get the
	     * length of the string.
	     */
	    pChar     = _DtCvStrPtr(_DtCvStringOfStringSeg(cur_seg),
				_DtCvIsSegWideChar(cur_seg), *cur_start);
	    stringLen = _DtCvStrLen (pChar, _DtCvIsSegWideChar(cur_seg));
    
	    /*
	     * get the pixel width of the text string.
	     */
	    textWidth = _DtCvGetStringWidth(canvas,cur_seg,pChar,stringLen)
			+ _DtCvGetTraversalWidth(canvas, cur_seg,
					lay_info->lst_hyper);
	    /*
	     * Will it fit in the current width?
	     */
	    if (stat_flag == True || textWidth <= workWidth)
	      {
		/*
		 * Yes, this segment or part of a segment can fit in the
		 * current width. But can the last character of this
		 * segment end a line and can the beginning of the next
		 * segment start a new line?
		 */
		if (stat_flag == True ||
			_DtCvCheckLineSyntax (canvas, cur_seg,
					*cur_start, stringLen, False) == TRUE)
		  {
	            /*
	             * check to see if this a hypertext that needs
	             * to be remembered.
	             */
	            _DtCvCheckAddHyperToTravList (canvas, cur_seg, _DtCvFALSE,
					&(lay_info->lst_vis),
					&(lay_info->lst_hyper),
					&(lay_info->cur_len));

		    /*
		     * The line syntax is good.
		     * Update the global and width variables.
		     */
		    lay_info->line_bytes += stringLen;
		    lay_info->cur_len    += textWidth;
		    _DtCvSetJoinInfo(lay_info,
					_DtCvIsSegNonBreakingChar(cur_seg),
					-1);

		    /*
		     * Check to see if this segment forces an end
		     */
		    if (_DtCvIsSegNewLine (cur_seg) && lay_info->line_bytes) {
			_DtCvSaveInfo (canvas, lay_info, max_width,
							r_margin, txt_justify);
			while (lay_info->delayed_search_saves > 0) {
	 		    _DtCvSetSearchEntryInfo(canvas,
						    canvas->txt_cnt - 1);
			    lay_info->delayed_search_saves--;
			}
		    }

		    return 0;
		  }
    
		/*
		 * CheckLineSyntax says that either this line couldn't
		 * end a line or the next segment couldn't start a line.
		 * Therefore, find out how much of the next segment or
		 * segments we need to incorporate to satisfy the Line
		 * Syntax rules.
		 */
		nWidth = _DtCvGetNextWidth (canvas, oldType,
				lay_info->lst_hyper,
				cur_seg->next_seg, 0, cur_seg,
				&retSeg, &retStart, &retCount);
		/*
		 * will this segment + the next segment fit?
		 */
		if (textWidth + nWidth <= workWidth)
		  {
	            /*
	             * check to see if this a hypertext that needs
	             * to be remembered.
	             */
	            _DtCvCheckAddHyperToTravList (canvas, cur_seg, _DtCvFALSE,
					&(lay_info->lst_vis),
					&(lay_info->lst_hyper),
					&(lay_info->cur_len));

		    /*
		     * YEAH Team!! It Fits!!
		     *
		     * Update the global and width variables.
		     */
		    lay_info->line_bytes += stringLen;
		    lay_info->cur_len    += textWidth;
		    _DtCvSetJoinInfo(lay_info, False, -1);
    
		    return 0;
		  }
	      }
    
	    /*
	     * the text width plus the next segment is tooo big
	     * to fit. Reduce the current segment if possible
	     */
	    done = False;
	    textWidth = 0;
	    stringLen = 0;
	    while (!done)
	      {
		nWidth = _DtCvGetNextWidth (canvas, oldType,
				lay_info->lst_hyper,
				cur_seg, *cur_start, NULL,
				&retSeg, &retStart, &retCount);

		if (retSeg == cur_seg && textWidth + nWidth <= workWidth)
		  {
	            /*
	             * check to see if this a hypertext that needs
	             * to be remembered.
	             */
	            _DtCvCheckAddHyperToTravList (canvas, cur_seg, _DtCvFALSE,
					&(lay_info->lst_vis),
					&(lay_info->lst_hyper),
					&(lay_info->cur_len));

		    _DtCvSetJoinInfo(lay_info, False, -1);
		    *cur_start     = retStart;
		    stringLen     += retCount;
		    textWidth     += nWidth;
		    spaceSize      = 0;

		    /*
		     * take into account a space if that is where it breaks.
		     */
		    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(cur_seg),
						_DtCvIsSegWideChar(cur_seg),
						*cur_start);
		    if ((_DtCvIsSegWideChar(cur_seg) &&
						(' ' == *((wchar_t *) pChar)))
					||
			(_DtCvIsSegRegChar(cur_seg) &&
						(' ' == *((char *) pChar))))
		      {
			    spaceSize = _DtCvGetStringWidth(canvas,
						cur_seg, pChar, 1)
					+ _DtCvGetTraversalWidth (canvas,
						cur_seg, lay_info->lst_hyper);
			    textWidth += spaceSize;
			    stringLen++;
			    (*cur_start)++;
			  }
		      }
		else
		  {
		    /*
		     * Done trying to find a segment that will
		     * fit in the size given
		     */
		    done = True;
		  }
	      }

	    /*
	     * Update the global variables
	     */
	    lay_info->line_bytes += stringLen;
	    lay_info->cur_len  += textWidth;

	    if (lay_info->join == True || lay_info->line_bytes == 0)
	      {
	        /*
		 * This line would be empty if we followed the rules.
		 * Or it would break a line improperly.
		 * Force this onto the line.
	         * check to see if this a hypertext that needs
	         * to be remembered.
	         */
	        _DtCvCheckAddHyperToTravList (canvas, cur_seg, _DtCvTRUE,
					&(lay_info->lst_vis),
					&(lay_info->lst_hyper),
					&(lay_info->cur_len));

		/*
		 * Couldn't find a smaller, have to
		 * go with the larger segment.
		 */
		pChar      = _DtCvStrPtr(_DtCvStringOfStringSeg(cur_seg),
						_DtCvIsSegWideChar(cur_seg),
						*cur_start);
		stringLen  = _DtCvStrLen (pChar, _DtCvIsSegWideChar(cur_seg));
		if (retCount > 0 && retCount < stringLen)
		    stringLen = retCount;

		lay_info->line_bytes += stringLen;
		lay_info->cur_len  += (_DtCvGetStringWidth(canvas, cur_seg,
							pChar, stringLen)
					+ _DtCvGetTraversalWidth (canvas,
					cur_seg, lay_info->lst_hyper));

		_DtCvSetJoinInfo(lay_info, False, -1);

		/*
		 * If we had to do a bigger segment,
		 * then we're done processing the target segment.
		 */
		if (stringLen == _DtCvStrLen(pChar,_DtCvIsSegWideChar(cur_seg)))
		  {
		    if (_DtCvCheckLineSyntax (canvas, cur_seg,
				*cur_start, stringLen, False) == False)
			_DtCvSetJoinInfo(lay_info, True, -1);
		    else if (_DtCvIsSegNewLine (cur_seg)) {
			_DtCvSaveInfo (canvas, lay_info, max_width,
							r_margin, txt_justify);
			while (lay_info->delayed_search_saves > 0) {
	 		    _DtCvSetSearchEntryInfo(canvas,
						    canvas->txt_cnt - 1);
			    lay_info->delayed_search_saves--;
			}
		    }
		    return 0;
		  }

		*cur_start     = retStart;
	      }
	    else if (spaceSize)
	      {
		/*
		 * If a space was included as the last character,
		 * remove it now.
		 */
		lay_info->line_bytes--;
		lay_info->cur_len -= spaceSize;
	      }

	    /*
	     * Save the information
	     */
	    _DtCvSaveInfo (canvas, lay_info, max_width, r_margin, txt_justify);
	    if (*cur_start == 0 && (cur_seg->type & _DtCvSEARCH_FLAG))
		lay_info->delayed_search_saves--;

	    while (lay_info->delayed_search_saves > 0) {
	        _DtCvSetSearchEntryInfo(canvas, canvas->txt_cnt - 1);
		lay_info->delayed_search_saves--;
	    }

	    /*
	     * Skip the spaces.
	     */
	    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(cur_seg),
						_DtCvIsSegWideChar(cur_seg),
						*cur_start);
	    if (_DtCvIsSegWideChar(cur_seg))
	      {
		wcp = pChar;
		while (' ' == *wcp)
		  {
		    wcp++;
		    (*cur_start)++;
		  }

	        pChar = wcp;
	      }
	    else /* single byte string */
	      {
		strPtr = pChar;
	        while (' ' == *strPtr)
		  {
		    strPtr++;
		    (*cur_start)++;
		  }

	        pChar = strPtr;
	      }

	    /*
	     * are we at the end of the segment?
	     */
	    if ((_DtCvIsSegWideChar(cur_seg) && 0 == *((wchar_t *) pChar))
			||
		(_DtCvIsSegRegChar(cur_seg) && 0 == *((char *) pChar)))
	        return 0;

	    if (*cur_start == 0 && (cur_seg->type & _DtCvSEARCH_FLAG))
		lay_info->delayed_search_saves++;

	    /*
	     * Initialize the global variables
	     */
	    lay_info->line_seg   = cur_seg;
	    lay_info->line_start = *cur_start;
	    lay_info->text_x_pos = l_margin;

	    if (CheckFormat(lay_info) == True)
		return 1;

            /*
             * check to see if this a hypertext that needs
             * to be remembered.
             */
            _DtCvCheckAddHyperToTravList (canvas, cur_seg, _DtCvTRUE,
					&(lay_info->lst_vis),
					&(lay_info->lst_hyper),
					&(lay_info->cur_len));
	  }
      }
    else if (_DtCvIsSegNewLine (cur_seg))
      {
	/*
	 * Force a save - even if it is an empty line.
	 */
	_DtCvSaveInfo (canvas, lay_info, max_width, r_margin, txt_justify);
	while (lay_info->delayed_search_saves > 0) {
	    _DtCvSetSearchEntryInfo(canvas, canvas->txt_cnt - 1);
	    lay_info->delayed_search_saves--;
	}
      }

    return 0;

} /* End _DtCvProcessStringSegment */

/******************************************************************************
 * Function:	_DtCvSetJoinInfo
 *
 * Returns:	sets the joining information to the given information.
 *
 *****************************************************************************/
void
_DtCvSetJoinInfo (
    _DtCvLayoutInfo	*lay_info,
    _DtCvValue		 flag,
    int			 txt_ln)
{
    lay_info->join      = flag;
    lay_info->join_line = txt_ln;
}

/******************************************************************************
 * Function:	_DtCvGetNextTravEntry
 *
 * Returns:	 >= 0 if success,
 *		-1 if failure.
 *
 * Purpose:	Return the next available entry in the traversal list.
 *
 *****************************************************************************/
int
_DtCvGetNextTravEntry (
    _DtCanvasStruct  *canvas)
{
    int  nxtEntry = canvas->trav_cnt;

    /*
     * does the list need to grow?
     */
    if (nxtEntry >= canvas->trav_max)
      {
	/*
	 * grow by a set amount
	 */
	canvas->trav_max += GROW_SIZE;

	/*
	 * realloc or malloc?
	 */
	if (NULL != canvas->trav_lst)
	    canvas->trav_lst = (_DtCvTraversalInfo *) realloc (
			(char *) canvas->trav_lst,
			((sizeof(_DtCvTraversalInfo)) * canvas->trav_max));
	else
	    canvas->trav_lst = (_DtCvTraversalInfo *) malloc (
			((sizeof(_DtCvTraversalInfo)) * canvas->trav_max));

	/*
	 * did the memory allocation work? if not return error code.
	 */
	if (NULL == canvas->trav_lst)
	  {
	    canvas->trav_max = 0;
	    canvas->trav_cnt = 0;
	    nxtEntry = -1;
          }
      }

    canvas->trav_lst[nxtEntry] = DefTravData;

    return nxtEntry;
}

/******************************************************************************
 * Function:	_DtCvSetTravEntryInfo
 *
 * Returns:	 0 if success,
 *		-1 if failure.
 *
 * Purpose:	Set the high level information in an entry of the traversal
 *              list.
 *****************************************************************************/
int
_DtCvSetTravEntryInfo (
    _DtCanvasStruct	*canvas,
    int			 entry,
    _DtCvTraversalType	 type,
    _DtCvSegmentI	*p_seg,
    int			 line_idx,
    _DtCvValue		 inc)
{
    int  result = -1;

    if (-1 != entry && entry <= canvas->trav_cnt)
      {
	_DtCvTraversalInfo *travEntry = &(canvas->trav_lst[entry]);

	travEntry->type    = type;
	travEntry->seg_ptr = p_seg;
	travEntry->idx     = line_idx;

	if (_DtCvTRUE == inc)
	    canvas->trav_cnt++;

	result = 0;
      }

    return result;
}

int
_DtCvGetNextSearchEntry(_DtCanvasStruct* canvas)
{
    if (canvas->search_cnt >= canvas->search_max) {
	canvas->search_max += GROW_SIZE;

	if (canvas->searchs)
	    canvas->searchs = (_DtCvSearchData *)
			realloc((void*)canvas->searchs,
				canvas->search_max * sizeof(_DtCvSearchData));
	else
	    canvas->searchs = (_DtCvSearchData *)
			malloc(canvas->search_max * sizeof(_DtCvSearchData));
    }

    canvas->searchs[canvas->search_cnt].idx = -1;

    return canvas->search_cnt++;
}

int
_DtCvSetSearchEntryInfo(_DtCanvasStruct* canvas, int line_idx)
{
    int search_idx;

    /* get a next available slot for search */
    search_idx = _DtCvGetNextSearchEntry(canvas);

    /* save information (i.e. line_idx) */
    canvas->searchs[search_idx].idx = line_idx;
}

/******************************************************************************
 * Function:	_DtCvSetTravEntryPos
 *
 * Returns:	 0 if success,
 *		-1 if failure.
 *
 * Purpose:	Set the position and dimension information of an entry in
 *		the traversal list.
 *
 *****************************************************************************/
int
_DtCvSetTravEntryPos (
    _DtCanvasStruct	*canvas,
    int			 entry,
    _DtCvUnit		 x,
    _DtCvUnit		 y,
    _DtCvUnit		 width,
    _DtCvUnit		 height)
{
    int  result = -1;

    if (-1 != entry && entry <= canvas->trav_cnt)
      {
	_DtCvTraversalInfo *travEntry = &(canvas->trav_lst[entry]);

	travEntry->x_pos  = x;
	travEntry->y_pos  = y;
	travEntry->width  = width;
	travEntry->height = height;

	result = 0;
      }

    return result;
}

/******************************************************************************
 * Function:	_DtCvCalcMarkPos
 *
 * Returns:	 0 if success,
 *		-1 if failure.
 *
 * Purpose:	Calcalate the position and dimension information of a mark.
 *
 *****************************************************************************/
int
_DtCvCalcMarkPos (
    _DtCanvasStruct	*canvas,
    int			 entry,
    _DtCvUnit		*ret_x,
    _DtCvUnit		*ret_y,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_height)
{
    int            result = -1;

    if (-1 != entry && entry <= canvas->mark_cnt)
      {
        _DtCvMarkData *mark = &(canvas->marks[entry]);

	/*
	 * if we've got a line index for the mark, get the positions.
	 */
	if (-1 != mark->beg.line_idx && -1 != mark->end.line_idx)
	  {
	    _DtCvDspLine  *line = &(canvas->txt_lst[mark->beg.line_idx]);

	    *ret_x = mark->beg.x;
	    *ret_y = mark->beg.y - line->ascent;

	    if (mark->beg.line_idx == mark->end.line_idx)
	        *ret_width = mark->end.x - *ret_x;
	    else
	        *ret_width = canvas->txt_lst[mark->beg.line_idx].max_x - *ret_x;

	    *ret_height = line->ascent + line->descent + 1;

	    result = 0;
	  }
      }

    return result;
}

/******************************************************************************
 * Function:	_DtCvSortTraversalList
 *
 * Returns:	 nothing
 *
 * Purpose:	Sort the traversal list
 *
 *****************************************************************************/
void
_DtCvSortTraversalList (
    _DtCanvasStruct	*canvas,
    _DtCvValue		 retain)
{
    int curTrav = canvas->cur_trav;

    if (1 < canvas->trav_cnt)
      {
	/*
	 * indicate this is the current traversal
	 */
	if (-1 != curTrav)
	    canvas->trav_lst[curTrav].active = retain;

	/*
	 * sort the items.
	 */
	qsort (canvas->trav_lst, canvas->trav_cnt, sizeof(_DtCvTraversalInfo),
			CompareTraversalPos);

	if (_DtCvTRUE == retain && -1 != curTrav &&
				_DtCvFALSE == canvas->trav_lst[curTrav].active)
	  {
	    curTrav = 0;
	    while (_DtCvFALSE == canvas->trav_lst[curTrav].active)
		curTrav++;

	    canvas->cur_trav = curTrav;
	  }

	/*
	 * clear the active flag
	 */
	if (-1 != curTrav)
	    canvas->trav_lst[curTrav].active = _DtCvFALSE;
      }
}

/*****************************************************************************
 * Function:	_DtCvCvtSegsToPts()
 *
 * Purpose:	Given a set of segments, determine the ending points.
 *
 *****************************************************************************/
_DtCvStatus
_DtCvCvtSegsToPts (
    _DtCanvasStruct	 *canvas,
    _DtCvSegPtsI	**segs,
    _DtCvSelectData	 *beg,
    _DtCvSelectData	 *end,
    _DtCvUnit		 *ret_y1,
    _DtCvUnit		 *ret_y2,
    _DtCvSegmentI	**ret_seg)
{
    int			 count;
    int			 cnt;
    int			 start;
    int			 length;
    int			 lineIdx;
    int			 linkIdx = -1;
    _DtCvValue		 lastVisLnk = _DtCvFALSE;
    _DtCvUnit		 minY = -1;
    _DtCvUnit		 maxY = 0;
    _DtCvUnit		 startX;
    _DtCvUnit		 endX;
    _DtCvUnit		 segWidth;
    _DtCvSegmentI	*pSeg;
    _DtCvSegmentI	*saveSeg;
    _DtCvSegmentI	**retSeg;
    _DtCvDspLine	*lines = canvas->txt_lst;
    _DtCvFlags		 result = _DtCvSTATUS_NONE;
    _DtCvSelectData	*tmpBeg;
    _DtCvSelectData	*tmpEnd;
    _DtCvSelectData	 bReg;
    _DtCvSelectData	 eReg;

    /*
     * initialize the structures.
     */
    bReg = DefSelectData;
    eReg = DefSelectData;
    *beg = DefSelectData;
    *end = DefSelectData;

    /*
     * go through each segment and determine the starting positions.
     */
    while (NULL != *segs)
      {
	result = _DtCvSTATUS_OK;

	/*
	 * what line is this segment on?
	 */
	lineIdx = (int) ((*segs)->segment->internal_use);

	/*
	 * get some information about the line
	 */
	length = lines[lineIdx].length;
	start  = lines[lineIdx].byte_index;
	startX = _DtCvGetStartXOfLine(&(lines[lineIdx]), &pSeg);
	pSeg   = lines[lineIdx].seg_ptr;

	/*
	 * now skip the segments on this line that aren't in the data pt.
	 */
	while (NULL != pSeg && pSeg != (*segs)->segment)
	  {
	    /*
	     * advance past any hypertext link offsets.
	     */
	    startX = _DtCvAdvanceXOfLine(canvas, pSeg, startX,
						&linkIdx, &lastVisLnk);

	    /*
	     * we know that this is not the segment we are looking for,
	     * so go past it.
	     */
	    _DtCvGetWidthOfSegment(canvas, pSeg, start, length,
							&cnt, &segWidth, NULL);

	    /*
	     * skip the segment's width, decrease the overall length by
	     * the segment's count, reset the character start point and
	     * go to the next segment.
	     */
	    startX += segWidth;
	    length -= cnt;
	    start   = 0;
	    pSeg    = pSeg->next_disp;
	  }

	/*
	 * This segment should be all or partially selected.
	 */
	if (NULL == pSeg)
	    return _DtCvSTATUS_BAD;

	/*
	 * now figure the start location.
	 */
	startX = _DtCvAdvanceXOfLine(canvas, pSeg, startX,
						&linkIdx, &lastVisLnk);
	/*
	 * guarenteed that this is the *first* line that the segment
	 * exists on. Therefore, may have to go to another line for
	 * the correct offset
	 */
	while (start + length < (*segs)->offset)
	  {
	    do { lineIdx++; } while (lineIdx < canvas->txt_cnt
					&& pSeg != lines[lineIdx].seg_ptr);

	    if (lineIdx >= canvas->txt_cnt)
		return _DtCvSTATUS_BAD;

	    length = lines[lineIdx].length;
	    start  = lines[lineIdx].byte_index;
	    startX = lines[lineIdx].text_x;

	    linkIdx    = -1;
	    lastVisLnk = False;
	    startX     = _DtCvAdvanceXOfLine(canvas, pSeg, startX,
						&linkIdx, &lastVisLnk);
	  }

	/*
	 * how many characters do we need to skip?
	 */
	count    = (*segs)->offset - start;
	segWidth = 0;
	if (0 < count)
            _DtCvGetWidthOfSegment(canvas, pSeg, start, count,
							&cnt, &segWidth, NULL);
	/*
	 * adjust the info by the width of the skipped characters.
	 */
	start  += count;
	length -= count;
	startX += segWidth;

	/*
	 * is this a region? If so set the region information instead.
	 */
	tmpBeg = beg;
	tmpEnd = end;
	retSeg = ret_seg;
	if (_DtCvIsSegRegion((*segs)->segment))
	  {
	    tmpBeg = &bReg;
	    tmpEnd = &eReg;
	    retSeg = &saveSeg;
	  }

	/*
	 * does this segment start the selection? text or region?
	 */
	if (tmpBeg->x == -1 || tmpBeg->y > lines[lineIdx].baseline ||
			(tmpBeg->line_idx == lineIdx && tmpBeg->x > startX))
	  {
	    tmpBeg->x        = startX;
	    tmpBeg->y        = lines[lineIdx].baseline;
	    tmpBeg->line_idx = lineIdx;
	    tmpBeg->char_idx = lines[lineIdx].length - length;
	    if (NULL != retSeg)
		*retSeg = (*segs)->segment;
	  }

	/*
	 * get the amount of this segment that is selected.
	 */
	count = (*segs)->len;

	/*
	 * is it longer than what's (left) on this line?
	 */
	while (count > length)
	  {
	    /*
	     * go to the next line containing the segment
	     */
	    do { 
		/*
		 * does this line have the minium y?
		 */
	        if (minY == -1 ||
			minY > lines[lineIdx].baseline - lines[lineIdx].ascent)
		   minY = lines[lineIdx].baseline - lines[lineIdx].ascent;

		 lineIdx++; 
	      } while (lineIdx < canvas->txt_cnt
					&& pSeg != lines[lineIdx].seg_ptr);
	    /*
	     * did we run out of lines?
	     */
	    if (lineIdx >= canvas->txt_cnt)
		return _DtCvSTATUS_BAD;
		
	    /*
	     * start over on this line
	     */
	    segWidth = 0;

	    /*
	     * get the true count to the next offset
	     */
	    cnt      = lines[lineIdx].byte_index - start;

	    /*
	     * get the next lines starting info.
	     */
	    start    = lines[lineIdx].byte_index;
	    length   = lines[lineIdx].length;
	    startX   = _DtCvGetStartXOfLine(&(lines[lineIdx]), &pSeg);
	    linkIdx    = -1;
	    lastVisLnk = False;
	    startX     = _DtCvAdvanceXOfLine(canvas, pSeg, startX,
						&linkIdx, &lastVisLnk);
	    /*
	     * subtract the previous length
	     */
	    count -= cnt;
	  }

	/*
	 * now go down the line, examining each segment.
	 */
	while (0 < count)
	  {
	    /*
	     * findout how many characters are in the next segment, and its
	     * width.
	     */
	    _DtCvGetWidthOfSegment(canvas,pSeg,start,count,&cnt,&segWidth,NULL);

	    /*
	     * there are less than in the count, go to the next segment.
	     */
	    if (cnt < count)
	      {
		pSeg   = pSeg->next_disp;
		start  = 0;
		startX += segWidth;
	      }

	    length -= cnt;
	    count -= cnt;
	  }

	endX   = startX + segWidth;

	/*
	 * does this segment end a segment?
	 */
	if (tmpEnd->x == -1 || tmpEnd->y < lines[lineIdx].baseline ||
			(tmpEnd->line_idx == lineIdx && tmpEnd->x < endX))
	  {
	    tmpEnd->x        = endX;
	    tmpEnd->y        = lines[lineIdx].baseline;
	    tmpEnd->line_idx = lineIdx;
	    tmpEnd->char_idx = lines[lineIdx].length - length;
	  }

	/*
	 * check for min and max values
	 */
	if (minY == -1 ||
			minY > lines[lineIdx].baseline - lines[lineIdx].ascent)
	    minY = lines[lineIdx].baseline - lines[lineIdx].ascent;

	if (maxY < lines[lineIdx].baseline + lines[lineIdx].descent)
	    maxY = lines[lineIdx].baseline + lines[lineIdx].descent;
	/*
	 * go to the next segment
	 */
	segs++;
      }

    /*
     * now determine if a region really starts the beginning of a
     * selection or a text does.
     *
     * was a region found?
     */
    if (-1 != bReg.x)
      {
	/*
	 * if no text was found, take the region information.
	 */
	if (-1 == beg->x)
	  {
	    *beg = bReg;
	    if (NULL != ret_seg)
		*ret_seg = saveSeg;
	  }
	/*
	 * or if the region is inline to the other
	 * text and it is before the text, then take it's x value.
	 */
	else if (bReg.x < beg->x &&
			(bReg.line_idx == beg->line_idx ||
	/*
	 * Or if the region is 'standalone' (a bullet of a list, a
	 * graphic to wrap around, etc.)  then check to see if it
	 * straddles the other information and is before the text.  If
	 * it does, take it's x value.
	 */
			_DtCvStraddlesPt(beg->y,
				bReg.y - lines[bReg.line_idx].ascent,
				bReg.y - lines[bReg.line_idx].descent)))
	  {
	    beg->x = bReg.x;

	    if (NULL != ret_seg)
		*ret_seg = saveSeg;
	  }
      }

    /*
     * now determine if a region really ends the selection or a text does.
     *
     * was a region found?
     */
    if (-1 != eReg.x)
      {
	/*
	 * if no text was found, take the region information.
	 */
	if (-1 == end->x)
	    *end = eReg;
	/*
	 * or if the region is inline to the other
	 * text and it is before the text, then take it's x value.
	 */
	else if (eReg.x > end->x &&
			(eReg.line_idx == end->line_idx ||
	/*
	 * Or if the region is 'standalone' (a bullet of a list, a
	 * graphic to wrap around, etc.)  then check to see if it
	 * straddles the other information and is before the text.  If
	 * it does, take it's x value.
	 */
			_DtCvStraddlesPt(end->y,
				eReg.y - lines[eReg.line_idx].ascent,
				eReg.y - lines[eReg.line_idx].descent)))
	    end->x = eReg.x;

      }
    if (NULL != ret_y1)
	*ret_y1 = minY;
    if (NULL != ret_y2)
	*ret_y2 = maxY;

    return result;
}

/*****************************************************************************
 * Function:	_DtCvAddToMarkList()
 *
 * Purpose:	Add a mark to the list of marks.
 *
 *****************************************************************************/
int
_DtCvAddToMarkList (
    _DtCanvasStruct	 *canvas,
    _DtCvPointer	  client_data,
    _DtCvValue		  flag,
    _DtCvSelectData	 *beg,
    _DtCvSelectData	 *end)
{
    _DtCvMarkData  *nxtMark;

    /*
     * does the array need more memory?
     */
    if (canvas->mark_cnt >= canvas->mark_max)
      {
	canvas->mark_max += GROW_SIZE;

        if (NULL == canvas->marks)
	    canvas->marks = (_DtCvMarkData *) malloc(
				sizeof(_DtCvMarkData) * canvas->mark_max);
        else
	    canvas->marks = (_DtCvMarkData *) realloc((void *) canvas->marks,
				sizeof(_DtCvMarkData) * canvas->mark_max);

	/*
	 * memory loss - bail
	 */
	if (NULL == canvas->marks)
	    return -1;
      }

    /*
     * set the mark information
     */
    nxtMark              = &(canvas->marks[canvas->mark_cnt]);
    nxtMark->on          = flag;
    nxtMark->client_data = client_data;
    nxtMark->beg         = *beg;
    nxtMark->end         = *end;
    canvas->mark_cnt++;

    return (canvas->mark_cnt - 1);
}
