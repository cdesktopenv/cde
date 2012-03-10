/*
 *+SNOTICE
 *
 *	$XConsortium: XtArgCollector.h /main/3 1996/04/21 19:53:12 drk $
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

#ifndef XTARG_COLLECTOR_H
#define XTARG_COLLECTOR_H

#include <X11/Intrinsic.h>

class XtArgCollector
{

    public:
	XtArgCollector();
	~XtArgCollector();

	void AddItemToList (String name, XtArgVal value);
	void SetValues(Widget w);
	Arg  *GetItems();
	int  GetNumItems();

    private:
	int		num_items;	// number of items in the list
	int		max_num_items;	// number of items for which we've
                                        //      allocated space.
	Arg		*list;		// the list of XmStrings
	int		increment; 	// number of additional items
                                        //      to allocate space for.
};

#endif // XTARG_COLLECTOR_H
