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
#include <X11/Xos_r.h>

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

