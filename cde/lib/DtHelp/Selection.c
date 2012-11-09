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
/* $XConsortium: Selection.c /main/22 1996/11/12 11:44:48 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     Selection.c
 **
 **   Project:  Cde DtHelp
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
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
#include "LayoutUtilI.h"
#include "SelectionI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
static	void	AdjustSelection (
			_DtCanvasStruct	*canvas,
			_DtCvSelectData	 next);
static	int	GetSelectedText(
			_DtCanvasStruct	*canvas,
			_DtCvSelectData	 start,
			_DtCvSelectData	 end,
			unsigned int         mask,
			_DtCvPointer	*ret_data);
/********    End Private Function Declarations    ********/

/********    Private Defines                 ********/
#define	GROW_SIZE	5
/********    End Private Defines             ********/

/********    Macros                          ********/
#define	Equal(a,b)		(a.y == b.y && a.x == b.x)
#define	LessThan(a,b)		((a.y < b.y) || (a.y == b.y && a.x < b.x))
#define	GreaterThan(a,b)	((a.y > b.y) || (a.y == b.y && a.x > b.x))
#define	LessThanEq(a,b)		(Equal(a,b) || LessThan(a,b))
#define	GreaterThanEq(a,b)	(Equal(a,b) || GreaterThan(a,b))

#define	InRegion(top,bot,min,max)	((min) <= (bot) && (top) <= (max))

/********    End Macros                      ********/

/********    Private Variable Declarations    ********/
static	const _DtCvSelectData	defaultSelect = { -1, -1, -1, -1};

/********    End Private Variable Declarations    ********/

/******************************************************************************
 *                             Private Functions
 ******************************************************************************/
/******************************************************************************
 * Function: StartXOfLine
 *
 * Purpose: Determine the start of a line, takes into consideration
 *          the traversal and link before values. The 'x' returned is
 *          exactly where the text/graphic is to be placed on the canvas.
 *****************************************************************************/
static _DtCvUnit
StartXOfLine(
    _DtCanvasStruct	*canvas,
    _DtCvDspLine	 line)
{
    _DtCvValue	 lastLinkVisible = FALSE;
    int		 lnkInd = -1;
    _DtCvUnit	 xPos;
    _DtCvSegmentI   *pSeg;

    xPos = _DtCvGetStartXOfLine(&line, &pSeg);
    return (_DtCvAdvanceXOfLine( canvas, pSeg, xPos,
			&lnkInd, &lastLinkVisible));

} /* End StartXOfLine */

/*****************************************************************************
 * Function: SearchForClosestLine
 *
 * Purpose:  Initializes the 'new' structure with information indicating
 *	     what line is closest to the target_y.
 *		next->y         Set to target_y if no line straddles it.
 *				Otherwise, it will be set to the minimum
 *				y of all lines straddling the target_y.
 *		next->x         Set to target_x if no line straddles
 *				target_y or if target_x is before the
 *				first line straddling target_y.
 *		next->line_idx  Set to -1 if no line straddles target_y.
 *				Otherwise, set to the first line that
 *				straddles target_x or is the minimum x
 *				that is greater than target_x of all the
 *				lines straddling target_x.
 *		next->char_idx  Set to -1 if no straddles target_y.
 *				Otherwise, set to the character that
 *				resides at target_x if target_x is in
 *				the middle of the line.  Set to zero if
 *				target_x is before the line, and set to
 *				the line count if target_x is after the
 *				line.
 *
 *****************************************************************************/
static	void
SearchForClosestLine (
    _DtCanvasStruct	*canvas,
    _DtCvUnit		 target_x,
    _DtCvUnit		 target_y,
    _DtCvSelectData *next)
{
    int		 i;
    int		 maxI;
    _DtCvUnit	 lineY;
    _DtCvUnit	 endX;
    _DtCvUnit	 begX;
    _DtCvUnit	 maxX = -1;
    _DtCvDspLine *lines = canvas->txt_lst;

    *next = defaultSelect;
    for (i = 0; i < canvas->txt_cnt; i++)
      {
	/*
	 * get the maximum y of the line
	 * if it straddles the target y, process it.
	 */
	lineY = lines[i].baseline + lines[i].descent;
	if (_DtCvStraddlesPt(target_y,lines[i].baseline-lines[i].ascent,lineY))
	  {
	    /*
	     * Is this the minimum y of all the maximum y values of the
	     * line straddling the target y?
	     */
	    if (next->y  == -1 || next->y > lineY)
	        next->y = lineY;

	    /*
	     * Get the maximum X position of the line.
	     * If this is the maximum X of all the lines straddling
	     * the target y, remember it.
	     */
	    endX = canvas->txt_lst[i].max_x;
	    if (maxX < endX && endX < target_x)
	      {
		maxX = endX;
		maxI = i;
	      }

	    /*
	     * Does this line straddle the x?
	     */
	    begX = StartXOfLine(canvas, lines[i]);
	    if (_DtCvStraddlesPt(target_x, begX, endX))
	      {
		next->line_idx = i;
		next->char_idx = _DtCvGetCharIdx(canvas,lines[i],target_x);
	      }
	  }
      }

    /*
     * remember what the target x was for this line.  If the target x is
     * less than the start of the line, then the selection process will
     * highlight the space before the line.  If its in the middle, it
     * will just highlight starting at the character.  If it's after the
     * end, the rest will be cut off at the end of the line.
     */
    next->x = target_x;

    /*
     * If we found a line straddling the target y, but it does not
     * straddle the target_x, check max x for the correct info.
     */
    if (next->line_idx == -1 && maxX > -1)
      {
	next->line_idx = maxI;
	next->char_idx = lines[maxI].length;
      }

    /*
     * didn't find a line straddling the target_y, set y.
     */
    if (next->y == -1)
        next->y = target_y;
}

/*****************************************************************************
 * Function: MarkLinesOutsideBoundary
 *
 *****************************************************************************/
static	void
MarkLinesOutsideBoundary (
    _DtCanvasStruct	*canvas,
    _DtCvUnit		 top_y,
    _DtCvUnit		 top_x,
    _DtCvUnit		 bot_y,
    _DtCvUnit		 bot_x)
{
    int       i;
    _DtCvUnit      maxY;
    _DtCvUnit      minY;
    _DtCvDspLine  *lines = canvas->txt_lst;

    for (i = 0; i < canvas->txt_cnt; i++)
      {
        maxY = lines[i].baseline + lines[i].descent;
	minY = lines[i].baseline - lines[i].ascent;

	/*
	 * is this line outside the boundary?
	 * If so, mark it so it's not processed.
	 */
        if (maxY < top_y || minY > bot_y )
	    _DtCvSetProcessed(lines[i]);

	else
	  {
	    /*
	     * does it straddle the top?
	     */
	    if (_DtCvStraddlesPt(top_y, minY, maxY))
	      {
	        /*
	         * Does it begin before the selection?
	         * If so, mark it so it's not processed.
	         */
	        if (canvas->txt_lst[i].max_x <= top_x)
		    _DtCvSetProcessed(lines[i]);
	      }
	
	    /*
	     * does it straddle the bottom?
	     */
	    if (_DtCvStraddlesPt(bot_y, minY, maxY))
	      {
	        /*
	         * Does it start after the selection?
	         * If so, mark it so it's not processed.
	         */
	        if (StartXOfLine(canvas, lines[i]) >= bot_x)
		    _DtCvSetProcessed(lines[i]);
	      }
          }
      }
}

/*****************************************************************************
 * Function: AdjustSelection
 *
 *****************************************************************************/
static	void
AdjustSelection (
    _DtCanvasStruct	*canvas,
    _DtCvSelectData next)
{
    _DtCvSelectData  start = canvas->select_start;
    _DtCvSelectData  end   = canvas->select_end;

    if (!(Equal(next, end)))
      {
	if (next.line_idx != -1 && next.line_idx == canvas->select_end.line_idx
		&&
	    next.char_idx != -1 && next.char_idx == canvas->select_end.char_idx)
	    return;

	if (GreaterThan(next, end))
	  {
	    if (LessThanEq(start, end))
		_DtCvDrawAreaWithFlags (canvas, end, next,
						0, _DtCvSELECTED_FLAG,
						_DtCvBAD_TYPE, NULL);

	    else if (GreaterThanEq(start, next))
		_DtCvDrawAreaWithFlags (canvas, end, next,
						_DtCvSELECTED_FLAG, 0,
						_DtCvBAD_TYPE, NULL);

	    else /* end < start < next */
	      {
		_DtCvDrawAreaWithFlags (canvas, end  , start,
						_DtCvSELECTED_FLAG, 0,
						_DtCvBAD_TYPE, NULL);
		_DtCvDrawAreaWithFlags (canvas, start, next ,
						0, _DtCvSELECTED_FLAG,
						_DtCvBAD_TYPE, NULL);
	      }
	  }
	else /* if (next < end) */
	  {
	    if (LessThanEq(start, next))
		_DtCvDrawAreaWithFlags (canvas, next, end,
						_DtCvSELECTED_FLAG, 0,
						_DtCvBAD_TYPE, NULL);

	    else if (GreaterThanEq(start, end))
		_DtCvDrawAreaWithFlags (canvas, next, end,
						0, _DtCvSELECTED_FLAG,
						_DtCvBAD_TYPE, NULL);

	    else /* next < start < end */
	      {
		_DtCvDrawAreaWithFlags (canvas, start, end  ,
						_DtCvSELECTED_FLAG, 0,
						_DtCvBAD_TYPE, NULL);
		_DtCvDrawAreaWithFlags (canvas, next , start,
						0, _DtCvSELECTED_FLAG,
						_DtCvBAD_TYPE, NULL);
	      }
	  }
      }

    canvas->select_end = next;
}

/*****************************************************************************
 * Function: SkipOtherLines
 *
 *****************************************************************************/
static void
SkipOtherLines(
    _DtCvDspLine *lines,
    int		 max_cnt,
    int		 idx,
    _DtCvUnit	 target_y,
    int		*ret_idx)
{
    while (idx < max_cnt && _DtCvIsNotProcessed(lines[idx]) &&
			lines[idx].baseline - lines[idx].ascent > target_y)
        idx++;

    *ret_idx = idx;
}

/*****************************************************************************
 * Function: CheckAndSwitchPoints
 *
 *****************************************************************************/
static int
CheckAndSwitchPoints(
    _DtCvSelectData *pt1,
    _DtCvSelectData *pt2)
{
    _DtCvSelectData temp;

    if (pt1->y > pt2->y || (pt1->y == pt2->y && pt1->x >  pt2->x))
      {
	temp = *pt2;
	*pt2 = *pt1;
	*pt1 = temp;
      }
}

/*****************************************************************************
 * Function: AddSegmentToData
 *
 *****************************************************************************/
static _DtCvUnit
AddSegmentToData(
    _DtCanvasStruct	*canvas,
    unsigned int	 mask,
    _DtCvUnit		 start_x,
    int			 line_idx,
    int			 char_idx,
    int			 copy_cnt,
    _DtCvFlags		 end_flag,
    _DtCvUnit		*ret_y,
    _DtCvPointer	*ret_data)
{
    _DtCvDspLine line   = canvas->txt_lst[line_idx];
    int		 result = _DtCvSTATUS_OK;
    int		 count  = line.length;
    int		 start  = line.byte_index;
    int		 lnkInd = -1;
    int		 cnt;
    int		 len;
    _DtCvUnit	 segWidth;
    _DtCvUnit	 xPos  = line.text_x;
    void 	*pChar;
    _DtCvSegmentI	*pSeg  = line.seg_ptr;
    _DtCvFlags	 flag  = 0;
    _DtCvValue	 done  = False;
    _DtCvValue	 lastLinkVisible = FALSE;
    _DtCvStringInfo strInfo;

    xPos = _DtCvGetStartXOfLine(&line, &pSeg);

    while (done == False && char_idx)
      {
	/*
	 * advance past the link and traversal info
	 */
	xPos = _DtCvAdvanceXOfLine(canvas, pSeg, xPos,
				&lnkInd, &lastLinkVisible);

        /*
         * advance the pointer by the width
         */
        _DtCvGetWidthOfSegment(canvas, pSeg, start, count,
							&cnt, &segWidth, NULL);
	if (cnt < char_idx)
	  {
	    xPos     += segWidth;
	    pSeg      = pSeg->next_disp;
	    count    -= cnt;
	    char_idx -= cnt;
	    start     = 0;
	  }
        else 
          {
            _DtCvGetWidthOfSegment(canvas, pSeg, start, char_idx,
							&cnt, &segWidth, NULL);
	    xPos  += segWidth;
	    start += cnt;
	    count -= cnt;
            done = True;
          }
      }

    if (start_x > xPos)
	start_x = xPos;

    while (_DtCvSTATUS_OK == result && pSeg != NULL && copy_cnt > 0)
      {
	/*
	 * advance past the link and traversal info
	 */
	xPos = _DtCvAdvanceXOfLine(canvas, pSeg, xPos,
				&lnkInd, &lastLinkVisible);

        switch (_DtCvPrimaryTypeOfSeg(pSeg))
          {
            case _DtCvSTRING:

                pChar = _DtCvStrPtr(_DtCvStringOfStringSeg(pSeg),
					_DtCvIsSegWideChar(pSeg), start);
                len   = _DtCvStrLen (pChar, _DtCvIsSegWideChar(pSeg));

                if (len > copy_cnt)
                    len = copy_cnt;

                segWidth = _DtCvGetStringWidth(canvas, pSeg, pChar, len);
		if (copy_cnt == len)
		    flag = end_flag;

		strInfo.string   = pChar;
		strInfo.byte_len = len;
		strInfo.wc       = _DtCvIsSegWideChar(pSeg);
		strInfo.font_ptr = _DtCvFontOfStringSeg(pSeg);

		if (canvas->virt_functions.build_selection != NULL)
		    result = (*(canvas->virt_functions.build_selection))(
				canvas->client_data,
				_DtCvSTRING_TYPE,
				mask,
				ret_data,
				xPos - start_x,
				segWidth,
				flag,
				(_DtCvPointer) &strInfo);

		if (_DtCvSTATUS_OK == result)
		  {
		    if (line.baseline + line.descent > *ret_y)
		    *ret_y  = line.baseline + line.descent;
                    start_x = xPos + segWidth;
		  }
		else if (_DtCvSTATUS_NONE == result)
		    result = _DtCvSTATUS_OK;

                xPos     += segWidth;
                copy_cnt -= len;
                start     = 0;
                break;

            case _DtCvREGION:
		if (copy_cnt == 1)
		    flag = end_flag;

		if (canvas->virt_functions.build_selection != NULL)
		    result = (*(canvas->virt_functions.build_selection))(
				canvas->client_data,
				_DtCvREGION_TYPE,
				mask,
				ret_data,
				xPos - start_x,
				_DtCvWidthOfRegionSeg(pSeg),
				flag,
				_DtCvInfoOfRegionSeg(pSeg));

		if (_DtCvSTATUS_OK == result)
		  {
		    if (line.baseline + line.descent > *ret_y)
		    *ret_y = line.baseline + line.descent;
                    start_x = xPos + segWidth;
		  }
		else if (_DtCvSTATUS_NONE == result)
		    result = _DtCvSTATUS_OK;

                copy_cnt--;
                xPos    += _DtCvWidthOfRegionSeg(pSeg);
                break;
          }
        pSeg = pSeg->next_disp;
      }

    if (result != _DtCvSTATUS_OK)
	return -1;

    return start_x;
}

/*****************************************************************************
 * Function: BuildLine
 *
 *****************************************************************************/
static int
BuildLine(
    _DtCanvasStruct	*canvas,
    unsigned int	 mask,
    _DtCvUnit		 prev_y,
    _DtCvUnit		 target_x,
    int			 line_idx,
    int			 char_idx,
    int			 copy_cnt,
    _DtCvFlags		 end_flag,
    _DtCvUnit		*ret_x,
    _DtCvUnit		*ret_y,
    _DtCvPointer	*ret_data)
{
    _DtCvUnit		 topY;
    _DtCvDspLine	*lines = canvas->txt_lst;
    _DtCvStringInfo	 strInfo = { NULL, 0, 1, NULL };

    topY = lines[line_idx].baseline - lines[line_idx].ascent;
    if (topY > prev_y && canvas->virt_functions.build_selection != NULL)
      {
	int   newLines;
	_DtCvUnit  lineSize = canvas->metrics.line_height / 2;
	if (lineSize < 1)
	    lineSize = 1;

	newLines = (topY - prev_y) / lineSize;
	while (newLines > 0)
	  {
	    newLines--;
	    if ((*(canvas->virt_functions.build_selection))(
				canvas->client_data, _DtCvSTRING_TYPE,
				mask, ret_data, 0, 0, _DtCvEND_OF_LINE,
				(_DtCvPointer) &strInfo) != _DtCvSTATUS_OK)
		return -1;
	  }
      }

    *ret_y = 0;
    *ret_x = AddSegmentToData (canvas, mask, target_x, line_idx, char_idx,
					copy_cnt, end_flag, ret_y, ret_data);
    _DtCvSetProcessed(lines[line_idx]);

    if (*ret_x == -1)
	return -1;
    return 0;
}

/*****************************************************************************
 * Function: FindMinX
 *
 *****************************************************************************/
static	int
FindMinX (
    _DtCvDspLine *lines,
    int		 txt_cnt,
    _DtCvUnit	 target_y,
    int		*ret_line)
{
    int	  i;
    int   cnt    = 0;
    _DtCvUnit  curMin = -1;
    _DtCvUnit  curX;

    for (i = 0; i < txt_cnt; i++)
      {
	if (_DtCvIsNotProcessed(lines[i]))
	  {
	    if (lines[i].baseline - lines[i].ascent < target_y &&
			target_y <= lines[i].baseline + lines[i].descent)
	      {
	        cnt++;
	        curX = lines[i].text_x;

	        if (curMin == -1 || curMin > curX)
		  {
	            curMin    = curX;
		    *ret_line = i;
		  }
              }
          }
      }
    return cnt;
}

/*****************************************************************************
 * Function: FindNextMinY
 *
 *****************************************************************************/
static _DtCvValue
FindNextMinY(
    _DtCvDspLine *lines,
    int		 max_cnt,
    _DtCvUnit	 target_y,
    _DtCvUnit	*ret_y)
{
    int		i     = 0;
    _DtCvUnit	maxY;
    _DtCvValue	found = False;

    while (i < max_cnt)
     {
	if (_DtCvIsNotProcessed(lines[i]))
	  {
	    maxY = lines[i].baseline + lines[i].descent;
	    if (target_y == -1 || maxY < target_y)
	      {
		found    = True;
		target_y = maxY;
	      }
	    SkipOtherLines (lines, max_cnt, i+1, target_y, &i);
	  }
	else
	    i++;
     }

    *ret_y = target_y;
    return found;
}

/*****************************************************************************
 * Function: GetSelectedText
 *
 *****************************************************************************/
static int
GetSelectedText(
    _DtCanvasStruct	*canvas,
    _DtCvSelectData next,
    _DtCvSelectData end,
    unsigned int         mask,
    _DtCvPointer	*ret_data)
{
    _DtCvUnit   maxY;
    _DtCvUnit   botY;
    int    i;
    int    lineCnt  = 0;
    int    junk;
    int    result   = 0;
    int    cpyCnt   = 0;
    int    txtCnt   = canvas->txt_cnt;
    _DtCvFlags endFlag;
    _DtCvValue  processing = True;
    _DtCvDspLine   *lines = canvas->txt_lst;

    for (i = 0; i < txtCnt; i++)
	_DtCvClearProcessed(lines[i]);

    MarkLinesOutsideBoundary(canvas, next.y, next.x, end.y, end.x);

    maxY = next.y;
    if (next.line_idx == -1)
      {
	/*
	 * find the first selected line
	 */
	if (FindNextMinY(lines, txtCnt, -1, &next.y) == False)
	    return 0;

	next.x  = 0;
	lineCnt = FindMinX(lines, txtCnt, next.y, &next.line_idx);
	next.char_idx = 0;
      }
    else
	lineCnt = FindMinX(lines, txtCnt, next.y, &junk);

    while (processing == True && result == 0)
      {
	/*
	 * process the next line of text.
	 */
	do
	  {
	    endFlag = 0;
	    cpyCnt  = lines[next.line_idx].length - next.char_idx;
	    if (next.line_idx == end.line_idx)
		cpyCnt = cpyCnt - lines[next.line_idx].length + end.char_idx;
	    else if (lineCnt == 1)
		endFlag = _DtCvEND_OF_LINE;

	    result  = BuildLine(canvas, mask, maxY, next.x,
					next.line_idx, next.char_idx,
					cpyCnt, endFlag,
					&next.x, &botY, ret_data);

	    if (botY > maxY)
		maxY = botY;

	    next.char_idx = 0;
	    lineCnt       = FindMinX(lines, txtCnt, next.y, &next.line_idx);

	  } while (result == 0 && lineCnt > 0);

	if (result == 0)
	  {
	    next.x = 0;
	    processing = FindNextMinY(lines, txtCnt, -1, &next.y);
	    if (processing == True)
		lineCnt = FindMinX(lines, txtCnt, next.y, &next.line_idx);
	  }
      }

    return result;

} /* End GetSelectedText */

/*****************************************************************************
 * Function:	GetSegsInArea()
 *
 * Purpose:	Retrieve the segments making up the selection.
 *
 *****************************************************************************/
static _DtCvStatus
GetSegsInArea (
    _DtCanvasStruct	*canvas,
    _DtCvSelectData	  *beg,
    _DtCvSelectData	  *end,
    _DtCvSegPts		***ret_segs,
    _DtCvUnit		  *ret_y1,
    _DtCvUnit		  *ret_y2)
{
    int			 cnt;
    int			 count;
    int			 start;
    int			 length;
    int			 lineCnt;
    int			 result = 0;
    _DtCvValue		 processing = True;
    _DtCvUnit		 minY;
    _DtCvUnit		 maxY;
    _DtCvUnit		 botY;
    _DtCvSelectData	 next;
    _DtCvSegPts		*newPt;
    _DtCvSegmentI	*pSeg;
    _DtCvDspLine	*lines = canvas->txt_lst;

    *ret_segs = NULL;

    if (beg->x == -1)
	return _DtCvSTATUS_NONE;

    /*
     * make sure the selection points are in the correct order.
     */
    CheckAndSwitchPoints(beg, end);

    /*
     * clear the processed bit
     */
    for (cnt = 0; cnt < canvas->txt_cnt; cnt++)
	_DtCvClearProcessed(lines[cnt]);

    /*
     * initialize the working structure
     * mark all the lines outside the selection regiion as invalid
     */
    next = *beg;
    MarkLinesOutsideBoundary(canvas, next.y, next.x, end->y, end->x);

    /*
     * start the minimum and maximum Y at this location.
     */
    minY = next.y;
    maxY = end->y;

    /*
     * is there a line at this location?
     */
    if (next.line_idx == -1)
      {
	/*
	 * find the first selected line within the region.
	 */
	if (FindNextMinY(lines, canvas->txt_cnt, -1, &next.y) == False)
	    processing = False;		/* empty of any text */
	else
	  {
	    /*
	     * now find the first line that is on this 'line' and
	     * the number of lines.
	     */
	    next.x  = 0;
	    lineCnt = FindMinX(lines, canvas->txt_cnt, next.y, &next.line_idx);
	    next.char_idx = 0;
          }
      }
    else /* find the number of lines on this 'line' */
	lineCnt = FindMinX(lines, canvas->txt_cnt, next.y, &cnt);

    /*
     * loop will there are segments to process
     */
    while (processing == True && result == 0)
      {
	/*
	 * process the next line of text.
	 */
	while (result == 0 && lineCnt > 0)
	  {
	    /*
	     * for each segment in this line (that is selected)
	     * create a segment point for it.
	     */
	    length = lines[next.line_idx].length;
	    start  = lines[next.line_idx].byte_index;

	    /*
	     * if this is the last line, shorten the length
	     * by the ending index.
	     */
	    if (next.line_idx == end->line_idx)
		length = end->char_idx;

	    /*
	     * move through the line's segments until we
	     * hit the segment starting the selection
	     */
	    pSeg = lines[next.line_idx].seg_ptr;
	    count = next.char_idx;
	    while (NULL != pSeg && 0 < count)
	      {
		/*
		 * get the byte count of this segment
		 */
		_DtCvGetWidthOfSegment(canvas, pSeg, start, length,
					&cnt, NULL, NULL);

		/*
		 * is the byte count of this segment larger than
		 * the starting index of the selection? If not,
		 * the selection is after this segment.
		 */
		if (count >= cnt)
		  {
		    start          = 0;
		    length        -= cnt;
		    pSeg           = pSeg->next_disp;
		  }
		else
		  {
		    length        -= count;
		    start          = start + count;
		  }
		count -= cnt;
	      }

	    while (0 == result && NULL != pSeg && 0 < length)
	      {
	        /*
		 * start with error condition. If the malloc works
	         * the error result gets reset to valid.
		 */
	        result = -1;
	        newPt = (_DtCvSegPts *) malloc (sizeof(_DtCvSegPts));
	        if (NULL != newPt)
		  {
		    /*
		     * indicate everything is okay.
		     */
		    result = 0;

		    /*
		     * get the width of this segment
		     */
		    _DtCvGetWidthOfSegment(canvas, pSeg, start, length,
							&cnt, NULL, NULL);

		    /*
		     * now set the segment point information and add it to the
		     * array of segment points.
		     */
	            newPt->offset  = start;
	            newPt->len     = cnt;
		    newPt->segment = pSeg;

		    *ret_segs = (_DtCvSegPts **) _DtCvAddPtrToArray(
							(void **) *ret_segs,
							(void  *)  newPt);
		    if (NULL == *ret_segs)
		        result = -1;

		    pSeg    = pSeg->next_disp;
		    length -= cnt;
		    start   = 0;
		  }
	      }

	    /*
	     * does this line extend below the selection y?
	     * if so, report it as the maximum y.
	     */
	    botY = lines[next.line_idx].baseline + lines[next.line_idx].descent;
	    if (botY > maxY)
		maxY = botY;

	    /*
	     * indicate this line has been processed.
	     */
	    _DtCvSetProcessed(lines[next.line_idx]);

	    /*
	     * get the next line
	     */
	    next.char_idx = 0;
	    lineCnt       = FindMinX(lines, canvas->txt_cnt, next.y,
							&next.line_idx);
	  }

	if (result == 0)
	  {
	    next.x = 0;
	    processing = FindNextMinY(lines,canvas->txt_cnt, -1, &next.y);
	    if (processing == True)
		lineCnt = FindMinX(lines,canvas->txt_cnt,next.y,&next.line_idx);
	  }
      }

    /*
     * if no errors, add a null to the array
     */
    if (0 != result)
      {
        *ret_segs = (_DtCvSegPts **) _DtCvAddPtrToArray((void **) *ret_segs,
							(void  *) NULL);
	if (NULL == *ret_segs)
	    result = -1; 
      }

    /*
     * if errors, free the segment points and return a bad status.
     */
    if (0 != result)
      {
	if (NULL != *ret_segs)
	  {
	    for (lineCnt = 0; NULL != (*ret_segs)[lineCnt]; lineCnt++)
		free((*ret_segs)[lineCnt]);
	    free(*ret_segs);
	  }
	return _DtCvSTATUS_BAD;
      }

    if (NULL != ret_y1)
	*ret_y1 = minY;
    
    if (NULL != ret_y2)
	*ret_y2 = minY;

    return _DtCvSTATUS_OK;
}

/******************************************************************************
 *                             Semi-Public Functions
 ******************************************************************************/
/*****************************************************************************
 * Function: _DtCvDrawAreaWithFlags
 *
 *****************************************************************************/
void
_DtCvDrawAreaWithFlags (
    _DtCanvasStruct	*canvas,
    _DtCvSelectData	 start,
    _DtCvSelectData	 end,
    _DtCvFlags		 old_flags,
    _DtCvFlags		 new_flags,
    _DtCvElemType	 trav_type,
    _DtCvPointer	 trav_data)
{
    int    i;
    int    len;
    int    count;
    int    startChar;
    int    lnkInd;
    _DtCvUnit   dstX;
    _DtCvUnit   topY;
    _DtCvUnit   botY;
    _DtCvUnit   superWidth;
    _DtCvUnit   subWidth;
    _DtCvUnit   superY;
    _DtCvUnit   subY;
    _DtCvUnit   scriptX;
    _DtCvUnit   segWidth;
    _DtCvSegmentI  *pSeg;
    _DtCvValue   lstLinkVis;
    _DtCvValue   lstWasSuper;
    _DtCvValue   lstWasSub;
    _DtCvValue   trimmed;
    _DtCvFlags	 flagMask = old_flags | new_flags;
    _DtCvFlags	 endFlag  = flagMask & _DtCvTRAVERSAL_END;

    _DtCvDspLine   *lines = canvas->txt_lst;

    /*
     * now use the flagMask to determine what else to look for.
     * I.e. if flagMask has _DtCvMARK_FLAG set, then it becomes
     * set to _DtCvSELECTED_FLAG and visa versa.
     */
    flagMask ^= (_DtCvSELECTED_FLAG | _DtCvMARK_FLAG);

    /*
     * strip the end flag from the other flags
     */
    new_flags &= ~(_DtCvTRAVERSAL_END);
    old_flags &= ~(_DtCvTRAVERSAL_END);

    if (Equal(start, end))
	return;

    for (i = 0; i < canvas->txt_cnt; i++)
      {
	topY  = lines[i].baseline - lines[i].ascent;
	botY  = lines[i].baseline + lines[i].descent;

	if (InRegion(topY, botY, start.y, end.y))
	  {
	    /*
	     * get the start of the text.
	     */
	    lstLinkVis  = False;
	    lstWasSuper = False;
	    lstWasSub   = False;
	    lnkInd      = -1;
	    dstX        = _DtCvGetStartXOfLine(&(lines[i]), &pSeg);
	    startChar   = lines[i].byte_index;
	    count       = lines[i].length;

	    while (pSeg != NULL && _DtCvIsSegNoop(pSeg))
	      {
	        startChar  = 0;
	        pSeg       = pSeg->next_disp;
	      }

	    /*
	     * advance the starting point
	     */
	    dstX = _DtCvAdvanceXOfLine(canvas, pSeg, dstX,
					&lnkInd, &lstLinkVis);
	    /*
	     * take into account super/sub scripting
	     */
	    dstX = _DtCvAdjustForSuperSub(canvas, pSeg, dstX, &scriptX,
					&superWidth, &superY, &subWidth, &subY,
					&lstWasSuper, &lstWasSub);

	    /*
	     * set this flag so that the first pass of 'while (cnt > 0)'
	     * doesn't do it again.
	     */
	    trimmed = True;

	    if (_DtCvStraddlesPt(start.y, topY, botY))
	      {
		/*
		 * skip this item?
		 * I.E. is this line before the start or after the end?
		 */
		if (canvas->txt_lst[i].max_x < start.x ||
					end.y == start.y && end.x <= dstX )
		    continue;

		/*
		 * does this line start the mark/selection?
		 */
		if (i == start.line_idx && start.x >= dstX)
		  {
		    int cnt = start.char_idx;

		    while (cnt > 0)
		      {
			if (trimmed == False)
			  {
			    /*
			     * advance the starting point
			     */
			    dstX = _DtCvAdvanceXOfLine(canvas, pSeg, dstX,
					&lnkInd, &lstLinkVis);
			    /*
			     * take into account super/sub scripting
			     */
			    dstX = _DtCvAdjustForSuperSub(canvas,
					pSeg, dstX, &scriptX,
					&superWidth, &superY, &subWidth, &subY,
					&lstWasSuper, &lstWasSub);
			  }

			/*
			 * take into account the length of the segment
			 */
			_DtCvGetWidthOfSegment(canvas, pSeg,
					startChar , cnt,
					&len, &segWidth, &trimmed);

			dstX       += segWidth;
			startChar  += len;
			if (trimmed == False)
			  {
			    startChar  = 0;
			    pSeg       = pSeg->next_disp;
			  }

			trimmed = False;

			cnt -= len;

		      }

		    count -= start.char_idx;
		  }

		/*
		 * otherwise this line is after the line that starts
		 * the mark/selection. Stick with its start x.
		 */
	      }

	    /*
	     * does this straddle the end point?
	     */
	    if (_DtCvStraddlesPt(end.y, topY, botY))
	      {
		/*
		 * does this start after the end of the mark/selection?
		 * if so, skip.
		 */
		if (end.x <= dstX)
		    continue;

		/*
		 * Does this segment end after the end of the mark/selection?
		 * If so, trim how much gets highlighted.
		 */
		if (canvas->txt_lst[i].max_x > end.x)
		    count -= (lines[i].length - end.char_idx);
	      }

	    /*
	     * while there is something to draw (un)mark/selected.
	     */
	    old_flags = old_flags | _DtCvMARK_BEGIN;
	    new_flags = new_flags | _DtCvMARK_BEGIN;
	    while (count > 0)
	      {
		/*
		 * the original count to render
		 */
		len = count;

		/*
		 * check for other marks and selection.
		 */
		_DtCvCheckLineMarks(canvas, i, startChar - lines[i].byte_index,
						count, dstX, flagMask,
						&len, &old_flags, &new_flags);

		/*
		 * if this is the last segment(s) of the (un)mark/selection
		 * set the end flags.
		 */
		if (len == count)
		  {
		    new_flags |= (endFlag | _DtCvLINK_END | _DtCvMARK_END);
		    old_flags |= (endFlag | _DtCvLINK_END | _DtCvMARK_END);
		  }

		/*
		 * draw the segments that are marked/unmarked.
		 */
	        dstX = _DtCvDrawSegments(canvas, lines[i],
					pSeg, startChar , len, &lnkInd,
					dstX, dstX, &scriptX,
					&superWidth, &superY, &subWidth, &subY,
					&lstWasSub, &lstWasSuper,
					&lstLinkVis, old_flags, new_flags,
					trav_type, trav_data);
		/*
		 * modify the count by the length processed
		 */
		count  -= len;

		/*
		 * did this do the entire length? If not, set the
		 * indexes ahead and do again.
		 */
		if (count > 0)
		    _DtCvSkipLineChars(canvas, pSeg, startChar , count + len,
						len, &startChar , &pSeg);

		/*
		 * strip the any begin flags.
		 */
		_DtCvRemoveBeginFlags(old_flags);
		_DtCvRemoveBeginFlags(new_flags);
	      }
	  }
      }
}

/*****************************************************************************
 * Function:	_DtCanvasGetSelectionPoints()
 *
 * Purpose:	Retrieve the segments making up the selection.
 *
 *****************************************************************************/
_DtCvStatus
_DtCvGetMarkSegs (
    _DtCanvasStruct	*canvas,
    _DtCvPointInfo	***ret_info)
{
    int			  i;

    *ret_info = NULL;

    for (i = 0; i < canvas->mark_cnt; i++)
      {
	_DtCvPointInfo	*nxtInfo;

	/*
	 * allocate mark information structure
	 */
	nxtInfo = (_DtCvPointInfo *) malloc (sizeof(_DtCvPointInfo));
	if (NULL == nxtInfo)
	    return _DtCvSTATUS_BAD;

	nxtInfo->client_data = canvas->marks[i].client_data;

	if (_DtCvSTATUS_BAD == GetSegsInArea(canvas, &(canvas->marks[i].beg),
						&(canvas->marks[i].end),
						&(nxtInfo->segs),
						NULL, NULL))
	    return _DtCvSTATUS_BAD;

	*ret_info = (_DtCvPointInfo **) _DtCvAddPtrToArray((void **) *ret_info,
							(void *) nxtInfo);
	if (NULL == *ret_info)
	    return _DtCvSTATUS_BAD;
	  }

    return _DtCvSTATUS_OK;
}

/******************************************************************************
 *                             Public Functions
 ******************************************************************************/
/*****************************************************************************
 * Function:	_DtCanvasGetSelection()
 *
 * Purpose:	Indicate the end point for a selection.
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasGetSelection (
    _DtCvHandle		 canvas_handle,
    unsigned int	 mask,
    _DtCvPointer	*ret_select)
{
    _DtCanvasStruct     *canvas = (_DtCanvasStruct *) canvas_handle;

    *ret_select = NULL;
    return(GetSelectedText(canvas, canvas->select_start, canvas->select_end,
							mask, ret_select));
}

/*****************************************************************************
 * Function:	_DtCanvasProcessSelection()
 *
 * Purpose:	Indicate an new point for a selection.
 *
 *****************************************************************************/
void
_DtCanvasProcessSelection (
    _DtCvHandle		canvas_handle,
    _DtCvUnit		x,
    _DtCvUnit		y,
    _DtCvSelectMode	mode)
{
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;
    _DtCvSelectData	 temp;

    switch (mode)
	  {
	case _DtCvSELECTION_CLEAR:
		CheckAndSwitchPoints(&(canvas->select_start),
							&(canvas->select_end));

	case _DtCvSELECTION_START:
		_DtCvDrawAreaWithFlags(canvas, canvas->select_start,
				canvas->select_end,
				_DtCvSELECTED_FLAG, 0,
				_DtCvBAD_TYPE, NULL);

		canvas->select_start = defaultSelect;
		if (mode == _DtCvSELECTION_START)
		    SearchForClosestLine(canvas, x, y, &(canvas->select_start));

		canvas->select_end   = canvas->select_start;
		break;

	case _DtCvSELECTION_END:
	case _DtCvSELECTION_UPDATE:
		SearchForClosestLine(canvas, x, y, &temp);

		AdjustSelection (canvas, temp);
		if (mode == _DtCvSELECTION_END)
		    CheckAndSwitchPoints(&(canvas->select_start),
							&(canvas->select_end));
		break;
      }
}

/*****************************************************************************
 * Function:	_DtCanvasGetSelectionPoints()
 *
 * Purpose:	Retrieve the segments making up the selection.
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasGetSelectionPoints (
    _DtCvHandle		   canvas_handle,
    _DtCvSegPts		***ret_segs,
    _DtCvUnit		  *ret_y1,
    _DtCvUnit		  *ret_y2)
{
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;

    return (GetSegsInArea(canvas, &(canvas->select_start),
			&(canvas->select_end), ret_segs, ret_y1, ret_y2));
}

/*****************************************************************************
 * Function:	_DtCanvasActivatePts()
 *
 * Purpose:	Activate the points given.
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasActivatePts (
    _DtCvHandle		 canvas_handle,
    unsigned int	 mask,
    _DtCvPointInfo	*info,
    _DtCvUnit		*ret_y1,
    _DtCvUnit		*ret_y2)
{
    int			 markIdx;
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;
    _DtCvSelectData	 startSel = defaultSelect;
    _DtCvSelectData	 endSel   = defaultSelect;
    _DtCvFlags		 flag;
    _DtCvSegmentI	*firstSeg;

#define	REQUIRE_SEGS \
		(_DtCvACTIVATE_MARK | _DtCvACTIVATE_SELECTION)
    /*
     * check to see if there is anything to do
     */
    if (0 == mask)
	return _DtCvSTATUS_NONE;

    if ((mask & _DtCvACTIVATE_MARK) && (mask & _DtCvDEACTIVATE))
	return _DtCvSTATUS_BAD;

    /*
     * Convert the segments into starting and ending positions.
     */
    if (((mask & _DtCvDEACTIVATE) && NULL == info->client_data)
		|| (mask & REQUIRE_SEGS))
      {
	if (NULL == info || NULL == info->segs ||
		_DtCvSTATUS_BAD == _DtCvCvtSegsToPts(canvas, info->segs,
						&startSel, &endSel,
						ret_y1, ret_y2, &firstSeg))
	    return _DtCvSTATUS_BAD;
      }

    /*
     * Activate as a selection
     */
    if (mask & _DtCvACTIVATE_SELECTION)
      {
	_DtCanvasProcessSelection (canvas_handle, 0, 0, _DtCvSELECTION_CLEAR);

	canvas->select_start = startSel;
	canvas->select_end   = endSel;

	_DtCvDrawAreaWithFlags (canvas, startSel, endSel,
						0, _DtCvSELECTED_FLAG,
						_DtCvBAD_TYPE, NULL);
      }

    /*
     * Activate as a mark
     */
    if (mask & _DtCvACTIVATE_MARK)
      {
	int		travIdx;
	_DtCvUnit	x;
	_DtCvUnit	y;
	_DtCvUnit	width;
	_DtCvUnit	height;

	markIdx = _DtCvAddToMarkList(canvas, info->client_data,
				_DtCvIsMarkMaskOn(mask), &startSel, &endSel);
	if (-1 == markIdx)
	    return _DtCvSTATUS_BAD;

	/*
	 * now put the mark in the traversal list and merge it into the
	 * proper place.
	 */
	travIdx = _DtCvGetNextTravEntry(canvas);
	if (-1 == travIdx
		|| 0 != _DtCvSetTravEntryInfo(canvas, travIdx,
						_DtCvTraversalMark, firstSeg,
						markIdx, _DtCvTRUE)
		|| 0 != _DtCvCalcMarkPos(canvas, markIdx,
						&x, &y, &width, &height)
		|| 0 != _DtCvSetTravEntryPos(canvas, travIdx,
						x, y, width, height))
	    return _DtCvSTATUS_BAD;

	_DtCvSortTraversalList(canvas, _DtCvTRUE);

	/*
	 * draw these segments marked.
	 */
	flag = _DtCvMARK_FLAG;
	if (_DtCvTRUE == canvas->marks[markIdx].on)
	    flag |= _DtCvMARK_ON;

	_DtCvDrawAreaWithFlags (canvas, startSel, endSel,
							0, flag,
							_DtCvBAD_TYPE, NULL);
      }

    /*
     * Clear the mark flag.
     */
    else if (mask & _DtCvDEACTIVATE)
      {
	int	travIdx;

	/*
	 * is there anything to deacivate?
	 */
	if (NULL == canvas->marks || 0 == canvas->mark_cnt)
	    return _DtCvSTATUS_BAD;

	/*
	 * was client data specified? If so, then look for it and ignore
	 * the segment data.
	 */
	markIdx = 0;
	if (NULL != info->client_data)
	  {
	    while (markIdx < canvas->mark_cnt &&
		   canvas->marks[markIdx].client_data != info->client_data)
		markIdx++;

	    /*
	     * initialize the selection points
	     */
	    if (markIdx < canvas->mark_cnt)
	      {
		startSel = canvas->marks[markIdx].beg;
		endSel   = canvas->marks[markIdx].end;
	      }
	  }
	/*
	 * look for the marked set using the segments.
	 */
	else
	  {
	    while (markIdx < canvas->mark_cnt
		   && startSel.line_idx != canvas->marks[markIdx].beg.line_idx
		   && startSel.char_idx != canvas->marks[markIdx].beg.char_idx
		   && endSel.line_idx != canvas->marks[markIdx].end.line_idx
		   && endSel.line_idx != canvas->marks[markIdx].end.char_idx)
		markIdx++;
	  }

	if (markIdx >= canvas->mark_cnt)
	    return _DtCvSTATUS_BAD;

	/*
	 * draw these segments unmarked.
	 */
	flag = _DtCvMARK_FLAG;
	if (_DtCvTRUE == canvas->marks[markIdx].on)
	    flag |= _DtCvMARK_ON;

	canvas->marks[markIdx].on = _DtCvFALSE;
	_DtCvDrawAreaWithFlags (canvas, startSel, endSel, flag, 0,
						_DtCvBAD_TYPE, NULL);

	/*
	 * remove the mark from the traversal list
	 *
	 * first find the traversal entry of the mark and adjust any
	 * traversal mark index values to reflect that the mark
	 * list is about to shrink by 1.
	 */
	for (travIdx = 0; _DtCvTraversalMark != canvas->trav_lst[travIdx].type
			|| markIdx != canvas->trav_lst[travIdx].idx; travIdx++)
	  {
	    /*
	     * is this mark after the one being removed?
	     * if so, decrease its index because it's about to move.
	     */
	    if (_DtCvTraversalMark == canvas->trav_lst[travIdx].type &&
					markIdx < canvas->trav_lst[travIdx].idx)
		canvas->trav_lst[travIdx].idx--;
	  }

	/*
	 * move the list of traversal entries to eliminate the mark entry.
	 */
	while (travIdx + 1 < canvas->trav_cnt)
	  {
	    canvas->trav_lst[travIdx] = canvas->trav_lst[travIdx + 1];

	    /*
	     * is this a mark after the one being removed?
	     * if so, decrease it's index because it's about to move.
	     */
	    if (_DtCvTraversalMark == canvas->trav_lst[travIdx].type &&
					markIdx < canvas->trav_lst[travIdx].idx)
		canvas->trav_lst[travIdx].idx--;

	    travIdx++;
	  }

	/*
	 * update the traversal count and back up to the previous traversal
	 * if the mark was at the end of the traversal list.
	 */
	canvas->trav_cnt--;
	if (canvas->cur_trav >= canvas->trav_cnt)
	    canvas->cur_trav--;

	/*
	 * move the list of marks up
	 */
	while (markIdx + 1 < canvas->mark_cnt)
	  {
	    canvas->marks[markIdx] = canvas->marks[markIdx + 1];
	    markIdx++;
	  }

	canvas->mark_cnt--;
      }
    else if ((_DtCvACTIVATE_MARK_ON | _DtCvACTIVATE_MARK_OFF) & mask)
      {
	markIdx = 0;
	if (NULL != info && NULL != info->client_data)
	  {
	    while (markIdx < canvas->mark_cnt &&
		   canvas->marks[markIdx].client_data != info->client_data)
		markIdx++;

	    /*
	     * was a mark with this client data found?
	     */
	    if (markIdx >= canvas->mark_cnt)
	        return _DtCvSTATUS_BAD;
	  }
	else
	  {
	    /*
	     * are there any traversals? Is the current one sitting on a mark?
	     */
	    if (0 == canvas->trav_cnt || -1 == canvas->cur_trav ||
		_DtCvTraversalMark != canvas->trav_lst[canvas->cur_trav].type)
	        return _DtCvSTATUS_BAD;

	    /*
	     * get the mark index
	     */
	    markIdx = canvas->trav_lst[canvas->cur_trav].idx;
	  }

	/*
	 * is this different than what it is set at now? If not, do nothing.
	 */
	if (_DtCvIsMarkMaskOn(mask) == canvas->marks[markIdx].on)
	    return _DtCvSTATUS_NONE;

	/*
	 * set to mask value.
	 */
	canvas->marks[markIdx].on = _DtCvIsMarkMaskOn(mask);

	/*
	 * set the flags correctly.
	 */
	flag = _DtCvMARK_FLAG;
	if (_DtCvTRUE == canvas->marks[markIdx].on)
	    flag |= _DtCvMARK_ON;
	if (_DtCvTRUE == canvas->trav_on &&
			markIdx == canvas->trav_lst[canvas->cur_trav].idx)
	    flag |= _DtCvTRAVERSAL_FLAG;

	/*
	 * draw the mark opposite what it was
	 */
	_DtCvDrawAreaWithFlags (canvas, canvas->marks[markIdx].beg,
				canvas->marks[markIdx].end,
				(flag ^ _DtCvMARK_ON), flag,
				_DtCvBAD_TYPE, NULL);
      }

    return _DtCvSTATUS_OK;
}

/*****************************************************************************
 * Function:	_DtCanvasGetMarkPositions()
 *
 * Purpose:	Return the position in the canvas of the marks.
 *
 *****************************************************************************/
_DtCvStatus
_DtCanvasGetMarkPositions (
    _DtCvHandle		    canvas_handle,
    _DtCvMarkPos	 ***ret_pos)
{
    int  i;
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;
    _DtCvMarkPos	*nextPos;
    _DtCvMarkData	*markLst = canvas->marks;

    *ret_pos = NULL;

    if (0 == canvas->mark_cnt)
	return _DtCvSTATUS_NONE;

    for (i = 0; i < canvas->mark_cnt; i++, markLst++)
      {
	/*
	 * malloc memory for the information
	 */
	nextPos = (_DtCvMarkPos *) malloc (sizeof(_DtCvMarkPos));
	if (NULL == nextPos)
	    return _DtCvSTATUS_BAD;

	/*
	 * client data and baselines
	 */
	nextPos->client_data = markLst->client_data;
	nextPos->baseline1   = canvas->txt_lst[markLst->beg.line_idx].baseline;
	nextPos->baseline2   = canvas->txt_lst[markLst->end.line_idx].baseline;

	/*
	 * top left corner
	 */
	nextPos->x1 = markLst->beg.x;
	nextPos->y1 = nextPos->baseline1 -
				canvas->txt_lst[markLst->beg.line_idx].ascent;

	/*
	 * bottom right corner
	 */
	nextPos->x2 = markLst->end.x;
	nextPos->y2 = nextPos->baseline2 +
				canvas->txt_lst[markLst->end.line_idx].descent;

	*ret_pos = (_DtCvMarkPos **) _DtCvAddPtrToArray((void **) *ret_pos,
							(void *) nextPos);
	if (NULL == *ret_pos)
	    return _DtCvSTATUS_BAD;
      }

    return _DtCvSTATUS_OK;
}
