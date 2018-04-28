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
/* $XConsortium: dial_auth.c /main/5 1996/05/09 04:28:31 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident	"@(#)dial_auth.c 1.20	96/02/09 SMI"

#include "unix_headers.h"

/*
 * pam_sm_auth_port	- This is the top level function in the
 *			module called by pam_auth_port in the framework
 *			Returns: PAM_SERVICE_ERR on failure, 0 on success
 */

int
pam_sm_authenticate(
	pam_handle_t	*pamh,
	int	flags,
	int	argc,
	const char	**argv)
{
	char	*ttyn, *user;
	struct	pam_conv	*pam_convp;
	FILE 	*fp;
	char 	defpass[30];
	char	line[80];
	char 	*p1, *p2;
	struct passwd 	pwd;
	char	pwd_buffer[1024];
	char	*password;
	static struct pam_response *ret_resp;
	int	retcode;
	char 	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	int 	num_msg;
	int	i;
	int	debug = 0;

	for (i = 0; i < argc; i++) {
		if (strcasecmp(argv[i], "debug") == 0)
			debug = 1;
		else
			syslog(LOG_DEBUG, "illegal option %s", argv[i]);
	}

	if ((retcode = pam_get_user(pamh, &user, NULL))
					!= PAM_SUCCESS ||
	    (retcode = pam_get_item(pamh, PAM_TTY, (void **)&ttyn))
					!= PAM_SUCCESS ||
	    (retcode = pam_get_item(pamh, PAM_CONV, (void **)&pam_convp))
					!= PAM_SUCCESS)
		return (retcode);

	if (debug) {
		syslog(LOG_DEBUG,
			"Dialpass authenticate user = %s, ttyn = %s",
			user, ttyn);
	}

	if (getpwnam_r(user, &pwd, pwd_buffer, sizeof (pwd_buffer)) == NULL)
		return (PAM_USER_UNKNOWN);

	if ((fp = fopen(DIAL_FILE, "r")) == NULL) {
		return (PAM_SUCCESS);
	}

	while ((p1 = fgets(line, sizeof (line), fp)) != NULL) {
		while (*p1 != '\n' && *p1 != ' ' && *p1 != '\t')
			p1++;
		*p1 = '\0';
		if (strcmp(line, ttyn) == 0)
			break;
	}

	(void) fclose(fp);

	if (p1 == NULL || (fp = fopen(DPASS_FILE, "r")) == NULL)
		return (PAM_SUCCESS);

	defpass[0] = '\0';
	p2 = 0;

	while ((p1 = fgets(line, sizeof (line)-1, fp)) != NULL) {
		while (*p1 && *p1 != ':')
			p1++;
		*p1++ = '\0';
		p2 = p1;
		while (*p1 && *p1 != ':')
			p1++;
		*p1 = '\0';
		if (pwd.pw_shell != NULL && strcmp(pwd.pw_shell, line) == 0)
			break;

		if (strcmp(SHELL, line) == 0)
			SCPYN(defpass, p2);
		p2 = 0;
	}

	(void) fclose(fp);

	if (!p2)
		p2 = defpass;

	if (*p2 != '\0') {
		strcpy(messages[0], PAM_MSG(pamh, 1, "Dialup Password: "));
		num_msg = 1;
		retcode = __pam_get_input(PAM_PROMPT_ECHO_OFF, pam_convp->conv,
				num_msg, messages, NULL, &ret_resp);
		if (retcode != PAM_SUCCESS)
			return (retcode);
		password = ret_resp->resp;

		/* UNIX passwords can only be 8 characters long */
		password[8] = '\0';

		if (strcmp(crypt(password, p2), p2)) {
			__pam_free_resp(num_msg, ret_resp);
			return (PAM_SERVICE_ERR);
		}
		__pam_free_resp(num_msg, ret_resp);
	}

	return (PAM_SUCCESS);

}

/*
 * dummy pam_sm_setcred - does nothing
 */
pam_sm_setcred(
	pam_handle_t    *pamh,
	int	flags,
	int	argc,
	const char    **argv)
{
	return (PAM_SUCCESS);
}
