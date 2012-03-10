/*******************************************************************************
**
**  session.h
**
**  static char sccsid[] = "@(#)session.h 1.3 93/11/08 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: session.h /main/3 1995/11/03 10:33:56 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _SESSION_H
#define _SESSION_H

#include "ansi_c.h"
#include "calendar.h"

extern void	CMSaveSessionCB		P((Widget,  XtPointer, XtPointer));
extern void	GetSessionInfo		P(());

#endif
