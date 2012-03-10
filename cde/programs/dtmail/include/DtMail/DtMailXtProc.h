/*
 *+SNOTICE
 *
 *
 *	$XConsortium: DtMailXtProc.h /main/4 1996/04/21 19:45:02 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _DTMAILXTPROC_H
#define _DTMAILXTPROC_H

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

    void DtMailXtInputProc(XtPointer, int *, XtInputId *);

    extern XtAppContext		DtMailDamageContext;

#ifdef __cplusplus
}
#endif

#endif
