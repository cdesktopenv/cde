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
/* $XConsortium: pam_loc.h /main/3 1996/05/09 04:25:37 drk $ */
/*
 *  "@(#)pam_loc.h 1.4 96/02/02
 *
 *  Copyright 1996 Sun Microsystems, Inc.
 *
 *  All Rights reserved
 */

#ifndef _PAM_LOC_H
#define	_PAM_LOC_H

#pragma ident	"@(#)pam_loc.h	1.4	96/02/02 SMI"	/* PAM 2.6 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PAM_MSG macro for return of internationalized text
 */

#define	PAM_MSG(pamh, number, string)\
	(char *) __pam_get_i18n_msg(pamh, "libpam", 2, number, string)

#ifdef __cplusplus
}
#endif

#endif	/* _PAM_LOC_H */
