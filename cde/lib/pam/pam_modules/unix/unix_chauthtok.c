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
/* $XConsortium: unix_chauthtok.c /main/5 1996/05/09 04:33:18 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident	"@(#)unix_chauthtok.c 1.83	95/12/12 SMI"

#include "unix_headers.h"

/*
 * pam_sm_chauthtok():
 *	To change authentication token.
 *
 *	This function handles all requests from the "passwd" command
 *	to change a user's password in all repositories specified
 *	in nsswitch.conf.
 */

int
pam_sm_chauthtok(
	pam_handle_t		*pamh,
	int			flags,
	int			argc,
	const char		**argv)
{
	int i;
	int debug = 0;			/* debug option from pam.conf */
	int authtok_aged = 0;		/* flag to check if password expired */
	unix_authtok_data *status;	/* status in pam handle stating if */
					/* password aged */

	/*
	 * Only check for debug here - parse remaining options
	 * in __update_authtok();
	 */
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
	}

	if (flags & PAM_PRELIM_CHECK) {
		/* do not do any prelim check at this time */
		if (debug)
			syslog(LOG_DEBUG,
			"unix pam_sm_chauthtok(): prelim check");
		return (PAM_SUCCESS);
	}

	/* make sure PAM framework is telling us to update passwords */
	if (!(flags & PAM_UPDATE_AUTHTOK)) {
		syslog(LOG_ERR, "unix pam_sm_chauthtok: bad flags: %d", flags);
		return (PAM_SYSTEM_ERR);
	}

	if (flags & PAM_CHANGE_EXPIRED_AUTHTOK) {
		if (pam_get_data(pamh, UNIX_AUTHTOK_DATA, (void **)&status)
							== PAM_SUCCESS) {
			switch (status->age_status) {
			case PAM_NEW_AUTHTOK_REQD:
				if (debug)
				    syslog(LOG_DEBUG,
				    "pam_sm_chauthtok: UNIX password aged");
				authtok_aged = 1;
				break;
			default:
				/* UNIX authtok did not expire */
				if (debug)
				    syslog(LOG_DEBUG,
				    "pam_sm_chauthtok: UNIX password young");
				authtok_aged = 0;
				break;
			}
		}
		if (!authtok_aged)
			return (PAM_IGNORE);
	}

	/*
	 * 	This function calls __update_authtok() to change passwords.
	 *	By passing PAM_REP_DEFAULT, the repository will be determined
	 *	by looking in nsswitch.conf.
	 *
	 *	To obtain the domain name (passed as NULL), __update_authtok()
	 *	will call: nis_local_directory();
	 */
	return (__update_authtok(pamh, flags, PAM_REP_DEFAULT, NULL,
			argc, argv));
}
