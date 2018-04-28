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
/* $XConsortium: BaseDataCollect.h /main/2 1996/07/18 16:40:12 drk $ */
#ifndef __BaseDataCollect_h
#define __BaseDataCollect_h

#include "Task.h"
#include "Token.h"
#include "FlexBuffer.h"

enum ActionType {
  DEFAULT_ACTION=0,
  IGNORE_ON=1,
  REMOVE_SPACES=2,
  GENERATE_ID=4
};

class BaseData : public ComplexTask {

protected:
  int f_base;
  FlexBuffer ValueBuffer;
  int data_avail;
  int data_complete;
  int ignore_status;

protected:
  BaseData( const Token &t, ActionType istat) { 
    f_base = t.level();
    data_avail = 1;
    data_complete = 0;
    ignore_status = istat & IGNORE_ON;
  }

public:  
  int   ContentIsEmpty()    { return ( ValueBuffer.GetSize() == 0 ); }
  const char  *content()    { return( ValueBuffer.GetBuffer() ); }
  int   content_size()      { return( ValueBuffer.GetSize() );   }
  int   DataWillBeAvailable()   { return( data_avail ); }
  int   DataIsComplete()        { return( data_complete ); }

};

#endif
