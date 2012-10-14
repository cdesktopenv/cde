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
// $TOG: Element.C /main/5 1998/04/17 11:48:25 mgreess $
#include <iostream>
using namespace std;
#include <stdlib.h>
#include "StyleSheetExceptions.h"
#include "Element.h"
#include "AttributeList.h"

// /////////////////////////////////////////////////////////////////////////
// Element
// /////////////////////////////////////////////////////////////////////////

#define OLIAS_SIBLING_INFO "LAST"


Element::Element(const Element& element) 
:
  f_gi(element.f_gi),
  f_sibling_number(element.f_sibling_number),
  f_attributes(element.f_attributes),
  f_olias_attributes(element.f_olias_attributes),
  f_freeAttrLists(false),
  f_relative_sibling_number(element.f_relative_sibling_number),
  f_last_child(element.f_last_child),
  f_relatively_last_child(element.f_relatively_last_child)

{
}

Element::Element(const Symbol& 	gi,
		 unsigned int	sibling_number,
		 AttributeList *attlist,
		 AttributeList *olias_attributes,
		 unsigned int  relative_sibling_number

		)
: f_gi(gi),
  f_sibling_number(sibling_number),
  f_attributes(attlist),
  f_olias_attributes(olias_attributes),
  f_freeAttrLists(true),
  f_relative_sibling_number(relative_sibling_number)
{
    f_last_child = 0;
    f_relatively_last_child = 0;

    Symbol name(gSymTab->intern(OLIAS_SIBLING_INFO));
    const Attribute* x = get_olias_attribute(name);

        if ( x && x -> value() ) {
            int code = atoi(x -> value());
            switch (code) {
               case 0:
               f_last_child = 0;
               f_relatively_last_child = 0;
               break;
             case 1:
               f_last_child = 0;
               f_relatively_last_child = 1;
               break;
             case 2:
               f_last_child = 1;
               f_relatively_last_child = 1;
               break;
             default:
               throw(CASTEXCEPT Exception());
            }
       }
}

Element::~Element()
{
  if ( f_freeAttrLists == true ) {
     delete f_olias_attributes;
     delete f_attributes;
  }
}


const Attribute *
Element::get_attribute(const Symbol &name) const
{
  if (f_attributes)
    return f_attributes->lookup(name);

  return 0 ;
}

const Attribute *
Element::get_olias_attribute(const Symbol &name) const
{
  if (f_olias_attributes)
    return f_olias_attributes->lookup(name);
  
  return 0 ;
}

ostream &
Element::print(ostream &o) const
{
  o << '<' << f_gi ;

#ifdef SIBLING_DEBUG
  o << "[" << this->f_sibling_number << "]";
  o << "[" << this->f_relative_sibling_number<< "]";
  o << "[" << this-> f_last_child << "]";
  o << "[" << this-> f_relatively_last_child<< "]";
#endif

  if (f_attributes)
    o << *f_attributes ;

  if (f_olias_attributes)
    o << " #" << *f_olias_attributes ;

  o << '>' ;

  return o;
}
