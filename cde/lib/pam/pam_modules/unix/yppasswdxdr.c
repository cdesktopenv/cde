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
/* $XConsortium: yppasswdxdr.c /main/2 1996/05/08 13:46:51 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1995,1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*******************************************************************************
**  yppasswd utilies
*******************************************************************************/

#if (PAM_NIS || PAM_NISPLUS)

#include "unix_headers.h"

bool_t
xdr_passwd(xdrs, pw)
	XDR *xdrs;
	struct passwd *pw;
{
	if (!xdr_wrapstring(xdrs, &pw->pw_name)) {
		return (FALSE);
	}
	if (!xdr_wrapstring(xdrs, &pw->pw_passwd)) {
		return (FALSE);
	}
	if (!xdr_uid_t(xdrs, &pw->pw_uid)) {
		return (FALSE);
	}
	if (!xdr_gid_t(xdrs, (&pw->pw_gid))){
		return (FALSE);
	}
	if (!xdr_wrapstring(xdrs, &pw->pw_gecos)) {
		return (FALSE);
	}
	if (!xdr_wrapstring(xdrs, &pw->pw_dir)) {
		return (FALSE);
	}
	if (!xdr_wrapstring(xdrs, &pw->pw_shell)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_yppasswd(xdrs, yppw)
	XDR *xdrs;
	struct yppasswd *yppw;
{
	if (!xdr_wrapstring(xdrs, &yppw->oldpass)) {
		return (FALSE);
	}
	if (!xdr_passwd(xdrs, &yppw->newpw)) {
		return (FALSE);
	}
	return (TRUE);
}

#else

/* Some compilers complain about empty compilation modules. */
static int dummy;

#endif /* PAM_NIS or PAM_NISPLUS */
