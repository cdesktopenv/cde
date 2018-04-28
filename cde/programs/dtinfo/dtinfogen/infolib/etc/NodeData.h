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
/* $XConsortium: NodeData.h /main/2 1996/07/18 16:47:34 drk $ */
// NodeData.h

#ifndef NODEDATA_HEADER
#define NODEDATA_HEADER

#include "Task.h"

class Token;
class FlexBuffer;
class NodeTask;
class SearchEngine;
class OL_Data;

class NodeData : public ComplexTask {

friend int nodedatalex();

public:
  NodeData( NodeTask *, const Token & );
  ~NodeData();
  void markup( const Token & );
  void data(const char *, size_t );
  NodeTask *node() const { return f_node; }
  const char *graphics_id();
  
protected:
  NodeTask *f_node;
  FlexBuffer *NodeBuffer;
  int        CollectObject;
  int        f_base;
  SearchEngine *f_search;
  void        write_record();
  void        reset();

private:
  
  int seq_no;
  FlexBuffer  *DbBuffer;
  FlexBuffer  *internal_buffer;
  OL_Data     *current_graphics_id;
  void         write_start_tag( const Token &t, FlexBuffer *buf );
  
};

#endif
