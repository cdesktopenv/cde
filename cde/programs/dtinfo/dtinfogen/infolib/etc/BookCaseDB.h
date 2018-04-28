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
