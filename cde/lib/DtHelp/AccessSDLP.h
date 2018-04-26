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
/* $XConsortium: AccessSDLP.h /main/7 1995/12/18 16:30:11 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        AccessSDLP.h
 **
 **   Project:     Run Time Project File Access
 **
 **   Description: Header file for Access.h
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
#ifndef _DtHelpAccessSDLP_h
#define _DtHelpAccessSDLP_h

#include "CanvasSegP.h"
#include "SDLI.h"

/****************************************************************************
 *			Private Structure
 ****************************************************************************/
/*
 * The following structure holds the sdl document specific information
 */
typedef	struct	_sdlDocInfo {
	char		*language;
	char		*char_set;
	char		*first_pg;
	char		*doc_id;
	char		*timestamp;
	char		*sdldtd;
} SDLDocInfo;

/*
 * The following structure holds the id specific information
 */
typedef	struct	_sdlIdInfo {
	SdlOption	 type;
	char		*rssi;
	int		 rlevel;
	int		 offset;
} SDLIdInfo;

/*
 * The following structure holds the toss style specific information
 */
typedef	struct	_sdlTossInfo {
	unsigned long    enum_values;
	unsigned long    num_values;
	unsigned long    str1_values;
	unsigned long    str2_values;

	enum SdlElement		 el_type;
	SdlOption		 clan;
	char			*ssi;
	union
	  {
		char		*str;
		char		*colj;
		char		*enter;
	  } str1;
	union
	  {
		char		*str;
		char		*colw;
		char		*exit;
	  } str2;
	int			 rlevel;
	_DtHelpFontHints	 font_specs;
} SDLTossInfo;

/*
 * The following structure holds the entry or index specific strings
 */
typedef	struct	_sdlEntryInfo {
	char	*main;
	char	*locs;
	char	*syns;
	char	*sort;
} SDLEntryInfo;

/*
 * The following structure holds loaded volumes.  The fields of this 
 * structure should not be accessed by any code outside of the volume
 * module.
 */
typedef	struct _sdlVolume {
    SDLDocInfo *sdl_info;	/* A pointer to the SDL doc info     */
    _DtCvSegment *toss;		/* A pointer to the Table of Styles  */
				/* and Semantics withing the volume  */
    _DtCvSegment *loids;	/* A pointer to the List of Ids      */
    _DtCvSegment *index;	/* A pointer to the List of Keywords */
    _DtCvSegment *title;	/* A pointer to the document head    */
    _DtCvSegment *snb;		/* A pointer to the document's snb   */
    _DtCvPointer  client_data;	/* data that must be used to free the title
				   and snb				*/
    void	(*destroy_region)(); /* The destroy region routine specified
				   when the title was read              */
    short      minor_no;	/* The minor number of the sdl version */
    short      title_processed;	/* If the title has already been searched for */

} CESDLVolume;

/****************************************************************************
 *			Private Macros
 ****************************************************************************/
	/*********************/
	/* SDLDocInfo macros */
	/*********************/

#ifndef	_SdlDocInfoPtrCharSet
#define	_SdlDocInfoPtrCharSet(x)	((x)->char_set)
#endif

#ifndef	_SdlDocInfoPtrDocId
#define	_SdlDocInfoPtrDocId(x)		((x)->doc_id)
#endif

#ifndef	_SdlDocInfoPtrFirstPg
#define	_SdlDocInfoPtrFirstPg(x)	((x)->first_pg)
#endif

#ifndef	_SdlDocInfoPtrLanguage
#define	_SdlDocInfoPtrLanguage(x)	((x)->language)
#endif

#ifndef	_SdlDocInfoPtrSdlDtd
#define	_SdlDocInfoPtrSdlDtd(x)		((x)->sdldtd)
#endif

#ifndef	_SdlDocInfoPtrStamp
#define	_SdlDocInfoPtrStamp(x)		((x)->timestamp)
#endif

	/***********************/
	/* SDLEntryInfo macros */
	/***********************/

#ifndef	_SdlSegEntryInfo
#define	_SdlSegEntryInfo(x)		((FrmtPrivInfoPtr(x))->entry)
#endif

#ifndef	_SdlSegToSdlEntryInfo
#define	_SdlSegToSdlEntryInfo(x)	((SDLEntryInfo *) _SdlSegEntryInfo(x))
#endif

	/********************/
	/* SDLIdInfo macros */
	/********************/

#ifndef	_SdlIdInfoPtrOffset
#define	_SdlIdInfoPtrOffset(x)		((x)->offset)
#endif

#ifndef	_SdlIdInfoPtrRlevel
#define	_SdlIdInfoPtrRlevel(x)		((x)->rlevel)
#endif

#ifndef	_SdlIdInfoPtrRssi
#define	_SdlIdInfoPtrRssi(x)		((x)->rssi)
#endif

#ifndef	_SdlIdInfoPtrType
#define	_SdlIdInfoPtrType(x)		((x)->type)
#endif

#ifndef	_SdlSegToSdlIdInfoPtr
#define	_SdlSegToSdlIdInfoPtr(x) \
		((SDLIdInfo *)((FrmtPrivInfoPtr(x))->id_info))
#endif

#ifndef	_SdlSegToSdlIdInfoRssi
#define	_SdlSegToSdlIdInfoRssi(x) \
		(((SDLIdInfo *)((FrmtPrivInfoPtr(x))->id_info))->rssi)
#endif

#ifndef	_SdlSegToSdlIdInfoType
#define	_SdlSegToSdlIdInfoType(x) \
		(((SDLIdInfo *)((FrmtPrivInfoPtr(x))->id_info))->type)
#endif

#ifndef	_SdlSegToSdlIdInfoLevel
#define	_SdlSegToSdlIdInfoLevel(x) \
		(((SDLIdInfo *)((FrmtPrivInfoPtr(x))->id_info))->rlevel)
#endif

	/**********************/
	/* SDLTossInfo macros */
	/**********************/

#ifndef	_SdlTossInfoPtrFlag1
#define	_SdlTossInfoPtrFlag1(x)		((x)->enum_values)
#endif

#ifndef	_SdlTossInfoPtrFlag2
#define	_SdlTossInfoPtrFlag2(x)		((x)->num_values)
#endif

#ifndef	_SdlTossInfoPtrFlag3
#define	_SdlTossInfoPtrFlag3(x)		((x)->str1_values)
#endif

#ifndef	_SdlTossInfoPtrFlag4
#define	_SdlTossInfoPtrFlag4(x)		((x)->str2_values)
#endif

#ifndef	_SdlTossInfoPtrEnter
#define	_SdlTossInfoPtrEnter(x)		((x)->str1.enter)
#endif

#ifndef	_SdlTossInfoPtrExit
#define	_SdlTossInfoPtrExit(x)		((x)->str2.exit)
#endif

#ifndef	_SdlTossInfoPtrColJ
#define	_SdlTossInfoPtrColJ(x)		((x)->str1.colj)
#endif

#ifndef	_SdlTossInfoPtrColW
#define	_SdlTossInfoPtrColW(x)		((x)->str2.colw)
#endif

#ifndef	_SdlTossInfoPtrRlevel
#define	_SdlTossInfoPtrRlevel(x)	((x)->rlevel)
#endif

#ifndef	_SdlTossInfoPtrStr1
#define	_SdlTossInfoPtrStr1(x)		((x)->str1.str)
#endif

#ifndef	_SdlTossInfoPtrStr2
#define	_SdlTossInfoPtrStr2(x)		((x)->str2.str)
#endif

#ifndef	_SdlTossInfoPtrSsi
#define	_SdlTossInfoPtrSsi(x)		((x)->ssi)
#endif

#ifndef	_SdlTossInfoPtrFontSpecs
#define	_SdlTossInfoPtrFontSpecs(x)	((x)->font_specs)
#endif

#ifndef	_SdlTossInfoPtrClan
#define	_SdlTossInfoPtrClan(x)		((x)->clan)
#endif

#ifndef	_SdlTossInfoPtrType
#define	_SdlTossInfoPtrType(x)		((x)->el_type)
#endif

#ifndef	_SdlSegTossInfo
#define	_SdlSegTossInfo(x)		((FrmtPrivInfoPtr(x))->toss)
#endif

#ifndef	_SdlSegPtrToTossInfo
#define	_SdlSegPtrToTossInfo(x)		((SDLTossInfo *) _SdlSegTossInfo(x))
#endif

#ifndef	_SdlSegPtrToTossType
#define	_SdlSegPtrToTossType(x)		((_SdlSegPtrToTossInfo(x))->el_type)
#endif

	/*****************/
	/* Volume macros */
	/*****************/

#ifndef	_SdlVolumeMinorNumber
#define	_SdlVolumeMinorNumber(x)	((x)->minor_no)
#endif

	/*******************/
	/* Language macros */
	/*******************/

#ifndef	_SdlSegLangChar
#define	_SdlSegLangChar(x)		((FrmtPrivInfoPtr(x))->lang_char)
#endif

#ifndef	_SdlSegPtrToLangChar
#define	_SdlSegPtrToLangChar(x)		((char **)_SdlSegLangChar(x))
#endif

#endif /* _DtHelpAccessSDLP_h */
