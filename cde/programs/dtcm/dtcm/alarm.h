/*******************************************************************************
**
**  alarm.h
**
**  static char sccsid[] = "@(#)alarm.h 1.8 94/11/07 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: alarm.h /main/3 1995/11/03 10:17:51 rswiston $
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

#ifndef _ALARM_H
#define _ALARM_H

#include <X11/Intrinsic.h>
#include <csa.h>
#include "ansi_c.h"

extern void compose		P((Calendar*, CSA_reminder_reference *, char**));
extern void flash_it		P((XtPointer, XtIntervalId*));
extern void mail_it		P((XtPointer, XtIntervalId*, CSA_reminder_reference *));
extern void open_it		P((XtPointer, XtIntervalId*, CSA_reminder_reference *));
extern void postup_show_proc	P((Calendar*, CSA_reminder_reference *));
extern void reminder_driver	P((XtPointer, XtIntervalId*));
extern void ring_it		P((XtPointer, XtIntervalId*));
extern void slp			P((int x));

#endif
