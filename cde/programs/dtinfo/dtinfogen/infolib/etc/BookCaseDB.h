/* $XConsortium: BookCaseDB.h /main/3 1996/09/23 22:23:30 cde-hal $ -*- c++ -*- */

#ifndef __BookCaseDB_h
#define __BookCaseDB_h

#include "DataBase.h"

class BookCaseDB : public DB{
public:
  BookCaseDB(const char *dir);
  ~BookCaseDB();
  
  /*
   * Enumeration of tables...
   */
  typedef enum {
    BookMeta,
    NodeMeta, NodeSGML, Link, Locator,
    TOCTree, TOCPath,
    Graphics,
    StyleSheet,
    XRef,
    TableQty
  }TableID;

  DBTable *table(int, int access = DB::CREATE);

  DBTable  *f_tables[TableQty];
};


#endif /* __BookCaseDB_h */
