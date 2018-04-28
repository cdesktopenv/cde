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
/* TableTask.h -*- c++ -*-
 * $XConsortium: TableTask.h /main/2 1996/07/18 15:22:35 drk $
 */

#ifndef __TableTask_h
#define __TableTask_h

#include "Task.h"

class FlexBuffer;

class TableTask : public Task{

public:
  TableTask(const Token& t);
  ~TableTask();
  
  void markup(const Token& t);
  void data (const char *chars, size_t len);

protected:
  void putC(char c);

  void puts(const char *);
  void write(const char *, size_t);

  enum { Start, Fmt_, _Row, _Entry, Object, End } state;

  static int TGroup, ColSpec, Entry, Row, TBody;

  void error(const char *fmt, ...);
  void expected(const char *right, const Token& wrong);

  virtual void write_record() = 0;

  FlexBuffer *f_buf;     /* output buffer */
  
private:
  int f_base;            /* level of <TGROUP> token */
  int f_entryBase;       /* level of <ENTRY> token */
  
  int f_cols;            /* total number of colums in the table */
  int f_col;             /* current column (in Row_ state) */
};


class SearchEngine;

class SearchableTableTask : public TableTask{
public:
  SearchableTableTask(SearchEngine *se, const Token& t);

protected:
  void write_record();
  
private:
  SearchEngine *f_search;
};


#endif /* __TableTask_h */
