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
/* $XConsortium: Literal.h /main/2 1996/07/18 16:47:09 drk $ */
#ifndef __Lithdr__
#define __Lithdr__

#include "BaseDataCollect.h"
#include "FlexBuffer.h"

class Token;

class Literal : public BaseData {

friend class FirstOf;
friend class OL_Data;
friend class Concat;  
  
protected:
  Literal( const Token &t, const char *str, ActionType mode );
};

inline
Literal::Literal( const Token &t, 
		  const char *str, 
		  ActionType mode ):BaseData(t, mode)
{
  data_complete = 1;
  if ( str ) {
    ValueBuffer.writeStr( str );
  }

}
  
#endif
