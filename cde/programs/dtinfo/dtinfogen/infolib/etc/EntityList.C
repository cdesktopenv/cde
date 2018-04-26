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
/* $XConsortium: EntityList.cc /main/2 1996/07/18 16:11:54 drk $ */

/* exported interfaces */
#include "EntityList.h"

/* imported interfaces */
#include <stdio.h>
#include "SGMLName.h"
#include "SGMLDefn.h"

//---------------------------------------------------------
EntityList::EntityList()
{
  head = NULL;
  tail = NULL;
  next = NULL;
}

//---------------------------------------------------------
EntityList::~EntityList()
{
  SGMLDefn *pt = head;
  while ( pt ) {
    SGMLDefn *tmp = pt;
    pt = pt->next;
    delete tmp;
  }
}
//---------------------------------------------------------
// EntityList:lookup
SGMLDefn *
EntityList::lookup(int ename ) const
{

  SGMLDefn *pt = head;

  while ( pt ) {

    if ( pt->getName() == ename ) {
      return ( pt );
    }

    pt = pt->next;
  }

  return ( NULL );

}

//---------------------------------------------------------
// EntityList::add
void
EntityList::insert( SGMLDefn *entry )
{

  if ( !tail ) {
    head = tail = entry;
  }
  else {
    tail->next = entry;
    tail       = entry;
  }

}
