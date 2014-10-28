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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: pam_svc.h /main/3 1996/10/30 11:13:40 drk $ */
/*******************************************************************************
**
**  "@(#)pam_svc.h 1.4 95/09/12 
**
**  Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
**
**  This file contains header info related to dtlogin use of PAM 
**  (Pluggable Authentication Module) library.
**
*******************************************************************************/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef _DTLOGIN_PAM_SVC_H
#define _DTLOGIN_PAM_SVC_H

#include <sys/types.h>

#define PAM_LOGIN_MAXTRIES 5
#define PAM_LOGIN_SLEEPTIME 4
#define PAM_LOGIN_DISABLETIME 20



 
/*
 *	External procedure declarations
 */


extern int _DtAuthentication(char*, char*, char*, char*, char*);
extern int _DtAccounting(char*, char*, char[], char*, 
			      char*, pid_t, int, int);
extern int _DtSetCred(char*, char *, uid_t, gid_t);
extern int _DtSetDevPerm(char *, uid_t, gid_t);
extern int _DtResetDevPerm(char *);

#ifdef sun
extern int audit_login_save_host(char *host);
extern int audit_login_save_ttyn(char *ttyn);
extern int audit_login_save_port(void);
extern int audit_login_success(void);
extern int audit_login_save_pw(struct passwd *pwd);
extern int audit_login_bad_pw(void);
extern int audit_login_maxtrys(void);
#endif /* sun */


#endif /* _DTLOGIN_PAM_SVC_H */
