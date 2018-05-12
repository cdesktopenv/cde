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
 * $XConsortium: HistoryList.hh /main/4 1996/12/11 18:19:48 cde-hal $
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

class HistoryListElement;
class ObjectId;

class HistoryList : public List
{
public: // functions
  HistoryList (int default_size);
  // Next line cause OK warning message
  virtual void append (FolioObject &);
  virtual void append (FolioObject *object)
    { append (*object); }
  virtual void remove_first();
  void set_length (int);
  virtual FolioObject *operator[](int) const;

protected: // functions

  void size_change(FolioObject *, u_int, void *, void *);

protected: // variables
  int f_first_element;
  // In a history list, f_internal_length is the max number of elements.
};

inline FolioObject *
HistoryList::operator[] (int position) const
{
  if (position >= f_length)
    abort();			// NOTE: clearly this won't do

  // Calculate the actual element position based on the first element
  // in the history ring.
  return ((FolioObject *)
	  f_list_element[(f_first_element + position) % f_internal_length]);
}
