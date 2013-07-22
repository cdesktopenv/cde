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
/* $XConsortium: CanvasI.h /main/18 1996/10/21 13:59:51 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   CanvasI.h
 **
 **  Project:
 **
 **  Description:  Public Header file for Canvas.c
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _DtCanvasI_h
#define _DtCanvasI_h


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/********    Internal Defines Declarations    ********/
#define	_DtCvLINE_HORZ	0
#define	_DtCvLINE_VERT	1

/********    Internal Enum Declarations    ********/
enum	_dtCvTraversalType
  {
	_DtCvTraversalNone,
	_DtCvTraversalLink,
	_DtCvTraversalMark
  };

/********    Internal Typedef Declarations    ********/

#if !defined(_DtCvLinkMgrP_h) && !defined(_DtCvLinkMgrI_h)
typedef	struct	_dtCvLinkDb*	_DtCvLinkDb;
#endif

typedef	enum	_dtCvTraversalType	_DtCvTraversalType;

/* adding this typedef so that debugging can access the structure */
typedef	struct	_dtCvSegment		_DtCvSegmentI;
typedef struct  _dtCvSegPts		_DtCvSegPtsI;

/********    Internal Structures Declarations    ********/

typedef	struct _dtCvDspLine {
	_DtCvValue	 processed;
	_DtCvUnit	 text_x;
	_DtCvUnit	 max_x;
	_DtCvUnit	 baseline;
	_DtCvUnit	 descent;
	_DtCvUnit	 ascent;
	int		 byte_index;
	int		 length;
	_DtCvSegmentI	*seg_ptr;
} _DtCvDspLine;

typedef	struct	_dtCvLineSeg {
	_DtCvValue	 processed;
	short		dir;
	_DtCvUnit	pos_x;
	_DtCvUnit	max_x;
	_DtCvUnit	pos_y;
	_DtCvUnit	max_y;
	_DtCvUnit	width;
	_DtCvPointer	data;
} _DtCvLineSeg;

typedef	struct _dtCvPointData {
	_DtCvUnit	x;
	_DtCvUnit	y;		/* baseline of the line */
	int		line_idx;
	int		char_idx;
} _DtCvPointData;

typedef	struct	_dtCvMarkData {
	_DtCvValue	on;
	_DtCvPointer	client_data;
	_DtCvPointData	beg;
	_DtCvPointData	end;
} _DtCvMarkData;

typedef	struct	_dtCvTraversalInfo {
	_DtCvValue		 active;
	_DtCvTraversalType	 type;	/* the type of the traversal */
	int			 idx;	/* the index into the approp. struct */
					/* either the txt_lst or marks  */
	_DtCvUnit		 x_pos;	/* x pos on the line.              */
	_DtCvUnit		 y_pos;	/* top y pos of bounding box       */
	_DtCvUnit		 width;	/* width of link on 1st line only  */
	_DtCvUnit		 height; /* height of 1st line bounding box */
	_DtCvSegmentI		*seg_ptr; /* ptr to first seg of link        */
} _DtCvTraversalInfo;

typedef	struct	_dtCvSearchData {
	int		idx;	/* the line index of the search hit */
	_DtCvDspLine	*lst;	/* pointer to the text line list    */
} _DtCvSearchData;

typedef	struct _dtCanvasStruct {
	int		error;
	long		txt_cnt;	/* maximum used in txt_list	*/
	int		txt_max;	/* maximum in txt_list		*/

	int		line_cnt;	/* maximum used in line_lst	*/
	int		line_max;	/* maximum in line_lst		*/
	int		mark_cnt;	/* mark counter  		*/
	int		mark_max;	/* maximum in mark_lst          */

	int		trav_cnt;	/* maximum used in trav_lst	*/
	int		trav_max;	/* maximum in trav_lst		*/
	int		cur_trav;	/* traversal indicator		*/

	int		search_cnt;
	int		search_max;

	int		brk_cnt;	/* the number of page breaks    */
	int		brk_max;	/* the maxium entries           */

	short		mb_length;	/* The maximum length of a char */

	_DtCvUnit		max_x;		/* The maximum x position */
	_DtCvUnit		max_y;		/* The maximum y position */
	_DtCvValue		constraint;	/* Indicates if the right
						   boundary can be breached */
	_DtCvValue		trav_on;	/* Indicates if the traversal
						   is on or off.            */
	_DtCvPointer		client_data;
	_DtCvMetrics		metrics;
	_DtCvSpaceMetrics	link_info;
	_DtCvSpaceMetrics	traversal_info;
	_DtCvLocale		locale;

	_DtCvSegmentI		*element_lst;
	_DtCvDspLine		*txt_lst;
	_DtCvLineSeg		*line_lst;
	_DtCvTraversalInfo	*trav_lst;
	_DtCvLinkDb		 link_data;
	_DtCvPointData		 select_start;
	_DtCvPointData		 select_end;
	_DtCvMarkData		*marks;
	_DtCvSearchData		*searchs;
	_DtCvUnit		*pg_breaks;
	_DtCvVirtualInfo	 virt_functions;

} _DtCanvasStruct;

/********    Internal Structure Typedef Declarations    ********/
typedef	struct _dtCvPointData	_DtCvSelectData;

/********    Internal Macros Declarations    ********/

#define	_DtCvHasTraversal(x)	((x) & _DtCvTRAVERSAL_FLAG)

/*
 * segment type access
 */
#define	_DtCvIsTypeNoop(x) \
		((((x) & _DtCvPRIMARY_MASK) == _DtCvNOOP) ? 1 : 0)

/*
 * Is a flag set
 */
#define	_DtCvIsSegVisibleLink(x)	_DtCvIsSegHyperText(x)
#define	_DtCvIsSegALink(x) \
			(_DtCvIsSegHyperText(x) || _DtCvIsSegGhostLink(x))

#define	_DtCvIsMarkMaskOn(x) \
			(((x) & _DtCvACTIVATE_MARK_ON) ? _DtCvTRUE : _DtCvFALSE)

/*
 * remove masks
 */
#define _DtCvRemoveBeginFlags(x)	x &= ~(_DtCvTRAVERSAL_BEGIN | \
					       _DtCvLINK_BEGIN      | \
					       _DtCvMARK_BEGIN      | \
					       _DtCvSEARCH_BEGIN)
#define _DtCvSetSearchEnd(x,seg)	x |= ((seg)->type & _DtCvSEARCH_END)
#define _DtCvSetSearchBegin(x,seg)	x |= ((seg)->type & _DtCvSEARCH_BEGIN)
#define _DtCvClearSearchFlags(x)	x &= ~(_DtCvSEARCH_FLAG  | \
					       _DtCvSEARCH_BEGIN | \
					       _DtCvSEARCH_END)

#define _DtCvClearLinkFlags(x)		x &= ~(_DtCvLINK_FLAG   | \
					       _DtCvLINK_BEGIN  | \
					       _DtCvLINK_END    | \
					       _DtCvLINK_POP_UP | \
					       _DtCvLINK_NEW_WINDOW)

#define	_DtCvClearProcessed(x)	(x).processed = False
#define	_DtCvSetProcessed(x)	(x).processed = True
#define	_DtCvIsProcessed(x)	(x).processed
#define	_DtCvIsNotProcessed(x)	((False == (x).processed) ? True : False)
#define _DtCvStraddlesPt(pt,min,max)	((min) <= (pt)  && (pt)  <= (max))

/********    Internal Function Declarations    ********/
extern	_DtCvUnit	_DtCvAdjustForSuperSub(
				_DtCanvasStruct   *canvas,
				_DtCvSegmentI     *p_seg,
				_DtCvUnit	  start_x,
				_DtCvUnit	 *script_x,
				_DtCvUnit	 *super_width,
				_DtCvUnit	 *super_y,
				_DtCvUnit	 *sub_width,
				_DtCvUnit	 *sub_y,
				_DtCvValue	 *super_flag,
				_DtCvValue	 *sub_flag);
extern	_DtCvUnit	_DtCvAdvanceXOfLine (
				_DtCanvasStruct   *canvas,
				_DtCvSegmentI     *p_seg,
				_DtCvUnit	  x_pos,
				int		 *link_idx,
				_DtCvValue	 *link_flag);
extern	_DtCvStatus	_DtCvCheckInfringement (
				_DtCvUnit	   tst_top,
				_DtCvUnit	   tst_bot,
				_DtCvUnit	   obj_top,
				_DtCvUnit	   obj_bot);
extern	void		_DtCvCheckLineMarks (
				_DtCanvasStruct   *canvas,
				int		   line_idx,
				int		   char_idx,
				int		   length,
				_DtCvUnit	   dst_x,
				_DtCvFlags	   check_flags,
				int		  *ret_len,
				_DtCvFlags	  *ret_old,
				_DtCvFlags	  *ret_new);
extern	void		_DtCvClearInternalUse (
				_DtCvSegmentI	 *list,
				_DtCvStatus	  flag);
extern	_DtCvUnit	_DtCvDrawSegments(
				_DtCanvasStruct	 *canvas,
				_DtCvDspLine	  line,
				_DtCvSegmentI	 *p_seg,
				int		  start_char,
				int		  count,
				int		 *prev_lnk,
				_DtCvUnit	  txt_x,
				_DtCvUnit	  sel_x,
				_DtCvUnit	 *scriptX,
				_DtCvUnit	 *super_width,
				_DtCvUnit	 *super_y,
				_DtCvUnit	 *sub_width,
				_DtCvUnit	 *sub_y,
				_DtCvValue	 *last_was_sub,
				_DtCvValue	 *last_was_super,
				_DtCvValue	 *last_link_vis,
				_DtCvFlags	  old_flag,
				_DtCvFlags	  new_flag,
				_DtCvElemType	  trav_flag,
				_DtCvPointer	  trav_data);
extern	int		_DtCvGetCharIdx(
				_DtCanvasStruct	 *canvas,
				_DtCvDspLine	  line,
				_DtCvUnit	  find_x);
extern	_DtCvUnit	_DtCvGetStartXOfLine(
				_DtCvDspLine	 *line,
				_DtCvSegmentI	**pSeg);
extern	void		_DtCvGetWidthOfSegment(
				_DtCanvasStruct	 *canvas,
				_DtCvSegmentI	 *p_seg,
				int		  start,
				int		  max_cnt,
				int		 *ret_cnt,
				_DtCvUnit	 *ret_w,
				_DtCvValue	 *ret_trimmed);
extern	_DtCvValue	_DtCvModifyXpos(
				_DtCvSpaceMetrics info,
				_DtCvSegmentI	*seg,
				_DtCvValue	 tst_result,
				_DtCvValue	 old_result,
				int		 idx,
				_DtCvUnit	*x_pos);
extern	void		_DtCvSkipLineChars(
				_DtCanvasStruct	 *canvas,
				_DtCvSegmentI	 *p_seg,
				int		  start,
				int		  max_cnt,
				int		  use_len,
				int		 *ret_start,
				_DtCvSegmentI	 **ret_seg);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtCanvasI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
