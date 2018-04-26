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
/* $XConsortium: unix_authenticate.c /main/5 1996/05/09 04:32:58 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_authenticate.c 1.58     96/02/09 SMI"

#include "unix_headers.h"

#ifdef PAM_SECURE_RPC
static void
unix_cleanup(
	pam_handle_t *pamh,
	void *data,
	int pam_status)
{
	free((unix_auth_data *)data);
}

#endif

static int attempt_authentication(char *, char *, char *, uid_t, int, int);

/*
 * pam_sm_authenticate		- Authenticate user
 */

int
pam_sm_authenticate(
	pam_handle_t	*pamh,
	int 	flags,
	int	argc,
	const char	**argv)
{
	struct 		spwd shpwd; /* Shadow password structure */
	struct		passwd pwd; /* password structure */
	char		shpwd_buf[1024];
	char		pwd_buf[1024];
	char 		*password = NULL;
	char		*dummy_passwd = "no:password";
	int		err = PAM_SUCCESS;
	int		retcode;
	int		debug = 0;
	int		try_first_pass = 0;
	int		use_first_pass = 0;
	int		i;
	uid_t		uid;
	char		*service, *user;
#ifdef PAM_SECURE_RPC
	unix_auth_data	*status;
	char		netname[MAXNETNAMELEN+1];
	int		estkey_stat;
#endif

	for (i = 0; i < argc; i++) {
		if (strcasecmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcasecmp(argv[i], "nowarn") == 0)
			flags = flags | PAM_SILENT;
		else if (strcmp(argv[i], "try_first_pass") == 0)
			try_first_pass = 1;
		else if (strcmp(argv[i], "use_first_pass") == 0)
			use_first_pass = 1;
		else {
			syslog(LOG_ERR, "illegal option %s", argv[i]);
		}
	}

	if ((err = pam_get_item(pamh, PAM_SERVICE, (void **)&service))
							!= PAM_SUCCESS ||
	    (err = pam_get_user(pamh, &user, NULL)) != PAM_SUCCESS)
		return (err);

	if (debug)
		syslog(LOG_DEBUG,
			"unix pam_sm_authenticate(%s %s), flags = %x ",
			service, (user)?user:"no-user", flags);

	if (!user) {
		return (PAM_USER_UNKNOWN);
	}

	/*
	 * Get the password and shadow password entry
	 */
	memset(pwd_buf, 0, sizeof (pwd_buf));
	memset(shpwd_buf, 0, sizeof (shpwd_buf));
	if (getpwnam_r(user, &pwd, pwd_buf, sizeof (pwd_buf)) == NULL ||
	    getspnam_r(user, &shpwd, shpwd_buf, sizeof (shpwd_buf)) == NULL) {
		err = PAM_USER_UNKNOWN;
		/*
		 * Mask unknown users.
		 * Set up a dummy password.
		 */
		shpwd.sp_pwdp = dummy_passwd;
	}

	/*
	 * Is a password check required?
	 */
	/* Is there anything there to check? */
	if ((shpwd.sp_pwdp == 0) || (*shpwd.sp_pwdp == '\0')) {
		/*
		 * The /etc/default/login file will specify if passwords
		 * are required.  If not, then simply return SUCCESS.
		 * Otherwise, flag the error, but still prompt the user
		 * for a password to mask the failure.
		 */
		if ((flags & PAM_DISALLOW_NULL_AUTHTOK) == 0)
			goto out;
		err = PAM_SERVICE_ERR;
		goto prompt;
	}

	if ((err = __pam_get_authtok(pamh, PAM_HANDLE, PAM_AUTHTOK,
		PASSWORD_LEN, NULL, &password)) != PAM_SUCCESS)
		goto out;

	if (try_first_pass) {
		/*
		 * Try to login using the password from the first
		 * scheme, e.g. DCE password. If anything goes wrong,
		 * then simply prompt users for password.
		 */

		retcode = attempt_authentication(user, password,
			shpwd.sp_pwdp, pwd.pw_uid,
			flags, debug);
		if (err != PAM_USER_UNKNOWN)
			err = retcode;

		if (err == PAM_SUCCESS)
			goto post_prompt;
		else
			goto prompt;
	} else if (use_first_pass) {
		/*
		 * Try to login using the password from the first
		 * scheme, e.g. DCE password. If anything goes wrong,
		 * quit, and return the error;
		 */
		retcode = attempt_authentication(user, password,
			shpwd.sp_pwdp, pwd.pw_uid,
			flags, debug);
		if (err != PAM_USER_UNKNOWN)
			err = retcode;

		if (err == PAM_SUCCESS)
			goto post_prompt;
		else
			goto out;
	}

prompt:
	/*
	 * Get the password from the user
	 */
	if ((password != NULL && password[0] != '\0') || try_first_pass) {
		if (password) {
			memset(password, 0, strlen(password));
			free(password);
			password = NULL;
		}
		if ((retcode = __pam_get_authtok(pamh, PAM_PROMPT,
			PAM_AUTHTOK, PASSWORD_LEN,
			PAM_MSG(pamh, 30, "UNIX Password: "),
			&password)) != PAM_SUCCESS) {
			if (err != PAM_USER_UNKNOWN)
				err = retcode;
			goto out;
		}
	} else {
		if (password) {
			memset(password, 0, strlen(password));
			free(password);
			password = NULL;
		}
		if ((retcode = __pam_get_authtok(pamh, PAM_PROMPT,
			PAM_AUTHTOK, PASSWORD_LEN,
			PAM_MSG(pamh, 31, "Password: "),
			&password)) != PAM_SUCCESS) {
			if (err != PAM_USER_UNKNOWN)
				err = retcode;
			goto out;
		}
	}

	retcode = attempt_authentication(user, password,
		shpwd.sp_pwdp, pwd.pw_uid, flags, debug);
	if (err != PAM_USER_UNKNOWN)
		err = retcode;

	if (err != PAM_SUCCESS)
		goto out;

post_prompt:

#ifdef PAM_SECURE_RPC
	/*
	 * Do a keylogin if the password is
	 * not null and its not a root login.
	 * This code used to be in pam_setcred().
	 */
	uid = pwd.pw_uid;
	if (password != NULL &&
	    password[0] != '\0' &&
	    uid != 0) {
		/*
		* we always ask to reestablish the private key with
		* keyserv to solve the problem that the keys may have
		* changed and a re-keylogin not done
		*/
		estkey_stat = establish_key(uid, password, 1, netname);

		/*
		 * Store the return value as module specific data
		 * to be printed out later in pam_setcred().
		 */
		if (pam_get_data(pamh, UNIX_AUTH_DATA, (void**)&status)
							!= PAM_SUCCESS) {
			if ((status = (unix_auth_data *)calloc
						(1, sizeof (unix_auth_data)))
							== NULL) {
				err = PAM_BUF_ERR;
				goto out;
			}
		}
		status->key_status = estkey_stat;
		strcpy(status->netname, netname);
		if (pam_set_data(pamh, UNIX_AUTH_DATA, status, unix_cleanup)
						!= PAM_SUCCESS) {
			err = PAM_SERVICE_ERR;
			goto out;
		}
	}
#endif /* PAM_SECURE_RPC */

out:
	if (password) {
		memset(password, 0, strlen(password));
		free(password);
	}

	return (err);
}

static int
attempt_authentication(char *user, char *password, char *enc_passwd,
	uid_t uid, int flags, int debug)
{
	struct passwd pwd;
	struct spwd shpwd;
	char	shpwd_buf[1024];
	char	pwd_buf[1024];
	int	estkey_stat;
	uid_t	orig_uid;
	int	err = 0;

	if (password == NULL || password[0] == '\0') {
		if (debug)
			syslog(LOG_DEBUG,
			"unix_auth: NULL passwd in attempt_authenticate()");
		return (PAM_AUTH_ERR);
	} else {

		/*
		 * Yes, there is some string in the sp_pwdp field.
		 * We have one of the following two situations:
		 *    1) the sp_pwdp string is actually the encrypted
		 *	 user's password,
		 * or 2) the sp_pwdp string is "*NP*", which means we
		 *	 didn't actually have permission to read the
		 *	 password field in the name service.
		 *
		 * In either case, we must obtain the password from the
		 * user. In situation 2, we can't actually tell yet
		 * whether the unix password is present or not. We must
		 * get the password from the user, just to establish
		 * the user's secure RPC credentials. Then, having
		 * established the user's Secure RPC credentials, we
		 * need to re-obtain the shpwd structure. At that point,
		 * if the unix password is present there, we check it
		 * against that too.
		 */

		if (strcmp(enc_passwd, "*NP*") == 0) {
#ifdef PAM_SECURE_RPC
			estkey_stat = establish_key(uid,
						password, 1, NULL);
			if (estkey_stat != ESTKEY_SUCCESS) {
				/* Failed to establish secret key. */
				switch (estkey_stat) {
				case ESTKEY_BADPASSWD:
					err = PAM_AUTH_ERR;
					break;
				case ESTKEY_NOCREDENTIALS:
					/*
					 * user requires credentials to
					 * read passwd field but doesn't
					 * have any should syslog() a
					 * message for admin
					 */
					syslog(LOG_ALERT,
						"User %s needs Secure RPC \
credentials to login.", user);
					err = PAM_SERVICE_ERR;
					break;
				default:
					err = PAM_SERVICE_ERR;
				}
				return (err);
			}

			orig_uid = geteuid();
			seteuid(uid);
			memset(pwd_buf, 0, sizeof (pwd_buf));
			memset(shpwd_buf, 0, sizeof (shpwd_buf));
			if (getpwnam_r(user, &pwd, pwd_buf,
					sizeof (pwd_buf)) == NULL ||
			    getspnam_r(user, &shpwd, shpwd_buf,
					sizeof (shpwd_buf)) == NULL) {
				seteuid(orig_uid);
				return (PAM_USER_UNKNOWN);
			}
			seteuid(orig_uid);

			if ((shpwd.sp_pwdp != 0) &&
			    (strcmp(shpwd.sp_pwdp, "*NP*") == 0)) {
				syslog(LOG_ALERT,
		"Permissions on the password database may be too restrictive");
				return (PAM_AUTH_ERR);
			}
			enc_passwd = shpwd.sp_pwdp;
#else
			return (PAM_AUTH_ERR);
#endif /* PAM_SECURE_RPC */
		}

		if ((enc_passwd == 0) ||
		    (*enc_passwd == '\0')) {
			if (flags & PAM_DISALLOW_NULL_AUTHTOK)
				return (PAM_AUTH_ERR);
			/* return success if passwords are not required */
			return (PAM_SUCCESS);
		}

		if (strcmp(crypt(password, enc_passwd),
		    enc_passwd) != 0) {
			return (PAM_AUTH_ERR);
		}
		/* success! */
		return (PAM_SUCCESS);
	}
}
