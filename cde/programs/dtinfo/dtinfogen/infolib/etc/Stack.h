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
/* $XConsortium: Stack.h /main/2 1996/07/18 16:49:58 drk $ */

#ifndef STACK_HEADRER
#define STACK_HEADER

//-----------------------------------------------------------------------
class Element{

friend class Stack;
friend class SearchEngine;  
  
protected:
  int name;
  Element *next;
  Element( int aName, Element *v=0) { name = aName; next = v; }
  int GetName() const { return name; }
  
};

class Stack {
private:
  Element *currentToken;
  
public:
  void push( Element *tok ) { tok->next = currentToken; currentToken = tok; }
  
  Element *pop()  {
    Element *ptr;
  
    if ( currentToken ) {
      ptr = currentToken;
      currentToken = ptr->next;
      return ( ptr );
    }
    else {
      return ( NULL );
    }
  }

  Element *GetTopToken() { return ( currentToken ); }

  Stack() { currentToken = NULL; }
  ~Stack();
    
};

inline
Stack::~Stack()
{
  Element *pt = currentToken;
  while ( pt ) {
    Element *tmp = pt;
    pt = pt->next;
    delete tmp;
  }
}
    
#endif
