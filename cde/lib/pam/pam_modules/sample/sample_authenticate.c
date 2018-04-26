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
/* $XConsortium: sample_authenticate.c /main/2 1996/05/09 04:29:50 drk $ */
/*
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)sample_authenticate.c 1.14     96/01/15 SMI"

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <syslog.h>
#include <libintl.h>

#include "sample_utils.h"

#define	SLEEPTIME	4

/*
 *
 * Sample module for pam_sm_authenticate.
 *
 * options -
 *
 *	debug
 *	use_first_pass
 *	try_first_pass
 *	first_pass_good  (first password is always good when used with use/try)
 *	first_pass_bad   (first password is always bad when used with use/try)
 *	pass=foobar	 (set good password to "foobar". default good password
 *			 is test)
 *	always_fail	 always return PAM_AUTH_ERR
 *	always_succeed   always return PAM_SUCCESS
 *	always_ignore
 *
 *
 */

/*
 * pam_sm_authenticate		- Authenticate user
 */

int
pam_sm_authenticate(
	pam_handle_t		*pamh,
	int 			flags,
	int			argc,
	const char		**argv)
{
	char			*user;
	struct pam_conv 	*pam_convp;
	int			err, result = PAM_AUTH_ERR;
	struct pam_response 	*ret_resp = (struct pam_response *)0;
	char 			messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	int			debug = 0;
	int			try_first_pass = 0;
	int			use_first_pass = 0;
	int			first_pass_good = 0;
	int			first_pass_bad = 0;
	int			i, num_msg;
	char			*firstpass, *password;
	char			the_password[64];

	if (debug)
		syslog(LOG_DEBUG, "Sample Authentication\n");

	strcpy(the_password, "test");

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcmp(argv[i], "try_first_pass") == 0)
			try_first_pass = 1;
		else if (strcmp(argv[i], "first_pass_good") == 0)
			first_pass_good = 1;
		else if (strcmp(argv[i], "first_pass_bad") == 0)
			first_pass_bad = 1;
		else if (strcmp(argv[i], "use_first_pass") == 0)
			use_first_pass = 1;
		else if (strcmp(argv[i], "always_fail") == 0)
			return (PAM_AUTH_ERR);
		else if (strcmp(argv[i], "always_succeed") == 0)
			return (PAM_SUCCESS);
		else if (strcmp(argv[i], "always_ignore") == 0)
			return (PAM_IGNORE);
		else if (sscanf(argv[i], "pass=%s", the_password) == 1) {
			/* nothing */;
		}
		else
			syslog(LOG_DEBUG, "illegal scheme option %s", argv[i]);
	}

	err = pam_get_item(pamh, PAM_USER, (void**) &user);
	if (err != PAM_SUCCESS)
		return (err);

	err = pam_get_item(pamh, PAM_CONV, (void**) &pam_convp);
	if (err != PAM_SUCCESS)
		return (err);

	(void) pam_get_item(pamh, PAM_AUTHTOK, (void **) &firstpass);

	if (firstpass && (use_first_pass || try_first_pass)) {

		if ((first_pass_good ||
			strcmp(firstpass, the_password) == 0) &&
				!first_pass_bad) {
					result = PAM_SUCCESS;
					goto out;
		}
		if (use_first_pass) goto out;
	}

	/*
	 * Get the password from the user
	 */
	if (firstpass) {
		(void) sprintf(messages[0], (const char *) PAM_MSG(pamh, 1,
			"TEST Password: "));
	} else {
		(void) sprintf(messages[0], (const char *) PAM_MSG(pamh, 2,
			"Password: "));
	}
	num_msg = 1;
	err = get_authtok(pam_convp->conv,
				num_msg, messages, NULL, &ret_resp);

	if (err != PAM_SUCCESS) {
		result = err;
		goto out;
	}

	password = ret_resp->resp;

	if (password == NULL) {
		result = PAM_AUTH_ERR;
		goto out;
	}

	/* one last ditch attempt to "login" to TEST */

	if (strcmp(password, the_password) == 0) {
		result = PAM_SUCCESS;
		if (firstpass == NULL) {
		/* this is the first password, stash it away */
		pam_set_item(pamh, PAM_AUTHTOK, password);
		}
	}

out:
	if (num_msg > 0) {
		if (ret_resp != 0) {
			if (ret_resp->resp != 0) {
				/* avoid leaving password cleartext around */
				memset(ret_resp->resp, 0,
					strlen(ret_resp->resp));
			}
			free_resp(num_msg, ret_resp);
			ret_resp = 0;
		}
	}

	return (result);
}
