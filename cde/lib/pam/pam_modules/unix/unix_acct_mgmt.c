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
/* $XConsortium: unix_acct_mgmt.c /main/5 1996/05/09 04:32:39 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_acct_mgmt.c 1.35     95/12/08 SMI"

#include "unix_headers.h"

static void
unix_cleanup(
	pam_handle_t *pamh,
	void *data,
	int pam_status)
{
	free((unix_authtok_data *)data);
}

/*
 * check_for_login_inactivity	- Check for login inactivity
 *
 */

static int
check_for_login_inactivity(
	struct 	passwd 	pwd,
	struct 	spwd 	shpwd)
{
	int		fdl;
	struct lastlog	ll;
	int		retval;
	long long	offset;

	offset = (long long) pwd.pw_uid * (long long) sizeof (struct lastlog);

	if ((fdl = open(LASTLOG, O_RDWR|O_CREAT, 0444)) >= 0) {
		/*
		 * Read the last login (ll) time
		 */
		if (llseek(fdl, offset, SEEK_SET) != offset) {
			/*
			 * XXX uid too large for database
			 */
			return (0);
		}

		retval = read(fdl, (char *)&ll, sizeof (ll));

		/* Check for login inactivity */

		if ((shpwd.sp_inact > 0) && (retval == sizeof (ll)) &&
		    ll.ll_time) {
			if (((ll.ll_time / DAY) + shpwd.sp_inact) < DAY_NOW) {
				/*
				 * Account inactive for too long
				 */
				(void) close(fdl);
				return (1);
			}
		}

		(void) close(fdl);
	}
	return (0);
}

/*
 * new_password_check()
 *
 * check to see if the user needs to change their password
 */

static int
new_password_check(pwd, shpwd, flags)
	struct 	passwd 	*pwd;
	struct 	spwd 	*shpwd;
	int 	flags;
{
	int	now  = DAY_NOW;

	/*
	 * We want to make sure that we change the password only if
	 * passwords are required for the system, the user does not
	 * have a password, AND the user's NULL password can be changed
	 * according to its password aging information
	 */

	if ((flags & PAM_DISALLOW_NULL_AUTHTOK) != 0) {
		if (shpwd->sp_pwdp[0] == '\0') {
			if ((pwd->pw_uid != 0) &&
				((shpwd->sp_max == -1) ||
				(shpwd->sp_lstchg > now) ||
				((now >= shpwd->sp_lstchg + shpwd->sp_min) &&
				(shpwd->sp_max >= shpwd->sp_min)))) {
					return (PAM_NEW_AUTHTOK_REQD);
			}
		}
	}
	return (PAM_SUCCESS);
}

/*
 * perform_passwd_aging_check
 *		- Check for password exipration.
 */

static	int
perform_passwd_aging_check(
	pam_handle_t *pamh,
	struct 	spwd 	shpwd,
	int	flags)
{
	int 	now = DAY_NOW;
	int	Idleweeks = -1;
	char	*ptr;
	char	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];


	if (defopen(LOGINADMIN) == 0) {
		if ((ptr = defread("IDLEWEEKS=")) != NULL)
			Idleweeks = atoi(ptr);
		(void) defopen(NULL);
	}

	if ((shpwd.sp_lstchg == 0) ||
	    ((shpwd.sp_max >= 0) &&
	    (now > (shpwd.sp_lstchg + shpwd.sp_max)) &&
	    (shpwd.sp_max >= shpwd.sp_min))) {
		if ((Idleweeks == 0) ||
		    ((Idleweeks > 0) &&
		    (now > (shpwd.sp_lstchg + (7 * Idleweeks))))) {
			if (!(flags & PAM_SILENT)) {
			    strcpy(messages[0], PAM_MSG(pamh, 20,
			    "Your password has been expired for too long."));
			    strcpy(messages[1], PAM_MSG(pamh, 21,
			    "Please contact the system administrator"));
			    __pam_display_msg(pamh, PAM_ERROR_MSG,
				2, messages, NULL);
			}
			return (PAM_AUTHTOK_EXPIRED);
		} else {
			return (PAM_NEW_AUTHTOK_REQD);
		}
	}
	return (PAM_SUCCESS);
}

/*
 * warn_user_passwd_will_expire	- warn the user when the password will
 *					  expire.
 */

static void
warn_user_passwd_will_expire(
	pam_handle_t *pamh,
	struct 	spwd shpwd)
{
	int 	now	= DAY_NOW;
	char	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	int	days;

	if ((shpwd.sp_warn > 0) && (shpwd.sp_max > 0) &&
	    (now + shpwd.sp_warn) >= (shpwd.sp_lstchg + shpwd.sp_max)) {
		days = (shpwd.sp_lstchg + shpwd.sp_max) - now;
		if (days <= 0)
			sprintf(messages[0], PAM_MSG(pamh, 22,
			    "Your password will expire within 24 hours."));
		else if (days == 1)
			sprintf(messages[0], PAM_MSG(pamh, 23,
			    "Your password will expire in %d day."), days);
		else
			sprintf(messages[0], PAM_MSG(pamh, 24,
			    "Your password will expire in %d days."), days);
		__pam_display_msg(pamh, PAM_TEXT_INFO, 1, messages, NULL);
	}
}


/*
 * pam_sm_acct_mgmt	- 	main account managment routine.
 *			  Returns: module error or specific error on failure
 */

int
pam_sm_acct_mgmt(
	pam_handle_t	*pamh,
	int	flags,
	int	argc,
	const char	**argv)
{
	struct 	spwd shpwd;
	struct 	passwd pwd;
	char	pwd_buf[1024];
	char	shpwd_buf[1024];
	int 	error = PAM_ACCT_EXPIRED;
	char    *user;
	int	i;
	int	debug = 0;
	int	nowarn = 0;
	uid_t	orig_uid;
	unix_authtok_data *status;

	for (i = 0; i < argc; i++) {
		if (strcasecmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcasecmp(argv[i], "nowarn") == 0) {
			nowarn = 1;
			flags = flags | PAM_SILENT;
		} else {
			syslog(LOG_ERR,
				"UNIX pam_sm_acct_mgmt: illegal option %s",
				argv[i]);
		}
	}

	if ((error = pam_get_item(pamh, PAM_USER, (void **)&user))
							!= PAM_SUCCESS)
		goto out;

	if (user == NULL) {
		error = PAM_USER_UNKNOWN;
		goto out;
	}

	/*
	 * Get the password and shadow password entries
	 */
	if (getpwnam_r(user, &pwd, pwd_buf, sizeof (pwd_buf)) == NULL ||
	    getspnam_r(user, &shpwd, shpwd_buf, sizeof (shpwd_buf)) == NULL) {
		error = PAM_USER_UNKNOWN;
		goto out;
	}

	if (shpwd.sp_pwdp != NULL && strcmp(shpwd.sp_pwdp, "*NP*") == 0) {
		orig_uid = geteuid();
		seteuid(pwd.pw_uid);
		memset(pwd_buf, 0, sizeof (pwd_buf));
		memset(shpwd_buf, 0, sizeof (shpwd_buf));
		if (getpwnam_r(user, &pwd, pwd_buf, sizeof (pwd_buf)) == NULL ||
		    getspnam_r(user, &shpwd, shpwd_buf, sizeof (shpwd_buf))
								== NULL) {
			error = PAM_USER_UNKNOWN;
			seteuid(orig_uid);
			goto out;
		}
		seteuid(orig_uid);
	}

	/*
	 * Check for account expiration
	 */
	if (shpwd.sp_expire > 0 &&
	    shpwd.sp_expire < DAY_NOW) {
		error = PAM_ACCT_EXPIRED;
		goto out;
	}

	/*
	 * Check for excessive login account inactivity
	 */
	if (check_for_login_inactivity(pwd, shpwd)) {
		error = PAM_PERM_DENIED;
		goto out;
	}

	/*
	 * Check to see if the user needs to change their password
	 */
	if (error = new_password_check(&pwd, &shpwd, flags)) {
		goto out;
	}

	/*
	 * Check to make sure password aging information is okay
	 */
	if ((error = perform_passwd_aging_check(pamh, shpwd, flags))
							!= PAM_SUCCESS) {
		goto out;
	}

	/*
	 * Finally, warn the user if their password is about to expire.
	 */
	if (!(flags & PAM_SILENT)) {
		warn_user_passwd_will_expire(pamh, shpwd);
	}

	/*
	 * All done, return Success
	 */
	error = PAM_SUCCESS;

out:

	memset(shpwd_buf, 0, sizeof (shpwd_buf));
	/* store the password aging status in the pam handle */
	if (pam_get_data(pamh, UNIX_AUTHTOK_DATA, (void **)&status)
							!= PAM_SUCCESS) {
		if ((status = (unix_authtok_data *)calloc
			(1, sizeof (unix_authtok_data))) == NULL) {
			return (PAM_BUF_ERR);
		}
	}
	status->age_status = error;
	if (pam_set_data(pamh, UNIX_AUTHTOK_DATA, status, unix_cleanup)
						!= PAM_SUCCESS) {
		free(status);
		return (PAM_SERVICE_ERR);
	}
	return (error);
}
