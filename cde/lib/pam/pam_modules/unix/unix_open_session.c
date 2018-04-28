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
/* $XConsortium: unix_open_session.c /main/5 1996/05/09 04:34:32 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_open_session.c 1.32     95/12/08 SMI"

/*
 * pam_sm_open_session 	- session management for individual users
 */

#include "unix_headers.h"


int
pam_sm_open_session(
	pam_handle_t	*pamh,
	int	flags,
	int	argc,
	const char	**argv)
{
	int	error;
	char	*ttyn, *rhost, *user;
	int	fdl;
	struct lastlog	newll;
	struct passwd pwd;
	char	buffer[2048];
	int	i;
	int	debug = 0;
	long long	offset;

	for (i = 0; i < argc; i++) {
		if (strcasecmp(argv[i], "debug") == 0)
			debug = 1;
		else
			syslog(LOG_ERR, "illegal option %s", argv[i]);
	}

	if ((error = pam_get_item(pamh, PAM_TTY, (void **)&ttyn))
							!= PAM_SUCCESS ||
	    (error = pam_get_item(pamh, PAM_USER, (void **)&user))
							!= PAM_SUCCESS ||
	    (error = pam_get_item(pamh, PAM_RHOST, (void **)&rhost))
							!= PAM_SUCCESS) {
		return (error);
	}

	if (getpwnam_r(user, &pwd, buffer, sizeof (buffer)) == NULL) {
		return (PAM_USER_UNKNOWN);
	}

	if ((fdl = open(LASTLOG, O_RDWR|O_CREAT, 0444)) >= 0) {

		/*
		 * The value of lastlog is read by the UNIX
		 * account management module
		 */

		offset = (long long) pwd.pw_uid *
					(long long) sizeof (struct lastlog);

		if (llseek(fdl, offset, SEEK_SET) != offset) {
			/*
			 * XXX uid too large for database
			 */
			return (PAM_SUCCESS);
		}

		(void) time(&newll.ll_time);
		strncpy(newll.ll_line,
			(ttyn + sizeof ("/dev/")-1),
			sizeof (newll.ll_line));
		strncpy(newll.ll_host, rhost, sizeof (newll.ll_host));

		(void) write(fdl, (char *)&newll, sizeof (newll));
		(void) close(fdl);
	}

	return (PAM_SUCCESS);
}
