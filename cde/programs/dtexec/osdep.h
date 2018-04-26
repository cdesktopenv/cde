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
#ifndef OSDEP_H
#define OSDEP_H

/******************************************************************
 *
 * $XConsortium: osdep.h /main/4 1996/01/15 11:43:44 rswiston $
 *
 * Bitmask routines to support the usage of select() in dtexec.
 * Revised for Spec1170 conformance.
 *
 */


#ifdef _POSIX_SOURCE
# include <limits.h>
#else
# define _POSIX_SOURCE
# include <limits.h>
# undef _POSIX_SOURCE
#endif

#include <sys/time.h>
#include <sys/types.h>
#ifndef __hpux
# include <sys/select.h>
# define FD_SET_CAST(x) (x)
#else
# define FD_SET_CAST(x) ((int *)(x))
#endif

#ifndef OPEN_MAX
# define OPEN_MAX 128
#endif

#if OPEN_MAX <= 128
# define MAXSOCKS (OPEN_MAX)
#else
# define MAXSOCKS 128
#endif

#define BITSET(buf, i)		FD_SET(i, &(buf))
#define BITCLEAR(buf, i)	FD_CLR(i, &(buf))
#define GETBIT(buf, i)		FD_ISSET(i, &(buf))
#define COPYBITS(src, dst)	(dst) = (src)
#define CLEARBITS(buf)		FD_ZERO(&buf)

#endif /* OSDEP_H */
