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
/* $XConsortium: Attribute.h /main/3 1996/06/11 17:05:27 cde-hal $ */
#ifndef _Attribute_h
#define _Attribute_h

#include "SymTab.h"

/* **************************************************************
   class Attribute

   a name/value pairing
   ************************************************************** */

class Attribute
{
public:
  Attribute(const Symbol &name, char *value = 0);
  ~Attribute();

  const Symbol &name() const	{ return f_name; }
  bool operator==(const Attribute &) const ;

  const char   *value() const	{ return f_value ; } 

  ostream &print(ostream &) const;

private:
  Symbol        f_name;
  char	       *f_value ;
};

inline
ostream &operator<<(ostream &o, const Attribute &a)
{
  return a.print(o);
}

#endif /* _Attribute_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
