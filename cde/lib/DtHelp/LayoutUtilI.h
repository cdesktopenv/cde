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
/* $XConsortium: LayoutUtilI.h /main/13 1996/10/10 12:24:06 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:       LayoutUtilI.h
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
#ifndef _CELayoutUtilI_h
#define _CELayoutUtilI_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Private Defines Declarations    ********/
#define	_CEFORMAT_ALL	-1

/********    Private Typedef Declarations    ********/

/********    Private Structures Declarations    ********/
typedef	struct	_dtCvLayoutStruct {
	_DtCvSegmentI	*line_seg;
	unsigned int	 line_start;
	unsigned int	 line_bytes;
	_DtCvUnit	 cur_len;
	_DtCvUnit	 max_x_pos;
	_DtCvUnit	 cur_max_x;
	_DtCvUnit	 y_pos;
	_DtCvUnit	 text_x_pos;
	_DtCvUnit	 leading;
	int		 lst_hyper;
	int		 format_y;
	int		 join_line;
	_DtCvValue	 lst_vis;
	_DtCvValue	 join;
	_DtCvValue	 align_flag;
	const char	*align_char;
	_DtCvUnit	 align_pos;
	int		delayed_search_saves;
} _DtCvLayoutInfo;

/*****************************************************************************
 *			Private Macro Declarations
 *****************************************************************************/

/********    Private Function Declarations    ********/

extern	void		_DtCvAddSpace (
				_DtCvUnit	 number,
				_DtCvUnit	*ret_y);
extern	int		_DtCvAddToMarkList(
				_DtCanvasStruct      *canvas,
				_DtCvPointer	 client_data,
				_DtCvValue	 flag,
				_DtCvSelectData	*beg,
				_DtCvSelectData	*end);
extern	int		_DtCvCalcMarkPos (
				_DtCanvasStruct	*canvas,
				int		 entry,
				_DtCvUnit	*ret_x,
				_DtCvUnit	*ret_y,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_height);
extern	void		_DtCvCheckAddHyperToTravList (
				_DtCanvasStruct	*canvas,
				_DtCvSegmentI	*p_seg,
				_DtCvValue	 flag,
				_DtCvValue	*lst_vis,
				int		*lst_hyper,
				_DtCvUnit	*cur_len);
extern	_DtCvValue	_DtCvCheckLineSyntax (
				_DtCanvasStruct	 *canvas,
				_DtCvSegmentI	 *pSeg,
				int		  start,
				int		  str_len,
				_DtCvValue	  skip_last_ck);
extern	_DtCvValue	_DtCvCheckOneByteCantBeginList (
				char		 c,
				char		*cant_begin_list);
extern	_DtCvValue	_DtCvCheckOneByteCantEndList (
				char		 c,
				char		*cant_end_list);
extern	_DtCvStatus	_DtCvCvtSegsToPts (
				_DtCanvasStruct	 *canvas,
				_DtCvSegPtsI	**segs,
				_DtCvSelectData	 *beg,
				_DtCvSelectData	 *end,
				_DtCvUnit	 *ret_y1,
				_DtCvUnit	 *ret_y2,
				_DtCvSegmentI	**ret_seg);
extern	int		_DtCvGetNextTravEntry (
				_DtCanvasStruct	 *canvas);
extern	int		_DtCvGetNextWidth (
				_DtCanvasStruct	 *canvas,
				int		  old_type,
				int		  lst_hyper,
				_DtCvSegmentI	 *pSeg,
				int		  start,
				_DtCvSegmentI	 *prev_seg,
				_DtCvSegmentI	**nextSeg,
				int		 *nextStart,
				int		 *widthCount);
extern	int		_DtCvGetTraversalWidth (
				_DtCanvasStruct	 *canvas,
				_DtCvSegmentI    *p_seg,
				int		  lst_hyper);
extern	void		_DtCvInitLayoutInfo (
				_DtCanvasStruct	 *canvas,
				_DtCvLayoutInfo	 *layout);
extern	int		_DtCvProcessStringSegment(
				_DtCanvasStruct	 *canvas,
				_DtCvLayoutInfo	 *lay_info,
				_DtCvUnit	  max_width,
				_DtCvUnit	  l_margin,
				_DtCvUnit	  r_margin,
				_DtCvSegmentI	 *cur_seg,
				unsigned int	 *cur_start,
				_DtCvFrmtOption	  txt_justify,
				_DtCvValue	  stat_flag);
extern	void		_DtCvSaveInfo (
				_DtCanvasStruct	 *canvas,
				_DtCvLayoutInfo	 *layout,
				_DtCvUnit         max_width,
				_DtCvUnit         r_margin,
				_DtCvFrmtOption      txt_justify);
extern	void		_DtCvSetJoinInfo (
				_DtCvLayoutInfo	 *layout,
				_DtCvValue	  flag,
				int		  txt_ln);
extern	int		_DtCvSetTravEntryInfo (
			      _DtCanvasStruct	 *canvas,
			      int		  entry,
			      _DtCvTraversalType  type,
			      _DtCvSegmentI	 *p_seg,
			      int		  line_idx,
			      _DtCvValue	  inc);
extern	int		_DtCvSetTravEntryPos (
			      _DtCanvasStruct	 *canvas,
			      int		  entry,
			      _DtCvUnit		  x,
			      _DtCvUnit		  y,
			      _DtCvUnit		  width,
			      _DtCvUnit		  height);
extern	void		_DtCvSortTraversalList (
			      _DtCanvasStruct	 *canvas,
			      _DtCvValue	  retain);
extern	int		_DtCvSetSearchEntryInfo (
			      _DtCanvasStruct* canvas,
			      int line_idx);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _CELayoutUtilI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
