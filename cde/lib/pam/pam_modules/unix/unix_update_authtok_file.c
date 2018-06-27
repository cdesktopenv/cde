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
/* $XConsortium: unix_update_authtok_file.c /main/5 1996/05/09 04:35:55 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_update_authtok_file.c 1.26     95/09/11 SMI"

#include "unix_headers.h"

static int	update_spent(pam_handle_t *, char *, char **,
			struct passwd *, struct spwd *, int *, int, int);
static int	process_passwd(pam_handle_t *, char *, char *,
			struct passwd *, int);

/*
 * update_authtok_file():
 * 	To update the authentication token file.
 *
 *	This function is called by either __set_authtoken_attr() to
 *	update the token attributes or pam_chauthtok() to update the
 *	authentication token.  The parameter "field" has to be specified
 * 	as "attr" if the caller wants to update token attributes, and
 *	the attribute-value pairs to be set needs to be passed in by parameter
 * 	"data".  If the function is called to update authentication
 *	token itself, then "field" needs to be specified as "passwd"
 * 	and the new authentication token has to be passed in by "data".
 */

int
update_authtok_file(pamh, field, data, unix_pwd, privileged, nowarn)
	pam_handle_t *pamh;
	char *field;
	char *data[];
	struct passwd *unix_pwd;
	int privileged;
	int nowarn;
{
	char *prognamep;
	char *usrname;
	struct stat	buf;
	int	found = 0;
	FILE 		*tsfp, *spfp;
	struct spwd	unix_sp;
	char		spbuf[1024];
	char 		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	int		passwd_flag = 0; /* attrs in shadow or passwd file */
	int		retcode;

	if ((retcode = pam_get_item(pamh, PAM_SERVICE, (void **)&prognamep))
							!= PAM_SUCCESS ||
	    (retcode = pam_get_item(pamh, PAM_USER, (void **)&usrname))
							!= PAM_SUCCESS)
		return (retcode);

	/*
	 * XXX:
	 * Assume effective UID already set to 0
	 * so we can update the passwd file.
	 *
	 * This will be the last update (after nis/nis+)
	 */

	errno = 0;

	/* lock the password file */
	if (lckpwdf() != 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 90,
			"%s%s: Password database busy. Try again later."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		return (PAM_AUTHTOK_LOCK_BUSY);
	}

	/* Mode of the shadow file should be 400 or 000 */
	if (stat(SHADOW, &buf) < 0) {
		syslog(LOG_ERR, "%s: stat of shadow file failed",
			prognamep);
		(void) ulckpwdf();
		return (PAM_AUTHTOK_ERR);
	}

	(void) umask(S_IAMB & ~(buf.st_mode & S_IRUSR));
	if ((tsfp = fopen(SHADTEMP, "w")) == NULL) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		(void) ulckpwdf();
		return (PAM_AUTHTOK_ERR);
	}

	/*
	 *	copy passwd files to temps, replacing matching lines
	 *	with new password attributes.
	 */
	if ((spfp = fopen(SHADOW, "r")) == NULL) {
		fclose(tsfp);
		goto err;
	}
	while (fgetspent_r(spfp, &unix_sp, spbuf, sizeof (spbuf)) != NULL) {
		if (strcmp(unix_sp.sp_namp, usrname) == 0) {
			found = 1;
			retcode = update_spent(pamh, field, data,
				unix_pwd, &unix_sp, &passwd_flag,
				privileged, nowarn);
			if (retcode != PAM_SUCCESS) {
				fclose(tsfp);
				fclose(spfp);
				goto err;
			}
			if (passwd_flag) {
				/* The attributes are in passwd file */
				if ((retcode = process_passwd(pamh,
					prognamep, usrname, unix_pwd,
					nowarn)) != PAM_SUCCESS) {
					fclose(tsfp);
					fclose(spfp);
					goto err;
				}
			}
		}
		if (putspent(&unix_sp, tsfp) != 0) {
			if (!nowarn) {
				sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
					prognamep, UNIX_MSG);
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			}
			fclose(tsfp);
			fclose(spfp);
			goto err;
		}
		memset(spbuf, 0, sizeof (spbuf));
	} /* end of while */

	if ((fclose(tsfp)) || (fclose(spfp))) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/* Check if user name exists */
	if (found == 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/*
	 *	Rename temp file back to  appropriate passwd file.
	 */

	/* remove old shadow file */
	if (unlink(OSHADOW) && access(OSHADOW, 0) == 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/* rename shadow file to old shadow file */
	if (rename(SHADOW, OSHADOW) == -1) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/* rename temparory shadow file to shadow file */
	if (rename(SHADTEMP, SHADOW) == -1) {
		(void) unlink(SHADOW);
		if (link(OSHADOW, SHADOW)) {
			if (!nowarn) {
				sprintf(messages[0], PAM_MSG(pamh, 92,
			"%s%s: Unexpected failure. Password database missing."),
					prognamep, UNIX_MSG);
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			}
			goto err;
		}
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	(void) ulckpwdf();
	memset(spbuf, 0, sizeof (spbuf));
	if (strcmp(field, "passwd") == 0) {
		sprintf(messages[0], PAM_MSG(pamh, 93,
			"%s%s: passwd successfully changed for %s"),
			prognamep, UNIX_MSG, usrname);
		(void) __pam_display_msg(pamh, PAM_TEXT_INFO,
				1, messages, NULL);
	}
	return (PAM_SUCCESS);
err:
	unlink(SHADTEMP);
	(void) ulckpwdf();
	memset(spbuf, 0, sizeof (spbuf));
	return (PAM_AUTHTOK_ERR);
}


/*
 * update_spent():
 * 	To update a shadow password file entry in the Unix
 *	authentication token file.
 *	This function is called by update_authtok_file() to
 *	update the token to token attributes.
 *	The parameter "field" indicates whenther token attributes or
 *	token itself will be changes, and the parameter "data" has
 *	the new values for the attributes or token.
 *
 */

static int
update_spent(pamh, field, data, unix_pwd, unix_sp,
		passwd_flag, privileged, nowarn)
	pam_handle_t *pamh;
	char *field;
	char **data;
	struct passwd *unix_pwd;
	struct spwd *unix_sp;
	int *passwd_flag;
	int privileged;
	int nowarn;
{
	char		*value;
	char		*char_p;
	char		*tmp_pwd_entry;
	char		**data_p = data;
	int		mindate;
	int		maxdate;
	int		warndate;
	char 		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	static char	*lkstring = "*LK*";	/* lock string to lock */
						/* user's password */

	if (strcmp(field, "attr") == 0) {
		while (*data != NULL) {
			/* check attribute: AUTHTOK_DEL */
			if ((value =
				attr_match("AUTHTOK_DEL", *data))
								!= NULL) {
				if (strcmp(value, "1") == 0) {

					/* delete password */
					if (unix_sp->sp_pwdp)
						memset(unix_sp->sp_pwdp, 0,
						strlen(unix_sp->sp_pwdp));

					/*
					 * set "AUTHTOK_EXT" will clear
					 * the sp_lstchg field. We do not
					 * want sp_lstchg field to be set
					 * if one execute passwd -d -f
					 * name	or passwd -l -f name.
					 */
					if (attr_find("AUTHTOK_EXP",
					    data_p) == 0)
						unix_sp->sp_lstchg = DAY_NOW;
				}
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_LK */
			if ((value = attr_match("AUTHTOK_LK", *data))
								!= NULL) {
				if (strcmp(value, "1") == 0) {
					memset(unix_sp->sp_pwdp, 0,
						strlen(unix_sp->sp_pwdp));
					/* lock password */
					unix_sp->sp_pwdp = lkstring;
					if (attr_find("AUTHTOK_EXP",
					    data_p) == 0)
						unix_sp->sp_lstchg = DAY_NOW;
				}
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_EXP */
			if ((value = attr_match("AUTHTOK_EXP", *data))
								!= NULL) {
				if (strcmp(value, "1") == 0) {
					/* expire password */
					unix_sp->sp_lstchg = (long) 0;
				}
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_MAXAGE */
			if ((value = attr_match("AUTHTOK_MAXAGE", *data))
								!= NULL) {
				/* set max field */
				maxdate = (int)strtol(value, &char_p, 10);
				if ((attr_find("AUTHTOK_MINAGE", data_p) ==
				    0) && unix_sp->sp_min == -1)
					unix_sp->sp_min = 0;
				if (maxdate == -1) {	/* turn off aging */
					unix_sp->sp_min = -1;
					unix_sp->sp_warn = -1;
				} else if (unix_sp->sp_max == -1)
					/*
					 * It was set to 0 before. That
					 * will force passwd change at the
					 * next login. There are several
					 * ways to force passwd change. I don't
					 * think turning on aging should imply
					 * that.
					 */
					unix_sp->sp_lstchg = DAY_NOW;

				unix_sp->sp_max = maxdate;
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_MINAGE */
			if ((value = attr_match("AUTHTOK_MINAGE", *data))
								!= NULL) {
				/* set min field */
				mindate = (int)strtol(value, &char_p, 10);
				if ((attr_find("AUTHTOK_MAXAGE", data_p) ==
				    0) &&
				    unix_sp->sp_max == -1 && mindate != -1) {
					return (PAM_AUTHTOK_DISABLE_AGING);
				}
				unix_sp->sp_min = mindate;
				data++;
				continue;
			}

			/* check attribute: AUTHTOK_WARNDATE */
			if ((value =
				attr_match
				("AUTHTOK_WARNDATE", *data)) != NULL) {
				/* set warn field */
				warndate = (int)strtol(value, &char_p, 10);
				if (unix_sp->sp_max == -1 && warndate != -1) {
					return (PAM_AUTHTOK_DISABLE_AGING);
				}
				unix_sp->sp_warn = warndate;
				data++;
				continue;
			}

			/* new shell */
			if ((value = attr_match("AUTHTOK_SHELL", *data))
			    != NULL) {
				if (unix_pwd == NULL) {
					if (!nowarn) {
					    sprintf(messages[0],
					    PAM_MSG(pamh, 94,
					    "%s: No local passwd record"),
						UNIX_MSG);
					    (void) __pam_display_msg(
						pamh, PAM_ERROR_MSG,
						1, messages, NULL);
					}
					return (PAM_AUTHTOK_RECOVERY_ERR);
				}
				tmp_pwd_entry = unix_pwd->pw_shell;
				unix_pwd->pw_shell =
					getloginshell(pamh, unix_pwd->pw_shell,
						privileged, nowarn);
				if (tmp_pwd_entry)
					free(tmp_pwd_entry);
				/* if NULL, shell unchanged */
				if (unix_pwd->pw_shell == NULL)
					return (PAM_SUCCESS);
				*passwd_flag = 1;
				data++;
				continue;
			}

			/* new homedir */
			if ((value = attr_match("AUTHTOK_HOMEDIR", *data))
			    != NULL) {
				if (unix_pwd == NULL) {
					if (!nowarn) {
					    sprintf(messages[0],
					    PAM_MSG(pamh, 94,
					    "%s: No local passwd record"),
						UNIX_MSG);
					    (void) __pam_display_msg(
						pamh, PAM_ERROR_MSG, 1,
						messages, NULL);
					}
					return (PAM_AUTHTOK_RECOVERY_ERR);
				}
				tmp_pwd_entry = unix_pwd->pw_dir;
				unix_pwd->pw_dir =
					gethomedir(pamh, unix_pwd->pw_dir,
						nowarn);
				if (tmp_pwd_entry)
					free(tmp_pwd_entry);
				/* if NULL, homedir unchanged */
				if (unix_pwd->pw_dir == NULL)
					return (PAM_SUCCESS);
				*passwd_flag = 1;
				data++;
				continue;
			}

			/* new gecos */
			if ((value = attr_match("AUTHTOK_GECOS", *data))
			    != NULL) {
				if (unix_pwd == NULL) {
					if (!nowarn) {
					    sprintf(messages[0],
					    PAM_MSG(pamh, 94,
					    "%s: No local passwd record"),
						UNIX_MSG);
					    (void) __pam_display_msg(
						pamh, PAM_ERROR_MSG, 1,
						messages, NULL);
					}
					return (PAM_AUTHTOK_RECOVERY_ERR);
				}
				tmp_pwd_entry = unix_pwd->pw_gecos;
				unix_pwd->pw_gecos =
					getfingerinfo(pamh, unix_pwd->pw_gecos,
						nowarn);
				if (tmp_pwd_entry)
					free(tmp_pwd_entry);
				/* if NULL, gecos unchanged */
				if (unix_pwd->pw_gecos == NULL)
					return (PAM_SUCCESS);
				*passwd_flag = 1;
				data++;
				continue;
			}
		}
	} else {
		if (strcmp(field, "passwd") == 0) { /* change password */
			unix_sp->sp_pwdp = *data_p;

			/* update the last change field */
			unix_sp->sp_lstchg = DAY_NOW;
			if (unix_sp->sp_max == 0) {   /* turn off aging */
				unix_sp->sp_max = -1;
				unix_sp->sp_min = -1;
			}
		}
	}
	return (PAM_SUCCESS);
}

/*
 * shell, homedir and gecos are in passwd file. The update is modeled
 * after shadow file.
 */
static int
process_passwd(pamh, prognamep, usrname, unix_pwd, nowarn)
	pam_handle_t	*pamh;
	char 		*prognamep;
	char 		*usrname;
	struct passwd	*unix_pwd;
	int		nowarn;
{
	FILE		*tpfp;	/* tmp passwd file pointer */
	FILE		*pwfp;	/* passwd file pointer */
	char		messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];
	struct passwd	*unix_p;
	struct passwd	unix_tmp;
	int		found;
	char		buf[4 * BUFSIZ];
	struct stat	stat_buf;

	if (stat(PASSWD, &stat_buf) < 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		return (PAM_AUTHTOK_ERR);
	}

	(void) umask(S_IAMB & ~(stat_buf.st_mode & S_IRUSR));
	if ((tpfp = fopen(PASSTEMP, "w")) == NULL) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		return (PAM_AUTHTOK_ERR);
	}

	if ((pwfp = fopen(PASSWD, "r")) == NULL) {
		fclose(tpfp);
		goto err;
	}


	while ((unix_p = fgetpwent_r(pwfp, &unix_tmp, buf, 4*BUFSIZ)) != NULL) {
		if (strcmp(unix_p->pw_name, usrname) == 0) {
			found = 1;
			unix_p->pw_gecos = unix_pwd->pw_gecos;
			unix_p->pw_dir = unix_pwd->pw_dir;
			unix_p->pw_shell = unix_pwd->pw_shell;
		}
		if (putpwent(unix_p, tpfp) != 0) {
			if (!nowarn) {
				sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
					prognamep, UNIX_MSG);
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			}
			fclose(tpfp);
			fclose(pwfp);
			goto err;
		}
		memset(buf, 0, 4 * BUFSIZ);

	} /* end of while */

	if ((fclose(tpfp)) || (fclose(pwfp))) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/* Check if user name exists */
	if (found == 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
			prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/*
	 *	Rename temp file back to  appropriate passwd file.
	 */

	/* remove old passwd file */
	if (unlink(OPASSWD) && access(OPASSWD, 0) == 0) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
				"%s%s: %s"),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/* rename password file to old password file */
	if (rename(PASSWD, OPASSWD) == -1) {
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	/* rename temporary password file to password file */
	if (rename(PASSTEMP, PASSWD) == -1) {
		(void) unlink(PASSWD);
		if (link(OPASSWD, PASSWD)) {
			if (!nowarn) {
				sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
					prognamep, UNIX_MSG);
				(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
					1, messages, NULL);
			}
			goto err;
		}
		if (!nowarn) {
			sprintf(messages[0], PAM_MSG(pamh, 91,
		"%s%s: Unexpected failure. Password database unchanged."),
				prognamep, UNIX_MSG);
			(void) __pam_display_msg(pamh, PAM_ERROR_MSG,
				1, messages, NULL);
		}
		goto err;
	}

	(void) chmod(PASSWD, 0644);
	return (PAM_SUCCESS);

err:
	(void) unlink(PASSTEMP);
	return (PAM_AUTHTOK_ERR);
}
