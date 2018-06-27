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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_object_list.C /main/3 1995/10/23 10:42:37 rswiston $ 			 				
/* @(#)tt_object_list.C	1.17 93/07/30
 *
 * tt_object_list.cc
 *
 * Copyright (c) 1991, 1992, 1993 by Sun Microsystems, Inc.
 * 
 * This is the list package.
 */

#include "util/tt_object_list.h"
#include "util/tt_assert.h"
#include "util/tt_iostream.h"

/*
 * Construct a new empty list.
 */
_Tt_object_list::
_Tt_object_list()
{
	first = 0;
	last =  0;
	_count = 0;
}

/*
 * Copy a list.  Sort of expensive, since elements aren't refcounted.
 * Should be used sparingly.
 */
_Tt_object_list::
_Tt_object_list(const _Tt_object_list &t)
{
	_Tt_object_list_element *p, *n;

	first = 0;
	last = 0;
	p = t.first;
	while (p != 0) {
		n = new _Tt_object_list_element;
		n->data = p->data;
		n->prev = last;
		n->next = 0;
		if (!first) {
			first = n;
		}
		if (last) {
			last->next = n;
		}
		last = n;
		p = p->next;
	}
	_count = t._count;
}

/*
 * Destroy list, freeing all the elements.
 */
_Tt_object_list::
~_Tt_object_list()
{
	_Tt_object_list_element *p, *q;
	p = first;
	while (p != 0) {
		q = p->next;
		delete p;
		p = q;
	}
}

/*
 *  Freeing all the elements, and reset.
 */
void _Tt_object_list::
flush()
{
	_Tt_object_list_element *p, *q;
	p = first;
	while (p != 0) {
		q = p->next;
		delete p;
		p = q;
	}
	first = 0;
	last =  0;
	_count = 0;
}

/*
 * push: put a new element on the top of the list.
 */
_Tt_object_list& _Tt_object_list::
push(const _Tt_object_ptr &e)
{
	_Tt_object_list_element *n = new _Tt_object_list_element;

	_count++;
	n->data = e;
	n->next = first;	/* n->prev is nulled in constructor */
	if (0==first)
          last = n;
	else
	  first->prev = n;
	first = n;
	return *this;
}

/*
 * pop: remove the element from the top of the list.
 */
_Tt_object_list& _Tt_object_list::
pop()
{
	_Tt_object_list_element *n;

	ASSERT(first,"Pop of empty list");
	_count--;
	n = first->next;
	if (0==n) {
		last = 0;
	} else {
		n->prev = 0;
	}
	delete first;
	first = n;
	return *this;
}

/*
 * append: put a new element at the bottom of the list
 */
_Tt_object_list& _Tt_object_list::
append(const _Tt_object_ptr &e)
{
	_Tt_object_list_element *n = new _Tt_object_list_element;

	_count++;
	n->data = e;
	n->prev = last;		/* n->next is nulled in constructor */
	if (0==last)
	  first = n;
	else
	  last->next = n;
	last = n;
	return *this;
}

/*
 * Create a new list which is the current list appended to the given list.
 */
_Tt_object_list& _Tt_object_list::
append(_Tt_object_list_ptr l)
{
	_Tt_object_list_ptr nl = new _Tt_object_list(*l);
	append_destructive(nl);
	return *this;
}

/*
 * append_destructive: append all elements of supplied list to the list.
 * as a side effect, empty the supplied list! This somewhat-surprising
 * side effect lets us do the append very cheaply by reusing all the
 * _Tt_object_list_element instances.
 */
_Tt_object_list& _Tt_object_list::
append_destructive(_Tt_object_list_ptr l)
{
	if (0==l->first) {
		/* second list is empty, append does nothing */
		return *this;
	} else if (0==first) {
		/* first list is null */
		first = l->first;
	last = l->last;
		_count = l->_count;
	} else {
		/* real work to do */
		l->first->prev = last;
		last->next = l->first;
		last = l->last;
		_count += l->_count;
	}
	/* empty the second list */
	l->first = 0;
	l->last = 0;
	l->_count = 0;
	return *this;
}
/*
 * dequeue: remove the element from the bottom of the list.
 */
_Tt_object_list& _Tt_object_list::
dequeue()
{
	_Tt_object_list_element *n;

	ASSERT(last,"Dequeue from empty list");

	_count--;
	n = last->prev;
	if (0==n) {
		first = 0;
	} else {
		n->next = 0;
	}
	delete last;
	last = n;
	return *this;
}

/*
 * Return a pointer to the contents of the element on the top of the list
 */
_Tt_object_ptr &_Tt_object_list::
top() const
{
	ASSERT(first,"top of empty list");
	return first->data;
}

/*
 * Return a pointer to the contents of the element on the bottom of the list
 */
_Tt_object_ptr &_Tt_object_list::
bot() const
{
	ASSERT(last,"bottom of empty list");
	return last->data;
}

/*
 * Return a pointer to the contents of the n-th element on the list
 *
 * Note: this implementation could be very slow for large lists.
 * The API routines use it heavily.  It's likely the API accesses would be
 * sequential, so an improvment would be to have a "cache pointer" to the
 * last retrieved element along with its index; then sequential accesses
 * could be very fast.
 */
_Tt_object_ptr &_Tt_object_list::
operator[](int n) const
{
	ASSERT(0<=n && n<=count(),"subscript out of range");
	_Tt_object_list_element *p;
	p = first;
	while(n--) p = p->next;
	return p->data;
}

/*
 * Predicate that returns 1 iff list is empty, else returns 0
 */
int _Tt_object_list::
is_empty() const
{
	return first==0;
}

/*
 * return number of elements in the list
 */
/* 
 * int _Tt_object_list::
 * count() const
 * {
 * 	return _count;
 * }
 */
/*
 *  XDR encoding and decoding function for lists
 */
bool_t _Tt_object_list::
xdr(XDR *xdrs, _Tt_new_xdrfn xdrfn, _Tt_object *(*make_new)())
{
        if (! xdr_int(xdrs, &_count)) {
		return(0);
	}
	if (xdrs->x_op == XDR_ENCODE) {
	        _Tt_object_list_cursor	cursor(this);
		_Tt_object_ptr          optr;

		while (cursor.next()) {
		        optr = *cursor;
			if (optr.is_null()) {
				continue;
			}
			if (! (*xdrfn)(xdrs, optr.c_pointer())) {
				return(0);
			}
		}
	} else {
	        int			i;
		int			len;
	        _Tt_object_ptr		ptr;

		len = _count;
		flush();
		for (i=len; i > 0; --i) {
			ptr = (*make_new)();
			if (! (*xdrfn)(xdrs, ptr.c_pointer())) {
				return(0);
			} else {
				append(ptr);
			}
		}
	}

	return(1);
}


void _Tt_object_list::
print(_Tt_object_printfn print_elt, const _Tt_ostream &os) const
{
	_Tt_object_list_element *cur;
	cur = first;
	while (cur) {
		(*print_elt)(os, (cur->data).c_pointer());
		os << " ";
		cur = cur->next;
	}
}

// verify that an object list is OK.  Since lists are so generic, about all
// we can do is ensure that back pointers do point back, and that every
// element points to a object with a reasonable refcount. Here "reasonable"
// is greater than zero (a must) and less than 1,000,000 (an arbitrarily
// chosen upper bound.)  We also allow the user to pass in a pointer to a
// predicate function which can verify the objects on the list.  We call
// that with each object in turn.  if it returns zero (false) there is a bad
// object on the list.
int _Tt_object_list::
verify(int (*verifier)(const _Tt_object *))
{
	_Tt_object_list_element *cur;
	int n = 0;
	cur = first;
	while (cur) {
		if (0==cur->prev && cur!=first) {
			fprintf(stderr,
				"_Tt_object_list::verify: "
				"bad null prev at element %d\n",
				n);
			return 0;
		}
		if (0!=cur->next && cur->next->prev!=cur) {
			fprintf(stderr,
				"_Tt_object_list::verify: "
				"bad next->prev at element %d\n",
				n);
			return 0;
		}
		if (0==cur->next && cur!=last) {
			fprintf(stderr,
				"_Tt_object_list::verify: "
				"bad null next at element %d\n",
				n);
			return 0;
		}
		if (cur->data.is_null()) {
			fprintf(stderr,
				"_Tt_object_list::verify: "
				"null data pointer at element %d\n",
				n);
			return 0;
		}
		if (cur->data->verify_refcount(1000000) < 0) {
			fprintf(stderr,
				"_Tt_object_list::verify: "
				"nonpositive _refcount_ at element %d\n",
				n);
			return 0;
		}
		if (cur->data->verify_refcount(1000000) > 0)  {
			fprintf(stderr,
				"_Tt_object_list::verify: "
				"absurdly large _refcount_ at element %d\n",
				n);
			return 0;
		}
		if (0 != verifier &&
		    0 == (*verifier)(cur->data.c_pointer())) {
			fprintf(stderr,
				"_Tt_object_list::verify: "
				"verifier rejected object at element %d\n",
				n);
			return 0;
		}

		cur = cur->next;
		++n;
	}
	return 1;
}
		
	


/*
 * Constructor for list elements
 */
_Tt_object_list_element::
_Tt_object_list_element()
{
	next = 0;
	prev = 0;
}

_Tt_object_list_element::
~_Tt_object_list_element()
{
}

/*
 * The _Tt_object_list_cursor methods implement an iterator which
 * provides for convenient stepping along the list.
 */
/*
 * The simple constructor for a cursor creates a cursor which
 * points nowhere. reset(_Tt_object_list_ptr l) must be done before doing
 * anything else with the cursor.
 */
_Tt_object_list_cursor::
_Tt_object_list_cursor()
{
	current = 0;
	flags = 0;
}

/*
 * The copy constructor for a cursor replicates the current position.
 * Note that if two cursors point to the same element and one deletes
 * it, the other cursor is hosed.
 */
_Tt_object_list_cursor::
_Tt_object_list_cursor(const _Tt_object_list_cursor &c)
{
	current = c.current;
	listhdr = c.listhdr;
	flags = c.flags;
}

/*
 * The usual constructor for a cursor supplies a list.  The cursor
 * is set before the first element in the list (as if reset(list) had
 * been done.)
 */
_Tt_object_list_cursor::
_Tt_object_list_cursor(const _Tt_object_list_ptr &l)
{
	current = 0;
	listhdr = l;
	flags = 1<<INIT;
	flags &= ~(1<<DELETED);
}

_Tt_object_list_cursor::
~_Tt_object_list_cursor()
{
}

/*
 * reset() moves a cursor back before the start of its list.
 */
_Tt_object_list_cursor& _Tt_object_list_cursor::
reset()
{
	ASSERT(flags&(1<<INIT),"Attempt to reset an unset cursor.");
	current = 0;
	flags &= ~(1<<DELETED);
	return *this;
}

/*
 * reset(_Tt_object_list_ptr l) resets the cursor to a new list, positioned
 * before the first element.
 */
_Tt_object_list_cursor& _Tt_object_list_cursor::
reset(const _Tt_object_list_ptr &l)
{
	current = 0;
	listhdr = _Tt_object_list_ptr(l);
	flags |= 1<<INIT;
	return *this;
}

/*
 * operator * returns a ref_counted pointer to the object under the cursor.
 */
_Tt_object_ptr &_Tt_object_list_cursor::
operator * () const
{
	if (current != 0) {
		return current->data;
	} else {
		return _Tt_object::null_ptr();
	}
}

/*
 * operator -> returns a ref-counted pointer to the object under the cursor,
 * which c++ will then apply the _Tt_object_PTR::operator-> to.
 */
_Tt_object_ptr &_Tt_object_list_cursor::
operator -> () const
{
	return current->data;
}

/*
 * next() moves the cursor to the next element in the list and returns 1
 * unless there is no next element, in which case it returns 0.
 */
int _Tt_object_list_cursor::
next()
{
	flags &= ~(1<<DELETED);
	if (current!=0) {
	        current = current->next;
	} else {
		if (listhdr.is_null()) {
			return 0;
		} else {
			current = listhdr->first;
		}
	}
	return current!=0;
}

/*
 * prev() moves the cursor to the prev element in the list and returns 1
 * unless there is no prev element, in which case it returns 0.
 */
int _Tt_object_list_cursor::
prev()
{
	if (flags&(1<<DELETED)) {
		/* already on prev element */
		flags &= ~(1<<DELETED);
	} else if (current!=0) {
		current = current->prev;
	} else {
		current = listhdr->last;
	}
	return current!=0;
}

/*
 * insert(_Tt_object_ptr p) inserts a new element after the current one.
 * inserting right after a delete puts the new element where the
 * old deleted one was.  inserting into a reset cursor puts the
 * new element first. The cursor points to the new element after insert.
 */
_Tt_object_list_cursor& _Tt_object_list_cursor::
insert(const _Tt_object_ptr &p)
{
	flags &= ~(1<<DELETED);
	if (current == 0) {	/* reset cursor */
		listhdr->push(p);
		current = listhdr->first;
	} else {
		_Tt_object_list_element *n = new _Tt_object_list_element;
		listhdr->_count++;
		if (current->next != 0)
		  current->next->prev = n;
		else
		  listhdr->last = n;
		n->next = current->next;
		n->prev = current;
		n->data = _Tt_object_ptr(p);
		current->next = n;
		current = n;
	}
	return *this;
}

/*
 * remove() deletes the element under the cursor. The cursor is set up
 * so that next() after remove() will get the element after the one
 * that was removed, and  prev() after remove() will get the element before
 * the one that was removed.
 */
_Tt_object_list_cursor& _Tt_object_list_cursor::
remove()
{
	ASSERT(current,"No current element to delete");
	_Tt_object_list_element *p = current;
	listhdr->_count--;
	if (p->next != 0)
	  p->next->prev = p->prev;
	else
	  listhdr->last = p->prev;
	if (p->prev != 0)
          p->prev->next = p->next;
	else
	  listhdr->first = p->next;
	flags |= 1<<DELETED;
	current = p->prev;
	delete p;
	return *this;
}
/*
 * is_valid returns 1 if there is an element under the cursor, else 0
 */
int _Tt_object_list_cursor::
is_valid() const
{
	return 0!=current;
}

implement_ptr_to(_Tt_object)
implement_ptr_to(_Tt_object_list)
