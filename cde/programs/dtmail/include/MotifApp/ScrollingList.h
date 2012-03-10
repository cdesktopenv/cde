/* $XConsortium: ScrollingList.h /main/4 1996/04/21 19:46:45 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
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
 *+ENOTICE
 */

#ifndef SCROLLINGLIST_H
#define SCROLLINGLIST_H

#include "UIComponent.h"
#include <Xm/List.h>

#include <stdlib.h>
class ScrollingList : public UIComponent {
private:
  static void defaultActionCallback( Widget, XtPointer, XmListCallbackStruct * );

public:

  ScrollingList ( Widget, char * );
  ~ScrollingList ();
  virtual const char *const className() { return ( "ScrollingList" ); }
  virtual void defaultAction( Widget, XtPointer, XmListCallbackStruct * ) = 0;
};

#endif
