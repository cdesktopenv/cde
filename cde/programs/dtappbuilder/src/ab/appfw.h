
/*
 *	$XConsortium: appfw.h /main/3 1995/11/06 17:18:44 rswiston $
 *
 *	@(#)appfw.h	1.1 27 Jun 1994	
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
 * appfw.h
 *
 * Functions for manipulating the Application Frameworks Editor
 */

#ifndef _appfw_h
#define _appfw_h

#include <ab_private/obj.h>
#include "appfw_ui.h"

void		appfw_show_dialog(
		    ABObj	project
		);

void		appfw_notify_new_project(
		    ABObj	project
		);

#endif /* _appfw_h */
