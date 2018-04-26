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
/* $XConsortium: ExprList.C /main/3 1996/08/21 15:46:33 drk $ */
/* imported interfaces */

/* exported interfaces */
#include "Expression.h"
#include "ExprList.h"

//------------------------------------------------------------------
ExprList::ExprList()
{
  head = 0;
}

//------------------------------------------------------------------
ExprList::ExprList( OL_Expression *elist )
{
  head = elist;
}

//------------------------------------------------------------------
ExprList::~ExprList()
{
  OL_Expression *ptr = head;
  while ( ptr ) {
    OL_Expression *tmp = ptr;
    ptr = ptr->next;
    delete tmp;
  }
}
  
//------------------------------------------------------------------
OL_Expression *
ExprList::first()
{
  return ( head );
}

//------------------------------------------------------------------
OL_Expression *
ExprList::next( OL_Expression *ptr )
{
  if ( ptr ) { return(ptr->next); }
  else { return 0; }
}
