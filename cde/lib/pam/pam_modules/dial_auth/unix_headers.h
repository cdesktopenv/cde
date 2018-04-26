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
/* $XConsortium: unix_headers.h /main/4 1996/05/09 04:28:47 drk $ */
/*
 * Copyright (c) 1992-1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef	_UNIX_HEADERS_H
#define	_UNIX_HEADERS_H

#pragma ident	"@(#)unix_headers.h	1.6	96/02/02 SMI"	/* PAM 2.6 */

#ifdef __cplusplus
extern "C" {
#endif

/*
******************************************************************

	PROPRIETARY NOTICE(Combined)

This source code is unpublished proprietary information
constituting, or derived under license from AT&T's UNIX(r) System V.
In addition, portions of such source code were derived from Berkeley
4.3 BSD under license from the Regents of the University of
California.



		Copyright Notice

Notice of copyright on this source code product does not indicate
publication.

	(c) 1986, 1987, 1988, 1989, 1990, 1991, 1992 Sun Microsystems, Inc
	(c) 1983, 1984, 1985, 1986, 1987, 1988, 1989  AT&T.
		All rights reserved.
*******************************************************************
*/


/*
******************************************************************** *
*									*
*			Unix Scheme Header Files			*
*									*
* ******************************************************************** */

#include <sys/param.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <pwd.h>
#include <shadow.h>
#include <string.h>
#include <rpc/types.h>
#include <rpc/auth.h>
#include <locale.h>
#include <crypt.h>
#include <syslog.h>

/*
 * Various useful files and string constants
 */
#define	DIAL_FILE	"/etc/dialups"
#define	DPASS_FILE	"/etc/d_passwd"
#define	SHELL		"/usr/bin/sh"

/*
 * PAM_MSG macro for return of internationalized text
 */

#define	PAM_MSG(pamh, number, string)\
	(char *) __pam_get_i18n_msg(pamh, "pam_unix", 2, number, string)

/*
 * Miscellaneous constants
 */
#define	SLEEPTIME	4
#define	ERROR		1
#define	OK		0
#define	MAXTRYS		5
#define	ROOTUID		0

/*
 * String manipulation macros: SCPYN, EQN and ENVSTRNCAT
 */
#define	SCPYN(a, b)	(void) strncpy(a, b, sizeof (a))

#ifdef __cplusplus
}
#endif

#endif	/* _UNIX_HEADERS_H */
