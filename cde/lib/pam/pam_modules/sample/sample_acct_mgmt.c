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
/* $XConsortium: sample_acct_mgmt.c /main/2 1996/05/09 04:29:37 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)sample_acct_mgmt.c 1.12     96/02/02 SMI"

#include <syslog.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <libintl.h>

static parse_allow_name(char *, char *);

/*
 * pam_sm_acct_mgmt	main account managment routine.
 *			It only checks the flag passed from pam_sm_auth_user().
 *			XXX: The routine just prints out a warning message.
 *			     It may need to force the user to change his/her
 *			     passwd.
 */

#include <security/pam_appl.h>
#define	PAMTXD	"SUNW_OST_SYSOSPAM"

int
pam_sm_acct_mgmt(
	pam_handle_t *pamh,
	int	flags,
	int	argc,
	const char **argv)
{
	char	*user;
	char	*pg;
	int	i;
	int	debug = 0;
	int	nowarn = 0;
	int	error = 0;

	if (argc == 0)
		return (PAM_SUCCESS);

	if (pam_get_item(pamh, PAM_USER, (void **)&user) != PAM_SUCCESS)
		return (PAM_SERVICE_ERR);

	if (pam_get_item(pamh, PAM_SERVICE, (void **)&pg) != PAM_SUCCESS)
		return (PAM_SERVICE_ERR);

	/*
	 * kludge alert. su needs to be handled specially for allow policy.
	 * we want to use the policy of the current user not the "destination"
	 * user. This will enable us to prevent su to root but not to rlogin,
	 * telnet, rsh, ftp to root.
	 *
	 * description of problem: user name is the "destination" name. not
	 * the current name. The allow policy needs to be applied to the
	 * current name in the case of su. user is "root" in this case and
	 * we will be getting the root policy instead of the user policy.
	 */
	if (strcmp(pg, "su") == 0) {
		struct passwd *pw;
		uid_t uid;
		uid = getuid();
		pw = getpwuid(uid);
		if (pw == NULL)
			return (PAM_SYSTEM_ERR);
		user = pw->pw_name;
	}

	if (user == 0 || *user == '\0' || (strcmp(user, "root") == 0))
		return (PAM_SUCCESS);

	for (i = 0; i < argc; i++) {
		if (strcasecmp(argv[i], "debug") == 0)
			debug = 1;
		else if (strcasecmp(argv[i], "nowarn") == 0) {
			nowarn = 1;
			flags = flags | PAM_SILENT;
		} else if (strncmp(argv[i], "allow=", 6) == 0)
			error |= parse_allow_name(user, (char *)(argv[i]+6));
		else
			syslog(LOG_DEBUG, "illegal option %s", argv[i]);
	}
	return (error?PAM_SUCCESS:PAM_AUTH_ERR);
}

static
parse_allow_name(char *who, char *cp)
{
	char name[256];
	static char *getname();

	/* catch "allow=" */
	if (*cp == '\0')
		return (0);
	while (cp) {
		cp = getname(cp, name);
		/* catch things such as =, and ,, */
		if (*name == '\0')
			continue;
		if (strcmp(who, name) == 0)
			return (1);
	}
	return (0);
} 

static char *
getname(char *cp, char *name)
{
	/* force name to be initially null string */
	*name = '\0';

	/* end of string? */
	if (*cp == '\0')
		return ((char *)0);
	while (*cp) {
		/* end of name? */
		if (*cp == ',' || *cp == '\0')
			break;
		*name++ = *cp++;
	}
	/* make name into string */
	*name++ = '\0';
	return ((*cp == '\0')? (char *)0 : ++cp);
}
