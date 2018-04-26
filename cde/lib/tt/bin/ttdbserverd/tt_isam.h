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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_isam.h /main/3 1995/10/20 16:43:31 rswiston $ 			 				 */
/*
 * @(#)tt_isam.h	1.9 95/04/10
 *
 * TT version of the NetISAM isam.h file.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef  _TT_ISAM_H
#define  _TT_ISAM_H

#include <isam.h>

extern "C" { int isaddindex(int, keydesc*); }
extern "C" { int isbuild(const char*, int, keydesc*, int); }
extern "C" { int isclose(int); }
extern "C" { int iscntl(int, int, ...); }
extern "C" { int isdelcurr(int); }
extern "C" { int isdelrec(int, long); }
extern "C" { int iserase(const char*); }
extern "C" { int isfsync(int); }
extern "C" { int isindexinfo(int, keydesc *, int); }
extern "C" { int isopen(const char*, int); }
extern "C" { int isread(int, char*, int); }
extern "C" { int isrename(char*, char*); }
extern "C" { int isrewcurr(int, char*); }
extern "C" { int isrewrec(int, long, char*); }
extern "C" { int isstart(int, keydesc*, int, char*, int); }
extern "C" { int iswrite(int, char*); }
extern "C" { int islock(int); }
extern "C" { int isunlock(int); }
extern "C" { int isgarbage(char *); }
extern "C" { int isgetcurpos(int isfd, int *len, char **buf); }
extern "C" { int issetcurpos(int isfd, char *buf); }

#endif /* _TT_ISAM_H */
