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
#if !defined (USL) && !defined(__uxp__)
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
