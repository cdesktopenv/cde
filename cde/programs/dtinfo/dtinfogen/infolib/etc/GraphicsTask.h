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
/* $XConsortium: GraphicsTask.h /main/3 1996/07/18 16:46:09 drk $ */
#ifndef GRAPHICS_TASK_HDR
#define GRAPHICS_TASK_HDR

#include "Task.h"
#include "FlexBuffer.h"

#include "oliasdb/olias_consts.h"

class Token;
class SearchEngine;
class OL_Data;

class GraphicsTask : public ComplexTask {

private:
  int f_base;
  SearchEngine *f_parent;
  const FlexBuffer   *termsbuf;
  OL_Data      *graphics_data;
  OL_Data      *f_title;
  void write_record( const Token & );
  GR_TYPE graphics_type( const char * );
  
public:
  int IsDone() { return( f_base == -1 ); }
  int HasSearchTerms() const;
  const FlexBuffer *GetTerms() { return(termsbuf); }

public:
  void markup ( const Token & );
  GraphicsTask( SearchEngine *parent, const Token &t);
  
};

inline
int
GraphicsTask::HasSearchTerms() const
{
  if ( !termsbuf ) {
    return 0;
  }
  else             {
    FlexBuffer *tmpBuffer = ( FlexBuffer *)termsbuf;
    return( tmpBuffer->GetSize() );
  }
}
  

#endif
  

  
