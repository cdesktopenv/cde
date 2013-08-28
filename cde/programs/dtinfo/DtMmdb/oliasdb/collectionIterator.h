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
/* $XConsortium: collectionIterator.h /main/3 1996/06/11 17:28:33 cde-hal $ */

#ifndef _collection_iterator_h
#define _collection_iterator_h 1

#include "api/info_base.h"
#include "object/cset.h"

// collectionIterator iterates over MMDB's cset objects through
// index.

class collectionIterator
{
public:
   collectionIterator(info_base*, int set_position);
   ~collectionIterator();

// advance the iterator to the next position.
// A newly constructed iterator's position is
// undefined. This operator must be called to
// advance to the 1st position.

// return 1 if advance successful and 0 otherwize.
   virtual unsigned int operator++() ;

protected:
   oid_t get_oid(int index);

protected:
   cset_handler* f_set_ptr;
   info_base* f_base;
   int f_index;
};

// iterator over node set
class nodeCollectionIterator : public collectionIterator
{

public:
   nodeCollectionIterator(info_base*);
   ~nodeCollectionIterator();

   const char* get_locator();
};

// iterator over graphic set
class graphicCollectionIterator : public collectionIterator
{

public:
   graphicCollectionIterator(info_base*);
   ~graphicCollectionIterator();

   const char* get_locator();
};

// iterator over stylesheet set
class stylesheetCollectionIterator : public collectionIterator
{

public:
   stylesheetCollectionIterator(info_base*);
   ~stylesheetCollectionIterator();

   const char* get_locator();
};

// iterator over locator set
class locatorCollectionIterator : public collectionIterator
{

public:
   locatorCollectionIterator(info_base*);
   ~locatorCollectionIterator();

   const char* get_locator();
};

#endif
