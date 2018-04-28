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
/* $XConsortium: dce_authenticate.c /main/5 1996/05/09 04:26:26 drk $ */
/*
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)dce_authenticate.c 1.34     96/02/14 SMI"

#include <dce/nbase.h>
#include <dce/sec_login.h>
#include <dce/dce_error.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <rpc/des_crypt.h>
#include <pwd.h>
#include <syslog.h>
#include <libintl.h>

#include "pam_impl.h"
#include "utils.h"

#ifdef XFN_MAPPING
#include "xfn_mapping.h"
#endif /* XFN_MAPPING */

#define	SLEEPTIME	4

/* maxmimum DCE_PASSWD_LENGTH. We need to pick something for 
 *  __pam_get_authtok to use.
 */

#define	DCE_PASSWD_LENGTH 256

static int
attempt_dce_login(
	void *pamh,
	dce_module_data_t	*dsd,
	error_status_t		*st,
	char			*user,
	char			*dce_pass
);

void
dce_cleanup(
	pam_handle_t *pamh,
	void *data,
	int pam_status
);

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
	int			err, result = PAM_AUTH_ERR;
	char 			messages[1][PAM_MAX_MSG_SIZE];
	error_status_t		st;
	char			*defpass;
	int 			num_msg = 0;
	int			debug = 0;
	int			warn = 1;
	int			passwd_flag = 0;
	int			try_first_pass = 0;
	int			use_first_pass = 0;
	int			ignore = 0;
	int			invalid_user = 0;

#ifdef XFN_MAPPING
	int			try_mapped_pass = 0;
	int			use_mapped_pass = 0;
#endif
	int			i;
	char			*firstpass = NULL, *password = NULL;
	uid_t			pw_uid;
	dce_module_data_t	*dsd = NULL;

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcmp(argv[i], "try_first_pass") == 0) {
			if (!passwd_flag) {
				try_first_pass = 1;
				passwd_flag = 1;
			}
		} else if (strcmp(argv[i], "use_first_pass") == 0) {
			if (!passwd_flag) {
				use_first_pass = 1;
				passwd_flag = 1;
			}
#ifdef XFN_MAPPING
		} else if (strcmp(argv[i], "try_mapped_pass") == 0) {
			if (!passwd_flag) {
				try_mapped_pass = 1;
				passwd_flag = 1;
			}
		} else if (strcmp(argv[i], "use_mapped_pass") == 0) {
			if (!passwd_flag) {
				use_mapped_pass = 1;
				passwd_flag = 1;
			}
#endif
		} else if (strcmp(argv[i], "nowarn") == 0) {
			warn = 0;
		} else {
			syslog(LOG_ERR, "illegal module option %s", argv[i]);
		}
	}

	if (flags & PAM_SILENT) warn = 0;

	if (debug)
		syslog(LOG_DEBUG, "DCE pam_sm_authenticate");

	err = pam_get_user(pamh, &user, NULL);

	if (err != PAM_SUCCESS)
		return (err);

	if (user == NULL || !user[0])
		return (PAM_AUTH_ERR);

	/* Don't bother to authenticate root in DCE */

	if (strcmp(user, "root") == 0)
		ignore = 1;

	/* make sure a password entry exists for this user */
	/* we also need the uid for XFN */

	if (!get_pw_uid(user, &pw_uid)) {
		invalid_user = 1;
	}

	if (pam_get_data(pamh, DCE_DATA, (void**)&dsd) != PAM_SUCCESS ||
		dsd == NULL) {

		dsd = calloc(1, sizeof (dce_module_data_t));
		if (dsd == NULL) {
			result = PAM_BUF_ERR;
			goto out;
		}

		if ((err = pam_set_data(pamh, DCE_DATA, dsd, &dce_cleanup))
			!= PAM_SUCCESS) {
				free(dsd);
				result = err;
				goto out;
		}

	} else {
		if (dsd->login_context != sec_login_default_handle) {
			error_status_t st;
			sec_login_purge_context(&dsd->login_context, &st);
		}
	}

	dsd->login_context = sec_login_default_handle;
	dsd->auth_status = PAM_AUTH_ERR;
	dsd->debug = debug;
	dsd->warn = warn;
	dsd->reset_passwd = 0;
	dsd->passwd_expired = 0;
	dsd->auth_src = sec_login_auth_src_network;

	/* see if a legitimate DCE user */

	if (!sec_login_setup_identity((unsigned char *)user,
			sec_login_no_flags, &dsd->login_context, &st)) {
		if (debug) {
			dce_error_string_t text;
			syslog(LOG_DEBUG,
				"PAM: DCE sec_login_setup_identity: %s",
			get_dce_error_message(st, text));
		}
		if (st == sec_rgy_object_not_found) {
			/* mask the unknown user case */
			invalid_user = 1;
		} else {
			result = PAM_AUTH_ERR;
			goto out;
		}
	}

	err = pam_get_item(pamh, PAM_AUTHTOK, (void **) &firstpass);

	if (err != PAM_SUCCESS && (use_first_pass || use_mapped_pass)) {
		result = PAM_AUTH_ERR;
		if (debug)
			syslog(LOG_DEBUG, "PAM: DCE goto out!");
		goto out;
	}

	if (firstpass != NULL && (ignore || invalid_user))
		goto out;

#ifdef XFN_MAPPING

	if (firstpass == NULL) {
		if (use_mapped_pass) goto out;
	} else if (try_mapped_pass || use_mapped_pass) {
		char dcepass[MAP_PASSLEN+1];
		uid_t saved_uid;
		int got_mapped_pass;

		saved_uid = geteuid();

		if (saved_uid != pw_uid && getuid() == 0 &&
				seteuid(pw_uid) < 0) {
			syslog(LOG_ERR,
				"xfn_get_mapped_passwd: seteuid: %m");
			/* continue since we might be able to get mapping */
		}

		got_mapped_pass = xfn_get_mapped_password(
			debug ? XFN_MAP_DEBUG : 0, user, DCE_XFN_PASS_ATTR,
				firstpass, dcepass, sizeof (dcepass));

		if (geteuid() != saved_uid && seteuid(saved_uid) < 0) {
			syslog(LOG_ERR,
				"xfn_get_mapped_passwd seteuid restore: %m");
			/* XXX:  what should we do here? */
		}

		if (got_mapped_pass) {

			result =
			    attempt_dce_login(pamh, dsd, &st, user, dcepass);

			memset(dcepass, 0, MAP_PASSLEN);

			if (result == PAM_SUCCESS ||
				st == sec_login_s_acct_invalid) goto out;
		}
		if (use_mapped_pass) goto out;
	}
#endif

	if (firstpass == NULL) {
		if (use_first_pass) goto out;
	} else if (use_first_pass || try_first_pass) {

		result = attempt_dce_login(pamh, dsd, &st,
						user, firstpass);

		if (result == PAM_SUCCESS ||
			st == sec_login_s_acct_invalid ||
			use_first_pass) {
			goto out;
		}
	}

	/*
	 * Get the password from the user
	 */

	if (debug)
		syslog(LOG_DEBUG, "DCE pam_sm_auth prompting for password");

	if (firstpass == NULL &&
			!(try_first_pass||try_mapped_pass||invalid_user))
		(void) sprintf(messages[0], (const char *) PAM_MSG(pamh, 10,
			"Password: "));
	else
		(void) sprintf(messages[0], (const char *) PAM_MSG(pamh, 11,
			"DCE Password: "));

	num_msg = 1;

	err = __pam_get_authtok(pamh, PAM_PROMPT, PAM_AUTHTOK,
			DCE_PASSWD_LENGTH,messages[0], &password);
	if (debug)
		syslog(LOG_DEBUG, "DCE __pam_get_authtok = %d", err);

	if (err != PAM_SUCCESS) {
		result = err;
		goto out;
	}

	if (password == NULL) {
		/* Need a password to proceed */
		result = PAM_AUTH_ERR;
		goto out;
	}

	if (firstpass == NULL) {
		/* this is the first password, stash it away */
		pam_set_item(pamh, PAM_AUTHTOK, password);
	}

	/* one last ditch attempt to login to DCE */

	if (invalid_user || ignore)
		goto out;

	result = attempt_dce_login(pamh, dsd, &st, user, password);

#ifdef XFN_MAPPING
	/* we had to prompt for DCE password, so attempt to */
	/* update mapping iff we got a good DCE password */

	if (try_mapped_pass && result == PAM_SUCCESS && firstpass) {
		uid_t saved_uid;

		saved_uid = geteuid();

		if (saved_uid != pw_uid && seteuid(pw_uid) < 0) {
			syslog(LOG_ERR, "xfn_get_mapped_passwd: seteuid: %m");
			goto out;
		}

		xfn_update_mapped_password(debug ? XFN_MAP_DEBUG : 0,
			user, DCE_XFN_PASS_ATTR, firstpass, password);

		if (geteuid() != saved_uid && seteuid(saved_uid) < 0) {
			syslog(LOG_ERR,
			"xfn_get_mapped_passwd seteuid restore: %m");
			/* XXX:  what should we do here? */
		}
	}
#endif

out:

	if (ignore) {
		result = PAM_IGNORE;
	}

	if (password != NULL)
		memset(password, 0, strlen(password));

	if (invalid_user)
		result = PAM_USER_UNKNOWN;

	if (dsd)
		dsd->auth_status = result;

	return (result);
}

static int
attempt_dce_login(
	void *pamh,
	dce_module_data_t	*dsd,
	error_status_t		*st,
	char *user,
	char *dce_pass)
{
	sec_passwd_rec_t	passwd_rec;
	boolean32		login_valid = 0;
	error_status_t		set_st;

	/* have to strdup password because the call clears it */
	passwd_rec.key.tagged_union.plain = (idl_char *) strdup(dce_pass);
	if (passwd_rec.key.tagged_union.plain == NULL) {
		return (PAM_BUF_ERR);
	}
	passwd_rec.key.key_type = sec_passwd_plain;
	passwd_rec.pepper = NULL;
	passwd_rec.version_number = sec_passwd_c_version_none;

	login_valid = sec_login_valid_and_cert_ident(dsd->login_context,
		&passwd_rec, &dsd->reset_passwd, &dsd->auth_src, st);

	if (dsd->debug) {
		dce_error_string_t text;

		syslog(LOG_DEBUG, "sec_login_valid_and_cert_ident: %s",
				get_dce_error_message(*st, text));
	}

	if (*st == sec_login_s_acct_invalid && dsd->warn) {
		char messages[1][PAM_MAX_MSG_SIZE];

		sprintf(messages[0],
		PAM_MSG(pamh, 12, "Error: Your DCE Account has expired.\n"));
		__pam_display_msg(pamh, PAM_ERROR_MSG, 1, messages, NULL);
	}

	/* the call to sec_login_valid_and_cert_ident already zeros */
	/* out the password, so we just free it */

	free(passwd_rec.key.tagged_union.plain);

	return (login_valid ? PAM_SUCCESS : PAM_AUTH_ERR);
}

void
dce_cleanup(
	pam_handle_t *pamh,
	void *data,
	int pam_status)
{
	error_status_t	st;
	dce_module_data_t *dsd = (dce_module_data_t *) data;
	int status;

	if (dsd->debug) {
		syslog(LOG_DEBUG, "DCE dce_cleanup pam_sm_auth_status(%d)",
			dsd->auth_status);
	}

	if (!dsd->login_context != sec_login_default_handle) {
		free(dsd);
		return;
	}

	/* if pam_end as PAM_SUCCESS, clean up based on value in */
	/* auth_status, otherwise just purge the context */

	if (pam_status == PAM_SUCCESS) {
		pam_sec_login_free_context(dsd->auth_status,
			&dsd->login_context, &st);
	} else {
		sec_login_purge_context(&dsd->login_context, &st);
		if (dsd->debug) {
			dce_error_string_t text;
			syslog(LOG_DEBUG, "sec_login_purge_context: %s",
				get_dce_error_message(st, text));
		}
	}

	free(dsd);
}
