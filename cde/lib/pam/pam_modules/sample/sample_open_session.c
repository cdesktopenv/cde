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
/* $XConsortium: sample_open_session.c /main/2 1996/05/09 04:30:22 drk $ */
/*
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ident  "@(#)sample_open_session.c 1.8     96/01/12 SMI"

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>

int
pam_sm_open_session(
	pam_handle_t *pamh,
	int	flags,
	int	argc,
	const char **argv)
{
	return (PAM_SUCCESS);
}
