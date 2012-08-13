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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: tt_log.C /main/7 1999/10/14 18:42:17 mgreess $ 			 				
/*
 *
 * tt_log.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef __osf__
#include <unistd.h>
#else
#if !defined (USL) && !defined(__uxp__) && !defined(linux) && !defined(CSRG_BASED)
#include <osfcn.h>
#endif
#endif /* __osf__ */

#define X_INCLUDE_TIME_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#if defined (USL) || defined(__uxp__)
#include <tt_options.h>
#if defined(OPT_BUG_UW_2) || defined(OPT_BUG_UXP)
#include <unistd.h>
#endif
#endif
#include <util/tt_log.h>
#include <sys/types.h>
#include <fcntl.h>

void
_tt_log_error(int errno, int line, char *file, char *msg)
{
	pid_t pid;
	char logfile[100];
	char *timestamp;
	FILE *fl;
	time_t clock;
	_Xctimeparams ctime_buf;

	memset((char*) &ctime_buf, 0, sizeof(_Xctimeparams));
	if (getenv("NSE_ENV") == (char *)0) {
		return;
	}
	pid = getpid();
	sprintf(logfile, "/tmp/tt.log.%d", pid);
	fl = fopen(logfile, "a");
	if (fl == (FILE *)0) {
		return;
	}
	fcntl(fileno(fl), F_SETFD, 1);	/* Close on exec */

	time(&clock);
	timestamp = _XCtime(&clock, ctime_buf);
	timestamp[19] = '\0';  // trim off year
	fprintf(fl, "%s: Error %d at line %d of file %s",
		timestamp+4, errno, line, file);  // skip day of week
	if (msg != (char *)0 && *msg) {
		fprintf(fl, ": %s", msg);
	}
	fprintf(fl, "\n");
	fclose(fl);
	return;
}
