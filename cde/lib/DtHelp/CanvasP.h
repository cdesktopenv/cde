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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: CanvasP.h /main/17 1999/10/14 13:19:41 mgreess $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:         CanvasP.h
 **
 **  Project:      Cde Help System
 **
 **  Description:  Private Header file for the UI independent core engine
 **                of the help system. It contains function prototypes,
 **                structure definitions, typedefs, etc.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 *****************************************************************************
 *************************************<+>*************************************/
#ifndef _DtCanvasP_h
#define _DtCanvasP_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Public Re-defines   ********/
#if !defined(_AIX)
#define _DtCvStrCaseCmp(s1,s2)		strcasecmp(s1,s2)
#define _DtCvStrNCaseCmp(s1,s2, n)	strncasecmp(s1,s2,n)
#else
#define _DtCvStrCaseCmp(s1,s2)		_DtHelpCeStrCaseCmp(s1,s2)
#define _DtCvStrNCaseCmp(s1,s2,n)	_DtHelpCeStrNCaseCmp(s1,s2, n)
#endif /* _AIX */

/********    Public Defines Declarations    ********/
/*
 * true, false and null
 */
#ifndef Null
#define Null    0
#endif
#ifndef NULL
#define NULL    0
#endif
#ifndef False
#define False   0
#endif
#ifndef True
#define True    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

/*
 * Get Selection Type Declarations
 */
#define _DtCvSELECTED_TEXT	(1 << 0)
#define _DtCvSELECTED_REGION	(1 << 1)
#define _DtCvSELECTED_ALL	(~((unsigned int) 0))

/*
 * hypertext link types
 */
#define _DtCvLinkType_Execute      3
#define _DtCvLinkType_ManPage      4
#define _DtCvLinkType_AppDefine    5
#define _DtCvLinkType_SameVolume   6
#define _DtCvLinkType_CrossLink    7
#define _DtCvLinkType_TextFile     8

/*
 * hypertext window hints
 */
#define _DtCvWindowHint_PopupWindow        1
#define _DtCvWindowHint_CurrentWindow      2
#define _DtCvWindowHint_NewWindow          3
#define _DtCvWindowHint_Original           4

/*********************
 * Rendering flags
 *********************/

/*
 * Rendering flags - Link
 */
#define _DtCvLINK_FLAG		(0x01 <<  0)
#define _DtCvLINK_BEGIN		(0x01 <<  1)
#define _DtCvLINK_END		(0x01 <<  2)

#define _DtCvLINK_POP_UP	(0x01 <<  3)
#define _DtCvLINK_NEW_WINDOW	(0x01 <<  4)

/*
 * Rendering flags - Search
 *
 * a gap is left for the search flags on a segment in CanvasSegP.h.  If these
 * change value, the other values must move to accommodate it.  */
#define _DtCvSEARCH_FLAG	(0x01 <<  5)
#define _DtCvSEARCH_BEGIN	(0x01 <<  6)
#define _DtCvSEARCH_END		(0x01 <<  7)
#define _DtCvSEARCH_CURR	(0x01 <<  8)

/*
 * Rendering flags - Mark
 */
#define _DtCvMARK_FLAG		(0x01 <<  9)
#define _DtCvMARK_BEGIN		(0x01 << 10)
#define _DtCvMARK_END		(0x01 << 11)
#define _DtCvMARK_ON		(0x01 << 12)

/*
 * Rendering flags - Traversal
 */
#define _DtCvTRAVERSAL_FLAG	(0x01 << 13)
#define _DtCvTRAVERSAL_BEGIN	(0x01 << 14)
#define _DtCvTRAVERSAL_END	(0x01 << 15)

/*
 * Rendering flags - Application defined flags.
 */
#define _DtCvAPP_FIELD_OFFSET   (16)
#define _DtCvAPP_FIELD_MASK     (0x0f << _DtCvAPP_FIELD_OFFSET)
#define _DtCvAPP_FIELD_S_MASK   (0x03 << _DtCvAPP_FIELD_OFFSET)
#define _DtCvAPP_FLAG1          (0x01 << (_DtCvAPP_FIELD_OFFSET + 0))
#define _DtCvAPP_FLAG2          (0x01 << (_DtCvAPP_FIELD_OFFSET + 1))
#define _DtCvAPP_FLAG3          (0x01 << (_DtCvAPP_FIELD_OFFSET + 2))
#define _DtCvAPP_FLAG4          (0x01 << (_DtCvAPP_FIELD_OFFSET + 3))

/*
 * Rendering flags - Selection
 */
#define _DtCvSELECTED_FLAG	(0x01 << 20)
#define _DtCvEND_OF_LINE	(0x01 << 21)

/*
 * Activation masks
 */
#define _DtCvACTIVATE_SELECTION	(0x01 << 0)
#define _DtCvACTIVATE_MARK	(0x01 << 1)
#define _DtCvDEACTIVATE		(0x01 << 2)
#define _DtCvACTIVATE_MARK_ON	(0x01 << 3)
#define _DtCvACTIVATE_MARK_OFF	(0x01 << 4)

/********    Public Enum Declarations    ********/

/*
 * Element types
 */
enum	_dtCvElemType
  {
    _DtCvBAD_TYPE,
    _DtCvCANVAS_TYPE,
    _DtCvLINE_TYPE,
    _DtCvLINK_TYPE,
    _DtCvLOCALE_TYPE,
    _DtCvMARK_TYPE,
    _DtCvREGION_TYPE,
    _DtCvSTRING_TYPE,
    _DtCvTRAVERSAL_TYPE
  };

typedef	enum _dtCvElemType	_DtCvElemType;

/*
 * Canvas Engine values. Either parameters or return values.
 */
enum	_dtCvValue
  {
/*
 * status types. i.e. True/False, Ok/Bad, etc.
 */
    _DtCvFALSE      = False,
    _DtCvSTATUS_OK  = False,
    _DtCvTRUE       = True,
    _DtCvSTATUS_BAD = True,
    _DtCvSTATUS_ID_BAD,
    _DtCvSTATUS_NONE,
    _DtCvSTATUS_LINK,
    _DtCvSTATUS_MARK,
/*
 * Render types
 */
    _DtCvRENDER_PARTIAL,
    _DtCvRENDER_COMPLETE,
/*
 * Traversal defines
 */
    _DtCvTRAVERSAL_OFF,
    _DtCvTRAVERSAL_ON,
    _DtCvTRAVERSAL_TOP,
    _DtCvTRAVERSAL_NEXT,
    _DtCvTRAVERSAL_PREV,
    _DtCvTRAVERSAL_BOTTOM,
    _DtCvTRAVERSAL_ID,
    _DtCvTRAVERSAL_MARK,
/*
 * processing a selection types
 */
    _DtCvSELECTION_CLEAR,
    _DtCvSELECTION_START,
    _DtCvSELECTION_UPDATE,
    _DtCvSELECTION_END,
/*
 * honor the right boundary when laying out information
 */
    _DtCvUSE_BOUNDARY,
    _DtCvUSE_BOUNDARY_MOVE,
    _DtCvIGNORE_BOUNDARY
  };

typedef	enum _dtCvValue	_DtCvValue;
typedef	enum _dtCvValue	_DtCvStatus;
typedef	enum _dtCvValue	_DtCvRenderType;
typedef	enum _dtCvValue	_DtCvTraversalCmd;
typedef	enum _dtCvValue	_DtCvSelectMode;

/*
 * line wrap mode types.
 */
enum	_dtCvModeType
  {
    _DtCvModeWrapNone,
    _DtCvModeWrapDown,
    _DtCvModeWrapUp
  };

typedef	enum _dtCvModeType	_DtCvModeType;

/********    Public Typedef Declarations    ********/
typedef	void*	_DtCvPointer;
typedef	void*	_DtCvHandle;

typedef	int	_DtCvUnit;

typedef	unsigned long	_DtCvFlags;

typedef	struct	_dtCvSegPts	_DtCvSegPts;
typedef	struct	_dtCvSegment	_DtCvSegment;

/********    Public Structures Declarations    ********/
typedef	struct	_dtCvMetrics {
	_DtCvUnit	width;		/* max width of canvas area  */
	_DtCvUnit	height;		/* max height of canvas area */
	_DtCvUnit	top_margin;	/* top margin to allow for   */
	_DtCvUnit	side_margin;	/* left margin to allow for  */
	_DtCvUnit	line_height;	/* the height of an average line of */
					/* text                             */
	_DtCvUnit	horiz_pad_hint;	/* the optimul spacing to use if    */
					/* lines have to be squeezed for    */
					/* _DtCvUSE_BOUNDARY                */
} _DtCvMetrics;

typedef	struct	_dtCvSpaceMetrics {
	_DtCvUnit	space_before;
	_DtCvUnit	space_after;
	_DtCvUnit	space_above;
	_DtCvUnit	space_below;
} _DtCvSpaceMetrics;

typedef	struct	_dtCvLocale {
	_DtCvModeType	 line_wrap_mode;	/* specifies the wrap mode  */
	const wchar_t	*cant_begin_chars;	/* specifies the characters */
						/* that can't begin a line  */
	const wchar_t	*cant_end_chars;	/* specifies the characters */
						/* that can't end a line    */
} _DtCvLocale;

typedef	struct	_dtCvStringInfo {
	const void	*string;
	int		 byte_len;
	int		 wc;
	_DtCvPointer	 font_ptr;
	_DtCvPointer	 csd;
} _DtCvStringInfo;

typedef	struct	_dtCvLineInfo {
	_DtCvUnit	x2;
	_DtCvUnit	y2;
	_DtCvUnit	width;
	_DtCvPointer	data;
} _DtCvLineInfo;

typedef	struct	_dtCvRenderInfo {
	_DtCvPointer	info;
	_DtCvUnit	box_x;
	_DtCvUnit	box_y;
	_DtCvUnit	box_height;
	_DtCvUnit	box_width;
} _DtCvRenderInfo;

typedef	struct	_dtCvLinkInfo {
	char		*specification;
	char		*description;
	int		 hyper_type;
	int		 win_hint;
	_DtCvUnit	 offset_x;
	_DtCvUnit	 offset_y;
} _DtCvLinkInfo;

typedef	struct	_dtCvPointInfo {
	_DtCvPointer	  client_data;
	_DtCvSegPts	**segs;
} _DtCvPointInfo;

/********    Public Structure Typedef Declarations    ********/
typedef	struct _dtCvTopicInfo*	_DtCvTopicPtr;

/********    Public Prototyped Procedures    ********/
typedef	void (*_DtCvGetMetrics)(
    _DtCvPointer		/* client_data   */,
    _DtCvElemType		/* elem_type     */,
    _DtCvPointer 		/* ret_metrics   */
);

typedef	void (*_DtCvRenderElem)(
    _DtCvPointer	/* client_data */,
    _DtCvElemType	/* elem_type   */,
    _DtCvUnit		/* x           */,
    _DtCvUnit		/* y           */,
    int			/* link_type   */, 
    _DtCvFlags		/* old_flags   */,
    _DtCvFlags		/* new_flags   */,
    _DtCvElemType	/* trav_type   */,
    _DtCvPointer	/* trav_data   */,
    _DtCvPointer	/* data        */
);

typedef	_DtCvUnit (*_DtCvGetElemWidth)(
    _DtCvPointer	/* client_data   */,
    _DtCvElemType	/* elem_type   */,
    _DtCvPointer	/* data        */
);

typedef	void (*_DtCvGetFontMetrics)(
    _DtCvPointer	/* client_data   */,
    _DtCvPointer	/* font_ptr      */,
    _DtCvUnit*		/* *ret_ascent   */,
    _DtCvUnit*		/* *ret_descent  */,
    _DtCvUnit*		/* *ret_width    */,
    _DtCvUnit*		/* *ret_super    */,
    _DtCvUnit*		/* *ret_sub      */
);

typedef	_DtCvStatus (*_DtCvBuildSelection)(
    _DtCvPointer	/* client_data   */,
    _DtCvElemType	/* elem_type     */,
    unsigned int	/* mask          */,
    _DtCvPointer*	/* prev_info     */,
    _DtCvUnit		/* space         */,
    _DtCvUnit		/* width         */,
    _DtCvFlags		/* flags         */,
    _DtCvPointer	/* data          */
);

typedef	int (*_DtCvFilterExecCmd)(
    _DtCvPointer	/* client_data   */,
    const char*		/* cmd           */,
    char**		/* ret_cmd       */
);

/********    Public Structures Declarations    ********/
typedef	struct _dtCvVirtualInfo {
	_DtCvGetMetrics		get_metrics;
	_DtCvRenderElem		render_elem;
	_DtCvGetElemWidth	get_width;
	_DtCvGetFontMetrics	get_font_metrics;
	_DtCvBuildSelection	build_selection;
	_DtCvFilterExecCmd	exec_cmd_filter;
} _DtCvVirtualInfo;

typedef	struct	_dtCvMarkPos {
	_DtCvPointer	client_data;
	_DtCvUnit	x1;
	_DtCvUnit	y1;
	_DtCvUnit	baseline1;
	_DtCvUnit	x2;
	_DtCvUnit	y2;
	_DtCvUnit	baseline2;
} _DtCvMarkPos;

/********    Public Macro Declarations    ********/

/********    Semi-Public Function Declarations    ********/
extern int		_DtCvGetSearchLineMetrics(
				_DtCvHandle	canvas_handle,
				int		search_hit_index,
				_DtCvUnit*	baseline,
				_DtCvUnit*	descent,
				_DtCvUnit*	ascent);

/********    Public Function Declarations    ********/
extern	_DtCvStatus	_DtCanvasActivatePts(
				_DtCvHandle	 canvas_handle,
				unsigned int	  mask,
				_DtCvPointInfo	*info,
				_DtCvUnit	 *ret_y1,
				_DtCvUnit	 *ret_y2);
extern	void		_DtCanvasClean(
				_DtCvHandle	 canvas_handle);
extern	_DtCvHandle	_DtCanvasCreate(
				_DtCvVirtualInfo virt_info,
				_DtCvPointer	 client_data);
extern	void		_DtCanvasDestroy(
				_DtCvHandle	 canvas_handle);
extern	_DtCvStatus	_DtCanvasGetCurLink(
				_DtCvHandle	 canvas_handle,
				_DtCvLinkInfo	*ret_info);
extern	_DtCvStatus	_DtCanvasGetCurTraversal(
				_DtCvHandle	 canvas_handle,
				_DtCvLinkInfo	*ret_info,
				_DtCvPointer	*ret_data);
extern	_DtCvStatus	_DtCanvasGetMarkPositions(
				_DtCvHandle	 canvas_handle,
				_DtCvMarkPos	***ret_pos);
extern	_DtCvStatus	_DtCanvasGetPosLink(
				_DtCvHandle	 canvas_handle,
				_DtCvUnit	 x1,
				_DtCvUnit	 y1,
				_DtCvUnit	 x2,
				_DtCvUnit	 y2,
				_DtCvLinkInfo	*ret_info);
extern	_DtCvStatus	_DtCanvasGetSelection(
				_DtCvHandle	 canvas_handle,
				unsigned int	 mask,
				_DtCvPointer	*ret_select);
extern	_DtCvStatus	_DtCanvasGetSelectionPoints(
				_DtCvHandle	   canvas_handle,
				_DtCvSegPts	***ret_segs,
				_DtCvUnit	  *ret_y1,
				_DtCvUnit	  *ret_y2);
extern	_DtCvStatus	_DtCanvasGetSpotInfo(
				_DtCvHandle	   canvas_handle,
				_DtCvUnit	   x,
				_DtCvUnit	   y,
				_DtCvSegment	 **ret_seg,
				_DtCvUnit	  *ret_offx,
				_DtCvUnit	  *ret_offy,
				_DtCvElemType	  *ret_element);
extern	_DtCvStatus	_DtCanvasMoveTraversal(
				_DtCvHandle	 canvas_handle,
				_DtCvTraversalCmd cmd,
				_DtCvValue	 wrap,
				_DtCvValue	 render,
				_DtCvPointer	 rid,
				_DtCvUnit	*ret_x,
				_DtCvUnit	*ret_y,
				_DtCvUnit	*ret_baseline,
				_DtCvUnit	*ret_height);
extern  void            _DtCanvasProcessSelection (
				_DtCvHandle     canvas_handle,
				_DtCvUnit        x,
				_DtCvUnit        y,
				_DtCvSelectMode	 mode);
extern	void		_DtCanvasRender(
				_DtCvHandle	 canvas_handle,
				_DtCvUnit	 x1,
				_DtCvUnit	 y1,
				_DtCvUnit	 x2,
				_DtCvUnit	 y2,
				_DtCvRenderType	 flag,
				_DtCvValue     	 pg_break,
				_DtCvUnit	*max_y,
				_DtCvUnit	*next_y);
extern	_DtCvStatus	_DtCanvasResize(
				_DtCvHandle	 canvas_handle,
				_DtCvValue	 force,
				_DtCvUnit	*ret_width,
				_DtCvUnit	*ret_height);
extern  _DtCvStatus     _DtCanvasSetTopic(
				_DtCvHandle     canvas_handle,
				_DtCvTopicPtr    topic_handle,
				_DtCvValue	 honor_size,
				_DtCvUnit       *ret_width,
				_DtCvUnit       *ret_height,
				_DtCvUnit       *ret_y);
extern	void		_DtCanvasLoadMetrics(
				_DtCvHandle	canvas_handle);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _DtCanvasP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
