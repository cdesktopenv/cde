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
/* $XConsortium: AttributeList.h /main/3 1996/06/11 17:05:38 cde-hal $ */
#ifndef _AttributeList_h
#define _AttributeList_h

#include "Element.h"
#include "Attribute.h"

/* **************************************************************
   class AttributeList

   A linked list of Attributes 
   ************************************************************** */


class AttributeList : private CC_TPtrSlist<Attribute>
{
public:
  AttributeList();
  virtual ~AttributeList();
  
  void add(Attribute *);

  const Attribute *lookup(const Symbol &name) const;

  ostream &print(ostream &) const ;

};

inline
ostream &operator<<(ostream &o, const AttributeList &attrlist)
{
  return attrlist.print(o);
}

#endif /* _AttributeList_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
