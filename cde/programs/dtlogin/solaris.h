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
/* $XConsortium: solaris.h /main/7 1996/10/30 11:12:37 drk $ */
/*******************************************************************************
**
**  solaris.h 1.9 95/09/10 
**
**  Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
**
**  This file contains header info specific to Sun Solaris login
*******************************************************************************/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*******************************************************************************
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/


#ifndef _DTLOGIN_SOLARIS_H
#define _DTLOGIN_SOLARIS_H

#include <sys/types.h>

#ifdef PAM
#include    <security/pam_appl.h>
#define	SOLARIS_SUCCESS	PAM_SUCCESS
#endif

#ifdef SUNAUTH
#include    <security/ia_appl.h>
#define SOALRIS_SUCCESS IA_SUCCESS
#endif


/* Solaris utmp mgt flags */

#define	SOLARIS_UPDATE_ENTRY  	1	/* Update an existing entry */
#define	SOLARIS_NOLOG		2	/* Don't log the new session */
#define	SOLARIS_LOGIN		4	/* login type entry (sigh...) */
 
/* Errors returned by solaris_setutmp_mgmt/solaris_reset_utmp_mgmt() */
#define	SOLARIS_NOENTRY		27 	/* No entry found */
#define	SOLARIS_ENTRYFAIL	28	/* Couldn't edit the entry */

/* user credential UID/GID erros */ 
#define	SOLARIS_BAD_GID		29	/* Invalid Group ID */
#define	SOLARIS_INITGROUP_FAIL	30	/* group IDs init failed */
#define	SOLARIS_BAD_UID		31	/* Invaid User ID */
#define	SOLARIS_SETGROUP_FAIL	32	/* Set of group IDs failed */

/*
 *	External procedure declarations
 */


extern int solaris_authentication(char*, char*, char*, char*, char*);
extern int solaris_accounting(char*, char*, char[], char*, 
			      char*, pid_t, int, int);
extern int solaris_setcred(char*, char *, uid_t, gid_t);
extern int solaris_setdevperm(char *, uid_t, gid_t);
extern int solaris_resetdevperm(char *);

extern int audit_login_save_host(char *host);
extern int audit_login_save_ttyn(char *ttyn);
extern int audit_login_save_port(void);
extern int audit_login_success(void);
extern int audit_login_save_pw(struct passwd *pwd);
extern int audit_login_bad_pw(void);
extern int audit_login_maxtrys(void);


#endif /* _DTLOGIN_SOLARIS_H */
