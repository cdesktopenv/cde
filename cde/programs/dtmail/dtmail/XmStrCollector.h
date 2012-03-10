/*
 *+SNOTICE
 *
 *	$XConsortium: XmStrCollector.h /main/3 1996/04/21 19:53:05 drk $
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

#ifndef XMSTR_COLLECTOR_H
#define XMSTR_COLLECTOR_H

class XmStrCollector
{

    public:
	XmStrCollector();
	~XmStrCollector();

	void AddItemToList (XmString item);
	XmString *GetItems();
	int GetNumItems();

    private:
	int		num_items;	// number of items in the list
	int		max_num_items;	// number of items for which we've
					// 	allocated space.
	XmString	*list;		// the list of XmStrings
	int		increment; 	// number of additional items
					//	to allocate space for.
};

#endif // XMSTR_COLLECTOR_H
