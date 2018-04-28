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
/* $XConsortium: LinkMgrI.h /main/6 1995/11/13 16:31:02 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:   LinkMgrI.h
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
#ifndef _DtCvLinkMgrI_h
#define _DtCvLinkMgrI_h


#ifdef __cplusplus
extern "C" {
#endif

/********    Semi-Public Enums    ********/
enum	_dtLinkType {
	_DtLinkTypeNone,
	_DtLinkTypeLink,
	_DtLinkTypeSwitch
};

/********    Semi-Public Enum Typedefs    ********/
typedef	enum _dtLinkType	_DtLinkType;

/********    Semi-Public Structures Declarations    ********/
typedef struct  _DtLinkData {
        int      lnk_type;
        int      hint;
        char    *spec;
        char    *descrip;
} _DtLinkData;

typedef struct  _DtSwitchData {
        char    *interp;
        char    *cmd;
        char    *branches;
} _DtSwitchData;

typedef struct  _dtCvLinkEntry {
        _DtLinkType	 type;
        char		*id;
	union {
	  _DtLinkData	link;
	  _DtSwitchData swtch;
	} info;
} _DtCvLinkEntry;

struct  _dtCvLinkDb {
        int              max;
        _DtCvLinkEntry  *list;
};

/********    Semi-Public typedef Structures Declarations    ********/
#if !defined(_DtCanvasI_h) && !defined(_DtCvLinkMgrP_h)
typedef       struct  _dtCvLinkDb*    _DtCvLinkDb;
#endif

/********    Semi-Public Function Declarations    ********/

extern	int		 _DtLinkDbGetHint(
				_DtCvLinkDb	 link_data,
				int		 link_index);
extern	int		 _DtLinkDbGetLinkInfo(
				_DtCvLinkDb	 link_data,
				int		 link_index,
				int		(*filter)(),
				void		*client_data,
				_DtCvLinkInfo	*ret_info);
extern	char		*_DtLinkDbGetLinkSpec(
				_DtCvLinkDb	 link_data,
				int		 link_index);
extern	int		 _DtLinkDbGetLinkType(
				_DtCvLinkDb	 link_data,
				int		 link_index);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtCvLinkMgrI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
