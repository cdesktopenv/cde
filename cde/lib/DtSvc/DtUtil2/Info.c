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
/* $XConsortium: Info.c /main/1 1996/03/25 19:10:11 barstow $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 * This file contains the main program for: dtinfo_start
 */

#include <stdlib.h>
#include <sys/param.h>		/* MAXHOSTNAMELEN */
#include <Tt/tt_c.h>
#include <Dt/Info.h>
#include <Dt/Connect.h>

/*
 * External declaration for Xegethostname
 */
extern int Xegetshorthostname (char * hostname, unsigned int size);


/*
 * Static variables
 */
static const char 	* DTINFOLIBDEFAULT_NAME = "DTINFOLIBDEFAULT";

static const char 	* DTINFOLIBDEFAULT_DEFAULT = "cde";

static const char      	* SHOW_INFO_OP_NAME = "DtInfo_ShowInfoAtLoc";

static const char       * SHOW_INFO_DEFAULT_ACTION = "DtInfoStartAtLoc";

static const char       * LOCALE_NAME = "LANG";

static const char       * DEFAULT_LOCALE = "C";

static const char       * STRING_ARG_TYPE = "string";


/*
 * Forward declarations for static functions
 */
static DtInfoShowStatus ConnectToMessageServer ();


/*
 * Public functions
 */
DtInfoShowStatus
DtInfoShowTopic (
	const char		* info_lib,	/* the InfoLib */
	const char		* locator)	/* the generalized locator
						   format */
{
	Tt_message		message;
	Tt_status		status;
	DtInfoShowStatus	ret_val;
	const char		* file = info_lib;
	const char		* locale;
	char			host[MAXHOSTNAMELEN];

	/*
	 * Check the arguments
	 */
	if (!locator)
		return (DtINFO_SHOW_BAD_LOCATOR);

	if ((ret_val = ConnectToMessageServer ()) != DtINFO_SHOW_OK)
		return (ret_val);

	if (!file) {
		if ((file = getenv (DTINFOLIBDEFAULT_NAME)) == NULL)
			file = DTINFOLIBDEFAULT_DEFAULT;
	}

	if ((locale = getenv (LOCALE_NAME)) == NULL)
		locale = DEFAULT_LOCALE;

	message = tt_message_create ();
	status = tt_ptr_error (message);
	if (status != TT_OK)
		return (DtINFO_SHOW_MSG_CREATE_FAIL);

	/*
	 * Initialize message
	 */
	tt_message_class_set 	(message, TT_REQUEST);
	tt_message_scope_set 	(message, TT_SESSION);
	tt_message_address_set 	(message, TT_PROCEDURE);
	tt_message_session_set 	(message, tt_default_session());
	tt_message_op_set 	(message, SHOW_INFO_OP_NAME);
	tt_message_file_set	(message, info_lib);

	/*
	 * Add the arguments
	 */
	tt_message_arg_add (message, TT_IN, STRING_ARG_TYPE, 
			    SHOW_INFO_DEFAULT_ACTION);

	if ((Xegetshorthostname (host, MAXHOSTNAMELEN)) != 0)
		tt_message_arg_add (message, TT_IN, STRING_ARG_TYPE, NULL);
	else
		tt_message_arg_add (message, TT_IN, STRING_ARG_TYPE, host);

	tt_message_arg_add (message, TT_IN, STRING_ARG_TYPE, locale);
	tt_message_arg_add (message, TT_IN, STRING_ARG_TYPE, locator);

	/*
	 * Send it
	 */
	status = tt_message_send (message);
	if (status != TT_OK)
		return (DtINFO_SHOW_MSG_SEND_FAIL);

	return (DtINFO_SHOW_OK);
}


static DtInfoShowStatus
ConnectToMessageServer (void)
{
	char	       		* procid;
	Tt_status		status;

	procid = tt_default_procid ();
	status = tt_ptr_error (procid);
	if (status == TT_OK) {
		tt_free (procid);
	}

	if ((status == TT_ERR_NOMP) || (status == TT_ERR_PROCID)) {
		procid = tt_open ();
		status = tt_ptr_error (procid);
		if (status != TT_OK) {
			return (DtINFO_SHOW_TT_OPEN_FAIL);
		}
	}

	return (DtINFO_SHOW_OK);
}
