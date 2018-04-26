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
