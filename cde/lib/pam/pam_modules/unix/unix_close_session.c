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
/* $XConsortium: unix_close_session.c /main/5 1996/05/09 04:33:38 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)unix_close_session.c 1.26     95/12/12 SMI"

#include "unix_headers.h"

/*
 * pam_sm_close_session	- Terminate a PAM authenticated session
 */

int
pam_sm_close_session(
	pam_handle_t	*pamh,
	int	flags,
	int	argc,
	const char	**argv)
{
	int	i;
	int	debug = 0;

	for (i = 0; i < argc; i++) {
		if (strcasecmp(argv[i], "debug") == 0)
			debug = 1;
		else
			syslog(LOG_ERR, "illegal option %s", argv[i]);
	}

	return (PAM_SUCCESS);
}
