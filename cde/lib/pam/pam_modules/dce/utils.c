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
/* $XConsortium: utils.c /main/4 1996/05/09 04:27:30 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)utils.c 1.13     95/09/19 SMI"

#include <dce/sec_login.h>
#include <security/pam_appl.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "utils.h"

/* ******************************************************************** */
/*									*/
/* 		Utilities Functions					*/
/*									*/
/* ******************************************************************** */


/* return a string given an error status */

unsigned char *
get_dce_error_message(
	error_status_t	status,
	unsigned char	*buffer
)
{
	int s;

	dce_error_inq_text(status, buffer, &s);

	if (s)
		sprintf((char *)buffer, "DCE error %u", status);

	return (buffer);
}

/* release login_context resources. if we are returning PAM_SUCCESS */
/* we also have to save KRB5CCNAME and set it after calling release */
/* because s_l_release_c unset's it even though we still need it */

void
pam_sec_login_free_context(
	int			pam_status,
	sec_login_handle_t	*login_context,
	error_status_t		*st)
{
	if (pam_status == PAM_SUCCESS) {
		static char *krb5 = "KRB5CCNAME";
		char *krb5_value = NULL;
		char *env = getenv(krb5);

		if (env) {
			/* we MUST malloc this for the putenv to work! */
			krb5_value = malloc(strlen(krb5)+1+strlen(env)+1);
			if (krb5_value)
				sprintf(krb5_value, "%s=%s", krb5, env);
		}

		sec_login_release_context(login_context, st);

		if (krb5_value)
			putenv(krb5_value);

	} else { /* login failed. purge this context */

		sec_login_purge_context(login_context, st);
	}
}

/*
 * get_pw_uid():
 *	To get the uid from the passwd entry for specified user
 *	It returns 0 if the user can't be found, otherwise returns 1.
 */

int
get_pw_uid(char *user, uid_t *uid)
{
	struct passwd sp;
	char buffer[1024];

	if (getpwnam_r(user, &sp, buffer, sizeof (buffer)) == NULL) {
		return (0);
	}

	*uid = sp.pw_uid;

	return (1);
}
