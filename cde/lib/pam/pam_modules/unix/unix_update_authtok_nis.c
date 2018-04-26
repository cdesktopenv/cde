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
/* $XConsortium: unix_update_authtok_nis.c /main/5 1996/05/09 04:36:14 drk $ */
/*
 * Copyright (c) 1992-1996, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_update_authtok_nis.c 1.43     96/02/02 SMI"

#include "unix_headers.h"

#ifdef PAM_NIS

static void	reencrypt_secret(pam_handle_t *, char *, char *, char *,
				int);
static int	update_nisattr(pam_handle_t *, char *, char **,
				struct passwd *, int, int);

int
update_authtok_nis(
	pam_handle_t	*pamh,
	char		*field,
	char		*data[],	/* encrypted new passwd */
					/* or new attribute info */
	char		*old,		/* old passwd: clear */
	char		*new,		/* new passwd: clear */
	struct passwd	*nis_pwd,	/* password structure */
	int		privileged,
	int		nowarn)		/* no compat mode: npd and yp server */
					/* take the same protocol */
{
	int 				ok;
	enum clnt_stat 			ans;
	char 				*domain;
	char 				*master;
	CLIENT 				*client;
	struct timeval 			timeout;
	char				*prognamep;
	char				*usrname;
	int				retcode = PAM_SYSTEM_ERR;
	char			messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	struct yppasswd		yppasswd;

	/* initialize this */
	yppasswd.oldpass = NULL;

	if ((retcode = pam_get_item(pamh, PAM_SERVICE, (void **)&prognamep))
						!= PAM_SUCCESS ||
	    (retcode = pam_get_item(pamh, PAM_USER, (void **)&usrname))
						!= PAM_SUCCESS)
		goto out;

	if (strcmp(field, "passwd") == 0) {
		/*
		 * ck_passwd() already checked the old passwd. It won't get here
		 * if the old passwd is not matched.
		 * We are just preparing the passwd update packet here.
		 */

		if ((yppasswd.oldpass = strdup(old)) == NULL) {
			retcode = PAM_BUF_ERR;
			goto out;
		}

		if (nis_pwd->pw_passwd) {
			memset(nis_pwd->pw_passwd, 0,
					strlen(nis_pwd->pw_passwd));
			free(nis_pwd->pw_passwd);
		}
		nis_pwd->pw_passwd = *data;	/* encrypted new passwd */
	} else {
		/*
		 * prompt for passwd: required for the options
		 * nis_pwd struct will be modified by update_nisattr().
		 * The encrypted passwd remains the same because we are not
		 * changing passwd here.
		 */

		retcode = __pam_get_authtok(pamh, PAM_PROMPT, 0, PASSWORD_LEN,
			PAM_MSG(pamh, 62, "Enter login(NIS) password: "),
			&(yppasswd.oldpass));
		if (retcode != PAM_SUCCESS)
			goto out;

		if ((retcode = update_nisattr(pamh, field, data,
			nis_pwd, privileged, nowarn)) != PAM_SUCCESS) {

			if (retcode == -1) {
				/* finger, shell, or gecos info unchanged */
				retcode = PAM_SUCCESS;
			}
			goto out;
		}
	}

	yppasswd.newpw = *nis_pwd;
	if (yp_get_default_domain(&domain) != 0) {
		syslog(LOG_ERR, "%s%s: can't get domain",
			prognamep, NIS_MSG);
		retcode = PAM_SYSTEM_ERR;
		goto out;
	}

	if (yp_master(domain, "passwd.byname", &master) != 0) {
		syslog(LOG_ERR, "%s%s: can't get master for passwd map",
			prognamep, NIS_MSG);
		retcode = PAM_SYSTEM_ERR;
		goto out;
	}
	client = clnt_create(master, YPPASSWDPROG, YPPASSWDVERS, "udp");
	if (client == NULL) {
		syslog(LOG_ERR,
			"%s%s: couldn't create client to YP master",
			prognamep, NIS_MSG);
		retcode = PAM_SYSTEM_ERR;
		goto out;
	}

	timeout.tv_usec = 0;
	timeout.tv_sec = 55;	/* npd uses 55 seconds */

	ans = CLNT_CALL(client, YPPASSWDPROC_UPDATE, xdr_yppasswd,
		(char *)&yppasswd, xdr_int, (char *)&ok, timeout);
	(void) clnt_destroy(client);
	if (ans != RPC_SUCCESS) {
		sprintf(messages[0], PAM_MSG(pamh, 100,
			"%s%s: couldn't change passwd/attributes"),
			prognamep, NIS_MSG);
		(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
			1, messages, NULL);

		retcode = PAM_PERM_DENIED;
		goto out;
	}

	if (ok != 0) {
		sprintf(messages[0], PAM_MSG(pamh, 101,
			"%s%s: Couldn't change passwd/attributes for %s"),
			prognamep, NIS_MSG, usrname);
		(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
			1, messages, NULL);

		retcode = PAM_PERM_DENIED;
		goto out;
	}

	sprintf(messages[0], PAM_MSG(pamh, 102,
		"NIS(YP) passwd/attributes changed on %s"),
		master);
	(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
		1, messages, NULL);

	reencrypt_secret(pamh, domain, old, new, nowarn);

	retcode = PAM_SUCCESS;
out:
	if (yppasswd.oldpass) {
		memset(yppasswd.oldpass, 0, strlen(yppasswd.oldpass));
		free(yppasswd.oldpass);
	}
	return (retcode);
}


static int
update_nisattr(pam_handle_t *pamh, char *field, char **data,
		struct passwd *nis_pwd, int privileged, int nowarn)
{
	char		*username;
	char		*value;
	char 		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	if (nis_pwd == NULL) {
		if (!nowarn) {
			pam_get_item(pamh, PAM_USER, (void **)&username);
			sprintf(messages[0], PAM_MSG(pamh, 103,
				"System error: no NIS passwd record for %s"),
				username);
			(void) __pam_display_msg(pamh,
				PAM_ERROR_MSG, 1, messages, NULL);
		}
		return (PAM_USER_UNKNOWN);
	}

	if (strcmp(field, "attr") == 0) {

		while (*data != NULL) {
			/* AUTHTOK_DEL: not applicable */

			if ((value = attr_match("AUTHTOK_SHELL", *data))
			    != NULL) {
				if (strcmp(value, "1") != 0) {
					if (!nowarn) {
					    sprintf(messages[0],
					    PAM_MSG(pamh, 104,
		    "%s: System error%s: shell is set illegally"),
					    value, NIS_MSG);
					(void) __pam_display_msg(pamh,
					    PAM_ERROR_MSG, 1,
					    messages, NULL);
					}
					return (PAM_SYSTEM_ERR);
				}
				nis_pwd->pw_shell =
				    getloginshell(pamh, nis_pwd->pw_shell,
					privileged, nowarn);
				/* if NULL, shell unchanged */
				if (nis_pwd->pw_shell == NULL)
					return (-1);
				data++;
				continue;
			}

			if ((value = attr_match("AUTHTOK_HOMEDIR", *data))
			    != NULL) {
				/* home directory */
				if (strcmp(value, "1") != 0) {
					if (!nowarn) {
						sprintf(messages[0],
						PAM_MSG(pamh, 105,
			"System error%s: homedir is set illegally."),
						NIS_MSG);
						(void) __pam_display_msg(
							pamh,
							PAM_ERROR_MSG, 1,
							messages, NULL);
					}
					return (PAM_SYSTEM_ERR);
				}
				nis_pwd->pw_dir =
				    gethomedir(pamh, nis_pwd->pw_dir, nowarn);
				/* if NULL, homedir unchanged */
				if (nis_pwd->pw_dir == NULL)
					return (-1);
				data++;
				continue;
			}

			if ((value = attr_match("AUTHTOK_GECOS", *data))
			    != NULL) {
				/* finger information */
				if (strcmp(value, "1") != 0) {
					if (!nowarn) {
						sprintf(messages[0],
						PAM_MSG(pamh, 106,
				"System error: gecos is set illegally."));
						(void) __pam_display_msg(
							pamh,
							PAM_ERROR_MSG, 1,
							messages, NULL);
					}
					return (PAM_SYSTEM_ERR);
				}
				nis_pwd->pw_gecos =
				    getfingerinfo(pamh, nis_pwd->pw_gecos,
						nowarn);
				/* if NULL, gecos unchanged */
				if (nis_pwd->pw_gecos == NULL)
					return (-1);
				data++;
				continue;
			}
		} /* while */
		return (PAM_SUCCESS);
	}
	return (PAM_AUTHTOK_ERR);
	/* NOTREACHED */
}

/*
 * If the user has a secret key, reencrypt it.
 * Otherwise, be quiet.
 */
static void
reencrypt_secret(pam_handle_t *pamh, char *domain, char *oldpass,
		char *newpass, int nowarn)
{
#ifdef PAM_SECURE_RPC
	char who[MAXNETNAMELEN+1];
	char secret[HEXKEYBYTES+1];
	char public[HEXKEYBYTES+1];
	char crypt[HEXKEYBYTES + KEYCHECKSUMSIZE + 1];
	char pkent[sizeof (crypt) + sizeof (public) + 1];
	char *master;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	getnetname(who);
	if (!getsecretkey(who, secret, oldpass)) {
		/*
		 * Quiet: net is not running secure RPC
		 */
		return;
	}
	if (secret[0] == 0) {
		/*
		 * Quiet: user has no secret key
		 */
		return;
	}
	if (getpublickey(who, public) == FALSE) {
		return;
	}
	(void) memcpy(crypt, secret, HEXKEYBYTES);
	(void) memcpy(crypt + HEXKEYBYTES, secret, KEYCHECKSUMSIZE);
	crypt[HEXKEYBYTES + KEYCHECKSUMSIZE] = 0;
	(void) xencrypt(crypt, newpass);
	(void) sprintf(pkent, "%s:%s", public, crypt);
	if (yp_update(domain, PKMAP, YPOP_STORE,
	    who, strlen(who), pkent, strlen(pkent)) != 0) {

		return;
	}
	if (yp_master(domain, PKMAP, &master) != 0) {
		master = "yp master";	/* should never happen */
	}

	sprintf(messages[0], PAM_MSG(pamh, 107,
		"%s: secret key reencrypted for %s on %s"),
		NIS_MSG, who, master);
	__pam_display_msg(pamh, PAM_TEXT_INFO, 1, messages, NULL);
#else
	return;
#endif /* PAM_SECURE_RPC */

}

#endif /* PAM_NIS */
