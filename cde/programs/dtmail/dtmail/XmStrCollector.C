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
 *	$TOG: XmStrCollector.C /main/4 1997/09/04 09:18:11 mgreess $
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
 * The XmStrCollector class is simply a dynamic array of XmStrings.
 * It is useful to build an XmStringTable to pass as an argument to
 * any Motif widget that needs one (eg. XmList).
 */
#include <Xm/List.h>
#include <stdlib.h>
#include <stdio.h>
#include "XmStrCollector.h"

XmStrCollector::XmStrCollector()
{
    // Initialize the class variables
    num_items = 0;
    max_num_items = 0;
    increment = 10;
    list = NULL;
}

XmStrCollector::~XmStrCollector() 
{
    // Free the list items
    for (int i=0; i<num_items; i++)
	XmStringFree (list[i]);

    // Free the list
    XtFree ((char *)list);
}

//
// Add an item to the string collector list
//
void
XmStrCollector::AddItemToList (XmString item)
{
    // Allocate memory for 10 items at a time.
    // When this memory is exceeded, allocate
    // space for 10 more.  
    if (num_items >= max_num_items)
    {
        max_num_items += increment;
 
        list = (XmString *) XtRealloc ((char *)list,
		sizeof (XmString) *max_num_items);
    }
 
    list[num_items] = item;

    num_items++;
}

//
// Get the XmStringTable
//
XmString *
XmStrCollector::GetItems()
{
    return (list);
}

//
// Get the number of items
// in the XmStringTable
//
int
XmStrCollector::GetNumItems()
{
    return (num_items);
}


