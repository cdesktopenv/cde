/* $XConsortium: EUSDebug.hh /main/3 1996/04/21 19:46:57 drk $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _DEBUG_HH
#define _DEBUG_HH

#if defined(DEBUG)
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern int EUSDebugLevel;

inline void
initDebug(void)
{
    char * level = getenv("EUS_DEBUG");

    if (level) {
	EUSDebugLevel = atoi(level);
    }
    else {
	EUSDebugLevel = 0;
    }
}

inline void
DebugPrintf(int level, const char * fmt, ...)
{
    va_list	args;
    
    if (level <= EUSDebugLevel) {
	va_start(args, fmt);
	fprintf(stderr, "EUS DEBUG: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
    }
}

#else

inline void initDebug(void) {}
inline void DebugPrintf(int, const char *, ...) {}

#endif

#endif
