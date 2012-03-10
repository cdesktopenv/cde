/* $XConsortium: DataTask.h /main/2 1996/07/18 16:42:57 drk $ */
/* $Id: DataTask.h /main/2 1996/07/18 16:42:57 drk $ */

#ifndef __DataTask_h
#define __DataTask_h

#include <stddef.h>
#include "Task.h"
#include "FlexBuffer.h"

class DataTask : public Task{
  /*
   * A DataTask collects all the data for an element.
   *
   * USE:
   *   if(t.type() == START && t.attrMatch(OLAF::OLIAS, OLAF::Title)){
   *     titleTask = addSubTask(new DataTask());
   *
   *     ... (more markup(), data() calls) ...
   *
   *   const char *title = titleTask.content()
   */
public:

  DataTask(const Token& t);

  virtual void markup(const Token& t) /* throw(Unexpected) */;

  virtual void data(const char *chars,
		    size_t len) /* throw(ResourcesExhausted) */;

  const char *content(size_t *length_return = NULL);

 private:
  int level; /* how far nested are we? */
  FlexBuffer buf;
};


#endif /* __DataTask_h */
