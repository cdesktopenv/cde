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
/* $XConsortium: dce_session.c /main/4 1996/05/09 04:27:00 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)dce_session.c 1.3     96/01/10 SMI"

#include <libintl.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <dce/sec_login.h>
#include <dce/dce_error.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>
#include <libintl.h>

#include "pam_impl.h"
#include "utils.h"

/*
 * pam_sm_open_session
 */
int
pam_sm_open_session(
	pam_handle_t *pamh,
	int   flags,
	int	argc,
	const char **argv)
{
	return (PAM_SUCCESS);
}

/*
 * pam_sm_close_session
 */
int
pam_sm_close_session(
	pam_handle_t *pamh,
	int   flags,
	int	argc,
	const char **argv)
{
	return (PAM_SUCCESS);
}
