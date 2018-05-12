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
 * $XConsortium: List.hh /main/7 1996/08/10 11:31:57 cde-hal $
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



class List : public FolioObject
{
public: // variables
  enum grow_method_t { GROW_ADD, GROW_MULTIPLY };
  // notifications 
  enum notifications { APPENDED = FolioObject::_LAST, INSERTED, REMOVED };

public: // functions
  // increment really defaults to initial_size 
  List (int initial_size, int increment = 0, grow_method_t method = GROW_ADD,
	bool delete_elements = FALSE);
  virtual ~List();
  u_int length () const
    { return (f_length); }
  // NOTE: clean up and use either reference or ptr.  figure this out - DJB 
  virtual void append (FolioObject &);
  virtual void append (FolioObject *object)
    { append (*object); }
  virtual void remove (FolioObject &);
  virtual void remove (FolioObject *object)
    { remove (*object); }
  virtual void remove (unsigned int);
  // Change find to return FolioObect * 
  virtual int  find   (FolioObject &);           // returns -1 on failure 
  virtual void replace (int, FolioObject &);
  virtual void replace (int i, FolioObject *obj);
  virtual void insert (unsigned int, FolioObject *);
  virtual void remove_all()
    { f_length = 0; }
  virtual void remove_all (bool delete_elements);

  virtual FolioObject *operator[](int) const ;

  virtual List *copy() const;	// NOTE: just copies the pointers, not the
				// pointed at objects - jbm 

  void dtor_delete_elements (bool delete_elements)
    { f_dtor_delete_elements = delete_elements; }

protected: // functions
  void check_space (int num_additions = 1);

protected: // variables
  FolioObject    **f_list_element;        // array of list elements
  unsigned short   f_length;
  unsigned short   f_internal_length;
  unsigned short   f_increment;
  grow_method_t    f_grow_method;
  bool             f_dtor_delete_elements : 1;
};

inline FolioObject *
List::operator[] (int position) const
{
  // NOTE: need to be able to "throw" an exception here!!
  if (position >= f_length)
    abort ();

  return (f_list_element[position]);
}

inline void
List::replace (int /* position */, FolioObject& /* element */)
{ 
  // This function is obsolete, do not try to use
  // someone needs to clean it up.
  abort();
}

inline void
List::replace (int position, FolioObject *element)
{
  replace (position, *element);
}

