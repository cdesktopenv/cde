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
/* $XConsortium: TOCTask.h /main/2 1996/07/18 15:21:52 drk $ */

#ifndef __TOCTask_h
#define __TOCTask_h

#include "Task.h"
#include "OL-Data.h"

class DBTable;
class BookTask;

class TOCTask : public ComplexTask{
/*
 * TOCTask is used to build two tables:
 *
 * Table name: ContentsTree
 *  Fields:
 *    STRING_CODE book/toc locator
 *    STRING_CODE node locator
 *    STRING_CODE parent node locator
 *    SHORT_LIST_CODE of STRING_CODE child node locator
 *
 * Table name: ContentsPath
 *  Fields:
 *    STRING_CODE book/toc locator
 *    STRING_CODE node locator
 *  
 * Each TOCTask maintains:
 *     a subtask to gather the locator of the node it refers to
 *        (or, in the case of the root TOC node, the locator of the TOC)
 *     a "current" child subtask
 * it also uses the subtasks[] array to hold tasks that are no longer
 * "active" (i.e. receiving markup() and data()).
 *
 * The TOCTask determines when to create a new "current" subtask
 * in its markup() method.
 *
 * When the root TOC node completes, it uses the write_tree() and write_path()
 * methods to walk the whole tree of TOCTasks twice to build the tables.
 */
 

public:
  TOCTask(const Token& t, BookTask *book,
	  TOCTask *parent = NULL, int sibling_index = 0);
  ~TOCTask() { delete f_locator; }
  
  void markup(const Token& t); /* throw Unexpected */

  void data(const char *chars, size_t len);
  
  const char *locator(); /* throw Unexpected */

protected:
  TOCTask *parent() { return f_parent; };

  TOCTask *left()   { return f_parent && f_sibling_index > 0 ?
		      (TOCTask*)f_parent->subtask(f_sibling_index - 1) : NULL;
		    };

  TOCTask *right()  { return f_parent &&
		      f_sibling_index + 1 < f_parent->used ?
		      (TOCTask*)f_parent->subtask(f_sibling_index + 1) : NULL;
		    };
  
  void write_path(DBTable *tbl);
  int  write_tree(DBTable *tbl); /* return size of subtree */
  
private:
  int f_base;
  BookTask *f_book;

  TOCTask *f_parent;
  int f_sibling_index;
  
  OL_Data *f_locator;
};

#endif 
