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
/* $XConsortium: EntityScope.h /main/2 1996/07/18 16:43:47 drk $ */
#ifndef ENT_SCOPE_HDR
#define ENT_SCOPE_HDR

#include "EntityList.h"

class EntityScope {
private:
  EntityList *currentEntityScope;

public:
  void push ( EntityList *escope ) {
    escope->next = currentEntityScope;
    currentEntityScope = escope;
  }

  EntityList *pop();
  EntityList *GetTopEntities() const { return(currentEntityScope); }

  SGMLDefn *LookupEntity( int ename );

  /*
   * Constructor EntityScope creates a topmost entitylist
   * so the stack will contain an element after it is constructed
   */
  
  EntityScope() { currentEntityScope = new EntityList(); }
  ~EntityScope();

};

//----------------------------------------------------------------
inline
SGMLDefn *
EntityScope::LookupEntity( int ename )
{
  return ( currentEntityScope->lookup( ename ) );
}

//-----------------------------------------------------------------
inline
EntityList *
EntityScope::pop()
{
  EntityList *ptr;
  
  ptr = currentEntityScope;
  currentEntityScope = ptr->next;
  return ( ptr );
}

//-----------------------------------------------------------------
inline
EntityScope::~EntityScope()
{
  EntityList *pt = currentEntityScope;
  while( pt ) {
    EntityList *tmp = pt;
    pt = pt->next;
    delete tmp;
  }
}

#endif

  
