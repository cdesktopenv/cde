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
/* $XConsortium: unix_set_authtokattr.c /main/5 1996/05/09 04:34:50 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)__set_authtoken_attr.c 1.33     95/09/11 SMI"

#include	"unix_headers.h"

/*
 * __set_authtoken_attr():
 *	To set authentication token attribute values.
 *
 * 	This function calls ck_perm() to check the caller's
 *	permission.  If the check succeeds, It will
 *	call update_authentok_file() and passes attributes/value
 *	pairs pointed by "sa_setattr" to set the authentication
 *	token attribute values of the user specified by the
 *	authentication handle "pamh".
 */

int
__set_authtoken_attr(
	pam_handle_t	*pamh,
	const char	**sa_setattr,
	int		repository,
	const char	*domain,
	int		argc,
	const char	**argv)
{
	int	i;
	int		retcode;
	char 		*usrname;
	char 		*prognamep;
	struct passwd	*pwd = NULL;
	struct spwd	*shpwd = NULL;
	int		privileged = 0;
	int		debug = 0;
	int		nowarn = 0;
	void		*passwd_res;

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcmp(argv[i], "nowarn") == 0)
			nowarn = 1;
		else
			syslog(LOG_ERR, "illegal UNIX module option %s",
				argv[i]);
	}


	if ((retcode = pam_get_item(pamh, PAM_SERVICE, (void **)&prognamep))
						!= PAM_SUCCESS ||
	    (retcode = pam_get_item(pamh, PAM_USER, (void **)&usrname))
						!= PAM_SUCCESS)
		return (retcode);

	if (debug)
		syslog(LOG_DEBUG,
			"__set_authtoken_attr(): repository %x, usrname %s",
			repository, usrname);

	retcode = ck_perm(pamh, repository,
			(char *)domain, &pwd, &shpwd, &privileged,
			(void **)&passwd_res, getuid(), debug, nowarn);
	if (retcode != 0) {
		return (retcode);
	}

	/*
	 * XXX: why do this???
	 * ignore all the signals
	 */
	for (i = 1; i < NSIG; i++)
		(void) sigset(i, SIG_IGN);

	/* update authentication token file */
	/* make sure the user exists before we update the repository */
#ifdef PAM_NIS
	if (IS_NIS(repository) && (pwd != NULL)) {
		retcode = update_authtok_nis(pamh, "attr",
		    (char **)sa_setattr, NULL, NULL, pwd,
		    privileged, nowarn);
		free_passwd_structs(pwd, shpwd);
		return (retcode);
	} else
#endif
#ifdef PAM_NISPLUS
	if (IS_NISPLUS(repository) && (pwd != NULL)) {
		/* nis+ needs clear versions of old and new passwds */
		retcode = update_authtok_nisplus(pamh,
		    (char *)domain, "attr", (char **)sa_setattr,
		    NULL, NULL, NULL, IS_OPWCMD(repository) ? 1 : 0, pwd,
		    NULL, privileged, (nis_result *)passwd_res, NULL,
		    debug, nowarn);
		free_passwd_structs(pwd, shpwd);
		return (retcode);
	} else
#endif
	if (IS_FILES(repository) && (pwd != NULL)) {
		retcode = update_authtok_file(pamh, "attr",
			(char **)sa_setattr, pwd,
			privileged, nowarn);
		free_passwd_structs(pwd, shpwd);
		return (retcode);
	}

	return (PAM_SUCCESS);
}
