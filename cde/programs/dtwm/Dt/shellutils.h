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
/*
 * File:	shellutils.h $XConsortium: shellutils.h /main/3 1995/10/26 16:13:31 rswiston $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef __SHELLUTILS_H_
#define __SHELLUTILS_H_

#ifdef __cplusplus
extern "C"
{
    char const *const *shellscan(char const *str, int *argc = (int *)0,
	    unsigned opts = 0);
}
#else
    extern char **shellscan();
#endif

#define SHX_NOGLOB	0x0001
#define SHX_NOTILDE	0x0002
#define SHX_NOVARS	0x0004
#define SHX_NOQUOTES	0x0008
#define SHX_NOSPACE	0x0010
#define SHX_NOMETA 	0x0020
#define SHX_NOCMD	0x0040
#define SHX_COMPLETE	0x0080

#define SHX_NOGRAVE	0x0040		/* Obsolete, use NOCMD */

#endif /* __SHELLUTILS_H_ */
