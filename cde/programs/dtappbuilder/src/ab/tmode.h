
/*
 *	$XConsortium: tmode.h /main/3 1995/11/06 17:54:45 rswiston $
 *
 * @(#)tmode.h	1.1 15 Jan 1995
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/* 
** File: tmode.h - *New* interface into Test Mode
*/

#ifndef _AB_TMODE_H
#define _AB_TMODE_H

#include <ab_private/AB.h>
#include <ab_private/obj.h>

/*
** Entry point into Test Mode
*/
extern void     tmode_enable(
                    ABObj     project,
                    BOOL      test_project
                );

/*
** Exit point from Test Mode
*/
extern void     tmode_disable(
                    ABObj     project
                );

#endif /* _AB_TMODE_H */
