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
// $XConsortium: collectionIterator.cc /main/4 1996/06/11 17:28:28 cde-hal $

#include "oliasdb/collectionIterator.h"
#include "oliasdb/olias_consts.h"
#include "oliasdb/node_hd.h"
#include "oliasdb/stylesheet_hd.h"
#include "oliasdb/graphic_hd.h"
#include "oliasdb/locator_hd.h"

collectionIterator::collectionIterator(info_base* base, int position) :
   f_set_ptr(base ->  get_set(position)), f_base(base), f_index(-1)
{
   if ( f_set_ptr == 0 )
     throw(stringException("can't find set"));
}

collectionIterator::~collectionIterator()
{
}

unsigned int collectionIterator::operator++()
{
   if ( f_index == -1 ) {
      f_index = 0;
      return 1;
   }

   if ( f_index == -2 ) {
     MESSAGE(cerr, "can't advance iterator any more");
     return 0;
   }

   if ( (*f_set_ptr) -> count() - 1 == f_index ) {
      f_index = -2;
      return 0;
   } else {
      f_index++;
      return 1;
   }
}

oid_t collectionIterator::get_oid(int index)
{
   c_index_handler* x = (*f_set_ptr) -> get_index_ptr(index);

   if ( x == 0 )
     throw(stringException("bad internal index"));

   return (*x) -> first_of_invlist(f_index);
}

nodeCollectionIterator::nodeCollectionIterator(info_base* base) :
   collectionIterator(base, NODE_SET_POS)
{
}

nodeCollectionIterator::~nodeCollectionIterator()
{
}

const char* nodeCollectionIterator::get_locator()
{
   oid_t id = get_oid(1);
   node_smart_ptr node(f_base, id);
   return node.locator();
}

stylesheetCollectionIterator::stylesheetCollectionIterator(info_base* base) :
   collectionIterator(base, STYLESHEET_SET_POS)
{
}

stylesheetCollectionIterator::~stylesheetCollectionIterator()
{
}


const char* stylesheetCollectionIterator::get_locator()
{
   oid_t id = get_oid(1);
   stylesheet_smart_ptr ss(f_base, id);
   return ss.name();
}

graphicCollectionIterator::graphicCollectionIterator(info_base* base) :
   collectionIterator(base, GRAPHIC_SET_POS)
{
}

graphicCollectionIterator::~graphicCollectionIterator()
{
}


const char* graphicCollectionIterator::get_locator()
{
   oid_t id = get_oid(1);
   graphic_smart_ptr gr(f_base, id);
   return gr.locator();
}

locatorCollectionIterator::locatorCollectionIterator(info_base* base) :
   collectionIterator(base, LOCATOR_SET_POS)
{
}

locatorCollectionIterator::~locatorCollectionIterator()
{
}


const char* locatorCollectionIterator::get_locator()
{
   oid_t id = get_oid(1);
   locator_smart_ptr lc(f_base, id);
   return lc.inside_node_locator_str();
}


