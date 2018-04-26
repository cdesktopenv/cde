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
/* $XConsortium: OL-Data.h /main/2 1996/07/18 16:47:56 drk $ */
#ifndef OL_DATA_H
#define OL_DATA_H

#include <string.h>

#include "ContentType.h"
#include "Token.h"
#include "BaseDataCollect.h"

class OL_Data : public BaseData {

friend class FirstOf;

private:
  ContentType tokContent;
  char *f_name;
  int   line_num;
  int   istat;      /* These are status info */
  int   removeSpaces;
  void reset();

protected:
  /*
   * This constructor is used by the derived class of OL_Data only
   */
  OL_Data( const Token &t, ActionType ignore=DEFAULT_ACTION );

public:

  /* 
   * istat is controlled by the invoker to determine if OL_Data should really
   * ignore the OLIAS IGNORE attribute. The reason that such action is 
   * controlled by the invoker of the class is that the corresponding
   * behavior of IGNORE is controlled by the invoker, not by the OL_Data class
   */
  
  OL_Data( const Token &t, 
	   int aType, 
	   ActionType mode=DEFAULT_ACTION);
  /* no action takes place by default */


  ~OL_Data() { delete f_name; }
  
public:
  void markup( const Token & );
  void data( const char *, size_t );
  char *filename() { return ( f_name ); }
  int   line_no()  { return ( line_num ); }
  
};

inline
OL_Data::OL_Data(const Token &t, ActionType mode):BaseData(t,mode)
{
  istat = mode & IGNORE_ON;
  removeSpaces = mode & REMOVE_SPACES;
  
  f_name = strdup( t.file() );
  line_num = t.line();
}

#endif
