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
/* $XConsortium: dce_acct_mgmt.c /main/5 1996/05/09 04:26:10 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)dce_acct_mgmt.c 1.3     95/08/02 SMI"

#include <syslog.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <libintl.h>
#include <dce/sec_login.h>
#include <dce/dce_error.h>

#include "pam_impl.h"
#include "utils.h"

/*
 * pam_sm_acct_mgmt	  main account managment routine.
 *			  XXX: The routine just prints out a warning message.
 *			  It may need to force the user to change his/her
 *			  passwd.
 */

#include <security/pam_appl.h>

#define	SECS_PER_HOUR	(60*60)
#define	SECS_PER_DAY	(SECS_PER_HOUR * 24)

static void
do_warn(pam_handle_t *pamh, time_t cur, time_t t, char *desc);

static void
do_warn_passwd(pam_handle_t *pamh);

int
pam_sm_acct_mgmt(
	pam_handle_t *pamh,
	int	flags,
	int	argc,
	const char **argv)
{
	dce_module_data_t	*dsd;
	sec_login_net_info_t	net_info;
	error_status_t		st;
	time_t			curtime;
	int			result = PAM_AUTH_ERR;
	int			i, debug = 0;
	int			warn = 1;
	int			allow_expired_passwd = 0;
	int			err;


	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcmp(argv[i], "allow_expired_passwd") == 0)
			allow_expired_passwd = 1;
		else if (strcmp(argv[i], "nowarn") == 0)
			warn = 0;
		else
			syslog(LOG_ERR,
				"illegal DCE acct_mgmt option %s", argv[i]);
	}

	if (flags & PAM_SILENT) warn = 0;

	if (debug) syslog(LOG_DEBUG, "DCE pam_sm_acct_mgmt");

	if (pam_get_data(pamh, DCE_DATA, (void**)&dsd) != PAM_SUCCESS ||
		dsd == NULL) {
		return (PAM_AUTH_ERR);
	}

	if (dsd->auth_status != PAM_SUCCESS)
		return (dsd->auth_status);

	if (dsd->auth_src == sec_login_auth_src_local) {
		/* we can't call sec_login_inquire_net_info on locally */
		/* authenticated contexts. Might want an option to dis-allow */
		/* them. For now we just allow them. */
		return (PAM_SUCCESS);
	}

	sec_login_inquire_net_info(dsd->login_context, &net_info, &st);

	if (st != error_status_ok && st != sec_login_s_not_certified) {
		if (debug) {
			dce_error_string_t text;

			syslog(LOG_DEBUG, "sec_login_inquire_net_info: %s",
				get_dce_error_message(st, text));
		}
		return (PAM_PERM_DENIED);
	}

	time(&curtime);

	if (warn) {
		if (dsd->reset_passwd) {
			do_warn_passwd(pamh);
		} else {
			do_warn(pamh, curtime,
				(time_t) net_info.passwd_expiration_date,
								"passwd");
		}
	}

	if (warn) do_warn(pamh, curtime,
		(time_t) net_info.acct_expiration_date, "account");

	result = PAM_SUCCESS;

	if ((net_info.passwd_expiration_date &&
			net_info.passwd_expiration_date < curtime) ||
			dsd->reset_passwd) {

		result = PAM_AUTHTOKEN_REQD;

		dsd->passwd_expired = 1;
	}

	/*
	 * I assume an expired account is worse then an expired password,
	 * so if both the password and account are expired we want to
	 * return PAM_ACCT_EXPIRED.
	 */

	if (net_info.acct_expiration_date &&
		net_info.acct_expiration_date < curtime) {
			result = PAM_ACCT_EXPIRED;
	}

	sec_login_free_net_info(&net_info);

	return (result);
}

static void
do_warn(pam_handle_t *pamh, time_t cur, time_t t, char *desc)
{
	char	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	if (t == 0)
		return; /* unlimited */

	if (cur > t) {
		sprintf(messages[0],
		PAM_MSG(pamh, 1, "Warning: Your DCE %s has expired.\n"), desc);
	} else if ((cur + SECS_PER_DAY) > t) {
		int hours;

		hours = (t - cur) / (SECS_PER_HOUR);
		hours = hours ? hours : 1;
		sprintf(messages[0], PAM_MSG(pamh, 2,
		"Warning: Your DCE %s will expire within %d hour%s.\n"),
			desc, hours, (hours == 1) ? "" : "s");
	} else if ((cur + (2*SECS_PER_DAY)) > t) {
		sprintf(messages[0], PAM_MSG(pamh, 3,
		"Warning: Your DCE %s will expire in 2 days.\n"), desc);
	} else {
		return;
	}
	__pam_display_msg(pamh, PAM_ERROR_MSG, 1, messages, NULL);
}


static void
do_warn_passwd(pam_handle_t *pamh)
{
	char	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	sprintf(messages[0],
		PAM_MSG(pamh, 4, "Warning: Your DCE passwd has expired.\n"));

	__pam_display_msg(pamh, PAM_ERROR_MSG, 1, messages, NULL);
}
