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
/* $TOG: Canvas.c /main/40 1999/10/14 13:17:22 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Canvas.c
 **
 **   Project:     Cde Help System
 **
 **   Description: UI independent layer for the help system.  These
 **		   routines manage the information within a 'canvas'.
 **		   The 'canvas' routines call UI dependent code to
 **		   render the information.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <stdlib.h>
#include <string.h>

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
#include "LinkMgrI.h"
#include "LayoutUtilI.h"
#include "SelectionI.h"
#include "VirtFuncsI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
static _DtCvUnit	DrawText(
				_DtCanvasStruct	*canvas,
				_DtCvDspLine	*line,
				int		 txt_line,
				_DtCvFlags	 old_flag,
				_DtCvFlags	 new_flag );
/********    End Private Function Declarations    ********/

/*****************************************************************************
 *		Private Variables
 *****************************************************************************/
static	_DtCanvasStruct	DefaultCanvas =
  {
	0,		/* int error;                  */
	0,		/* int txt_cnt;                */
	0,		/* int txt_max;                */
	0,		/* int line_cnt;               */
	0,		/* int line_max;               */
	0,		/* int mark_cnt;               */
	0,		/* int mark_max;               */
	0,		/* int trav_cnt;               */
	0,		/* int trav_max;               */
	0,		/* int cur_trav;               */
	0,		/* search_cnt */
	0,		/* search_max */
	0,		/* brk_cnt                     */
	0,		/* brk_max                     */
	1,		/* short mb_length;            */
	0,		/* _DtCvUnit max_x;            */
	0,		/* _DtCvUnit max_y;            */
	_DtCvIGNORE_BOUNDARY,	/* _DtCvValue	constraint; */
	_DtCvFALSE,	/* _DtCvValue	trav_on;            */
	NULL,		/* _DtCvPointer    client_data;     */
	  {		/* CanvasMetrics metrics;           */
	    0,			/* _DtCvUnit width;         */
	    0,			/* _DtCvUnit height;        */
	    0,			/* _DtCvUnit top_margin;    */
	    0,			/* _DtCvUnit line_height;   */
	    0,			/* _DtCvUnit horiz_pad_hint;   */
	  },
	  {		/* _DtCvSpaceMetrics link_info; */
	    0,			/* _DtCvUnit space_before; */
	    0,			/* _DtCvUnit space_after;  */
	    0,			/* _DtCvUnit space_above;  */
	    0,			/* _DtCvUnit space_below;  */
	  },
	  {		/* _DtCvSpaceMetrics traversal_info; */
	    0,			/* _DtCvUnit space_before; */
	    0,			/* _DtCvUnit space_after;  */
	    0,			/* _DtCvUnit space_above;  */
	    0,			/* _DtCvUnit space_below;  */
	  },
	  {		/* _DtCvLocale locale; */
	    _DtCvModeWrapNone,	/* _DtCvModeType  line_wrap_mode;    */
	    NULL,		/* const wchar_t *cant_begin_chars;  */
	    NULL,		/* const wchar_t *cant_end_chars;    */
	  },
	NULL,		/* _DtCvSegment      *element_lst;   */
	NULL,		/* _DtCvDspLine       *txt_lst;       */
	NULL,		/* _DtCvLineSeg  *line_lst;       */
	NULL,		/* _DtCvTraversalInfo *trav_lst;      */
	NULL,		/* _DtCvLinkDb	      link_data; */
	  {		/* CESelection select_start; */
		-1,		/* _DtCvUnit x;           */
		-1, 		/* _DtCvUnit y;           */
		-1,		/* int  line_idx;    */
		-1,		/* int	char_idx;    */
	  },
	  {		/* CESelection select_end; */
		-1,		/* _DtCvUnit x;           */
		-1, 		/* _DtCvUnit y;           */
		-1,		/* int  line_idx;    */
		-1,		/* int	char_idx;    */
	  },
	NULL,		/* _DtCvMarkData	*marks; */
	NULL,		/* searchs */
	NULL,		/* pg_breaks */
	{ NULL },	/* _DtCvVirtualInfo	 virt_functions; */
  };

/*****************************************************************************
 *		Private Functions
 *****************************************************************************/
/*****************************************************************************
 * Function: RenderSubSet
 *
 * Returns:	nothing
 * Purpose:	Render the items next to an item of text.
 *
 *****************************************************************************/
static	void
RenderSubSet (
    _DtCanvasStruct	*canvas,
    _DtCvDspLine	*lines,
    int			 cnt,
    _DtCvUnit		 y1,
    _DtCvUnit		 y2,
    _DtCvUnit		*last_y)
{
    int		i;
    _DtCvUnit	minY;
    _DtCvUnit	maxY;

    /*
     * Loop through the list looking for the item(s) next to the text.
     */
    for (i = 0; NULL != lines && i < cnt; lines++, i++)
      {
	/*
	 * get the minimum and maximum y of the next line
	 */
	minY = lines->baseline - lines->ascent;
	maxY = lines->baseline + lines->descent;

	/*
	 * has this line been reviewed yet?
         * is this line on the 'page'?
         * Does it hang off the 'page' (and if so is it allowed)?
         */
	if (_DtCvIsNotProcessed(*lines) && maxY >= y1 && maxY <= y2)
	  {
	    (void) DrawText (canvas, lines, i, 0, 0);

	    /*
	     * indicate that this line has been rendered.
	     */
	    _DtCvSetProcessed(*lines);

	    /*
	     * is this the maximum that we've rendered?
	     */
	    if (*last_y < maxY)
		*last_y = maxY;

	    /*
	     * now render anything next to this!
	     */
	    RenderSubSet(canvas, canvas->txt_lst, cnt, minY, y2, last_y);
	  }
      }
}

/*****************************************************************************
 * Function: CheckAround
 *
 * Returns:	_DtCvSTATUS_NONE	if no other text is to the side of
 *					this text.
 *		_DtCvFALSE		if other text is to the side, but the
 *					maximum	y position is not violated.
 *		_DtCvTRUE		if other text is to the side and the
 *					maximum y position is violated.
 * Purpose:  Find if another line of text intrudes upon this line.
 *
 *****************************************************************************/
static	_DtCvStatus
CheckAround (
    _DtCvDspLine	*lines,
    int			 cnt,
    int			 idx,
    _DtCvUnit		 y2)
{
    int		i    = 0;
    _DtCvUnit	topY = lines[idx].baseline - lines[idx].ascent;
    _DtCvUnit	botY = lines[idx].baseline + lines[idx].descent;
    _DtCvUnit	minY;
    _DtCvUnit	maxY;
    _DtCvStatus result = _DtCvSTATUS_NONE;

    /*
     * set the processed flag so that we don't test something that's
     * already tested.
     */
    _DtCvSetProcessed(lines[idx]);

    /*
     * go through looking for unprocessed lines to test.
     */
    while (i < cnt && _DtCvTRUE != result)
      {
	if (_DtCvIsNotProcessed(lines[i]))
	  {
	    /*
	     * calculate the minimum and maximum y positions for the line.
	     */
	    minY = lines[i].baseline - lines[i].ascent;
	    maxY = lines[i].baseline + lines[i].descent;

	    /*
	     * Does this line infringe vertically on the test line?
	     */
	    if (maxY > topY && minY < botY &&
		_DtCvTRUE == _DtCvCheckInfringement(topY, botY, minY, maxY))
	      {
		/*
		 * indicate that it is not clear to the side.
		 */
		result = _DtCvFALSE;

	        /*
		 * Does it have something else infringing one it?
		 * Or does it hang down below the test line?
		 */
		if (maxY > y2 || _DtCvTRUE == CheckAround(lines, cnt, i, y2))
		    result = _DtCvTRUE;
	      }
	  }

	i++;
      }

    /*
     * Clear that this line has be processed. Otherwise, rendering will
     * think this has been rendered when it hasn't.
     */
    _DtCvClearProcessed(lines[idx]);

    return result;
}

/*****************************************************************************
 * Function: FindChar
 *
 *    FindChar calculates the char that is x pixels into the string.
 *
 *****************************************************************************/
static	int
FindChar (
    _DtCanvasStruct	*canvas,
    _DtCvSegmentI	*segment,
    void		*string,
    int			 max_len,
    _DtCvUnit		 x_pos,
    _DtCvUnit		*diff)
{
    int     myIndex;
    _DtCvUnit    myDiff = 0;
    _DtCvUnit    len;
    _DtCvUnit    charWidth;
    _DtCvValue triedBack    = False;
    _DtCvValue triedForward = False;

    /*
     * get information about the font used
     */
    _DtCvFontMetrics(canvas, _DtCvFontOfStringSeg(segment),
					NULL, NULL, &charWidth, NULL, NULL);

    /*
     * try to get close to the correct index.
     */
    myIndex = x_pos / charWidth;
    if (myIndex >= max_len)
	myIndex = max_len - 1;
    
    while (!triedBack || !triedForward)
      {
	len = _DtCvGetStringWidth(canvas, segment, string, myIndex + 1);

	if (len > x_pos)
	  {
	    myDiff       = len - x_pos;
	    triedForward = True;
	    if (!triedBack && myIndex)
	 	myIndex--;
	    else
		triedBack = True;
	  }
	else if (len < x_pos)
	  {
	    myDiff    = x_pos - len;
	    triedBack = True;
	    myIndex++;
	    if (myIndex >= max_len)
	      {
		myIndex--;
		triedForward = True;
	      }
	  }
	else /* len == x_pos */
	  {
	    myIndex++;
	    triedBack    = True;
	    triedForward = True;
	    myDiff       = 0;
	  }
      }

    if (diff != NULL)
	*diff = myDiff;

    return (myIndex);
}

/*****************************************************************************
 * Function: DrawCanvasLines
 *
 *****************************************************************************/
static void
DrawCanvasLines(
    _DtCanvasStruct	*canvas,
    _DtCvUnit		 x1,
    _DtCvUnit		 y1,
    _DtCvUnit		 x2,
    _DtCvUnit		 y2,
    _DtCvRenderType	 flag,
    _DtCvUnit		*ret_y,
    _DtCvUnit		*ret_next)
{
    int    i;
    _DtCvUnit	  newY2 = y2;
    _DtCvLineSeg *pLS;
    _DtCvLineInfo lnInfo;

    /*
     * are there any lines?
     */
    if (canvas->line_lst != NULL && canvas->line_cnt &&
				NULL != canvas->virt_functions.render_elem)
      {
	/*
	 * find the maximum y of all the lines that fit in the page.
	 * do this only if the flag indicates whole lines.
	 */
	if (_DtCvRENDER_COMPLETE == flag)
          {
	    for (i = 0, pLS = canvas->line_lst;
					i < canvas->line_cnt; i++, pLS++)
	      {
		/*
		 * Does this line end off the page?
		 */
		if (pLS->max_y > newY2 && pLS->pos_y < newY2)
		    newY2 = pLS->pos_y - 1;
	      }
	  }

	/*
	 * check each line to see if it is on the 'page'
	 */
        for (i = 0, pLS = canvas->line_lst; i < canvas->line_cnt; i++, pLS++)
          {
	    lnInfo.width = pLS->width;
	    lnInfo.data  = pLS->data;
	    lnInfo.x2    = pLS->pos_x;
	    lnInfo.y2    = pLS->pos_y;

	    /*
	     * horizontal or vertial line?
	     */
	    if (_DtCvLINE_HORZ == pLS->dir)
		lnInfo.x2 = pLS->max_x;
	    else
		lnInfo.y2 = pLS->max_y;

	    /*
	     * does it fit on the page? Take into account the rendering type.
	     */
	    if (pLS->max_y >= y1 && pLS->pos_y <= newY2
		&& lnInfo.x2 >= x1 && pLS->pos_x <= x2
		&& (_DtCvRENDER_PARTIAL == flag || pLS->max_y <= newY2))
	      {
		if (pLS->max_y > *ret_y)
		    *ret_y = pLS->max_y;

		(*(canvas->virt_functions.render_elem))(
			canvas->client_data, _DtCvLINE_TYPE,
			pLS->pos_x, pLS->pos_y, -1, 0, 0,
			_DtCvBAD_TYPE, NULL, &lnInfo);
	      }
	    /*
	     * otherwise, would this start the next page?
	     * make sure this is in the horizontal space.
	     */
	    else if (lnInfo.x2 >= x1 && pLS->pos_x <= x2 && pLS->max_y > newY2
				&& (-1 == *ret_next || *ret_next > pLS->pos_y))
		*ret_next = pLS->pos_y;
          }
      }
} /* End DrawCanvasLines */

/******************************************************************************
 * Function: DrawText
 *
 * Parameters:
 *		canvas		Specifies the canvas on which to render
 *				the text.
 *		line		Specifies the line in the line table.
 *		start_x		Specifies the starting x position to use
 *				for selected text. If greater than the
 *				starting position for the segment, determine
 *				the closest character to 'start_x' and
 *				use it.
 *		end_x		Specifies the ending x position to use
 *				for selected text. If -1, means display
 *				the entire set of segments.
 *		old_flag	Specifies....
 *		new_flag	Specifies....
 *
 * Returns:	max_x		Returns the maximum x unit processed.
 *
 * Purpose: DrawText draws text segments on one line in the
 *          line table.
 *
 *****************************************************************************/
static _DtCvUnit
DrawText(
    _DtCanvasStruct	*canvas,
    _DtCvDspLine	*line,
    int			 txt_line,
    _DtCvFlags		 old_flag,
    _DtCvFlags		 new_flag )
{
    int          len;
    int          curIdx  = 0;
    int          lastLnk = -1;
    int          count   = line->length;
    int          start   = line->byte_index;
    _DtCvUnit	 xPos;
    _DtCvUnit	 superWidth = 0;
    _DtCvUnit	 superY     = 0;
    _DtCvUnit	 subWidth   = 0;
    _DtCvUnit	 subY       = 0;
    _DtCvUnit	 scriptX    = 0;
    _DtCvValue	 lastWasSuper = False;
    _DtCvValue	 lastWasSub   = False;
    _DtCvValue	 lastLnkVis   = False;
    _DtCvSegmentI *pSeg;

    xPos = _DtCvGetStartXOfLine(line, &pSeg);

    /*
     * get the corrected x for links and traversals.
     */
    xPos = _DtCvAdvanceXOfLine(canvas, pSeg, xPos, &lastLnk, &lastLnkVis);

    /*
     * take into account the if this is a super or sub script - or not.
     */
    xPos = _DtCvAdjustForSuperSub(canvas, pSeg, xPos,
					&scriptX, &superWidth, &superY,
					&subWidth, &subY,
					&lastWasSuper, &lastWasSub);
    /*
     * now process the line
     */
    while (NULL != pSeg && 0 < count)
      {
	len = count;

	/*
	 * check for selected and marked text.
	 */
	_DtCvCheckLineMarks(canvas, txt_line, curIdx, count, xPos,
				(_DtCvSELECTED_FLAG | _DtCvMARK_FLAG),
				&len, &old_flag, &new_flag);

	/*
	 * if this is the last segment(s) of the (un)selection
	 * set the end flags.
	 */
	if (len == count)
	  {
	    new_flag |= (_DtCvTRAVERSAL_END | _DtCvLINK_END);
	    old_flag |= (_DtCvTRAVERSAL_END | _DtCvLINK_END);
	  }

	/*
	 * render the segment length returned by _DtCvCheckLineMarks
	 */
	xPos = _DtCvDrawSegments(canvas, *line, pSeg, start, len,
				&lastLnk, xPos, xPos,
				&scriptX, &superWidth, &superY, &subWidth,
				&subY, &lastWasSub, &lastWasSuper,
				&lastLnkVis, old_flag, new_flag,
				_DtCvBAD_TYPE, NULL);
	/*
	 * decrement the count by the length processed
	 */
	count  -= len;
	curIdx += len;
	if (0 < count)
		_DtCvSkipLineChars(canvas, pSeg, start, count + len, len,
						&start, &pSeg);
      }

    return xPos;

} /* End DrawText */

/*****************************************************************************
 * Function: IsLineSpecial (
 *
 * Purpose:	Call a virtual function to draw the traversal indicator
 *****************************************************************************/
static	_DtCvValue
IsLineSpecial (
    _DtCvSelectData	start,
    _DtCvSelectData	end,
    _DtCvDspLine	line,
    int			line_idx,
    int			char_idx,
    int			length,
    _DtCvUnit		dst_x,
    int			*ret_len,
    _DtCvFlags		*ret_flag)
{
    _DtCvUnit  maxY = line.baseline + line.descent;
    _DtCvUnit  minY = line.baseline - line.ascent;
    _DtCvFlags flag = 0;

    /*
     * zero out the return flag (which will be a logical OR of
     * the mark flags.
     */
    if (NULL != ret_flag)
	*ret_flag = 0;

    /*
     * initialize the return value to the given inspection length.
     */
    *ret_len = length;

    /*
     * is there anything to look at?
     */
    if (start.y == -1 || maxY < start.y || minY >= end.y)
	return False;

    /*
     * starts the mark/selection?
     */
    if (line_idx == start.line_idx)
      {
	/*
	 * does this segment straddle the start of the mark/selection?
	 */
        if (start.char_idx > char_idx)
	  {
	    /*
	     * draw part(or all) of the segment un-mark/selected.
	     * never return a value larger than the inspection length!
	     */
            if (start.char_idx < char_idx + length)
	        *ret_len = start.char_idx - char_idx;

	    return False;
	  }

	/*
	 * does this segment start the line? Set the start flag if so.
	 */
        if (start.char_idx == char_idx)
	    flag |= _DtCvMARK_BEGIN;

	/*
	 * does this line end the mark/selection?
	 */
	if (line_idx == end.line_idx)
	  {
	    /*
	     * does this line straddle the end?
	     */
	    if (char_idx >= end.char_idx)
	      {
		/*
		 * draw this un mark/selected.
		 * Its after the mark/selected part.
		 */
		return False;
	      }

	    if (char_idx + length > end.char_idx)
	      {
		/*
		 * draw the mark/selected part
		 */
		*ret_len = end.char_idx - char_idx;
	        flag    |= _DtCvMARK_END;
	      }
	  }

	/*
	 * draw the current *ret_len as mark/selected
	 */
      }

    /*
     * does this start the mark/selection?
     */
    else if (line_idx == end.line_idx)
      {
	/*
	 * does not start the mark/selection.
	 * does end the mark/selection.
	 */
	if (char_idx >= end.char_idx)
	    return False;

	/*
	 * straddle the end position?
	 */
	if (char_idx + length > end.char_idx)
	  {
	    *ret_len = end.char_idx - char_idx;
	    flag    |= _DtCvMARK_END;
	  }
	/*
	 * draw the current *ret_len as mark/selected
	 */
      }

    /*
     * start.y != -1
     * start.y <= maxY && minY < end.y
     */
    else if (minY < start.y)
      {
	/*
	 * straddles the start y
	 */
	if (dst_x < start.x)
	    return False;

	/*
	 * dst_x > start.x
	 */
	if (start.y != end.y)
	  {
	    if (NULL != ret_flag)
		*ret_flag = flag;
	    return True;
	  }

	/*
	 * dst_x >= end.x
	 */
	if (dst_x > end.x)
	    return False;
      }
    /*
     * start.y <= minY and maxY
     * minY < end.y
     */
    else if (end.y <= maxY)
      {
	/*
	 * straddles the end y position
	 */
	if (dst_x >= end.x)
	    return False;
      }
    /*
     * start.y <= minY and maxY
     * minY && maxY < end.y
     */
    if (NULL != ret_flag)
	*ret_flag = flag;

    return True;
}

/*****************************************************************************
 * Function: DrawTraversalIndicator (_DtCanvasStruct *canvas, _DtCvValue flag)
 *
 * Purpose: (Un)draws the traversal around the currently active link.
 *****************************************************************************/
static	void
DrawTraversalIndicator (
    _DtCanvasStruct	*canvas,
    _DtCvValue		 render,
    _DtCvValue		 draw_flag,
    _DtCvUnit		*ret_x,
    _DtCvUnit		*ret_y,
    _DtCvUnit		*ret_baseline,
    _DtCvUnit		*ret_height)
{
    int     count;
    int     len;
    int     start;
    int     wrkChr;
    int     totCnt;
    int     travIdx = canvas->cur_trav;
    int     curIdx = 0;
    int     txtLine   = canvas->trav_lst[travIdx].idx;
    int     linkIndex;
    int     lstLnk    = -1;

    _DtCvUnit    height = 0;
    _DtCvUnit    dstX;
    _DtCvUnit    tmpWidth;
    _DtCvUnit    superWidth   = 0;
    _DtCvUnit    superY       = 0;
    _DtCvUnit    subWidth     = 0;
    _DtCvUnit    subY         = 0;
    _DtCvUnit    scriptX      = 0;

    _DtCvFlags   oldFlag = 0;
    _DtCvFlags   newFlag = 0;

    _DtCvValue lastWasSub   = False;
    _DtCvValue lastWasSuper = False;
    _DtCvValue lstLnkVis    = False;

    _DtCvSegmentI	*pSeg;
    _DtCvSegmentI	*tmpSeg;

    /*
     * determine the flags for rendering.
     */
    if (draw_flag)
        newFlag = _DtCvTRAVERSAL_FLAG;
    else
        oldFlag = _DtCvTRAVERSAL_FLAG;

    /*
     * allow traversal to marks.
     */
    if (_DtCvTraversalMark == canvas->trav_lst[travIdx].type)
      {
	int markIdx = canvas->trav_lst[travIdx].idx;
        if (True == render)
	  {
	    oldFlag = oldFlag | _DtCvMARK_FLAG
			      | _DtCvTRAVERSAL_BEGIN | _DtCvTRAVERSAL_END;
	    newFlag = newFlag | _DtCvMARK_FLAG
			      | _DtCvTRAVERSAL_BEGIN | _DtCvTRAVERSAL_END;

	    if (_DtCvTRUE == canvas->marks[markIdx].on)
	      {
	        oldFlag |= _DtCvMARK_ON;
	        newFlag |= _DtCvMARK_ON;
	      }

	    _DtCvDrawAreaWithFlags(canvas,
				canvas->marks[markIdx].beg,
				canvas->marks[markIdx].end,
				oldFlag, newFlag,
				_DtCvMARK_TYPE,
				canvas->marks[markIdx].client_data);
	  }

        if (ret_height)
	    *ret_height = canvas->marks[markIdx].end.y -
		canvas->marks[markIdx].beg.y +
		canvas->txt_lst[canvas->marks[markIdx].end.line_idx].descent +
		canvas->txt_lst[canvas->marks[markIdx].beg.line_idx].ascent;

        /*
         * set some return variables
         */
        if (ret_x)
	    *ret_x = canvas->marks[markIdx].beg.x;

        if (ret_y)
	    *ret_y = canvas->marks[markIdx].beg.y -
		canvas->txt_lst[canvas->marks[markIdx].beg.line_idx].ascent;

        if (ret_baseline)
	    *ret_baseline = canvas->marks[markIdx].beg.y;

	return;
      }

    /*
     * get the link index
     */
    linkIndex = canvas->trav_lst[travIdx].seg_ptr->link_idx;

    /*
     * determine the location of the hypertext segment.
     */
    pSeg  = canvas->trav_lst[travIdx].seg_ptr;
    start = canvas->txt_lst[txtLine].byte_index;
    count = canvas->txt_lst[txtLine].length;

    /*
     * get the start of the line
     */
    dstX  = _DtCvGetStartXOfLine(&(canvas->txt_lst[txtLine]), &pSeg);

    while (pSeg->link_idx != linkIndex)
      {
	/*
	 * get the corrected x
	 */
	dstX = _DtCvAdvanceXOfLine (canvas, pSeg, dstX, &lstLnk, &lstLnkVis);

	/*
	 * move the text x position base on if this is a super or
	 * sub script - or not.
	 */
	dstX = _DtCvAdjustForSuperSub(canvas, pSeg, dstX, &scriptX,
			    &superWidth, &superY, &subWidth, &subY,
			    &lastWasSuper, &lastWasSub);

	/*
	 * get the width of the segment.
	 */
	_DtCvGetWidthOfSegment(canvas, pSeg, start, count,
						    &len, &tmpWidth, NULL);
	dstX += tmpWidth;

	/*
	 * update pointers
	 */
	lstLnk  = pSeg->link_idx;
	count  -= len;
	curIdx += len;
	pSeg    = pSeg->next_disp;
	start   = 0;
      }

    /*
     * set some return variables
     */
    if (ret_x)
	*ret_x = dstX;

    if (ret_y)
	*ret_y = canvas->txt_lst[txtLine].baseline -
					canvas->txt_lst[txtLine].ascent;
    if (ret_baseline)
	*ret_baseline = canvas->txt_lst[txtLine].baseline;

    /*
     * start drawing the traversals
     */
    height = 0;
    if (True == render)
      {
	while (txtLine < canvas->txt_cnt && linkIndex == pSeg->link_idx)
	  {
	    /*
	     * get the corrected x
	     */
	    dstX = _DtCvAdvanceXOfLine (canvas,pSeg, dstX, &lstLnk, &lstLnkVis);

	    /*
	     * move the text x position base on if this is a super or
	     * sub script - or not.
	     */
	    dstX = _DtCvAdjustForSuperSub(canvas, pSeg, dstX, &scriptX,
				&superWidth, &superY, &subWidth, &subY,
				&lastWasSuper, &lastWasSub);

	    /*
	     * now count up the number of bytes to display for
	     * the traversal.
	     */
	    totCnt = count;
	    tmpSeg = pSeg;
	    count  = 0;
	    wrkChr = start;
	    while  (totCnt > 0 && tmpSeg != NULL
					&& tmpSeg->link_idx == linkIndex)
	      {
		_DtCvGetWidthOfSegment(canvas, tmpSeg, wrkChr,
						totCnt, &len, NULL, NULL);
		totCnt    -= len;
		count     += len;
		wrkChr     = 0;
		tmpSeg     = tmpSeg->next_disp;
	      }

	    /*
	     * set the begin flag.
	     */
	    newFlag |= (_DtCvTRAVERSAL_BEGIN | _DtCvLINK_BEGIN);
	    oldFlag |= (_DtCvTRAVERSAL_BEGIN | _DtCvLINK_BEGIN);
	    while (count > 0 && pSeg != NULL && pSeg->link_idx == linkIndex)
	      {
		/*
		 * the original count for the traversal.
		 */
		len = count;

		/*
		 * if there is mark/selected text, determine, how much
		 */
		_DtCvCheckLineMarks(canvas, txtLine, curIdx, count, dstX,
					(_DtCvSELECTED_FLAG | _DtCvMARK_FLAG),
					&len, &oldFlag, &newFlag);
		/*
		 * if this is the last segment(s) of the traversal
		 * set the end flags.
		 */
		if (len == count)
		  {
		    newFlag |= (_DtCvTRAVERSAL_END | _DtCvLINK_END);
		    oldFlag |= (_DtCvTRAVERSAL_END | _DtCvLINK_END);
		  }

		/*
		 * render the segments
		 */
		dstX = _DtCvDrawSegments(canvas, canvas->txt_lst[txtLine],
			    pSeg, start, len, &lstLnk, dstX, dstX,
			    &scriptX,&superWidth,&superY,&subWidth,&subY,
			    &lastWasSub, &lastWasSuper,
			    &lstLnkVis, oldFlag, newFlag,
			    _DtCvLINK_TYPE, NULL);

		count  -= len;
		curIdx += len;
		if (count > 0)
		  {
		    _DtCvSkipLineChars(canvas, pSeg, start, count + len, len,
					&start, &pSeg);
		    newFlag &= ~(_DtCvTRAVERSAL_BEGIN);
		    oldFlag &= ~(_DtCvTRAVERSAL_BEGIN);
		  }
	      }

	    height += canvas->txt_lst[txtLine].ascent
				+ canvas->txt_lst[txtLine].descent;
	    txtLine++;
	    if (txtLine < canvas->txt_cnt)
	      {
		start  = canvas->txt_lst[txtLine].byte_index;
		count  = canvas->txt_lst[txtLine].length;
		curIdx       = 0;
		superWidth   = 0;
		superY       = 0;
		subWidth     = 0;
		subY         = 0;
		scriptX      = 0;
		lstLnk       = -1;
		lastWasSuper = False;
		lastWasSub   = False;
		lstLnkVis    = False;

		/*
		 * get the correct x
		 */
		dstX = _DtCvGetStartXOfLine(&(canvas->txt_lst[txtLine]), &pSeg);
	      }
	  }
      }

    if (ret_height)
	*ret_height = height;

} /* End DrawTraversalIndicator */

/*****************************************************************************
 *		Semi-Public Functions
 *****************************************************************************/
/*****************************************************************************
 * Function: _DtCvGetSearchLineMetrics (
 *
 * Purpose:  gets the text line metrics for the search item.
 *****************************************************************************/
int
_DtCvGetSearchLineMetrics(_DtCvHandle handle, int idx, _DtCvUnit* baseline,
				_DtCvUnit* descent, _DtCvUnit* ascent)
{
    int ret = 0;
    _DtCanvasStruct* canvas = (_DtCanvasStruct*)handle;
    _DtCvDspLine* line;

    if (idx < 0 || idx >= canvas->search_cnt)
	return -1;

    line = &(canvas->txt_lst[canvas->searchs[idx].idx]);

    *baseline = line->baseline;
    *descent  = line->descent;
    *ascent   = line->ascent;

    return ret;
}

/*****************************************************************************
 * Function: _DtCvCheckInfringement (
 *
 * Purpose:  Checks to see if one object infringes vertically on another
 *           object.
 *****************************************************************************/
_DtCvStatus
_DtCvCheckInfringement (
    _DtCvUnit            tst_top,
    _DtCvUnit            tst_bot,
    _DtCvUnit            obj_top,
    _DtCvUnit            obj_bot)
{
    _DtCvStatus  result = False;

    /*
     * check to see if the object is to the left or right of the test
     * object and that it 'infringes' on the vertical space of the test
     * object.
     *
     * I.e.     ----obj_top------
     *          |               |   ----tst_top----
     *          ----obj_bot------   |             |
     *                              ----tst_bot----
     *
     * I.e.                         ----tst_top----
     *          ----obj_top-------  |             |
     *          |                |  ----tst_bot----
     *          ----obj_bot-------
     *
     * I.e.     ----obj_top------
     *          |               |   ----tst_top----
     *          |               |   |             |
     *          |               |   ----tst_bot----
     *          ----obj_bot------
     *
     * I.e.                         ----tst_top----
     *          ----obj_top-------  |             |
     *          |                |  |             |
     *          ----obj_bot-------  |             |
     *                              ----tst_bot----
     */
    if ((obj_top <  tst_top && tst_top <  obj_bot)
         || (obj_top <  tst_bot && tst_bot <  obj_bot)
         || (obj_top <= tst_top && tst_bot <= obj_bot)
         || (tst_top <  obj_top && obj_bot <  tst_bot))
        result = True;

    return result;
}

/*****************************************************************************
 * Function: _DtCvCheckLineMarks (
 *
 * Parameters:
 *		canvas		Specifies the canvas to check for
 *				marks and/or selections.
 *		line_idx	Specifies the line index into the
 *				list of text lines in the canvas.
 *		char_idx	Specifies the starting character index
 *				in the text line.
 *		length		Specifies the length of the text line
 *				to consider.
 *		dst_x		Specifies the x position of the
 *				starting character in the text line.
 *		check_flags	Specifies which type to look for -
 *				selection, marks or both.
 *		ret_len		Returns the length of the text line
 *				starting at the starting character
 *				index for which the flags returned
 *				in ret_old and ret_new are valid.
 *		ret_old, ret_new
 *				Returns the values in ret_old and ret_new
 *				and may add _DtCvSELECTED_FLAG and/or
 *				_DtCvMARK_FLAG.
 *
 * Purpose:	Find out how much of the line is (un)marked in some way.
 *****************************************************************************/
void
_DtCvCheckLineMarks (
    _DtCanvasStruct	*canvas,
    int			line_idx,
    int			char_idx,
    int			length,
    _DtCvUnit		dst_x,
    _DtCvFlags		check_flags,
    int			*ret_len,
    _DtCvFlags		*ret_old,
    _DtCvFlags		*ret_new)
{
    int         i;
    _DtCvFlags	flag = 0;

    /*
     * check the selection
     */
    if ((check_flags & _DtCvSELECTED_FLAG) && canvas->select_start.y != -1)
      {
	_DtCvSelectData  start = canvas->select_start;
	_DtCvSelectData  end   = canvas->select_end;

	/*
	 * check to see if we need to switch the selection points
	 */
	if (start.y > end.y || (start.y == end.y && start.x > end.x))
	  {
	    end   = canvas->select_start;
	    start = canvas->select_end;
	  }

	/*
	 * clear the selected flag
	 */
	*ret_old &= ~(_DtCvSELECTED_FLAG);
	*ret_new &= ~(_DtCvSELECTED_FLAG);

	if (IsLineSpecial(start, end,
				canvas->txt_lst[line_idx], line_idx,
				char_idx, length, dst_x,
				&length, NULL))
	  {
	    /*
	     * set the selected flag.
	     */
	    *ret_old = *ret_old | _DtCvSELECTED_FLAG;
	    *ret_new = *ret_new | _DtCvSELECTED_FLAG;
	  }
      }

    if ((check_flags & _DtCvMARK_FLAG) && 0 < canvas->mark_cnt)
      {
	/*
	 * strip the mark flags from the old and new flags
	 */
	*ret_old &= ~(_DtCvMARK_FLAG | _DtCvMARK_BEGIN |
						_DtCvMARK_END | _DtCvMARK_ON);
	*ret_new &= ~(_DtCvMARK_FLAG | _DtCvMARK_BEGIN |
						_DtCvMARK_END | _DtCvMARK_ON);

	/*
	 * now add the correct flags into the old/new flags
	 */
	for (i = 0; i < canvas->mark_cnt; i++)
	  {
	    if (IsLineSpecial(canvas->marks[i].beg, canvas->marks[i].end,
				canvas->txt_lst[line_idx], line_idx,
				char_idx, length, dst_x,
				&length, &flag))
	      {
		/*
		 * A false return from IsLineSpecial means that 'length'
		 * is outside this mark.
		 *
		 * When true, it means that some part of this mark will
		 * be rendered on the call. Therefore set the mark flag
		 * and any other flags returned and check for mark 'on'.
		 */
		if (_DtCvTRUE == canvas->marks[i].on)
		    flag |= _DtCvMARK_ON;

		*ret_old = *ret_old | _DtCvMARK_FLAG | flag;
		*ret_new = *ret_new | _DtCvMARK_FLAG | flag;
	      }
	  }
      }

    /*
     * return the next length that is marked/unmarked in someway.
     */
    *ret_len = length;
}

/******************************************************************************
 * Function: _DtCvSkipLineChars
 *
 * Parameters:
 *		canvas		Specifies the canvas on which to render
 *				the text.
 *
 * Purpose: Given a length, skip ahead that number of 'characters' on
 *          the line.
 *****************************************************************************/
void
_DtCvSkipLineChars(
    _DtCanvasStruct	*canvas,
    _DtCvSegmentI	*p_seg,
    int			 start,
    int			 max_cnt,
    int			 use_len,
    int			*ret_start,
    _DtCvSegmentI	**ret_seg)
{
    int  len;

    /*
     * not all of the traversal line was displayed because
     * part of it is selected. So skip what's been rendered,
     * and do it again.
     */
    while (use_len > 0)
      {
	/*
	 * get the byte length of the segment processed.
	 */
	_DtCvGetWidthOfSegment(canvas, p_seg, start, max_cnt, &len, NULL, NULL);
	/*
	 * increment the start index by the number of total
	 * bytes processed. If this is more that what is in
	 * the segment, then the if stmt will catch this and
	 * set the start index to zero.
	 */
	if (len > use_len)
	  {
	    len    = use_len;
	    start += len;
	  }
	else /* if (len <= use_len) */
	  {
	    start = 0;
	    p_seg = p_seg->next_disp;
	  }

	/*
	 * reduce the total number of bytes
	 * processed by the number in this segment.
	 */
	use_len -= len;
	max_cnt -= len;
      }

    *ret_start = start;
    *ret_seg   = p_seg;
}

/******************************************************************************
 * Function: _DtCvClearInternalUse
 *
 * Init every internal_use pointer on containers to NULL.
 *****************************************************************************/
void
_DtCvClearInternalUse(
    _DtCvSegmentI	*list,
    _DtCvStatus		 flag)
{
    while (NULL != list)
      {
	/*
	 * initialize the internal variables
	 */
	list->internal_use = (void *) -1;

	if (_DtCvIsSegContainer(list))
	    _DtCvClearInternalUse(_DtCvContainerListOfSeg(list), flag);

        list = list->next_seg;
      }
}

/******************************************************************************
 * Function: _DtCvGetCharIdx
 *
 * Parameters:
 *		canvas		Specifies the canvas on which to render
 *				the text.
 *		line		Specifies the line in the line table.
 *		find_x		Specifies the x position of the character.
 *
 * Returns:	??		Returns the idx of the character.
 *
 * Purpose:
 *****************************************************************************/
int
_DtCvGetCharIdx(
    _DtCanvasStruct	*canvas,
    _DtCvDspLine	 line,
    _DtCvUnit		 find_x)
{
    void	*pChar;
    _DtCvValue	 done         = FALSE;
    _DtCvValue	 lastLinkVisible = FALSE;
    int          count        = line.length;
    int          start        = line.byte_index;
    int          len    = -1;
    int		 lnkInd = -1;
    _DtCvUnit	 segWidth;
    _DtCvUnit	 xPos;
    _DtCvSegmentI   *pSeg;

    xPos = _DtCvGetStartXOfLine(&line, &pSeg);

    /*
     * check to see if the start is in the middle of the line.
     * If so, bump the x position and start indexes to the
     * correct locations.
     */
    while (!done && find_x > xPos && count > 0)
      {
	xPos = _DtCvAdvanceXOfLine(canvas, pSeg, xPos,
						&lnkInd, &lastLinkVisible);

	if (xPos < find_x)
	  {
            /*
             * advance the pointer by the width
             */
            _DtCvGetWidthOfSegment(canvas, pSeg, start, count,
							&len, &segWidth, NULL);
	    if (segWidth + xPos <= find_x)
	      {
	        xPos  += segWidth;
	        pSeg   = pSeg->next_disp;
	        count -= len;
	        start  = 0;
	      }
            else  /* if (xPos < find_x && find_x < xPos + segWidth) */
              {
	        if (_DtCvIsSegString(pSeg))
	          {
                    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg),
					_DtCvIsSegWideChar(pSeg), start);
		    len   = _DtCvStrLen (pChar, _DtCvIsSegWideChar(pSeg));

	            if (len > count)
		        len = count;

	            count -= FindChar(canvas, pSeg, pChar, len,
							find_x - xPos, NULL);
                  }
                done = True;
              }
          }

	len = line.length - count;
      }

    return len;
}

/*****************************************************************************
 * Function: _DtCvGetStartXOfLine
 *
 * Purpose: Get the starting 'x' of the specified line
 *          Does *not* take into account traversal or link info.
 *****************************************************************************/
_DtCvUnit
_DtCvGetStartXOfLine (
    _DtCvDspLine	 *line,
    _DtCvSegmentI	**p_seg)
{
    *p_seg = line->seg_ptr;

    return line->text_x;
}

/*****************************************************************************
 * Function: _DtCvAdvanceXOfLine
 *
 * Purpose: Move the 'x' to after the traversal and link info.
 *****************************************************************************/
_DtCvUnit
_DtCvAdvanceXOfLine (
    _DtCanvasStruct	*canvas,
    _DtCvSegmentI	*p_seg,
    _DtCvUnit		 x_pos,
    int			*link_idx,
    _DtCvValue		*link_flag)
{
    _DtCvValue	junk;

    /*
     * take into account the link before and after space
     */
    junk = _DtCvIsSegVisibleLink(p_seg);
    *link_flag = _DtCvModifyXpos (canvas->link_info, p_seg, junk,
                        *link_flag, *link_idx, &x_pos);

    /*
     * take into account the traversal before and after space
     */
    junk = _DtCvIsSegALink(p_seg);
    (void) _DtCvModifyXpos (canvas->traversal_info, p_seg, junk,
                        ((_DtCvValue) True), *link_idx, &x_pos);

    *link_idx = p_seg->link_idx;

    return x_pos;
}

/******************************************************************************
 * Function: _DtCvGetWidthOfSegment
 *
 *  DetermineWidthOfSegment determines the width of the segment.
 *  The segment must have been already initialized with the correct
 *  font (for strings), the spc resolve, the graphic loaded, etc.
 *
 *****************************************************************************/
void
_DtCvGetWidthOfSegment(
    _DtCanvasStruct	*canvas,
    _DtCvSegmentI	*p_seg,
    int			 start,
    int			 max_cnt,
    int			*ret_cnt,
    _DtCvUnit		*ret_w,
    _DtCvValue		*ret_trimmed)
{
    void	*pChar;

    /*
     * return the width of the segment.
     */
    *ret_cnt = 0;
    if (ret_w != NULL)
        *ret_w = 0;

    if (ret_trimmed != NULL)
        *ret_trimmed = False;

    if (!(_DtCvIsSegNoop(p_seg)))
      {
        if (_DtCvIsSegRegion(p_seg))
          {
	    *ret_cnt = 1;
	    if (ret_w != NULL)
                *ret_w = _DtCvWidthOfRegionSeg(p_seg);
          }
	else 
	  {
            pChar    = _DtCvStrPtr(_DtCvStringOfStringSeg(p_seg),
					_DtCvIsSegWideChar(p_seg), start);
	    *ret_cnt = _DtCvStrLen (pChar, _DtCvIsSegWideChar(p_seg));
            if (*ret_cnt > max_cnt)
	      {
	        *ret_cnt = max_cnt;
		if (ret_trimmed != NULL)
		    *ret_trimmed = True;
	      }

	    /*
	     * determine the width of the string.
	     */
	    if (ret_w != NULL)
                *ret_w = _DtCvGetStringWidth(canvas, p_seg,pChar,*ret_cnt);
          }
      }
}

/******************************************************************************
 * Function: _DtCvModifyXpos
 *****************************************************************************/
_DtCvValue
_DtCvModifyXpos (
    _DtCvSpaceMetrics	 info,
    _DtCvSegmentI	*seg,
    _DtCvValue		 tst_result,
    _DtCvValue		 cur_flag,
    int			 last_idx,
    _DtCvUnit		*x)
{
    int    addx = 0;

    /*
     * take into account the link before and after space
     */
    if (tst_result)
      {
	/*
	 * Ignore if the same link
	 */
	if (last_idx != seg->link_idx)
	  {
	    /*
	     * if one link followed by another add the space after.
	     */
	    if (last_idx != -1)
		addx = info.space_after;

	    /*
	     * add the space before the link
	     */
	    addx += info.space_before;
	  }
	cur_flag = True;
      }
    else
      {
	if (last_idx != -1 && cur_flag == True)
	    addx = info.space_after;
	cur_flag = False;
      }

    *x += addx;
    return cur_flag;
}

/*****************************************************************************
 * Function: _DtCvAdjustForSuperSub
 *
 * Parameters:
 *		canvas		Specifies the canvas.
 *		start_x		Specifies the current text x position.
 *              script_x        Specifies the current super and sub
 *				scripting x position.  Returns the same
 *				value as start_x if the segment is not a
 *				super or sub script.
 *		super_width     Specifies the width of the previously
 *				rendered super script.  Set to 0 if the
 *				next segment is not a super or sub
 *				script.
 *		super_y         Specifies the y offset for super
 *				scripts.  Set to a new value if the last
 *				segment was not a super or sub script.
 *		sub_width       Specifies the width of the previously
 *				rendered sub script.  Set to 0 if the
 *				next segment is not a super or sub
 *				script.
 *		sub_y           Specifies the y offset for sub scripts.
 *				Set to a new value if the last segment
 *				was not a super or sub script.
 *		last_was_super	Specifies if the last item was a super
 *				script. Set to False if the segment
 *				is not a super or sub script.
 *		last_was_sub	Specifies if the last item was a sub
 *				script. Set to False if the segment
 *				is not a super or sub script.
 * Returns: new text x positon.
 *
 * Purpose: Determines the super and sub scripting positions for text.
 *	    If the last item was not a script, then the base offset for
 *	    scripting (script_x) is moved to start_x. If the current
 *	    item is a string, its scripting y position is determined
 *	    (super_y and sub_y).  If the new item is a super or sub
 *	    script, the next text placement (start_x) is moved to after
 *	    the script_x plus the super or sub script size currently
 *	    active(super_width and sub_width).  Otherwise, the the flags
 *	    are set to false and the widths are set to 0.
 *
 *****************************************************************************/
_DtCvUnit
_DtCvAdjustForSuperSub(
    _DtCanvasStruct	*canvas,
    _DtCvSegmentI	*pSeg,
    _DtCvUnit		 start_x,
    _DtCvUnit		*script_x,
    _DtCvUnit		*super_width,
    _DtCvUnit		*super_y,
    _DtCvUnit		*sub_width,
    _DtCvUnit		*sub_y,
    _DtCvValue		*last_was_super,
    _DtCvValue		*last_was_sub)
{
    /*
     * if the last item was not a super or sub script,
     * move the script x to the end of the last output.
     */
    if (!(*last_was_super || *last_was_sub))
        *script_x = start_x;

    /*
     * check for super and sub scripts.
     * adjust text x positioning accordingly.
     */
    if (_DtCvIsSegSuperScript(pSeg))
      {
        start_x         = *script_x + *super_width;
        *last_was_super = True;
      }
    else if (_DtCvIsSegSubScript(pSeg))
      {
        start_x       = *script_x + *sub_width;
        *last_was_sub = True;
      }
    else if (*last_was_super || *last_was_sub)
      {
        *sub_width      = 0;
        *super_width    = 0;
        *last_was_super = False;
        *last_was_sub   = False;
      }

    /*
     * if this wasn't a super or sub script, find out where
     * they get placed on this string.
     */
    if (!(*last_was_super || *last_was_sub))
      {
	if (_DtCvIsSegString(pSeg))
	    _DtCvFontMetrics (canvas,_DtCvFontOfStringSeg(pSeg),
				NULL, NULL, NULL, super_y, sub_y);
	else if (_DtCvIsSegRegion(pSeg))
	  {
	    *super_y = _DtCvHeightOfRegionSeg(pSeg) * 4 / 10;
	    *sub_y   = *super_y;
	  }
      }

    return start_x;
}

/******************************************************************************
 * Function: _DtCvDrawSegments
 *
 * Parameters:
 *		canvas		Specifies the canvas on which to render
 *				the text.
 *		line		Specifies the line metrics.
 *		p_seg		Specifies the starting segment.
 *		start_char	Specifies the starting index in a string
 *				segment. 0 for all others.
 *              count           Specifies the number of characters
 *				(including special characters to
 *				render).
 *              prev_lnk        Indicates the previous link index.  Used
 *				to calculate extra spacing needed for
 *				traversal and link markup.
 *              txt_x           Specifies the starting x of the
 *				segment(s).  This does *NOT* take into
 *				account traversal or link spacing.  This
 *				routine will do that.  This is so
 *				selected links will have correct spacing
 *				indicated.
 *              sel_x           Specifies where the selection x position
 *				begins.  Usually it equals txt_x, but
 *				sometimes it will be less than it to
 *				indicate blank space has been selected.
 *		super_width	Specifies the last super script x offset.
 *		super_y		Specifies the last super script y offset.
 *		sub_width		Specifies the last sub script x offset.
 *		sub_y		Specifies the last sub script y offset.
 *		last_was_sub	Specifies if the last element was a
 *				subscript.
 *		last_was_super	Specifies if the last element was a
 *				superscript.
 *              last_was_vis    Specifies if the last element was a
 *				visible hypertext link.
 *		old_flag	Specifies what the line use to look like.
 *		new_flag	Specifies what the line is to look like.
 *
 * Returns:	txt_x		Returns the maximum x unit processed.
 *
 * Purpose: _DtCvDrawSegments draws one or more segments based on
 *	    the count passed in.
 *
 *          This routine adds CELink to new_flag when rendering segments
 *	    that are hypertext links.  At the same time it will
 *	    determine the correct window hint and may place in old_flag
 *	    and new_flag either _DtCvLINK_POP_UP or _DtCvLINK_NEW_WINDOW.
 *
 *          This routine strips the _DtCvTRAVERSAL_END from old_flag and
 *	    new_flag (based on what's in new_flag).  It will restore
 *	    these flags (if specified) when it renders the last element
 *	    in the count sequence.
 *
 *****************************************************************************/
_DtCvUnit
_DtCvDrawSegments(
    _DtCanvasStruct	*canvas,
    _DtCvDspLine	 line,
    struct _dtCvSegment	*p_seg,
    int			 start_char,
    int			 count,
    int			*prev_lnk,
    _DtCvUnit		 txt_x,
    _DtCvUnit		 sel_x,
    _DtCvUnit		*script_x,
    _DtCvUnit		*super_width,
    _DtCvUnit		*super_y,
    _DtCvUnit		*sub_width,
    _DtCvUnit		*sub_y,
    _DtCvValue		*last_was_sub,
    _DtCvValue		*last_was_super,
    _DtCvValue		*last_link_vis,
    _DtCvFlags		 old_flag,
    _DtCvFlags		 new_flag,
    _DtCvElemType	 trav_type,
    _DtCvPointer	 trav_data )
{
    int		 linkType = 0;
    int          len;
    short	 cropped  = _DtCvFALSE;
    short	 image_offset = _DtCvFALSE;
    _DtCvUnit	 segWidth;
    _DtCvUnit	 yPos;
    void	*pChar;
    _DtCvFlags	 saveEnd = new_flag &
	(_DtCvTRAVERSAL_END | _DtCvLINK_END | _DtCvMARK_END);
    _DtCvElemType   elemType;
    _DtCvRenderInfo posInfo;
    _DtCvStringInfo strInfo;

    /*
     * skip any leading no-op lines
     */
    while (p_seg != NULL && _DtCvIsSegNoop(p_seg))
      {
	start_char = 0;
	p_seg  = p_seg->next_disp;
      }

    /*
     * strip the any end info from the flags.
     * it will be put back on with the last element that makes up the count.
     */
    old_flag &= ~saveEnd;
    new_flag &= ~saveEnd;

    /*
     * now process the segments included in 'count'
     */
    while (p_seg != NULL && count > 0)
      {
	/*
	 * reset the baseline.
	 * when processing super or sub scripts, this gets changed.
	 */
	yPos = line.baseline;

	/*
	 * take into account the visible link and traversal info.
	 */
	txt_x = _DtCvAdvanceXOfLine(canvas, p_seg, txt_x,
						prev_lnk, last_link_vis);

	/*
	 * check for super and sub scripts.
	 * adjust text x positioning accordingly.
	 */
	txt_x = _DtCvAdjustForSuperSub(canvas, p_seg, txt_x,
					script_x, super_width, super_y,
					sub_width, sub_y,
					last_was_super, last_was_sub);
	if (sel_x > txt_x)
	    sel_x = txt_x;

	/*
	 * set visible link indicator flags
	 */
	_DtCvClearLinkFlags(old_flag);
	_DtCvClearLinkFlags(new_flag);

	/*
	 * is this a visible link?
	 */
	if (_DtCvIsSegVisibleLink(p_seg))
	  {
	    /*
	     * visible link - set the flags.
	     */
	    new_flag |= _DtCvLINK_FLAG;
	    old_flag |= _DtCvLINK_FLAG;

	    /*
	     * is this the start of a new link? If so, set the begin flag.
	     */
	    if (*prev_lnk != p_seg->link_idx)
	      {
		new_flag |= _DtCvLINK_BEGIN;
		old_flag |= _DtCvLINK_BEGIN;
	      }

	    /*
	     * get the link type and set the window hint.
	     */
	    linkType  = _DtLinkDbGetLinkType(canvas->link_data,p_seg->link_idx);
	    switch (_DtLinkDbGetHint(canvas->link_data, p_seg->link_idx))
	      {
		case _DtCvWindowHint_PopupWindow:
				new_flag |= _DtCvLINK_POP_UP;
				old_flag |= _DtCvLINK_POP_UP;
				break;
		case _DtCvWindowHint_NewWindow:
				new_flag |= _DtCvLINK_NEW_WINDOW;
				old_flag |= _DtCvLINK_NEW_WINDOW;
				break;
	      }
	  }
	
    	/*
	 * rememeber this link index.
         */
        *prev_lnk = p_seg->link_idx;

	/*
	 * set the search flag
	 */
	_DtCvClearSearchFlags(old_flag);
	_DtCvClearSearchFlags(new_flag);

	old_flag |= (p_seg->type & _DtCvSEARCH_FLAG);
	new_flag |= (p_seg->type & _DtCvSEARCH_FLAG);

	if (0 == start_char)
	  {
	    _DtCvSetSearchBegin(old_flag, p_seg);
	    _DtCvSetSearchBegin(new_flag, p_seg);
	    
	  }

	old_flag &= ~_DtCvAPP_FLAG2;
	new_flag &= ~_DtCvAPP_FLAG2;
	old_flag |= p_seg->type & _DtCvAPP_FLAG2;
	new_flag |= p_seg->type & _DtCvAPP_FLAG2;

	old_flag &= ~(_DtCvAPP_FLAG3 | _DtCvAPP_FLAG4);
	new_flag &= ~(_DtCvAPP_FLAG3 | _DtCvAPP_FLAG4);

	/*
	 * init some variables
	 */
	segWidth = 0;
	len      = 0;

	/*
	 * adjust the yPos for sub/superscripts.
	 */
	if (_DtCvIsSegSuperScript(p_seg))
	    yPos -= *super_y;
	else if (_DtCvIsSegSubScript(p_seg))
	    yPos += *sub_y;

	elemType = -1;
	switch (_DtCvPrimaryTypeOfSeg(p_seg))
	  {
	    case _DtCvSTRING:
		/*
		 * set the type
		 */
		elemType = _DtCvSTRING_TYPE;

		/*
		 * get the string and its length.
		 */
                pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(p_seg),
					_DtCvIsSegWideChar(p_seg), start_char);
	        len   = _DtCvStrLen (pChar, _DtCvIsSegWideChar(p_seg));

		/*
		 * if length of the string is longer than we want to
		 * output, crop.
		 */
	        if (len > count)
		  {
	            len     = count;
		    cropped = _DtCvTRUE;
		  }

		/*
		 * initialize the string structure.
		 */
		strInfo.string   = pChar;
		strInfo.byte_len = len;
		strInfo.wc       = _DtCvIsSegWideChar(p_seg);
		strInfo.font_ptr = _DtCvFontOfStringSeg(p_seg);
		strInfo.csd      = p_seg->client_use;

		/*
		 * now get the width of the string to update the x positions
		 */
		segWidth     = _DtCvGetStringWidth(canvas, p_seg, pChar, len);

		/*
		 * attach the string information to the position info.
		 */
		posInfo.info = &strInfo;

		/*
		 * reset starting index.
		 */
	        start_char   = 0;
		break;

	    case _DtCvREGION:
		/*
		 * set the type, length and width
		 */
		elemType = _DtCvREGION_TYPE;
		len      = 1;
		segWidth = _DtCvWidthOfRegionSeg(p_seg);

		/*
		 * attach the region information to the position info.
		 */
		posInfo.info       = _DtCvInfoOfRegionSeg(p_seg);

		break;
	  }

	/*
	 * do we have valid information?
	 */
	if (-1 != elemType)
	  {
	    /*
	     * now set up the position information
	     */
	    posInfo.box_x      = sel_x;
	    posInfo.box_y      = line.baseline - line.ascent;
	    posInfo.box_height = line.ascent + line.descent + 1;
	    posInfo.box_width  = segWidth;

	    /*
	     * if this is the last segment to be rendered,
	     * restore the end flags.
	     */
	    if (len == count)
	      {
	        new_flag |= saveEnd;
	        old_flag |= saveEnd;

	      }

	    /*
	     * if the item (string) was not cropped, set the Search end
	     * flag accordingly.
	     */
	    if (_DtCvFALSE == cropped)
	      {
		_DtCvSetSearchEnd(old_flag, p_seg);
		_DtCvSetSearchEnd(new_flag, p_seg);

		if (new_flag & _DtCvSEARCH_END && new_flag & _DtCvAPP_FLAG2 &&
		    !(new_flag & _DtCvSELECTED_FLAG)) {
		    new_flag |= _DtCvAPP_FLAG4;
		}
	      }

	    if (image_offset == _DtCvTRUE) {
		old_flag |= _DtCvAPP_FLAG3;
		new_flag |= _DtCvAPP_FLAG3;
		image_offset = _DtCvFALSE;
	    }

	    /*
	     * render the element
	     */
	    if (NULL != canvas->virt_functions.render_elem)
	        (*(canvas->virt_functions.render_elem))(
				canvas->client_data, elemType,
				txt_x, yPos,
				linkType, old_flag, new_flag,
				trav_type, trav_data, &posInfo);

	    if (cropped == _DtCvFALSE && new_flag & _DtCvSEARCH_END
				      && new_flag & _DtCvAPP_FLAG2
				      && !(new_flag & _DtCvSELECTED_FLAG))
		image_offset = _DtCvTRUE;
	  }

        /*
         * strip the any begin flags.
         */
	_DtCvRemoveBeginFlags(old_flag);
	_DtCvRemoveBeginFlags(new_flag);

	/*
	 * take into account subscripting and superscripting.
	 */
	if (_DtCvIsSegSuperScript(p_seg))
	    *super_width += segWidth;
	else if (_DtCvIsSegSubScript(p_seg))
	    *sub_width += segWidth;

	/*
	 * adjust the pointers
	 */
	txt_x += segWidth;
	sel_x  = txt_x;
	count -= len;

	p_seg = p_seg->next_disp;
      }

    return txt_x;

} /* End _DtCvDrawSegments */

/*****************************************************************************
 *		Public Functions
 *****************************************************************************/
/*****************************************************************************
 * Function:	_DtCvHandle _DtCanvasCreate (_DtCvVirtualInfo virt_info);
 *
 * Parameters:
 *		virt_info	Specifies the virtual functions to attach
 *				to the created canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:	Create a canvas and attach the appropriate virtual functions
 *		to the canvas.
 *****************************************************************************/
_DtCvHandle
_DtCanvasCreate (
    _DtCvVirtualInfo	virt_info,
    _DtCvPointer	client_data)
{
    _DtCanvasStruct	*newCanvas;

    newCanvas = (_DtCanvasStruct *) malloc (sizeof (_DtCanvasStruct));
    if (newCanvas == NULL)
	return NULL;

    *newCanvas                = DefaultCanvas;
    newCanvas->virt_functions = virt_info;
    newCanvas->client_data    = client_data;
    newCanvas->mb_length      = MB_CUR_MAX;

    /*
     * load the metrics
     */
    _DtCanvasLoadMetrics((_DtCvHandle)newCanvas);

    return ((_DtCvHandle)(newCanvas));

} /* End _DtCanvasCreate */

void _DtCanvasLoadMetrics(_DtCvHandle handle)
{
    _DtCanvasStruct *canvas = (_DtCanvasStruct*)handle;
    (*(canvas->virt_functions.get_metrics))(canvas->client_data,
					_DtCvCANVAS_TYPE, &(canvas->metrics));
    (*(canvas->virt_functions.get_metrics))(canvas->client_data,
					_DtCvLINK_TYPE, &(canvas->link_info));
    (*(canvas->virt_functions.get_metrics))(canvas->client_data,
			_DtCvTRAVERSAL_TYPE, &(canvas->traversal_info));
    if (MB_CUR_MAX > 1)
        (*(canvas->virt_functions.get_metrics))(canvas->client_data,
			_DtCvLOCALE_TYPE, &(canvas->locale));
}

/*****************************************************************************
 * Function:	void _DtCanvasClean (_DtCvHandle canvas_handle);
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:	Create a canvas and attach the appropriate virtual functions
 *		to the canvas.
 *****************************************************************************/
void
_DtCanvasClean (_DtCvHandle canvas_handle)
{
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;

    /*
     * clean the selection
     */
    _DtCanvasProcessSelection (canvas_handle, 0, 0, _DtCvSELECTION_CLEAR);

    /*
     * zero the lists
     */
    canvas->txt_cnt  = 0;
    canvas->line_cnt = 0;
    canvas->mark_cnt = 0;
    canvas->trav_cnt = 0;
    canvas->search_cnt = 0;
    canvas->brk_cnt    = 0;

    /*
     * reset some indicators
     */
    canvas->error    = 0;
    canvas->cur_trav = -1;

    /*
     * free the internal use structures.
     */
    _DtCvClearInternalUse(canvas->element_lst, _DtCvTRUE);
    canvas->element_lst = NULL;
    canvas->link_data   = NULL;

} /* End _DtCanvasClean */

/*****************************************************************************
 * Function:	void _DtCanvasDestroy (_DtCvHandle canvas_handle);
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:	Create a canvas and attach the appropriate virtual functions
 *		to the canvas.
 *****************************************************************************/
void
_DtCanvasDestroy (_DtCvHandle canvas_handle)
{
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;

    _DtCanvasClean (canvas_handle);

    if (NULL != canvas->txt_lst)
        free ((void *) canvas->txt_lst);
    if (NULL != canvas->line_lst)
        free ((void *) canvas->line_lst);
    if (NULL != canvas->trav_lst)
        free ((void *) canvas->trav_lst);
    if (NULL != canvas->marks)
      free ((void*) canvas->marks);
    if (NULL != canvas->searchs)
      free ((void*) canvas->searchs);
    if (NULL != canvas->pg_breaks)
      free ((void*) canvas->pg_breaks);
      
    free ((void *) canvas);
    return;

} /* End _DtCanvasDestroy */

/*****************************************************************************
 * Function:	void _DtCanvasRender (_DtCvHandle canvas_handle);
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtCanvasRender (
    _DtCvHandle		 canvas_handle,
    _DtCvUnit		 x1,
    _DtCvUnit		 y1,
    _DtCvUnit		 x2,
    _DtCvUnit		 y2,
    _DtCvRenderType	 flag,
    _DtCvValue		 pg_break,
    _DtCvUnit		*max_y,
    _DtCvUnit		*next_y)
{
    int			 i        = 0;
    _DtCvUnit		 lastY    = 0;
    _DtCvUnit		 nextY    = -1;
    _DtCvUnit		 minY;
    _DtCvUnit		 maxY;
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;
    _DtCvDspLine	*lines;
    _DtCvFlags		 sideCk;

    /*
     * check the list of page breaks, it may constrain y2
     */
    if (_DtCvTRUE == pg_break && 0 != canvas->brk_cnt)
      {
	i = 0;
	while (y1 > canvas->pg_breaks[i]) i++;

	if (i < canvas->brk_cnt && y2 > canvas->pg_breaks[i])
	    y2 = canvas->pg_breaks[i];
      }

    /*
     * Draw the lines first, they may constrain y2
     */
    DrawCanvasLines (canvas, x1, y1, x2, y2, flag, &lastY, &nextY);

    if (-1 != nextY && y2 > nextY)
	y2 = nextY - 1;

    /*
     * clear the processed flag from all the text lines.
     */
    for (i = 0; i < canvas->txt_cnt; i++)
	_DtCvClearProcessed(canvas->txt_lst[i]);

    for (lines = canvas->txt_lst, i = 0;
			NULL != lines && i < canvas->txt_cnt; lines++, i++)
      {
	/*
	 * get the minimum and maximum y of the next line
	 */
	minY = lines->baseline - lines->ascent;
	maxY = lines->baseline + lines->descent;

	/*
	 * is this line on the 'page'?
	 * Does it hang off the 'page' (and if so is it allowed)?
	 */
	sideCk = _DtCvSTATUS_NONE;
	if (_DtCvIsNotProcessed(*lines) && maxY >= y1 && minY <= y2 &&
	    (_DtCvRENDER_PARTIAL == flag ||
		(maxY <= y2 &&
		_DtCvTRUE != (sideCk = CheckAround(canvas->txt_lst, canvas->txt_cnt, i, y2)))))
	  {
	    (void) DrawText (canvas, lines, i, 0, 0);

	    /*
	     * indicate that this line has been rendered.
	     */
	    _DtCvSetProcessed(*lines);

	    /*
	     * if doing complete printing, get any other lines that exist
	     * next to this one, but don't fit the [y1,y2] pair. This will
	     * catch scrolling problems using _DtCvRENDER_COMPLETE.
	     * 
	     * The previous CheckAround() call will have set sideCk to
	     * _DtCvFALSE if there are other items to the side, but these
	     * items did not violate the maximum y.
	     *
	     * sideCk will be _DtCvSTATUS_NONE if there is nothing to the
	     * side for _DtCvRENDER_COMPLETE or if flag is _DtCvRENDER_PARTIAL.
	     */
	    if (_DtCvFALSE == sideCk)
	        RenderSubSet(canvas, canvas->txt_lst, canvas->txt_cnt,
						minY, maxY, &lastY);

	    /*
	     * is this the maximum that we've rendered?
	     */
	    if (lastY < maxY)
	        lastY = maxY;
	  }
	/*
	 * otherwise, would this 'start' the next 'page'?
	 *
	 * a) the render type is _DtCvRENDER_PARTIAL but the top of the
	 *    text(minY) is beyound y2 (and so would maxY).
	 * b) the render type is _DtCvRENDER_COMPLETE and the line is
	 *    split across a page boundary (maxY greater than y2).
	 * c) the render type is _DtCvRENDER_COMPLETE and there is text
	 *    to the side of this text and it is split across a page
	 *    boundary (sideCk == _DtCvTRUE).
	 */
	else if ((-1 == nextY || nextY > minY) &&
					(maxY > y2 || _DtCvTRUE == sideCk))
	    nextY = minY;
      }

    /*
     * if doing _DtCvRENDER_PARTIAL, lastY will end up larger than
     * actually rendered. So set it back.
     */
    if (lastY > y2)
	lastY = y2;

    /*
     * return the values if the user asked for them.
     */
    if (NULL != max_y)
	*max_y = lastY;

    if (NULL != next_y)
	*next_y = nextY;

} /* End _DtCanvasRender */

/*****************************************************************************
 * Function:	void _DtCanvasMoveTraversal ()
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasMoveTraversal (
    _DtCvHandle		 canvas_handle,
    _DtCvTraversalCmd	 cmd,
    _DtCvValue		 wrap,
    _DtCvValue		 render,
    _DtCvPointer	 rid,
    _DtCvUnit		*ret_x,
    _DtCvUnit		*ret_y,
    _DtCvUnit		*ret_baseline,
    _DtCvUnit		*ret_height)
{
    int			 newIndex;
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;

    if (0 == canvas->trav_cnt)
	return _DtCvSTATUS_NONE;

    newIndex = canvas->cur_trav;
    if (-1 == newIndex)
	newIndex = 0;

    switch (cmd)
      {
	case _DtCvTRAVERSAL_TOP:
		newIndex = 0;
		break;

	case _DtCvTRAVERSAL_NEXT:
		newIndex++;
		if (newIndex >= canvas->trav_cnt)
		  {
		    newIndex--;
		    if (wrap == True)
			newIndex = 0;
		  }
		break;

	case _DtCvTRAVERSAL_PREV:
		newIndex--;
		if (newIndex < 0)
		  {
		    newIndex = 0;
		    if (wrap == True)
			newIndex = canvas->trav_cnt - 1;
		  }
		break;

	case _DtCvTRAVERSAL_BOTTOM:
		newIndex = canvas->trav_cnt - 1;
		break;

	case _DtCvTRAVERSAL_ID:
	case _DtCvTRAVERSAL_MARK:
		if (NULL != rid)
		  {
		    int		 idx;
		    char	*lnkId;
		    _DtCvValue	 found = False;

		    newIndex = 0;
		    while (False == found && newIndex < canvas->trav_cnt)
		      {
			if (_DtCvTRAVERSAL_ID == cmd &&
			    _DtCvTraversalLink ==
						canvas->trav_lst[newIndex].type)
			  {
			    idx  = canvas->trav_lst[newIndex].seg_ptr->link_idx;
			    lnkId = _DtLinkDbGetLinkSpec(canvas->link_data,
									idx);

			    if (_DtCvStrCaseCmpLatin1(lnkId, rid) == 0)
				found = True;
			  }
			else if (_DtCvTRAVERSAL_MARK == cmd &&
				 _DtCvTraversalMark == canvas->trav_lst[newIndex].type)
			  {
			    idx = canvas->trav_lst[newIndex].idx;
			    if (rid == canvas->marks[idx].client_data)
				found = True;
			   }

			if (False == found)
			    newIndex++;
		      }

		    if (False == found)
			return _DtCvSTATUS_BAD;
		  }
		break;
      }

    /*
     * turn off the old traversal
     */
    if (cmd == _DtCvTRAVERSAL_OFF)
      {
	if (-1 != canvas->cur_trav)
	    DrawTraversalIndicator (canvas, render, False,
					NULL, NULL, NULL, NULL);
	canvas->trav_on = _DtCvFALSE;
      }
    /*
     * turn off the old traversal and turn on the new one.
     */
    else if (newIndex != canvas->cur_trav)
      {
	if (-1 != canvas->cur_trav)
	    DrawTraversalIndicator (canvas, render, False,
					NULL, NULL, NULL, NULL);

        canvas->cur_trav = newIndex;
	DrawTraversalIndicator (canvas, render, True,
					ret_x, ret_y, ret_baseline, ret_height);
	canvas->trav_on = _DtCvTRUE;
	return _DtCvSTATUS_OK;
      }
    /*
     * Other wise turn on the traversal
     */
    else if (cmd == _DtCvTRAVERSAL_ON && -1 != canvas->cur_trav)
      {
	DrawTraversalIndicator (canvas, render, True,
                                       ret_x, ret_y, ret_baseline, ret_height);
	canvas->trav_on = _DtCvTRUE;
	return _DtCvSTATUS_OK;
      }

    return _DtCvSTATUS_NONE;
}

/*****************************************************************************
 * Function:	void _DtCanvasGetPosLink (_DtCvHandle canvas_handle,
 *						_DtCvUnit x, _DtCvUnit y);
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasGetPosLink (
    _DtCvHandle  canvas_handle,
    _DtCvUnit	 x1,
    _DtCvUnit	 y1,
    _DtCvUnit	 x2,
    _DtCvUnit	 y2,
    _DtCvLinkInfo *ret_info)
{
    int			 travIdx;
    int			 line;
    int			 len;
    int			 count;
    int			 startChar;
    _DtCvUnit		 topY;
    _DtCvUnit		 botY;
    _DtCvUnit		 startX;
    _DtCvUnit		 endX;
    void		*pChar;
    _DtCvValue		 junk;
    _DtCvStatus		 found = _DtCvSTATUS_NONE;
    _DtCvSegmentI	*pSeg  = NULL;
    _DtCanvasStruct	*canvas   = (_DtCanvasStruct *) canvas_handle;
    _DtCvTraversalInfo	*lnkSegs  = canvas->trav_lst;

    if (0 == canvas->trav_cnt)
	return _DtCvSTATUS_NONE;

    botY = 0;
    topY = 0;
    line = 0;
    while (line < canvas->txt_cnt && _DtCvSTATUS_NONE == found)
      {
	topY   = canvas->txt_lst[line].baseline - canvas->txt_lst[line].ascent;
	botY   = canvas->txt_lst[line].baseline + canvas->txt_lst[line].descent;
	startX = canvas->txt_lst[line].text_x;

	/*
	 * make sure the requested link is on this line.
	 */
	if (topY <= y1 && y1 <= botY && startX <= x1 &&
					x1 <= canvas->txt_lst[line].max_x)
	  {
	    int  lnkIndx = -1;
	    _DtCvValue lstVisible = False;

	    count     = canvas->txt_lst[line].length;
	    pSeg      = canvas->txt_lst[line].seg_ptr;
	    startChar = canvas->txt_lst[line].byte_index;
    
	    while (count > 0 && _DtCvSTATUS_NONE == found)
	      {
		/*
		 * adjust the starting position by the link space
		 */
		junk = _DtCvIsSegVisibleLink(pSeg);
		lstVisible = _DtCvModifyXpos (canvas->link_info, pSeg,
			    junk, lstVisible, lnkIndx, &startX);
		/*
		 * adjust the starting position by the traversal space
		 */
		junk = _DtCvIsSegALink(pSeg);
		(void) _DtCvModifyXpos (canvas->traversal_info, pSeg,
			    junk, ((_DtCvValue) True), lnkIndx, &startX);

		lnkIndx = pSeg->link_idx;

		/*
		 * skip no-op
		 */
		if (_DtCvIsSegNoop(pSeg))
		    len = 0;
		/*
		 * check region
		 */
		else if (_DtCvIsSegRegion(pSeg))
		  {
		    len  = 1;
		    endX = startX + _DtCvWidthOfRegionSeg(pSeg);
		  }
		else
		  {
		    /*
		     * initialize the pointer to the string
		     */
                    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg),
					_DtCvIsSegWideChar(pSeg), startChar);
    
		    /*
		     * get the length of the current string.
		     * If it is longer than the line count indicates,
		     * it must be wrapped to the next line. We are
		     * only interested in in the part of the line
		     * that is on the line selected.
		     */
		    len = _DtCvStrLen (pChar, _DtCvIsSegWideChar(pSeg));
		    if (len > count)
			len = count;
    
		    /*
		     * calculate the ending pixel postion for
		     * this string segment.
		     */
		    endX = startX + _DtCvGetStringWidth(canvas, pSeg,
								pChar, len);
		  }
    
		/*
		 * test to see if the selected segment was this segment.
		 */
		if (x1 >= startX && x1 < endX && x2 >= startX && x2 < endX)
		  {
		    found = _DtCvSTATUS_OK;
		    /*
		     * Find the hypertext entry.
		     */
		    travIdx = 0;
		    while (travIdx < canvas->trav_cnt
				&& _DtCvTraversalLink == lnkSegs->type
				&& lnkSegs->seg_ptr->link_idx != pSeg->link_idx)
		      {
			lnkSegs++;
			travIdx++;
		      }
		  }
		else
		  {
		    /*
		     * go to the next segment.
		     */
		    pSeg = pSeg->next_disp;
    
		    /*
		     * adjust for the new begining.
		     */
		    startX    = endX;
		    count     = count - len;
		    startChar = 0;
		  }
	      }
	  }
	line++;
      }

    /*
     * check to see if we found a segment and 
     * see if it is a hypertext segment
     */
    if (_DtCvSTATUS_OK == found)
      {
	found = _DtCvSTATUS_NONE;
	if (_DtCvIsSegALink(pSeg) &&
		_DtLinkDbGetLinkInfo(canvas->link_data, pSeg->link_idx,
				canvas->virt_functions.exec_cmd_filter,
				canvas->client_data, ret_info) == 0)
	  {
	    ret_info->offset_x = x1 - startX;
	    ret_info->offset_y = y1 - topY;
	    found = _DtCvSTATUS_OK;
          }
      }

    return found;

}  /* End _DtCanvasGetPosLink */

/*****************************************************************************
 * Function:	void _DtCanvasGetCurLink ()
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasGetCurLink (
    _DtCvHandle		 canvas_handle,
    _DtCvLinkInfo	*ret_info)
{
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;
    _DtCvSegmentI	*pSeg;
    int			 curTrav = canvas->cur_trav;

    /*
     * if there isn't any traversal entry or it is a mark, return nothing.
     */
    if (0 == canvas->trav_cnt || -1 == curTrav ||
			_DtCvTraversalMark == canvas->trav_lst[curTrav].type)
	return _DtCvSTATUS_NONE;

    /*
     * otherwise this is a hypertext link
     */
    if (NULL != ret_info &&
			_DtCvTraversalLink == canvas->trav_lst[curTrav].type)
      {
        pSeg = canvas->trav_lst[curTrav].seg_ptr;
	if (_DtLinkDbGetLinkInfo(canvas->link_data, pSeg->link_idx,
				canvas->virt_functions.exec_cmd_filter,
				canvas->client_data, ret_info) == 0)
	return _DtCvSTATUS_OK;
      }

    return _DtCvSTATUS_BAD;
}

/*****************************************************************************
 * Function:	void _DtCanvasGetCurTraversal ()
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasGetCurTraversal (
    _DtCvHandle		 canvas_handle,
    _DtCvLinkInfo	*ret_info,
    _DtCvPointer	*ret_data)
{
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;
    _DtCvSegmentI	*pSeg;
    int			 curTrav = canvas->cur_trav;

    /*
     * if there isn't any traversal entry, return nothing.
     */
    if (0 == canvas->trav_cnt || -1 == curTrav)
	return _DtCvSTATUS_NONE;

    /*
     * if this is a mark, return the client data.
     */
    if (NULL != ret_data &&
			_DtCvTraversalMark == canvas->trav_lst[curTrav].type)
      {
	*ret_data = canvas->marks[canvas->trav_lst[curTrav].idx].client_data;
	return _DtCvSTATUS_MARK;
      }

    /*
     * otherwise this is a hypertext link
     */
    if (NULL != ret_info &&
			_DtCvTraversalLink == canvas->trav_lst[curTrav].type)
      {
        pSeg = canvas->trav_lst[curTrav].seg_ptr;
	if (_DtLinkDbGetLinkInfo(canvas->link_data, pSeg->link_idx,
				canvas->virt_functions.exec_cmd_filter,
				canvas->client_data, ret_info) == 0)
	return _DtCvSTATUS_LINK;
      }

    return _DtCvSTATUS_BAD;
}


/*****************************************************************************
 * Function:	void _DtCanvasGetSpotInfo (_DtCvHandle canvas_handle,
 *						_DtCvUnit x, _DtCvUnit y);
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	_DtCvSTATUS_OK		if a segment was found at x, y.
 *		_DtCvSTATUS_NONE	if no segment found at that location.
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasGetSpotInfo (
    _DtCvHandle    canvas_handle,
    _DtCvUnit	   x,
    _DtCvUnit	   y,
    _DtCvSegment **ret_seg,
    _DtCvUnit	  *ret_offx,
    _DtCvUnit	  *ret_offy,
    _DtCvElemType *ret_elem)
{
    int			 line;
    int			 len;
    int			 count;
    int			 startChar;
    _DtCvUnit		 topY;
    _DtCvUnit		 botY;
    _DtCvUnit		 startX;
    _DtCvUnit		 endX;
    void		*pChar;
    _DtCvValue		 junk;
    _DtCvStatus		 found = _DtCvSTATUS_NONE;
    _DtCvSegmentI	*pSeg  = NULL;
    _DtCanvasStruct	*canvas   = (_DtCanvasStruct *) canvas_handle;

    if (NULL != ret_seg)
        *ret_seg = NULL;

    botY = 0;
    topY = 0;
    line = 0;
    while (line < canvas->txt_cnt && _DtCvSTATUS_NONE == found)
      {
	topY   = canvas->txt_lst[line].baseline - canvas->txt_lst[line].ascent;
	botY   = canvas->txt_lst[line].baseline + canvas->txt_lst[line].descent;
	startX = canvas->txt_lst[line].text_x;

	/*
	 * make sure the requested link is on this line.
	 */
	if (topY <= y && y <= botY && startX <= x &&
					x <= canvas->txt_lst[line].max_x)
	  {
	    int  lnkIndx = -1;
	    _DtCvValue lstVisible = False;

	    count     = canvas->txt_lst[line].length;
	    pSeg      = canvas->txt_lst[line].seg_ptr;
	    startChar = canvas->txt_lst[line].byte_index;
    
	    while (count > 0 && _DtCvSTATUS_NONE == found)
	      {
		/*
		 * adjust the starting position by the link space
		 */
		junk = _DtCvIsSegVisibleLink(pSeg);
		lstVisible = _DtCvModifyXpos (canvas->link_info, pSeg,
			    junk, lstVisible, lnkIndx, &startX);
		/*
		 * adjust the starting position by the traversal space
		 */
		junk = _DtCvIsSegALink(pSeg);
		(void) _DtCvModifyXpos (canvas->traversal_info, pSeg,
			    junk, ((_DtCvValue) True), lnkIndx, &startX);

		lnkIndx = pSeg->link_idx;

		/*
		 * skip no-op
		 */
		if (_DtCvIsSegNoop(pSeg))
		    len = 0;
		/*
		 * check region
		 */
		else if (_DtCvIsSegRegion(pSeg))
		  {
		    len  = 1;
		    endX = startX + _DtCvWidthOfRegionSeg(pSeg);
		  }
		else
		  {
		    /*
		     * initialize the pointer to the string
		     */
                    pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg),
					_DtCvIsSegWideChar(pSeg), startChar);
    
		    /*
		     * get the length of the current string.
		     * If it is longer than the line count indicates,
		     * it must be wrapped to the next line. We are
		     * only interested in in the part of the line
		     * that is on the line selected.
		     */
		    len = _DtCvStrLen (pChar, _DtCvIsSegWideChar(pSeg));
		    if (len > count)
			len = count;
    
		    /*
		     * calculate the ending pixel postion for
		     * this string segment.
		     */
		    endX = startX + _DtCvGetStringWidth(canvas, pSeg,
								pChar, len);
		  }
    
		/*
		 * test to see if the selected segment was this segment.
		 */
		if (x >= startX && x < endX)
		    found = _DtCvSTATUS_OK;
		else
		  {
		    /*
		     * go to the next segment.
		     */
		    pSeg = pSeg->next_disp;
    
		    /*
		     * adjust for the new begining.
		     */
		    startX    = endX;
		    count     = count - len;
		    startChar = 0;
		  }
	      }
	  }
	line++;
      }

    /*
     * check to see if we found a segment.
     */
    if (_DtCvSTATUS_OK == found)
      {
	*ret_elem = _DtCvREGION_TYPE;
	if (_DtCvIsSegString(pSeg))
	    *ret_elem = _DtCvSTRING_TYPE;

	if (NULL != ret_seg)
	    *ret_seg  = pSeg;
	if (NULL != ret_offx)
	    *ret_offx = x - startX;
	if (NULL != ret_offy)
	    *ret_offy = y - topY;
      }

    return found;

}  /* End _DtCanvasGetPosition */
