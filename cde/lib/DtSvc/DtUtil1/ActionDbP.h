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
/*****************************************************************************
 *
 * File:         ActionDbP.h
 * RCS:          $XConsortium: ActionDbP.h /main/3 1995/10/26 14:58:53 rswiston $
 * Description:  Private header file for the action database functions.
 * Language:     C
 * Package:      N/A
 *
 *
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 *****************************************************************************/

#ifndef _Dt_ActionDbP_h
#define _Dt_ActionDbP_h

#include <Dt/DbReader.h>
#include <Dt/ActionDb.h>

#define		_DtMAX_NUM_FIELDS		30
#define		_ActDb_MAX_NUM_FIELDS	_DtMAX_NUM_FIELDS


/*
 * Bitmask field definitions for the action converter bit mask.
 * These are NOT the bits for the mask in struct ACTION.
 */

#define		_ActDb_LABEL_SET	(1<<0)
#define		_ActDb_TYPE_SET		(1<<1)
#define		_ActDb_ARG_CLASS_SET	(1<<2)
#define		_ActDb_ARG_TYPE_SET	(1<<3)
#define		_ActDb_ARG_COUNT_SET	(1<<4)
#define		_ActDb_ARG_MODE_SET	(1<<5)
#define		_ActDb_DESCRIPTION_SET	(1<<6)
#define		_ActDb_ICON_SET		(1<<7)

#define		_ActDb_MAP_ACTION_SET	(1<<10)

#define		_ActDb_EXEC_STRING_SET	(1<<12)
#define		_ActDb_EXEC_HOST_SET	(1<<13)
#define		_ActDb_CWD_SET		(1<<14)
#define		_ActDb_WINDOW_TYPE_SET	(1<<15)
#define		_ActDb_TERM_OPTS_SET	(1<<16)

#define		_ActDb_TT_CLASS_SET	(1<<18)
#define		_ActDb_TT_SCOPE_SET	(1<<19)
#define		_ActDb_TT_OPERATION_SET	(1<<20)
#define		_ActDb_TT_FILE_SET	(1<<21)
#define		_ActDb_TT_ARGN_MODE_SET	(1<<22)
#define		_ActDb_TT_ARGN_VTYP_SET	(1<<23)
#define		_ActDb_TT_ARGN_RTYP_SET	(1<<24)
#define		_ActDb_TT_ARGN_VAL_SET	(1<<25)

#ifdef	_DT_ALLOW_DT_MSGS
#define		_ActDb_DT_REQ_NAME_SET	(1<<26)
#define		_ActDb_DT_SVC_SET	(1<<27)

#define		_ActDb_DT_NTFY_NAME_SET	(1<<28)
#define		_ActDb_DT_NGROUP_SET	(1<<29)
#define		_ActDb_DT_ARGN_VAL_SET	(1<<30)
/* no mask for ARGn strings */


#define _ActDb_DT_REQUEST_BITS	( _ActDb_DT_REQ_NAME_SET  \
				  | _ActDb_DT_SVC_SET )
#define _ActDb_DT_NOTIFY_BITS	( _ActDb_DT_NTFY_NAME_SET \
				  | _ActDb_DT_NGROUP_SET )
#endif  /* _DT_ALLOW_DT_MSGS */
#define _ActDb_TT_BITS		( _ActDb_TT_CLASS_SET \
				  | _ActDb_TT_SCOPE_SET \
				  | _ActDb_TT_OPERATION_SET \
				  | _ActDb_TT_ARGN_MODE_SET \
				  | _ActDb_TT_ARGN_VTYP_SET \
				  | _ActDb_TT_ARGN_RTYP_SET \
				  | _ActDb_TT_ARGN_VAL_SET \
				  | _ActDb_TT_FILE_SET )
#define _ActDb_CMD_BITS		( _ActDb_EXEC_STRING_SET \
				  | _ActDb_CWD_SET \
				  | _ActDb_WINDOW_TYPE_SET \
				  | _ActDb_TERM_OPTS_SET )
#define _ActDb_MAP_BITS		( _ActDb_MAP_ACTION_SET )

#define _ActDb_TT_ARGN_BITS	( _ActDb_TT_ARGN_MODE_SET \
				  | _ActDb_TT_ARGN_VTYP_SET \
				  | _ActDb_TT_ARGN_RTYP_SET \
				  | _ActDb_TT_ARGN_VAL_SET )

#ifdef	_DT_ALLOW_DT_MSGS
#define _ActDb_ARGN_BITS	( _ActDb_TT_ARGN_BITS \
				  | _ActDb_DT_ARGN_VAL_SET )
#else
#define _ActDb_ARGN_BITS	( _ActDb_TT_ARGN_BITS )
#endif  /* _DT_ALLOW_DT_MSGS */


/******************************************************************************
 *
 * Private (but external) entry points for internal use by Action Database
 * Library code only.
 *
 *****************************************************************************/

#  ifdef __cplusplus
extern "C" {
#  endif


extern Boolean	_DtActionConverter( DtDtsDbField *fields,
               DtDbPathId pathId,
               char *hostPrefix,
               Boolean rejectionStatus);


#  ifdef __cplusplus
}
#  endif
#endif	/* _Dt_ActionDbP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
