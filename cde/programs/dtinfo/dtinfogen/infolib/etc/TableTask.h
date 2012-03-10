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
