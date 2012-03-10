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
