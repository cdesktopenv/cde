/*
 * File:	boolean.h $XConsortium: boolean.h /main/3 1995/10/26 16:10:48 rswiston $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef __BOOLEAN_H_
#define __BOOLEAN_H_

#if defined(__aix)
#undef  NULL
#define NULL 0
#endif

#if defined(SVR4)
#include <sys/types.h>

#if defined(sun) && defined(_XOPEN_SOURCE)
#ifndef B_TRUE
#define B_TRUE _B_TRUE
#endif
#ifndef B_FALSE
#define B_FALSE _B_FALSE
#endif
#endif   /* sun && _XOPEN_SOURCE */

#ifndef boolean
typedef	boolean_t	boolean;
#endif

#ifndef TRUE
#define TRUE B_TRUE
#endif

#ifndef FALSE
#define FALSE B_FALSE
#endif
#endif /* SVR4 */


#if !defined(SVR4)
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef boolean
typedef int boolean;
#endif
#endif	/* ! SVR4 */
#endif /* __BOOLEAN_H_ */
