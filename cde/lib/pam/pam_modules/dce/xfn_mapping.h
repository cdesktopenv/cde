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
/* $XConsortium: xfn_mapping.h /main/4 1996/05/09 04:28:15 drk $ */

/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef _XFN_MAPPING_H
#define	_XFN_MAPPING_H

#ident  "@(#)xfn_mapping.h 1.6     95/09/19 SMI"

#ifdef	__cplusplus
extern "C" {
#endif

#define	MAP_PASSLEN	32		/* XXX max mapped passwd length */
#define	MAP_HEXLEN	(2*MAP_PASSLEN)	/* XXX max passwd length in hex */
#define	DCE_XFN_PASS_ATTR	"onc_dce_passwd"

#define	XFN_MAP_DEBUG	0x1	/* enable debugging */

int
xfn_get_mapped_password(
	int flags,		/* XFN_MAP_DEBUG, etc */
	char *user,		/* xfn user */
	char *xfn_attr,		/* xfn attribute that holds the target pass */
	char *key,		/* key (password) encrypting target password */
	char *out,		/* un-encrypted target password */
	int max_out_len		/* buffer size of out */
);

int
xfn_update_mapped_password(
	int flags,		/* XFN_MAP_DEBUG, etc */
	char *user,		/* xfn user */
	char *xfn_attr,		/* xfn attribute that holds the target pass */
	char *key,		/* key (password) encrypting target password */
	char *password		/* un-encrypted target password */
);

#endif

#ifdef	__cplusplus
}
#endif
