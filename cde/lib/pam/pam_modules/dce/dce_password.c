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
/* $XConsortium: dce_password.c /main/5 1996/05/09 04:26:43 drk $ */
/*
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)dce_password.c 1.22     96/02/14 SMI"

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

#include <dce/acct.h>
#include <dce/uuid.h>
#include <dce/binding.h>
#include <dce/sec_login.h>
#include <dce/dce_error.h>

#include "utils.h"

#include "pam_impl.h"

#ifdef XFN_MAPPING
#include "xfn_mapping.h"
#endif

static int
dce_changepw(
	char *account_name,
	char *old_pass,
	char *new_pass);

static char *
get_passwd(
	pam_handle_t *pamh,
	char *prompt);

/*
 * XXX: This module is NOT finished!
 *
 */
int
pam_sm_chauthtok(
	pam_handle_t		*pamh,
	int			flags,
	int			argc,
	const char		**argv)
{
	char			*user;
	int			err, result = PAM_AUTH_ERR;
	char			*newpass = NULL, *vnewpass = NULL;
	char			*oldpass = NULL;
	int			try_first_pass = 0;
	int			use_first_pass = 0;
	char			*firstpass = NULL;

#ifdef XFN_MAPPING
	int			try_mapped_pass = 0;
	int			use_mapped_pass = 0;
	uid_t			saved_uid;
#endif
	int			i;
	int			debug = 0;
	uid_t			pw_uid;
	dce_module_data_t	*dsd = NULL;
	error_status_t		status;

	if (debug)
		syslog(LOG_DEBUG, "DCE Authentication\n");

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcmp(argv[i], "try_first_pass") == 0)
			try_first_pass = 1;
		else if (strcmp(argv[i], "use_first_pass") == 0)
			use_first_pass = 1;
#ifdef XFN_MAPPING
		else if (strcmp(argv[i], "try_mapped_pass") == 0)
			try_mapped_pass = 1;
		else if (strcmp(argv[i], "use_mapped_pass") == 0)
			use_mapped_pass = 1;
#endif
		else
			syslog(LOG_DEBUG, "illegal scheme option %s", argv[i]);
	}


	if (flags & PAM_PRELIM_CHECK) {

		/* try and bind to registry master of local cell */

		sec_rgy_handle_t	rh = sec_rgy_default_handle;

		sec_rgy_site_open_update(NULL, &rh, &status);
		if (status == error_status_ok) {
			sec_rgy_site_close(rh, &status);
			return (PAM_SUCCESS);
		}
		return (PAM_AUTHTOK_ERR);
	}

	/* make sure PAM framework is telling us to update passwords */
	if (!(flags & PAM_UPDATE_AUTHTOK)) {
		syslog(LOG_ERR, "dce pam_sm_chauthtok: bad flags: %d", flags);
		return (PAM_SYSTEM_ERR);
	}


	if (flags & PAM_CHANGE_EXPIRED_AUTHTOK) {
		if (pam_get_data(pamh, DCE_DATA, (void **)&dsd)
							== PAM_SUCCESS) {
			if (!dsd->passwd_expired)
				return (PAM_IGNORE);
		}
	}

	if ((err = pam_get_item(pamh, PAM_USER, (void **) &user)) < 0)
		return (err);

	/* Don't bother to handle root in DCE */

	if (strcmp(user, "root") == 0)
		return (PAM_IGNORE);

#ifdef XFN_MAPPING
	if (use_mapped_pass || try_mapped_pass) {
		int got_mapped = 0, updated_mapped = 0;
		char dcepass[MAP_PASSLEN+1];

		if ((err = pam_get_item(pamh, PAM_AUTHTOK,
					(void **) &newpass)) < 0)
			return (err);
		if ((err = pam_get_item(pamh, PAM_OLDAUTHTOK,
					(void **) &oldpass)) < 0)
			return (err);

		if (!get_pw_uid(user, &pw_uid)) {
			return (PAM_AUTHTOK_ERR);
		}

		/* XXX: need to seteuid */

		saved_uid = geteuid();

		if (saved_uid != pw_uid && getuid() == 0 &&
				seteuid(pw_uid) < 0) {
			syslog(LOG_ERR,
				"xfn_get_mapped_passwd: seteuid: %m");
			return (PAM_AUTHTOK_ERR);
		}

		got_mapped = xfn_get_mapped_password(0, user,
			DCE_XFN_PASS_ATTR, oldpass, dcepass, sizeof (dcepass));

		if (got_mapped) {

			updated_mapped = xfn_update_mapped_password(
				0, user, DCE_XFN_PASS_ATTR, newpass,
						dcepass);

			memset(dcepass, 0, sizeof (dcepass));

		} else {
			/* probably should prompt for DCE password */
			/* and attempt to update it */
		}

		if (geteuid() != saved_uid && seteuid(saved_uid) < 0) {
			syslog(LOG_ERR,
				"xfn_get_mapped_passwd seteuid restore: %m");
			/* XXX:  what should we do here? */
		}

		if (use_mapped_pass) {
			if (updated_mapped)
				return (PAM_SUCCESS);
			else
				return (PAM_AUTHTOK_ERR);
		}

		return (PAM_AUTHTOK_ERR);
	}
#endif /* XFN_MAPPING */

	if (try_first_pass || use_first_pass) {

		if ((err = pam_get_item(pamh, PAM_AUTHTOK,
					(void **) &newpass)) < 0)
			return (err);
		if ((err = pam_get_item(pamh, PAM_OLDAUTHTOK,
					(void **) &oldpass)) < 0)
			return (err);

		result = dce_changepw(user, oldpass, newpass);

		if (result == PAM_SUCCESS) {
		/* might need to update PAM_OLDAUTHTOK and PAM_NEWAUTHTOK */
			goto out;
		}

		/* assume we need to prompt for old DCE password? */

		oldpass = get_passwd(pamh, PAM_MSG(pamh, 20,
						"Old DCE password: "));

		if (oldpass == NULL || oldpass[0] == '\0') {
			/* Need a password to proceed */
			result = PAM_AUTHTOK_ERR;
			goto out;
		}

		result = dce_changepw(user, oldpass, newpass);

		if (result == PAM_SUCCESS) {
		/* might need to update PAM_OLDAUTHTOK and PAM_NEWAUTHTOK */
		}
		goto out;
	}


	/* prompt for both old and new passwords */

	if (firstpass == NULL && !(try_first_pass||try_mapped_pass))
		oldpass = get_passwd(pamh, PAM_MSG(pamh, 20,
						"Old DCE password: "));
	else
		oldpass = get_passwd(pamh, PAM_MSG(pamh, 20,
						"Old DCE password: "));

	if (oldpass == NULL || oldpass[0] == '\0') {
		/* Need a password to proceed */
		result = PAM_AUTHTOK_ERR;
		goto out;
	}

	if (firstpass == NULL && !(try_first_pass||try_mapped_pass))
		newpass = get_passwd(pamh, PAM_MSG(pamh, 21,
						"New DCE password: "));
	else
		newpass = get_passwd(pamh, PAM_MSG(pamh, 21,
						"New DCE password: "));

	if (newpass == NULL || newpass[0] == '\0') {
		/* Need a password to proceed */
		result = PAM_AUTHTOK_ERR;
		goto out;
	}

	if (firstpass == NULL && !(try_first_pass||try_mapped_pass))
		vnewpass = get_passwd(pamh,
					PAM_MSG(pamh, 22,
					"Re-enter new DCE password: "));
	else
		vnewpass = get_passwd(pamh,
					PAM_MSG(pamh, 22,
					"Re-enter new DCE password: "));

	if (vnewpass == NULL || vnewpass[0] == '\0') {
		/* Need a password to proceed */
		result = PAM_AUTHTOK_ERR;
		goto out;
	}

	if (strcmp(newpass, vnewpass)) {
		result = PAM_AUTHTOK_ERR;
		goto out;
	}

	result = dce_changepw(user, oldpass, newpass);

	if (result == PAM_SUCCESS) {
		/* might need to update PAM_OLDAUTHTOK and PAM_NEWAUTHTOK) */
	}
out:

	if (oldpass) {
		memset(oldpass, 0, strlen(oldpass));
		free(oldpass);
	}

	if (newpass) {
		memset(newpass, 0, strlen(newpass));
		free(newpass);
	}


	if (vnewpass) {
		memset(vnewpass, 0, strlen(vnewpass));
		free(vnewpass);
	}

	return (result);
}

static int
dce_changepw(
	char *account_name,
	char *old_pass,
	char *new_pass)
{
	error_status_t		status;
	sec_rgy_handle_t	rgy_handle = sec_rgy_default_handle;
	sec_rgy_login_name_t	name_key;
	sec_rgy_cursor_t	account_cursor;
	sec_rgy_login_name_t	name_result;
	sec_rgy_sid_t		id_sid;
	sec_rgy_unix_sid_t	unix_sid;
	sec_rgy_acct_key_t	key_parts;
	sec_rgy_acct_user_t	user_part;
	sec_rgy_acct_admin_t	admin_part;
	sec_passwd_rec_t	new_key, caller_key;
	sec_passwd_version_t	new_key_version;
	sec_login_handle_t	login_context = sec_login_default_handle;
	sec_passwd_rec_t	passwd_rec;
	boolean32		reset_pass = 0, login_valid = 0;
	sec_login_auth_src_t	auth_src;
	char			*env;
	static char		 *krb5 = "KRB5CCNAME";
	char			*krb5_value = NULL;

	/* stash away the value of KRB5CCNAME, if there is one set */
	env = getenv(krb5);
	if (env) {
		krb5_value = malloc(strlen(krb5)+1+strlen(env)+1);
		if (krb5_value)
			sprintf(krb5_value, "%s=%s", krb5, env);
		else {
			status = sec_login_s_no_current_context;
			goto out;
		}
	}

	/* first we a get a login context. A future version should */
	/* check and see if there is a current context with the */
	/* same name as the user we are setting */

	if (!sec_login_setup_identity((unsigned_char_p_t) account_name,
		sec_login_no_flags, &login_context, &status)) {
			goto out;
	}

	/* have to strdup password because the call clears it */
	passwd_rec.key.tagged_union.plain =
			(idl_char *) strdup(old_pass);
	if (passwd_rec.key.tagged_union.plain == NULL) {
		status = sec_login_s_no_current_context;
		goto out;
	}

	passwd_rec.key.key_type = sec_passwd_plain;
	passwd_rec.pepper = NULL;
	passwd_rec.version_number = sec_passwd_c_version_none;

	login_valid = sec_login_validate_identity(login_context,
		    &passwd_rec, &reset_pass, &auth_src, &status);

	free(passwd_rec.key.tagged_union.plain); /* already memset */

	if (!login_valid)
		goto out;

	sec_login_set_context(login_context, &status);

	if (status != error_status_ok)
		goto out;

	/* we now have a context with the same account as the one */
	/* we are trying to change the password on. Lets talk to the */
	/* registry... */

	sec_rgy_site_open(NULL, &rgy_handle, &status);

	if (status != error_status_ok) {
		goto out;
	}

	caller_key.key.key_type = sec_passwd_plain;
	caller_key.pepper = NULL;
	caller_key.version_number = sec_passwd_c_version_none;
	caller_key.key.tagged_union.plain = (idl_char *) old_pass;

	sec_rgy_cursor_reset(&account_cursor);

	strcpy((char *)name_key.pname, account_name);
	name_key.gname[0] = '\0';
	name_key.oname[0] = '\0';

	/* need to do this in order to get the org and group for the */
	/* account. All we want is name_result... */

	sec_rgy_acct_lookup(
		rgy_handle,
		&name_key,
		&account_cursor,
		&name_result,
		&id_sid,
		&unix_sid,
		&key_parts,
		&user_part,
		&admin_part,
		&status);

	if (status != error_status_ok)
		goto out;

	caller_key.key.key_type = sec_passwd_plain;
	caller_key.pepper = NULL;
	caller_key.version_number = sec_passwd_c_version_none;
	caller_key.key.tagged_union.plain = (idl_char *) old_pass;

	new_key.key.key_type = sec_passwd_plain;
	new_key.pepper = NULL;
	new_key.version_number = sec_passwd_c_version_none;
	new_key.key.tagged_union.plain = (idl_char *) new_pass;

	/* rock and roll. lets try to update the password... */

	sec_rgy_acct_passwd(
		rgy_handle,
		&name_result,
		&caller_key,
		&new_key,
		sec_passwd_des,
		&new_key_version,
		&status);

out:
	/* restore the old KRB5CCNAME value */
	if (krb5_value) {
		putenv(krb5_value);
	}

	if (rgy_handle != sec_rgy_default_handle) {
		error_status_t	st; /* don't trash status */

		sec_rgy_site_close(rgy_handle, &st);
	}

	if (login_context != sec_login_default_handle) {
		error_status_t	st; /* don't trash status */

		sec_login_purge_context(&login_context, &st);
	}

	if (status == error_status_ok) {
		return (PAM_SUCCESS);
	} else {
		return (PAM_AUTHTOK_ERR);
	}
}



static char *
get_passwd(
	pam_handle_t *pamh,
	char *prompt)
{
	int			err;
	char			*p;

	err = __pam_get_authtok(pamh, PAM_PROMPT, 0, 256, prompt, &p);

	if (err != PAM_SUCCESS) {
		return (NULL);
	}

	return (p);
}
