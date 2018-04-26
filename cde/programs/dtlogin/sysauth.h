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
/* $XConsortium: sysauth.h /main/4 1995/10/27 16:16:09 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        sysauth.h
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Header file for system authentication routine
 **
 **                Defines, structure definitions, and external declarations
 **		   are specified here.
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _SYSAUTH_H
#define _SYSAUTH_H

/****************************************************************************
 *
 *  Defines
 *
 ****************************************************************************/

/*
 *  Authenticate return codes...
 */

#define VF_OK			1
#define VF_INVALID		2
#define VF_HOME			3
#define VF_MAX_USERS		4
#define VF_PASSWD_AGED		5
#define VF_BAD_UID		6
#define VF_BAD_GID		7
#define VF_BAD_AID		8
#define VF_BAD_AFLAG		9
#define VF_NO_LOGIN		10
#define VF_BAD_HOSTNAME		11
#define VF_CHALLENGE            12
#define VF_MESSAGE              13

#ifdef BLS
#define VF_BAD_SEN_LEVEL	14
#endif
 
/****************************************************************************
 *
 *  External procedure declarations
 *
 ****************************************************************************/

extern int  Authenticate(struct display *d, char *name, char *passwd,
                         char **msg) ;


#endif /* _SYSAUTH_H */
