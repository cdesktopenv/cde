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

/*
 *	$XConsortium: loadP.h /main/3 1995/11/06 18:30:26 rswiston $
 *
 *	@(#)loadP.h	1.8 03 Apr 1995	cde_app_builder/src/libABil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * loadP.h - private load utilities
 */
#ifndef _ABIL_BIL_LOADP_H_
#define _ABIL_BIL_LOADP_H_

#include <ab_private/istr.h>

/*
 * Undef a few standard error codes (yes, this is a hack and should be fixed)
 */
#undef ERR_EOF
#undef ERR_NO_MEMORY

/*
 * Error messages.
 */
#define ERR_BAD_ANCHOR          1
#define ERR_BAD_ATT_FOR_OBJ     2
#define ERR_BAD_IDENTIFIER      3
#define ERR_EOF                 4
#define ERR_FILE_FORMAT         5
#define ERR_NOT_IMPL            6
#define ERR_NOT_IMPL_IGNOR      7
#define ERR_NO_MEMORY           8
#define ERR_UNKNOWN             9
#define ERR_UNKNOWN_ATTR        10
#define ERR_UNKNOWN_MENU_TYPE   11
#define ERR_UNKNOWN_OBJ         12
#define ERR_UNKNOWN_OBJECT      13
#define ERR_UNKNOWN_WHEN        14
#define ERR_WANT_ARG            15
#define ERR_WANT_BOOLEAN        16
#define ERR_WANT_FULL_NAME      17
#define ERR_WANT_INTEGER        18
#define ERR_WANT_KEYWORD        19
#define ERR_WANT_LIST           20
#define ERR_WANT_MENU_NAME      21
#define ERR_WANT_NAME           22
#define ERR_WANT_OBJECT         23
#define ERR_WANT_STRING         24
#define ERR_NOT_PROJECT         27
#define ERR_NOT_MODULE         	28

/*
 * Warnings.
 */
#define WARN_BAD_HANDLER        25
#define WARN_NO_MEMBERS         26

/* NOTE: BIL_ERRMSG_COUNT must be greater than last #define'd ERR/WARN token  */
#define BIL_ERRMSG_COUNT	30

/* Structure used to build BIL error message table */
typedef struct _AbilErrorMessageRec {
	short	msg_id;		/* message ID in libABil's message set */
	char    *def_str;	/* default message text */
} AbilErrorMessage;

typedef int (AbilGetLineNumberCallbackRec)(void);
typedef AbilGetLineNumberCallbackRec *AbilGetLineNumberCallback;

/*
 * Functions
 */
STRING	abil_loadmsg(STRING);   /* output error message */
int	abil_loadmsg_clear(void);
int	abil_loadmsg_set_object(STRING objname);
int	abil_loadmsg_set_att(STRING attname);
int	abil_loadmsg_set_action_att(STRING actattname);
int	abil_loadmsg_set_file(STRING bilfile);
int	abil_loadmsg_set_line_number_callback(AbilGetLineNumberCallback cb);

int	abil_loadmsg_get_line_number(void);

/*
 * Module-wide variables
 */
extern int abilP_err_line_number;

#endif /* _ABIL_BIL_LOADP_H_ */

