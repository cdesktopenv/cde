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
 *	$TOG: Sort.hh /main/6 1998/09/02 18:26:17 mgreess $
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

#ifndef SORT_HH
#define SORT_HH

#include <DtMail/DtMail.hh>
#include "FindDialog.h"
#include "MsgScrollingList.hh"

//
// What method to use to sort the message list.
//
typedef enum sortBy {
    SortTimeDate = 1,
    SortSender,
    SortSubject,	
    SortSize,
    SortStatus,
    SortMsgNum
} SortBy;

//
// Used for sorting the mailbox.
//
class Sort {

public:


  //
  // Sorts the array of message handles.
  //
  int sortMessages(MsgScrollingList	*display_list,
  		    DtMail::MailBox	*mbox,
		    sortBy		howToSort);

private:

  //
  // This is the message record that is sorted (not returned). It
  // is for internal use only and is never passed up to the calling
  // public member functions.
  //
  // The variable 'link' is used to sort the array in place. After
  // msort() (below) is called, the 'link' variable is set to the
  // link-order (1,2,...,N). Un-like qsort(), msort() sorts in place
  // and does not do memcpy(), msort() - updates the 'link' variable.
  // 
  
  struct messageRecord {
    int		 link;		// Needed by msort(). (see source).
    char	*primary_key_str;
    int		 primary_key_int;
    int		 secondary_key_int;
    MsgStruct	*msg_struct;
  };

  //
  // Similar to qsort().
  //
  int	_msort(char		* base,		// Base of array.
	       int		  nel,		// Number of elements in array.
	       int		  width,	// Size of each element.
	       int		  offset,	// Offset to'link'(see source).

	       // Compare function.
	       int		(*compar)(char ** one, char ** two));

  //
  // This is the function that msort() calls to compare two records.
  //
  static int	_sortCmp(char **one, char **two);
};

#endif // SORT_HH
