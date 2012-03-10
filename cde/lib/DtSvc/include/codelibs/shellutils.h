/*
 * File:	shellutils.h $XConsortium: shellutils.h /main/3 1995/10/26 16:13:31 rswiston $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef __SHELLUTILS_H_
#define __SHELLUTILS_H_

#ifdef __cplusplus
extern "C"
{
    char const *const *shellscan(char const *str, int *argc = (int *)0,
	    unsigned opts = 0);
}
#else
    extern char **shellscan();
#endif

#define SHX_NOGLOB	0x0001
#define SHX_NOTILDE	0x0002
#define SHX_NOVARS	0x0004
#define SHX_NOQUOTES	0x0008
#define SHX_NOSPACE	0x0010
#define SHX_NOMETA 	0x0020
#define SHX_NOCMD	0x0040
#define SHX_COMPLETE	0x0080

#define SHX_NOGRAVE	0x0040		/* Obsolete, use NOCMD */

#endif /* __SHELLUTILS_H_ */
