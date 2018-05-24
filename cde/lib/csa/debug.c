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
/* $XConsortium: debug.c /main/3 1996/11/21 20:00:19 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <stdio.h>

#define X_INCLUDE_TIME_H
#define XOS_USE_NO_LOCKING

#if defined(__linux__)
#undef SVR4
#endif
#include <X11/Xos_r.h>
#if defined(__linux__)
#define SVR4
#endif

#include <syslog.h>

static int inited = 0;

extern void
_DtCm_print_tick(time_t t)
{
        char *a;
	_Xctimeparams ctime_buf;
 
        a = _XCtime(&t, ctime_buf);
        (void) fprintf (stderr, "%d %s\n", t, a);
}

extern void
_DtCm_print_errmsg(const char *msg)
{
	if (inited == 0) {
		openlog("libcsa", 0, 0);
		inited = 1;
	}

#ifdef CM_DEBUG
	fprintf(stderr, "libcsa: %s\n", msg);
#else
	syslog(LOG_ERR, "%s\n", msg);
#endif

}

