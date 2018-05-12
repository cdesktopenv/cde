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
// $XConsortium: OrderList.hh /main/3 1996/06/11 16:21:08 cde-hal $
#ifndef _OrderListType_hh
#define _OrderListType_hh


// List status definitions

#define OLIST_ERROR        (int)-1
#define OLIST_OK           (int)1
#define OLIST_NOOP         (int)0
#define OLIST_LAST_REMOVD  (int)100
#define OLIST_TAIL_REMOVD  (int)101
#define OLIST_HEAD_REMOVD  (int)102

// Where flags for add

enum AddCode {
 	addAfter,               // Add after list cursor
	addBefore,              // Add before list cursor
	addHead,                // Add at list head
	addTail                 // Add at list tail
};

class ListEntry;

class OrderList {
 public:
    // Constructor
    OrderList();
    // Destructor
    ~OrderList();
    // Public methods
    void       clear();
    int        add(ListEntry *entry, AddCode where, bool mvcursor = 1);
    int        remove();
    ListEntry *extract();
    bool    isempty();
    int        size();
    int        next();
    int        prev();
    int        head();
    int        tail();
    ListEntry *value();
    ListEntry *set_cursor(ListEntry *cursor_pos);
    // NOTE: could just put the OrderList as user data
    ListEntry *iterate(bool (*fn)(ListEntry *, void *), void *usr_def);
    ListEntry *iterate(bool (*fn)(OrderList *, ListEntry *, void *), void *usr_def);
  private:
    // Private data
    int			f_size   ;
    ListEntry	       *f_head   ;
    ListEntry	       *f_tail   ;
    ListEntry	       *f_cursor ;
    
    // Private methods
    void insertNew    (ListEntry *node);
    void insertAfter  (ListEntry *node);
    void insertBefore (ListEntry *node);
    void insertTail   (ListEntry *node);
    void insertHead   (ListEntry *node);
};

inline OrderList::OrderList()
: f_size(0),
  f_head(NULL),
  f_tail(NULL),
  f_cursor(NULL)
{
}

inline int
OrderList::size()
{
    return f_size ;
}

inline bool
OrderList::isempty()
{
    if (f_size)
      return FALSE;
    else
      return TRUE;
}

inline ListEntry    *
OrderList::value()
{
    return f_cursor;
}

inline ListEntry *
OrderList::set_cursor(ListEntry *value)
{
    assert(value != NULL);
    f_cursor = value ;
    return f_cursor ;
}
// //////////////////////////////////////////////////////////////
// List entry class
// //////////////////////////////////////////////////////////////

class ListEntry : public FolioObject {
friend class OrderList;
    
  public:
    ListEntry () ;
    virtual ~ListEntry ();
    
  private:
    ListEntry	*f_prev ;
    ListEntry	*f_next ;
} ;

inline 
ListEntry::ListEntry() 
: f_prev(NULL),
  f_next(NULL)
{
}

#endif				/* _OrderListType_hh */
