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
/* $XConsortium: dce_setcred.c /main/5 1996/05/09 04:27:17 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)dce_setcred.c 1.21     96/01/11 SMI"

#include <libintl.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <dce/sec_login.h>
#include <dce/dce_error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>
#include <libintl.h>

#include "pam_impl.h"
#include "utils.h"

static void
do_cred_warn(void *pamh)
{
	char			messages[1][PAM_MAX_MSG_SIZE];

	sprintf(messages[0],
	PAM_MSG(pamh, 30, "warning: Unable to obtain DCE credentials.\n"));

	(void) __pam_display_msg(pamh, PAM_ERROR_MSG, 1, messages, NULL);
}

/*
 * pam_sm_setcred
 */
int
pam_sm_setcred(
	pam_handle_t *pamh,
	int   flags,
	int	argc,
	const char **argv)
{
	int			debug = 0;
	int			warn = 1;
	int			i;
	dce_module_data_t	*dsd;
	error_status_t		st;
	int			err;

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
		if (strcmp(argv[i], "nowarn") == 0)
			warn = 0;
	}

	if (flags & PAM_SILENT) warn = 0;

	if (debug)
		syslog(LOG_DEBUG, "DCE pam_setcred");

	if (pam_get_data(pamh, DCE_DATA, (void**)&dsd) != PAM_SUCCESS ||
		dsd == NULL) {
		if (flags & PAM_DELETE_CRED) {
			sec_login_handle_t context;

			/* attempt to get current context and purge */
			sec_login_get_current_context(&context, &st);
			if (st == error_status_ok) {
				sec_login_purge_context(&context, &st);
			}
			return (st == error_status_ok ? PAM_SUCCESS :
					PAM_CRED_UNAVAIL);
		}

		if (warn) do_cred_warn(pamh);
		return (PAM_IGNORE); /* since pam_auth was never called */
	}

	if (dsd->auth_status != PAM_SUCCESS)
		return (dsd->auth_status);

	if (flags & PAM_DELETE_CRED) {
		(void) sec_login_purge_context(dsd->login_context, &st);

		if (debug) {
			dce_error_string_t text;

			syslog(LOG_DEBUG,
				"PAM: DCE sec_login_purge_context: %s",
				get_dce_error_message(st, text));
		}

	} else {
		if (dsd->auth_src == sec_login_auth_src_local) {
			/* we can't set_context on locally authenticated */
			/* contexts */
			st = sec_login_s_auth_local;
		} else {
			(void) sec_login_set_context(dsd->login_context, &st);
		}

		if (debug) {
			dce_error_string_t text;

			syslog(LOG_DEBUG, "PAM: DCE sec_login_set_context: %s",
				get_dce_error_message(st, text));
		}
	}

	/* might want an option to return success even if set_context failed */

	if (st == error_status_ok) {
		return (PAM_SUCCESS);
	} else {
		if (warn) do_cred_warn(pamh);

		if (debug) syslog(LOG_DEBUG,
				"Can't set user login context");
		return (PAM_CRED_ERR);
	}
}
