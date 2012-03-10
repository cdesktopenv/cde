/* $TOG: Dt.h /main/12 1999/10/18 14:49:57 samborn $ */
/* 
 * (c) Copyright 1997, The Open Group 
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993-1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _Dt_Dt_h
#define _Dt_Dt_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constants
 */

/* CDE Version information */

#define DtVERSION     		2
#define DtREVISION    		1
#define DtUPDATE_LEVEL    	30

#define DtVERSION_NUMBER	(DtVERSION * 10000 +  \
				DtREVISION * 100 + \
				DtUPDATE_LEVEL)

#define DtVERSION_STRING "CDE Version 2.1.30"


/* 
 * CDE Version information 
 */

externalref const int DtVersion;
externalref const char *DtVersionString;


/*
 * Functions
 */

extern Boolean DtInitialize(
		Display		*display,
		Widget		widget,
		char		*name,
		char		*tool_class);

extern Boolean DtAppInitialize(
		XtAppContext	app_context,
		Display		*display,
		Widget		widget,
		char		*name,
		char		*tool_class);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_Dt_h */
