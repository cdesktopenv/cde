/*
 * File:         bms.h $XConsortium: bms.h /main/4 1996/06/21 17:20:07 ageorge $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _bms_h
#define _bms_h

#ifdef SVR4
#include <netdb.h>		/* MAXHOSTNAMELEN */
#else
#include <sys/param.h>		/* MAXHOSTNAMELEN */
#endif

/* WARNINGS: 						      */
/* 							      */
/*   1) <bms/sbport.h> MUST included before this file.         */
/*   2) Any <X11/...> includes MUST come before this file.    */
/* ---------------------------------------------------------- */

#ifndef _XtIntrinsic_h		     /* Define "Boolean" if not already */
   typedef char Boolean;	     /* defined from <X11/Intrinsic.h>  */
#endif

#ifndef TRUE
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif 

extern char *XeToolClass;

extern XeString         XeProgName;                  /* From noXinit.c     */

extern XeString				/* Returns Malloc'ed memory */
XeSBTempPath
#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
   (XeString);
#else
   ();
#endif

/* DON'T ADD STUFF AFTER THIS #endif */
#endif /* _bms_h */

