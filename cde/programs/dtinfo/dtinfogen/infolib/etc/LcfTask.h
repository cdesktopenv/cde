/* $XConsortium: LcfTask.h /main/2 1996/07/18 16:46:47 drk $ */
#ifndef LCF_TASK_H
#define LCF_TASK_H

#include "Task.h"

class NodeTask;

class LcfTask : public ComplexTask{
friend class NodeTask;

public:
  LcfTask( NodeTask *f_parent , const Token &t );
  void markup(const Token& t);
  
protected:
  void reset();
  void write_record();

  // void setNode(NodeTask *n) { f_node = n; }

private:
  NodeTask *f_node;
  int f_base;
};

#endif /* LcfTask.h */
