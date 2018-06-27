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
/* $XConsortium: unix_utils.c /main/5 1996/05/09 04:36:55 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_utils.c 1.53     95/09/11 SMI"

#include "unix_headers.h"

static char 	*spskip();
static int	special_case();
static int	illegal_input();
static int	copy_passwd_structs(struct passwd **, struct passwd *,
			struct spwd **, struct spwd *);

void		free_passwd_structs(struct passwd *, struct spwd *);

/* ******************************************************************** */
/*									*/
/* 		Utilities Functions					*/
/*									*/
/* ******************************************************************** */

#ifdef PAM_SECURE_RPC

/*
 * Establish the Secure RPC secret key for the given uid using
 * the given password to decrypt the secret key, and store it with
 * the key service.
 *
 * If called with a nonzero 'reestablish' parameter, the key
 * is obtained from the name service, decrypted, and stored
 * even if the keyserver already has a key stored for the uid.
 * If the 'reestablish' parameter is zero, the function will not
 * try to reset the key.  It will return immediately with
 * ESTKEY_ALREADY.
 *
 * Returns one of the following codes:
 *   ESTKEY_ALREADY - reestablish flag was zero, and key was already set.
 *   ESTKEY_SUCCESS - successfully obtained, decrypted, and set the key
 *   ESTKEY_NOCREDENTIALS - the user has no credentials.
 *   ESTKEY_BADPASSWD - the password supplied didn't decrypt the key
 *   ESTKEY_CANTSETKEY - decrypted the key, but couldn't store key with
 *			    the key service.
 *
 * If netnamebuf is a non-NULL pointer, the netname will be returned in
 * netnamebuf, provided that the return status is not
 * ESTKEY_NOCREDENTIALS or ESTKEY_ALREADY.  If non-NULL, the
 * netnamebuf pointer must point to a buffer of length at least
 * MAXNETNAMELEN+1 characters.
 */

int
establish_key(uid, password, reestablish, netnamebuf)
	uid_t   uid;
	char 	*password;
	int	reestablish;
	char    *netnamebuf;
{
	char    netname[MAXNETNAMELEN+1];
	struct 	key_netstarg netst;
	uid_t   orig_uid;

	orig_uid = geteuid();
	if (seteuid(uid) == -1)
		/* can't set uid */
		return (ESTKEY_NOCREDENTIALS);


	if (!reestablish && key_secretkey_is_set()) {
		/* key is already established and we are not to reestablish */
		(void) seteuid(orig_uid);
		return (ESTKEY_ALREADY);
	}


	if (!getnetname(netname)) {
		/* can't construct netname */
		(void) seteuid(orig_uid);
		return (ESTKEY_NOCREDENTIALS);
	}

	if (!getsecretkey(netname, (char *) &(netst.st_priv_key), password)) {
		/* no secret key */
		(void) seteuid(orig_uid);
		return (ESTKEY_NOCREDENTIALS);
	}

	if (netnamebuf) {
		/* return copy of netname in caller's buffer */
		(void) strcpy(netnamebuf, netname);
	}

	if (netst.st_priv_key[0] == 0) {
		/* password does not decrypt secret key */
		(void) seteuid(orig_uid);
		return (ESTKEY_BADPASSWD);
	}


	/* secret key successfully decrypted at this point */

	/* store with key service */

	if ((netst.st_netname = strdup(netname)) == NULL) {
		(void) seteuid(orig_uid);
		return (PAM_BUF_ERR);
	}
	(void) memset(netst.st_pub_key, 0, HEXKEYBYTES);

	if (key_setnet(&netst) < 0) {
		free(netst.st_netname);
		(void) seteuid(orig_uid);
		return (ESTKEY_CANTSETKEY);
	}

	free(netst.st_netname);
	(void) seteuid(orig_uid);
	return (ESTKEY_SUCCESS);
}

#endif /* PAM_SECURE_RPC */

/* ******************************************************************** */
/*									*/
/* 		Utilities Functions					*/
/*									*/
/* ******************************************************************** */

/*
 * ck_perm():
 * 	Check the permission of the user specified by "usrname".
 *
 * 	It returns PAM_PERM_DENIED if (1) the user has a NULL pasword or
 * 	shadow password file entry, or (2) the caller is not root and
 *	its uid is not equivalent to the uid specified by the user's
 *	password file entry.
 */

int
ck_perm(pamh, repository, domain, pwd, shpwd, privileged, passwd_res, uid,
	debug, nowarn)
	pam_handle_t *pamh;
	int	repository;
	char *domain;
	struct passwd **pwd;
	struct spwd **shpwd;
	int *privileged;
	void **passwd_res;
	uid_t uid;
	int debug;
	int nowarn;
{
	FILE	*pwfp, *spfp;
	char	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	struct passwd local_pwd, *local_pwdp;
	struct spwd local_shpwd, *local_shpwdp;
	char pwdbuf[1024], shpwdbuf[1024];
	char *prognamep;
	char *usrname;
	int retcode = 0;
#ifdef PAM_NISPLUS
	char    buf[NIS_MAXNAMELEN+1];
	nis_name local_principal;
	nis_name pwd_domain;
#endif

	if (debug)
		syslog(LOG_DEBUG,
			"ck_perm() called: repository=%s",
			repository_to_string(repository));

	if ((retcode = pam_get_item(pamh, PAM_SERVICE, (void **)&prognamep))
							!= PAM_SUCCESS ||
	    (retcode = pam_get_item(pamh, PAM_USER, (void **)&usrname))
							!= PAM_SUCCESS) {
		*pwd = NULL;  *shpwd = NULL;
		return (retcode);
	}

	if (repository == PAM_REP_FILES) {
		if (((pwfp = fopen(PASSWD, "r")) == NULL) ||
		    ((spfp = fopen(SHADOW, "r")) == NULL)) {
			*pwd = NULL;  *shpwd = NULL;
			syslog(LOG_ERR,
				"ck_perm: can not open passwd/shadow file");
			return (PAM_PERM_DENIED);
		}
		while ((local_pwdp = fgetpwent_r(pwfp, &local_pwd, pwdbuf,
					sizeof (pwdbuf))) != NULL)
			if (strcmp(local_pwd.pw_name, usrname) == 0)
				break;
		while ((local_shpwdp = fgetspent_r(spfp, &local_shpwd,
				shpwdbuf, sizeof (shpwdbuf))) != NULL)
			if (strcmp(local_shpwd.sp_namp, usrname) == 0)
				break;
		(void) fclose(pwfp);
		(void) fclose(spfp);
		if (local_pwdp == NULL || local_shpwdp == NULL) {
			*pwd = NULL;  *shpwd = NULL;
			return (PAM_USER_UNKNOWN);
		}

		if (uid != 0 && uid != local_pwd.pw_uid) {
			/*
			 * Change passwd for another person:
			 * Even if you are nis+ admin, you can't do anything
			 * locally. Don't bother to continue.
			 */
				if (!nowarn) {
					sprintf(messages[0], PAM_MSG(pamh, 140,
					"%s%s: Permission denied"), prognamep,
						UNIX_MSG);
					sprintf(messages[1], PAM_MSG(pamh, 141,
				"%s%s: Can't change local passwd file\n"),
						prognamep, UNIX_MSG);
					(void) __pam_display_msg(
						pamh, PAM_ERROR_MSG, 2,
						messages, NULL);
				}
				*pwd = NULL;  *shpwd = NULL;
				return (PAM_PERM_DENIED);
		}
		return (copy_passwd_structs(pwd, local_pwdp,
						shpwd, local_shpwdp));
	}

#ifdef PAM_NIS
	if (repository == PAM_REP_NIS) {
		/*
		 * Special case root: don't bother to get root from nis(yp).
		 */
		if (strcmp(usrname, "root") == 0) {
			*pwd = NULL;	*shpwd = NULL;
			return (PAM_USER_UNKNOWN);
		}

		/* get pwd struct from yp */

		local_pwdp = getpwnam_from(usrname, PAM_REP_NIS);
		local_shpwdp = getspnam_from(usrname, PAM_REP_NIS);

		if (local_pwdp == NULL || local_shpwdp == NULL)
			return (PAM_USER_UNKNOWN);

		if (uid != local_pwdp->pw_uid) {
			if (!nowarn) {
				sprintf(messages[0], PAM_MSG(pamh, 140,
					"%s%s: Permission denied"), prognamep,
					NIS_MSG);
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			}
			*pwd = NULL;	*shpwd = NULL;
			return (PAM_PERM_DENIED);
		}
		return (copy_passwd_structs(pwd, local_pwdp,
						shpwd, local_shpwdp));
	}
#endif /* PAM_NIS */

#ifdef PAM_NISPLUS
	if (repository == PAM_REP_NISPLUS) {
		/*
		 * Special case root: don't bother to get root from nis+.
		 */
		if (strcmp(usrname, "root") == 0) {
			*pwd = NULL;	*shpwd = NULL;
			return (PAM_USER_UNKNOWN);
		}

		if (debug)
			syslog(LOG_DEBUG, "ck_perm(): NIS+ domain=%s", domain);

		/*
		 * We need to use user id to
		 * make any nis+ request. But don't give up the super
		 * user power yet. It may be needed elsewhere.
		 */
		(void) setuid(0);	/* keep real user id as root */
		(void) seteuid(uid);

		local_shpwdp = getspnam_from(usrname, PAM_REP_NISPLUS);
		local_pwdp = getpwnam_from(usrname, PAM_REP_NISPLUS);

		if (local_pwdp == NULL || local_shpwdp == NULL)
			return (PAM_USER_UNKNOWN);

		/*
		 * local_principal is internal, it is not meant to be free()ed
		 */
		local_principal = nis_local_principal();

		if ((9 + strlen(usrname) + strlen(domain) + PASSTABLELEN) >
		    (size_t) NIS_MAXNAMELEN) {
			sprintf(messages[0], PAM_MSG(pamh, 140,
			    "%s%s: Permission denied"), prognamep, NISPLUS_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
			*pwd = NULL;	*shpwd = NULL;
			return (PAM_PERM_DENIED);
		}

		sprintf(buf, "[name=%s],%s.%s", usrname, PASSTABLE, domain);
		if (buf[strlen(buf) - 1] != '.')
			(void) strcat(buf, ".");

		/*
		 * We must use an authenticated handle to get the cred
		 * table information for the user we want to modify the
		 * cred info for. If we can't even read that info, we
		 * definitely wouldn't have modify permission. Well..
		 */
		*passwd_res = (void *) nis_list(buf,
			USE_DGRAM+FOLLOW_LINKS+FOLLOW_PATH+MASTER_ONLY,
			NULL, NULL);
		if (((nis_result *)(*passwd_res))->status != NIS_SUCCESS) {
			sprintf(messages[0], PAM_MSG(pamh, 140,
			    "%s%s: Permission denied"), prognamep, NISPLUS_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
			*pwd = NULL;	*shpwd = NULL;
			return (PAM_PERM_DENIED);
		}

		pwd_domain =
			NIS_RES_OBJECT((nis_result *)(*passwd_res))->zo_domain;
		if (strcmp(nis_leaf_of(pwd_domain), "org_dir") == 0) {
			pwd_domain = nis_domain_of(
			NIS_RES_OBJECT((nis_result *)(*passwd_res))->zo_domain);

		}
		*privileged = __nis_isadmin(local_principal, "passwd",
			pwd_domain);

		return (copy_passwd_structs(pwd, local_pwdp,
						shpwd, local_shpwdp));
	}
#endif /* PAM_NISPLUS */

	if (!nowarn) {
		sprintf(messages[0], PAM_MSG(pamh, 142,
		"%s%s: System error: repository out of range"),
			prognamep, UNIX_MSG);
		(void) __pam_display_msg(pamh, PAM_ERROR_MSG, 1,
			messages, NULL);
	}
	*pwd = NULL;	*shpwd = NULL;
	return (PAM_PERM_DENIED);
}

/*
 * attr_match():
 *
 *	Check if the attribute name in string s1 is equivalent to
 *	that in string s2.
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 */

char *
attr_match(s1, s2)
	char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '=')
		return (s2);
	if (*s1 == '\0' && *(s2-1) == '=')
		return (s2);
	return (NULL);
}

/*
 * attr_find():
 *
 *	Check if the attribute name in string s1 is present in the
 *	attribute=value pairs array pointed by s2.
 *	s1 is name
 *	s2 is an array of name=value pairs
 *	if s1 match the name of any one of the name in the name=value pairs
 *	pointed by s2, then 1 is returned; else 0 is returned
 */

int
attr_find(s1, s2)
	char *s1, *s2[];
{
	int 	i;
	char 	*sa, *sb;

	i = 0;
	while (s2[i] != NULL) {
		sa = s1;
		sb = s2[i];
		while (*sa++ == *sb++) {
			if ((*sa == '\0') && (*sb == '='))
				return (1); /* find */
		}
		i++;
	}

	return (0); /* not find */
}

/*
 * free_setattr():
 *	free storage pointed by "setattr"
 */

void
free_setattr(setattr)
	char * setattr[];
{
	int i;

	for (i = 0; setattr[i] != NULL; i++)
		free(setattr[i]);

}

/*
 * setup_attr():
 *	allocate memory and copy in attribute=value pair
 *	into the array of attribute=value pairs pointed to
 *	by "dest_attr"
 */

void
setup_attr(dest_attr, k, attr, value)
	char *dest_attr[];
	int k;
	char attr[];
	char value[];
{
	if (attr != NULL) {
		dest_attr[k] = (char *)calloc(PAM_MAX_ATTR_SIZE, sizeof (char));
		(void) strncpy(dest_attr[k], attr, PAM_MAX_ATTR_SIZE);
		(void) strncat(dest_attr[k], value, PAM_MAX_ATTR_SIZE);
	} else
		dest_attr[k] = NULL;
}

#ifdef PAM_NISPLUS
static char *
spskip(p)
	char *p;
{
	while (*p && *p != ':' && *p != '\n')
		++p;
	if (*p == '\n')
		*p = '\0';
	else if (*p)
		*p++ = '\0';
	return (p);
}

void
nisplus_populate_age(enobj, sp)
	struct nis_object *enobj;
	struct spwd *sp;
{
	char *oldage, *p, *end;
	long x;

	/*
	 * shadow (col 7)
	 */

	sp->sp_lstchg = -1;
	sp->sp_min = -1;
	sp->sp_max = -1;
	sp->sp_warn = -1;
	sp->sp_inact = -1;
	sp->sp_expire = -1;
	sp->sp_flag = 0;

	if ((p = ENTRY_VAL(enobj, 7)) == NULL)
		return;
	oldage = strdup(p);

	p = oldage;

	x = strtol(p, &end, 10);
	if (end != memchr(p, ':', strlen(p)))
		return;
	if (end != p)
		sp->sp_lstchg = x;

	p = spskip(p);
	x = strtol(p, &end, 10);
	if (end != memchr(p, ':', strlen(p)))
		return;
	if (end != p)
		sp->sp_min = x;

	p = spskip(p);
	x = strtol(p, &end, 10);
	if (end != memchr(p, ':', strlen(p)))
		return;
	if (end != p)
		sp->sp_max = x;

	p = spskip(p);
	x = strtol(p, &end, 10);
	if (end != memchr(p, ':', strlen(p)))
		return;
	if (end != p)
		sp->sp_warn = x;

	p = spskip(p);
	x = strtol(p, &end, 10);
	if (end != memchr(p, ':', strlen(p)))
		return;
	if (end != p)
		sp->sp_inact = x;

	p = spskip(p);
	x = strtol(p, &end, 10);
	if (end != memchr(p, ':', strlen(p)))
		return;
	if (end != p)
		sp->sp_expire = x;

	p = spskip(p);
	x = strtol(p, &end, 10);
	if ((end != memchr(p, ':', strlen(p))) &&
	    (end != memchr(p, '\n', strlen(p))))
		return;
	if (end != p)
		sp->sp_flag = x;

	free(oldage);
}
#endif /* PAM_NISPLUS */

/*
 * getloginshell() displays old login shell and asks for new login shell.
 *	The new login shell is then returned to calling function.
 */
char *
getloginshell(pamh, oldshell, privileged, nowarn)
	pam_handle_t *pamh;
	char *oldshell;
	int privileged;
	int nowarn;
{
	char newshell[PAM_MAX_MSG_SIZE];
	char *cp, *valid, *getusershell();
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	struct pam_response *ret_resp = (struct pam_response *)0;

	if (oldshell == 0 || *oldshell == '\0')
		oldshell = DEFSHELL;

	if (privileged == 0) {
		mutex_lock(&_priv_lock);
		setusershell();
		for (valid = getusershell(); valid; valid = getusershell())
			if (strcmp(oldshell, valid) == 0)
				break;
		mutex_unlock(&_priv_lock);
		if (valid == NULL) {
			if (!nowarn) {
				sprintf(messages[0], PAM_MSG(pamh, 143,
				    "Cannot change from restricted shell %s"),
				    oldshell);
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			}
			return (NULL);
		}
	}
	sprintf(messages[0],
		PAM_MSG(pamh, 144, "Old shell: %s"), oldshell);
	(void) __pam_display_msg(pamh, PAM_TEXT_INFO, 1, messages, NULL);
	sprintf(messages[0],
		PAM_MSG(pamh, 145, "New shell: "));
	(void) __pam_get_input(pamh, PAM_PROMPT_ECHO_ON,
		1, messages, NULL, &ret_resp);
	strncpy(newshell, ret_resp->resp, PAM_MAX_MSG_SIZE);
	newshell[PAM_MAX_RESP_SIZE-1] = '\0';
	__pam_free_resp(1, ret_resp);

	cp = strchr(newshell, '\n');
	if (cp)
		*cp = '\0';
	if (newshell[0] == '\0' || (strcmp(newshell, oldshell) == 0)) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 146,
				"Login shell unchanged."));
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		return (NULL);
	}
	/*
	 * XXX:
	 * Keep in mind that, for whatever this validation is worth,
	 * a root on a machine can edit /etc/shells and get any shell
	 * accepted as a valid shell in the NIS+ table.
	 */
	mutex_lock(&_priv_lock);
	setusershell();
	if (!privileged) {
		for (valid = getusershell(); valid; valid = getusershell()) {
			/*
			 * Allow user to give shell w/o preceding pathname.
			 */
			if (newshell[0] == '/') {
				cp = valid;
			} else {
				cp = strrchr(valid, '/');
				if (cp == 0)
					cp = valid;
				else
					cp++;
			}
			if (strcmp(newshell, cp) == 0) {
				strncpy(newshell, valid, strlen(valid));
				break;
			}
		}
	}
	mutex_unlock(&_priv_lock);

	if (newshell == 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 147,
				"%s is unacceptable as a new shell"),
				newshell);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		__pam_free_resp(1, ret_resp);
		return (NULL);
	}
	if (access(newshell, X_OK) < 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 148,
				"warning: %s is unavailable on this machine"),
				newshell);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
	}
	return (strdup(newshell));
}

/*
 * Get name.
 */
char *
getfingerinfo(pamh, old_gecos, nowarn)
	pam_handle_t *pamh;
	char	*old_gecos;
	int nowarn;
{
	char 	new_gecos[PAM_MAX_MSG_SIZE];
	char	messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	struct pam_response *ret_resp = (struct pam_response *)0;

	if (!nowarn) {
		sprintf(messages[0], PAM_MSG(pamh, 149,
			"Default values are printed inside of '[]'."));
		sprintf(messages[1], PAM_MSG(pamh, 150,
			"To accept the default, type <return>."));
		sprintf(messages[2], PAM_MSG(pamh, 151,
			"To have a blank entry, type the word 'none'."));
		(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
			3, messages, NULL);
	}

	/*
	 * Get name.
	 */
	do {
		sprintf(messages[0], " ");
		(void) __pam_display_msg(pamh, PAM_TEXT_INFO, 1,
			messages, NULL);
		sprintf(messages[0], PAM_MSG(pamh, 152,
			"Name [%s]: "), old_gecos);
		(void) __pam_get_input(pamh, PAM_PROMPT_ECHO_ON,
			1, messages, NULL, &ret_resp);
		strncpy(new_gecos, ret_resp->resp, PAM_MAX_MSG_SIZE);
		new_gecos[PAM_MAX_MSG_SIZE-1] = '\0';
		__pam_free_resp(1, ret_resp);
		if (special_case(new_gecos, old_gecos))
			break;
	} while (illegal_input(pamh, new_gecos, nowarn));
	if (strcmp(new_gecos, old_gecos) == 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 153,
				"Finger information unchanged."));
			(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
				1, messages, NULL);
		}
		return (NULL);
	}
	return (strdup(new_gecos));
}

/*
 * Get Home Dir.
 */
char *
gethomedir(pamh, olddir, nowarn)
	pam_handle_t *pamh;
	char *olddir;
	int nowarn;
{
	char newdir[PAM_MAX_MSG_SIZE];
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	struct pam_response *ret_resp = (struct pam_response *)0;

	if (!nowarn) {
		sprintf(messages[0], PAM_MSG(pamh, 149,
			"Default values are printed inside of '[]'."));
		sprintf(messages[1], PAM_MSG(pamh, 150,
			"To accept the default, type <return>."));
		sprintf(messages[2], PAM_MSG(pamh, 151,
			"To have a blank entry, type the word 'none'."));
		(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
			3, messages, NULL);
	}
	do {
		sprintf(messages[0], " ");
		(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
				1, messages, NULL);
		sprintf(messages[0], PAM_MSG(pamh, 154,
			"Home Directory [%s]: "), olddir);
		(void) __pam_get_input(pamh, PAM_PROMPT_ECHO_ON,
			1, messages, NULL, &ret_resp);
		strncpy(newdir, ret_resp->resp, PAM_MAX_MSG_SIZE);
		newdir[PAM_MAX_MSG_SIZE-1] = '\0';
		__pam_free_resp(1, ret_resp);
		if (special_case(newdir, olddir))
			break;
	} while (illegal_input(pamh, newdir, nowarn));
	if (strcmp(newdir, olddir) == 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 155,
				"Homedir information unchanged."));
			(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
				1, messages, NULL);
		}
		return (NULL);
	}
	return (strdup(newdir));
}

char *
repository_to_string(int repository)
{
	/* if NISPLUS and FILES */
	switch (repository) {
	case (PAM_REP_FILES | PAM_REP_NISPLUS):
		return ("files and nisplus");
	case (PAM_REP_FILES | PAM_REP_NIS):
		return ("files and nis");
	case PAM_REP_NISPLUS:
		return ("nisplus");
	case PAM_REP_NIS:
		return ("nis");
	case PAM_REP_FILES:
		return ("files");
	case PAM_REP_DEFAULT:
		return ("default");
	default:
		return ("bad repository");
	}
}

/*
 * Prints an error message if a ':' or a newline is found in the string.
 * A message is also printed if the input string is too long.
 * The password sources use :'s as separators, and are not allowed in the "gcos"
 * field.  Newlines serve as delimiters between users in the password source,
 * and so, those too, are checked for.  (I don't think that it is possible to
 * type them in, but better safe than sorry)
 *
 * Returns '1' if a colon or newline is found or the input line is too long.
 */
static int
illegal_input(pamh, input_str, nowarn)
	pam_handle_t *pamh;
	char *input_str;
	int nowarn;
{
	char *ptr;
	int error_flag = 0;
	int length = (int)strlen(input_str);
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	if (strchr(input_str, ':')) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 156,
				"':' is not allowed."));
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		error_flag = 1;
	}
	if (input_str[length-1] != '\n') {
		/* the newline and the '\0' eat up two characters */
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 157,
			    "Maximum number of characters allowed is %d."),
			    PAM_MAX_MSG_SIZE-2);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		error_flag = 1;
	}
	/*
	 * Delete newline by shortening string by 1.
	 */
	input_str[length-1] = '\0';
	/*
	 * Don't allow control characters, etc in input string.
	 */
	for (ptr = input_str; *ptr != '\0'; ptr++) {
		/* 040 is ascii char "space" */
		if ((int) *ptr < 040) {
			if (!nowarn) {
				sprintf(messages[0], PAM_MSG(pamh, 158,
				    "Control characters are not allowed."));
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			}
			error_flag = 1;
			break;
		}
	}
	return (error_flag);
}



/*
 *  special_case returns true when either the default is accepted
 *  (str = '\n'), or when 'none' is typed.  'none' is accepted in
 *  either upper or lower case (or any combination).  'str' is modified
 *  in these two cases.
 */
static int
special_case(str, default_str)
	char *str, *default_str;
{
	static char word[] = "none\n";
	char *ptr, *wordptr;

	/*
	 *  If the default is accepted, then change the old string do the
	 *  default string.
	 */
	if (*str == '\n') {
		(void) strcpy(str, default_str);
		return (1);
	}
	/*
	 *  Check to see if str is 'none'.  (It is questionable if case
	 *  insensitivity is worth the hair).
	 */
	wordptr = word - 1;
	for (ptr = str; *ptr != '\0'; ++ptr) {
		++wordptr;
		if (*wordptr == '\0')	/* then words are different sizes */
			return (0);
		if (*ptr == *wordptr)
			continue;
		if (isupper(*ptr) && (tolower(*ptr) == *wordptr))
			continue;
		/*
		 * At this point we have a mismatch, so we return
		 */
		return (0);
	}
	/*
	 * Make sure that words are the same length.
	 */
	if (*(wordptr+1) != '\0')
		return (0);
	/*
	 * Change 'str' to be the null string
	 */
	*str = '\0';
	return (1);
}

static int
copy_passwd_structs(struct passwd **pwd, struct passwd *local_pwd,
		struct spwd **shpwd, struct spwd *local_shpwd)
{

	/* copy the passwd information */
	if ((*pwd = (struct passwd *)
		calloc(1, sizeof (struct passwd))) == NULL)
		return (PAM_BUF_ERR);
	if (local_pwd->pw_name) {
		if (((*pwd)->pw_name = strdup(local_pwd->pw_name)) == NULL)
			goto out;
	}
	if (local_pwd->pw_passwd) {
		if (((*pwd)->pw_passwd = strdup(local_pwd->pw_passwd)) == NULL)
			goto out;
	}

	(*pwd)->pw_uid = local_pwd->pw_uid;
	(*pwd)->pw_gid = local_pwd->pw_gid;

	if (local_pwd->pw_gecos) {
		if (((*pwd)->pw_gecos = strdup(local_pwd->pw_gecos)) == NULL)
			goto out;
	}
	if (local_pwd->pw_dir) {
		if (((*pwd)->pw_dir = strdup(local_pwd->pw_dir)) == NULL)
			goto out;
	}
	if (local_pwd->pw_shell) {
		if (((*pwd)->pw_shell = strdup(local_pwd->pw_shell)) == NULL)
			goto out;
	}

	/* copy the shadow passwd information */
	if ((*shpwd = (struct spwd *)
		calloc(1, sizeof (struct spwd))) == NULL)
		goto out;
	**shpwd = *local_shpwd;
	if (local_shpwd->sp_namp) {
		if (((*shpwd)->sp_namp = strdup(local_shpwd->sp_namp)) == NULL)
			goto out;
	}
	if (local_shpwd->sp_pwdp) {
		if (((*shpwd)->sp_pwdp = strdup(local_shpwd->sp_pwdp)) == NULL)
			goto out;
	}

	return (PAM_SUCCESS);
out:
	free_passwd_structs(*pwd, *shpwd);
	return (PAM_BUF_ERR);
}

void
free_passwd_structs(struct passwd *pwd, struct spwd *shpwd)
{
	if (pwd) {
		if (pwd->pw_name)
			free(pwd->pw_name);
		if (pwd->pw_passwd) {
			memset(pwd->pw_passwd, 0, strlen(pwd->pw_passwd));
			free(pwd->pw_passwd);
		}
		if (pwd->pw_gecos)
			free(pwd->pw_gecos);
		if (pwd->pw_dir)
			free(pwd->pw_dir);
		if (pwd->pw_shell)
			free(pwd->pw_shell);
		free(pwd);
	}

	if (shpwd) {
		if (shpwd->sp_namp)
			free(shpwd->sp_namp);
		if (shpwd->sp_pwdp) {
			memset(shpwd->sp_pwdp, 0, strlen(shpwd->sp_pwdp));
			free(shpwd->sp_pwdp);
		}
		free(shpwd);
	}
}
