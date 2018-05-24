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
/* $XConsortium: CanvasSegP.h /main/19 1996/10/04 18:30:19 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   CanvasSegP.h
 **
 **  Project:
 **
 **  Description:  Public Header file for Canvas Engine
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _DtCanvasSegP_h
#define _DtCanvasSegP_h

#ifdef __cplusplus
extern "C" {
#endif

/********    Public Defines Declarations    ********/
/*****************************************************************************
 *  Some bits in passed in as a _DtCvFlags and the 'type' in _DtCvSegment
 *  must match because they are passed straight thought from the segment
 *  to the application. The following map is to help prevent colisions.
 *
 *   (-------------------------> _DtCvEND_OF_LINE
 *   | (-----------------------> _DtCvSELECTED_FLAG
 *   | |
 *   | | (-------------------------> _DtCvAPP_FLAG4 \
 *   | | | (-----------------------> _DtCvAPP_FLAG3  \
 *   | | | | (---------------------> _DtCvAPP_FLAG2  / _DtCvAPP_FIELD
 *   | | | | | (-------------------> _DtCvAPP_FLAG1 /
 *   | | | | | |  
 *   | | | | | | (---------------------> _DtCvTRAVERSAL_END
 *   | | | | | | | (-------------------> _DtCvTRAVERSAL_BEGIN
 *   | | | | | | | | (-----------------> _DtCvTRAVERSAL_FLAG
 *   | | | | | | | | |
 *   | | | | | | | | | (-------------------> _DtCvMARK_ON
 *   | | | | | | | | | | (-----------------> _DtCvMARK_END
 *   | | | | | | | | | | | (---------------> _DtCvMARK_BEGIN
 *   | | | | | | | | | | | | (-------------> _DtCvMARK_FLAG
 *   | | | | | | | | | | | | |
 *   | | | | | | | | | | | | | (-------------> _DtCvSEARCH_CURR
 *   | | | | | | | | | | | | | | (-----------> _DtCvSEARCH_END
 *   | | | | | | | | | | | | | | | (---------> _DtCvSEARCH_BEGIN
 *   | | | | | | | | | | | | | | | | (-------> _DtCvSEARCH_FLAG
 *   | | | | | | | | | | | | | | | | |
 *   | | | | | | | | | | | | | | | | | (---------> _DtCvLINK_NEW_WINDOW
 *   | | | | | | | | | | | | | | | | | | (-------> _DtCvLINK_POP_UP
 *   | | | | | | | | | | | | | | | | | | | (-----> _DtCvLINK_END
 *   | | | | | | | | | | | | | | | | | | | | (---> _DtCvLINK_BEGIN
 *   | | | | | | | | | | | | | | | | | | | | | (-> _DtCvLINK_FLAG
 *   | | | | | | | | | | | | | | | | | | | | | |
 *   v v v v v v#v v v v v v v v#v v v v v v v v 
 *   1 0 9 8 7 6#5 4 3 2 1 0 9 8#7 6 5 4 3 2 1 0
 *  ------------#---------------#----------------
 *  |.|.|.|.|.|.#.|.|.|.|.|.|.|.#.|.|.|.|.|.|.|.|
 *  ------------#---------------#----------------
 *     ^ ^ ^ ^ ^#^ ^ ^ ^ ^ ^ ^ ^#^ ^ ^   ^ ^^^^^
 *     | | | | | | | | | | | | | | | |   |  \|/
 *     | | | | | | | | | | | | | | | |   |   |
 *     | | | | | | | | | | | | | | | |   |   (->  Segment type
 *     | | | | | | | | | | | | | | | |   (-----> _DtCvWIDE_CHAR
 *     | | | | | | | | | | | | | | | |
 *     | | | | | | | | | | | | | | | (-----> _DtCvSEARCH_FLAG
 *     | | | | | | | | | | | | | | (-------> _DtCvSEARCH_BEGIN
 *     | | | | | | | | | | | | | (---------> _DtCvSEARCH_END
 *     | | | | | | | | | | | | (-----------> _DtCvSEARCH_CURR
 *     | | | | | | | | | | | |
 *     | | | | | | | | | | | (---------> _DtCvCONTROLLER/_DtCvBLOCK_LINE/
 *     | | | | | | | | | | |                         _DtCvNEW_LINE
 *     | | | | | | | | | | (-----------> _DtCvSUB_SCRIPT
 *     | | | | | | | | | (-------------> _DtCvSUPER_SCRIPT
 *     | | | | | | | | (---------------> _DtCvGHOST_LINK
 *     | | | | | | | (-----------------> _DtCvHYPER_TEXT
 *     | | | | | | (-------------------> _DtCvNON_BREAK
 *     | | | | | (---------------------> _DtCvIN_LINE
 *     | | | | |
 *     | | | | (---------------> _DtCvAPP_FLAG1  \
 *     | | | (-----------------> _DtCvAPP_FLAG2   \ _DtCvAPP_FIELD
 *     | | (-------------------> _DtCvAPP_FLAG3   /
 *     | (---------------------> _DtCvAPP_FLAG4  /
 *     |
 *     (------------------> _DtCvPAGE_BREAK
 *
 *****************************************************************************/
/*
 * segment types
 */
#define	_DtCvNOOP	(0x00)
#define	_DtCvCONTAINER	(0x01)
#define	_DtCvLINE	(0x02)
#define	_DtCvMARKER	(0x03)
#define	_DtCvREGION	(0x04)
#define	_DtCvSTRING	(0x05)
#define	_DtCvTABLE	(0x06)

#define	_DtCvPRIMARY_MASK	(0x07)
#define	_DtCvSHIFT_PAST_PRIMARY	3

/*
 * character size manipulation
 */
#define	_DtCvWIDE_CHAR		(0x01 << _DtCvSHIFT_PAST_PRIMARY)

/*
 * segment flags
 */
/* leave a gap for the search flags defined in CanvasP.h */
#define	_DtCvSHIFT_TO_FLAGS	9

/* container only */
#define	_DtCvCONTROLLER		(0x01 << (_DtCvSHIFT_TO_FLAGS +  0))

/* line type */
#define	_DtCvBLOCK_LINE		(0x01 << (_DtCvSHIFT_TO_FLAGS +  0))

/* string & region (_DtCvNEW_LINE works on _DtCvNOOP too) */
#define	_DtCvNEW_LINE		(0x01 << (_DtCvSHIFT_TO_FLAGS +  0))
#define	_DtCvSUB_SCRIPT		(0x01 << (_DtCvSHIFT_TO_FLAGS +  1))
#define	_DtCvSUPER_SCRIPT	(0x01 << (_DtCvSHIFT_TO_FLAGS +  2))
#define	_DtCvGHOST_LINK		(0x01 << (_DtCvSHIFT_TO_FLAGS +  3))
#define	_DtCvHYPER_TEXT		(0x01 << (_DtCvSHIFT_TO_FLAGS +  4))
#define	_DtCvNON_BREAK		(0x01 << (_DtCvSHIFT_TO_FLAGS +  5))

/* region only */
#define	_DtCvIN_LINE		(0x01 << (_DtCvSHIFT_TO_FLAGS +  6))

/* application flags - defined in CanvasP.h (starting at 16th bit for 4 bits) */

/* valid on any segment */
#define	_DtCvPAGE_BREAK		(0x01 << (_DtCvSHIFT_TO_FLAGS + 11))

/* highlight macros */
#define HILITE_OVERLINE			0
#define HILITE_STRIKETHROUGH		1
#define HILITE_UNDERLINE		2

/********    Public Enum Declarations    ********/
enum	_dtCvFrmtOption
  {
    _DtCvOPTION_BAD,
/*
 * string breaking types
 */
    _DtCvLITERAL,
    _DtCvDYNAMIC,
/*
 * table border types
 */
    _DtCvBORDER_NONE,
    _DtCvBORDER_FULL,
    _DtCvBORDER_HORZ,
    _DtCvBORDER_VERT,
    _DtCvBORDER_TOP,
    _DtCvBORDER_BOTTOM,
    _DtCvBORDER_LEFT,
    _DtCvBORDER_RIGHT,
    _DtCvBORDER_TOP_LEFT,
    _DtCvBORDER_TOP_RIGHT,
    _DtCvBORDER_BOTTOM_LEFT,
    _DtCvBORDER_BOTTOM_RIGHT,
/*
 * controller horizontal orientation
 */
    _DtCvJUSTIFY_LEFT_CORNER,
    _DtCvJUSTIFY_LEFT,		/* also container horizontal text placement */
    _DtCvJUSTIFY_LEFT_MARGIN,
    _DtCvJUSTIFY_CENTER,	/* also container horiz/vert text placement */
    _DtCvJUSTIFY_RIGHT_MARGIN,
    _DtCvJUSTIFY_RIGHT,		/* also container horizontal text placement */
    _DtCvJUSTIFY_RIGHT_CORNER,
/*
 * extra container horizontal text placement
 */
    _DtCvJUSTIFY_NUM,
    _DtCvJUSTIFY_CHAR,
/*
 * inherit the parent's horizontal text placement
 */
    _DtCvINHERIT,
/*
 * controller vertical orientation, includes _DtCvJUSTIFY_CENTER
 */
    _DtCvJUSTIFY_TOP,		/* also container vertical text placement */
    _DtCvJUSTIFY_BOTTOM,	/* also container vertical text placement */
/*
 * controller flow types
 */
    _DtCvWRAP,
    _DtCvWRAP_NONE,
    _DtCvWRAP_JOIN
  };

typedef enum {
    hilite_overline      = 0x01 << HILITE_OVERLINE,
    hilite_strikethrough = 0x01 << HILITE_STRIKETHROUGH,
    hilite_underline     = 0x01 << HILITE_UNDERLINE
} text_hilite_t;

/********    Public Enum Typedef Declarations    ********/
typedef	enum   _dtCvFrmtOption	_DtCvFrmtOption;

/********    Public Structures Declarations    ********/
typedef	struct	_dtCvLine {
	_DtCvUnit	width;
	_DtCvPointer	data;
} _DtCvLine;

typedef struct  _dtCvContainer {
	char            *id;
	char            *justify_char;
	_DtCvFrmtOption  type;
	_DtCvFrmtOption  border;
	_DtCvFrmtOption  justify;
	_DtCvFrmtOption  vjustify;
	_DtCvFrmtOption  orient;
	_DtCvFrmtOption  vorient;
	_DtCvFrmtOption  flow;
	int              percent;
	_DtCvUnit        leading;
	_DtCvUnit        fmargin;
	_DtCvUnit        lmargin;
	_DtCvUnit        rmargin;
	_DtCvUnit        tmargin;
	_DtCvUnit        bmargin;
	_DtCvLine	 bdr_info;
        struct _dtCvSegment    *seg_list;
} _DtCvContainer;

typedef struct  _DtCvString {
	void            *string;
	_DtCvPointer     font;
} _DtCvString;

typedef struct  _dtCvRegion {
	_DtCvPointer    info;
	_DtCvUnit       width;
	_DtCvUnit       height;
	_DtCvUnit       ascent;
} _DtCvRegion;

typedef struct  _dtCvTable {
	int			  num_cols;
	char			**col_w;
	_DtCvFrmtOption		 *col_justify;
	char			 *justify_chars;
	char			**cell_ids;
	struct _dtCvSegment	**cells;
} _DtCvTable;

typedef union   _dtCvSegHandles {
	_DtCvContainer	 container;
	_DtCvString	 string;
	_DtCvRegion	 region;
	_DtCvTable	 table;
	_DtCvLine	 rule;
	char		*marker;
} _DtCvSegHandles;

struct	_dtCvSegment {
	unsigned long		 type;
	int			 link_idx;
	_DtCvSegHandles		 handle;
	struct _dtCvSegment	*next_seg;
	struct _dtCvSegment	*next_disp;
	_DtCvPointer		 client_use;
	_DtCvPointer		 internal_use;
};

struct	_dtCvSegPts {
	struct _dtCvSegment	*segment;
	int			 offset;
	int			 len;
};

typedef struct	_dtCvTopicInfo {
	char			 *id_str;
	struct _dtCvSegment	 *seg_list;
	_DtCvPointInfo		**mark_list;
        struct _dtCvLinkDb       *link_data;
} _DtCvTopicInfo;

typedef struct _dtCvStringClientData {
	unsigned int vcc;
	unsigned int vclen;

	unsigned int hilite_type;

	char* bg_color;
	char* fg_color;
	unsigned long bg_pixel;
	unsigned long fg_pixel;
} _DtCvStringClientData;

typedef struct _dtCvRegionClientData {
	_DtCvPointer GraphicHandle;
} _DtCvRegionClientData;

typedef union _dtCvClientData {
	_DtCvRegionClientData region;
	_DtCvStringClientData string;
} _DtCvClientData;

/********    Public Structure Typedef Declarations    ********/

/********    Public Prototyped Procedures    ********/

/********    Public Macro Declarations    ********/
/*
 * set segment type
 */
#define	_DtCvSetTypeToContainer(x) \
			(((x) & ~(_DtCvPRIMARY_MASK)) | _DtCvCONTAINER)
#define	_DtCvSetTypeToLine(x) \
			(((x) & ~(_DtCvPRIMARY_MASK)) | _DtCvLINE)
#define	_DtCvSetTypeToMarker(x) \
			(((x) & ~(_DtCvPRIMARY_MASK)) | _DtCvMARKER)
#define	_DtCvSetTypeToNoop(x) \
			(((x) & ~(_DtCvPRIMARY_MASK)) | _DtCvNOOP)
#define	_DtCvSetTypeToRegion(x) \
			(((x) & ~(_DtCvPRIMARY_MASK)) | _DtCvREGION)
#define	_DtCvSetTypeToString(x) \
			(((x) & ~(_DtCvPRIMARY_MASK)) | _DtCvSTRING)
#define	_DtCvSetTypeToTable(x) \
			(((x) & ~(_DtCvPRIMARY_MASK)) | _DtCvTABLE)
/*
 * set segment flags
 */
#define	_DtCvSetTypeToBlockLine(x)	((x) | _DtCvBLOCK_LINE)
#define	_DtCvSetTypeToController(x)	((x) | _DtCvCONTROLLER)
#define	_DtCvSetTypeToGhostLink(x)	((x) | _DtCvGHOST_LINK)
#define	_DtCvSetTypeToHyperText(x)	((x) | _DtCvHYPER_TEXT)
#define	_DtCvSetTypeToInLine(x)		((x) | _DtCvIN_LINE)
#define	_DtCvSetTypeToNewLine(x)	((x) | _DtCvNEW_LINE)
#define	_DtCvSetTypeToNonBreak(x)	((x) | _DtCvNON_BREAK)
#define	_DtCvSetTypeToSearch(x)		((x) | _DtCvSEARCH_FLAG)
#define	_DtCvSetTypeToSearchBegin(x)	((x) | _DtCvSEARCH_BEGIN)
#define	_DtCvSetTypeToSearchEnd(x)	((x) | _DtCvSEARCH_END)
#define	_DtCvSetTypeToSubScript(x)	((x) | _DtCvSUB_SCRIPT)
#define	_DtCvSetTypeToSuperScript(x)	((x) | _DtCvSUPER_SCRIPT)

/*
 * set character size
 */
#define	_DtCvSetTypeToWideChar(x)	((x) | _DtCvWIDE_CHAR)

/*
 * set application flags and/or field
 */
#define	_DtCvSetAppFlag1(x)		((x) | _DtCvAPP_FLAG1)
#define	_DtCvSetAppFlag2(x)		((x) | _DtCvAPP_FLAG2)
#define	_DtCvSetAppFlag3(x)		((x) | _DtCvAPP_FLAG3)
#define	_DtCvSetAppFlag4(x)		((x) | _DtCvAPP_FLAG4)
#define	_DtCvSetAppField(x,y) \
		(((x) & ~(_DtCvAPP_FIELD_MASK)) | \
			((y & _DtCvAPP_FIELD_MASK) << _DtCvAPP_FIELD_OFFSET))
#define	_DtCvSetAppFieldSmall(x,y) \
		(((x) & ~(_DtCvAPP_FIELD_S_MASK)) | \
			((y & _DtCvAPP_FIELD_S_MASK) << _DtCvAPP_FIELD_OFFSET))

/*
 * get the application field value
 */
#define	_DtCvGetAppField(x) \
			(((x) & _DtCvAPP_FIELD_MASK) >> _DtCvAPP_FIELD_OFFSET)
#define	_DtCvGetAppFieldSmall(x) \
			(((x) & _DtCvAPP_FIELD_S_MASK) >> _DtCvAPP_FIELD_OFFSET)

/********    Semi-Public Macro Declarations    ********/
/*
 * get segment type
 */
#define _DtCvPrimaryTypeOfSeg(x)        ((x->type) & _DtCvPRIMARY_MASK)
#define _DtCvIsSegContainer(x) \
		(((((x)->type) & _DtCvPRIMARY_MASK) == _DtCvCONTAINER) ? 1 : 0)
#define _DtCvIsSegLine(x) \
		(((((x)->type) & _DtCvPRIMARY_MASK) == _DtCvLINE) ? 1 : 0)
#define _DtCvIsSegMarker(x) \
		(((((x)->type) & _DtCvPRIMARY_MASK) == _DtCvMARKER) ? 1 : 0)
#define _DtCvIsSegNoop(x) \
		(((((x)->type) & _DtCvPRIMARY_MASK) == _DtCvNOOP) ? 1 : 0)
#define _DtCvIsSegRegion(x) \
		(((((x)->type) & _DtCvPRIMARY_MASK) == _DtCvREGION) ? 1 : 0)
#define _DtCvIsSegString(x) \
		(((((x)->type) & _DtCvPRIMARY_MASK) == _DtCvSTRING) ? 1 : 0)
#define _DtCvIsSegTable(x) \
		(((((x)->type) & _DtCvPRIMARY_MASK) == _DtCvTABLE) ? 1 : 0)

/*
 * get segment flags
 */
#define _DtCvIsSegController(x) \
				((((x)->type) & _DtCvCONTROLLER) ? 1 : 0)
#define _DtCvIsSegBlockLine(x) \
				((((x)->type) & _DtCvBLOCK_LINE) ? 1 : 0)
#define _DtCvIsSegGhostLink(x) \
				((((x)->type) & _DtCvGHOST_LINK) ? 1 : 0)
#define _DtCvIsSegHyperText(x) \
				((((x)->type) & _DtCvHYPER_TEXT) ? 1 : 0)
#define _DtCvIsSegInLine(x) \
				((((x)->type) & _DtCvIN_LINE) ? 1 : 0)
#define _DtCvIsSegNewLine(x) \
				((((x)->type) & _DtCvNEW_LINE) ? 1 : 0)
#define _DtCvIsSegNonBreakingChar(x) \
				((((x)->type) & _DtCvNON_BREAK) ? 1 : 0)
#define _DtCvIsSegPageBreak(x) \
				((((x)->type) & _DtCvPAGE_BREAK) ? 1 : 0)
#define	_DtCvIsSegRegChar(x) \
				((((x)->type) & _DtCvWIDE_CHAR) ? 0 : 1)
#define _DtCvIsSegSubScript(x) \
				((((x)->type) & _DtCvSUB_SCRIPT) ? 1 : 0)
#define _DtCvIsSegSuperScript(x) \
				((((x)->type) & _DtCvSUPER_SCRIPT) ? 1 : 0)
#define	_DtCvIsSegWideChar(x) \
				((((x)->type) & _DtCvWIDE_CHAR) ? 1 : 0)
/*
 * segment access
 */
#define	_DtCvNextSeg(x)		((x)->next_seg)
#define	_DtCvNextDisp(x)	((x)->next_disp)

/*
 * container access
 */
#define	_DtCvContainerOfSeg(x) \
				((x)->handle.container)
#define	_DtCvContainerBMarginOfSeg(x) \
				((x)->handle.container.bmargin)
#define	_DtCvContainerBorderOfSeg(x) \
				((x)->handle.container.border)
#define	_DtCvContainerFlowOfSeg(x) \
				((x)->handle.container.flow)
#define	_DtCvContainerFMarginOfSeg(x) \
				((x)->handle.container.fmargin)
#define	_DtCvContainerIdOfSeg(x) \
				((x)->handle.container.id)
#define	_DtCvContainerJustifyCharOfSeg(x) \
				((x)->handle.container.justify_char)
#define	_DtCvContainerJustifyOfSeg(x) \
				((x)->handle.container.justify)
#define	_DtCvContainerLeadingOfSeg(x) \
				((x)->handle.container.leading)
#define	_DtCvContainerLineWidthOfSeg(x) \
				((x)->handle.container.bdr_info.width)
#define	_DtCvContainerLineDataOfSeg(x) \
				((x)->handle.container.bdr_info.data)
#define	_DtCvContainerListOfSeg(x) \
				((x)->handle.container.seg_list)
#define	_DtCvContainerLMarginOfSeg(x) \
				((x)->handle.container.lmargin)
#define	_DtCvContainerOrientOfSeg(x) \
				((x)->handle.container.orient)
#define	_DtCvContainerPercentOfSeg(x) \
				((x)->handle.container.percent)
#define	_DtCvContainerRMarginOfSeg(x) \
				((x)->handle.container.rmargin)
#define	_DtCvContainerTMarginOfSeg(x) \
				((x)->handle.container.tmargin)
#define	_DtCvContainerTypeOfSeg(x) \
				((x)->handle.container.type)
#define	_DtCvContainerVJustifyOfSeg(x) \
				((x)->handle.container.vjustify)
#define	_DtCvContainerVOrientOfSeg(x) \
				((x)->handle.container.vorient)

/*
 * marker access
 */
#define	_DtCvIdOfMarkerSeg(x)		((x)->handle.marker)

/*
 * marker access
 */
#define	_DtCvDataOfLineSeg(x)		((x)->handle.rule.data)
#define	_DtCvWidthOfLineSeg(x)		((x)->handle.rule.width)

/*
 * string access
 */
#define _DtCvStringOfStringSeg(x)	((x)->handle.string.string)
#define _DtCvFontOfStringSeg(x)		((x)->handle.string.font)

/*
 * region access
 */
#define _DtCvInfoOfRegionSeg(x)		((x)->handle.region.info)
#define _DtCvWidthOfRegionSeg(x)	((x)->handle.region.width)
#define _DtCvHeightOfRegionSeg(x)	((x)->handle.region.height)
#define _DtCvAscentOfRegionSeg(x)	((x)->handle.region.ascent)

/*
 * table access
 */
#define	_DtCvNumColsOfTableSeg(x)	((x)->handle.table.num_cols)
#define	_DtCvColWOfTableSeg(x)		((x)->handle.table.col_w)
#define	_DtCvColJustifyOfTableSeg(x)	((x)->handle.table.col_justify)
#define	_DtCvJustifyCharsOfTableSeg(x)	((x)->handle.table.justify_chars)
#define	_DtCvCellIdsOfTableSeg(x)	((x)->handle.table.cell_ids)
#define	_DtCvCellsOfTableSeg(x)		((x)->handle.table.cells)

/********    Semi-Public Function Declarations    ********/
/********    Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtCanvasSegP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
