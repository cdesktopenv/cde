/*
 * $XConsortium: CC_Listbase.h /main/4 1996/08/21 15:48:41 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#ifndef _CC_Listbase_hh
#define _CC_Listbase_hh

#include <stdio.h>
#include "dti_cc/types.h"

#ifndef NULL
#define NULL 0
#endif

// forward declaration
class CC_List_Iterator_base;
class CC_Listbase;

// Base classes for List template. 
// Link_base can be used by both doubly and singly linked list
class CC_Link_base
{
friend class CC_Listbase;
friend class CC_List_Iterator_base;
public:
  CC_Link_base():f_next(NULL),f_prev(NULL) {}

private:

  CC_Link_base *f_next;
  CC_Link_base *f_prev;
};



class CC_Listbase
{
friend class CC_List_Iterator_base;

protected:
  CC_Link_base *remove(CC_List_Iterator_base &);

public:
  CC_Listbase()
    : f_head (NULL), f_tail (NULL), f_length (0)
    { }

  void insert (CC_Link_base *);  /* both insert and append is the same,
			       * throw ccException() if element is NULL
			       */

  void append (CC_Link_base *e)  // in order to be Rogue Wave compatible
  { insert(e); }

  void prepend (CC_Link_base *); /*
			       * throw ccException() if element is NULL
			       */



  size_t entries() const        // RW compatible
    { return (f_length); }

  CC_Link_base *first() const
  { return (f_head); }

  CC_Link_base *last() const
  { return (f_tail); }

  CC_Link_base *removeLast();
  CC_Link_base *removeFirst();

protected:
  CC_Link_base    *f_head;
  CC_Link_base    *f_tail;
  size_t        f_length;
};


class CC_List_Iterator_base
{
friend class CC_Listbase;
public:

  CC_List_Iterator_base (CC_Listbase *list); /* will throw ccException if 
					       * list == NULL 
					       */
  // Obtain the current link entry.
  CC_Link_base *item() const
    { return (f_current); }

  // Reset the iterator to the first list element. 
  void reset();

  // Increment the iterator. 
  CC_Boolean operator++();
  CC_Boolean operator--();

protected:
  const CC_Listbase *f_list;
  CC_Link_base       *f_previous;
  CC_Link_base       *f_current;
};

#endif /* _CC_Listbasehh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
