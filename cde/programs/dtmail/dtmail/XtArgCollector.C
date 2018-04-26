/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *+SNOTICE
 *
 *	$XConsortium: XtArgCollector.C /main/3 1996/04/21 19:53:08 drk $
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

/*
 *
 * The XtArgCollector class is simply a dynamic array of Args.
 * It is useful to build an ArgList to pass as an argument to
 * XtSetValues.  This is a way to collect resources and values
 * over several functions and then finally set them at another
 * point.  This can help prevent multiple redisplays in widgets
 * like XmList which don't have a DisableRedisplay convenience
 * function like XmText.  It's a class for setting resources
 * similar to the following:
 * 	n=0;
 *	XtSetArg (args[n], XmNmarginHeight, 5); n++
 * 	XtSetValues (widget, args, n);
 */


#include <stdlib.h>
#include <stdio.h>
#include "XtArgCollector.h"

XtArgCollector::XtArgCollector()
{
    // Initialize the class variables
    num_items = 0;
    max_num_items = 0;
    increment = 10;
    list = NULL;
}

XtArgCollector::~XtArgCollector() 
{
    // Free the list
    XtFree ((char *)list);
}

void
XtArgCollector::AddItemToList (String name, XtArgVal value)
{
    // Allocate memory for 10 items at a time.
    // When this memory is exceeded, allocate
    // space for 10 more.
    if (num_items >= max_num_items)
    {
	max_num_items += increment;

	list = (Arg *) XtRealloc ((char *)list, sizeof (Arg) *max_num_items);
    }

    XtSetArg(list[num_items], name, value);

    num_items++;
}

//
// Perform an XtSetValues on the list of args
//
void
XtArgCollector::SetValues(Widget w)
{
    XtSetValues (w, list, num_items);
}

//
// Get the list of args
//
Arg *
XtArgCollector::GetItems()
{
    return (list);
}

//
// Get the number of items
//
int
XtArgCollector::GetNumItems()
{
    return (num_items);
}


