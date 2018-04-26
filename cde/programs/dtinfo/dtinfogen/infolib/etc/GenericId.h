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
/* $XConsortium: GenericId.h /main/2 1996/07/18 16:45:48 drk $ */
#ifndef __gen_Id__
#define __gen_Id__

#include "BaseDataCollect.h"

class Token;
class OL_Data;

class GenericId : public BaseData {

friend class FirstOf;
friend class OL_Data;
friend class Concat;  
  
private:
  int giname;
  int done;
  OL_Data *f_data;
  
protected:
  GenericId( const Token &t, int giname,ActionType mode);
  void markup( const Token &t );
  void data( const char *str, size_t sz );

};
  
  
#endif
