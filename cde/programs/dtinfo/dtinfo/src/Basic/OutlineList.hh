/*
 * $XConsortium: OutlineList.hh /main/3 1996/06/11 16:21:30 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

class OutlineList : public List
{
public: // functions
  // increment defaults to initial size 
  OutlineList (int initial_size, int increment = 0,
	       grow_method_t grow_method = List::GROW_ADD);

  ~OutlineList();

  BitHandle get_data_handle()	{ return f_bit_field.get_handle() ; }
  void free_data_handle (BitHandle data_handle)
  { f_bit_field.free_handle(data_handle) ; }

  virtual u_int count_expanded (BitHandle data_handle);

  void copy_selected(BitHandle source, BitHandle destination);
  void copy_expanded(BitHandle source, BitHandle destination);
  void recursive_select(BitHandle data_handle);
  void select_all(BitHandle data_handle); // and children too
  void deselect_all(BitHandle data_handle); // and children too

  // If l == NULL new list is created and returned, else items
  // that are selected are appended to l.  l is NOT emptied first. 
  List *selected_items (BitHandle data_handle, List *l = NULL);

protected: // functions

protected: // variables

  BitField	f_bit_field;
};


// /////////////////////////////////////////////////////////////////
// OutlineList Iterator Class
// /////////////////////////////////////////////////////////////////

#if 0
// NOTE: This isn't finished!!

class OutlineListIterator : public Iterator
{
public:
  OutlineListIterator (const OutlineList &olist)
    : f_olist (olist), f_index (0)
    { }
  FolioObject *init();

protected:
  const OutlineList &f_olist;
  u_int              f_index;
};
#endif
