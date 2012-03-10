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
