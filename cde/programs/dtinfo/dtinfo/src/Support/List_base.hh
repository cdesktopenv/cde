/*
 * $TOG: List_base.hh /main/5 1997/12/29 10:27:35 bill $
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

#ifndef _List_base_hh
#define _List_base_hh

#ifndef NULL
#define NULL 0
#endif

// Base classes for List template. 

class Link_base
{
friend class List_base;
friend class List_Iterator_base;
private:
  Link_base *f_next;
};

class List_Iterator_base;

class List_base
{
friend class List_Iterator_base;
public:
  List_base()
    : f_head (NULL), f_tail (NULL), f_length (0)
    { }

  void insert (Link_base *);
  void insert_before (List_Iterator_base &, Link_base *);
  void insert_after (List_Iterator_base &, Link_base *);
  void append (Link_base *);
  Link_base *remove (List_Iterator_base &);

  unsigned int length() const
    { return (f_length); }

private:
  Link_base    *f_head;
  Link_base    *f_tail;
  unsigned int  f_length;
};


class List_Iterator_base
{
friend class List_base;
protected:
  List_Iterator_base()
    : f_current (NULL)
    { }
  List_Iterator_base (const List_base *list);
  // Obtain the current link entry.
  Link_base *item() const
    { return (f_current); }
  // Reset the iterator to the first list element. 
  void reset();
  // Reset the iterator to the last list element.
  void last();
  // Set the iterator to a new list.
  void operator= (const List_base *list)
    { f_list = list; reset(); }
  // Increment the iterator. 
  void *operator++();
  // For testing EOL. 
  operator void*() const
    { return (f_current); }
#ifndef DEC
  int operator!=(int i)
    { return (f_current != (Link_base *)(size_t)i); }
#endif

  // For internal List_base usage. 
  Link_base *previous() const
    { return (f_previous); }
  
private:
  const List_base *f_list;
  Link_base       *f_previous;
  Link_base       *f_current;
};

#endif /* _List_base_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
