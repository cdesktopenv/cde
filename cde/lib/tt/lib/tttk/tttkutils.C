//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tttkutils.C /main/4 1996/02/27 16:06:59 drk $ 			 				
/*
 * @(#)tttkutils.C	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "api/c/tt_c.h"
#include "util/tt_port.h"
#include "api/c/api_api.h"
#include "tttk/tttk.h"
#include "tttk/tttkutils.h"

char _TttkKeys[ _TttkNumKeys ] = { 0 };

// Array of supported desktop messages

const char *TtDtOps[] = {
	"TtDtOpNone",
	"Created",
	"Deleted",
	"Do_Command",
	"Set_Environment",
	"Get_Environment",
	"Set_Geometry",
	"Get_Geometry",
	"Set_Iconified",
	"Get_Iconified",
	"Set_Locale",
	"Get_Locale",
	"Set_Mapped",
	"Get_Mapped",
	"Modified",
	"Reverted",
	"Get_Modified",
	"Moved",
	"Pause",
	"Resume",
	"Quit",
	"Raise",
	"Lower",
	"Save",
	"Revert",
	"Saved",
	"Set_Situation",
	"Get_Situation",
	"Signal",
	"Started",
	"Stopped",
	"Status",
	"Get_Status",
	"Get_Sysinfo",
	"Set_XInfo",
	"Get_XInfo",
	"Abstract",
	"Deposit",
	"Display",
	"Edit",
	"Edit",
	"Interpret",
	"Print",
	"Translate",
	"Mail",
	"Mail",
	"Mail",
	"Instantiate"
};

Tt_callback_action
_ttDtCallbackAction(
	Tt_message msg
)
{
	Tt_status status = tt_ptr_error( msg );
	if ((status != TT_OK) || (msg == 0)) {
		return TT_CALLBACK_PROCESSED;
	} else {
		return TT_CALLBACK_CONTINUE;
	}
}

void
_ttDtPrint(
	const char     *whence,
	const char     *msg
)
{
	_tt_syslog( 0, LOG_ERR, "%s: %s", whence, msg );
}

void
_ttDtPrintStatus(
	const char     *whence,
	const char     *expr,
	Tt_status	status
)
{
	_tt_syslog( 0, LOG_ERR, "%s: %s: %s\n",
		    whence, expr, tt_status_message( status ));
}

void
_ttDtPrintInt(
	const char     *whence,
	const char     *expr,
	int             n
)
{
	_tt_syslog( 0, LOG_ERR, "%s: %s: %d\n", whence, expr, n );
}

void
_ttDtPError(
	const char     *whence,
	const char     *msg
)
{
	_tt_syslog( 0, LOG_ERR, "%s: %s: %m", whence, msg );
}

Tttk_op
tttk_string_op(
	const char *s
)
{
	if (s == 0) return TTDT_OP_NONE;
	for (int op = TTDT_OP_NONE; op < TTDT_OP_LAST; op++) {
		const char *opStr = _ttDtOp( (Tttk_op)op );
		if (opStr == 0) {
			continue;
		}
		if (strcmp( opStr, s ) == 0) {
			return (Tttk_op)op;
		}
	}
	return TTDT_OP_NONE;
}

const char *
_ttDtOp(
	Tttk_op op
)
{
	if ((op > TTDT_OP_NONE) && (op < TTDT_OP_LAST)) {
		return TtDtOps[ op ];
	} else {
		return 0;
	}
}

char *
tttk_op_string(
	Tttk_op op
)
{
	return _tt_strdup( _ttDtOp( op ));
}
