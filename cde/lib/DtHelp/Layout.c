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
/* $XConsortium: Layout.c /main/31 1996/10/25 12:00:15 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Layout.c
 **
 **   Project:     CDE Info System
 **
 **   Description: Lays out the information on a canvas.
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
#include <stdio.h>
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
#include "bufioI.h"
#include "CanvasI.h"
#include "CvStringI.h"
#include "LayoutUtilI.h"
#include "SelectionI.h"
#include "VirtFuncsI.h"

#ifdef NLS16
#include <nl_types.h>
#endif

/******************************************************************************
 *
 * Private Macros
 *
 *****************************************************************************/
#define	ObjHorizOrient(x)	(_DtCvContainerOrientOfSeg(x))
#define	ObjVertOrient(x)	(_DtCvContainerVOrientOfSeg(x))
#define	TxtHorizJustify(x)	(_DtCvContainerJustifyOfSeg(x))
#define	TxtVertJustify(x)	(_DtCvContainerVJustifyOfSeg(x))
#define	Border(x)		(_DtCvContainerBorderOfSeg(x))
#define	BrdWidth(x)		(_DtCvContainerLineWidthOfSeg(x))
#define	BrdData(x)		(_DtCvContainerLineDataOfSeg(x))
#define	NotJoining(x)		(False == (x)->info.join)
#define	JoinSet(x)		(True == (x)->info.join)

#define CheckAddToHyperList(a, b) \
	_DtCvCheckAddHyperToTravList(a, b, _DtCvTRUE, \
			&(layout->info.lst_vis), \
			&(layout->info.lst_hyper), &(layout->info.cur_len))

/*****************************************************************************
 *		Private Defines
 *****************************************************************************/
/*
 * Defines for the dimension arrays
 */
#define	DIMS_LEFT	0
#define	DIMS_RIGHT	1

#define	DIMS_LM		0
#define	DIMS_CENTER	1
#define	DIMS_RM		2

#define	DIMS_TOP	0
#define	DIMS_BOTTOM	2

#define	DIMS_WIDTH	0
#define	DIMS_HEIGHT	1
#define	DIMS_YPOS	1

#define	DIMS_TC		0
#define	DIMS_BC		1


/*
 */
#define	GROW_SIZE	10

/******************************************************************************
 *
 * Private typedefs
 *
 *****************************************************************************/
/*
 * top/bottom dimension array
 *                  -------------------------------------------
 *                 / DIMS_HEIGHT / DIMS_HEIGHT / DIMS_HEIGHT /|
 *                /   DIMS_YPOS /   DIMS_YPOS /   DIMS_YPOS / |
 *               /-------------/-------------/-------------/  |
 *              / DIMS_WIDTH  / DIMS_WIDTH  / DIMS_WIDTH  /| /|
 *             --------------|-------------|-------------| |/ |
 * DIMS_TOP    |   DIMS_LM   | DIMS_CENTER |   DIMS_RM   | | /|
 *             |-------------|-------------|-------------|/|/ |
 *  unused     |             |             |             | | /
 *             |-------------|-------------|-------------|/|/
 * DIMS_BOTTOM |   DIMS_LM   | DIMS_CENTER |   DIMS_RM   | /
 *             ------------------------------------------|/
 */
typedef	_DtCvUnit	TopDims[DIMS_BOTTOM+1][DIMS_RM+1][DIMS_HEIGHT+1];

/* 
 * left/right side dimension array - contains only the height or y_pos.
 *             ----------------------------
 * DIMS_TOP    |  DIMS_LEFT | DIMS_RIGHT  |
 *             |------------|-------------|
 * DIMS_CENTER |  DIMS_LEFT | DIMS_RIGHT  |
 *             |------------|-------------|
 * DIMS_BOTTOM |  DIMS_LEFT | DIMS_RIGHT  |
 *             ----------------------------
 */
typedef	_DtCvUnit	SideDims[DIMS_BOTTOM+1][DIMS_RIGHT+1];

/*
 * corner dimension array - contains only the height or y_pos.
 *            ----------------------------
 * DIMS_TC    |  DIMS_LEFT | DIMS_RIGHT  |
 *            |------------|-------------|
 * DIMS_BC    |  DIMS_LEFT | DIMS_RIGHT  |
 *            ----------------------------
 */
typedef	_DtCvUnit	CornerDims[DIMS_BC+1][DIMS_RIGHT+1];

/*
 * flow dimension array
 *            -------------|--------------
 * DIMS_LEFT  | DIMS_WIDTH | DIMS_HEIGHT |
 *            |            |  DIMS_YPOS  |
 *            |------------|-------------|
 * DIMS_RIGHT | DIMS_WIDTH | DIMS_HEIGHT |
 *            |            |  DIMS_YPOS  |
 *            ----------------------------
 */
typedef	_DtCvUnit	FlowDims[DIMS_RIGHT+1][DIMS_HEIGHT+1];

/*
 * margin data - important for determining flowing
 */
typedef	struct	_dataPoint {
	_DtCvUnit	left;
	_DtCvUnit	right;
	_DtCvUnit	y_pos;
	_DtCvUnit	x_units;
	struct _dataPoint *next_pt;
} DataPoint;

/*
 * count information
 */
typedef	struct	_cntInfo {
	int		beg_txt;
	int		end_txt;
	int		beg_ln;
	int		end_ln;
	int		my_lines;
	int		beg_brk;
	int		end_brk;
} CntInfo;

/*
 * group information
 */
typedef	struct	_grpInfo {
	_DtCvUnit	min_x;
	_DtCvUnit	max_x;
	_DtCvUnit	top_y;
	_DtCvUnit	bot_y;
	CntInfo		cnt;
	struct _grpInfo *next_info;
} GrpInfo;

/*
 * layout information per container
 */
typedef	struct	_layFrmtInfo {
	_DtCvUnit	height;
	_DtCvUnit	width;
	CntInfo		cnt;
	struct _layFrmtInfo *next_info;
} LayFrmtInfo;

/*
 * the layout information carried around
 */
typedef	struct	_layoutInfo {
	_DtCvUnit	 max_width;	/* the current max width             */
	_DtCvUnit	 id_Ypos;	/* the y coordinate of the id found. */
	_DtCvUnit	 left;		/* the current left margin for the
					   current active container (incl br)*/
	_DtCvUnit	 right;		/* the current right margin for the
					   current active container (incl br)*/
	_DtCvUnit	 first;		/* the current first indent          */
	_DtCvUnit	 lmargin;	/* the current absolute left  margin */
	_DtCvUnit	 rmargin;	/* the current absolute right margin */
	_DtCvUnit	 divisor;	/* the current margin divisor        */
	_DtCvUnit	 string_end;
	_DtCvUnit	 sub_end;
	_DtCvUnit	 super_end;
	_DtCvValue	 id_found;	/* indicates if the id has been found*/
	_DtCvValue	 super_script;
	_DtCvValue	 sub_script;
	_DtCvValue	 stat_flag;
	_DtCvValue	 margin_non_zero;
	_DtCvValue	 brdr_flag;     /* within container with a border    */
	_DtCvValue	 table_flag;    /* within table                      */
	_DtCvFrmtOption	 txt_justify;
	unsigned int	 cur_start;	/* offset into the current segment
					   to process                        */
	_DtCvSegmentI	*lst_rendered;	/* indicates the last string/region  */
	char		*target_id;	/* if non-null, the id to search for */
        DataPoint	*data_pts;
	GrpInfo		*grp_lst;	/* list of groups		*/
	_DtCvLayoutInfo	 info;
} LayoutInfo;

/*
 * information for laying out cells in a table.
 */
typedef	struct	{
	int		 col_spn;
	int		 row_spn;
	_DtCvUnit	 pos_x;
	LayFrmtInfo	 info;
	_DtCvSegmentI	*cell_seg;
} CellInfo;

/*
 * column description information for a table.
 */
typedef	struct _columnSpec {
	_DtCvUnit	min;
	_DtCvUnit	max;
	_DtCvUnit	actual;
	_DtCvValue	hanger;
	_DtCvFrmtOption	justify;
} ColumnSpec;

/*
 * row description information
 */
typedef	struct	_rowSpec {
	int		 column;
	_DtCvUnit	 y_adj;
	_DtCvUnit	 height;
	_DtCvUnit	 lst_height;
	char		*next_id;
} RowSpec;

/*****************************************************************************
 *		Private Variables/Data
 *****************************************************************************/
static const LayFrmtInfo DefLayFrmtInfo = { 0, 0, {-1, -1, -1, -1, 0}, NULL };
static const GrpInfo     DefGrpInfo     = { 0, 0, 0, 0, {-1, -1, -1, -1, 0}};

static	const LayoutInfo DefInfo = {
	0,		/* _DtCvUnit     max_width;    */
	0,		/* _DtCvUnit	 id_Ypos;      */
	0,		/* _DtCvUnit	 left;         */
	0,		/* _DtCvUnit	 right;        */
	0,		/* _DtCvUnit	 first;        */
	0,		/* _DtCvUnit	 lmargin;      */
	0,		/* _DtCvUnit	 rmargin;      */
	1,		/* _DtCvUnit	 divisor;      */
	0,		/* _DtCvUnit	 string_end;   */
	0,		/* _DtCvUnit	 sub_end;      */
	0,		/* _DtCvUnit	 super_end;    */
	False,		/* _DtCvValue	 id_found;     */
	False,		/* _DtCvValue	 super_script; */
	False,		/* _DtCvValue	 sub_script;   */
	False,		/* _DtCvValue	 stat_flag;    */
	False,		/* _DtCvValue	 margin_non_zero; */
	False,		/* _DtCvValue	 brdr_flag;    */
	False,		/* _DtCvValue	 table_flag;   */
	_DtCvJUSTIFY_LEFT,	/* _DtCvFrmtOption txt_justify;   */
	0,		/* unsigned int	 cur_start;    */
	NULL,		/* _DtCvSegmentI *lst_rendered; */
	NULL,		/* char		*target_id;    */
        NULL,		/* DataPoint	*data_pts;     */
        NULL,		/* GrpInfo  	*data_pts;     */
};

static const DataPoint	DefDataPt = { 0, 0, _CEFORMAT_ALL, 0, NULL};

static const double	HeadDivisor = 10000.0;

static const char *PeriodStr = ".";

static const char  *DefWidth[2] = { "", NULL };

static	const	_DtCvSegmentI	BlankTableCell =
  {
	_DtCvCONTAINER,		/* type         */
	-1,			/* link_idx     */
	  {			/* container info */
	    { NULL },		  /* id           */
	    NULL,		  /* justify_char */
	    _DtCvDYNAMIC,	  /* type         */
	    _DtCvBORDER_NONE,	  /* border       */
	    _DtCvJUSTIFY_LEFT,	  /* justify      */
	    _DtCvJUSTIFY_TOP,	  /* vjustify     */
	    _DtCvJUSTIFY_LEFT,	  /* orient       */
	    _DtCvJUSTIFY_TOP,	  /* vorient      */
	    _DtCvWRAP_NONE,	  /* flow         */
	    0,			  /* percent      */
	    0,			  /* leading      */
	    0,			  /* fmargin      */
	    0,			  /* lmargin      */
	    0,			  /* rmargin      */
	    0,			  /* tmargin      */
	    0,			  /* bmargin      */
	    _DtCvBORDER_NONE,  /* bdr_info     */
	    NULL 		  /* seg_list     */
	  },
	NULL,			/* next_seg     */
	NULL,			/* next_disp    */
	NULL,			/* client_use   */
	NULL 			/* internal_use */
  };

/*****************************************************************************
 *		Private Function Declarations
 *****************************************************************************/
static	void		AdjustForBorders(
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvFrmtOption	 brdr,
				_DtCvUnit	 line_width,
				_DtCvUnit	*ret_bot,
				_DtCvUnit	*ret_right);
static	void		AdjustHeadPosition(
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvSegmentI	*p_seg,
				TopDims		*top_bot,
				SideDims	*side,
				CornerDims	*corner,
				FlowDims	*flow,
				LayFrmtInfo	*info,
				_DtCvUnit	 base_y,
				_DtCvUnit	 base_left,
				_DtCvUnit	 block_width,
				_DtCvUnit	 left_margin,
				_DtCvUnit	 right_margin);
static	void		AdjustObjectPosition(
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvFrmtOption	 justify,
				int		 start_txt,
				int		 start_ln,
				int		 start_brk,
				int		 end_txt,
				int		 end_ln,
				int		 end_brk,
				int		 brdr_cnt,
				_DtCvUnit	 height_adj,
				_DtCvUnit	 x_adj,
				_DtCvUnit	 y_adj,
				_DtCvUnit	 internal_y);
static	void		CheckSaveInfo (
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvSegmentI	*new_seg,
				int		 start);
static	void		DetermineFlowConstraints(
				LayoutInfo	*layout,
				FlowDims	 flow_dims,
				_DtCvUnit	 left_margin,
				_DtCvUnit	 right_margin,
				_DtCvUnit	 start_y,
				DataPoint	*left_pt,
				DataPoint	*right_pt);
static	void		DetermineHeadPositioning(
				TopDims	*top_bot,
				SideDims	*side,
				CornerDims	*corner,
				FlowDims	*flow,
				_DtCvUnit	 start_y,
				_DtCvUnit	 max_top,
				_DtCvUnit	 block_size,
				_DtCvUnit	*ret_side_size);
static	void		DetermineMaxDims(
				TopDims		*top_bot,
				CornerDims	*corner,
				_DtCvUnit	 left_margin,
				_DtCvUnit	 right_margin,
				_DtCvUnit	*top_height,
				_DtCvUnit	*bot_height,
				_DtCvUnit	*max_width);
static	int		DrawBorders(
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvFrmtOption	 brdr,
				_DtCvPointer	 data,
				_DtCvUnit	 line_width,
				_DtCvUnit	 top_y,
				_DtCvUnit	 bot_y,
				_DtCvUnit	 left_x,
				_DtCvUnit	 right_x);
static void		FormatCell(
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvSegmentI	*cell_seg,
				_DtCvUnit	 span_width,
				_DtCvUnit	 min_height,
				DataPoint	 base_pt,
				int		*ret_ln,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_height,
				_DtCvValue	*ret_tab_flag);
static	void		InitDimArrays(
				TopDims		*top_bot,
				SideDims	*side,
				CornerDims	*corner,
				FlowDims	*flow);
static _DtCvStatus	LayoutCanvasInfo (
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvUnit	 divisor,
				char		*target_id);
static _DtCvValue	LinesMayChange(
				_DtCanvasStruct	*canvas,
				int		 start_ln,
				int		 end_ln,
				int		 brdr_cnt);
static _DtCvUnit	MoveGroup (
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				GrpInfo		*tst_grp,
				_DtCvUnit	 needed);
static _DtCvUnit	MoveLines (
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				int		 idx,
				_DtCvUnit	 top_y,
				_DtCvUnit	 bot_y,
				_DtCvUnit	 needed);
static _DtCvUnit	MoveText (
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				int		 idx,
				_DtCvUnit	 top_y,
				_DtCvUnit	 bot_y,
				_DtCvUnit	 needed);
static void		ProcessContainer(
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvSegmentI	*con_seg,
				_DtCvUnit	 min_y,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_max_x,
				int		*ret_cnt);
static	void		ProcessSegmentList(
				_DtCanvasStruct	*canvas,
				LayoutInfo	*layout,
				_DtCvSegmentI	*cur_seg,
				_DtCvUnit	 min_y,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_max_x,
				int		**ret_vert);

/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function: AdjustTextPositions
 *
 * changes the baseline and text_x positioning of the text line by the
 * offsets indicated.
 *****************************************************************************/
static void
AdjustTextPositions(
    _DtCanvasStruct     *canvas,
    int			 beg,
    int			 end,
    _DtCvUnit		 x_offset,
    _DtCvUnit		 y_offset)
{
    if ( 0 != y_offset || 0 != x_offset)
      {
        while (beg < end)
          {
	    canvas->txt_lst[beg].baseline += y_offset;
	    canvas->txt_lst[beg].text_x   += x_offset;
	    beg++;
          }
      }
}

/******************************************************************************
 * Function: AdjustLinePositions
 *
 * changes the x & positions and the max x & y values by the offsets indicated.
 *****************************************************************************/
static void
AdjustLinePositions(
    _DtCanvasStruct     *canvas,
    int			 beg,
    int			 end,
    _DtCvUnit		 x_offset,
    _DtCvUnit		 y_offset)
{
    if ( 0 != y_offset || 0 != x_offset)
      {
        while (beg < end)
          {
	    canvas->line_lst[beg].pos_y += y_offset;
	    canvas->line_lst[beg].max_y += y_offset;
	    canvas->line_lst[beg].pos_x += x_offset;
	    canvas->line_lst[beg].max_x += x_offset;

	    beg++;
          }
      }
}

/******************************************************************************
 * Function: AdjustPgBrk
 *
 * changes the y position of a page break.
 *****************************************************************************/
static void
AdjustPgBrk(
    _DtCanvasStruct     *canvas,
    int			 beg,
    int			 end,
    _DtCvUnit		 y_offset)
{
    if (0 != y_offset)
      {
        while (beg < end)
	    canvas->pg_breaks[beg++] += y_offset;
      }
}

/******************************************************************************
 * Function: MaxXOfLine
 *
 *  MaxXOfLine determines the max X of a line segment.
 *
 *****************************************************************************/
/******************************************************************************
 * Function: MaxXOfLine
 *
 *  MaxXOfLine determines the max X of a line segment.
 *
 *****************************************************************************/
static _DtCvUnit
MaxXOfLine(
    _DtCanvasStruct     *canvas,
    _DtCvDspLine        *line)
{
    _DtCvValue   lastLinkVisible = FALSE;
    int          count        = line->length;
    int          start        = line->byte_index;
    int          len;
    int          lnkInd = -1;
    _DtCvUnit    xPos;
    _DtCvUnit    tmpWidth;
    _DtCvSegmentI *pSeg;

    xPos = _DtCvGetStartXOfLine(line, &pSeg);

    while (pSeg != NULL && count > 0)
      {
        xPos = _DtCvAdvanceXOfLine(canvas, pSeg, xPos,
						&lnkInd, &lastLinkVisible);

        _DtCvGetWidthOfSegment(canvas, pSeg, start, count,
							&len, &tmpWidth, NULL);
        xPos  += tmpWidth;
        count -= len;
        start  = 0;
        pSeg   = pSeg->next_disp;
      }

    return xPos;

} /* End MaxXOfLine */

/*****************************************************************************
 * Function:	void GetLinkInfo ()
 *
 * Parameters:
 *		canvas		Specifies the handle for the canvas.
 *
 * Returns:	A handle to the canvas or NULL if an error occurs.
 *
 * Purpose:
 *
 *****************************************************************************/
static _DtCvValue
GetLinkInfo (
    _DtCvHandle	  canvas_handle,
    int		  indx,
    _DtCvUnit	 *ret_x,
    _DtCvUnit	 *ret_y,
    _DtCvUnit	 *ret_width,
    _DtCvUnit	 *ret_height)
{
    int			 len;
    int			 line;
    int			 count;
    int			 startChar;
    int			 lnkIndx   = -1;
    _DtCvUnit		 startX;
    _DtCanvasStruct	*canvas    = (_DtCanvasStruct *) canvas_handle;
    _DtCvSegmentI	*pSeg;
    _DtCvValue           lstVisible = False;
    _DtCvValue		 found      = False;
    _DtCvValue		 junk;
    _DtCvUnit  endX;
    void                *pChar;

    /*
     * get the line index
     */
    line        = canvas->trav_lst[indx].idx;

    /*
     * get some information from the line
     */
    pSeg        = canvas->txt_lst[line].seg_ptr;
    count       = canvas->txt_lst[line].length;
    startChar   = canvas->txt_lst[line].byte_index;
    startX      = canvas->txt_lst[line].text_x;
    *ret_y      = canvas->txt_lst[line].baseline - canvas->txt_lst[line].ascent;
    *ret_height = canvas->txt_lst[line].ascent + canvas->txt_lst[line].descent;

    while (count > 0 && !found && pSeg != NULL)
      {
	if (startX < canvas->txt_lst[line].text_x)
	    startX = canvas->txt_lst[line].text_x;

	/*
	 * adjust the starting position by the link space
	 */
	junk = _DtCvIsSegVisibleLink(pSeg);
	lstVisible = _DtCvModifyXpos (canvas->link_info, pSeg, junk,
		    lstVisible, lnkIndx, &startX);
	/*
	 * adjust the starting position by the traversal space
	 */
	junk = _DtCvIsSegALink(pSeg);
	(void) _DtCvModifyXpos (canvas->traversal_info, pSeg, junk,
		    ((_DtCvValue) True), lnkIndx, &startX);

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
	    endX = startX + _DtCvGetStringWidth(canvas,pSeg,pChar,len);

	  }

	/*
	 * test to see if the selected segment was this segment.
	 */
	if (pSeg == canvas->trav_lst[indx].seg_ptr)
	  {
	    found = True;
	    *ret_x     = startX;
	    *ret_width = endX - startX;
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

    return found;
}
/******************************************************************************
 * Function: CheckId
 *
 * Check to see if the id matches the target id.
 * If so, set the 'found id' flag and indicate its y coordinate.
 *****************************************************************************/
static void
CheckId(
    LayoutInfo		*layout,
    char		*id)
{
    if (layout->target_id != NULL && NULL != id &&
			_DtCvStrCaseCmpLatin1(id, layout->target_id) == 0)
      {
        layout->id_Ypos  = layout->info.y_pos;
        layout->id_found = True;
      }
}

/******************************************************************************
 * Function: CheckSetLineStart
 *
 * Check to see if the line information is at the beginning.
 * If so, set the pointers to the current segment and offset.
 *****************************************************************************/
static void
CheckSetLineStart(
    LayoutInfo		*layout,
    _DtCvSegmentI	*cur_seg)
{
    if (layout->info.line_bytes == 0)
      {
	layout->info.line_seg   = cur_seg;
	layout->info.line_start = 0;
      }
}

/******************************************************************************
 * Function: CheckForPageBreak
 *
 * Check to see if there is a page break on this segment.
 * If so, remember its position.
 *****************************************************************************/
static int
CheckForPageBreak(
    _DtCanvasStruct	*canvas,
    _DtCvSegmentI	*cur_seg,
    _DtCvUnit		 position)
{
    /*
     * does this segment cause a page break?
     */
    if (_DtCvIsSegPageBreak(cur_seg))
      {
	/*
	 * check to see if there is room to save the page break.
	 * if not, allocate more room.
	 */
	if (canvas->brk_cnt >= canvas->brk_max)
	  {
	    canvas->brk_max += GROW_SIZE;
	    if (NULL != canvas->pg_breaks)
	        canvas->pg_breaks = (_DtCvUnit *) realloc (
					(void *) canvas->pg_breaks,
					(sizeof(_DtCvUnit) * canvas->brk_max));
	    else
	        canvas->pg_breaks = (_DtCvUnit *) malloc (
					(sizeof(_DtCvUnit) * canvas->brk_max));
	  }

	/*
	 * failed to allocate memory, abort.
	 */
	if (NULL == canvas->pg_breaks)
	  {
	    canvas->brk_max = 0;
	    canvas->brk_cnt = 0;
	    return 1;
	  }

	/*
	 * save the y position of the page break for later.
	 */
	canvas->pg_breaks[canvas->brk_cnt++] = position;
      }

    return 0;
}

/******************************************************************************
 * Function:	SetBeginCounts
 *
 * Parameters:
 *		canvas	Specifies the canvas.
 *		f_info	Specifies the layout count info.
 *
 * Returns:	Nothing
 *
 * Purpose:	Initializes the layout count information for a container.
 *****************************************************************************/
static	void
SetBeginCounts (
    _DtCanvasStruct	*canvas,
    CntInfo		*cnt_info)
{
    /*
     * text counts
     */
    cnt_info->beg_txt = canvas->txt_cnt;
    cnt_info->end_txt = canvas->txt_cnt;

    /*
     * line counts
     */
    cnt_info->beg_ln = canvas->line_cnt;
    cnt_info->end_ln = canvas->line_cnt;

    /*
     * break counts
     */
    cnt_info->beg_brk = canvas->brk_cnt;
    cnt_info->end_brk = canvas->brk_cnt;
}

/******************************************************************************
 * Function:	SetEndCounts
 *
 * Parameters:
 *		canvas	Specifies the canvas.
 *		f_info	Specifies the layout count info.
 *
 * Returns:	Nothing
 *
 * Purpose:	Sets the ending layout counts for a container.
 *****************************************************************************/
static	void
SetEndCounts (
    _DtCanvasStruct	*canvas,
    CntInfo		*cnt_info,
    int			 end_ln)
{
    /*
     * text counts
     */
    cnt_info->end_txt = canvas->txt_cnt;

    /*
     * line counts
     */
    cnt_info->end_ln = canvas->line_cnt;

    /*
     * break counts
     */
    cnt_info->end_brk = canvas->brk_cnt;

    /*
     * the number of lines for this container.
     * If negative, indicates the first line is for the
     * bottom of the container and goes the length.
     */
    cnt_info->my_lines = end_ln;
}

/******************************************************************************
 * Function:	SkipToNumber
 *
 * Returns:
 *****************************************************************************/
static	void
SkipToNumber (
    const char	**string)
{
    if (string != NULL)
      {
        const char *str = *string;

        while (*str == ' ' && *str != '\0')
	    str++;
        *string = str;
      }
}

/******************************************************************************
 * Function:	GetValueFromString
 *
 * Returns:
 *****************************************************************************/
static	int
GetValueFromString (
    const char	**string,
    int		  def_num)
{
    int  value = def_num;

    if (string != NULL && *string != NULL && **string != '\0')
      {
        const char *str = *string;

	if ('0' <= *str && *str <= '9')
	    value = atoi(str);

	while ('0' <= *str && *str <= '9')
	    str++;

	while (*str != ' ' && *str != '\0' && (*str < '0' || *str > '9'))
	    str++;

        *string = str;
      }

    return value;
}

/******************************************************************************
 * Function:	PushDataPoint
 *
 * Returns:
 *****************************************************************************/
static	void
PushDataPoint (
    LayoutInfo		*layout,
    DataPoint		*data_pt)
{
    data_pt->x_units = 0;
    data_pt->next_pt = layout->data_pts;
    layout->data_pts = data_pt;
}

/******************************************************************************
 * Function:	InsertDataPoint
 *
 * Returns:
 *****************************************************************************/
static	void
InsertDataPoint (
    LayoutInfo		*layout,
    DataPoint		*data_pt)
{
    DataPoint	*lastPt = NULL;
    DataPoint	*nextPt = layout->data_pts;

    while (nextPt != NULL &&
	nextPt->y_pos != _CEFORMAT_ALL &&
	(data_pt->y_pos == _CEFORMAT_ALL || nextPt->y_pos < data_pt->y_pos))
      {
	lastPt = nextPt;
	nextPt = nextPt->next_pt;
      }

    data_pt->next_pt = nextPt;
    data_pt->x_units = 0;

    if (lastPt == NULL)
        layout->data_pts = data_pt;
    else
	lastPt->next_pt = data_pt;
}

/******************************************************************************
 * Function:	RemoveDataPoint
 *
 * Returns:
 *****************************************************************************/
static	void
RemoveDataPoint (
    LayoutInfo		*layout,
    DataPoint		*data_pt)
{
    DataPoint	*lastPt = NULL;
    DataPoint	*curPt  = layout->data_pts;

    while (curPt != NULL && curPt != data_pt)
      {
	lastPt = curPt;
	curPt  = curPt->next_pt;
      }

    if (curPt != NULL)
      {
        data_pt->x_units = layout->info.cur_max_x - data_pt->left;
        if (lastPt == NULL)
	    layout->data_pts = curPt->next_pt;
        else
	    lastPt->next_pt  = curPt->next_pt;
      }
}

/******************************************************************************
 * Function:	GetCurrentDataPoint
 *
 * Returns:
 *****************************************************************************/
static	void
GetCurrentDataPoint (
    LayoutInfo		*layout,
    DataPoint		*data_pt)
{
    data_pt->left  = 0;
    data_pt->right = 0;
    data_pt->y_pos = _CEFORMAT_ALL;

    if (layout->data_pts != NULL)
	*data_pt = *(layout->data_pts);
}

/******************************************************************************
 * Function:	SetMargins
 *
 * Purpose: Sets the margins.
 *****************************************************************************/
static	void
SetMargins (
    LayoutInfo		*layout)
{
    layout->lmargin = 0;
    layout->rmargin = 0;
    layout->info.format_y = _CEFORMAT_ALL;

    if (layout->data_pts != NULL)
      {
	/*
	 * base 
	 */
	layout->lmargin = layout->data_pts->left;
	layout->rmargin = layout->data_pts->right;

        layout->info.format_y = layout->data_pts->y_pos;
      }

    layout->lmargin += layout->left;
    layout->rmargin += layout->right;
}

/******************************************************************************
 * Function:	SetTextPosition
 *
 * Purpose: Sets the text beginning position to the left margin.
 *	    If 'first' is true, adds its value to the text beginning
 *	    position value.
 *****************************************************************************/
static	void
SetTextPosition (
    LayoutInfo		*layout,
    _DtCvValue		 first)
{
    layout->info.text_x_pos = layout->lmargin;
    if (first == True)
        layout->info.text_x_pos += layout->first;
}

/******************************************************************************
 * Function:	CheckFormat
 *
 * Purpose: Checks to see if the flowing txt boundaries have been exceeded.
 *          If a boundary has been exceeded, then removes that boundary
 *	    information from the stack until it finds a valid boundary point.
 *
 *	    Calls SetMargins to set the correct margin
 *	    Calls SetTextPosition to set the text beginning position.
 *****************************************************************************/
static	void
CheckFormat (
    LayoutInfo		*layout,
    _DtCvValue		 first)
{
    while (layout->data_pts != NULL &&
		layout->data_pts->y_pos != _CEFORMAT_ALL &&
				layout->data_pts->y_pos < layout->info.y_pos)
	RemoveDataPoint (layout, layout->data_pts);

    SetMargins(layout);
    SetTextPosition(layout, first);
}

/******************************************************************************
 * Function: SaveLine
 *
 * Initializes a line table element to the segment it should display.
 *****************************************************************************/
static void
SaveLine (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    int			 direction,
    _DtCvPointer	 data,
    _DtCvUnit		 line_width,
    _DtCvUnit		 x,
    _DtCvUnit		 y,
    _DtCvUnit		 length)
{
    int       i  = canvas->line_cnt;
    _DtCvUnit x2 = x;
    _DtCvUnit y2 = y;

    if (i >= canvas->line_max)
      {
	canvas->line_max += GROW_SIZE;
	if (canvas->line_lst)
	    canvas->line_lst = (_DtCvLineSeg *) realloc (
				(void *) canvas->line_lst,
				(sizeof(_DtCvLineSeg) * canvas->line_max));
	else
	    canvas->line_lst = (_DtCvLineSeg *) malloc (
				(sizeof(_DtCvLineSeg) * canvas->line_max));
/*
 * NOTE....should this routine return a value?
 * If (re)alloc error occurs, this simply ignores the problem.
 */
	if (canvas->line_lst == NULL)
	  {
	    canvas->line_max = 0;
	    canvas->line_cnt = 0;
	    return;
	  }
      }

    /*
     * does this line exceed the current maximum?
     */
    if (_DtCvLINE_HORZ == direction)
      {
	x2 += length;
	y2 += line_width;
      }
    else
      {
	x2 += line_width;
	y2 += length;
      }

    if (layout->info.max_x_pos < x2)
	layout->info.max_x_pos = x2;

    if (layout->info.cur_max_x < x2)
	layout->info.cur_max_x = x2;

    /*
     * save the line information
     */
    canvas->line_lst[i].dir    = direction;
    canvas->line_lst[i].pos_x  = x;
    canvas->line_lst[i].max_x  = x2;
    canvas->line_lst[i].pos_y  = y;
    canvas->line_lst[i].max_y  = y2;
    canvas->line_lst[i].width  = line_width;
    canvas->line_lst[i].data   = data;

    canvas->line_cnt++;
}

/******************************************************************************
 * Function: SaveInfo
 *
 * Purpose: Saves the current information into the txt line struct.
 *          Checks to see if this line exceeds the flowing text
 *          boundary and resets the internal global margins.
 *****************************************************************************/
static void
SaveInfo (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*new_seg,
    int			 start)
{
    _DtCvSaveInfo (canvas, &(layout->info),
		  layout->max_width, layout->rmargin, layout->txt_justify);

    while (layout->info.delayed_search_saves > 0) {
	_DtCvSetSearchEntryInfo(canvas, canvas->txt_cnt - 1);
	layout->info.delayed_search_saves--;
    }

    layout->super_end       = 0;
    layout->sub_end         = 0;
    layout->super_script    = False;
    layout->sub_script      = False;
    layout->info.line_seg   = new_seg;
    layout->info.line_start = start;
    layout->info.cur_len    = 0;

    CheckFormat(layout, FALSE);
}

/******************************************************************************
 * Function: CheckSaveInfo
 *
 * Purpose: Checks to see if there is any information to save and saves
 *          it if there is any.
 *****************************************************************************/
static void
CheckSaveInfo (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*new_seg,
    int			 start)
{
    if (layout->info.line_bytes)
	SaveInfo (canvas, layout, new_seg, start);
}

/******************************************************************************
 * Function: ProcessStringSegment
 *
 * chops a string segment up until its completely used.
 *
 *****************************************************************************/
static void
ProcessStringSegment(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*cur_seg)
{
    layout->cur_start = 0;
    if (!(_DtCvIsSegSuperScript(cur_seg) || _DtCvIsSegSubScript(cur_seg))
	&& (layout->super_script == True || layout->sub_script == True))
      {
	layout->super_end = 0;
	layout->sub_end   = 0;
	layout->super_script = False;
	layout->sub_script   = False;
      }

    while (_DtCvProcessStringSegment(canvas, &(layout->info),
			layout->max_width, layout->lmargin, layout->rmargin,
			cur_seg , &(layout->cur_start),
			layout->txt_justify, layout->stat_flag) == 1)
	CheckFormat(layout, False);

} /* End ProcessStringSegment */


/******************************************************************************
 * Function: _DtCvUnit ResolveHeight(
 *
 * Parameters:
 *
 * Purpose: Determines the height of a row that is spanned.
 *****************************************************************************/
static _DtCvUnit
ResolveHeight(
    RowSpec  *row_info,
    CellInfo *cell_info,
    int       max_cols,
    int       row,
    int       span)
{
    int i;
    int col;
    int cell;
    int topRow;
    int topCell;
    int zeroed;
    int total;
    int rowHeight = 0;

    for (col = 0, cell = row * max_cols; col < max_cols; col++, cell++)
      {
	/*
	 * if we have a row spanning cell in this column,
	 * but it isn't from a previous column, try to fill out
	 * the height using it.
	 */
	if (cell_info[cell].row_spn == -1 && cell_info[cell].col_spn != -1)
	  {
	    /*
	     * back up to the cell information containing the row
	     * span value.
	     */
	    topRow  = row;
	    topCell = cell;
	    do
	      {
		topCell--;
		topRow--;
	      } while (cell_info[topCell].row_spn == -1);

	    /*
	     * from here, start calculating the height of the row
	     * spanning cell, and find out how big the row must
	     * be to contain it.
	     */
	    i      = cell_info[topCell].row_spn;
	    zeroed = span + 1;
	    total  = 0;
	    while (i > 0 && zeroed > 0)
	      {
		total += row_info[topRow].height;
		if (row_info[topRow].height == 0)
		   zeroed--;
		i--;
		topRow++;
	      }

	    /*
	     * if zeroed is greater than zero, that means only the
	     * allowd number of row heights were zeroed out (the ones
	     * we are looking for).
	     * go ahead and calculate a new height. Otherwise,
	     * it may be tried later.
	     */
	    if (zeroed > 0)
	      {
		/*
		 * make sure we get a positive value out of
		 * this for our height.
		 */
		if (cell_info[topCell].info.height > total)
		  {
		    total = cell_info[topCell].info.height - total;
		    total = total / span + (total % span ? 1 : 0);
		  }

		/*
		 * make sure we take the biggest value possible
		 * for this row. If it needs to be smaller for
		 * cells, we'll adjust the positioning within
		 * the cell.
		 */
		if (rowHeight < total)
		    rowHeight = total;
	      }
	  }
      }

   row_info[row].height = rowHeight;
   return rowHeight;
}

/******************************************************************************
 * Function: void AdjustHeight(
 *
 * Parameters:
 *
 * Purpose: Determines the height of rows that are spanned but the 
 *          the spanner needs more room than the calculated row height
 *          allow.
 *****************************************************************************/
static void
AdjustHeight(
   CellInfo	*cell_info,
   RowSpec	*row_specs,
   int		 max_cols,
   int		 row,
   int		 col)
{
    int    cell = row * max_cols + col;
    int    i, j;
    _DtCvUnit   total;
    _DtCvUnit   adjustValue;
    _DtCvUnit   value;

    if (cell_info[cell].col_spn == -1 || cell_info[cell].row_spn == -1)
	return;

    for (j = row, i = 0, total = 0; i < cell_info[cell].row_spn; i++, j++)
	total += row_specs[j].height;

    /*
     * adjust the row height to include all of the row spanning cell.
     */
    if (total < cell_info[cell].info.height)
      {
	_DtCvUnit   totalUsed = 0;

	/*
	 * first, try to grow the cells on a percentage basis.
	 * This way, a small cell will grow the same amount relative
	 * to the larger cells.
	 */
	adjustValue = cell_info[cell].info.height - total;

	/*
	 * now if total is zero, we'll get a divide by zero error.
	 * So check for this and set total to the number of rows spanned.
	 */
	if (0 == total)
	    total = cell_info[cell].row_spn;

	for (i = 0; i < cell_info[cell].row_spn; i++)
	  {
	    value = (((row_specs[row + i].height * 100) / total) * adjustValue)
				/ 100;
	    row_specs[row + i].height += value;
	    totalUsed                 += value;
	  }

	/*
	 * if didn't use all the size up - apply it evenly.
	 */
	if (totalUsed < adjustValue)
	  {
	    adjustValue = adjustValue - totalUsed;
	    for (i = 0, j = cell_info[cell].row_spn;
		adjustValue > 0 && i < cell_info[cell].row_spn; i++, j--)
	      {
		value = adjustValue / j + (adjustValue % j ? 1 : 0);
		row_specs[row + i].height += value;
		adjustValue               -= value;
		totalUsed                 += value;
	      }
	  }

	total = totalUsed;
      }
}

/******************************************************************************
 * Function: void ReFormatCell(
 *
 * Parameters:
 *		canvas		Specifies the specific information about the
 *				rendering area.
 *		layout		Specifies the currently active information
 *				affecting the layout of information.
 *
 * Purpose: Based on a height and width, relay out the information in a cell.
 *****************************************************************************/
static void
ReFormatCell(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    CellInfo		*this_cell,
    ColumnSpec		*col_specs,
    int			 col,
    _DtCvUnit		 new_height,
    _DtCvUnit		 new_y)
{
    int         i;
    int         saveTxt   = canvas->txt_cnt;
    int         saveLn    = canvas->line_cnt;
    _DtCvUnit   saveYpos  = layout->info.y_pos;
    _DtCvUnit   saveMaxX  = layout->info.cur_max_x;
    _DtCvUnit	cellWidth = 0;
    _DtCvUnit	junk;
    _DtCvValue	junkValue;
    DataPoint	basePt;

    /*
     * reset the y_pos to the correct placement.
     * reset the line counts to the original values.
     * since we aren't changing the width, the number
     * of lines used will not change.
     */
    layout->info.y_pos = new_y;
    canvas->txt_cnt    = this_cell->info.cnt.beg_txt;
    canvas->line_cnt   = this_cell->info.cnt.beg_ln;

    /*
     * determine the maximum width for the cell.
     */
    for (i = this_cell->col_spn; i > 0; i--)
	cellWidth += col_specs[col++].actual;

    /*
     * get the current left and right margins.
     */
    GetCurrentDataPoint(layout, &basePt);

    /*
     * re-format the cell
     */
    FormatCell(canvas, layout, this_cell->cell_seg, cellWidth,
				new_height,
				basePt, &i, &cellWidth, &junk, &junkValue);

    /*
     * calculate the new height.
     */
    this_cell->info.height = layout->info.y_pos - new_y;

    /*
     * if the new cell does not use the same number of lines as the
     * old formatting did, zero the length.
     */
    while (canvas->txt_cnt < this_cell->info.cnt.end_txt)
	canvas->txt_lst[canvas->txt_cnt++].length = 0;

    /*
     * restore the saved counters
     */
    canvas->txt_cnt        = saveTxt;
    canvas->line_cnt       = saveLn;
    layout->info.y_pos     = saveYpos;
    layout->info.cur_max_x = saveMaxX;
}


/******************************************************************************
 * Function: void FormatCell(
 *
 * Parameters:
 *		canvas		Specifies the specific information about the
 *				rendering area.
 *		layout		Specifies the currently active information
 *				affecting the layout of information.
 *		span_width	Specifies the desired size constraining
 *				the layout of information.
 *		base_pt		Specifies the base margins.
 *		this_cell	Specifies the cell structure to fill out.
 *
 * Purpose: Determines the width and height of the cell. Also the begin/end
 *          counts on text, graphics and lines.
 *****************************************************************************/
static void
FormatCell(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*cell_seg,
    _DtCvUnit		 span_width,
    _DtCvUnit		 min_height,
    DataPoint		 base_pt,
    int			*ret_ln,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_height,
    _DtCvValue		*ret_tab_flag)
{
    _DtCvStatus only1Col  = True;
    _DtCvUnit   maxX;
    _DtCvUnit   minY      = -1;
    _DtCvUnit   saveYpos  = layout->info.y_pos;
    _DtCvUnit   saveRight = layout->right;
    _DtCvUnit   saveMaxX  = layout->info.cur_max_x;
    _DtCvSegmentI *nextSeg;

    /*
     * set the limits/margins
     * assume the left_margin has been set by a previous call.
     */
    layout->right = layout->max_width - base_pt.left - base_pt.right
				- layout->left - span_width;
    /*
     * set the minimum Y for the container.
     */
    if (0 < min_height)
	minY = saveYpos + min_height;

    /*
     * If a segment was specified for this cell, format it.
     */
    if (cell_seg != NULL && &BlankTableCell != cell_seg)
      {
        ProcessContainer(canvas, layout, cell_seg, minY,
					ret_width, &maxX, ret_ln);

	/*
	 * check to see if the only thing in this container is a
	 * table.
	 */
	nextSeg = _DtCvContainerListOfSeg(cell_seg);
	*ret_tab_flag = True;
	while (True == *ret_tab_flag && NULL != nextSeg)
	  {
	     /*
	      * check to see if there is any segments that aren't one
	      * column tables.
	      */
	     if (_DtCvIsSegTable(nextSeg) &&
					1 < _DtCvNumColsOfTableSeg(nextSeg))
		only1Col = False;

	     else if (!(_DtCvIsSegTable(nextSeg) || _DtCvIsSegNoop(nextSeg)))
		*ret_tab_flag = False;

	     nextSeg = _DtCvNextSeg(nextSeg);
	  }

	if (True == *ret_tab_flag && True == only1Col)
	    *ret_tab_flag = False;
      }

    /*
     * Calculate the height and return it.
     */
    *ret_height = layout->info.y_pos - saveYpos;

    /*
     * restore the right margin
     */
    layout->right          = saveRight;

    if (layout->info.cur_max_x < saveMaxX)
        layout->info.cur_max_x = saveMaxX;
}

/******************************************************************************
 * Function: AdjustFrmtTxtOption
 *
 *****************************************************************************/
static void
AdjustFrmtTxtOption(
    _DtCvSegmentI	*p_seg,
    _DtCvFrmtOption	 option)
{
    if (p_seg != NULL && _DtCvIsSegContainer(p_seg))
      {
	TxtHorizJustify(p_seg) = option;

	p_seg = _DtCvContainerListOfSeg(p_seg);
	while (p_seg != NULL)
	  {
	    AdjustFrmtTxtOption(p_seg, option);
	    p_seg = p_seg->next_seg;
	  }
      }
}

/******************************************************************************
 * Function: ResolveCell
 *
 *****************************************************************************/
static _DtCvValue
ResolveCell(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*table,
    ColumnSpec		*col_specs,
    RowSpec		*row_specs,
    int			 col,
    int			 row,
    int			 max_cols,
    int			 max_rows,
    CellInfo		*ret_info)
{
    register int i;
    char  *id;
    char  *idRefs;
    char  *ptr;
    char   c;
    int    count;
    int    len;
    int    done;
    int    brdCnt;
    int    myCol = col;
    int	   cell  = row * max_cols + col;
    _DtCvUnit   cellWidth;
    _DtCvUnit   retWidth;
    _DtCvUnit   retHeight;
    _DtCvUnit   saveTop;
    CellInfo	*thisCell = &ret_info[cell];
    DataPoint	 basePt;
    _DtCvValue	 reformat = False;
    _DtCvValue	 retTabFlag = False;
    _DtCvSegmentI **f_data = _DtCvCellsOfTableSeg(table);

    /*
     * if this column is spanned, skip
     */
    if (thisCell->col_spn == -1 || thisCell->row_spn == -1)
	return False;

    GetCurrentDataPoint(layout, &basePt);

    if (thisCell->cell_seg == NULL)
      {
	idRefs = row_specs[row].next_id;

        /*
         * find the end of the id
         */
        done      = False;

        /*
         * set the starting info
         */
        thisCell->col_spn = 1;
        thisCell->pos_x   = basePt.left + layout->left;

        while (!done && col < max_cols)
	  {
            ptr       = idRefs;
            id        = idRefs;

            /*
             * move the ptr to the next id,
	     * counting the characters in this id at the same time.
             */
	    len = 0;
            while (NULL != ptr && *ptr != ' ' && *ptr != '\0')
	      {
                ptr++;
		len++;
	      }

	    /*
	     * set idRefs to the next id.
	     */
            idRefs = ptr;
            while (NULL != idRefs && *idRefs == ' ')
                idRefs++;

            /*
             * Is this id and the next the same? If so,
             * it spans the columns
             */
            if (0 != len && _DtCvStrNCaseCmpLatin1(id, idRefs, len) == 0 &&
				(idRefs[len] == ' ' || idRefs[len] == '\0'))
              {
                col++;
                thisCell->col_spn++;
		ret_info[++cell].col_spn = -1;
              }
            else
                done = True;
          }

	row_specs[row].next_id = idRefs;

	/*
	 * find the segment
	 */
	if (NULL != id && '\0' != *id)
	  {
	    c       = *ptr;
            *ptr    = '\0';
	    while (f_data != NULL && NULL != *f_data &&
	        _DtCvStrCaseCmpLatin1(_DtCvContainerIdOfSeg(*f_data),id) != 0)
	        f_data++;
	    /*
	     * make sure to break the link to the next segment.
	     * Otherwise, the formatting routines will format too much
	     * for the cell.
	     */
	    if (NULL != f_data && NULL != *f_data)
	      {
	        _DtCvNextSeg(*f_data) = NULL;

		/*
		 * assign the data to this cell.
		 */
		thisCell->cell_seg = *f_data;
	      }
	    else /* there is no id for this cell, use a blank container */
	        thisCell->cell_seg = (struct _dtCvSegment*) &BlankTableCell;
	  }
	else /* there is no id for this cell, use a blank container */
	    thisCell->cell_seg = (struct _dtCvSegment*) &BlankTableCell;

	/*
	 * how many rows does this cell span?
	 */
	len = 0;
	if (NULL != id)
	    len = strlen(id);

	for (done = False, count = 1, i = row + 1;
				0 < len && i < max_rows && False == done; i++)
	  {
	    done = True;
	    if (_DtCvStrNCaseCmpLatin1(id, row_specs[i].next_id, len) == 0 &&
		(row_specs[i].next_id[len] == ' ' ||
					row_specs[i].next_id[len] == '\0'))
	      {
		int    k;

		done = False;
		count++;

		/*
		 * invalidate the columns spanned in this row
		 */
		for (k = 0; k < thisCell->col_spn && k + myCol < max_cols; k++)
		  {
		    ret_info[i * max_cols + myCol + k].col_spn = -1;
		    ret_info[i * max_cols + myCol + k].row_spn = -1;
		  }
		
		idRefs  = row_specs[i].next_id;
		do
		  {
		    /*
		     * skip the current id.
		     */
		    while (*idRefs != ' ' && *idRefs != '\0')
			idRefs++;

		    /*
		     * skip the space to the next id.
		     */
		    while (*idRefs == ' ')
			idRefs++;

		    /*
		     * now test to see if this is a match
		     * cycle if so. quit if the end of string
		     * or not a match.
		     */
		  } while (*idRefs != '\0' &&
			_DtCvStrNCaseCmpLatin1(id, idRefs, len) == 0 &&
				(idRefs[len] == ' ' || idRefs[len] == '\0'));

		/*
		 * the next non-spanned column in this row will use this id.
		 */
		row_specs[i].next_id = idRefs;
	      }
	  }

	if (NULL != id && '\0' != *id)
	    *ptr = c;

	thisCell->row_spn = count;
      }

    for (i = 0, cellWidth = 0; i < thisCell->col_spn; i++)
	cellWidth += col_specs[myCol + i].actual;

    /*
     * set the start line and text information
     */
    SetBeginCounts(canvas, &(thisCell->info.cnt));

    /*
     * check to see if this cell is overhung by a previous cell.
     * If so, zero the top margin. But remember and restore it
     * because resizing may eliminate the need for the overhang!
     */
    if (&BlankTableCell != thisCell->cell_seg)
      {
	saveTop = _DtCvContainerTMarginOfSeg(thisCell->cell_seg);
	if (_DtCvTRUE == col_specs[myCol].hanger
		&& 0 != row_specs[row].y_adj && myCol > row_specs[row].column)
	    _DtCvContainerTMarginOfSeg(thisCell->cell_seg) = 0;
      }

    /*
     * Format the cell
     */
    FormatCell(canvas, layout, thisCell->cell_seg, cellWidth,
			row_specs[row].height, basePt,
				&brdCnt, &retWidth, &retHeight, &retTabFlag);

    if (&BlankTableCell != thisCell->cell_seg)
	_DtCvContainerTMarginOfSeg(thisCell->cell_seg) = saveTop;

    /*
     * set some ending information
     */
    thisCell->info.height = retHeight;
    thisCell->info.width  = retWidth;
    SetEndCounts(canvas, &(thisCell->info.cnt), brdCnt);

    /*
     * check the height against previous heights
     */
    if (row_specs[row].height < retHeight && thisCell->row_spn == 1)
	row_specs[row].height = retHeight;

    /*
     * check for run over of the desired widths
     */
    if (retWidth > cellWidth)
      {
	register int j;
	_DtCvUnit  cellMax;
	_DtCvUnit  maxSlop;
	_DtCvUnit  value;
	_DtCvUnit  percent;
	_DtCvUnit  slopUsed = 0;
	_DtCvUnit  slop   = retWidth - cellWidth; /* the amount of room
						   required of the neighbors */

	/*
	 * set the reformat flag
	 */
	if (False == retTabFlag)
	    reformat = True;

	/*
	 * determine the maximum size the column can occupy
	 */
	for (j = 0, cellMax = 0;
			j < thisCell->col_spn && j + myCol < max_cols; j++)
	    cellMax += col_specs[myCol+j].max;
	
	/*
	 * determine the maximum available space from the neighbors.
	 */
	for (j = myCol + thisCell->col_spn, maxSlop = 0; j < max_cols; j++)
	    maxSlop = maxSlop + col_specs[j].actual - col_specs[j].min;

	/*
	 * If the slop demanded is larger than available,
	 * simply reduced the other column specifications to their smallest
	 * values.
	 */
	if (slop >= maxSlop)
	  {
	    for (j = myCol + thisCell->col_spn; j < max_cols; j++)
	        col_specs[j].actual = col_specs[j].min;

	    /*
	     * Is it allowed for this column to 'hang over' the others?
	     *
	     * And is it the first one? I.e. don't allow more than one
	     * cell per row to overhang it's neighbors.
	     *
	     * And only allow it if the vertical text justification places
	     * the text at the top of the cell.
	     */
	    if (_DtCvTRUE == col_specs[myCol].hanger
		&& 0 == row_specs[row].y_adj
		&& _DtCvJUSTIFY_TOP == TxtVertJustify(thisCell->cell_seg))
	      {
		/*
		 * now set the other columns to start a little lower
		 * in their objects. Remove the bottom margin from the
		 * adjustment. One would hope that each of the containers
		 * for the columns in the row have the same bottom margin
		 * so that the overhung cell will push the next row down
		 * by the appropriate amount.
		 */
		row_specs[row].y_adj  = retHeight -
				_DtCvContainerBMarginOfSeg(thisCell->cell_seg);
		row_specs[row].column = myCol;
		retWidth = cellMax;

		/*
		 * clear the reformat flag
		 */
		reformat = False;
	      }
	  }
	else if (slop > 0)
	  {
	    /*
	     * the maximum slop available from the neighbors is
	     * enough. Now take space from my neighbors based
	     * on their orginal size. I.e. the larger they
	     * are, the more I take from them.
	     */
	    for (j = myCol + thisCell->col_spn; j < max_cols; j++)
	      {
	        percent   = col_specs[j].actual - col_specs[j].min;
	        value     = slop * percent / maxSlop;
	        slopUsed += value;
	        col_specs[j].actual -= value;
	      }

	    /*
	     * if any more slop is needed, grab it on a strictly
	     * straight basis.
	     */
	    do {
	        slop -= slopUsed;
	        for (j = myCol + thisCell->col_spn, slopUsed = 0;
					slop > slopUsed && j < max_cols; j++)
	          {
	            if (col_specs[j].actual > col_specs[j].min)
	              {
	                col_specs[j].actual--;
	                slopUsed++;
	              }
	          }
	      } while (slopUsed > 0 && slop > slopUsed);
	  }

	/*
	 * set the column width in the controlling column struct.
	 */
	if (thisCell->col_spn == 1)
	  {
	    col_specs[myCol].actual = retWidth;
	  }
	else
	  {
	    /*
	     * how much to spread among the columns?
	     */
	    slop = cellMax - retWidth;

	    /*
	     * if the aggragate max width is smaller than required,
	     * allocate the excess among the columns.
	     */
	    if (slop < 0)
	      {
		/*
		 * set the desired to the max and calculate the leftover
		 * slop and the maximum desired size.
		 */
		for (j = 0, slop = retWidth, maxSlop = 0;
			myCol + j < max_cols && j < thisCell->col_spn; j++)
		  {
		    col_specs[j].actual = col_specs[myCol + j].max;
		    slop    -= col_specs[myCol + j].max;
		    maxSlop += col_specs[myCol + j].max;
		  }
		/*
		 * now allocate the leftover slop to each colum
		 * based on the maximum desired size.
		 */
		for (j = 0, slopUsed = 0;
			slop > slopUsed && myCol + j < max_cols
						&& j < thisCell->col_spn; j++)
		  {
		    value = slop * col_specs[myCol + j].max / maxSlop;
		    if (((slop*col_specs[myCol+j].max) % maxSlop) >= (maxSlop/2))
			value++;
		    col_specs[myCol + j].actual += value;
		    slopUsed += value;
		  }
	      }
	    else if (slop > 0)
	      {
	        slopUsed = 0;
	        for (j = myCol;
			j < max_cols && j < myCol + thisCell->col_spn; j++)
	          {
	            percent   = col_specs[j].max - col_specs[j].actual;
	            value     = slop * percent / maxSlop;
	            slopUsed += value;
	            col_specs[j].actual += value;
	          }

	        do {
	            slop -= slopUsed;
	            for (j = myCol, slopUsed = 0; slop > slopUsed &&
			j < max_cols && j < myCol + thisCell->col_spn; j++)
	              {
	                if (col_specs[j].actual < col_specs[j].max)
	                  {
	                    col_specs[j].actual++;
	                    slopUsed++;
	                  }
	              }
	          } while (slopUsed > 0 && slop > slopUsed);
	      }
	    else /* if (slop == 0) */
	      {
		for (j = 0; myCol + j < max_cols && j < thisCell->col_spn; j++)
		    col_specs[j].actual = col_specs[myCol + j].max;
	      }
	  }
      }

    return reformat;
}

/******************************************************************************
 * Function: ProcessTable
 *
 *****************************************************************************/
static void
ProcessTable(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*table,
    _DtCvUnit		 min_y)
{
    int		   a;
    int		   b;
    int		   c;
    int		   col;
    int		   row;
    int		   cell;
    int		   divisor;
    int		   maxCols;
    int		   maxRows;
    int		   maxRowSpn;
    int		   saveLnStart  = canvas->line_cnt;
    int		   saveTxtStart = canvas->txt_cnt;
    int		   saveTravCnt  = canvas->trav_cnt;
    _DtCvUnit	   workWidth;
    _DtCvUnit	   newLeft;
    _DtCvUnit	   saveLeft    = layout->left;
    _DtCvUnit	   saveYpos    = layout->info.y_pos;
    _DtCvUnit	   tableYpos;
    _DtCvUnit	   newHeight;
    _DtCvUnit	   newWidth;
    _DtCvUnit	   oldAlignPos;
    short	   anchorRow = -1;

    const char	 **widthStr;
    const char	  *saveAlignChar = layout->info.align_char;
    char	 **rowIds;
    char	  *alignCharacters = NULL;
    char	   alignBuf[16];

    _DtCvFrmtOption   saveTxtJustify = layout->txt_justify;
    _DtCvFrmtOption   colJustify  = _DtCvJUSTIFY_LEFT;
    _DtCvFrmtOption  *colJustSpec;

    _DtCvValue	   oldFound  = layout->id_found;
    _DtCvValue	   haveBrds  = False;
    _DtCvValue	   saveState = layout->table_flag;
    _DtCvValue	   saveAlignFlag = layout->info.align_flag;
    _DtCvValue	   saveAlignPos  = layout->info.align_pos;
    _DtCvValue	   redo;

    CellInfo	 defCell;
    CellInfo	*cellInfo = &defCell;
    ColumnSpec	 defColumn;
    ColumnSpec	*colSpecs = &defColumn;
    RowSpec	 defRow;
    RowSpec	*rowSpecs = &defRow;
    DataPoint	 basePt;
    GrpInfo	 grpInfo = DefGrpInfo;

    /*
     * get the base margins that the table will be working in.
     */
    GetCurrentDataPoint(layout, &basePt);

    /*
     * find out how many rows there are.
     */
    for (rowIds = _DtCvCellIdsOfTableSeg(table), maxRows = 0;
			rowIds != NULL && rowIds[maxRows] != NULL;  maxRows++);

    if (maxRows == 0)
	return;

    /*
     * get the number of columns and the column widths
     */
    maxCols     = _DtCvNumColsOfTableSeg(table);
    widthStr    = (const char **)_DtCvColWOfTableSeg(table);
    colJustSpec = _DtCvColJustifyOfTableSeg(table);

    if (maxCols < 1)
	maxCols = 1;

    if (widthStr == NULL)
	widthStr = DefWidth;

    /*
     * determine the width the table has to work with.
     */
    workWidth = layout->max_width - basePt.left - basePt.right -
						layout->left - layout->right;
    if (workWidth < 0)
	workWidth = 0;

    /*
     * turn the string specifying column widths into units.
     */
    if (maxCols != 1)
      {
        colSpecs = (ColumnSpec *) malloc (sizeof(ColumnSpec) * maxCols);
	if (colSpecs == NULL)
	    return;
      }
    if (maxRows != 1)
      {
        rowSpecs = (RowSpec *) malloc (sizeof(RowSpec) * maxRows);
	if (rowSpecs == NULL)
	  {
	    if (maxCols > 1)
	        free(colSpecs);
	    return;
	  }
      }
    if (maxRows != 1 || maxCols != 1)
      {
        cellInfo = (CellInfo *) malloc (sizeof(CellInfo) * maxCols * maxRows);
	if (cellInfo == NULL)
	  {
	    if (maxCols > 1)
	        free(colSpecs);
	    if (maxRows > 1)
	        free(rowSpecs);
	    return;
	  }
      }

    /*
     * for each column, process the width specification.
     *    '+Optimal,Take,Give'
     *
     *     +       - means the cell can 'hang over' its neighbors.
     *               It will take everything it can get and then
     *               push the other below it. (Labeled lists).
     *     Optimal - The desired percentage of the available space to
     *               use for the column.
     *     Take    - The percentage amount the column will take from
     *               other columns to make itself 'fit'.
     *     Give    - The percentage amount the column is willing to give up
     *               to other columns for them to 'fit'.
     */
    for (col = 0, divisor = 0; col < maxCols; col++)
      {
	const char  *nxtWidth = *widthStr;

	/*
	 * move to the meat of the width specification string.
	 */
	SkipToNumber(&nxtWidth);

	/*
	 * set the correct 'allow hangers' flag.
	 */
	colSpecs[col].hanger  = _DtCvFALSE;
	if ('+' == *nxtWidth)
	  {
	    colSpecs[col].hanger = _DtCvTRUE;
	    nxtWidth++;
	  }

	/*
	 * now process the O,G,T specification.
	 */
	a = GetValueFromString(&nxtWidth, 1); if (a < 1) a = 1;
	b = GetValueFromString(&nxtWidth, 0); if (b < 0) b = 0;
	c = GetValueFromString(&nxtWidth, b); if (c > a) c = a;

	/*
	 * for now just get the base percentages.
	 */
	colSpecs[col].min     = a - c;
	colSpecs[col].actual  = a;
	colSpecs[col].max     = a + b;

	/*
	 * get the column justification.
	 */
	if (NULL != colJustSpec)
	  {
	    colJustify = *colJustSpec;
	    colJustSpec++;
	  }
	colSpecs[col].justify = colJustify;
	if (_DtCvINHERIT == colJustify)
	    colSpecs[col].justify = saveTxtJustify;

	/*
	 * up the divisor value.
	 */
	divisor += colSpecs[col].actual;

	/*
	 * skip to the next set of width specifications
	 */
	if (col + 1 < maxCols && NULL != widthStr[1])
	    widthStr++;

	/*
	 * initialize the cell information for the rows in this column
	 */
	for (row = 0; row < maxRows; row++)
	  {
	    cellInfo[row * maxCols + col].cell_seg = NULL;
	    cellInfo[row * maxCols + col].col_spn  = 0;
	    cellInfo[row * maxCols + col].row_spn  = 0;
	    cellInfo[row * maxCols + col].info     = DefLayFrmtInfo;
	  }
      }

    /*
     * initialize the row specs
     */
    newHeight = 0;
    if (-1 != min_y && 1 == maxRows)
        newHeight = min_y - saveYpos;
    for (row = 0; row < maxRows; row++)
      {
	rowSpecs[row].column  = -1;
	rowSpecs[row].y_adj   = 0;
	rowSpecs[row].height  = newHeight;
	rowSpecs[row].height  = newHeight;
	rowSpecs[row].next_id = rowIds[row];
      }

    /*
     * now figure the real values
     */
    if (divisor < 1)
	divisor = 1;
    for (col = 0; col < maxCols; col++)
      {
	colSpecs[col].min    = workWidth * colSpecs[col].min    / divisor;
	colSpecs[col].actual = workWidth * colSpecs[col].actual / divisor;
	colSpecs[col].max    = workWidth * colSpecs[col].max    / divisor;

	if (colSpecs[col].min < 1)
	    colSpecs[col].min = 1;
	if (colSpecs[col].actual < 1)
	    colSpecs[col].actual = 1;
	if (colSpecs[col].max < 1)
	    colSpecs[col].max = 1;
      }

    /*
     * now process the table.
     */
    tableYpos = layout->info.y_pos;
    maxRowSpn = 1;
    alignCharacters = _DtCvJustifyCharsOfTableSeg(table);

    /*
     * set up the state of table processing and
     * the beginning line/text counts.
     */
    layout->table_flag = True;
    SetBeginCounts(canvas, &(grpInfo.cnt));

    /*
     * now process each column, row by row.
     * Doing it row by row allows the columns to shake out their
     * sizing with less reformatting.
     */
    for (col = 0; col < maxCols; col++)
      {
	/*
	 * remember where this column starts.
	 */
	saveTravCnt  = canvas->trav_cnt;
        saveLnStart  = canvas->line_cnt;
        saveTxtStart = canvas->txt_cnt;
        newLeft      = layout->left;
	colJustify   = layout->txt_justify;
	layout->txt_justify = colSpecs[col].justify;

	/*
	 * initialize the JUSTIFY_NUM or JUSTIFY_CHAR information
	 */
	layout->info.align_pos  = 0;
	layout->info.align_char = PeriodStr;
	if (_DtCvJUSTIFY_CHAR == layout->txt_justify)
	  {
	    /*
	     * are any alignment characters specified?
	     */
	    if (NULL != alignCharacters && '\0' != alignCharacters)
	      {
		int len = mblen(alignCharacters, MB_CUR_MAX);

		/*
		 * copy the character into a buffer
		 */
		strncpy(alignBuf, alignCharacters, len);
		alignBuf[len] = '\0';
		layout->info.align_char = alignBuf;

		/*
		 * are there more characters? If so increment for the
		 * next column that may have JUSTIFY_CHAR. Otherwise,
		 * leave alone and re-use for other columns.
		 */
		if ('\0' != alignCharacters[len])
		    alignCharacters += len;
	      }

	    /* no...then default */
	    else
		layout->txt_justify = _DtCvJUSTIFY_LEFT;
	  }

        do {
	    /*
	     * reset the counts to the start
	     */
	    canvas->trav_cnt = saveTravCnt;
	    canvas->line_cnt = saveLnStart;
	    canvas->txt_cnt  = saveTxtStart;
	    layout->left     = newLeft;

	    /*
	     * for each row, format the cell in this columns
	     */
	    for (row = 0, redo = False, cell = col;
				row < maxRows && redo == False;
						row++, cell += maxCols)
	      {
		/*
		 * remember the old height
		 */
		rowSpecs[row].lst_height = rowSpecs[row].height;

		/*
		 * set the alignment flag for each column in the row.
		 */
		layout->info.align_flag = False;
		if (_DtCvJUSTIFY_CHAR == layout->txt_justify ||
					_DtCvJUSTIFY_NUM == layout->txt_justify)
		    layout->info.align_flag = True;

		/*
		 * remember the alignment position
		 */
		oldAlignPos = layout->info.align_pos;

		/*
		 * layout all the cells jammed to the top of the table.
		 * later, they get moved down to their position.
		 */
	        layout->info.y_pos = tableYpos;
	        redo = ResolveCell(canvas, layout, table,
				    colSpecs, rowSpecs, col, row,
				    maxCols, maxRows, cellInfo);
		/*
		 * check for maximum row span
		 */
		if (maxRowSpn < cellInfo[cell].row_spn)
		    maxRowSpn = cellInfo[cell].row_spn;

		/*
		 * did the cell have borders?
		 */
		if (0 != cellInfo[cell].info.cnt.my_lines)
		    haveBrds = True;

	        /*
	         * check to see if the specified anchor has been found in
	         * this row. If so, save some information for later use.
	         */
	        if (anchorRow == -1 && oldFound != layout->id_found)
		    anchorRow = row;

		/*
		 * check to see if the alignment position has changed.
		 * for all but the first row!
		 */
		if (0 != row && oldAlignPos != layout->info.align_pos)
		    redo = True;
	      }

	    if (True == redo)
	      {
		for (a = 0; a < row; a++)
		  {
		    /*
		     * restore the old row heights for this column
		     */
		    rowSpecs[a].height = rowSpecs[a].lst_height;

		    /*
		     * reset the hanging cell information.
		     */
		    if (col == rowSpecs[a].column)
		      {
		        rowSpecs[a].column = -1;
		        rowSpecs[a].y_adj  = 0;
		      }
		  }
	      }

          } while (redo == True);
	
	/*
	 * push the next column to the right by size of this column
	 */
	layout->left += colSpecs[col].actual;

	/*
	 * restore the horizontal text justification
	 */
	layout->txt_justify = colJustify;
      }

    /*
     * set the ending counts for the lines/text in the table.
     * and save the information as long as we are not a table
     * that is a descendant of some container with a border and
     * our cells include borders and we're going to have to honor
     * boundaries.
     */
    SetEndCounts(canvas, &(grpInfo.cnt), 0);
    if (_DtCvUSE_BOUNDARY_MOVE == canvas->constraint &&
				False == layout->brdr_flag && True == haveBrds)
      {
	GrpInfo *info = (GrpInfo *) malloc (sizeof(GrpInfo));

	/*
	 * warning - nothing done if malloc error.
	 */
	if (NULL != info)
	  {
	    /*
	     * initialize to the line counts for the table
	     */
	    *info = grpInfo;

	    /*
	     * set the linked list information
	     */
	    info->next_info = layout->grp_lst;
	    layout->grp_lst = info;
	  }
      }

    /*
     * Now go back and search for zeroed row heights and fill them in
     * based on spanned rows. This can only happen if maxRowSpn is
     * greater than 1! Otherwise, a row height really did end up zero!
     */
    if (maxRowSpn > 1)
      {
	/*
	 * try to resolve the zero height rows
	 */
	for (a = 1, redo = True; redo && a < maxRowSpn; a++)
	  {
	    redo = False;
	    for (row = 0; row < maxRows; row++)
	      {
	        if (rowSpecs[row].height == 0 &&
		    ResolveHeight(rowSpecs, cellInfo, maxCols, row, a) == 0)
			redo = True;
	      }
	  }
	
	/*
	 * if any of the rows comes up unresolved, force to an average
	 * line height.
         *
         * But only do this if the first cell *does not* span all the rows
         * and columns.
	 */
	if (redo &&
		cellInfo[0].row_spn != maxRows && cellInfo[0].col_spn != maxCols)
	  {
	    for (row = 0; row < maxRows; row++)
	      {
	        if (rowSpecs[row].height == 0)
		    rowSpecs[row].height = canvas->metrics.line_height;
	      }
	  }

        /*
         * Now, double check that the row heights will accomodate
	 * all the cells.
         */
        for (row = 0; row < maxRows; row++)
	    for (col = 0; col < maxCols; col++)
	        AdjustHeight(cellInfo, rowSpecs, maxCols, row, col);
      }

    /*
     * now check that the minimum heights used for the rows matches
     * or exceeds the minimum y position required.
     */
    if (-1 != min_y)
      {
	_DtCvUnit  pad;

	for (newHeight = 0, row = 0; row < maxRows; row++)
	    newHeight += rowSpecs[row].height;

	if (tableYpos + newHeight < min_y)
	  {
	    newHeight = tableYpos - min_y;
	    for (row = 0; 0 < newHeight && row < maxRows; row++)
	      {
		pad                   = (newHeight/(maxRows-row));
		rowSpecs[row].height += pad;
		newHeight            -= pad;
	      }
	  }
      }

    /*
     * now reposition the cells based on the final row heights.
     */
    layout->info.y_pos = tableYpos;
    for (tableYpos = 0, cell = 0, row = 0; row < maxRows;
	tableYpos = tableYpos + rowSpecs[row].height + rowSpecs[row].y_adj,
	row++)
      {
	/*
	 * check to see if the specified anchor has been found in this
	 * row. If so, adjust the found position.
	 */
	if (anchorRow == row)
	    layout->id_Ypos += tableYpos;

	for (col = 0, layout->left = saveLeft; col < maxCols;
				layout->left += colSpecs[col++].actual, cell++)
	  {
	    if (cellInfo[cell].cell_seg != NULL)
	      {
		/*
		 * calculate the new height
		 */
		for (newHeight = 0, a = 0; a < cellInfo[cell].row_spn; a++)
		  {
		    newHeight += rowSpecs[row + a].height;
		    if (col != rowSpecs[row + a].column)
			newHeight += rowSpecs[row + a].y_adj;
		  }

		/*
		 * calculate the new width.
		 */
		for (newWidth = 0, a = 0; a < cellInfo[cell].col_spn; a++)
		    newWidth += colSpecs[col + a].actual;

		/*
		 * now get the overhang space for this cell
		 */
		workWidth = 0;
		if (col > rowSpecs[row].column)
		    workWidth = rowSpecs[row].y_adj;

		/*
		 * if the heights (and/or width for spanning columns)
		 * are different, check to see if the
		 * cell contains lines that may be affected by the
		 * height adjustment.
		 *
		 * It is strongly assumed that if a table specifies that
		 * a cell can hang over it's neighbors that it will *NOT*
		 * have borders (whereby LinesMayChange is false) and
		 * newWidth will not be greater than the cell's width.
		 */
		if ((newHeight > cellInfo[cell].info.height &&
			True == LinesMayChange(canvas,
					cellInfo[cell].info.cnt.beg_ln,
					cellInfo[cell].info.cnt.end_ln,
					cellInfo[cell].info.cnt.my_lines))
				||
			(1 < cellInfo[cell].col_spn &&
				newWidth > cellInfo[cell].info.width))
		    ReFormatCell(canvas, layout, &cellInfo[cell], colSpecs,
					col, newHeight,
					layout->info.y_pos + tableYpos);

		else
		    /* adjust the cell rather than reformatting */
	            AdjustObjectPosition(canvas, layout,
				TxtVertJustify(cellInfo[cell].cell_seg),
				cellInfo[cell].info.cnt.beg_txt ,
				cellInfo[cell].info.cnt.beg_ln  ,
				cellInfo[cell].info.cnt.beg_brk ,
				cellInfo[cell].info.cnt.end_txt ,
				cellInfo[cell].info.cnt.end_ln  ,
				cellInfo[cell].info.cnt.end_brk ,
				cellInfo[cell].info.cnt.my_lines,
				newHeight - cellInfo[cell].info.height,
				0,
				tableYpos, workWidth);
	      }
	  }
      }

    /*
     * increment the maximum y.
     */
    layout->info.y_pos += tableYpos;
    layout->left        = saveLeft;

    if (maxCols > 1)
        free(colSpecs);
    if (maxRows > 1)
        free(rowSpecs);
    if (maxRows > 1 || maxCols > 1)
        free(cellInfo);

    layout->txt_justify = saveTxtJustify;
    layout->table_flag  = saveState;

    /*
     * restore the alignment information
     */
    layout->info.align_flag = saveAlignFlag;
    layout->info.align_char = saveAlignChar;
    layout->info.align_pos  = saveAlignPos;
}

/******************************************************************************
 * Function: UpdateDimensionArrays
 *
 * Purpose: Based on the object's orientation and justification,
 *	    update the correct dimension array(s).
 *
 *****************************************************************************/
static void
UpdateDimensionArrays(
    _DtCvSegmentI *p_seg,
    _DtCvUnit	 width,
    _DtCvUnit	 height,
    TopDims	*top_bot,
    SideDims	*side,
    CornerDims	*corner,
    FlowDims	*flow,
    _DtCvUnit	*max_left,
    _DtCvUnit	*max_right)
{
    int		    i;
    int		    j;
    int		   *marginPtr;
    _DtCvFrmtOption orient   = ObjHorizOrient(p_seg);
    _DtCvFrmtOption vOrient  = ObjVertOrient(p_seg);

    /*
     * modify the width that headSize should be
     */
    j = DIMS_LM;
    i = DIMS_TOP;
    switch (orient)
      {
	case _DtCvJUSTIFY_RIGHT_MARGIN:
		j++;
	case _DtCvJUSTIFY_CENTER:
		j++;
	case _DtCvJUSTIFY_LEFT_MARGIN:
		if (vOrient == _DtCvJUSTIFY_BOTTOM)
		    i = DIMS_BOTTOM;

		if ((*top_bot)[i][j][DIMS_WIDTH] < width)
		    (*top_bot)[i][j][DIMS_WIDTH] = width;

		if (_DtCvWRAP_JOIN != _DtCvContainerFlowOfSeg(p_seg))
		    (*top_bot)[i][j][DIMS_HEIGHT] += height;
		break;

      }

    /*
     * check left & right margins.
     */
    marginPtr = max_left;
    j = DIMS_LEFT;
    i = DIMS_TOP;
    switch(orient)
      {

	case _DtCvJUSTIFY_RIGHT_CORNER:
	case _DtCvJUSTIFY_RIGHT:
		j = DIMS_RIGHT;
		marginPtr = max_right;

	case _DtCvJUSTIFY_LEFT_CORNER:
	case _DtCvJUSTIFY_LEFT:
		if (vOrient != _DtCvJUSTIFY_TOP)
		    i++;
		if (vOrient == _DtCvJUSTIFY_BOTTOM)
		    i++;

		/*
		 * push i to zero or 4
		 */
		if (orient == _DtCvJUSTIFY_RIGHT_CORNER ||
					    orient == _DtCvJUSTIFY_LEFT_CORNER)
		  {
		    if (i) i = DIMS_BC;
		    (*corner)[i][j] += height;
		    if (*marginPtr < width)
		        *marginPtr = width;
		  }
		else if (_DtCvContainerFlowOfSeg(p_seg) != _DtCvWRAP)
		  {
		    (*side)[i][j] += height;
		    if (*marginPtr < width)
		        *marginPtr = width;
		  }
		else
		  {
		    (*flow)[j][DIMS_HEIGHT] += height;
		    if ((*flow)[j][DIMS_WIDTH] < width)
			(*flow)[j][DIMS_WIDTH] = width;
		  }
		break;
      }
}

/******************************************************************************
 * Function: DetermineMaxDims
 *
 *****************************************************************************/
static void
DetermineMaxDims(
    TopDims	*top_bot,
    CornerDims	*corner,
    _DtCvUnit	 left_margin,
    _DtCvUnit	 right_margin,
    _DtCvUnit	*top_height,
    _DtCvUnit	*bot_height,
    _DtCvUnit	*max_width)
{
    register int	j;
    _DtCvUnit     topWidth;
    _DtCvUnit     botWidth;

    /*
     * now process all the information gathered about the (sub)headings
     * to determine the bounding box for the (head) txt. Start by figuring
     * out the maximums for the dimensions.
     *
     * figure the current top and bottom max widths.
     */
    topWidth  = left_margin + right_margin;
    botWidth  = left_margin + right_margin;
    *top_height = 0;
    *bot_height = 0;
    for (j = DIMS_LM; j <= DIMS_RM; j++)
      {
	topWidth = topWidth + (*top_bot)[DIMS_TOP]   [j][DIMS_WIDTH];
	botWidth = botWidth + (*top_bot)[DIMS_BOTTOM][j][DIMS_WIDTH];

	if (*top_height < (*top_bot)[DIMS_TOP][j][DIMS_HEIGHT])
	    *top_height = (*top_bot)[DIMS_TOP][j][DIMS_HEIGHT];

	if (*bot_height < (*top_bot)[DIMS_BOTTOM][j][DIMS_HEIGHT])
	    *bot_height = (*top_bot)[DIMS_BOTTOM][j][DIMS_HEIGHT];
      }

    /*
     * for the maximum top and bottom heights, take into
     * consideration the corner values
     */
    if (*top_height < (*corner)[DIMS_TC][DIMS_LEFT])
	*top_height = (*corner)[DIMS_TC][DIMS_LEFT];
    if (*top_height < (*corner)[DIMS_TC][DIMS_RIGHT])
	*top_height = (*corner)[DIMS_TC][DIMS_RIGHT];

    if (*bot_height < (*corner)[DIMS_BC][DIMS_LEFT])
	*bot_height = (*corner)[DIMS_BC][DIMS_LEFT];
    if (*bot_height < (*corner)[DIMS_BC][DIMS_RIGHT])
	*bot_height = (*corner)[DIMS_BC][DIMS_RIGHT];

    *max_width = topWidth;
    if (*max_width < botWidth)
	*max_width = botWidth;

}

/******************************************************************************
 * Function: DetermineFlowConstraints
 *
 *****************************************************************************/
static void
DetermineFlowConstraints(
    LayoutInfo	*layout,
    FlowDims	 flow_dims,
    _DtCvUnit	 left_margin,
    _DtCvUnit	 right_margin,
    _DtCvUnit	 start_y,
    DataPoint	*left_pt,
    DataPoint	*right_pt)
{
    _DtCvUnit	leftSide  = flow_dims[DIMS_LEFT][DIMS_HEIGHT];
    _DtCvUnit	rightSide = flow_dims[DIMS_RIGHT][DIMS_HEIGHT];

    /*
     * Now, if there is flowing text required, put points on the
     * stack to indicate them.
     */
    left_margin  += flow_dims[DIMS_LEFT][DIMS_WIDTH];
    right_margin += flow_dims[DIMS_RIGHT][DIMS_WIDTH];

    GetCurrentDataPoint(layout, left_pt);
    GetCurrentDataPoint(layout, right_pt);
    left_pt->left   += left_margin;
    left_pt->right  += right_margin;
    left_pt->y_pos   = _CEFORMAT_ALL;
    right_pt->left  += left_margin;
    right_pt->right += right_margin;
    right_pt->y_pos  = _CEFORMAT_ALL;

    while (leftSide > 0 || rightSide > 0)
      {
	if (leftSide > 0)
	  {
	    if (rightSide == 0 || leftSide <= rightSide)
	      {
		left_pt->right = right_margin;
		left_pt->y_pos = start_y + leftSide;
		if (leftSide != rightSide)
		    left_margin = 0;
		leftSide = 0;
		InsertDataPoint(layout, left_pt);
	      }
	  }

	if (rightSide > 0)
	  {
	    if (leftSide == 0 || leftSide > rightSide)
	      {
		right_pt->left  = left_margin;
		right_pt->y_pos = start_y + rightSide;
		if (leftSide != rightSide)
		    right_margin = 0;
		rightSide = 0;
		InsertDataPoint(layout, right_pt);
	      }
	  }
      }
}

/******************************************************************************
 * Function: DetermineHeadPositioning
 *
 *****************************************************************************/
static void
DetermineHeadPositioning(
    TopDims	*top_bot,
    SideDims	*side,
    CornerDims	*corner,
    FlowDims	*flow,
    _DtCvUnit	 start_y,
    _DtCvUnit	 max_top,
    _DtCvUnit	 block_size,
    _DtCvUnit	*ret_side_size)
{
    int    i;
    _DtCvUnit   leftSideHeight  = 0;
    _DtCvUnit   rightSideHeight = 0;
    _DtCvUnit   sideHeight      = 0;

    /*
     * determine the maximum side heights
     */
    for (i = DIMS_TOP; i <= DIMS_BOTTOM; i++)
      {
	leftSideHeight  += (*side)[i][DIMS_LEFT];
	rightSideHeight += (*side)[i][DIMS_RIGHT];
      }

    /*
     * determine the maximum side height
     */
    sideHeight = block_size;
    if (sideHeight < leftSideHeight)
	sideHeight = leftSideHeight;
    if (sideHeight < rightSideHeight)
	sideHeight = rightSideHeight;
    if (sideHeight < (*flow)[DIMS_LEFT][DIMS_HEIGHT])
        sideHeight  = (*flow)[DIMS_LEFT][DIMS_HEIGHT];
    if (sideHeight < (*flow)[DIMS_RIGHT][DIMS_HEIGHT])
        sideHeight = (*flow)[DIMS_RIGHT][DIMS_HEIGHT];

    /*
     * calculate the starting Y position for each of the positions
     * reuse the arrays that were used to save the max dimension values.
     */
    for (i = DIMS_LM; i <= DIMS_RM; i++)
      {
	(*top_bot)[DIMS_TOP]   [i][DIMS_YPOS] = start_y;
	(*top_bot)[DIMS_BOTTOM][i][DIMS_YPOS] = start_y + max_top + sideHeight;
      }

    (*corner)[DIMS_TC][DIMS_LEFT]  = start_y;
    (*corner)[DIMS_TC][DIMS_RIGHT] = start_y;

    (*corner)[DIMS_BC][DIMS_LEFT]  = start_y + max_top + sideHeight;
    (*corner)[DIMS_BC][DIMS_RIGHT] = start_y + max_top + sideHeight;

    (*side)[DIMS_TOP][DIMS_LEFT]  = start_y + max_top;
    (*side)[DIMS_TOP][DIMS_RIGHT] = start_y + max_top;

    (*flow)[DIMS_LEFT ][DIMS_YPOS] = start_y + max_top;
    (*flow)[DIMS_RIGHT][DIMS_YPOS] = start_y + max_top;

    (*side)[DIMS_CENTER][DIMS_LEFT]  = start_y + max_top +
			(sideHeight - (*side)[DIMS_CENTER][DIMS_LEFT]) / 2;
    (*side)[DIMS_CENTER][DIMS_RIGHT] = start_y + max_top +
			(sideHeight - (*side)[DIMS_CENTER][DIMS_RIGHT]) / 2;

    (*side)[DIMS_BOTTOM][DIMS_LEFT]  = start_y + max_top +
			sideHeight - (*side)[DIMS_BOTTOM][DIMS_LEFT];
    (*side)[DIMS_BOTTOM][DIMS_RIGHT] = start_y + max_top +
			sideHeight - (*side)[DIMS_BOTTOM][DIMS_RIGHT];

    if (ret_side_size != NULL)
	*ret_side_size = sideHeight;
}

/******************************************************************************
 * Function: AdjustHead
 *
 * Parameters:
 *		base_left	Specifies the x position that the controller
 *				occupying 'left_margin' space would start
 *				at.
 *		block_width	Specifies the body's width for which to
 *				center or align a controller with.
 *		left_margin	Specifies the space used by a controller
 *				that is on the left side of the body.
 *		right_margin	Specifies the space used by a controller
 *				on the right side of the body.
 *
 *****************************************************************************/
static void
AdjustHeadPosition(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*p_seg,
    TopDims		*top_bot,
    SideDims		*side,
    CornerDims		*corner,
    FlowDims		*flow,
    LayFrmtInfo		*info,
    _DtCvUnit		 base_y,
    _DtCvUnit		 base_left,
    _DtCvUnit		 block_width,
    _DtCvUnit		 left_margin,
    _DtCvUnit		 right_margin)
{
    int		  i         = DIMS_TOP;		/* also DIMS_TC */
    int		  j         = DIMS_LEFT;	/* also DIMS_LM */
    int		  divisor   = 2;
    _DtCvUnit	  adjustX   = 0;
    _DtCvUnit	  adjustY   = 0;
    _DtCvUnit	  newY      = 0;
    _DtCvUnit	  headWidth = info->width;
    _DtCvFrmtOption  orient    = ObjHorizOrient(p_seg);
    _DtCvFrmtOption  vOrient   = ObjVertOrient(p_seg);

    if (_DtCvContainerPercentOfSeg(p_seg) == 10000
			&& orient == _DtCvJUSTIFY_CENTER
			&& TxtHorizJustify(p_seg) == _DtCvJUSTIFY_LEFT)
	headWidth = block_width;

    switch (orient)
      {
	case _DtCvJUSTIFY_RIGHT_MARGIN:
		divisor = 1;
		j++;
	case _DtCvJUSTIFY_CENTER:
		adjustX = (block_width - headWidth) / divisor;
		j++;
	case _DtCvJUSTIFY_LEFT_MARGIN:
		adjustX += left_margin;
		if (vOrient == _DtCvJUSTIFY_BOTTOM)
		    i = DIMS_BOTTOM;

		newY  = (*top_bot)[i][j][DIMS_YPOS];
		(*top_bot)[i][j][DIMS_YPOS] += info->height;
		break;

	case _DtCvJUSTIFY_RIGHT_CORNER:
		adjustX = block_width + left_margin;
		j       = DIMS_RIGHT;

	case _DtCvJUSTIFY_LEFT_CORNER:
		if (vOrient == _DtCvJUSTIFY_BOTTOM)
		    i = DIMS_BC;

		newY = (*corner)[i][j];
		(*corner)[i][j] += info->height;
		break;

	case _DtCvJUSTIFY_RIGHT:
		adjustX = block_width + left_margin;
		j = DIMS_RIGHT;

	case _DtCvJUSTIFY_LEFT:
		if (vOrient != _DtCvJUSTIFY_TOP)
		    i++;
		if (vOrient == _DtCvJUSTIFY_BOTTOM)
		    i++;

		if (_DtCvContainerFlowOfSeg(p_seg) == _DtCvWRAP)
		  {
		    if (orient == _DtCvJUSTIFY_LEFT)
			adjustX += left_margin;
		    else
			adjustX -= headWidth;

		    newY = (*flow)[j][DIMS_YPOS];
		    (*flow)[j][DIMS_YPOS] += info->height;
		  }
		else
		  {
		    newY = (*side)[i][j];
		    (*side)[i][j] += info->height;
		  }
		break;
      }

    adjustY  = newY - base_y;
    adjustX += base_left;

    /*
     * adjust the text positions
     */
    AdjustTextPositions(canvas, info->cnt.beg_txt, info->cnt.end_txt,
								adjustX, adjustY);

    /*
     * adjust the lines positions
     */
    AdjustLinePositions(canvas, info->cnt.beg_ln, info->cnt.end_ln,
								adjustX, adjustY);

    /*
     * adjust the page breaks, but only if necessary.
     */
    AdjustPgBrk(canvas, info->cnt.beg_brk, info->cnt.end_brk, adjustY);
}

/******************************************************************************
 * Function: InitDimArrays
 *
 *****************************************************************************/
static void
InitDimArrays(
    TopDims		*top_bot,
    SideDims		*side,
    CornerDims		*corner,
    FlowDims		*flow)
{
    int		    i;
    int		    j;

    for (i = DIMS_TOP; i <= DIMS_BOTTOM; i++)
      {
	for (j = DIMS_LM; j <= DIMS_RM; j++)
	  {
	    (*top_bot)[i][j][DIMS_WIDTH] = 0;
	    (*top_bot)[i][j][DIMS_HEIGHT] = 0;
	  }
	for (j = DIMS_LEFT; j <= DIMS_RIGHT; j++)
	    (*side)[i][j] = 0;
      }
    for (i = DIMS_LEFT; i <= DIMS_RIGHT; i++)
      {
	for (j = DIMS_WIDTH; j <= DIMS_HEIGHT; j++)
	 {
	    (*corner)[i][j] = 0;
	    (*flow)[i][j]   = 0;
	 }
      }
}

/******************************************************************************
 * Function: ProcessController
 *****************************************************************************/
static LayFrmtInfo *
ProcessController(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*cur_seg)
{
    int		   getLn;
    int		   saveTravCnt  = canvas->trav_cnt;
    _DtCvUnit	   saveYPos     = layout->info.y_pos;
    _DtCvUnit	   saveMaxWidth = layout->max_width;
    _DtCvUnit	   saveLeft     = layout->left;
    _DtCvUnit	   saveRight    = layout->right;
    _DtCvUnit	   maxWidth;
    _DtCvUnit	   maxXPos;
    _DtCvUnit	   myMaxWidth;
    LayFrmtInfo   *frmtInfo;
    DataPoint	   basePt;
    DataPoint	   zeroPt;
    _DtCvValue	   redo;

    /*
     * Controllers always break the formatting sequence.
     * So save any information in the buffer, reset the margins and
     * add the appropriate lines, and check for going over boundaries.
     */
    CheckSaveInfo(canvas, layout, cur_seg, 0);
    CheckFormat(layout, True);

    /*
     * Get the controller specific information.
     * disallow some of the orientation & vOrient combinations
     */
    if ((ObjHorizOrient(cur_seg) == _DtCvJUSTIFY_CENTER
			&& ObjVertOrient(cur_seg) != _DtCvJUSTIFY_BOTTOM)
		||
	(ObjVertOrient(cur_seg) == _DtCvJUSTIFY_CENTER
			&& ObjHorizOrient(cur_seg) != _DtCvJUSTIFY_LEFT
			&& ObjHorizOrient(cur_seg) != _DtCvJUSTIFY_RIGHT))
	ObjVertOrient(cur_seg) = _DtCvJUSTIFY_TOP;

    if (_DtCvContainerFlowOfSeg(cur_seg) == _DtCvWRAP
			&&
	    (ObjVertOrient(cur_seg) != _DtCvJUSTIFY_TOP
				||
		(ObjVertOrient(cur_seg) == _DtCvJUSTIFY_TOP
			&& ObjHorizOrient(cur_seg) != _DtCvJUSTIFY_LEFT
			&& ObjHorizOrient(cur_seg) != _DtCvJUSTIFY_RIGHT)))
	_DtCvContainerFlowOfSeg(cur_seg) = _DtCvWRAP_NONE;

    if (_DtCvContainerFlowOfSeg(cur_seg) == _DtCvWRAP_JOIN
			&&
	    (ObjVertOrient(cur_seg) != _DtCvJUSTIFY_TOP
		|| ObjHorizOrient(cur_seg) != _DtCvJUSTIFY_LEFT_MARGIN))
      {
	ObjVertOrient(cur_seg)  = _DtCvJUSTIFY_TOP;
	ObjHorizOrient(cur_seg) = _DtCvJUSTIFY_LEFT_MARGIN;
      }
    
    /*
     * malloc a formatting dimension structure and initialize it with
     * default values. This will be returned to the caller.
     */
    frmtInfo  = (LayFrmtInfo *) malloc (sizeof(LayFrmtInfo));
    *frmtInfo = DefLayFrmtInfo;

    /*
     * the controller object begins here.
     */
    SetBeginCounts(canvas, &(frmtInfo->cnt));

    /*
     * set the parent's data point in the stack
     */
    GetCurrentDataPoint(layout, &basePt);

    /*
     * calculate the amount of space the controller can occupy.
     * first calculate the amount of space to work with.
     * then truncate to zero if necessary.
     * then use the percentage of that remaining area as the space
     *      the controller's segments can occupy.
     */
    myMaxWidth = layout->max_width - basePt.left - basePt.right
							- saveLeft - saveRight;
    if (myMaxWidth < 0)
	myMaxWidth = 0;

    myMaxWidth = (_DtCvUnit) (((double) myMaxWidth)
			* ((double) _DtCvContainerPercentOfSeg(cur_seg))
			/ HeadDivisor);


    /*
     * Format the controller at a 'zero'ed point.
     * The lines it generates will be moved later to their correct position.
     */
    layout->left  = 0;
    layout->right = 0;
    zeroPt = DefDataPt;
    PushDataPoint(layout, &zeroPt);

    /*
     * now process as a regular container
     */
    do {
	/*
	 * set some counts and flags (necessary for a redo).
	 */
	redo = False;
	canvas->trav_cnt    = saveTravCnt;
	canvas->line_cnt    = frmtInfo->cnt.beg_ln;
	canvas->txt_cnt     = frmtInfo->cnt.beg_txt;
        layout->max_width   = myMaxWidth;
        layout->info.y_pos  = 0;

	/*
	 * process the container
	 */
        ProcessContainer(canvas,layout,cur_seg,-1,&maxWidth,&maxXPos,&getLn);

	/*
	 * check to see if we need to reformat because the minimum size
	 * is larger than we asked for.
	 */
	if (maxXPos + _DtCvContainerRMarginOfSeg(cur_seg) > myMaxWidth)
	  {
	    redo       = True;
	    myMaxWidth = maxXPos + _DtCvContainerRMarginOfSeg(cur_seg);
	  }
      } while (True == redo);
    
    /*
     * remove this element's data points from the stack.
     */
    RemoveDataPoint(layout, &zeroPt);

    /*
     * set the ending counts for the items in this container.
     */
    SetEndCounts(canvas, &(frmtInfo->cnt), getLn);
    frmtInfo->width  = myMaxWidth;
    frmtInfo->height = layout->info.y_pos;

    /*
     * does this controller want to join with the lines in a non-controller?
     */
    if (_DtCvWRAP_JOIN == _DtCvContainerFlowOfSeg(cur_seg)
				&& frmtInfo->cnt.beg_txt != canvas->txt_cnt)
	_DtCvSetJoinInfo(&(layout->info), True, canvas->txt_cnt - 1);

    /*
     * Restore the previous information
     */
    if (NULL != layout->lst_rendered)
	layout->lst_rendered->next_disp = NULL;

    layout->left           = saveLeft;
    layout->right          = saveRight;
    layout->max_width      = saveMaxWidth;
    layout->lst_rendered   = NULL;
    layout->info.y_pos     = saveYPos;

    return frmtInfo;
}

/******************************************************************************
 * Function: AdjustForBorders
 *
 * Initializes the display line and graphic tables.
 *****************************************************************************/
static void
AdjustForBorders(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvFrmtOption	 brdr,
    _DtCvUnit		 line_width,
    _DtCvUnit		*ret_bot,
    _DtCvUnit		*ret_right)
{
    /*
     * if the line_width is zero, make it 1 so that is really takes
     * up some space.
     */
    if (0 == line_width)
	line_width = 1;

    /*
     * set the flag for processing a border
     */
    if (_DtCvBORDER_NONE != brdr)
	layout->brdr_flag = True;

    /*
     * check to see if this element has a border. If so, adjust the
     * boundaries.
     */
    if (brdr == _DtCvBORDER_FULL || brdr == _DtCvBORDER_HORZ
				 || brdr == _DtCvBORDER_TOP
				 || brdr == _DtCvBORDER_TOP_LEFT
				 || brdr == _DtCvBORDER_TOP_RIGHT)
        layout->info.y_pos += line_width;

    *ret_bot = 0;
    if (brdr == _DtCvBORDER_FULL || brdr == _DtCvBORDER_HORZ
				 || brdr == _DtCvBORDER_BOTTOM
				 || brdr == _DtCvBORDER_BOTTOM_LEFT
				 || brdr == _DtCvBORDER_BOTTOM_RIGHT)
        *ret_bot = line_width;

    if (brdr == _DtCvBORDER_FULL || brdr == _DtCvBORDER_VERT
				 || brdr == _DtCvBORDER_LEFT
				 || brdr == _DtCvBORDER_TOP_LEFT
				 || brdr == _DtCvBORDER_BOTTOM_LEFT)
        layout->left += line_width;

    *ret_right = 0;
    if (brdr == _DtCvBORDER_FULL || brdr == _DtCvBORDER_VERT
				 || brdr == _DtCvBORDER_RIGHT
				 || brdr == _DtCvBORDER_TOP_RIGHT
				 || brdr == _DtCvBORDER_BOTTOM_RIGHT)
      {
        layout->right += line_width;
	*ret_right     = line_width;
      }
}

/******************************************************************************
 * Function: DrawBorders
 *
 * Parameters:
 *		canvas		Specifies the virtual canvas on which
 *				lines are drawn.
 *		layout		Specifies the current layout information.
 *		brdr		Specifies the type of border.
 *		top_y		Specifes the top y of the bounding box for
 *				the object. Any border drawn should be
 *				completely below this y.
 *		bot_y		Specifes the bottom y of the bounding box
 *				for the object. Any border drawn should be
 *				completely below this y.
 *		left_x		Specifies the left x of the bounding box
 *				for the object. Any border drawn should be
 *				completely to the right of this x.
 *		right_x		Specifies the right x of the bounding box
 *				for the object. Any border drawn should be
 *				completely to the left of this x.
 *
 *          left_x               right_x
 *            |                     |
 *            v                     v
 * top_y ---> xxxxxxxxxxxxxxxxxxxxxx
 *            xxxxxxxxxxxxxxxxxxxxxx
 *            xx------------------xx
 *            xx|                |xx  (xx represents the line.)
 *            xx|                |xx
 *            xx|                |xx
 *            xx|                |xx
 *            xx------------------xx
 * bot_y ---> xxxxxxxxxxxxxxxxxxxxxx
 *            xxxxxxxxxxxxxxxxxxxxxx
 *
 *****************************************************************************/
static int
DrawBorders(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvFrmtOption	 brdr,
    _DtCvPointer	 data,
    _DtCvUnit		 line_width,
    _DtCvUnit		 top_y,
    _DtCvUnit		 bot_y,
    _DtCvUnit		 left_x,
    _DtCvUnit		 right_x)
{
    int         mod       = 1;
    int		cnt       = canvas->line_cnt;
    _DtCvUnit	width;

    /*
     * if line_width is zero, make it 1 so that it really takes
     * up some space.
     */
    if (0 == line_width)
	line_width = 1;

    /*
     * calculate the width of the element.
     */
    width = right_x - left_x;

    /*
     * If borders are specified, draw them
     */
    if (brdr != _DtCvBORDER_NONE)
      {
	/*
	 * now do the horizontal borders. the coordinates are the top,
	 * left most unit of the line.
	 */
	switch(brdr)
	  {
	    case _DtCvBORDER_FULL:
	    case _DtCvBORDER_HORZ:
	    case _DtCvBORDER_BOTTOM:
	    case _DtCvBORDER_BOTTOM_LEFT:
	    case _DtCvBORDER_BOTTOM_RIGHT:
			SaveLine(canvas, layout, _DtCvLINE_HORZ,
						data, line_width,
						left_x, bot_y, width);
			mod = -1;
			if (brdr == _DtCvBORDER_BOTTOM
					|| brdr == _DtCvBORDER_BOTTOM_LEFT
					|| brdr == _DtCvBORDER_BOTTOM_RIGHT)
			    break;
    
	    case _DtCvBORDER_TOP:
	    case _DtCvBORDER_TOP_LEFT:
	    case _DtCvBORDER_TOP_RIGHT:
			SaveLine(canvas, layout, _DtCvLINE_HORZ,
						data, line_width,
						left_x, top_y, width);
	  }
    
	/*
	 * for vertical lines, the coordinates are the top, right most
	 * unit of the line.
	 */
	switch(brdr)
	  {
	    case _DtCvBORDER_FULL:
	    case _DtCvBORDER_BOTTOM_LEFT:
			/*
			 * include the line width in length for a full
			 * border.
			 */
			bot_y += line_width;

	    case _DtCvBORDER_VERT:
	    case _DtCvBORDER_LEFT:
	    case _DtCvBORDER_TOP_LEFT:
			SaveLine(canvas, layout, _DtCvLINE_VERT,
						data, line_width,
						left_x, top_y, bot_y - top_y);
			if (brdr == _DtCvBORDER_LEFT
					|| brdr == _DtCvBORDER_TOP_LEFT
					|| brdr == _DtCvBORDER_BOTTOM_LEFT)
			    break;
    
	    case _DtCvBORDER_BOTTOM_RIGHT:
			/*
			 * if we didn't fall thru from above, we need to
			 * add the extension to the bottom to get the
			 * full length for the right vertical line.
			 */
			if (brdr == _DtCvBORDER_BOTTOM_RIGHT)
			    bot_y += line_width;

	    case _DtCvBORDER_RIGHT:
	    case _DtCvBORDER_TOP_RIGHT:
			SaveLine(canvas, layout, _DtCvLINE_VERT,
						data, line_width,
				right_x - line_width, top_y, bot_y - top_y);
	  }
      }

    return ((canvas->line_cnt - cnt) * mod);
}

/******************************************************************************
 * Function: AdjustObjectPosition
 *
 * Parameters:
 *		canvas		Specifies the virtual canvas on which
 *				lines are drawn.
 *		justify		Specifies the vertical adjustment for the
 *				object.
 *		start_txt	Specifies the start index of the text list.
 *		start_gr	Specifies the start index of the graphics list.
 *		start_ln	Specifies the start index of the line list.
 *		end_txt		Specifies the end indext of the text list.
 *		end_gr		Specifies the end indext of the graphics list.
 *		end_ln		Specifies the end indext of the line list.
 *		height_adj	Specifies the internal height adjust value.
 *				Depending on the justify type, this may
 *				add to y_adj for text and regions, bottom
 *				lines and the height of vertical lines.
 *		y_adj		Specifies the y position adjustment.
 *				Lines, text and regions are moved this
 *				amount.
 *		internal_y	Specifies the internal y position adjustment.
 *				height_adj includes this value. Text and
 *				regions are moved this amount.
 *
 * Return:	nothing.
 *
 *****************************************************************************/
static void
AdjustObjectPosition(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvFrmtOption	 justify,
    int			 start_txt,
    int			 start_ln,
    int			 start_brk,
    int			 end_txt,
    int			 end_ln,
    int			 end_brk,
    int			 brdr_cnt,
    _DtCvUnit		 height_adj,
    _DtCvUnit		 x_adj,
    _DtCvUnit		 y_adj,
    _DtCvUnit		 internal_y)
{
    int   mod  = 1;
    _DtCvUnit  yOff = 0;

    /*
     * If border count is negative, indicates the first line in the
     * list is bottom line. This requires special handling in adjusting
     * its position. Set flags accordingly.
     */
    if (brdr_cnt < 0)
      {
	mod = -1;
	brdr_cnt = -brdr_cnt;
      }

    /*
     * calculate the offset value within the object for other objects
     * contained in this object.
     */
    if (justify != _DtCvJUSTIFY_TOP)
      {
	yOff = height_adj - internal_y;
        if (justify == _DtCvJUSTIFY_CENTER)
            yOff /= 2;
      }
    
    yOff += y_adj;
    yOff += internal_y;

    /*
     * don't modify the border lines around this object yet.
     */
    end_ln -= brdr_cnt;

    /*
     * modify the border lines of the objects contained within
     * this object.
     */
    AdjustLinePositions(canvas, start_ln, end_ln, x_adj, yOff);

    /*
     * now adjust the border lines around this object.
     */
    start_ln = end_ln;
    end_ln += brdr_cnt;
    AdjustLinePositions(canvas, start_ln, end_ln, x_adj, y_adj);

    /*
     * now fix the lines if they've changed height and move
     * the first line to its bottom position if necessary.
     */
    if (0 != height_adj)
      {
        while (start_ln < end_ln)
          {
	    /*
	     * indicates the bottom line is the first line in the
	     * list. Move it down the height adjustment.
	     */
	    if (mod < 0)
	      {
                canvas->line_lst[start_ln].pos_y += height_adj;
                canvas->line_lst[start_ln].max_y += height_adj;

		mod = 1;
	      }
	    /*
	     * stretch the vertical lines
	     */
	    else if (canvas->line_lst[start_ln].dir == _DtCvLINE_VERT)
                canvas->line_lst[start_ln].max_y += height_adj;

            start_ln++;
          }
      }

    /*
     * adjust the position of the text within this object.
     */
    AdjustTextPositions (canvas, start_txt, end_txt, x_adj, yOff);

    /*
     * adjust the position of the text within this object.
     */
    AdjustPgBrk (canvas, start_brk, end_brk, yOff);
}

/******************************************************************************
 * Function: LinesMayChange
 *
 * Parameters:
 *		canvas		Specifies the virtual canvas on which
 *				lines are drawn.
 *		start_ln	Specifies the start index of the line list.
 *		end_ln		Specifies the end indext of the line list.
 *
 * Return:	True	if there is a vertical line as a child of a container.
 *		False	if there are no vertical lines in the child of a
 *			container/cell.
 *
 *****************************************************************************/
static _DtCvValue
LinesMayChange(
    _DtCanvasStruct	*canvas,
    int			 start_ln,
    int			 end_ln,
    int			 brdr_cnt)
{
    /*
     * If border count is negative, indicates that one of the lines
     * is for the bottom of the container/cell. Ignore for now.
     * we want to check the lines in the container/cell.
     */
    if (brdr_cnt < 0)
	brdr_cnt = -brdr_cnt;

    /*
     * get rid of the line count for this object,
     * AdjustObjectPosition can take care of it.
     */
    end_ln -= brdr_cnt;

    /*
     * Now check for vertical lines that would
     * be affected by a height change.
     */
    while (start_ln < end_ln)
      {
	if (_DtCvLINE_VERT == canvas->line_lst[start_ln].dir)
	    return True;
        start_ln++;
      }

    return False;
}

/******************************************************************************
 * Function: ProcessContainer
 *
 * Initializes the display line and graphic tables.
 *****************************************************************************/
static void
ProcessContainer(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*con_seg,
    _DtCvUnit		 min_y,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_max_x,
    int			*ret_cnt)
{
    int		   getLn;
    const char	  *saveJustifyChar = layout->info.align_char;
    _DtCvUnit	   yPad         = 0;
    _DtCvUnit	   xPad         = 0;
    _DtCvUnit	   maxWidth     = 0;
    _DtCvUnit	   maxXPos      = 0;
    _DtCvUnit	   myMinY       = -1;
    _DtCvUnit	   saveLeft     = layout->left;
    _DtCvUnit	   saveRight    = layout->right;
    _DtCvUnit	   saveLead     = layout->info.leading;
    _DtCvUnit	   saveFirst    = layout->first;
    _DtCvUnit	   saveYpos     = layout->info.y_pos;
    _DtCvValue	   saveStatic   = layout->stat_flag;
    _DtCvValue	   saveBrdr     = layout->brdr_flag;
    _DtCvFrmtOption   saveJustify  = layout->txt_justify;
    LayFrmtInfo    frmtInfo;
    DataPoint	   basePt;
    DataPoint	   curPt;

    /*
     * check to see if this element breaks the formatting sequence.
     * If so save any information in the buffer, reset the margins and
     * add the appropriate lines, and check for going over boundaries.
     */
    if (NotJoining(layout))
      {
	CheckSaveInfo(canvas, layout, con_seg, 0);
	CheckFormat(layout, True);
      }

    /*
     * check to see if this segment is the segment we want as our first
     * visible line.
     */
    CheckId(layout, _DtCvContainerIdOfSeg(con_seg));

    /*
     * Set beginning text and line counts
     */
    frmtInfo = DefLayFrmtInfo;
    SetBeginCounts(canvas, &(frmtInfo.cnt));

    /*
     * Get the first indent and set the current container pointer to me.
     */
    layout->first = _DtCvContainerFMarginOfSeg(con_seg) / layout->divisor;
    layout->left  = _DtCvContainerLMarginOfSeg(con_seg) / layout->divisor;
    layout->right = _DtCvContainerRMarginOfSeg(con_seg) / layout->divisor;
    layout->info.leading = _DtCvContainerLeadingOfSeg(con_seg);

    /*
     * check to see if we violate the horiz_pad_hint on the left, right or
     * first margins.
     */
    if (canvas->metrics.horiz_pad_hint > _DtCvContainerLMarginOfSeg(con_seg))
	layout->left = _DtCvContainerLMarginOfSeg(con_seg);
    else if (layout->left < canvas->metrics.horiz_pad_hint)
	layout->left = canvas->metrics.horiz_pad_hint;

    if (canvas->metrics.horiz_pad_hint > _DtCvContainerRMarginOfSeg(con_seg))
	layout->right = _DtCvContainerRMarginOfSeg(con_seg);
    else if (layout->right < canvas->metrics.horiz_pad_hint)
	layout->right = canvas->metrics.horiz_pad_hint;

    if (canvas->metrics.horiz_pad_hint > _DtCvContainerFMarginOfSeg(con_seg))
	layout->first = _DtCvContainerFMarginOfSeg(con_seg);
    else if (layout->first < canvas->metrics.horiz_pad_hint)
	layout->first = canvas->metrics.horiz_pad_hint;

    /*
     * check to see if there is more squeeze room available.
     */
    if (layout->left > canvas->metrics.horiz_pad_hint
			|| layout->right > canvas->metrics.horiz_pad_hint
			|| layout->first > canvas->metrics.horiz_pad_hint)
	layout->margin_non_zero = True;

    /*
     * set the formatting type for this container
     */
    layout->stat_flag = False;
    if (_DtCvContainerTypeOfSeg(con_seg) == _DtCvLITERAL)
	layout->stat_flag = True;

    /*
     * check to see if this element breaks the formatting sequence.
     * If so, add lines, etc.
     */
    if (NotJoining(layout))
      {
        /*
         * Adjust margins and y position for bordering
         */
        AdjustForBorders (canvas, layout, Border(con_seg), BrdWidth(con_seg),
								&yPad, &xPad);

        _DtCvAddSpace(_DtCvContainerTMarginOfSeg(con_seg),
							&(layout->info.y_pos));
	/*
	 * check for flow limits.
	 */
        CheckFormat(layout, True);

        /*
         * get the parent's data point in the stack
         * and add the current container's left and right to it.
         */
        GetCurrentDataPoint(layout, &basePt);
        basePt.left  += saveLeft;
        basePt.right += saveRight;

	/*
	 * if we don't inherit the the text justification
	 * set the new value.
	 */
	if (_DtCvINHERIT != _DtCvContainerJustifyOfSeg(con_seg))
	  {
	    layout->txt_justify  = _DtCvContainerJustifyOfSeg(con_seg);
	    if (_DtCvJUSTIFY_NUM == layout->txt_justify)
		layout->info.align_char = PeriodStr;
	    else if (_DtCvJUSTIFY_CHAR == layout->txt_justify)
	      {
		layout->info.align_char = _DtCvContainerJustifyCharOfSeg(con_seg);
		/*
		 * check to see if the character is 'valid'.
		 * if not, default out of JUSTIFY_CHAR
		 */
		if (NULL != layout->info.align_char ||
					'\0' == layout->info.align_char)
		    layout->txt_justify = _DtCvJUSTIFY_LEFT;
	      }
	  }

	/*
	 * terminate the previous rendering list
	 */
	if (NULL != layout->lst_rendered)
	    layout->lst_rendered->next_disp = NULL;
	layout->lst_rendered = NULL;

	/*
	 * push the data point and reset margin/text info.
	 */
        PushDataPoint(layout, &basePt);
	SetMargins(layout);
	SetTextPosition(layout, True);
      }

    /*
     * determine the minimum Y that the child of this container should
     * occupy.  To do so, subtract off the bottom border pad and the
     * bottom margin.
     */
    if (0 < min_y)
      {
	myMinY = min_y - yPad;
	if (_DtCvWRAP_JOIN != _DtCvContainerFlowOfSeg(con_seg))
	  {
	    _DtCvUnit bPad = 0;

	    _DtCvAddSpace(_DtCvContainerBMarginOfSeg(con_seg), &bPad);
	    myMinY -= bPad;
	  }
      }

    /*
     * format the segment
     */
    saveYpos = layout->info.y_pos;

    GetCurrentDataPoint(layout, &curPt);

    /*
     * reset the max x variable
     */
    layout->info.cur_max_x = 0;
    ProcessSegmentList(canvas, layout, _DtCvContainerListOfSeg(con_seg),
					myMinY,
					&maxWidth, &maxXPos, NULL);
    /*
     * if this container forces a wrap join of the next item,
     * save the current line, but don't add space or null the
     * last rendered item.
     */
    CheckSaveInfo(canvas, layout, con_seg, 0);
    if (maxWidth < layout->info.cur_max_x - curPt.left + layout->right)
        maxWidth = layout->info.cur_max_x - curPt.left + layout->right;
    if (maxXPos < layout->info.cur_max_x)
        maxXPos = layout->info.cur_max_x;

    if (_DtCvWRAP_JOIN != _DtCvContainerFlowOfSeg(con_seg))
      {
        /*
         * Save any information in the buffer,
         * reset the margins and add the appropriate lines,
         * and check for going over boundaries.
         */
        _DtCvAddSpace(_DtCvContainerBMarginOfSeg(con_seg),
							&(layout->info.y_pos));

	/*
	 * terminate the previous rendering list
	 */
	if (NULL != layout->lst_rendered)
	    layout->lst_rendered->next_disp = NULL;
        layout->lst_rendered = NULL;
      }

    /*
     * remove this element's data points from the stack.
     */
    RemoveDataPoint(layout, &basePt);

    /*
     * include the bottom border (if needed) in the ending y position
     */
    layout->info.y_pos += yPad;

    /*
     * Set the ending counts for the lines and text in me.
     * This sets the ending counts for the lines contained in me,
     * NOT the lines in my border.
     */
    SetEndCounts(canvas, &(frmtInfo.cnt), 0);

    /*
     * does this object need to be adjust within its height?
     */
    if (0 < min_y && layout->info.y_pos < min_y)
      {
	AdjustObjectPosition(canvas, layout, TxtVertJustify(con_seg),
			frmtInfo.cnt.beg_txt, frmtInfo.cnt.beg_ln,
			frmtInfo.cnt.beg_brk,
			frmtInfo.cnt.end_txt, frmtInfo.cnt.end_ln,
			frmtInfo.cnt.end_brk,
			0, min_y - layout->info.y_pos - yPad, 0, 0, 0);
	layout->info.y_pos = min_y;
      }

    /*
     * Now draw the borders
     * If borders are drawn, cur_max_x & max_x_pos may get changed
     * if a right side border is drawn.
     */
    if (maxWidth < layout->max_width - curPt.left - curPt.right)
	maxWidth = layout->max_width - curPt.left - curPt.right;

    getLn = DrawBorders (canvas, layout, Border(con_seg),
				BrdData(con_seg), BrdWidth(con_seg),
				saveYpos, layout->info.y_pos - yPad,
				curPt.left,
				curPt.left + maxWidth);

    /*
     * check to see if we need to save the container counts away
     * because we might need to move the entire container as one
     * to honor the boundary. This will occur if the flag to honor
     * a boundary is set to _DtCvUSE_BOUNDARY_MOVE, this container has
     * border lines, the container's parent is not a table nor
     * is the container within another container that has a border.
     */
    if (_DtCvUSE_BOUNDARY_MOVE == canvas->constraint
			&& True == layout->brdr_flag
			&& False == saveBrdr && False == layout->table_flag)
      {
	GrpInfo *info = (GrpInfo *) malloc (sizeof(GrpInfo));

	/*
	 * warning - nothing done if malloc error.
	 */
	if (NULL != info)
	  {
	    /*
	     * initialize to the end information of the container.
	     */
	    info->cnt = frmtInfo.cnt;

	    /*
	     * take into account the borders for this container
	     */
	    SetEndCounts(canvas, &(info->cnt), getLn);

	    /*
	     * set the linked list information
	     */
	    info->next_info = layout->grp_lst;
	    layout->grp_lst = info;
	  }
      }

    /*
     * set the return values.
     */
    *ret_max_x = layout->info.cur_max_x;
    *ret_width = maxWidth;
    if (*ret_width < layout->max_width - curPt.left - curPt.right)
	*ret_width = layout->max_width - curPt.left - curPt.right;

    /*
     * Restore the previous information
     */
    layout->left      = saveLeft;
    layout->right     = saveRight;
    layout->first     = saveFirst;
    layout->stat_flag = saveStatic;
    layout->brdr_flag = saveBrdr;
    layout->info.leading = saveLead;

    /*
     * Besides checking for flow constraints, also (re)sets margins and
     * text information.
     */
    CheckFormat(layout, True);
    layout->txt_justify  = saveJustify;
    layout->info.align_char  = saveJustifyChar;

    /*
     * for tables and such, return how many lines were drawn around this
     * container.
     */
    if (ret_cnt)
	*ret_cnt = getLn;

    return;
}

/******************************************************************************
 * Function: ProcessSegmentList
 *
 * Process the segment list, laying it out according to left, right,
 * and first margins specified. Returns the max_width of the all
 * segments processed and the maximum x coordinate used.
 *****************************************************************************/
static void
ProcessSegmentList(
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvSegmentI	*cur_seg,
    _DtCvUnit		 min_y,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_max_x,
    int			**ret_vert)
{
    int		 junk;
    int		 saveTravCnt  = canvas->trav_cnt;
    int		 saveTxtCnt   = canvas->txt_cnt;
    int		 saveLineCnt  = canvas->line_cnt;
    int		 saveBrkCnt   = canvas->brk_cnt;
    _DtCvUnit	 tempX;
    _DtCvUnit	 tempLen;
    _DtCvUnit	 width;
    _DtCvUnit	 nWidth       = 0;
    _DtCvUnit	 leftMargin   = 0;
    _DtCvUnit	 rightMargin  = 0;
    _DtCvUnit	 topHeight    = 0;
    _DtCvUnit	 botHeight    = 0;
    _DtCvUnit	 maxWidth     = 0;
    _DtCvUnit	 saveYpos     = layout->info.y_pos;
    _DtCvSegmentI	*segStart     = cur_seg;
    LayFrmtInfo		*headInfo     = NULL;
    LayFrmtInfo		*lastHead     = NULL;
    LayFrmtInfo		*nxtHead;
    _DtCvLayoutInfo	 startInfo;

    _DtCvValue	 redo         = False;
    _DtCvValue	 joinCleared  = False;
    _DtCvValue	 flag;

    TopDims	 topBot;
    SideDims	 sideDims;
    CornerDims	 cornerDims;
    FlowDims	 flowDims;
    DataPoint	 basePt;
    DataPoint	 leftPt;
    DataPoint	 rightPt;

    /*
     * clear the controller arrays
     */
    InitDimArrays(&topBot, &sideDims, &cornerDims, &flowDims);

    /*
     * get the current left and right values or 'base'.
     */
    GetCurrentDataPoint(layout, &basePt);

    /*
     * ???
     */
    leftPt  = basePt;
    rightPt = basePt;
    leftPt.y_pos  = _CEFORMAT_ALL;
    rightPt.y_pos = _CEFORMAT_ALL;

    /*
     * process all the controller type containers in the segment list
     */
    while (NULL != cur_seg)
      {
	if (_DtCvIsSegContainer(cur_seg) && _DtCvIsSegController(cur_seg))
	  {
	    /*
	     * want to clear this once and only once. Then any join
	     * directives will survive, though if two or more controllers
	     * have the directive set, the 'last' one will win out.
	     */
	    if (False == joinCleared)
	      {
	        _DtCvSetJoinInfo(&(layout->info), False, -1);
		joinCleared = True;
	      }

	    /*
	     * process the 'controller'
	     */
	    nxtHead = ProcessController(canvas, layout, cur_seg);

	    /*
	     * update the dimension arrays so that the controller
	     * will get placed correctly.
	     */
	    UpdateDimensionArrays(cur_seg, nxtHead->width, nxtHead->height,
				&topBot, &sideDims, &cornerDims, &flowDims,
				&leftMargin, &rightMargin);

	    /*
	     * remember this controller.
	     */
	    if (NULL == headInfo)
		headInfo = nxtHead;
	    else
		lastHead->next_info = nxtHead;
	    lastHead = nxtHead;
	  }

	/*
	 * go to the next segment
	 */
	cur_seg = cur_seg->next_seg;
      }

    /*
     * Now reset the margins based on the controllers found
     */
    if (NULL != headInfo)
      {
	DetermineMaxDims(&topBot, &cornerDims, leftMargin, rightMargin,
			    &topHeight, &botHeight, &maxWidth);
	layout->info.y_pos += topHeight;
	layout->left	   += leftMargin;
	layout->right	   += rightMargin;
	DetermineFlowConstraints(layout, flowDims,
			    basePt.left, basePt.right,
			    layout->info.y_pos, &leftPt, &rightPt);

	/*
	 * get rid of the leftMargin and rightMargin values in maxWidth
	 * otherwise the use of layout->left & layout->right will double
	 * the value.
	 */
	maxWidth = maxWidth - leftMargin - rightMargin;
	if (layout->max_width < maxWidth + basePt.left + basePt.right +
			    layout->left + layout->right)
	    layout->max_width = maxWidth + basePt.left + basePt.right +
					    layout->left + layout->right;

	SetMargins(layout);
	SetTextPosition(layout, True);
	if (JoinSet(layout))
	  {
	    int  cnt;
	    int  joinLine = layout->info.join_line;
	    int  start    = canvas->txt_lst[joinLine].byte_index;
	    int  count    = canvas->txt_lst[joinLine].length;
	    _DtCvSegmentI *pSeg = canvas->txt_lst[joinLine].seg_ptr;
	    _DtCvUnit	  tmpWidth;

	    /*
	     * change the starting location of the following text.
	     * take into account the left margin that *hasn't*
	     * been added to the controlling container.
	     */
	    layout->info.text_x_pos  = canvas->txt_lst[joinLine].text_x
							+ layout->lmargin;
	    layout->info.cur_len = 0;

	    /*
	     * now calculate the width of this line.
	     */
	    while (pSeg != NULL && count > 0)
	      {
		_DtCvGetWidthOfSegment(canvas,pSeg,start,count,
						    &cnt, &tmpWidth, NULL);
		layout->info.text_x_pos += tmpWidth;
		count -= cnt;
		start  = 0;
		pSeg   = pSeg->next_disp;
	      }
	  }
      }

    /*
     * now format for non-controller containers and non-containers.
     * re-start at the beginning.
     */
    cur_seg   = segStart;

    /*
     * Save some information incase we have to redo the layout. I.e.
     * we overflow the sizing.
     */
    startInfo = layout->info;
    saveTravCnt = canvas->trav_cnt;
    saveTxtCnt   = canvas->txt_cnt;
    saveLineCnt  = canvas->line_cnt;
    saveBrkCnt   = canvas->brk_cnt;

    while (NULL != cur_seg)
      {
        width = layout->max_width - layout->info.text_x_pos
				- layout->rmargin - layout->info.cur_len;

	/*
	 * check to see if this item should start a line.
	 */
	CheckSetLineStart(layout, cur_seg);

	/*
	 * check to see if this item will cause a page break.
	 */
	CheckForPageBreak(canvas, cur_seg, layout->info.y_pos);

	switch (_DtCvPrimaryTypeOfSeg(cur_seg))
          {
	    case _DtCvCONTAINER:
		    if (!(_DtCvIsSegController(cur_seg)))
			ProcessContainer(canvas, layout, cur_seg, min_y,
						&junk, &junk, &junk);
		    break;

	    case _DtCvREGION:
		    /*
		     * flag that this segment needs a line number
		     */
		    cur_seg->internal_use = (void *) -1;

		    /*
		     * process the segment
		     */
		    if (_DtCvIsSegInLine(cur_seg))
		      {
			/*
			 * if a hypertext link, this will add it to
			 * the internal list.
			 */
			CheckAddToHyperList(canvas, cur_seg);

			/*
			 * get the traversal width
			 */
			nWidth = _DtCvGetTraversalWidth(canvas, cur_seg,
						    layout->info.lst_hyper);

			/*
			 * check to see if this region can end a line
			 */
			flag = _DtCvCheckLineSyntax(canvas,cur_seg,0,0,False);

			/*
			 * if this can't end a line, get the length up to
			 * the next segment that can and base whether to
			 * save pased on that.
			 */
			tempLen = 0;
			if (False == flag)
			  {
			    tempLen = _DtCvGetNextWidth(canvas,
						_DtCvSTRING,
						layout->info.lst_hyper,
						cur_seg->next_seg,
						0, cur_seg, NULL, NULL, NULL);
			    /*
			     * if the next width is zero, reset the flag.
			     */
			    if (tempLen <= 0)
				flag = True;
			  }
			tempLen += nWidth;

			/*
			 * if not joining, but my length goes over the
			 * working width, save out the current buffered
			 * information
			 */
			if (NotJoining(layout) &&
			    _DtCvWidthOfRegionSeg(cur_seg) + tempLen > width)
			    CheckSaveInfo (canvas, layout, cur_seg, 0);

			/*
			 * up counts on the buffered information.
			 */
			layout->info.line_bytes += 1;
			layout->info.cur_len    += 
				(_DtCvWidthOfRegionSeg(cur_seg) + nWidth);

			/*
			 * does the next segment need to join with
			 * this one? If so, set the information
			 */
			_DtCvSetJoinInfo(&(layout->info), (flag ? 0 : 1), -1);
		        if (_DtCvIsSegNewLine(cur_seg))
			    SaveInfo(canvas, layout, cur_seg, 0);
		      }
		    else
		      {
			/*
			 * clear out the join information
			 * standalone figures can't join with others.
			 */
			_DtCvSetJoinInfo(&(layout->info), False, -1);

			/*
			 * figures are standalone. Save any
			 * information in the buffer away.
			 */
			CheckSaveInfo (canvas, layout, cur_seg, 0);

			/*
			 * check to see if this segment is
			 * a hypertext. If so, add it to the
			 * list if it hasn't been added yet.
			 */
			CheckAddToHyperList(canvas, cur_seg);

			/*
			 * get the traversal width, if any.
			 */
			nWidth = _DtCvGetTraversalWidth(canvas, cur_seg,
						    layout->info.lst_hyper);

			/*
			 * now save the standalone figure
			 */
			layout->info.line_bytes += 1;
			layout->info.cur_len    += 
				_DtCvWidthOfRegionSeg(cur_seg) + nWidth;
			SaveInfo(canvas, layout, cur_seg->next_seg, 0);

			/*
			 * check for wrapping overflow
			 */
			CheckFormat(layout, False);
		      }

		    /*
		     * indicate this segment as the last item rendered
		     */
		    if (NULL != layout->lst_rendered)
		        layout->lst_rendered->next_disp = cur_seg;
		    layout->lst_rendered = cur_seg;
		    break;

	    case _DtCvLINE:
		    /*
		     * lines are standalone. Save any
		     * information in the buffer away.
		     */
		    CheckSaveInfo (canvas, layout, cur_seg, 0);

		    /*
		     * if the line_width is zero, make it 1 so
		     * that it really does take some space.
		     */
		    nWidth = _DtCvWidthOfLineSeg(cur_seg);
		    if (0 == nWidth)
			nWidth = 1;

		    /*
		     * start with it going all the way across the window.
		     */
		    width = layout->max_width;
		    tempX = 0;

		    /*
		     * or does it only extend across the container?
		     */
		    if (_DtCvIsSegBlockLine(cur_seg))
		      {
			tempX = layout->lmargin;
			width = layout->max_width - tempX - layout->rmargin;
		      }

		    SaveLine (canvas, layout, _DtCvLINE_HORZ,
				_DtCvDataOfLineSeg(cur_seg), nWidth, tempX,
				layout->info.y_pos, width);

		    layout->info.y_pos += nWidth;
		    break;

	    case _DtCvMARKER:
		    /*
		     * check to see if marker is the target id
		     */
		    CheckId(layout, _DtCvIdOfMarkerSeg(cur_seg));
		    break;

	    case _DtCvNOOP:
		    if (_DtCvIsSegNewLine(cur_seg))
			SaveInfo(canvas, layout, cur_seg, 0);
		    break;

	    case _DtCvSTRING:
		    /*
		     * flag that this segment needs a line number
		     */
		    cur_seg->internal_use = (void *) -1;

		    /*
		     * process the string
		     */
		    ProcessStringSegment(canvas, layout, cur_seg);

		    /*
		     * check for wrapping overflow.
		     */
		    if (_DtCvIsSegNewLine(cur_seg))
			CheckFormat(layout, False);

		    /*
		     * indicate this segment as the last item rendered
		     */
		    if (NULL != layout->lst_rendered)
		        layout->lst_rendered->next_disp = cur_seg;
		    layout->lst_rendered = cur_seg;
		    break;

	    case _DtCvTABLE:
		    ProcessTable(canvas, layout, cur_seg, min_y);
	    default:
		    break;
	  }

	/*
	 * get the next segment
	 */
	cur_seg = cur_seg->next_seg;

	/*
	 * check the flowing text points
	 */
	if (leftPt.y_pos > 0 &&
		leftPt.x_units > layout->max_width - leftPt.left - leftPt.right)
	  {
	    layout->max_width = leftPt.x_units + leftPt.left + leftPt.right;
	    redo = True;
	  }
	if (rightPt.y_pos > 0 &&
		rightPt.x_units > layout->max_width-rightPt.left-rightPt.right)
	  {
	    layout->max_width = rightPt.x_units + rightPt.left + rightPt.right;
	    redo = True;
	  }

	/*
	 * have we violated the available space?
	 * if so, we'll have to reformat.
	 */
	if (redo == True)
	  {
	    redo             = False;
	    cur_seg          = segStart;
	    canvas->trav_cnt = saveTravCnt;
	    canvas->txt_cnt  = saveTxtCnt;
	    canvas->line_cnt = saveLineCnt;
	    canvas->brk_cnt  = saveBrkCnt;
	    layout->info     = startInfo;

	    if (rightPt.y_pos > 0)
	      {
	        RemoveDataPoint(layout, &rightPt); /* make sure its gone */
	        InsertDataPoint(layout, &rightPt);
	      }
	    if (leftPt.y_pos > 0)
	      {
	        RemoveDataPoint(layout, &leftPt); /* make sure its gone */
	        InsertDataPoint(layout, &leftPt);
	      }

	    /*
	     * Now reset the margins based on the controllers found
	     */
	    if (NULL != headInfo)
	      {
		layout->left   += leftMargin;
		layout->right  += rightMargin;
		SetMargins(layout);
		SetTextPosition(layout, True);
		if (JoinSet(layout))
		  {
		    int  cnt;
		    int  joinLine = layout->info.join_line;
		    int  start    = canvas->txt_lst[joinLine].byte_index;
		    int  count    = canvas->txt_lst[joinLine].length;
		    _DtCvSegmentI *pSeg = canvas->txt_lst[joinLine].seg_ptr;
		    _DtCvUnit	  tmpWidth;
	
		    /*
		     * change the starting location of the following text.
		     * take into account the left margin that *hasn't*
		     * been added to the controlling container.
		     */
		    layout->info.text_x_pos  = canvas->txt_lst[joinLine].text_x
							+ layout->lmargin;
		    layout->info.cur_len = 0;
	
		    /*
		     * now calculate the width of this line.
		     */
		    while (pSeg != NULL && count > 0)
		      {
			_DtCvGetWidthOfSegment(canvas,pSeg,start,count,
							&cnt, &tmpWidth, NULL);
			layout->info.text_x_pos += tmpWidth;
			count -= cnt;
			start  = 0;
			pSeg   = pSeg->next_disp;
		      }
		  }
	      }
	  }
      }

    RemoveDataPoint(layout, &leftPt);
    RemoveDataPoint(layout, &rightPt);

    /*
     * if there were heads, now place them correctly.
     */
    if (NULL != headInfo)
      {
	_DtCvUnit	blockHeight;
	_DtCvUnit	blockWidth;

	/*
	 * make sure all of the information in the body is saved out
	 */
	CheckSaveInfo (canvas, layout, NULL, 0);

	/*
	 * now calculate the non-controllers overall height.
	 */
	blockHeight = layout->info.y_pos - saveYpos - topHeight;

	/*
	 * now figure the head positions.
	 */
	DetermineHeadPositioning(&topBot, &sideDims, &cornerDims, &flowDims,
				saveYpos, topHeight,
				blockHeight, &blockHeight);
	/*
	 * if the maximum available space was exceeded by the text
	 * calculate a new max width
	 */
	if (layout->max_width < layout->info.cur_max_x + layout->right)
	    layout->max_width = layout->info.cur_max_x + layout->right;

	blockWidth = layout->max_width - basePt.left - basePt.right
						- layout->left - layout->right;

	nxtHead = headInfo;
	cur_seg = segStart;
	while (cur_seg != NULL)
	  {
	    if (_DtCvIsSegContainer(cur_seg) && _DtCvIsSegController(cur_seg))
	      {
	        AdjustHeadPosition(canvas, layout, cur_seg,
			&topBot, &sideDims, &cornerDims, &flowDims, nxtHead,
			0, basePt.left + layout->left - leftMargin, blockWidth,
			leftMargin, rightMargin);
	        /*
	         * go to the next head element
		 */
	        nxtHead = nxtHead->next_info;

	        /*
	         * free the information.
	         */
	        free(headInfo);
	        headInfo = nxtHead;
	      }

	    /*
	     * got to the next segment
	     */
	    cur_seg = cur_seg->next_seg;
	  }

	if (layout->info.y_pos < saveYpos + topHeight + blockHeight)
	    layout->info.y_pos = saveYpos + topHeight + blockHeight;

	layout->info.y_pos += botHeight;
      }

    /*
     * set the return values
     */
    *ret_width = layout->info.cur_max_x - basePt.left + layout->right;
    *ret_max_x = layout->info.cur_max_x;

    return;
}

/*****************************************************************************
 * Function:    static _DtCvUnit MaxOfGroup (
 *
 * Purpose: Determine the maximum of a group.
 *****************************************************************************/
static void
MaxOfGroup (
    GrpInfo		*group,
    _DtCvDspLine	*text,
    _DtCvLineSeg	*lines,
    _DtCvUnit		 max_x,
    _DtCvUnit		 max_y)
{
    int		 i;

    /*
     * initialize
     */
    group->min_x = max_x;
    group->max_x = 0;
    group->top_y = max_y;
    group->bot_y = 0;

    /*
     * find the maximum of the group
     */
    for (i = group->cnt.beg_txt; i < group->cnt.end_txt; i++)
      {
	/*
	 * check for min's and max's
	 */
	if (group->min_x > text[i].text_x)
	    group->min_x = text[i].text_x;

	if (group->max_x < text[i].max_x)
	    group->max_x = text[i].max_x;

	if (group->top_y > text[i].baseline - text[i].ascent)
	    group->top_y = text[i].baseline - text[i].ascent;

	if (group->bot_y < text[i].baseline + text[i].descent)
	    group->bot_y = text[i].baseline + text[i].descent;

	/*
	 * indicate that this line has been processed already
	 */
	_DtCvSetProcessed(text[i]);
      }

    for (i = group->cnt.beg_ln; i < group->cnt.end_ln; i++)
      {
	/*
	 * check for min's and max's
	 */
	if (group->min_x > lines[i].pos_x)
	    group->min_x = lines[i].pos_x;

	if (group->max_x < lines[i].max_x)
	    group->max_x = lines[i].max_x;

	if (group->top_y > lines[i].pos_y)
	    group->top_y = lines[i].pos_y;

	if (group->bot_y < lines[i].max_y)
	    group->bot_y = lines[i].max_y;

	/*
	 * indicate that this line has been processed already
	 */
	_DtCvSetProcessed(lines[i]);
      }
}

/*****************************************************************************
 * Function:    static _DtCvUnit TestSpacing (
 *
 * Parameters:
 *
 * Returns:	True	if the object is before (x wise) the test object.
 *		False	if the object is not before (x wise) the text object.
 *
 * Purpose:
 * 
 *****************************************************************************/
static _DtCvStatus
TestSpacing (
    _DtCvUnit		 tst_top,
    _DtCvUnit		 tst_bot,
    _DtCvUnit		 tst_min,
    _DtCvUnit		 obj_top,
    _DtCvUnit		 obj_bot,
    _DtCvUnit		 obj_max,
    _DtCvUnit		 needed,
    _DtCvUnit		 min_space,
    _DtCvUnit		*ret_amount)
{
    _DtCvStatus  result = False;

    /*
     * check to see if the object is to the left of the test object
     * to move and that it 'infringes' on the vertical
     * space of the test object.
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
    if (obj_max < tst_min
	&& True == _DtCvCheckInfringement(tst_top, tst_bot, obj_top, obj_bot)
	&& needed > tst_min - obj_max)
      {
	/*
	 * okay, this infringes on the object's space.
	 * truncate the amount of room there is to move the object
	 */
	result = True;
	needed = tst_min - obj_max;

	/*
	 * is the space between these two objects already squeezed
	 * below the minimum allowed?
	 */
	if (needed < min_space)
	    needed = 0;
	else /* leave the minimum space between the objects */
	    needed -= min_space;
      }

    *ret_amount = needed;
    return result;
}

/*****************************************************************************
 * Function:    static void MoveLeft (
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:	Moves the object's rules/lines and text lines to the left.
 * 
 *****************************************************************************/
static void
MoveLeft (
    _DtCanvasStruct	*canvas,
    int			 beg_txt,
    int			 end_txt,
    int			 beg_ln,
    int			 end_ln,
    _DtCvUnit		 space)
{
    int		i;

    /*
     * bail now if nothing to do.
     */
    if (1 > space)
	return;

    /*
     * move each text/region line.
     */
    for (i = beg_txt; i < end_txt; i++)
      {
	canvas->txt_lst[i].text_x -= space;
	canvas->txt_lst[i].max_x  -= space;
      }

    /*
     * move each line/rule.
     */
    for (i = beg_ln; i < end_ln; i++)
      {
	canvas->line_lst[i].pos_x -= space;
	canvas->line_lst[i].max_x -= space;
      }
}

/*****************************************************************************
 * Function:    static _DtCvUnit CheckSpacing (
 *
 * Purpose:	Check the spacing before an object and move any objects
 *		before it to the left to make room.
 *
 *****************************************************************************/
static _DtCvUnit
CheckSpacing (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    GrpInfo		*tst_grp,
    int			 txt_idx,
    int			 line_idx,
    _DtCvUnit		 top_y,
    _DtCvUnit		 bot_y,
    _DtCvUnit		 min_x,
    _DtCvUnit		 needed)
{
    int		 i;
    _DtCvUnit	 space;
    _DtCvUnit	 maxSpace;
    _DtCvUnit	 topY;
    _DtCvUnit	 botY;
    GrpInfo     *nxtGrp   = layout->grp_lst;

    /*
     * truncate if the amount needed is more than available.
     */
    if (min_x < needed)
	needed = min_x;

    maxSpace = needed;

    /*
     * see what group is before this group and how much space there is
     */
    while (NULL != nxtGrp)
      {
	/*
	 * as long as I'm not comparing against myself, try it.
	 */
	if (nxtGrp != tst_grp)
	  {
	    /*
	     * is this group before(x wise) the test group, infringing
	     * upon the test group's top and bottom positioning (y)
	     * and is the amount of space (x wise) between them smaller
	     * than the current smallest space found?
	     */
	    if (True == TestSpacing (top_y, bot_y, min_x,
					nxtGrp->top_y, nxtGrp->bot_y,
					nxtGrp->max_x, needed,
					canvas->metrics.horiz_pad_hint,
					&space))
	      {
		space += MoveGroup(canvas, layout, nxtGrp, needed - space);
		if (maxSpace > space)
		    maxSpace = space;
	      }
	  }

	/*
	 * check the next group
	 */
	nxtGrp = nxtGrp->next_info;
      }

    /*
     * look at each of the text lines;
     */
    for (i = 0; i < canvas->txt_cnt; i++)
      {
	/*
	 * Only look at those lines not already processed.
	 */
	if (i != txt_idx && _DtCvIsNotProcessed(canvas->txt_lst[i]))
	  {
	    topY = canvas->txt_lst[i].baseline - canvas->txt_lst[i].ascent;
	    botY = canvas->txt_lst[i].baseline - canvas->txt_lst[i].descent;
	    if (True == TestSpacing(top_y, bot_y, min_x, topY, botY,
				canvas->txt_lst[i].max_x, needed,
				canvas->metrics.horiz_pad_hint, &space))
	      {
		space += MoveText(canvas, layout, i, topY, botY, needed-space);
		if (maxSpace > space)
		    maxSpace = space;
	      }
	  }
      }

    /*
     * look at each of the rules/lines;
     */
    for (i = 0; i < canvas->line_cnt; i++)
      {
	/*
	 * Only look at those lines not already processed.
	 */
	if (i != line_idx && _DtCvIsNotProcessed(canvas->line_lst[i]))
	  {
	    /*
	     * calculate the top and bottom of the line
	     */
	    topY = canvas->line_lst[i].pos_y;
	    botY = canvas->line_lst[i].max_y;

	    if (True == TestSpacing(top_y, bot_y, min_x, topY, botY,
				canvas->line_lst[i].max_x, needed,
				canvas->metrics.horiz_pad_hint, &space))
	      {
		space += MoveLines(canvas, layout, i, topY, botY, needed-space);
		if (maxSpace > space)
		    maxSpace = space;
	      }
	  }
      }

    return maxSpace;
}

/*****************************************************************************
 * Function:    static _DtCvUnit MoveGroup (_DtCanvasStruct canvas);
 *
 * Purpose:	To move groupings (container, tables, etc.) as a group to
 *		honor boundaries.
 * 
 *****************************************************************************/
static _DtCvUnit
MoveGroup (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    GrpInfo		*tst_grp,
    _DtCvUnit		 needed)
{
    _DtCvUnit	 space;

    /*
     * find out what's in front of it. And how much 'extra' room
     * there is.
     */
    space = CheckSpacing(canvas, layout, tst_grp, -1, -1,
				tst_grp->top_y, tst_grp->bot_y, tst_grp->min_x,
				needed);
    /*
     * now move the group
     */
    MoveLeft(canvas, tst_grp->cnt.beg_txt, tst_grp->cnt.end_txt,
			tst_grp->cnt.beg_ln, tst_grp->cnt.end_ln, space);

    tst_grp->max_x -= space;
    tst_grp->min_x -= space;

    return space;
}

/*****************************************************************************
 * Function:    static _DtCvUnit MoveText (_DtCanvasStruct canvas);
 *
 * Purpose: To move text lines to honor boundaries.
 * 
 *****************************************************************************/
static _DtCvUnit
MoveText (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    int			 idx,
    _DtCvUnit		 top_y,
    _DtCvUnit		 bot_y,
    _DtCvUnit		 needed)
{
    _DtCvUnit	 space;

    /*
     * find out what's in front of it. And how much 'extra' room
     * there is.
     */
    space = CheckSpacing(canvas, layout, NULL, idx, -1,
				top_y, bot_y,
				canvas->txt_lst[idx].text_x,
				needed);
    /*
     * now move the group
     */
    MoveLeft(canvas, idx, idx, -1, -1, space);

    return space;
}

/*****************************************************************************
 * Function:    static _DtCvUnit MoveLines (_DtCanvasStruct canvas);
 *
 * Purpose: To move groupings (container, tables, etc.) as a group to
 *	    honor boundaries.
 * 
 *****************************************************************************/
static _DtCvUnit
MoveLines (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    int			 idx,
    _DtCvUnit		 top_y,
    _DtCvUnit		 bot_y,
    _DtCvUnit		 needed)
{
    _DtCvUnit	 space;

    /*
     * find out what's in front of it. And how much 'extra' room
     * there is.
     */
    space = CheckSpacing(canvas, layout, NULL, -1, idx,
				top_y, bot_y,
				canvas->line_lst[idx].pos_x,
				needed);

    /*
     * now move the group
     */
    MoveLeft(canvas, -1, -1, idx, idx, space);

    return space;
}

/*****************************************************************************
 * Function:    static void CheckMoveInfo (_DtCanvasStruct canvas);
 *
 * Purpose:	To move each of the groupings, rules and text lines to
 *		honor boundaries.
 * 
 *****************************************************************************/
static void
CheckMoveInfo (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout)
{
    int		 i;
    _DtCvUnit	 topY;
    _DtCvUnit	 botY;
    _DtCvUnit	 maxWidth = canvas->metrics.width;
    GrpInfo	*nxtGrp;

    /*
     * fill in the max x of each group
     */
    for (nxtGrp = layout->grp_lst; NULL != nxtGrp; nxtGrp = nxtGrp->next_info)
      {
	/*
	 * find the maximum of the group
	 */
	MaxOfGroup(nxtGrp, canvas->txt_lst, canvas->line_lst,
				layout->info.max_x_pos, layout->info.y_pos);
      }

    /*
     * now check each group for exceeding the boundary.
     */
    for (nxtGrp = layout->grp_lst; NULL != nxtGrp; nxtGrp = nxtGrp->next_info)
      {
	/*
	 * does this group exceed the boundary?
	 */
	if (maxWidth < nxtGrp->max_x)
	     (void) MoveGroup(canvas, layout, nxtGrp, nxtGrp->max_x - maxWidth);
      }

    /*
     * look at each of the text lines;
     */
    for (i = 0; i < canvas->txt_cnt; i++)
      {
	/*
	 * Only look at those lines not already processed.
	 */
	if (_DtCvIsNotProcessed(canvas->txt_lst[i]) &&
					maxWidth < canvas->txt_lst[i].max_x)
	  {
	    topY = canvas->txt_lst[i].baseline - canvas->txt_lst[i].ascent;
	    botY = canvas->txt_lst[i].baseline - canvas->txt_lst[i].descent;
	    (void) MoveText(canvas, layout, i, topY, botY,
					canvas->txt_lst[i].max_x - maxWidth);
	  }
      }

    /*
     * look at each of the rules/lines;
     */
    for (i = 0; i < canvas->line_cnt; i++)
      {
	/*
	 * Only look at those lines not already processed.
	 */
	if (_DtCvIsNotProcessed(canvas->line_lst[i]) &&
					maxWidth < canvas->line_lst[i].max_x)
	  {
	    /*
	     * calculate the top and bottom of the line
	     */
	    topY = canvas->line_lst[i].pos_y;
	    botY = canvas->line_lst[i].max_y;

	    (void) MoveLines(canvas, layout, i, topY, botY,
					canvas->line_lst[i].max_x - maxWidth);
	  }
      }
}

/*****************************************************************************
 * Function:    static void CompareUnits ()
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
static int
CompareUnits (
    const void  *a,
    const void  *b)
{
    _DtCvUnit *aPtr = (_DtCvUnit *) a;
    _DtCvUnit *bPtr = (_DtCvUnit *) b;

    if (*aPtr <  *bPtr) return -1;
    if (*aPtr == *bPtr) return 0;

    return 1;
}

/*****************************************************************************
 * Function:    static void CompareSearchs ()
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
static int
CompareSearchs (
    const void  *a,
    const void  *b)
{
    _DtCvSearchData *searchA = (_DtCvSearchData *) a;
    _DtCvSearchData *searchB = (_DtCvSearchData *) b;
    _DtCvDspLine    *lineA   = &(searchA->lst[searchA->idx]);
    _DtCvDspLine    *lineB   = &(searchB->lst[searchB->idx]);
    _DtCvUnit        topA    = lineA->baseline - lineA->ascent;
    _DtCvUnit        topB    = lineB->baseline - lineB->ascent;
    _DtCvUnit        heightA = lineA->ascent   + lineA->descent;
    _DtCvUnit        heightB = lineB->ascent   + lineB->descent;
    _DtCvUnit        centA   =  topA + (heightA >> 1);
    _DtCvUnit        centB   =  topB + (heightB >> 1);

    if (lineA->baseline + lineA->descent < centB && centA < topB)
	return -1;

    if (lineB->baseline + lineB->descent < centA && centB < topA)
	return 1;

    if (lineA->text_x != lineB->text_x)
	return ((lineA->text_x < lineB->text_x) ? -1 : 1);

    if (topA != topB)
	return ((topA < topB) ? -1 : 1);

    if (heightA != heightB)
	return ((heightA < heightB) ? -1 : 1);

    if (lineA->max_x != lineB->max_x)
	return ((lineA->max_x < lineB->max_x) ? -1 : 1);

    return 0;
}

/*****************************************************************************
 * Function:    static Status LayoutCanvasInfo (_DtCvHandle canvas);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
static _DtCvStatus
LayoutCanvasInfo (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    _DtCvUnit		 divisor,
    char		*target_id)
{
    int                  i = 0;
    DataPoint		 basePt;
    _DtCvUnit		 maxWidth = 0;
    _DtCvUnit		 maxXPos  = 0;
    _DtCvStatus		 result   = _DtCvSTATUS_OK;

    *layout           = DefInfo;
    layout->divisor   = divisor;
    layout->max_width = canvas->metrics.width;
    layout->left      = canvas->metrics.side_margin;
    layout->right     = canvas->metrics.side_margin;
    layout->target_id = target_id;

    _DtCvInitLayoutInfo(canvas, &(layout->info));

    basePt = DefDataPt;
    PushDataPoint(layout, &basePt);
    SetMargins (layout);
    SetTextPosition (layout, True);

    ProcessSegmentList(canvas, layout, canvas->element_lst, -1,
						&maxWidth, &maxXPos, NULL);

    RemoveDataPoint(layout, &basePt);

    /*
     * fill in the max_x of each line of text/regions.
     */
    for (i = 0; i < canvas->txt_cnt; i++)
        canvas->txt_lst[i].max_x = MaxXOfLine(canvas, &(canvas->txt_lst[i]));

    /*
     * calculate the actual right hand side boundary.
     */
    layout->info.max_x_pos += canvas->metrics.side_margin;

    /*
     * the max_x_pos so far has indicated where the *next* character,
     * line or region will be *started*. Therefore back up one to
     * indicate the true last position used.
     */
    layout->info.max_x_pos--;

    return result;

}  /* End LayoutCanvasInfo */

/*****************************************************************************
 * Function:    static Status LayoutCanvas (_DtCvHandle canvas);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
static _DtCvStatus
LayoutCanvas (
    _DtCanvasStruct	*canvas,
    LayoutInfo		*layout,
    char		*target_id)
{
    _DtCvUnit		 divisor = 1;
    _DtCvValue		 redo;
    _DtCvStatus		 result;

    int i, search_cnt = canvas->search_cnt;
    
    do {
	redo = False;
        result = LayoutCanvasInfo(canvas, layout, divisor, target_id);

        /*
         * Are we suppose to honor the boundary?
	 * If so, do any lines go over the boundary?
	 * Is there any margins that can be decremented?
         */
	if (_DtCvSTATUS_OK == result
		&& (_DtCvUSE_BOUNDARY == canvas->constraint ||
			_DtCvUSE_BOUNDARY_MOVE == canvas->constraint)
		&& layout->info.max_x_pos >= canvas->metrics.width)
          {
	    if (_DtCvUSE_BOUNDARY_MOVE == canvas->constraint)
	      {
		int i;

		/*
		 * clear the processed flag from all the text/region lines.
		 */
		for (i = 0; i < canvas->txt_cnt; i++)
		   _DtCvClearProcessed(canvas->txt_lst[i]);

		/*
		 * clear the processed flag from all the line/rules.
		 */
		for (i = 0; i < canvas->line_cnt; i++)
		   _DtCvClearProcessed(canvas->line_lst[i]);

		CheckMoveInfo(canvas, layout);

		/*
		 * recalculate the new max x
		 */
		layout->info.max_x_pos = 0;
		for (i = 0; i < canvas->txt_cnt; i++)
		    if (layout->info.max_x_pos < canvas->txt_lst[i].max_x)
		        layout->info.max_x_pos = canvas->txt_lst[i].max_x;

		for (i = 0; i < canvas->line_cnt; i++)
		    if (layout->info.max_x_pos < canvas->line_lst[i].max_x)
		        layout->info.max_x_pos = canvas->line_lst[i].max_x;

		layout->info.max_x_pos--;
	      }
	    else if (True == layout->margin_non_zero)
	      {
	        redo = True;
	        divisor *= 2;
	        canvas->txt_cnt  = 0;
	        canvas->line_cnt = 0;
	        canvas->trav_cnt = 0;
              }
          }

      } while (True == redo);

    /*
     * clean up table information
     */
    if (NULL != layout->grp_lst)
	free(layout->grp_lst);

    /*
     * subtract one from the y position to indicate the *last*
     * pixel/column/etc that will be rendered.
     */
    canvas->max_y = layout->info.y_pos - 1;
    canvas->max_x = layout->info.max_x_pos;

    for (i = search_cnt; i < canvas->search_cnt; i++)
      {
	canvas->searchs[i - search_cnt]     = canvas->searchs[i];
	canvas->searchs[i - search_cnt].lst = canvas->txt_lst;
      }

    canvas->search_cnt -= search_cnt;

    /*
     * are there any search hits?
     */
    if (0 != canvas->search_cnt)
	qsort (canvas->searchs, canvas->search_cnt, sizeof(_DtCvSearchData),
							CompareSearchs);
    /*
     * sort the page break list.
     */
    if (0 != canvas->brk_cnt)
	qsort (canvas->pg_breaks, canvas->brk_cnt, sizeof(_DtCvUnit),
								CompareUnits);

    return result;

}  /* End LayoutCanvas */

/*****************************************************************************
 * Function:    static void SortTraversal (_DtCvHandle canvas);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
static void
SortTraversal (_DtCanvasStruct *canvas)
{
    int i;

    /*
     * sort the links correctly. First, establish the x,y,width,height
     * of each link.
     */
    for (i = 0; i < canvas->trav_cnt; i++)
      {
	if (_DtCvTraversalLink == canvas->trav_lst[i].type)
	    GetLinkInfo(canvas, i, &(canvas->trav_lst[i].x_pos),
					&(canvas->trav_lst[i].y_pos),
					&(canvas->trav_lst[i].width),
					&(canvas->trav_lst[i].height));
	else
	    _DtCvCalcMarkPos(canvas, canvas->trav_lst[i].idx,
					&(canvas->trav_lst[i].x_pos),
					&(canvas->trav_lst[i].y_pos),
					&(canvas->trav_lst[i].width),
					&(canvas->trav_lst[i].height));
      }

    _DtCvSortTraversalList(canvas, _DtCvFALSE);
}

/*****************************************************************************
 * Function:    static void ProcessMarks (_DtCanvasStruct *canvas);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
static _DtCvStatus
ProcessMarks (
    _DtCanvasStruct	 *canvas,
    _DtCvPointInfo	**mark_lst)
{
    int			markIdx;
    int			result = _DtCvSTATUS_OK;
    _DtCvSelectData	beg;
    _DtCvSelectData	end;
    _DtCvSegmentI	*firstSeg;

    while (NULL != mark_lst && NULL != *mark_lst)
      {
	/*
	 * convert the segments to begin and end points
	 */
	if (_DtCvSTATUS_BAD == _DtCvCvtSegsToPts(canvas, (*mark_lst)->segs,
					&beg, &end, NULL, NULL, &firstSeg))
	    /*
	     * just set a return value since this indicates bad data
	     * and not system failure.
	     */
	    result = _DtCvSTATUS_BAD;

	/*
	 * now add it to the mark list
	 */
	else
	  {
	    markIdx =  _DtCvAddToMarkList(canvas, (*mark_lst)->client_data,
						_DtCvFALSE, &beg, &end);
	    /*
	     * now put the mark in the traversal list, but don't sort
	     * or fill out position and dimension information.
	     * SortTraversal() will do that.
	     *
	     * bail here if system failure indicated.
	     */
	    if (-1 == markIdx || 0 != _DtCvSetTravEntryInfo(canvas,
						_DtCvGetNextTravEntry(canvas),
						_DtCvTraversalMark, firstSeg,
						markIdx, _DtCvTRUE))
	        return _DtCvSTATUS_BAD;
	  }

	mark_lst++;
      }

    return result;
}

/*****************************************************************************
 *              Public Functions
 *****************************************************************************/
/*****************************************************************************
 * Function:    void _DtCanvasResize (_DtCvHandle canvas);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
_DtCvStatus
_DtCanvasResize (
    _DtCvHandle		 canvas_handle,
    _DtCvValue		 force,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_height )
{
    int			 i;
    _DtCvStatus		 selectStatus;
    _DtCvStatus		 retStatus = _DtCvSTATUS_NONE;
    _DtCanvasStruct	*canvas    = (_DtCanvasStruct *) canvas_handle;
    _DtCvUnit		 oldWidth  = canvas->metrics.width;
    LayoutInfo		 layOut;
    _DtCvPointInfo       selPt;
    _DtCvPointInfo	**markInfo;

    selPt.client_data = NULL;
    selPt.segs        = NULL;

    /*
     * check to see if the width has changed - if not,
     * don't do anything (but re-initialize the metrics
     * to get the new height).
     */
    (*(canvas->virt_functions.get_metrics))(canvas->client_data,
				_DtCvCANVAS_TYPE, &(canvas->metrics));

    if (canvas->metrics.width != oldWidth || _DtCvTRUE == force)
      {
        /*
         * remember the current selection.
         */
        selectStatus = _DtCanvasGetSelectionPoints(canvas, &(selPt.segs),
								NULL, NULL);
        /*
         * remember the marks
         */
        if (_DtCvSTATUS_BAD == _DtCvGetMarkSegs(canvas, &markInfo))
	    return _DtCvSTATUS_BAD;

        /*
         * Re-Layout the information.
         * First step - invalidate some counters.
         */
        canvas->trav_cnt = 0;	/* zero this only because we re-process */
				/* do not zero cur_hyper or we'll loose */
				/* where we are in the TOC              */
        canvas->txt_cnt  = 0;
        canvas->line_cnt = 0;
        canvas->mark_cnt = 0;
        canvas->brk_cnt  = 0;

        /*
         * Layout the information if there is anything to do
         */
        if (_DtCvSTATUS_BAD == LayoutCanvas (canvas, &layOut, NULL))
	    return _DtCvSTATUS_BAD;
    
        /*
         * restore the current selection.
         */
        if (_DtCvSTATUS_OK == selectStatus)
          {
	    _DtCanvasActivatePts(canvas,_DtCvACTIVATE_SELECTION, &selPt,
								NULL,NULL);
	    _DtCvFreeArray((void **) selPt.segs);
          }

        /*
         * now place the marks in the mark and traversal lists
         */
        ProcessMarks(canvas, markInfo);
        if (NULL != markInfo)
	  {
	    for (i = 0; NULL != markInfo[i]; i++)
		_DtCvFreeArray((void **) (markInfo[i]->segs));
	    _DtCvFreeArray((void **) markInfo);
	  }

        /*
         * sort the traversal list.
         */
        SortTraversal(canvas);
	retStatus = _DtCvSTATUS_OK;
      }

    /*
     * return the maximum height and width used
     */
    if (ret_width != NULL)
        *ret_width  = canvas->max_x;
    if (ret_height != NULL)
        *ret_height = canvas->max_y;

    return retStatus;

}  /* End _DtCanvasResize */

/*****************************************************************************
 * Function:    void _DtCanvasSetTopic (_DtCvHandle canvas);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 * 
 *****************************************************************************/
_DtCvStatus
_DtCanvasSetTopic (
    _DtCvHandle		 canvas_handle,
    _DtCvTopicPtr	 topic,
    _DtCvValue		 honor_size,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_height,
    _DtCvUnit		*ret_y )
{
    _DtCvStatus		 result = _DtCvSTATUS_OK;
    _DtCanvasStruct	*canvas = (_DtCanvasStruct *) canvas_handle;
    LayoutInfo		layOut;

    /*
     * clean the canvas
     */
    _DtCanvasClean (canvas_handle);

    /*
     * attach to the canvas
     */
    canvas->element_lst = topic->seg_list;

    /*
     * Attach the link information
     */
    canvas->link_data = topic->link_data;

    /*
     * init the internal use pointer in all containers to NULL
     */
    _DtCvClearInternalUse(canvas->element_lst, _DtCvFALSE);

    /*
     * Layout the information if there is anything to do
     */
    canvas->constraint = honor_size;
    if (_DtCvSTATUS_BAD == LayoutCanvas (canvas, &layOut, topic->id_str))
	return _DtCvSTATUS_BAD;
    
    /*
     * add the marks to the mark and traversal lists
     */
    ProcessMarks(canvas, topic->mark_list);

    /*
     * sort the traversal list.
     */
    SortTraversal(canvas);

    /*
     * return the maximum height and width used
     * And the location of the id.
     */
    if (ret_width != NULL)
        *ret_width = canvas->max_x;
    if (ret_height != NULL)
        *ret_height = canvas->max_y;
    if (ret_y != NULL)
      {
	if (NULL != layOut.target_id && True != layOut.id_found)
	    result = _DtCvSTATUS_ID_BAD;
        *ret_y = layOut.id_Ypos;
      }

    return result;

}  /* End _DtCanvasSetTopic */
