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
 * File:         ActionDb.h
 * RCS:          $XConsortium: ActionDb.h /main/3 1995/10/26 14:58:38 rswiston $
 * Description:  Internal header file for the action database functions.
 * Language:     C
 * Package:      N/A
 * Status:       Experimental (Do Not Distribute)
 *
 *
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 *****************************************************************************/

#ifndef _Dt_ActionDb_h
#define _Dt_ActionDb_h


/*
 * The following constants are used by the database parsing code to
 * identify the field names for an action definition.
 *
 * Recognized Field names for any action definition.
 */
#define	_DtACTION_NAME			"ACTION"
#define _DtACTION_TYPE			"TYPE"
#define _DtACTION_LABEL			"LABEL"
#define _DtACTION_ARG_CLASS		"ARG_CLASS"
#define _DtACTION_ARG_MODE		"ARG_MODE"
#define _DtACTION_ARG_TYPE		"ARG_TYPE"
#define _DtACTION_ARG_COUNT		"ARG_COUNT"
#define _DtACTION_DESCRIPTION		"DESCRIPTION"
#define _DtACTION_ICON			"ICON"
#define _DtACTION_INSTANCE_ICON		"INSTANCE_ICON"

/*
 * Miscellaneous syntax strings
 */
#define	_DtACT_ANY			"*"
#define _DtACT_LIST_SEPARATOR_CHAR	','
#define _DtACT_GT_CHAR			'>'
#define	_DtACT_LT_CHAR			'<'


/*
 * Valid values for ACTION_TYPE
 */
#define _DtACTION_MAP		"MAP"
#define _DtACTION_COMMAND	"COMMAND"
#define _DtACTION_TT_MSG	"TT_MSG"

#ifdef  _DT_ALLOW_DT_MSGS

#define _DtACTION_DT_REQUEST	"DT_REQUEST"
#define _DtACTION_DT_NOTIFY	"DT_NOTIFY"
#endif	/* _DT_ALLOW_DT_MSGS */

/*
 * Field names for "MAP" type actions
 */

#define _DtACTION_MAP_ACTION	"MAP_ACTION"

/*
 * Field names for the "COMMAND" type actions.
 */
#define _DtACTION_EXEC_STRING		"EXEC_STRING"
#define _DtACTION_EXEC_HOST		"EXEC_HOST"
#define _DtACTION_CWD			"CWD"
#define _DtACTION_WINDOW_TYPE		"WINDOW_TYPE"
#define _DtACTION_TERM_OPTS		"TERM_OPTS"

/*
 * Field names for "TT_MSG" type actions
 */
#define _DtACTION_TT_CLASS		"TT_CLASS"
#define _DtACTION_TT_SCOPE		"TT_SCOPE"
#define _DtACTION_TT_OPERATION		"TT_OPERATION"
#define _DtACTION_TT_FILE		"TT_FILE"
/* definitions to decifer TT_ARGn_* fields */
#define _DtACTION_TTN_ARG		"TT_ARG"
#define _DtACTION_TTN_MODE		"_MODE"
#define _DtACTION_TTN_VTYPE		"_VTYPE"
#define _DtACTION_TTN_REP_TYPE		"_REP_TYPE"
#define _DtACTION_TTN_VALUE		"_VALUE"


#ifdef _DT_ALLOW_DT_MSGS
/*
 * Field names for "DT_REQUEST" type actions
 */
#define _DtACTION_DT_REQUEST_NAME	"DT_REQUEST_NAME"
#define _DtACTION_DT_SVC		"DT_SVC"

/*
 * Field names for "DT_NOTIFY" type actions
 */
#define _DtACTION_DT_NOTIFY_NAME	"DT_NOTIFY_NAME"
#define _DtACTION_DT_NGROUP		"DT_NGROUP"

/*
 * Field names for DT ARGn (either request or notify)
 */
#define _DtACTION_DTN_ARG		"DT_ARG"
#define _DtACTION_DTN_VALUE		"_VALUE"

#endif	/* _DT_ALLOW_DT_MSGS */

/*
 * Valid  Field value strings
 *	for Command Actions Window Types
 */
#define _DtACTION_TERMINAL		"TERMINAL"
#define	_DtACTION_PERM_TERMINAL		"PERM_TERMINAL"
#define _DtACTION_NO_STDIO		"NO_STDIO"

/*
 * Valid Field values for ARG_CLASS records.
 */
#define	_DtACTION_BUFFER		"BUFFER"
#define _DtACTION_FILE			"FILE"
#define _DtACTION_STRING		"STRING"

/*
 * Valid Field values for ARG_MODE 
 */

#define		_DtACT_ARG_MODE_WRITE		"w"
#define		_DtACT_ARG_MODE_NOWRITE		"!w"
#define		_DtACT_ARG_MODE_ANY		_DtACT_ANY


/*
 * Valid Field values for TT_CLASS records.
 */
#define _DtACTION_TT_NOTICE		"TT_NOTICE"
#define _DtACTION_TT_REQUEST		"TT_REQUEST"

/*
 * Valid Field values for TT_SCOPE records.
 */
#define _DtACTION_TT_SESSION		"TT_SESSION"
#define _DtACTION_TT_BOTH		"TT_BOTH"
#define _DtACTION_TT_FILE_IN_SESSION	"TT_FILE_IN_SESSION"
/* --- same as TT_MSG's TT_FILE field name --------
 #define _DtACTION_TT_FILE		"TT_FILE"
 */

/*
 * Valid Field values for TT_ARGn_MODE
 */
#define _DtACTION_TT_MODE_IN		"TT_IN"
#define _DtACTION_TT_MODE_OUT		"TT_OUT"
#define _DtACTION_TT_MODE_INOUT		"TT_INOUT"

/*
 * Valid Field values for TT_ARGn_REP_TYPE
 */
#define _DtACTION_TT_RTYP_UND	"TT_REP_UNDEFINED"
#define _DtACTION_TT_RTYP_INT	"TT_REP_INTEGER"
#define _DtACTION_TT_RTYP_BUF	"TT_REP_BUFFER"
#define _DtACTION_TT_RTYP_STR	"TT_REP_STRING"

/*
 * Action Keywords -- 
 *		    these keywords appear in the action database files
 *                  in the form: %<qualifier><keyword><prompt string>%
 *		    where the optional qualifier in enclosed in: ()
 *		    and the optional prompt string is enclosed in: ""
 */
#define	_DtACT_DATABASEHOST_STR		"DatabaseHost"
#define _DtACT_DISPLAYHOST_STR		"DisplayHost"
#define _DtACT_LOCALHOST_STR		"LocalHost"
#define _DtACT_SESSIONHOST_STR		"SessionHost"
#define _DtACT_ARGS_STR			"Args"
#define _DtACT_ARG_UNDER_STR		"Arg_"

#define _DtACT_STRING_QUALIFIER		"(String)"
#define _DtACT_FILE_QUALIFIER		"(File)"



/*
 * Default action field value definitions (these should be strings)
 * for recognized fields.
 */
#define		_DtACT_LBL_DFLT		NULL
#define		_DtACT_TYPE_DFLT	_DtACTION_COMMAND
#define		_DtACT_ARG_CLASS_DFLT	_DtACT_ANY
#define		_DtACT_ARG_MODE_DFLT	_DtACT_ANY
#define		_DtACT_ARG_TYPE_DFLT	_DtACT_ANY
#define		_DtACT_ARG_CNT_DFLT	_DtACT_ANY
#define		_DtACT_DESC_DFLT	NULL
#define		_DtACT_ICON_DFLT	NULL
#define		_DtACT_MAP_ACT_DFLT	NULL
#define		_DtACT_EXEC_STRG_DFLT	NULL
#define		_DtACT_EXEC_HOST_DFLT	\
	"%" _DtACT_DATABASEHOST_STR "%,%" _DtACT_LOCALHOST_STR "%"
#define		_DtACT_CWD_DFLT		NULL
#define		_DtACT_WIN_TYPE_DFLT	_DtACTION_PERM_TERMINAL
#define		_DtACT_TERM_OPTS_DFLT	NULL
#define		_DtACT_TT_CLASS_DFLT	NULL
#define		_DtACT_TT_SCOPE_DFLT	NULL
#define		_DtACT_TT_OP_DFLT	NULL
#define		_DtACT_TT_FILE_DFLT	NULL

#ifdef	_DT_ALLOW_DT_MSGS
#define		_DtACT_DT_REQ_DFLT	NULL
#define		_DtACT_DT_SVC_DFLT	NULL
#define		_DtACT_DT_NTFY_DFLT	NULL
#define		_DtACT_DT_NGRP_DFLT	NULL
#endif	/* _DT_ALLOW_DT_MSGS */



#endif	/* _Dt_ActionDb_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
