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
/* $XConsortium: ansi_c.h /main/1 1996/04/21 19:21:28 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _ANSI_C_H
#define _ANSI_C_H

#include <stdlib.h>

/*
**  For all function declarations, if ANSI, then use a prototype
*/
#if  defined(__STDC__)
#define P(args)  args
#else
#define P(args)  ()
#endif

#ifdef SunOS
extern char	*fconvert	P((double, int, int*, int*, char*));
extern int	isascii		P((int));
extern int	kill		P((long, int));
extern char	*mktemp		P((char*));
extern int	strcasecmp	P((const char*, const char*));
extern int	strncasecmp	P((const char*, const char*, size_t));
extern char	*strdup		P((const char*));
#endif

#ifdef NEED_STRCASECMP
int	strcasecmp	P((const char*, const char*));
int	strncasecmp	P((const char*, const char*, size_t));
#endif

#endif
