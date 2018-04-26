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
/* $XConsortium: BookTasks.h /main/3 1996/07/18 15:15:19 drk $ */

#ifndef __BookTasks_h
#define __BookTasks_h

#include "Task.h"

class BookTask;
class NodeTask;
class StyleTask;
class OL_Data;
class DBTable;
class DB;
class SearchStorage;

class BookCaseDB;

class BookCaseTask : public ComplexTask{
public:
  BookCaseTask(const char* infolib);
  
  void markup(const Token&);

  BookCaseDB *database();      /* throw(Unexpected) */
  DBTable *table(int);         /* throw(Unexpected) */
  const char *bookcasename();  /* throw(Unexpected) */
  void write_full_text_record( const char *str,
			       int sz,
			       const char *nodelocator,
			       const char *node_title
			     );

  const char *styleName();     /* throw(Unexpected) */

  StyleTask *styleTask()  { return style; };

private:
  char *library;
  int f_base;

  BookCaseDB *f_db;
  StyleTask *style;
  BookTask *book;
  SearchStorage *f_search_storage;
  
  OL_Data *bookCaseName;
  OL_Data *bookCaseDesc;

  OL_Data *f_style;
};


class BookTask : public ComplexTask{
public:

  BookTask(BookCaseTask *);
  ~BookTask();

  void reset(void);

  void markup(const Token&);

  BookCaseTask *bookcase() { return f_bookcase; };
  int           sequencenum() const {  return f_seq_no; }

  const char *locator();          /* Locator for this book, i.e.
				   * for the TOC node for this book
				   */

  const char *styleName();        /* throw(Unexpected) */
  const char *book_short_title(); /* throw(Unexpected) */
  const char *book_title();       /* throw(Unexpected) */

protected:
  void write_record(void);

private:
  int f_base;                  /* tag nesting level of <BOOK> elt */

  int f_seq_no;                /* fulltext index document sequence number */

  BookCaseTask *f_bookcase;    /* 'parent' bookcase object */

  OL_Data *shortTitle;        /* short title collection task */
  OL_Data *title;             /* title collection task */

  char *tocLocator;            /* locator of TOC node */

  OL_Data *tabName;
  OL_Data *tabLocator;

  class StringList *tabNames;
  class StringList *tabLocators;
  class StringList *tabLines;
  class StringList *tabFiles;

  NodeTask *f_node;

  Task     *f_toc;

  OL_Data *f_style;

  void  encrypt( const Token & );
  char *e_string;               /*
				 * The encrypted string that is associated
				 * with the access permission for this book
				 */
  int   e_len;
};


#endif /* __BookTasks_h */

