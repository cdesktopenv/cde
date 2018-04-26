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
