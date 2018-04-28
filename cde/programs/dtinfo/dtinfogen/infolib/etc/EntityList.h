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
/* $XConsortium: EntityList.h /main/2 1996/07/18 16:43:25 drk $ */
#ifndef ENT_LIST_HDR
#define ENT_LIST_HDR

#include "SGMLDefn.h"

class EntityList {

friend class EntityScope;
friend class Dispatch;
  
protected:
  SGMLDefn *head;
  SGMLDefn *tail;
  EntityList *next;

  SGMLDefn *lookup( int ) const;
  void      insert  ( SGMLDefn * );

  // SGMLDefn *GetFirstAttr() const;
  // SGMLDefn *GetNextAttr( const SGMLDefn *) const;

  EntityList();
  ~EntityList();
};

#endif


