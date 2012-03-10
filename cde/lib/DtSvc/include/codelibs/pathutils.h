/*
 * $XConsortium: pathutils.h /main/4 1996/05/08 11:21:09 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef __PATHUTILS_H_
#define __PATHUTILS_H_

#include <stddef.h>
#include <codelibs/boolean.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
extern char *pathcollapse(const char *src, 
			  char *dst = NULL, 
			  boolean show_dir = FALSE);
#elif defined(__STDC__)
extern char *pathcollapse(const char *src, char *dst, boolean show_dir);
#else /* old-style C */
extern char *pathcollapse();
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PATHUTILS_H_ */
