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
