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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: OrderList.C /main/6 1996/08/21 15:41:43 drk $
#include <stdio.h>

#define C_OrderList
#define L_Basic
#include <Prelude.h>

#if !defined(__uxp__) && !defined(USL)
#include <strings.h>
#endif

// //////////////////////////////////////////////////////////////
// Public methods
// //////////////////////////////////////////////////////////////

ListEntry::~ListEntry()
{
}

OrderList::~OrderList()
{
	if (f_size)
		clear();
}

void 
OrderList::clear()
{
	ListEntry *node;

    f_cursor = f_head;
    while (f_cursor != NULL) {
        node = f_cursor;
        f_cursor = f_cursor->f_next;
        delete node;
    }
    f_head = NULL;
    f_tail = NULL;
    f_cursor = NULL;
	f_size = 0;
    return;
}

int
OrderList::add(ListEntry *node, AddCode where, bool mvcursor)
{
    if (node != NULL) {
        if (f_cursor == NULL)
	  insertNew(node);
	else {
	    switch (where)
	      {
		case addAfter:
		  if (f_cursor == f_tail)
		    insertTail(node);
		  else
		    insertAfter(node);
		  break;
		case addBefore:
		  if (f_cursor == f_head)
		    insertHead(node);
		  else
		    insertBefore(node);
		  break;
		case addHead:
		  insertHead(node);
		  break;
		case addTail:
		  insertTail(node);
		  break;
		default:
		  return OLIST_ERROR;
	      }
	}
        if ((mvcursor) || (f_cursor == NULL))
	  f_cursor = node;
    }
    else
        return OLIST_ERROR;

	f_size++;

    return OLIST_OK;
}

int
OrderList::remove()
{
    ListEntry      *node;
    int             status = OLIST_OK;

    node = f_cursor;
    if ((node == f_head) && (node == f_tail)) {
        f_head = NULL;
        f_cursor = NULL;
        f_tail = NULL;
        status = OLIST_LAST_REMOVD;
    }
    else if (node == f_head) {
        f_head = node->f_next;
        f_cursor = node->f_next;
        f_cursor->f_prev = NULL;
        status = OLIST_HEAD_REMOVD;
    }
    else if (node == f_tail) {
        f_tail = node->f_prev;
        f_cursor = node->f_prev;
        f_cursor->f_next = NULL;
        status = OLIST_TAIL_REMOVD;
    }
    else {
        node->f_prev->f_next = node->f_next;
        node->f_next->f_prev = node->f_prev;
        f_cursor = node->f_next;
    }

    delete node;

    f_size--;

    return status;
}

ListEntry *
OrderList::extract()
{
    ListEntry      *node;
//  int             status = OLIST_OK;

    node = f_cursor;
    if ((node == f_head) && (node == f_tail)) {
        f_head = NULL;
        f_cursor = NULL;
        f_tail = NULL;
//      status = OLIST_LAST_REMOVD;
    }
    else if (node == f_head) {
        f_head = node->f_next;
        f_cursor = node->f_next;
        f_cursor->f_prev = NULL;
//      status = OLIST_HEAD_REMOVD;
    }
    else if (node == f_tail) {
        f_tail = node->f_prev;
        f_cursor = node->f_prev;
        f_cursor->f_next = NULL;
//      status = OLIST_TAIL_REMOVD;
    }
    else {
        node->f_prev->f_next = node->f_next;
        node->f_next->f_prev = node->f_prev;
        f_cursor = node->f_next;
    }

    // delete node;

    f_size--;

    return node;
}

int
OrderList::next()
{
  if (f_cursor != NULL)
    f_cursor = f_cursor->f_next;

  return OLIST_OK;
}

int
OrderList::prev()
{
    int             status = OLIST_ERROR;
    ListEntry      *index;

    if (f_cursor != NULL) {
        index = f_cursor->f_prev;
        if (index != NULL) {
            f_cursor = index;
            status = OLIST_OK;
        }
    }
    return status;
}

int
OrderList::head()
{
    int             status = OLIST_ERROR;

    if (f_head != NULL) {
        f_cursor = f_head;
        status = OLIST_OK;
    }
    return status;
}

int
OrderList::tail()
{
    int             status = OLIST_ERROR;

    if (f_tail != NULL) {
        f_cursor = f_tail;
        status = OLIST_OK;
    }
    return status;
}

ListEntry    *
OrderList::iterate(bool (*fn)(ListEntry *, void *), void *usr_def)
{
    // NOTE: keep this routine in sync with iterate below !
    ListEntry      *index;
    bool         keep_going = TRUE;
    ListEntry      *entry = NULL;

	// Iterate from head (arbitrary choice)
    index = f_head;
    while ((index != NULL) && (keep_going)) {
        keep_going = (*fn) (index, usr_def);
        if (!(keep_going))
            entry = index;
        else
            index = index->f_next;
    }
    return entry;
}
ListEntry    *
OrderList::iterate(bool (*fn)(OrderList *, ListEntry *, void *), void *usr_def)
{
    // NOTE: keep this routine in sync with iterate above

    ListEntry      *index;
    bool         keep_going = TRUE;
    ListEntry      *entry = NULL;

	// Iterate from head (arbitrary choice)
    index = f_head;
    while ((index != NULL) && (keep_going)) {
        keep_going = (*fn) (this, index, usr_def);
        if (!(keep_going))
            entry = index;
        else
            index = index->f_next;
    }
    return entry;
}

// //////////////////////////////////////////////////////////////
// Private methods
// //////////////////////////////////////////////////////////////

void 
OrderList::insertNew(ListEntry *node)
{
	f_head = node;
	f_tail = node;
	node->f_prev = NULL;
	node->f_next = NULL;
	return;
}

void 
OrderList::insertAfter(ListEntry *node)
{
	f_cursor->f_next->f_prev = node;
	node->f_next = f_cursor->f_next;
	f_cursor->f_next = node;
	node->f_prev = f_cursor;
	return;
}

void 
OrderList::insertBefore(ListEntry *node)
{
	f_cursor->f_prev->f_next = node;
	node->f_prev = f_cursor->f_prev;
	f_cursor->f_prev = node;
	node->f_next = f_cursor;
	return;
}

void 
OrderList::insertTail(ListEntry *node)
{
	ListEntry          *tnode;

	tnode = f_tail;
	f_tail = node;
	tnode->f_next = node;
	node->f_prev = tnode;
	node->f_next = NULL;
	return;
}

void 
OrderList::insertHead(ListEntry *node)
{
	ListEntry          *tnode;

	tnode = f_head;
	f_head = node;
	tnode->f_prev = node;
	node->f_next = tnode;
	node->f_prev = NULL;
	return;
}


