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
/* $XConsortium: SDLI.h /main/7 1995/11/28 14:24:36 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   SDLI.h
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
 **
 *******************************************************************
 *************************************<+>*************************************/
#ifndef _DtHelpSDLI_h
#define _DtHelpSDLI_h


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * SDL DTD versioning defines
 *****************************************************************************/
/*
 * The first version
 */
#define	SDL_DTD_VERSION		1

/*
 * The first minor version
 */
#define SDL_DTD_1_0		0

/*
 * Virpage levels went from 1-n.
 * Any virpage with a level of
 * zero (or less) are hidden.
 */
#define SDL_DTD_1_1		1

/*****************************************************************************
 * Attribute type enumerations
 *****************************************************************************/
enum	SdlAttrDataType {
/* bad data type */
	SdlAttrDataTypeInvalid,
/* valid data types */
	SdlAttrDataTypeId  , SdlAttrDataTypeNumber, SdlAttrDataTypeCdata,
	SdlAttrDataTypeFont, SdlAttrDataTypeEnum };

/*****************************************************************************
 * Option enumerations
 *****************************************************************************/
enum	sdlOption
  {
/*
 * all _DtCv enums are from the canvas engine
 */
/* bad option indicator */
	SdlOptionBad = _DtCvOPTION_BAD,
/* string breakng types */
	SdlLiteral   = _DtCvLITERAL,
	SdlDynamic   = _DtCvDYNAMIC,
/* table border types */
	SdlBorderNone   = _DtCvBORDER_NONE,
	SdlBorderFull   = _DtCvBORDER_FULL,
	SdlBorderHorz   = _DtCvBORDER_HORZ,
	SdlBorderVert   = _DtCvBORDER_VERT,
	SdlBorderTop    = _DtCvBORDER_TOP,
	SdlBorderBottom = _DtCvBORDER_BOTTOM,
	SdlBorderLeft   = _DtCvBORDER_LEFT,
	SdlBorderRight  = _DtCvBORDER_RIGHT,
/* horizontal orientation */
	SdlJustifyLeftCorner  = _DtCvJUSTIFY_LEFT_CORNER,
	SdlJustifyLeft        = _DtCvJUSTIFY_LEFT,
	SdlJustifyLeftMargin  = _DtCvJUSTIFY_LEFT_MARGIN,
	SdlJustifyCenter      = _DtCvJUSTIFY_CENTER,
	SdlJustifyRightMargin = _DtCvJUSTIFY_RIGHT_MARGIN,
	SdlJustifyRight       = _DtCvJUSTIFY_RIGHT,
	SdlJustifyRightCorner = _DtCvJUSTIFY_RIGHT_CORNER,
/* extra horizontal text placement */
	SdlJustifyNum         = _DtCvJUSTIFY_NUM,
/* extra vertical orientation */
	SdlJustifyTop         = _DtCvJUSTIFY_TOP,
	SdlJustifyBottom      = _DtCvJUSTIFY_BOTTOM,
/* flow types */
	SdlWrap     = _DtCvWRAP,
	SdlNoWrap   = _DtCvWRAP_NONE,
	SdlWrapJoin = _DtCvWRAP_JOIN,
/* SDL specific options */
/* Boolean */
	SdlBoolYes      , SdlBoolNo     ,

/* key classes */
	SdlClassAcro    , SdlClassBook    , SdlClassEmph    , SdlClassJargon ,
	SdlClassL10n    , SdlClassName    , SdlClassQuote   , SdlClassProduct,
	SdlClassTermdef , SdlClassTerm    , SdlClassMachIn  , SdlClassMachOut,
	SdlClassMachCont, SdlClassPubLit  , SdlClassUdefkey ,

/* head classes */
	SdlClassLabel     , SdlClassHead , SdlClassCaption ,
	SdlClassAnnotation, SdlClassPHead, SdlClassUdefhead,

/* format classes */
	SdlClassTable   , SdlClassCell    , SdlClassList,
	SdlClassItem    , SdlClassText    , SdlClassUdeffrmt,

/* graphic classes */
	SdlClassFigure, SdlClassInLine  , SdlClassButton  ,
	SdlClassIcon  , SdlClassUdefgraph,

/* phrase styles */
	SdlClassSuper   , SdlClassSub     ,

/* apps types */
	SdlAppAll       , SdlAppHelp      , SdlAppTutorial  ,
	SdlAppRef       , SdlAppSys       ,

/* timing */
	SdlTimingSync   , SdlTimingAsync  ,

/* types */
	SdlTypeLiteral  , SdlTypeLined    ,
	SdlTypeDynamic  , SdlTypeUnlinedLiteral,
	SdlTypeCdata    ,

/* links */
	SdlWindowCurrent , SdlWindowNew, SdlWindowPopup,
/* traversal */
	SdlTraversalReturn, SdlTraversalNoReturn,
/* <id> types */
	SdlIdVirpage , SdlIdBlock   , SdlIdForm    , SdlIdPara    ,
	SdlIdCPara   , SdlIdHead    , SdlIdSubHead , SdlIdAnchor  ,
	SdlIdSwitch  , SdlIdSnRef   , SdlIdGraphic , SdlIdText    ,
	SdlIdAudio   , SdlIdVideo   , SdlIdAnimate ,
	SdlIdCrossDoc, SdlIdManPage , SdlIdTextFile,
	SdlIdSysCmd  , SdlIdScript  , SdlIdCallback,
/* <script> interpreter types */
	SdlInterpKsh, SdlInterpTcl, SdlInterpCsh, SdlInterpSh,
/* <headstyle> placement types */
	SdlPlaceParent, SdlPlaceObject,
/* <headstyle> stack types */
	SdlStackHoriz , SdlStackVert
    };

typedef	enum sdlOption SdlOption;

/*****************************************************************************
 * SDL Element enumerations
 *****************************************************************************/
enum	SdlElement
    {
	SdlElementNone,
	SdlElementComment,
	SdlElementSdlDoc,
	SdlElementVStruct,
	SdlElementVirpage,
	SdlElementHead,
	SdlElementSubHead,
	SdlElementSnb,
	SdlElementBlock,
	SdlElementForm,
	SdlElementPara,
	SdlElementCPara,
	SdlElementFdata,
	SdlElementFstyle,
	SdlElementFrowvec,
	SdlElementKey,
	SdlElementCdata,
/* System notations */
	SdlElementGraphic,
	SdlElementText,
	SdlElementAudio,
	SdlElementVideo,
	SdlElementAnimate,
	SdlElementCrossDoc,
	SdlElementManPage,
	SdlElementTextFile,
	SdlElementSysCmd,
	SdlElementCallback,
	SdlElementScript,
/* System notations (end) */
	SdlElementAnchor,
	SdlElementLink,
	SdlElementSwitch,
	SdlElementSnRef,
	SdlElementRefItem,
	SdlElementAltText,
	SdlElementSphrase,
	SdlElementRev,
	SdlElementSpc,
/* Conditionals */
	SdlElementIf,
	SdlElementCond,
	SdlElementThen,
	SdlElementElse,
/* Conditionals (end) */
/* The sgml header to the document */
	SdlElementDocument,
	SdlElementSgml,
	SdlElementDocType,
/* Generated elements in the document */
	SdlElementLoids,
	SdlElementToss,
	SdlElementLoPhrases,
	SdlElementPhrase,
	SdlElementIndex,
	SdlElementEntry,
	SdlElementRelDocs,
	SdlElementRelFile,
	SdlElementNotes,
/* Generated elements (end) */
/* Table of styles and sematics. */
	SdlElementKeyStyle,
	SdlElementHeadStyle,
	SdlElementFormStyle,
	SdlElementFrmtStyle,
	SdlElementGrphStyle,
/* Table of styles and sematics.(end) */
	SdlElementId,
/* Support asyncronous blocks */
	SdlElementBlockAsync,
/* Support getting the title of a virpage */
	SdlElementTitle,
/* Support content that has optional markup followed by PCDATA */
	SdlPcDataFollows
    };

/********    Private Defines Declarations    ********/
/* sort of enumerated types */
#define	ENUM_VALUE	0x01
#define	NUMBER_VALUE	0x02
#define	STRING1_VALUE	0x03
#define	STRING2_VALUE	0x04

#define	VALUE_MASK	0x07

#define	VALUE_TYPE_SHIFT	3
#define	MINOR_MASK	(0x07 << VALUE_TYPE_SHIFT)

#define	SET_ENUM_ATTR(x)	((x) << VALUE_TYPE_SHIFT | ENUM_VALUE)
#define	SET_NUMBER_ATTR(x)	((x) << VALUE_TYPE_SHIFT | NUMBER_VALUE)
#define	SET_STRING_ATTR(x)	((x) << VALUE_TYPE_SHIFT | STRING1_VALUE)
#define	SET_STRING2_ATTR(x)	((x) << VALUE_TYPE_SHIFT | STRING2_VALUE)

#define	ATTRS_EQUAL(type, x, y)	\
		(((type) != SdlAttrDataTypeEnum) ? \
			((x) == (y) ? 1 : 0) : \
		(((((x) & VALUE_MASK) == ((y) & VALUE_MASK)) && \
		((x) & (y) & ~(VALUE_MASK))) ? 1 : 0))

#define	SDLIsString1(x)		((((x) & VALUE_MASK) == STRING1_VALUE) ? 1 : 0)
#define	SDLIsString2(x)		((((x) & VALUE_MASK) == STRING2_VALUE) ? 1 : 0)

#define	SDLIsStrAttrSet(pa, attr) \
	(((((attr) & VALUE_MASK)==STRING1_VALUE) ? ((pa).str1_values & attr) : \
		((((attr) & VALUE_MASK)==STRING2_VALUE) ? \
			((pa).str2_values & attr) : 0)) & (~VALUE_MASK))

#define	SDLIsAttrSet(pa, attr) \
	(((((attr) & VALUE_MASK)==ENUM_VALUE) ? ((pa).enum_values & attr) : \
	 ((((attr) & VALUE_MASK)==NUMBER_VALUE) ? ((pa).num_values & attr) : \
	 ((((attr) & VALUE_MASK)==STRING1_VALUE) ? ((pa).str1_values & attr) : \
				((pa).str2_values & attr)))) & (~VALUE_MASK))

#define  SDLAttrFlagType(x)      ((x) & VALUE_MASK)

/************************************************************************
 * Enum types
 ************************************************************************/
/*
 * this set has the same attribute 'class',
 * but each has a different set of values.
 */
#define	SDL_ATTR_CLASS		SET_ENUM_ATTR(1 << 3)
#define	SDL_ATTR_CLASSALL	(SDL_ATTR_CLASS)
#define	SDL_ATTR_CLASSH		SET_ENUM_ATTR((1 << 3) | 0x01)
#define	SDL_ATTR_CLASSF		SET_ENUM_ATTR((1 << 3) | 0x02)
#define	SDL_ATTR_CLASSK		SET_ENUM_ATTR((1 << 3) | 0x03)
#define	SDL_ATTR_CLASSG		SET_ENUM_ATTR((1 << 3) | 0x04)
#define	SDL_ATTR_CLASSFG	SET_ENUM_ATTR((1 << 3) | 0x05)
#define	SDL_ATTR_CLASSP		SET_ENUM_ATTR((1 << 3) | 0x06)

/*
 * this set has the same attribute 'type',
 * but each has a different set of values.
 */
#define	SDL_ATTR_TYPE		SET_ENUM_ATTR((1 << 4) | 0x00)
#define	SDL_ATTR_TYPEFRMT	SET_ENUM_ATTR((1 << 4) | 0x01)
#define	SDL_ATTR_TYPEID		SET_ENUM_ATTR((1 << 4) | 0x02)

#define	SDL_ATTR_APP		SET_ENUM_ATTR(1 << 5)
#define	SDL_ATTR_TIMING		SET_ENUM_ATTR(1 << 6)
#define	SDL_ATTR_HDR		SET_ENUM_ATTR(1 << 7)

/* Format Style Attributes */
#define	SDL_ATTR_BORDER		SET_ENUM_ATTR(1 << 8)
#define	SDL_ATTR_JUSTIFY	SET_ENUM_ATTR((1 << 9) | 0x00)
#define	SDL_ATTR_JUSTIFY1	SET_ENUM_ATTR((1 << 9) | 0x01)
#define	SDL_ATTR_JUSTIFY2	SET_ENUM_ATTR((1 << 9) | 0x02)
#define	SDL_ATTR_VJUST		SET_ENUM_ATTR(1 << 10)

/* Head Style Attributes */
#define	SDL_ATTR_ORIENT		SET_ENUM_ATTR(1 << 11)
#define	SDL_ATTR_VORIENT	SET_ENUM_ATTR(1 << 12)
#define	SDL_ATTR_FLOW		SET_ENUM_ATTR(1 << 13)
#define	SDL_ATTR_WINDOW		SET_ENUM_ATTR(1 << 14)
#define	SDL_ATTR_PLACEMENT	SET_ENUM_ATTR(1 << 15)
#define	SDL_ATTR_STACK		SET_ENUM_ATTR(1 << 16)

/* Link Attribute */
#define	SDL_ATTR_TRAVERSAL	SET_ENUM_ATTR(1 << 17)

/* Font Style Attributes */
#define	SDL_ATTR_STYLE		SET_ENUM_ATTR(1 << 18)
#define	SDL_ATTR_SPACING	SET_ENUM_ATTR(1 << 19)
#define	SDL_ATTR_WEIGHT		SET_ENUM_ATTR(1 << 20)
#define	SDL_ATTR_SLANT		SET_ENUM_ATTR(1 << 21)
#define	SDL_ATTR_SPECIAL	SET_ENUM_ATTR(1 << 22)
#define	SDL_ATTR_INTERP 	SET_ENUM_ATTR(1 << 23)

/************************************************************************
 * number types
 ************************************************************************/
/*
 * LEVEL and RLEVEL share the same data element in the structure
 */
#define	SDL_ATTR_LEVEL		 SET_NUMBER_ATTR(1 << 3)
#define	SDL_ATTR_RLEVEL		 SET_NUMBER_ATTR(1 << 4)

#define	SDL_ATTR_LENGTH		 SET_NUMBER_ATTR(1 << 5)
#define	SDL_ATTR_NCOLS		 SET_NUMBER_ATTR(1 << 6)
#define	SDL_ATTR_COUNT		 SET_NUMBER_ATTR(1 << 7)

/* Font Style Attributes */
#define	SDL_ATTR_POINTSZ	 SET_NUMBER_ATTR(1 << 8)
#define	SDL_ATTR_SETWIDTH	 SET_NUMBER_ATTR(1 << 9)

/* Format Style Attributes */
#define	SDL_ATTR_FMARGIN	 SET_NUMBER_ATTR(1 << 10)
#define	SDL_ATTR_LMARGIN	 SET_NUMBER_ATTR(1 << 11)
#define	SDL_ATTR_RMARGIN	 SET_NUMBER_ATTR(1 << 12)
#define	SDL_ATTR_TMARGIN	 SET_NUMBER_ATTR(1 << 13)
#define	SDL_ATTR_BMARGIN	 SET_NUMBER_ATTR(1 << 14)
#define	SDL_ATTR_SRCHWT		 SET_NUMBER_ATTR(1 << 15)
#define	SDL_ATTR_OFFSET		 SET_NUMBER_ATTR(1 << 16)

/* Head Style Attributes */
#define	SDL_ATTR_HEADWDTH	 SET_NUMBER_ATTR(1 << 17)

/************************************************************************
 * string1 types
 ************************************************************************/
#define	SDL_ATTR_VERSION	SET_STRING_ATTR(1 << 0)
#define	SDL_ATTR_LANGUAGE	SET_STRING_ATTR(1 << 1)
#define	SDL_ATTR_DOCID		SET_STRING_ATTR(1 << 2)

/* style attr */
#define	SDL_ATTR_PHRASE		SET_STRING_ATTR(1 << 3)

/* refitem */
#define	SDL_ATTR_BUTTON		SET_STRING_ATTR(1 << 4)

/* link */
#define	SDL_ATTR_DESCRIPT	SET_STRING_ATTR(1 << 5)
#define	SDL_ATTR_LINKINFO	SET_STRING_ATTR(1 << 6)

/* Font Style Attributes */
#define	SDL_ATTR_COLOR		SET_STRING_ATTR(1 <<  7)
#define	SDL_ATTR_XLFD		SET_STRING_ATTR(1 <<  8)
#define	SDL_ATTR_XLFDI		SET_STRING_ATTR(1 <<  9)
#define	SDL_ATTR_XLFDB		SET_STRING_ATTR(1 << 10)
#define	SDL_ATTR_XLFDIB		SET_STRING_ATTR(1 << 11)
#define	SDL_ATTR_TYPENAM	SET_STRING_ATTR(1 << 12)
#define	SDL_ATTR_TYPENAMI	SET_STRING_ATTR(1 << 13)
#define	SDL_ATTR_TYPENAMB	SET_STRING_ATTR(1 << 14)
#define	SDL_ATTR_TYPENAMIB	SET_STRING_ATTR(1 << 15)

/*
 * start share of element 'id'.
 */
#define	SDL_ATTR_ID		SET_STRING_ATTR(1 << 16)
#define	SDL_ATTR_NAME  		SET_STRING_ATTR(1 << 17)
#define	SDL_ATTR_RID		SET_STRING_ATTR(1 << 18)
#define	SDL_ATTR_CELLS		SET_STRING_ATTR(1 << 19)
#define	SDL_ATTR_FRST_PG	SET_STRING_ATTR(1 << 20)

/*
 * SSI and RSSI share the same element 'ssi'.
 */
#define	SDL_ATTR_SSI		SET_STRING_ATTR(1 << 21)
#define	SDL_ATTR_RSSI		SET_STRING_ATTR(1 << 22)

/************************************************************************
 * string2 types
 ************************************************************************/
/*
 * Group A, B, C, and D all share the same data space
 *
 * Group A.
 */
#define	SDL_ATTR_CHARSET	SET_STRING2_ATTR(1 <<  0)
#define	SDL_ATTR_COLW		SET_STRING2_ATTR(1 <<  1)
#define	SDL_ATTR_COLJ		SET_STRING2_ATTR(1 <<  2)
#define	SDL_ATTR_TEXT		SET_STRING2_ATTR(1 <<  3)
#define	SDL_ATTR_BRANCHES	SET_STRING2_ATTR(1 <<  4)
#define	SDL_ATTR_ABBREV		SET_STRING2_ATTR(1 <<  5)
#define	SDL_ATTR_ENTER		SET_STRING2_ATTR(1 <<  6)
#define	SDL_ATTR_EXIT		SET_STRING2_ATTR(1 <<  7)

/*
 * Group B.
 */
#define	SDL_ATTR_PRODUCT	SET_STRING2_ATTR(1 <<  8)
#define	SDL_ATTR_LICENSE	SET_STRING2_ATTR(1 <<  9)
#define	SDL_ATTR_TIMESTAMP	SET_STRING2_ATTR(1 << 10)
#define	SDL_ATTR_PRODPN		SET_STRING2_ATTR(1 << 11)
#define	SDL_ATTR_PRODVER	SET_STRING2_ATTR(1 << 12)
#define	SDL_ATTR_AUTHOR 	SET_STRING2_ATTR(1 << 13)
#define	SDL_ATTR_SDLDTD 	SET_STRING2_ATTR(1 << 14)
#define	SDL_ATTR_SRCDTD 	SET_STRING2_ATTR(1 << 15)
#define	SDL_ATTR_PUBID		SET_STRING2_ATTR(1 << 16)

/*
 * Group C.
 * the group uses the SDL_ATTR_PRODUCT & SDL_ATTR_LICENSE
 */
#define	SDL_ATTR_FORMAT		SET_STRING2_ATTR(1 << 17)
#define	SDL_ATTR_METHOD		SET_STRING2_ATTR(1 << 18)
#define	SDL_ATTR_XID		SET_STRING2_ATTR(1 << 19)
#define	SDL_ATTR_COMMAND	SET_STRING2_ATTR(1 << 20)
#define	SDL_ATTR_DATA		SET_STRING2_ATTR(1 << 21)

/*
 * Group D.
 * Used for <entry>
 */
#define	SDL_ATTR_MAIN		SET_STRING2_ATTR(1 << 22)
#define	SDL_ATTR_LOCS		SET_STRING2_ATTR(1 << 23)
#define	SDL_ATTR_SYNS		SET_STRING2_ATTR(1 << 24)
#define	SDL_ATTR_SORT		SET_STRING2_ATTR(1 << 25)

/* Attribute Value defines */
enum	SdlAttrValue
  {
	SdlAttrValueBad,
	SdlAttrValueImplied,
	SdlAttrValueRequired,
	SdlAttrValueImpliedDef
  };

/* Segment types */
#define	SDL_ELEMENT	CE_ELEMENT
#define	SDL_STRING	CE_STRING
#define	SDL_GRAPHIC	CE_GRAPHIC

#define	SDL_ELEMENT_SHIFT	CE_SHIFT_PAST_CHAR_SIZE
#define	SDL_ELEMENT_MASK	(0xfffff  << SDL_ELEMENT_SHIFT)

/*
 * this is the maximum number of bits in an unsigned long value.
 * This is for the inclusion and exclustion masks.
 */
/* On Hewlett-Packard Machines! */
#define	SDL_MASK_SIZE	32
#define	SDL_MASK_LEN	 2

/********    Private Macro Declarations    ********/

#define	SDLSetTypeOfSegment(x,y)	(((x) & ~(CE_PRIMARY_MASK)) | (y))
#define	SDLGetTypeOfSegent(x)	_CEGetPrimaryType(x)

#define	SDLSetTypeToElement(x)	SDLSetTypeOfSegment((x), SDL_ELEMENT)
#define	SDLSetTypeToString(x)	SDLSetTypeOfSegment((x), SDL_STRING)
#define	SDLSetTypeToGraphic(x)	SDLSetTypeOfSegment((x), SDL_GRAPHIC)
#define	SDLSetTypeToSpecial(x)	SDLSetTypeOfSegment((x), CE_SPECIAL)

#ifndef	SDLAttrOffset
#define	SDLAttrOffset(p_type, field) \
	(((char *)(&(((p_type)NULL)->field)))-((char *)NULL))
#endif

#define	SDLSearchMask(mask, value) \
	((mask[(value)/SDL_MASK_SIZE] & \
		(((unsigned)1L) << ((value) % SDL_MASK_SIZE))) ? True : False)

#define	SDLMaskValue1(value) \
	(((value) < SDL_MASK_SIZE ? ((unsigned)1L) : 0) \
						<< ((value) % SDL_MASK_SIZE))

#define	SDLMaskValue2(value) \
	(((value) < SDL_MASK_SIZE ? 0: ((unsigned)1L)) \
						<< ((value) % SDL_MASK_SIZE))

#define	SDLClearAllBits	{ 0, 0 }

#define	SDLSetAllBits	\
	{ ~(SDLMaskValue1(SdlElementNone)), ~(SDLMaskValue2(SdlElementNone)) }

#define	SDLCheckMask(mask) \
	(((mask)[0] != 0 || (mask)[1] != 0) ? 1 : 0)

#define	SDLStripFromMask(mask, value) \
	(mask)[(value)/SDL_MASK_SIZE] = \
		((mask)[(value)/SDL_MASK_SIZE] & \
			(~(((unsigned)1L) << ((value) % SDL_MASK_SIZE))))

#define	SDLInitMask(value)	\
	{ \
	  SDLMaskValue1(value), \
	  SDLMaskValue2(value), \
	}

#define	SDLInitMaskTwo(value1, value2) \
	{ \
	  SDLMaskValue1(value1) | SDLMaskValue1(value2), \
	  SDLMaskValue2(value1) | SDLMaskValue2(value2), \
	}
#define	SDLInitMaskThree(value1, value2, value3) \
	{ \
	  SDLMaskValue1(value1) | SDLMaskValue1(value2) | SDLMaskValue1(value3), \
	  SDLMaskValue2(value1) | SDLMaskValue2(value2) | SDLMaskValue2(value3), \
	}
#define	SDLInitMaskFour(value1, value2, value3, value4) \
	{ \
	  SDLMaskValue1(value1)   | SDLMaskValue1(value2) | \
	    SDLMaskValue1(value3) | SDLMaskValue1(value4),  \
	  SDLMaskValue2(value1)   | SDLMaskValue2(value2) | \
	    SDLMaskValue2(value3) | SDLMaskValue2(value4),  \
	}
#define	SDLInitMaskFive(value1, value2, value3, value4, value5) \
	{ \
	  SDLMaskValue1(value1) | SDLMaskValue1(value2) | SDLMaskValue1(value3) | \
	    SDLMaskValue1(value4) | SDLMaskValue1(value5),\
	  SDLMaskValue2(value1) | SDLMaskValue2(value2) | SDLMaskValue2(value3) | \
	    SDLMaskValue2(value4) | SDLMaskValue2(value5),\
	}
#define	SDLInitMaskSix(value1, value2, value3, value4, value5, value6) \
	{ \
	  SDLMaskValue1(value1)   | SDLMaskValue1(value2) | \
	    SDLMaskValue1(value3) | SDLMaskValue1(value4) | \
	    SDLMaskValue1(value5) | SDLMaskValue1(value6),  \
	  SDLMaskValue2(value1)   | SDLMaskValue2(value2) | \
	    SDLMaskValue2(value3) | SDLMaskValue2(value4) | \
	    SDLMaskValue2(value5) | SDLMaskValue2(value6),  \
	}
#define	SDLInitMaskSeven(value1, value2, value3,\
				value4, value5, value6, value7) \
	{ \
	  SDLMaskValue1(value1)   | SDLMaskValue1(value2) | \
	    SDLMaskValue1(value3) | SDLMaskValue1(value4) | \
	    SDLMaskValue1(value5) | SDLMaskValue1(value6) | \
	    SDLMaskValue1(value7),  \
	  SDLMaskValue2(value1)   | SDLMaskValue2(value2) | \
	    SDLMaskValue2(value3) | SDLMaskValue2(value4) | \
	    SDLMaskValue2(value5) | SDLMaskValue2(value6) | \
	    SDLMaskValue2(value7),  \
	}

#define	SDLInitMaskEight(value1, value2, value3, value4, value5, \
				value6, value7, value8) \
	{ \
	  SDLMaskValue1(value1)   | SDLMaskValue1(value2) | \
	    SDLMaskValue1(value3) | SDLMaskValue1(value4) | \
	    SDLMaskValue1(value5) | SDLMaskValue1(value6) | \
	    SDLMaskValue1(value7) | SDLMaskValue1(value8),  \
	  SDLMaskValue2(value1)   | SDLMaskValue2(value2) | \
	    SDLMaskValue2(value3) | SDLMaskValue2(value4) | \
	    SDLMaskValue2(value5) | SDLMaskValue2(value6) | \
	    SDLMaskValue2(value7) | SDLMaskValue2(value8),  \
	}

#define	SDLInitMaskNine(value1, value2, value3, value4, value5, \
				value6, value7, value8, value9) \
	{ \
	  SDLMaskValue1(value1)   | SDLMaskValue1(value2) | \
	    SDLMaskValue1(value3) | SDLMaskValue1(value4) | \
	    SDLMaskValue1(value5) | SDLMaskValue1(value6) | \
	    SDLMaskValue1(value7) | SDLMaskValue1(value8) | \
	    SDLMaskValue1(value9),                         \
	  SDLMaskValue2(value1)   | SDLMaskValue2(value2) | \
	    SDLMaskValue2(value3) | SDLMaskValue2(value4) | \
	    SDLMaskValue2(value5) | SDLMaskValue2(value6) | \
	    SDLMaskValue2(value7) | SDLMaskValue2(value8) | \
	    SDLMaskValue2(value9),                         \
	}

#define	SDLInitMaskEleven(value1, value2, value3, value4, value5, \
				value6, value7, value8, value9, value10, \
				value11) \
	{ \
	  SDLMaskValue1(value1)    | SDLMaskValue1(value2)  | \
	    SDLMaskValue1(value3)  | SDLMaskValue1(value4)  | \
	    SDLMaskValue1(value5)  | SDLMaskValue1(value6)  | \
	    SDLMaskValue1(value7)  | SDLMaskValue1(value8)  | \
	    SDLMaskValue1(value9)  | SDLMaskValue1(value10) | \
	    SDLMaskValue1(value11),                          \
	  SDLMaskValue2(value1)    | SDLMaskValue2(value2)  | \
	    SDLMaskValue2(value3)  | SDLMaskValue2(value4)  | \
	    SDLMaskValue2(value5)  | SDLMaskValue2(value6)  | \
	    SDLMaskValue2(value7)  | SDLMaskValue2(value8)  | \
	    SDLMaskValue2(value9)  | SDLMaskValue2(value10) | \
	    SDLMaskValue2(value11),                          \
	}

#define	SDLInitMaskTwelve(value1, value2, value3, value4, value5, \
				value6, value7, value8, value9, value10, \
				value11, value12) \
	{ \
	  SDLMaskValue1(value1)    | SDLMaskValue1(value2)  | \
	    SDLMaskValue1(value3)  | SDLMaskValue1(value4)  | \
	    SDLMaskValue1(value5)  | SDLMaskValue1(value6)  | \
	    SDLMaskValue1(value7)  | SDLMaskValue1(value8)  | \
	    SDLMaskValue1(value9)  | SDLMaskValue1(value10) | \
	    SDLMaskValue1(value11) | SDLMaskValue1(value12),  \
	  SDLMaskValue2(value1)    | SDLMaskValue2(value2)  | \
	    SDLMaskValue2(value3)  | SDLMaskValue2(value4)  | \
	    SDLMaskValue2(value5)  | SDLMaskValue2(value6)  | \
	    SDLMaskValue2(value7)  | SDLMaskValue2(value8)  | \
	    SDLMaskValue2(value9)  | SDLMaskValue2(value10) | \
	    SDLMaskValue2(value11) | SDLMaskValue2(value12),  \
	}

#define	SDLInitMaskThirteen(value1, value2, value3, value4, value5, \
				value6 , value7 , value8 , value9, value10, \
				value11, value12, value13) \
	{ \
	  SDLMaskValue1(value1)    | SDLMaskValue1(value2)  | \
	    SDLMaskValue1(value3)  | SDLMaskValue1(value4)  | \
	    SDLMaskValue1(value5)  | SDLMaskValue1(value6)  | \
	    SDLMaskValue1(value7)  | SDLMaskValue1(value8)  | \
	    SDLMaskValue1(value9)  | SDLMaskValue1(value10) | \
	    SDLMaskValue1(value11) | SDLMaskValue1(value12) | \
	    SDLMaskValue1(value13),                          \
	  SDLMaskValue2(value1)    | SDLMaskValue2(value2)  | \
	    SDLMaskValue2(value3)  | SDLMaskValue2(value4)  | \
	    SDLMaskValue2(value5)  | SDLMaskValue2(value6)  | \
	    SDLMaskValue2(value7)  | SDLMaskValue2(value8)  | \
	    SDLMaskValue2(value9)  | SDLMaskValue2(value10) | \
	    SDLMaskValue2(value11) | SDLMaskValue2(value12) | \
	    SDLMaskValue2(value13),                          \
	}

#define	SDLInitMaskFourteen(value1, value2, value3, value4, value5, \
				value6 , value7 , value8 , value9, value10, \
				value11, value12, value13, value14) \
	{ \
	  SDLMaskValue1(value1)    | SDLMaskValue1(value2)  | \
	    SDLMaskValue1(value3)  | SDLMaskValue1(value4)  | \
	    SDLMaskValue1(value5)  | SDLMaskValue1(value6)  | \
	    SDLMaskValue1(value7)  | SDLMaskValue1(value8)  | \
	    SDLMaskValue1(value9)  | SDLMaskValue1(value10) | \
	    SDLMaskValue1(value11) | SDLMaskValue1(value12) | \
	    SDLMaskValue1(value13) | SDLMaskValue1(value14),  \
	  SDLMaskValue2(value1)    | SDLMaskValue2(value2)  | \
	    SDLMaskValue2(value3)  | SDLMaskValue2(value4)  | \
	    SDLMaskValue2(value5)  | SDLMaskValue2(value6)  | \
	    SDLMaskValue2(value7)  | SDLMaskValue2(value8)  | \
	    SDLMaskValue2(value9)  | SDLMaskValue2(value10) | \
	    SDLMaskValue2(value11) | SDLMaskValue2(value12) | \
	    SDLMaskValue2(value13) | SDLMaskValue1(value14),  \
	}

/********    Private Typedef Declarations    ********/
typedef	char*	SDLId;
typedef	char*	SDLCdata;

typedef	int	SDLNumber;
typedef	unsigned long	SDLMask;	/* must be a 32 bit value */

typedef	_DtHelpFontHints*	_DtHelpFontHintPtr;

/********    Private Structures Declarations    ********/

typedef struct  {
	const char	*string;
	SdlOption	 option_value;
} OptionList;

typedef struct  {
	unsigned long		attr_value;
	const OptionList	*options;
} SDLAttributeOptions;

typedef	struct {
	const char	*string;
	_DtHelpFontValue option_value;
} FontSpecOption;

typedef	struct {
	unsigned long		 attr_value;
	const FontSpecOption	*options;
} SDLFontSpecList;

typedef	struct {
	SDLMask	mask[SDL_MASK_LEN];
	int	(*model)();
} SDLContent;

enum	SdlStructType {
	SdlStructTypeBad = -1,
	SdlIgnore,
	SdlFontSpecific,
	SdlElementSpecific,
	SdlTableSpecific,
	SdlContainerSpecific
};

typedef	struct {
	const char		*name;
	enum SdlAttrDataType	 data_type;
	enum SdlStructType	 struct_type;
	int			 sdl_attr_define;
	int			 field_ptr;
} SDLAttribute;

typedef	struct {
	int			 sdl_attr_define;
	enum SdlAttrValue	 sdl_value;
	const char		*def_string;
} SDLElementAttrList;

enum	_sdlElToGeneral
  {
    SdlToNone,
    SdlToContainer,
    SdlToMarker,
    SdlToGraphic,
    SdlToString,
    SdlToSpc,
    SdlToSnb
  };

typedef	enum _sdlElToGeneral	SdlToGeneral;

typedef	struct _sdlElementInfo {
	const char		*element_str;     /* the element string     */
	const char		*element_end_str; /* the ending string      */
	int			 str_sig_chars;   /* # of significant chars */
	int			 end_sig_chars;   /* # of significant chars */
	enum	SdlElement	 sdl_element;     /* The element's value    */
	SDLMask			 exceptions[SDL_MASK_LEN];
						  /* what elements cannot
						     appear in this element */
	const SDLElementAttrList *attrib_list;    /* attribute list         */
	const SDLContent	*content;         /* The element's content  */
	int			 elim_flag;	  /* If this element's
						     segment list is null,
						     eleminate the element  */
	int			 cdata_flag;	  /* If no attributes, is
						     there cdata before the
						     end                    */
	int			 formatting;	  /* does this cause a
						     formatting break       */
	int			 new_fonts;	  /* does this cause new
						     fonts to be pulled in? */
	SdlToGeneral		 sdl_to_gen;	  /* What kind of general
						     type is this element?  */
} SDLElementInfo;

/********    Private Macros Declarations    ********/

#ifndef	_SdlIsTossStyle
#define	_SdlIsTossStyle(x)	(SdlElementKeyStyle  == (x) || \
				 SdlElementHeadStyle == (x) || \
				 SdlElementFormStyle == (x) || \
				 SdlElementFrmtStyle == (x) || \
				 SdlElementGrphStyle == (x))
#endif

/********    Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtHelpSDLI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
