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
/* $XConsortium: ConcatTask.h /main/2 1996/07/18 16:40:43 drk $ */
#ifndef __Concat_h__
#define __Concat_h__

#include "Task.h"
#include "OL-Data.h"

class ExprList;
class Token;

class Concat : public OL_Data {
  
friend class FirstOf;  
friend class OL_Data;  

protected:
  Concat( const Token &t, ExprList *elist, ActionType mode );
  void markup( const Token &t ) { OL_Data::markup(t); }
  void data( const char *str, size_t sz ) { OL_Data::data(str, sz); }

};

#endif
